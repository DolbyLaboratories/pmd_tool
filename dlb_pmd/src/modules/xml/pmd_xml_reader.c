/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

/**
 * @file dlb_pmd_xml_reader.c
 * @brief xml parser
 */

#include "xml_hex.h"
#include "xml_uuid.h"
#include "xml_eidr.h"
#include "xml_ad_id.h"
#include "xml_cdata.h"

#include "pmd_model.h"

#include "dlb_pmd_xml.h"

#include "dlb_xml/include/dlb_xml.h"


#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#ifdef _MSC_VER
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#  define strtoll _strtoi64
__pragma(warning(disable:4127))
#endif

#include <stdarg.h>

/*
 * define PMD_XML_DEBUG to enable parser tracing
 */
//#define PMD_XML_DEBUG
#ifdef PMD_XML_DEBUG
#  define PMD_XML_TRACE(x) printf x
#else
#  define PMD_XML_TRACE(x)
#endif

#define TRACE(x) PMD_XML_TRACE(x)


/**
 * @def MAX_STRING_SIZE
 * @brief max string size
 */
#define MAX_STRING_SIZE DLB_PMD_NAME_ARRAY_SIZE


/**
 * @def MAX_XML_ESCAPE_SIZE
 *
 * &apos; is the max length of an escape sequence, *but* we can also
 * escape individual characters. The max allowable unicode character
 * is 0x10ffff, which has escape code &#x10ffff, or 9 chars. In
 * decimal, this is &#1114111.
 */
#define MAX_XML_ESCAPE_SIZE (9)

#include "parser_tagstack.h"
#include "klv.h"
#include "pmd_idmap.h"


/**
 * @brief hidden switch to disable version number checks
 *
 * In normal operation, we want to rigidly check that specified bitstream
 * version numbers match the version
 *
 * However, for unit testing, we want to check that we can set,
 * encode, decode and read any version number.  So we have a 'hidden
 * global' variable that is known *only* to the unit test code.
 */
pmd_bool global_testing_version_numbers = 0;


/**
 * @brief state of parser
 */
struct parser
{
    unsigned int coroutine_line;   /**< coroutine return point */

    tag_stack tagstack;            /**< current tag nesting stack, (for printing on error) */
    dlb_xmlpmd_line_callback lcb;  /**< line buffer request callback */
    dlb_xmlpmd_error_callback ecb; /**< error report callback or NULL */
    void *cbarg;                   /**< client-supplied callback argument */
    const char *line;              /**< pointer to current line (for printing on error) */
    unsigned int lineno;           /**< current XML file line number */

    dlb_pmd_model *model;          /**< current model */
    dlb_pmd_bool  strict;          /**< strict checking of presentation config fields? */

    unsigned int atsc3_channel_bsid;   /**< placeholder for parsing ATSC3 IAT distribution ids */
    unsigned int atsc3_channel_majno;  /**< placeholder for parsing ATSC3 IAT distribution ids */
    unsigned int atsc3_channel_minno;  /**< placeholder for parsing ATSC3 IAT distribution ids */
    unsigned int atsc3_unread_fields;  /**< bitmap for unread fields (for error checking) */
    
    unsigned int current_raw_type;     /**< placeholder for parsing current 'type' attribute of "Raw" tags */


    /* current attribute values ................................................. */
    unsigned int profile_number;       /**< PMD XML ProfessionalMetadata profile number */
    unsigned int profile_level;        /**< PMD XML ProfessionalMetadata profile level */
    dlb_pmd_speaker current_target;    /**< current output target */
    char current_language[4];          /**< current language */
    float source_gain_db;              /**< current source gain in dB */
    unsigned int current_dynamic_tag;  /**< current Dynamic Tag local ID, if > 65535, no such
                                        * tag */
    dlb_pmd_element_id current_element_id;
    dlb_pmd_presentation_id current_presentation_id;
    dlb_pmd_signal current_signal_id;
    unsigned int current_eac3_id;
    unsigned int current_etd_id;
    unsigned int update_time;          /**< current XYZ sample offset time */

    /* PMD API structures to populate .......................................... */
    dlb_pmd_source sources[DLB_PMD_MAX_BED_SOURCES];
    dlb_pmd_source *source;
    dlb_pmd_bed bed;
    dlb_pmd_object object;
    dlb_pmd_update update;
    dlb_pmd_presentation presentation;
    dlb_pmd_element_id elements[DLB_PMD_MAX_PRESENTATION_ELEMENTS];
    unsigned int presentation_contents[PMD_CLASS_RESERVED];
    dlb_pmd_bool presentation_config_objects_present;
    dlb_pmd_bool presentation_config_cm_me_present;
    dlb_pmd_bool presentation_config_cm;
    dlb_pmd_loudness loudness;
    dlb_pmd_eac3 eac3;
    dlb_pmd_ed2_turnaround etd;
    dlb_pmd_turnaround *turnaround;
    dlb_pmd_identity_and_timing iat;
    dlb_pmd_headphone headphone;
};
    

/**
 * @brief send an error message to the client's error callback
 */
static
void
errmsg
    (parser *p        /**< [in] parser state */
    ,const char *fmt  /**< [in] message format */
    ,...              /**< [in] misc args */
    )
{
    if (NULL != p->ecb)
    {
        char msg[128];
        
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(msg, sizeof(msg), fmt, ap);
        va_end(ap);
        p->ecb(msg, p->cbarg);
    }
}


/**
 * @brief after parsing an AudioSignal tag, add the specified signal to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_signal_to_model
    (parser *p
    )
{
    if (dlb_pmd_add_signal(p->model, p->current_signal_id))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh bed structure to populate during AudioBed tag parsing
 */
static
void
new_bed
    (parser *p
    )
{
    memset(&p->bed, '\0', sizeof(p->bed));
    memset(&p->sources, '\0', sizeof(p->sources));
    p->bed.sources = p->sources;
    p->bed.id = p->current_element_id;
    p->current_element_id = 0;
    p->source = p->sources;
}


/**
 * @brief after parsing an AudioBed tag, add the specified bed to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_bed_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_bed(p->model, &p->bed))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh object structure to populate during AudioObject tag parsing
 */
static
void
new_object
    (parser *p
    )
{
    memset(&p->object, '\0', sizeof(p->object));
    p->object.id = p->current_element_id;
    p->current_element_id = 0;
}


/**
 * @brief parser pop action to insert object into model once an entire AudioObject
 * tag has been written
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_object_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_object(p->model, &p->object))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh presentation structure to populate during Presentation tag parsing
 */
static
void
new_presentation
    (parser *p
    )
{
    memset(&p->presentation, '\0', sizeof(p->presentation));
    memset(&p->elements, '\0', sizeof(p->elements));
    p->presentation.id = p->current_presentation_id;
    p->presentation.elements = p->elements;
    p->current_presentation_id = 0;
    memset(p->presentation_contents, '\0', sizeof(p->presentation_contents));
    p->presentation_config_objects_present = 0;
    p->presentation_config_cm_me_present = 0;
}


/**
 * @brief after parsing a Presentation tag, add the specified presentation struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_presentation_to_model
    (parser *p
    )
{
    /* validate the presentation config */
    static const char *object_class_names[PMD_CLASS_RESERVED] =
    {
        "dialog", "VDS", "voiceover", "generic", "spoken subtitle",
        "emergency alert", "emergency info"
    };

    dlb_pmd_presentation *pres = &p->presentation;
    
    /* we don't accept things like "Portable Speaker + 3O" */
    if (pres->config < DLB_PMD_SPEAKER_CONFIG_PORTABLE)
    {
        unsigned int actual_presentation_contents[PMD_CLASS_RESERVED];
        dlb_pmd_bool actual_objects_present = 0;
        unsigned int i;
        
        /* step 1: compute the actual presentation content */
        memset(actual_presentation_contents, 0, sizeof(actual_presentation_contents));
        for (i = 0; i != pres->num_elements; ++i)
        {
            dlb_pmd_object object;
            
            if (!dlb_pmd_object_lookup(p->model, pres->elements[i], &object))
            {
                actual_presentation_contents[object.object_class] += 1;
                actual_objects_present = 1;
            }
        }
        
        if (actual_objects_present && !p->presentation_config_objects_present)
        {
            errmsg(p, "%s: presentation %u contains objects,"
                      "but the presentation config string does not",
                   p->strict ? "Error" : "Warning",
                   pres->id);
            if (p->strict)
            {
                return 1;
            }
        }
        else
        {
            /* step 2: compare against the contents listed in the presentation_config
             * string by the #decode_presentation_config function */
            for (i = 0; i != PMD_CLASS_RESERVED; ++i)
            {
                if (p->presentation_contents[i] != actual_presentation_contents[i])
                {
                    errmsg(p, "presentation config string specifies %u %s objects, "
                           " but presentation %u has %u",
                           p->presentation_contents[i],
                           object_class_names[i],
                           pres->id,
                           actual_presentation_contents[i]);
                    return 1;
                }
            }
        }
    }

    if (dlb_pmd_set_presentation(p->model, &p->presentation))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh loudness structure to populate during PresentationLoudness tag parsing
 */
static
void
new_loudness
    (parser *p
    )
{
    memset(&p->loudness, '\0', sizeof(p->loudness));
}


/**
 * @brief after parsing a PresentationLoudness tag, add the specified loudness struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_loudness_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_loudness(p->model, &p->loudness))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh EAC3 parameters structure to populate during Eac3EncodingParameters tag parsing
 */
static
void
new_eac3
    (parser *p
    )
{
    memset(&p->eac3, '\0', sizeof(p->eac3));
    p->eac3.id = p->current_eac3_id;
    p->current_eac3_id = 0;
}


/**
 * @brief after parsing an Eac3EncodingParameters tag, add the specified struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_eac3_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_eac3(p->model, &p->eac3))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh ED2 turnaround structure to populate during ED2Turnaround tag parsing
 */
static
void
new_etd
    (parser *p
    )
{
    memset(&p->etd, '\0', sizeof(p->etd));
    p->etd.id = p->current_etd_id;
    p->current_etd_id = 0;
}


/**
 * @brief prepare a new ED2 turnaround sub-element of an ED2- turnaround struct
 */
static
void
new_ed2_turnaround
    (parser *p
    )
{
    p->turnaround = &p->etd.ed2_turnarounds[p->etd.ed2_presentations];
    memset(p->turnaround, '\0', sizeof(*p->turnaround));
    p->etd.ed2_presentations += 1;
}


/**
 * @brief prepare a new DE turnaround sub-element of an ED2- turnaround struct
 */
static
void
new_de_turnaround
    (parser *p
    )
{
    p->turnaround = &p->etd.de_turnarounds[p->etd.de_presentations];
    memset(p->turnaround, '\0', sizeof(*p->turnaround));
    p->etd.de_presentations += 1;
}



/**
 * @brief after parsing an ED2Turnaround tag, add the specified ED2 turnaround struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_ed2_turnaround_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_ed2_turnaround(p->model, &p->etd))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh update structure to populate during DynamicUpdate tag parsing
 */
static
void
new_update
    (parser *p
    )
{
    memset(&p->update, '\0', sizeof(p->update));
    p->update.sample_offset = p->update_time;
    p->update_time = 0;
}


/**
 * @brief after parsing a DynamicUpdate tag, add the specified update struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_update_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_update(p->model, &p->update))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh IAT structure to populate during IAT tag parsing
 */
static
void
new_iat
    (parser *p
    )
{
    memset(&p->iat, '\0', sizeof(p->iat));
}


/**
 * @brief after parsing an IAT tag, add the specified IAT struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_iat_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_iat(p->model, &p->iat))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a fresh headphone structure to populate during HeadphoneElement tag parsing
 */
static
void
new_headphone
    (parser *p
    )
{
    memset(&p->headphone, '\0', sizeof(p->headphone));
    p->headphone.channel_mask = 0xffff;
}


/**
 * @brief after parsing a HeadphoneElement tag, add the specified headphone struct to the model
 */
static
dlb_pmd_success            /** @return 1 on failures, 0 on success */
add_headphone_to_model
    (parser *p
    )
{
    if (dlb_pmd_set_headphone_element(p->model, &p->headphone))
    {
        errmsg(p, dlb_pmd_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief generate parser tracing messages (if debugging)
 */
static
void
parser_trace
    (parser *p
    ,const char *fmt
    ,...
    )
{
#ifdef PMD_XML_DEBUG
    unsigned int i;
    va_list ap;

    for (i = 0; i != p->tagstack.top; ++i)
    {
        printf("    ");
    }
    va_start(ap, fmt);
    vprintf(fmt, ap);
#else
    (void)p;
    (void)fmt;
#endif
}


/**
 * @brief report an error
 */
static
void
report_error
    (void *context
    ,char *tag
    ,char *text
    )
{
    char msg[128];
    parser *p = (parser *)context;
    tagloc *loc;
    int i;
    
    (void)text;

    if (NULL != p->ecb)
    {
        snprintf(msg, sizeof(msg), "ERROR: parsing tag \"%s\" at line %u", tag, p->lineno);
        p->ecb(msg, p->cbarg);

        i = p->tagstack.top - 1;
        loc = &p->tagstack.stack[i];
    
        while (i >= 0)
        {
            snprintf(msg, sizeof(msg), "    parsing tag %s at line %u", loc->tag, loc->lineno);
            p->ecb(msg, p->cbarg);
            --loc;
            --i;
        }
    }
}


/* ------------------------ COROUTINE MACROS ------------------------- */
/* The dlb_xml library walks through the XML file and invokes its
 * element_callback whenever it discovers a new tag.  However, we also
 * want to write the recursive descent parser in a manner that reflects
 * the grammer - the shape of the XML we want to parse.  We use a
 * coroutine approach to allow both.
 *
 * see http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
 */

/**
 * @def COROUTINE_BEGIN
 * @brief C function prologue to ensure function behaves like a coroutine
 *
 * The basic idea is that the coroutine function is one giant switch of
 * line numbers, and whenever we reenter the function, we jump to the
 * line number we were last at to continue processing.
 *
 * These macros use case statements and __LINE__ numbers to ensure whenever
 * we return from the coroutine, we remember the current line number, and
 * jump back to it upon next entry.
 */
#define COROUTINE_BEGIN                                         \
    parser_trace(p, "read %s%s%s<%s%s>\n",                      \
                 text ? "\"" : "",                              \
                 text ? text : "",                              \
                 text ? "\"" : "",                              \
                 text ? "/" : "", tag);                         \
    switch(p->coroutine_line) { case 0:

#define COROUTINE_RETURN(x) p->coroutine_line = __LINE__; return x; case __LINE__:;
#define COROUTINE_END }


/* ------------------------ RECURSIVE DESCENT PARSING COMBINATORS ------------------------- */


/**
 * @brief decide whether the tag matches <literal>
 */
static
int  /** @return 1 if the tag is <literal>, 0 otherwise */
is_open_tag
    (const char *tag      /**< [in] current tag */
    ,const char *text     /**< [in] tag's text */
    ,const char *literal  /**< [in] pattern to match */
    )
{
    return !strcasecmp(tag, literal) && NULL == text;
}


/**
 * @brief decide whether the tag matches </literal>
 */
static
int  /** @return 1 if the tag is </literal>, 0 otherwise */
is_closed_tag
    (const char *tag      /**< [in] current tag */
    ,const char *text     /**< [in] tag's text */
    ,const char *literal  /**< [in] pattern to match */
    )
{
    return !strcasecmp(tag, literal) && NULL != text;
}



#define next_tag()           COROUTINE_RETURN(0)
#define open_tag(literal)    (is_open_tag(tag, text, literal))
#define closed_tag(literal)  (is_closed_tag(tag, text, literal))
#define error()              { report_error(context, tag, text); return 1; }
#define push_tag()           tag_stack_push(&p->tagstack, tag, p->lineno, NULL)
#define push_action_tag(a)   tag_stack_push(&p->tagstack, tag, p->lineno, a)
#define pop_tag()            if (tag_stack_pop(&p->tagstack, p)) error()
    
#define loop_until_closed(name)                     \
    push_tag();                                     \
    while (!closed_tag(name))                       \
    {                                               \
        parser_trace(p, "loop tag <%s>\n", name);   \

                                                          
#define end_loop()                              \
    }                                           \
    pop_tag();
    

#define if_begin_popaction_tag(name, action, popaction)         \
    if (open_tag(name))                                         \
    {                                                           \
        parser_trace(p, "parsing body of <%s>\n", name);        \
        push_action_tag(popaction);                             \
        next_tag();                                             \
        action;                                                 \
        while (!closed_tag(name)) 

#define if_begin_tag(name, action)              \
    if_begin_popaction_tag(name, action, NULL)

#define end_if_begin_tag()                          \
        pop_tag();                                  \
        parser_trace(p, "body complete\n");         \
    }                                               \
    else                                            \
    {                                               \
        error();                                    \
    }


#define if_tag(name)                                            \
    if (open_tag(name))                                         \
    {                                                           \
        parser_trace(p, "parsing body of <%s>\n", name);        \
        push_tag();                                             \
        next_tag();                                             \
        parser_trace(p, "**** %s: %s\n", name, text);           \
        if (!closed_tag(name)) error();                         \


#define elif_tag(name)  \
    } else if_tag(name)


#define if_open_tag(name)                                   \
    if (open_tag(name))                                     \
    {                                                       \
        parser_trace(p, "parsing body of <%s>\n", name);    \
        push_tag();                                         \
        next_tag();
      
#define elif_open_tag(name)  \
    }                        \
    else if_open_tag(name)

    
#define elif_begin_tag(name,action)             \
    }                                           \
    else if_begin_tag(name,action)


#define elif_begin_popaction_tag(name,action,popaction)  \
    }                                                    \
    else if_begin_popaction_tag(name,action,popaction)


#define endif()                                 \
    }                                           \
    else                                        \
    {                                           \
        error();                                \
    }                                           \
    pop_tag();                                  \
    next_tag();                                 \



/* ------------------- MODEL CONSTRUCTORS ----------------------------- */


/**
 * @def set_profile
 * @brief set up the profile constraints, if any, which will have been
 * retrieved by the attribute callback parser
 */
#define set_profile(p)                                                  \
    if (dlb_pmd_set_profile(p->model, p->profile_number, p->profile_level)) error()



/* -------------------------------------- PARSE TYPES ----------------------------- */


/**
 * @brief decode an XML escape code
 */
static
int                      /** @return 0 on failure, 1 on success */
decode_xml_escape
    (const char **textptr
    ,unsigned int *unicode
    )
{
    const char *text = *textptr;
    const char *end = strchr(text, ';');
    
    if (NULL == end)
    {
        return 0;
    }
    
    *textptr = end + 1;
    if (1 == sscanf(text, "&#%u;", unicode))
    {
        return 1;
    }
    if (1 == sscanf(text, "&#x%x;", unicode))
    {
        return 1;
    }
    if (0 == strncmp(text, "&amp;", 5))
    {
        *unicode = '&';
        return 1;
    }
    if (0 == strncmp(text, "&lt;", 4))
    {
        *unicode = '<';
        return 1;
    }
    if (0 == strncmp(text, "&gt;", 4))
    {
        *unicode = '>';
        return 1;
    }
    if (0 == strncmp(text, "&quot;", 6))
    {
        *unicode = '\"';
        return 1;
    }
    if (0 == strncmp(text, "&apos;", 6))
    {
        *unicode = '\'';
        return 1;
    }

    return 0;
}


/**
 * @brief write a unicode character to buffer
 */
static
int                       /** @return 0 on failure, 1 on success */
write_utf8_char
    (uint8_t **bufptr     /**< [in] buffer to write to */
    ,uint8_t *end         /**< [in] 1st character after buffer end */
    ,unsigned int unicode /**< [in] unicode character to write */
    )
{
    uint8_t *buf = *bufptr;
    if (unicode < 128)
    {
        if (buf >= end)
        {
            return 0;
        }
        *buf++ = (uint8_t)unicode;
    }
    else if (unicode < 0x800)
    {
        if (buf+1 >= end)
        {
            return 0;
        }
        *buf++ = 0xC0 | ((unicode >> 6) & 0x1F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else if (unicode < 0x10000)
    {
        if (buf+2 >= end)
        {
            return 0;
        }
        *buf++ = 0xE0 | ((unicode >> 12) & 0x0F);
        *buf++ = 0x80 | ((unicode >> 6) & 0x3F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else if (unicode < 0x110000)
    {
        if (buf+3 >= end)
        {
            return 0;
        }
        *buf++ = 0xF0 | ((unicode >> 18) & 0x07);
        *buf++ = 0x80 | ((unicode >> 12) & 0x3F);
        *buf++ = 0x80 | ((unicode >> 6) & 0x3F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else
    {
        return 0;
    }
    *bufptr = buf;
    return 1;
}


/**
 * @brief decode a string
 */
static
int                      /** @return 0 on failure, 1 on success */
decode_string
    (parser *p           /**< [in] parser state */
    ,const char *text    /**< [in] text to parse */
    ,uint8_t *buf        /**< [in] array to store string */
    ,unsigned int maxlen /**< [in] max allowed string name */
    )
{
    uint8_t tmpbuf[MAX_STRING_SIZE * MAX_XML_ESCAPE_SIZE];
    size_t len = strlen(text);
    const char *end = text + len;
    uint8_t *tmp = tmpbuf;
    uint8_t *tmpend = tmp + sizeof(tmpbuf);

    assert(maxlen <= MAX_STRING_SIZE);

    if (len > sizeof(tmpbuf))
    {
        errmsg(p, "String name too long: length is %u, limit is %u", len, maxlen);
        return 0;
    }

    memset(tmp, '\0', sizeof(tmpbuf));
    while (text < end)
    {
        unsigned char c = *text;
        if ('&' == c)
        {
            unsigned int unicode;
            if (!decode_xml_escape(&text, &unicode))
            {
                errmsg(p, "Couldn't decode XML escape code at: \"&%s\"", text);
                return 0;
            }
            if (unicode < 0xd800 ||
                (unicode >= 0xe000 && unicode <= 0xfffd) ||
                (unicode >= 0x10000 && unicode <= 0x10ffff))
            {
                if (!write_utf8_char(&tmp, tmpend, unicode))
                {
                    errmsg(p, "Couldn't write UTF-8 to buffer");
                    return 0;
                }
            }
            else
            {
                errmsg(p, "Illegal character in XML");
                return 0;
            }
        }
        else
        {
            *tmp++ = c;
            ++text;
        }
    }

    if (tmp - tmpbuf > (int)maxlen)
    {
        errmsg(p, "String name too long: length is %u, limit is %u", tmp - tmpbuf, maxlen);
        return 0;
    }
    memset((void*)buf, '\0', maxlen);
    memmove(buf, tmpbuf, tmp - tmpbuf);
    return 1;
}


/**
 * @brief parse the string for an unsigned integer
 *
 * Note that this will not work for very large integers (i.e., those
 * with top bit set).
 */
static 
int                     /** @return 0 on failure, 1 on success */
decode_uint
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    ,const char *name   /**< [in] name of thing being parsed */
    ,unsigned int max   /**< [in] max acceptable value */
    ,unsigned int *val  /**< [out] unsigned integer */
    )
{
    char *endp;
    long long v;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag: %s", name);
        return 0;
    }

    v = strtoll(text, &endp, 0);
    if (v < 0 || v > (long long)max || endp == text)
    {
        errmsg(p, "Invalid %s: \"%s\"", name, text);
        return 0;
    }
    *val = (unsigned int)v;
    return 1;
}


/**
 * @brief parse a SMPTE 2109 container config dynamic tag assignment
 *
 * A dynamic tag assignment is a temporary remapping of a local tag
 * to a given 16-byte Universal Key.  Upon entry to this function, the
 * specific local tag should be stored in the parser's current_dynamic_tag
 * field.
 */
static
int                     /** @return 0 on failure, 1 on success */
decode_dynamic_tag
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    )
{
    pmd_smpte2109 *smpte2109 = &p->model->smpte2109;
    pmd_dynamic_tag *tag;
    unsigned int i;
    uint8_t *ul;

    if (p->current_dynamic_tag > 65536)
    {
        errmsg(p, "No local tag specified ('id' attribute for tag)");
        return 0;
    }

    if (smpte2109->num_dynamic_tags >= PMD_MAX_DYNAMIC_TAGS)
    {
        errmsg(p, "Too many dynamic tags, up to %u supported", PMD_MAX_DYNAMIC_TAGS);
        return 0;
    }
    
    tag = &smpte2109->dynamic_tags[smpte2109->num_dynamic_tags];
    ul = tag->universal_label;
    
    for (i = 0; i != 15; ++i)
    {
        if (read_hex(text, 2, ul)) return 0;
        if (text[2] != '.') return 0;
        text += 3;
        ul += 1;
    }
    if (read_hex(text, 2, ul)) return 0;
    if (text[2] != '\0') return 0;

    /* exclude forbidden tags */
    if (tag->local_tag == KLV_PMD_LOCAL_TAG_CONFIG)
    {
        errmsg(p, "Cannot override Container Config local key");
        return 0;
    }

    tag->local_tag = p->current_dynamic_tag;
    smpte2109->num_dynamic_tags += 1;
    p->current_dynamic_tag = ~0u;
    return 1;
}


/**
 * @brief parse a SMPTE 2109 payload config sample offset
 */
static
int                     /** @return 0 on failure, 1 on success */
decode_sample_offset
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    )
{
    unsigned int tmp;
    if (decode_uint(p, text, "SampleOffset", 65535, &tmp))
    {
        p->model->smpte2109.sample_offset = (uint16_t)tmp;
        return 1;
    }
    return 0;
}


/**
 * @brief parse the overall title
 *
 * Must be able to fit into a string of size PMD_TITLE_SIZE - "[ab/xy]" 
 */
static
int                     /** @return 0 on failure, 1 on success */
decode_title
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    )
{
    return decode_string(p, text, (uint8_t*)p->model->title, sizeof(p->model->title)-8);
}


/**
 * @brief parse the string for a PMD co-ordinate
 *
 * accepted values: -1.0 - 1.0
 * X: -1.0 = left, 1.0 = right
 * Y: -1.0 = front, 1.0 = back
 * Z: -1.0 = floor, 0.0 = horizon, 1.0 = ceiling
 */
static 
int                     /** @return 0 on failure, 1 on success */
decode_coordinate
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    ,float *pos         /**< [out] co-ordinate */
    )
{
    char *endp;
    float f;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    f = (float)strtod(text, &endp);
    if (f < -1.0f || f > 1.0f || endp == text)
    {
        errmsg(p, "Invalid co-ordinate: \"%s\"",  text);
        return 0;
    }
    *pos = f;
    return 1;
}


/**
 * @brief helper function to check if text represents -inf dB.
 */
static inline
pmd_bool
text_is_minf_db
    (const char *text
    )
{
    return !strcasecmp(text, "-infdB");
}


/**
 * @brief parse the string for a PMD gain value
 */
static 
int                     /** @return 0 on failure, 1 on success */
decode_gain
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    ,float *gain        /**< [out] PMD gain */
    )
{
    char *endp;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (text_is_minf_db(text))
    {
        *gain = -INFINITY;
        return 1;
    }
    
    *gain = (float)strtod(text, &endp);
    if (*gain < -25.0f || *gain > 6.0f || endp == text)
    {
        errmsg(p, "Invalid gain: \"%s\"",  text);
        return 0;
    }
    if (strcasecmp(endp, "dB"))
    {
        errmsg(p, "Invalid gain: \"%s\" (missing 'dB' suffix)", text);
        return 0;
    }
    return 1;
}


/**
 * @brief parse the string for a PMD size value
 *
 * Range: 0.0 - 1.0, 0
 *    where 0.0 means 'point source'
 *      and 1.0 means entire field.
 */
static 
int                     /** @return 0 on failure, 1 on success */
decode_size
    (parser *p          /**< [in] parser state */
    ,const char *text   /**< [in] text to parse */
    ,float *sz          /**< [out] PMD size value */
    )
{
    char *endp;
    float f;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    f = (float)strtod(text, &endp);
    if (f < 0 || f > 1 || endp == text)
    {
        errmsg(p, "Invalid size: \"%s\"",  text);
        return 0;
    }
    *sz = f;
    return 1;
}


/**
 * @brief generic parse enumeration routine
 */
static
int                     /** @return 0 on failure, 1 on success */
decode_enum
    (const char *text   /**< [in] text to parse */
    ,const char **names /**< [in] array of acceptable names */
    ,unsigned int count /**< [in] size of #names array */
    )
{
    unsigned int i;

    for (i = 0; i != count; ++i)
    {
        if (!strcasecmp(text, names[i])) return i;
    }
    return -1;
}


/**
 * @brief convert from speaker name to pmd_speaker
 */
static
int                     /** @return 0 on failure, 1 on success */
decode_speaker_name
    (parser *p          /**< [in] parser state */
    ,const char *text      /**< [in] speaker name to convert */
    ,dlb_pmd_speaker *idx  /**< [out] speaker id */
    )
{
    static const char *speakernames[17] =
    { "ERROR",
      "Left", "Right", "Center", "LFE",
      "Left Surround", "Right Surround",
      "Left Rear Surround", "Right Rear Surround",
      "Left Top Front", "Right Top Front", 
      "Left Top Middle", "Right Top Middle",
      "Left Top Rear", "Right Top Rear",
      "Left Front Wide", "Right Front Wide"
    };
    
    int res = decode_enum(text, speakernames, 17);
    if (res < 0)
    {
        errmsg(p, "Unknown speaker name \"%s\"", text);
        return 0;
    }
    *idx = (dlb_pmd_speaker)res;
    return 1;
}


/**
 * @brief parse a channel element's speaker config
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_speaker_config
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,dlb_pmd_speaker_config *cfg  /**< [out] channel speaker config */
    )
{
    static const char *sc[] =
    {
        "2.0",                  /* 0 */
        "3.0",                  /* 1 */
        "5.1",                  /* 2 */
        "5.1.2",                /* 3 */
        "5.1.4",                /* 4 */
        "7.1.4",                /* 5 */
        "9.1.6",                /* 6 */
        "Portable Speaker",     /* 7 */
        "Portable Headphone"    /* 8 */
    };
    int res;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    res = decode_enum(text, sc, 9);
    if (res < 0)
    {
        errmsg(p, "Error: unknown speaker config \"%s\"", text);
        return 0;
    }
    *cfg = (dlb_pmd_speaker_config)res;
    return 1;
}


/**
 * @brief parse a presentation's config
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_presentation_config
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,dlb_pmd_speaker_config *cfg  /**< [out] channel speaker config */
    )
{
    char tmp[32];
    char num_obj_str[32];
    size_t len;

    unsigned int num_digits = 0;
    unsigned int digit_string_int = 0;
    unsigned int tmp_iterator = 0;
    size_t i;
    
    memset(num_obj_str, '\0', sizeof(num_obj_str));
    memset(p->presentation_contents, 0, sizeof(p->presentation_contents));
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    len = strlen(text);
    /* check for Portable Speakers/Headphones */
    for (i = 0; i != len; ++i)
    {
        if (text[i] == ' ' && text[i] != '\0')
        {
            if (!strncasecmp(text, "Portable", i))
            {
                continue;
            }
            break;
        }
    }

    if (i < (int)sizeof(tmp))
    {
        strncpy(tmp, text, i);
        tmp[i] = '\0';
        if (!decode_speaker_config(p, tmp, cfg))
        {
            return 0;
        }
    }

    ++i;
    for (; i <= len; ++i)
    {
        if (text[i] == ' ' || i == len)
        {   
            if (!strncasecmp(tmp, "ME", tmp_iterator))
            {
                p->presentation_config_cm_me_present = 1;
                p->presentation_config_cm = 0;
                break;
            }
            else if (!strncasecmp(tmp, "CM", tmp_iterator))
            {
                p->presentation_config_cm_me_present = 1;
                p->presentation_config_cm = 1;
                break;
            }
        }
        tmp[tmp_iterator] = text[i];
        tmp_iterator++;
    }
    memset(tmp, '\0', sizeof(tmp));
    tmp_iterator = 0;
    i++;
    for (; i <= len; ++i)
    {
        digit_string_int = 0;

        if (isdigit(text[i]) && text[i] != ' ' && (tmp[0] == '+'))
        {    
            num_obj_str[num_digits] = text[i];
            num_digits++;
            continue;
        }
        if ((text[i] == ' ' || i == len) && text[i-1] != '+')
        {   
            if (num_digits)
            {
                digit_string_int = atoi(num_obj_str);
                memset(num_obj_str, '\0', sizeof(num_obj_str));
            }
            else
            {
                digit_string_int = 1;
            }
            if (!strncasecmp(tmp, "+ D", 3))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_DIALOG] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
            else if (!strncasecmp(tmp, "+ VDS", 5))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_VDS] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
            else if (!strncasecmp(tmp, "+ VO", 4))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_VOICEOVER] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
            else if (!strncasecmp(tmp, "+ O", 3))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_GENERIC] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
            else if (!strncasecmp(tmp, "+ SS", 4))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_SUBTITLE] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
            else if (!strncasecmp(tmp, "+ EA", 4))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_EMERGENCY_ALERT] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
            else if (!strncasecmp(tmp, "+ EI", 4))
            {
                memset(tmp, '\0', sizeof(tmp));
                p->presentation_contents[PMD_CLASS_EMERGENCY_INFO] += digit_string_int;
                p->presentation_config_objects_present = 1;
                num_digits = 0;
                tmp_iterator = 0;
                continue;
            }
        }
        tmp[tmp_iterator] = text[i];
        tmp_iterator++;
    }
    return 1;
}


/**
 * @brief parse an object element's class
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_object_class
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_object_class *cls        /**< [out] object class */
    )
{
    static const char *classes[] = { "Dialog", "VDS", "Voice Over", "Generic", "Spoken Subtitle",
                                     "Emergency Alert", "Emergency Information" };

    int res;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    res = decode_enum(text, classes, 7);
    if (res < 0)
    {
        errmsg(p, "Error: unknown object class \"%s\"", text);
        return 0;
    }
    *cls = (pmd_object_class)res;
    return 1;
}


/**
 * @brief parse a generic boolean
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_bool
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_bool *b                  /**< [out] boolean */
    )
{
    static const char *ot[] = { "False", "True" };
    int res;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    res = decode_enum(text, ot, 2);
    if (res < 0)
    {
        errmsg(p, "Error: unknown boolean \"%s\"", text);
        return 0;
    }
    *b = res;
    return 1;
}


/**
 * @brief parse an ISO 639-1 or ISO 639-2 language code
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_langcode
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_langcode *code           /**< [out] language code */
    )
{
    if (NULL == text)
    {
        errmsg(p, "No language code specified");
        return 0;
    }
    else if (pmd_decode_langcode(text, code))
    {
        errmsg(p, "unknown language code \"%s\"", text);
        return 0;
    }
    return 1;    
}


static
int
decode_language
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,char *lang                   /**< [out] language code */
    )
{
    pmd_langcode code = 0;

    memset(lang, '\0', 4);
    if (decode_langcode(p, text, &code))
    {
        memmove(lang, text, 4);
        return 1;
    }
    return 0;
}
    

/**
 * @brief parse an element ID in a headphone element description
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_hed_render_mode
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,dlb_pmd_render_mode *rmode   /**< [out] space to populate result */
    )
{
    unsigned int i;
    
    if (!decode_uint(p, text, "RenderMode", 127, &i))
    {
        return 0;
    }

    *rmode = (dlb_pmd_render_mode)i;
    return 1;
}


/**
 * @brief parse an element ID in a headphone element description
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_hed_channel_id
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,dlb_pmd_headphone *hed       /**< [in] headphone element description */
    )
{
    dlb_pmd_speaker idx;
    
    if (dlb_pmd_bed_lookup(p->model, hed->audio_element_id, &p->bed, DLB_PMD_MAX_BED_SOURCES,
                           p->sources))
    {
        errmsg(p, "element id %u is not a bed, but has been given a channel mask",
               hed->audio_element_id);
        return 0;
    }

    if (decode_speaker_name(p, text, &idx))
    {
        dlb_pmd_channel_mask bit = 1ul << (idx-1);
        hed->channel_mask &= ~bit;
    }
    return 1;
}


/**
 * @brief parse a presentation element
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_pres_element
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    )
{
    dlb_pmd_presentation *pres = &p->presentation;
    unsigned int eid = 0;

    /* do we want to check for duplicates? */
    if (pres->num_elements >= DLB_PMD_MAX_PRESENTATION_ELEMENTS)
    {
        errmsg(p, "Too many elements for presentation %u", p->presentation.id);
        return 0;
    }
    
    if (!decode_uint(p, text, "presentation element", DLB_PMD_MAX_ELEMENT_ID, &eid))
    {
        return 0;
    }

    pres->elements[pres->num_elements] = (dlb_pmd_element_id)eid;
    pres->num_elements += 1;
    return 1;
}


/**
 * @brief parse a presentation name
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_pres_name
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    )
{
    dlb_pmd_presentation *pres = &p->presentation;
    dlb_pmd_presentation_name *name;

    /* do we want to check for duplicates? */
    if (pres->num_names >= DLB_PMD_MAX_PRESENTATION_NAMES)
    {
        errmsg(p, "Too many names for presentation %u", pres->id);
        return 0;
    }
    
    if (0 == p->current_language[0])
    {
        errmsg(p, "No language specified for presentation name \"%s\"", text);
        return 0;
    }

    name = &pres->names[pres->num_names];

    memmove(name->language, p->current_language, sizeof(name->language));
    p->current_language[0] = 0;
    
    if (!decode_string(p, text, (uint8_t*)name->text, sizeof(name->text)))
    {
        return 0;
    }

    pres->num_names += 1;
    return 1;
}


/**
 * @brief parse an eac3 encoding parameter ID
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_eepid
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,uint16_t *id                 /**< [out] APM id */
    )
{
    char *endp;
    int v;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    v = strtol(text, &endp, 0);
    if (v < 1 || v > 255 || endp == text)
    {
        errmsg(p, "Invalid EAC3 encoding parameter id: \"%s\"", text);
        return 0;
    }
    *id = (uint16_t)v;
    return 1;
}


/**
 * @brief parse DE framerate in ED2 Turnaround struct
 *
 * Note that although PMD supports framerates up to 120 fps for PCM+PMD,
 * for DE/ED2 turnarounds, only 23.98 - 30 fps are supported.
 */
static
int                                /** @return 0 on failure, 1 on success */
decode_framerate
    (parser *p                     /**< [in] parser state */
    ,const char *text              /**< [in] text to parse */
    ,dlb_pmd_frame_rate *framerate /**< [out] frame rate value */
    )
{
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (!strcasecmp(text, "23.98"))
    {
        *framerate = DLB_PMD_FRAMERATE_2398;
        return 1;
    }
    else if (!strcasecmp(text, "24"))
    {
        *framerate = DLB_PMD_FRAMERATE_2400;
        return 1;
    }
    else if (!strcasecmp(text, "25"))
    {
        *framerate = DLB_PMD_FRAMERATE_2500;
        return 1;
    }
    else if (!strcasecmp(text, "29.97"))
    {
        *framerate = DLB_PMD_FRAMERATE_2997;
        return 1;
    }
    else if (!strcasecmp(text, "30"))
    {
        *framerate = DLB_PMD_FRAMERATE_3000;
        return 1;
    }
    else
    {
        errmsg(p, "Error: unknown frame rate \"%s\"", text);
        return 0;
    }
}


/**
 * @brief parse bsmod value, used in encoder config
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_bsmod
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_bitstream_mode *bsmod    /**< [out] bsmod */
    )
{
    static const char *bm0[] =
        { "Complete Main", "Music and Effects", "Visually Impaired", "Hearing Impaired",
          "Dialogue", "Commentary", "Emergency", "Voice Over" 
        };

    static const char *bm1[] = { "CM", "ME", "VI", "HI", "D", "C", "E", "VO" };
    int res;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    res = decode_enum(text, bm0, 8);
    if (res < 0)
    {
        res = decode_enum(text, bm1, 8);
        if (res < 0)
        {
            errmsg(p, "Error: unsupported bsmod \"%s\"", text);
            return 0;
        }
    }
    *bsmod = res;
    return 1;
}


/**
 * @brief parse dialnorm in BSI
 */
static 
int                               /** @return 0 on failure, 1 on success */
decode_dialnorm
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_dialogue_norm *val       /**< [out] dialogue normalization level */
    )
{
    unsigned int tmp;
    
    if (decode_uint(p, text, "dialnorm", 31, &tmp))
    {
        *val = (uint8_t)tmp;
        return 1;
    }
    return 0;
}


/**
 * @brief parse center downmix level
 */
static
int                                    /** @return 0 on failure, 1 on success */
decode_cmixlev
    (parser *p                         /**< [in] parser state */
    ,const char *text                  /**< [in] text to parse */
    ,pmd_cmix_level *mixlev            /**< [out] extbsi downmix level */
    )
{
    char *endp;
    double v;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    
    v = strtod(text, &endp);
    if (text_is_minf_db(text))
    {
        *mixlev = PMD_CMIX_LEVEL_MINF;
        return 1;
    }
    else if (v == 3.0)  *mixlev = PMD_CMIX_LEVEL_30;
    else if (v == 1.5)  *mixlev = PMD_CMIX_LEVEL_15;
    else if (v == 0.0)  *mixlev = PMD_CMIX_LEVEL_00;
    else if (v == -1.5) *mixlev = PMD_CMIX_LEVEL_M15;
    else if (v == -3.0) *mixlev = PMD_CMIX_LEVEL_M30;
    else if (v == -4.5) *mixlev = PMD_CMIX_LEVEL_M45;
    else if (v == -6.0) *mixlev = PMD_CMIX_LEVEL_M60;
    else goto error;
    
    while (*endp == ' ') ++endp;
    if (strcasecmp(endp, "dB")) goto error;
    return 1;

  error:
    errmsg(p, "Unknown downmix level, \"%s\"", text);
    return 0;
}


/**
 * @brief parse surround downmix level
 */
static
int                                    /** @return 0 on failure, 1 on success */
decode_surmixlev
    (parser *p                         /**< [in] parser state */
    ,const char *text                  /**< [in] text to parse */
    ,pmd_surmix_level *mixlev          /**< [out] extbsi downmix level */
    )
{
    char *endp;
    double v;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    
    v = strtod(text, &endp);
    if (text_is_minf_db(text))
    {
        *mixlev = PMD_SURMIX_LEVEL_MINF;
        return 1;
    }
    else if (v == -1.5) *mixlev = PMD_SURMIX_LEVEL_M15;
    else if (v == -3.0) *mixlev = PMD_SURMIX_LEVEL_M30;
    else if (v == -4.5) *mixlev = PMD_SURMIX_LEVEL_M45;
    else if (v == -6.0) *mixlev = PMD_SURMIX_LEVEL_M60;
    else goto error;
    
    while (*endp == ' ') ++endp;
    if (strcasecmp(endp, "dB")) goto error;
    return 1;

  error:
    errmsg(p, "Unknown downmix level, \"%s\"", text);
    return 0;
}


/**
 * @brief parse heights downmix level
 */
static
int                                    /** @return 0 on failure, 1 on success */
decode_hmixlev
    (parser *p                         /**< [in] parser state */
    ,const char *text                  /**< [in] text to parse */
    ,unsigned char *hmixlev            /**< [out] extbsi heights downmix level */
    )
{
    char *endp;
    double v;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    
    if (text_is_minf_db(text))
    {
        *hmixlev = 31;
        return 1;
    }

    v = strtod(text, &endp);
    if (v <= 0.0 && v >= -30.0 && endp != text)
    {
        *hmixlev = -v;
    }
    else
    {
        goto error;
    }
    
    while (*endp == ' ') ++endp;
    if (strcasecmp(endp, "dB")) goto error;
    return 1;

  error:
    errmsg(p, "Unknown heights downmix level, \"%s\"", text);
    return 0;
}


/**
 * @brief parse dsurmod indicator
 */
static 
int                               /** @return 0 on failure, 1 on success */
decode_dsurmod
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_surround_mode *dsurmod   /**< [out] surround mode */
    )
{
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (!strcasecmp(text, "Not Indicated")) *dsurmod = PMD_DSURMOD_NI;
    else if (!strcasecmp(text, "Not Dolby Surround Encoded")) *dsurmod = PMD_DSURMOD_NO;
    else if (!strcasecmp(text, "Dolby Surround Encoded")) *dsurmod = PMD_DSURMOD_YES;
    else
    {
        errmsg(p, "Invalid dsurmod: \"%s\"", text);
        return 0;
    }
    return 1;
}


/**
 * @brief parse compression mode (DRC)
 */
static 
int                               /** @return 0 on failure, 1 on success */
decode_compr
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_compression_mode *compr  /**< [out] compression mode */
    )
{
    static const char *modes[] = { "None", "Film Standard", "Film Light",
                                   "Music Standard", "Music Light", "Speech" };
    int res;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    res = decode_enum(text, modes, 6);
    if (res < 0)
    {
        errmsg(p, "Invalid compression mode: \"%s\"", text);
        return 0;
    }
    *compr = res;
    return 1;
}


/**
 * @brief parse surround 90: surrounds 90 degree phase shifted?
 */
static 
int                               /** @return 0 on failure, 1 on success */
decode_sur90
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_bool *sur90              /**< [out] surround-90 enabled */
    )
{
    static const char *ot[] = { "Off", "On" };
    int res;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    
    res = decode_enum(text, ot, 2);
    if (res < 0)
    {
        errmsg(p, "Invalid surround 90 setting: \"%s\"", text);
        return 0;
    }
    *sur90 = res;
    return 1;
}


/**
 * @brief parse preferred downmix mode setting (Extended BSI in encoder config)
 */
static 
int                                 /** @return 0 on failure, 1 on success */
decode_dmixmod
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    ,pmd_preferred_downmix *dmixmod /**< [out] preferred downmix */
    )
{
    static const char *ot1[] = { "Not Indicated", "LtRt", "LoRo", "PLII" };
    int res;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    
    res = decode_enum(text, ot1, 4);
    if (res < 0)
    {
        errmsg(p, "Invalid surround 90 setting: \"%s\"", text);
        return 0;
    }
    *dmixmod = res;
    return 1;
}


/**
 * @brief parse DE program config
 */
static
int                                    /** @return 0 on failure, 1 on success */
decode_pgmcfg
    (parser *p                         /**< [in] parser state */
    ,const char *text                  /**< [in] text to parse */
    ,dlb_pmd_de_program_config *pgmcfg /**< [out] DE program config */
    )
{
    static const char *pgmcfgs[] =
        { "5.1+2", "5.1+1+1", "4+4", "4+2+2", "4+2+1+1", "4+1+1+1+1",
          "2+2+2+2", "2+2+2+1+1", "2+2+1+1+1+1", "2+1+1+1+1+1+1",
          "1+1+1+1+1+1+1+1", "5.1", "4+2", "4+1+1", "2+2+2", "2+2+1+1",
          "2+1+1+1+1", "1+1+1+1+1+1", "4", "2+2", "2+1+1", "1+1+1+1",
          "7.1", "7.1s"
        };
#define NUM_PGMCFGS (sizeof(pgmcfgs)/sizeof(pgmcfgs[0]))
    
    char tmp[17];
    char *w;
    unsigned int i;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    w = tmp;
    i = 0;
    memset(tmp, '\0', sizeof(tmp));
    while (i < sizeof(tmp)-1 && text[i])
    {
        if (text[i] != ' ' && text[i] != '\t')
        {
            *w++ = text[i];
        }
        ++i;
    }
    
    if (i < sizeof(tmp)-1)
    {
        unsigned int j;

        for (j = 0; j != NUM_PGMCFGS; ++j)
        {
            if (!strcmp(tmp, pgmcfgs[j]))
            {
                *pgmcfg = (dlb_pmd_de_program_config)j;
                return 1;
            }
        }
    }
    errmsg(p, "Error: unsupported DE program config \"%s\"", text);
    return 0;
}


/**
 * @brief parse IAT content identifier field
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_content_id
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    ,pmd_iat_content_id_type type   /**< [in] content id type */
    ,pmd_bool ishex                 /**< [in] is hex, when content id is raw */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    
    switch (type)
    {
        case PMD_IAT_CONTENT_ID_UUID:
            if (read_uuid(text, iat->content_id.data))
            {
                iat->content_id.size = 16;
                iat->content_id.type = type;
                return 1;
            }
            errmsg(p, "Incorrect UUID \"%s\"", text);
            break;
            
        case PMD_IAT_CONTENT_ID_EIDR:
            if (read_eidr(text, iat->content_id.data))
            {
                iat->content_id.size = 12;
                iat->content_id.type = type;
                return 1;
            }
            errmsg(p, "Incorrect EIDR \"%s\"", text);
            break;
            
        case PMD_IAT_CONTENT_ID_AD_ID:
            if (read_ad_id(text, iat->content_id.data))
            {
                iat->content_id.size = (uint8_t)strlen((char*)iat->content_id.data);
                iat->content_id.type = type;
                return 1;
            }
            errmsg(p, "Incorrect Ad-ID \"%s\"", text);
            break;
            
        default:
        {
            size_t size = sizeof(iat->content_id.data);
            
            if (read_cdata(text, iat->content_id.data, &size, ishex))
            {
                errmsg(p, "Could not read IAT content ID");
                return 0;
            }
            iat->content_id.type = type;
            iat->content_id.size = (uint8_t)size;
            return 1;
        }
    }
    return 0;
}


/**
 * @brief parse IAT distribution identifier field, BSID subfield for ATSC3 Channel ID
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_distid_atsc3_bsid
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (iat->distribution_id.type != PMD_IAT_DISTRIBUTION_ID_ATSC3)
    {
        errmsg(p, "BSID field only pertains to ATSC3 VP1 Channel ID");
        return 0;
    }

    if (!(p->atsc3_unread_fields & 1))
    {
        errmsg(p, "BSID field occurs more than once");
        return 0;
    }

    if (decode_uint(p, text, "BSID", (1ul<<16)-1, &p->atsc3_channel_bsid))
    {
        p->atsc3_unread_fields &= ~1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse IAT distribution identifier field, Major channel subfield for ATSC3 Channel ID
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_distid_atsc3_majno
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (iat->distribution_id.type != PMD_IAT_DISTRIBUTION_ID_ATSC3)
    {
        errmsg(p, "Major_Channel_Number field only pertains to ATSC3 VP1 Channel ID");
        return 0;
    }

    if (!(p->atsc3_unread_fields & 2))
    {
        errmsg(p, "Major_Channel_Number field occurs more than once");
        return 0;
    }

    if (decode_uint(p, text, "Major_Channel_Number", (1ul<<10)-1, &p->atsc3_channel_majno))
    {
        p->atsc3_unread_fields &= ~2;
        return 1;
    }
    return 0;
}


/**
 * @brief parse IAT distribution identifier field, Minor Channel subfield for ATSC3 Channel ID
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_distid_atsc3_minno
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (iat->distribution_id.type != PMD_IAT_DISTRIBUTION_ID_ATSC3)
    {
        errmsg(p, "Minor_Channel_Number field only pertains to ATSC3 VP1 Channel ID");
        return 0;
    }

    if (!(p->atsc3_unread_fields & 4))
    {
        errmsg(p, "Minor_Channel_Number field occurs more than once");
        return 0;
    }

    if (decode_uint(p, text, "Minor_Channel_Number", (1ul<<10)-1, &p->atsc3_channel_minno))
    {
        p->atsc3_unread_fields &= ~4;
        return 1;
    }
    return 0;
}


/**
 * @brief parse IAT distribution identifier field, non-ATSC3
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_distid_raw
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    ,unsigned int ty                /**< [in] raw type */
    ,pmd_bool ishex                 /**< [in] 1 hex, 0 ascii */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;
    size_t size = sizeof(iat->distribution_id);

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (ty < 1 || ty > 7)
    {
        errmsg(p, "IAT raw distribution ID %u out of range\n", ty);
        return 0;
    }

    iat->distribution_id.type = ty;
    if (read_cdata(text, iat->distribution_id.data, &size, ishex))
    {
        errmsg(p, "Could not read IAT distribution ID");
        return 0;
    }
    iat->distribution_id.size = (uint8_t)size;
    return 1;
}


/**
 * @brief final assembly of distribution id
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_distid_default
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;

    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (iat->distribution_id.type == PMD_IAT_DISTRIBUTION_ID_ATSC3)
    {
        uint16_t bsid = p->atsc3_channel_bsid;
        uint16_t maj  = p->atsc3_channel_majno;
        uint16_t min  = p->atsc3_channel_minno;

        if (p->atsc3_unread_fields)
        {
            errmsg(p, "ATSC3 Channel ID incomplete");
            return 0;
        }

        if ('\0' != text[0])
        {
            errmsg(p, "unnecessary text after ATSC3 Channel ID");
            return 0;
        }

        iat->distribution_id.size = 5;
        iat->distribution_id.data[0] = (bsid >> 8) & 0xff;
        iat->distribution_id.data[1] = bsid & 0xff;
        iat->distribution_id.data[2] = 0xf0 | ((maj >> 6) & 0x0f);
        iat->distribution_id.data[3] = ((maj & 0x3f) << 2) | ((min >> 8) & 0x3);
        iat->distribution_id.data[4] = (min & 0xff);
        return 1;
    }
    return 1;
}


/**
 * @brief parse IAT timestamp
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_timestamp
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;
    char *endp;
    long long v;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    v = strtoll(text, &endp, 0);
    if (v < 0 || v > (long long)(1ull<<35)-1 || endp == text)
    {
        errmsg(p, "Invalid IAT Timestamp: \"%s\"", text);
        return 0;
    }
    iat->timestamp = (uint64_t)v;
    return 1;
}


/**
 * @brief parse IAT offset
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_offset
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;
    unsigned int tmp;
    
    if (decode_uint(p, text, "Offset", (1ul<<11)-1, &tmp))
    {
        iat->offset.present = 1;
        iat->offset.offset = (uint16_t)tmp;
        return 1;
    }
    return 0;
}


/**
 * @brief parse IAT validity duration
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_validity_dur
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;
    unsigned int tmp;
    
    if (decode_uint(p, text, "Validity_Duration", (1ul<<11)-1, &tmp))
    {
        iat->validity_duration.present = 1;
        iat->validity_duration.vdur = (uint16_t)tmp;
        return 1;
    }
    return 0;
}


/**
 * @brief parse IAT user data
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_user_data
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    ,pmd_bool ishex                 /**< [in] 1 for hex, 0 for ascii */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;
    size_t size = sizeof(iat->user_data);
        
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (read_cdata(text, iat->user_data.data, &size, ishex))
    {
        errmsg(p, "Could not read IAT User Data");
        return 0;
    }
    iat->user_data.size = (uint8_t)size;
    return 1;
}


/**
 * @brief parse IAT extension
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_iat_extension
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    ,pmd_bool ishex                 /**< [in] 1 hex encoded, 0 ascii */
    )
{
    dlb_pmd_identity_and_timing *iat = &p->iat;
    size_t size = sizeof(iat->extension.data);
        
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    if (read_cdata(text, iat->extension.data, &size, ishex))
    {
        errmsg(p, "Could not read IAT Extension Data");
        return 0;
    }
    iat->extension.size = (uint8_t)size * 8;
    return 1;
}


/**
 * @brief parse a presentation's ID
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_pld_presid
    (parser *p                    /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    )
{
    char *endp;
    int v;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    v = strtol(text, &endp, 0);
    if (v < 0 || v > MAX_PRESENTATIONS || endp == text)
    {
        errmsg(p, "Invalid presentation id: \"%s\"", text);
        return 0;
    }

    p->loudness.presid = (dlb_pmd_presentation_id)v;
    return 1;
}


/**
 * @brief parse loudness practice type
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_practype
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    static const char *lptname[] =
    {
        "NI", "ATSC", "EBU", "ARIB", "FreeTV", "5", "6", "7",
        "8", "9", "10", "11", "12", "13", "Manual", "Consumer"
    };
    
    char tmp[32];
    char *t;
    unsigned int i;
    size_t len;
    int res;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    memset(tmp, '\0', sizeof(tmp));
    len = strlen(text);
    t = tmp;
    for (i = 0; i != len; ++i)
    {
        t[i] = tolower(text[i]);
    }
    
    res = decode_enum(tmp, lptname, sizeof(lptname)/sizeof(lptname[0]));
    if (res < 0)
    {
        errmsg(p, "Error: unknown loudness practice type \"%s\"", text);
        return 0;
    }
    p->loudness.loud_prac_type = (dlb_pmd_loudness_practice)res;
    return 1;
}


/**
 * @brief parse loudness value
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_lufs
    (parser *p                      /**< [in] parser state */
    ,const char   *field            /**< [in] field name */
    ,const char   *text             /**< [in] text to parse */
    ,dlb_pmd_lufs *val              /**< [out] loudness value (if successful) */
    )
{
    char *endp;
    double f;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    f = strtod(text, &endp);
    if (f < DLB_PMD_LUFS_MIN || f > DLB_PMD_LUFS_MAX || endp == text)
    {
        errmsg(p, "Invalid LUFS value \"%s\" in field %s",  text, field);
        return 0;
    }
    *val = (dlb_pmd_lufs)f;
    return 1;
}


/**
 * @brief parse loudness value, relative-gated
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_loudrelgat
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "loudrelgat", text, &p->loudness.loudrelgat))
    {
        p->loudness.b_loudrelgat = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse loudness value, speech-gated
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_loudspchgat
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "loudspchgat", text, &p->loudness.loudspchgat))
    {
        p->loudness.b_loudspchgat = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse loudness value, short-term 3 seconds
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_loud3sgat
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "loud3sgat", text, &p->loudness.loudstrm3s))
    {
        p->loudness.b_loudstrm3s = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse max loudness value, short-term 3 seconds
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_maxloud3sgat
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "maxloud3sgat", text, &p->loudness.max_loudstrm3s))
    {
        p->loudness.b_max_loudstrm3s = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse true peak loudness
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_truepeak
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "truepeak", text, &p->loudness.truepk))
    {
        p->loudness.b_truepk = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse max true peak loudness
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_max_truepeak
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "maxtruepeak", text, &p->loudness.max_truepk))
    {
        p->loudness.b_max_truepk = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse loudness programme boundary
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_prgmbndy
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    char *endp;
    long v;
    long x;
    int sign;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    v = strtol(text, &endp, 0);
    x = labs(v);
    sign = v < 0 ? -1 : 1;
    if (x < 2 || x > 512 || endp == text)
    {
        errmsg(p, "Invalid programme boundary: \"%s\"", text);
        return 0;
    }
    /* check this is a power of two: */
    if ((x & (x - 1)) != 0)
    {
        errmsg(p, "Invalid programme boundary: \"%s\", must be a power of two\n", text);
        return 0;
    }
    /* now compute log-base-2 */
    switch (x)
    {
        case 2:     p->loudness.prgmbndy = (pmd_programme_boundary)1 * sign; break;
        case 4:     p->loudness.prgmbndy = (pmd_programme_boundary)2 * sign; break;
        case 8:     p->loudness.prgmbndy = (pmd_programme_boundary)3 * sign; break;
        case 16:    p->loudness.prgmbndy = (pmd_programme_boundary)4 * sign; break;
        case 32:    p->loudness.prgmbndy = (pmd_programme_boundary)5 * sign; break;
        case 64:    p->loudness.prgmbndy = (pmd_programme_boundary)6 * sign; break;
        case 128:   p->loudness.prgmbndy = (pmd_programme_boundary)7 * sign; break;
        case 256:   p->loudness.prgmbndy = (pmd_programme_boundary)8 * sign; break;
        case 512:   p->loudness.prgmbndy = (pmd_programme_boundary)9 * sign; break;
        default: abort(); break;
    }

    p->loudness.b_prgmbndy = 1;
    return 1;
}


/**
 * @brief parse loudness range
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_lra
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    char *endp;
    double f;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    f = strtod(text, &endp);
    if (f < DLB_PMD_LU_MIN || f > DLB_PMD_LU_MAX || endp == text)
    {
        errmsg(p, "Invalid LRA value \"%s\"",  text);
        return 0;
    }
    p->loudness.lra = (dlb_pmd_lu)f;
    p->loudness.b_lra = 1;
    return 1;
}


/**
 * @brief parse momentary loudness value
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_loudmntry
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "loudmntry", text, &p->loudness.loudmntry))
    {
        p->loudness.b_loudmntry = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse maximum momentary loudness value
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_max_loudmntry
    (parser *p                      /**< [in] parser state */
    ,const char *text               /**< [in] text to parse */
    )
{
    if (decode_pld_lufs(p, "maxloudmntry", text, &p->loudness.max_loudmntry))
    {
        p->loudness.b_max_loudmntry = 1;
        return 1;
    }
    return 0;
}


/**
 * @brief parse loudness extension
 */
static
int                                 /** @return 0 on failure, 1 on success */
decode_pld_extension
    (parser *p                      /**< [in] parser */
    ,const char *text               /**< [in] text to parse */
    ,pmd_bool ishex                 /**< [in] 1 if hex coded, 0 otherwise */
    )
{
    size_t size = sizeof(p->loudness.extension);

    if (read_cdata(text, p->loudness.extension.data, &size, ishex))
    {
        errmsg(p, "Could not read PLD extension");
        return 0;
    }

    if ((unsigned int)-1 == p->loudness.extension.size)
    {
        p->loudness.extension.size = (unsigned int)(size * 8);
    }
    return 1;
}


static
int                                 /** @return 0 on failure, 1 on success */
decode_signal_id
    (parser *p                      /**< [in] parser */
    ,const char *text               /**< [in] text to parse */
    ,dlb_pmd_signal *id             /**< [out] signal id */
    )
{
    unsigned int tmp;
    
    if (decode_uint(p, text, "signal id", DLB_PMD_MAX_SIGNAL_ID, &tmp))
    {
        *id = (dlb_pmd_signal)tmp;
        return 1;
    }
    return 0;
}


static
int                                 /** @return 0 on failure, 1 on success */
decode_element_id
    (parser *p                      /**< [in] parser */
    ,const char *text               /**< [in] text to parse */
    ,dlb_pmd_element_id *id         /**< [out] element id */
    )
{
    unsigned int tmp;
    
    if (decode_uint(p, text, "element id", DLB_PMD_MAX_ELEMENT_ID, &tmp))
    {
        *id = (dlb_pmd_element_id)tmp;
        return 1;
    }
    return 0;
}


static
int                                 /** @return 0 on failure, 1 on success */
decode_presentation_id
    (parser *p                      /**< [in] parser */
    ,const char *text               /**< [in] text to parse */
    ,dlb_pmd_presentation_id *id    /**< [out] element id */
    )
{
    unsigned int tmp;
    
    if (decode_uint(p, text, "presentation id", DLB_PMD_MAX_PRESENTATION_ID, &tmp))
    {
        *id = (dlb_pmd_presentation_id)tmp;
        return 1;
    }
    return 0;
}



#define parse_signal_id(s)       if (!decode_signal_id(p, text, &s)) error()
#define parse_element_id(e)      if (!decode_element_id(p, text, &e)) error()
#define parse_presentation_id(i) if (!decode_presentation_id(p, text, &i)) error()
#define parse_presentation_element(p) if (!decode_pres_element(p, text)) error()
#define parse_presentation_name(p) if (!decode_pres_name(p, text)) error()

#define parse_bool(b)              if (!decode_bool(p, text, &b)) error()

/* container config */
#define parse_dynamic_tag()        if (!decode_dynamic_tag(p, text)) error()
#define parse_sample_offset()      if (!decode_sample_offset(p, text)) error()

/* professional metadata */
#define parse_title()              if (!decode_title(p, text)) error()
#define parse_uint8(s)             if (!decode_uint8(p, text, "uint8", &s)) error()
#define parse_speaker_config(s)    if (!decode_speaker_config(p, text, s)) error()
#define parse_presentation_config(s) if (!decode_presentation_config(p, text, &s)) error()
#define parse_object_class(oc)     if (!decode_object_class(p, text, &oc)) error()

#define parse_coordinate(s)        if (!decode_coordinate(p, text, &s)) error()
#define parse_size(s)              if (!decode_size(p, text, &s)) error() 
#define parse_gain(g)              if (!decode_gain(p, text, &g)) error()


#define parse_hed_element(p)      if (!decode_element_id(p, text, &p->headphone.audio_element_id)) error()
#define parse_hed_tracking(p)     if (!decode_bool(p, text, &p->headphone.head_tracking_enabled)) error()
#define parse_hed_render_mode(p)  if (!decode_hed_render_mode(p, text, &p->headphone.render_mode)) error()
#define parse_hed_channel_id(p)   if (!decode_hed_channel_id(p, text, &p->headphone)) error()

#define parse_language(lc)        if (!decode_language(p, text, lc)) error()
#define parse_eep_id(id)          if (!decode_eepid(p, text, &id)) error()

#define parse_pld_presid(p)       if (!decode_pld_presid(p, text)) error()
#define parse_pld_practype(p)     if (!decode_pld_practype(p, text)) error()
#define parse_pld_loudrelgat(p)   if (!decode_pld_loudrelgat(p, text)) error()
#define parse_pld_loudspchgat(p)  if (!decode_pld_loudspchgat(p, text)) error()
#define parse_pld_loud3sgat(p)    if (!decode_pld_loud3sgat(p, text)) error()
#define parse_pld_maxloud3sgat(p) if (!decode_pld_maxloud3sgat(p, text)) error()
#define parse_pld_truepeak(p)     if (!decode_pld_truepeak(p, text)) error()
#define parse_pld_max_truepeak(p) if (!decode_pld_max_truepeak(p, text)) error()
#define parse_pld_prgmbndy(p)     if (!decode_pld_prgmbndy(p, text)) error()
#define parse_pld_lra(p)          if (!decode_pld_lra(p, text)) error()
#define parse_pld_loudmntry(p)    if (!decode_pld_loudmntry(p, text)) error()
#define parse_pld_max_loudmntry(p)if (!decode_pld_max_loudmntry(p, text)) error()
#define parse_pld_extension(p,h)  if (!decode_pld_extension(p, text, h)) error()

#define parse_element_name(el)    if (!decode_string(p, text, (uint8_t*)el, DLB_PMD_MAX_NAME_LENGTH)) error()

/* AC3 program metadata */
#define parse_datarate(datarate)   if (!decode_datarate(p, text, &datarate)) error()
#define parse_framerate(framerate) if (!decode_framerate(p, text, &framerate)) error()
#define parse_bsmod(bsmod)         if (!decode_bsmod(p, text, &bsmod)) error()
#define parse_dsurmod(dsurmod)     if (!decode_dsurmod(p, text, &dsurmod)) error()
#define parse_dialnorm(dialnorm)   if (!decode_dialnorm(p, text, &dialnorm)) error()
#define parse_compr(compr)         if (!decode_compr(p, text, &compr)) error()
#define parse_surround90(sur90)    if (!decode_sur90(p, text, &sur90)) error()
#define parse_dmixmod(dmixmod)     if (!decode_dmixmod(p, text, &dmixmod)) error()
#define parse_cmixlev(mixlev)      if (!decode_cmixlev(p, text, &mixlev)) error()
#define parse_surmixlev(mixlev)    if (!decode_surmixlev(p, text, &mixlev)) error()
#define parse_hmixlev(mixlev)      if (!decode_hmixlev(p, text, &mixlev)) error()
#define parse_pgmcfg(pgmcfg)       if (!decode_pgmcfg(p, text, &pgmcfg)) error()

/* IAT */
#define parse_iat_content_id(p,ty,h)      if (!decode_iat_content_id(p, text, ty, h)) error()
#define parse_iat_distid_atsc3_bsid(p)    if (!decode_iat_distid_atsc3_bsid(p, text)) error()
#define parse_iat_distid_atsc3_majno(p)   if (!decode_iat_distid_atsc3_majno(p, text)) error()
#define parse_iat_distid_atsc3_minno(p)   if (!decode_iat_distid_atsc3_minno(p, text)) error()
#define parse_iat_distid_raw(p,ty,h)      if (!decode_iat_distid_raw(p, text, ty, h)) error()
#define parse_iat_distid_default(p)       if (!decode_iat_distid_default(p, text)) error()
#define parse_iat_timestamp(p)            if (!decode_iat_timestamp(p, text)) error()
#define parse_iat_offset(p)               if (!decode_iat_offset(p, text)) error()
#define parse_iat_validity_duration(p)    if (!decode_iat_validity_dur(p, text)) error()
#define parse_iat_user_data(p,h)          if (!decode_iat_user_data(p, text, h)) error()
#define parse_iat_extension(p,h)          if (!decode_iat_extension(p, text, h)) error()


/* -------------------------------------- POST-PARSING VALIDATION ----------------------------- */


static
int
element_callback
    (void *context
    ,char *tag
    ,char *text
    )
{
    parser *p = (parser *)context;

    COROUTINE_BEGIN;

    if_begin_tag("Smpte2109",)
    {
        if_begin_tag("ContainerConfig",)
        {
            if_tag("SampleOffset") parse_sample_offset();
            elif_begin_tag("DynamicTags",)
            {
                if_tag("Tag") parse_dynamic_tag();
                endif();
            }
            endif();
        }
        elif_begin_tag("ProfessionalMetadata", set_profile(p))
        {
            if_tag("Title")
            {
                parse_title();
            }
            elif_begin_tag("AudioSignals",)
            {
                if_begin_popaction_tag("AudioSignal",,add_signal_to_model)
                {
                    if_tag("Name");
                    endif();
                }
                endif();
            }
            elif_begin_tag("AudioElements",)
            {
                if_begin_popaction_tag("AudioBed", new_bed(p), add_bed_to_model)
                {
                    if_tag("Name")            parse_element_name(p->bed.name);
                    elif_tag("SpeakerConfig") parse_speaker_config(&p->bed.config);
                    elif_tag("SourceBedId")
                    {
                        parse_element_id(p->bed.source_id);
                        p->bed.bed_type = PMD_BED_DERIVED;
                    }
                    elif_begin_tag("OutputTargets",)
                    {
                        if_begin_tag("OutputTarget",)
                        {
                            if_begin_tag("AudioSignals",)
                            {
                                if_tag("ID")
                                {
                                    parse_signal_id(p->source->source);
                                    /* current target set by attribute_callback
                                     * when parsing <OutputTarget> tag */
                                    p->source->target = (pmd_speaker)p->current_target;
                                    /* gain set by attribute_callback when parsing <ID> tag */
                                    p->source->gain = p->source_gain_db;
                                    /* reset for next source */
                                    p->source_gain_db = 0.0f;
                                    p->source += 1;
                                    p->bed.num_sources += 1;
                                }
                                endif();
                            }
                            endif();
                        }
                        endif();
                    }
                    endif();
                }
                elif_begin_popaction_tag("AudioObject", new_object(p), add_object_to_model)
                {
                    if_tag("Name")               parse_element_name   (p->object.name);
                    elif_tag("Class")            parse_object_class   (p->object.object_class); 
                    elif_tag("DynamicUpdates")   parse_bool           (p->object.dynamic_updates);
                    elif_tag("X_Pos")            parse_coordinate     (p->object.x);
                    elif_tag("Y_Pos")            parse_coordinate     (p->object.y);
                    elif_tag("Z_Pos")            parse_coordinate     (p->object.z);
                    elif_tag("Size")             parse_size           (p->object.size);
                    elif_tag("Size_3D")          parse_bool           (p->object.size_3d);
                    elif_tag("Diverge")          parse_bool           (p->object.diverge);
                    elif_tag("AudioSignal")      parse_signal_id      (p->object.source);
                    elif_tag("SourceGainDB")     parse_gain           (p->object.source_gain);
                    endif();
                }
                endif();
            }
            elif_begin_tag("Presentations",)
            {
                if_begin_popaction_tag("Presentation", new_presentation(p), add_presentation_to_model)
                {
                    if_tag("Config")     parse_presentation_config(p->presentation.config);
                    elif_tag("Language") parse_language(p->presentation.audio_language);
                    elif_tag("Element")  parse_presentation_element(p);
                    elif_tag("Name")     parse_presentation_name(p)
                    endif();
                }
                endif();
            }
            elif_begin_tag("PresentationLoudness",)
            {
                if_begin_popaction_tag("Presentation", new_loudness(p), add_loudness_to_model)
                {
                    if_tag("PresentationId")          parse_pld_presid(p);
                    elif_tag("PracticeType")          parse_pld_practype(p);
                    elif_tag("LoudnessRelativeGated") parse_pld_loudrelgat(p);
                    elif_tag("LoudnessSpeechGated")   parse_pld_loudspchgat(p);
                    elif_tag("Loudness3Seconds")      parse_pld_loud3sgat(p);
                    elif_tag("MaxLoudness3Seconds")   parse_pld_maxloud3sgat(p);
                    elif_tag("TruePeak")              parse_pld_truepeak(p);
                    elif_tag("MaxTruePeak")           parse_pld_max_truepeak(p);
                    elif_tag("ProgramBoundary")       parse_pld_prgmbndy(p);
                    elif_tag("LoudnessRange")         parse_pld_lra(p);
                    elif_tag("MomentaryLoudness")     parse_pld_loudmntry(p);
                    elif_tag("MaxMomentaryLoudness")  parse_pld_max_loudmntry(p);
                    elif_open_tag("Extension")
                    {
                        if_tag("ascii")    parse_pld_extension(p, 0);
                        elif_tag("base16") parse_pld_extension(p, 1);
                        endif();
                    }
                    endif();
                }
                endif();
            }
            elif_begin_tag("EncoderConfigurations",)
            {
                if_begin_popaction_tag("Eac3EncodingParameters", new_eac3(p), add_eac3_to_model)
                {
                    if_tag("Name");
                    elif_begin_tag("Bitstream",)
                    {
                        p->eac3.b_bitstream_params = 1;
                        if_tag("BsMod")             parse_bsmod(p->eac3.bsmod);
                        elif_tag("SurMod")          parse_dsurmod(p->eac3.dsurmod);
                        elif_tag("Dialnorm")        parse_dialnorm(p->eac3.dialnorm);
                        elif_tag("PrefDMixMod")     parse_dmixmod(p->eac3.dmixmod);
                        elif_tag("LtRtCMixLev")     parse_cmixlev(p->eac3.ltrtcmixlev);
                        elif_tag("LtRtSurMixLev")   parse_surmixlev(p->eac3.ltrtsurmixlev);
                        elif_tag("LoRoCMixLev")     parse_cmixlev(p->eac3.lorocmixlev);
                        elif_tag("LoRoSurMixLev")   parse_surmixlev(p->eac3.lorosurmixlev);
                        endif();
                    }
                    elif_begin_tag("Encoder",)
                    {
                        p->eac3.b_encoder_params = 1;
                        if_tag("DynrngProf")    parse_compr(p->eac3.dynrng_prof);
                        elif_tag("ComprProf")   parse_compr(p->eac3.compr_prof);
                        elif_tag("Surround90")  parse_surround90(p->eac3.surround90);
                        elif_tag("HMixLev")     parse_hmixlev(p->eac3.hmixlev);
                        endif();
                    }
                    elif_begin_tag("DRC",)
                    {
                        p->eac3.b_drc_params = 1;
                        if_tag("Portable_Speakers_DRC_Profile")     parse_compr(p->eac3.drc_port_spkr);
                        elif_tag("Portable_Headphones_DRC_Profile") parse_compr(p->eac3.drc_port_hphone);
                        elif_tag("Flat_Panel_DRC_Profile")          parse_compr(p->eac3.drc_flat_panl);
                        elif_tag("Home_Theater_DRC_Profile")        parse_compr(p->eac3.drc_home_thtr);
                        elif_tag("DDplus_DRC_Profile")              parse_compr(p->eac3.drc_ddplus);
                        endif();
                    }
                    elif_begin_tag("Presentations", )
                    {
                        if_tag("ID")
                        {
                            if (p->eac3.num_presentations == PMD_EEP_MAX_PRESENTATIONS)
                            {
                                errmsg(p, "Too many presentation IDS in encoding params");
                                return 1;
                            }
                            parse_presentation_id(p->eac3.presentations[p->eac3.num_presentations])
                            p->eac3.num_presentations += 1;
                        }
                        endif();
                    }
                    endif();
                }
                elif_begin_popaction_tag("ED2Turnaround", new_etd(p), add_ed2_turnaround_to_model)
                {
                    if_tag("Name");
                    elif_begin_tag("ED2",)
                    {
                        if_tag("FrameRate")  parse_framerate(p->etd.ed2_framerate);
                        elif_begin_tag("Presentations",)
                        {
                            if_begin_tag("Presentation", new_ed2_turnaround(p))
                            {
                                if_tag("ID")
                                {
                                    parse_presentation_id(p->turnaround->presid);
                                }
                                elif_tag("Eac3EncodingParameters")
                                {
                                    parse_eep_id(p->turnaround->eepid);
                                }
                                endif();
                            }
                            endif();
                        }
                        endif();
                    }
                    elif_begin_tag("DolbyE",)
                    {
                        if_tag("FrameRate")  parse_framerate(p->etd.de_framerate);
                        elif_tag("ProgramConfiguration") parse_pgmcfg(p->etd.pgm_config);
                        elif_begin_tag("Presentations",)
                        {
                            if_begin_tag("Presentation", new_de_turnaround(p))
                            {
                                if_tag("ID")
                                {
                                    parse_presentation_id(p->turnaround->presid);
                                }
                                elif_tag("Eac3EncodingParameters")
                                {
                                    parse_eep_id(p->turnaround->eepid);
                                }
                                endif();
                            }
                            endif();
                        }
                        endif();
                    }
                    endif();
                }
                endif();
            }
            elif_begin_popaction_tag("DynamicUpdate", new_update(p), add_update_to_model)
            {
                loop_until_closed("DynamicUpdate")
                {
                    if_tag("ID")      parse_element_id(p->update.id);
                    elif_tag("X_Pos") parse_coordinate(p->update.x);
                    elif_tag("Y_Pos") parse_coordinate(p->update.y);
                    elif_tag("Z_Pos") parse_coordinate(p->update.z);
                    endif();
                }
                end_loop();
            }
            elif_begin_popaction_tag("IAT", new_iat(p), add_iat_to_model)
            {
                if_open_tag("Content_ID")
                {
                    if_tag("UUID")    parse_iat_content_id(p, PMD_IAT_CONTENT_ID_UUID, 0);
                    elif_tag("EIDR")  parse_iat_content_id(p, PMD_IAT_CONTENT_ID_EIDR, 0);
                    elif_tag("Ad-ID") parse_iat_content_id(p, PMD_IAT_CONTENT_ID_AD_ID, 0);
                    elif_open_tag("Raw")
                    {
                        if_tag("ascii")    parse_iat_content_id(p, p->current_raw_type, 0);
                        elif_tag("base16") parse_iat_content_id(p, p->current_raw_type, 1);
                        endif();
                    }
                    endif();
                }
                elif_open_tag("Distribution_ID")
                {
                    if_open_tag("ATSC3")
                    {
                        p->iat.distribution_id.type = PMD_IAT_DISTRIBUTION_ID_ATSC3;
                        p->atsc3_channel_bsid = 0;
                        p->atsc3_channel_majno = 0;
                        p->atsc3_channel_minno = 0;
                        p->atsc3_unread_fields = 0x7;
                        
                        loop_until_closed("ATSC3")
                        {
                            if_tag("BroadcastStreamID")      parse_iat_distid_atsc3_bsid(p);
                            elif_tag("Major_Channel_Number") parse_iat_distid_atsc3_majno(p);
                            elif_tag("Minor_Channel_Number") parse_iat_distid_atsc3_minno(p);
                            endif();
                        }
                        end_loop();
                    }
                    elif_open_tag("Raw")
                    {
                        if_tag("ascii")    parse_iat_distid_raw(p, p->current_raw_type, 0);
                        elif_tag("base16") parse_iat_distid_raw(p, p->current_raw_type, 1);
                        endif();
                    }
                    endif();
                    parse_iat_distid_default(p);
                }
                elif_tag("Timestamp")         parse_iat_timestamp(p);
                elif_tag("Offset"   )         parse_iat_offset(p);
                elif_tag("Validity_Duration") parse_iat_validity_duration(p);
                elif_open_tag("User_Data")
                {
                    if_tag("ascii")    parse_iat_user_data(p, 0);
                    elif_tag("base16") parse_iat_user_data(p, 1);
                    endif();
                }
                elif_open_tag("Extension")
                {
                    if_tag("ascii")    parse_iat_extension(p, 0);
                    elif_tag("base16") parse_iat_extension(p, 1);
                    endif();
                }
                endif();
            }
            elif_begin_tag("HeadphoneElements",)
            {
                if_begin_popaction_tag("HeadphoneElement", new_headphone(p), add_headphone_to_model)
                {
                    if_tag("Element")                    parse_hed_element(p);
                    elif_tag("HeadTrackingEnabled")      parse_hed_tracking(p);
                    elif_tag("RenderMode")               parse_hed_render_mode(p);
                    elif_begin_tag("ChannelExclusions",)
                    {
                        if_tag("ID") parse_hed_channel_id(p);
                        endif();
                    }
                    endif();
                }
                endif();
            }
            endif();
        }
        endif();
    }
    end_if_begin_tag();    
    
    COROUTINE_END;
    
    return 0;
}
    
    
static
int
attribute_callback
    (void *context
    ,char *tag
    ,char *attribute
    ,char *value
    )
{
    parser *p = (parser*)context;
    unsigned int time;
    unsigned int id;
    char *endp;

    if (!strcasecmp(tag, "xml"))
    {
        return 0;
    }

    if (!strcasecmp(attribute, "version"))
    {
        if (!strcasecmp(tag, "ProfessionalMetadata"))
        {
            int maj;
            int min;
            int pos;
            if (2 != sscanf(value, "%d.%d%n", &maj, &min, &pos)
                || maj < 0 || maj > 255
                || min < 0 || min > 255
                || value[pos] != '\0')
            {
                errmsg(p, "incorrect version format \"%s\"", value);
                return 1;
            }

            if (!global_testing_version_numbers)
            {
                if (p->model->version_avail)
                {
                    if (maj != p->model->version_maj || min != p->model->version_min)
                    {
                        errmsg(p, "version number already specified as %u.%u, not %u.%u",
                               p->model->version_maj, p->model->version_min, maj, min);
                        return 1;
                    }
                }

                /* the version check is disabled only when testing that
                 * version numbers are encoded/decoded properly as part of
                 * unit testing */
                if (maj != PMD_BITSTREAM_VERSION_MAJOR)
                {
                    errmsg(p, "major version number in XML (%u) "
                           "incompatible with supported version (%u)",
                           maj, PMD_BITSTREAM_VERSION_MAJOR);
                    return 1;
                }
            }
            
            p->model->version_avail = 1;
            p->model->version_maj = (uint8_t)(maj & 0xff);
            p->model->version_min = (uint8_t)(min & 0xff);
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "profile_number"))
    {
        if (!strcasecmp(tag, "ProfessionalMetadata"))
        {
            long int tmp = strtol(value, &endp, 0);
            if (tmp < 1 || tmp > MAX_PROFILE_NUMBER)
            {
                errmsg(p, "profile_number attribute has illegal value %ld", tmp);
                return 1;
            }
            p->profile_number = (unsigned int)tmp;
        }
    }
    else if (!strcasecmp(attribute, "profile_level"))
    {
        if (!strcasecmp(tag, "ProfessionalMetadata"))
        {
            long int tmp = strtol(value, &endp, 0);
            if (tmp < 1 || tmp > MAX_PROFILE_LEVEL)
            {
                errmsg(p, "profile_level attribute has illegal value %ld", tmp);
                return 1;
            }
            p->profile_level = (unsigned int)tmp;
        }
    }
    else if (!strcasecmp(attribute, "id"))
    {
        if (!strcasecmp(tag, "Tag"))
        {
            unsigned int localtag;
            int pos;
            if (1 != sscanf(value, "%x%n", &localtag, &pos)
                || localtag > 255
                || value[pos] != '\0')
            {
                errmsg(p, "incorrect Local Tag format \"%s\"", value);
                return 1;
            }
            p->current_dynamic_tag = localtag;
            return 0;
        }
        else if (!strcasecmp(tag, "OutputTarget"))
        {
            if (!decode_speaker_name(p, value, &p->current_target))
            {
                return 1;
            }
            return 0;
        }
        if (!decode_uint(p, value, "ID", DLB_PMD_MAX_AUDIO_ELEMENTS, &id))
        {
            errmsg(p, "id attribute on tag %s has bad value %s (expected: 1 - %d)\n",
                   tag, value, DLB_PMD_MAX_AUDIO_ELEMENTS);
            return 1;
        }
        
        if (!strcasecmp(tag, "AudioSignal"))
        {
            p->current_signal_id = id;
            return 0;
        }
        else if (!strcasecmp(tag, "AudioBed"))
        {
            p->current_element_id = id;
            return 0;
        }
        else if (!strcasecmp(tag, "AudioObject"))
        {
            p->current_element_id = id;
            return 0;
        }
        else if (!strcasecmp(tag, "Presentation"))
        {
            p->current_presentation_id = id;
            return 0;
        }
        else if (!strcasecmp(tag, "Eac3EncodingParameters"))
        {
            p->current_eac3_id = id;
            return 0;
        }
        else if (!strcasecmp(tag, "ED2Turnaround"))
        {
            p->current_etd_id = id;
            return 0;
        }
        else if (!strcasecmp(tag, "HeadphoneElement"))
        {
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "source_gain_db"))
    {
        return !decode_gain(p, value, &p->source_gain_db);
    }
    else if (!strcasecmp(attribute, "sample_time"))
    {
        if (!strcasecmp(tag, "DynamicUpdate"))
        {
            if (!decode_uint(p, value, "update time", DLB_PMD_MAX_UPDATE_TIME, &time))
            {
                errmsg(p, "sample_time attribute on tag %s has bad value %s (expected: 0 - %d)\n",
                       tag, value, DLB_PMD_MAX_UPDATE_TIME);
                return 1;
            }
            /* round down to 32-block chunks */
            p->update_time = time;
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "type"))
    {
        if (!strcasecmp(tag, "Raw"))
        {
            if (decode_uint(p, value, "Raw type attribute", 0x1e, &p->current_raw_type))
            {
                return 0;
            }
            errmsg(p, "unable to parse raw type \"%s\"", value);
        }
    }
    /* loudness */
    else if (!strcasecmp(attribute, "dialgate"))
    {
        static const char *dialgate[] =
        {
            "NI", "Center", "Front", "Manual", "4", "5", "6", "7"
        };

        if (!strcasecmp(tag, "PracticeType"))
        {
            int idx = decode_enum(value, dialgate, 8);
            if (idx < 0)
            {
                errmsg(p, "Unknown dialgate value \"%s\" in %s tag", value, tag);
                return 1;
            }
            p->loudness.loudcorr_gating = (dlb_pmd_dialgate_practice)idx;
            p->loudness.b_loudcorr_gating = 1;
            return 0;
        }
        else if (!strcasecmp(tag, "LoudnessSpeechGated"))
        {
            int idx = decode_enum(value, dialgate, 4);
            if (idx < 0)
            {
                errmsg(p, "Unknown dialgate value \"%s\" in %s tag", value, tag);
                return 1;
            }
            p->loudness.loudspch_gating = (dlb_pmd_dialgate_practice)idx;
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "offset"))
    {
        if (!strcasecmp(tag, "ProgramBoundary"))
        {
            if (!decode_uint(p, value, "Programme boundary offset",
                             2047, &p->loudness.prgmbndy_offset))
            {
                return 1;
            }
            p->loudness.b_prgmbndy_offset = 1;
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "practice"))
    {
        if (!strcasecmp(tag, "LoudnessRange"))
        {
            unsigned int tmp;
            int res = !decode_uint(p, value, "Loudness Range Practice", 1, &tmp);
            p->loudness.lra_prac_type = (dlb_pmd_loudness_range_practice)tmp;
            return res;
        }
    }
    else if (!strcasecmp(attribute, "bits"))
    {
        unsigned int bits;

        if (!decode_uint(p, value, "bits", 8*sizeof(p->loudness.extension.data), &bits))
        {
            p->loudness.extension.size = bits;
            return 0;
        }
        return 1;
    }
    else if (!strcasecmp(attribute, "correction_type"))
    {
        if (!strcasecmp(tag, "PracticeType"))
        {
            if (!strcasecmp(value, "file"))
            {
                p->loudness.loudcorr_type = PMD_PLD_CORRECTION_FILE_BASED;
                return 0;
            }
            if (!strcasecmp(value, "realtime"))
            {
                p->loudness.loudcorr_type = PMD_PLD_CORRECTION_REALTIME;
                return 0;
            }
            errmsg(p, "Unknown loudness correction type \"%s\"\n", value);
            return 1;
        }
    }
    else if (!strcasecmp(attribute, "language"))
    {
        pmd_langcode code;
        if (!strcasecmp(tag, "Name"))
        {
            if (!decode_langcode(p, value, &code))
            {
                errmsg(p, "Unknown language code \"%s\"\n", code);
                return 1;
            }
            memmove(p->current_language, value, sizeof(p->current_language));
            return 0;
        }
    }
    
    errmsg(p, "Unexpected attribute %s on tag %s (with value %s)\n", attribute, tag, value);
    return 1;
}


static
char*  /** has to return ptr to 1st char of next line, or NULL when no more available */
line_callback
    (void *context
    )
{
    parser *p = (parser *)context;
    p->line = p->lcb(p->cbarg);
    p->lineno += 1;
    return (char*)p->line;
}


static
void
dlb_xml_error_callback
    (void *context
    ,char *msg
    )
{
    parser *p = (parser *)context;
    if (NULL != p->ecb)
    {
        char report[1024];
        snprintf(report, sizeof(report), "ERROR at line %u: %s", p->lineno, msg);
        p->ecb(report, p->cbarg);
    }
}


dlb_pmd_bool
dlb_xmlpmd_is_pmd
    (const char *buffer
    ,size_t length
    )
{
    char tmp[1024];
    unsigned int i;
    
    memset(tmp, '\0', sizeof(tmp));
    
    if (length > sizeof(tmp)-1)
    {
        length = sizeof(tmp)-1;
    }
    
    for (i = 0; i != length; ++i)
    {
        tmp[i] = tolower(*buffer++);
    }
        
    if (NULL != strstr(tmp, "<smpte2109"))
    {
        return 1;
    }
    return 0;
}


dlb_pmd_success
dlb_xmlpmd_parse
    (dlb_xmlpmd_line_callback lcb
    ,dlb_xmlpmd_error_callback ecb
    ,void *cbarg
    ,dlb_pmd_model *model
    ,dlb_pmd_bool strict
    )
{
    dlb_pmd_success res = PMD_SUCCESS;
    parser p;
    
    pmd_mutex_lock(&model->lock);

    p.coroutine_line = 0;
    p.lcb = lcb;
    p.ecb = ecb;
    p.cbarg = cbarg;
    p.line = NULL;
    p.lineno = 0;
    p.model = model;
    tag_stack_init(&p.tagstack);
    p.current_dynamic_tag = (unsigned int)-1;
    p.source_gain_db = 0.0f;
    p.profile_number = 0;
    p.profile_level = 0;
    p.strict = strict;

    model->version_avail = 0;
    model->version_maj = 0xff;
    model->version_min = 0xff;

    if (dlb_xml_parse2(&p, &line_callback, &element_callback, &attribute_callback,
                       dlb_xml_error_callback))
    {
        res = PMD_FAIL;
    }
    if (!model->version_avail)
    {
        errmsg(&p, "PMD version not specified");
        res = PMD_FAIL;
    }

    pmd_mutex_unlock(&model->lock);

    return res;
}

