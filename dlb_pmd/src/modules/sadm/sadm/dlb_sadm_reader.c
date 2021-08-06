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
 * @file dlb_sadm_reader.c
 * @brief read in Dolby-Serial ADM XML format and populate sADM model
 */

/* taken from the PMD's own XML reader/writer module */
#include "xml_hex.h"
#include "xml_uuid.h"
#include "xml_eidr.h"
#include "xml_ad_id.h"
#include "xml_cdata.h"

#include "pmd_model.h"
#include "dlb_pmd_xml.h"
#include "dlb_xml/include/dlb_xml.h"

#include "sadm/dlb_sadm_reader.h"
#include "dlb_sadm_common_definitions.h"
#include "memstuff.h"

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
 * define SADM_XML_DEBUG to enable parser tracing
 */
//#define SADM_XML_DEBUG
#ifdef SADM_XML_DEBUG
#  define SADM_XML_TRACE(x) printf x
#else
#  define SADM_XML_TRACE(x)
#endif

#define TRACE(x) SADM_XML_TRACE(x)


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
#include "pmd_idmap.h"
#include "sadm/dlb_sadm_model.h"


/**
 * @def IGNORE_AUDIO_FORMAT_CUSTOM_ELEMENT
 *
 * @brief If this is defined, we'll ignore entirely the audioFormatCustom
 * element. Otherwise, we'll try to parse it.  But we only know about one
 * kind of custom audioFormatCustomSet, and the reader will break if there
 * is another kind of audioFormatCustomSet present.  So, ignore it for now.
 */
#define IGNORE_AUDIO_FORMAT_CUSTOM_ELEMENT

/**
 * @brief identify coordinate when parsing audioBlockFormat position tags
 */
typedef enum
{
    COORD_NONE,
    COORD_X,
    COORD_Y,
    COORD_Z,
    COORD_AZIMUTH,
    COORD_ELEVATION,
    COORD_DISTANCE
} coordinate_type;

static const char *coordinate_type_names[] = { "X", "Y", "Z", "azimuth", "elevation", "distance" };

/**
 * @brief record relationship between track_uid and channel number (for forward references in transportTrackFormat)
 */
typedef struct 
{
    dlb_sadm_id track_uid;
    unsigned int channel_number;
} track_uid_to_channel_record;

/**
 * @brief record information about parsing audioBlockFormat coordinates
 */
typedef struct 
{
    dlb_pmd_bool got_cartesian;

    dlb_pmd_bool got_x;
    dlb_pmd_bool got_y;
    dlb_pmd_bool got_z;

    dlb_pmd_bool got_azimuth;
    dlb_pmd_bool got_elevation;
    dlb_pmd_bool got_distance;

} block_format_state;


/**
 * @brief state of parser
 */
struct dlb_sadm_reader
{
    unsigned int coroutine_line;   /**< coroutine return point */

    tag_stack tagstack;            /**< current tag nesting stack, (for printing on error) */
    dlb_xmlpmd_line_callback lcb;  /**< line buffer request callback */
    dlb_xmlpmd_error_callback ecb; /**< error report callback or NULL */
    void *cbarg;                   /**< client-supplied callback argument */
    const char *line;              /**< pointer to current line (for printing on error) */
    unsigned int lineno;           /**< current XML file line number */

    dlb_sadm_model *model;         /**< current model */
    dlb_sadm_counts limits;

    /* associate track UID to channel number*/
    track_uid_to_channel_record *channel_assignments;
    size_t channel_assignment_count;

    /* temporary attribute parsing */
    unsigned int ignore_attributes;
    uint8_t current_name[DLB_PMD_NAME_ARRAY_SIZE];
    uint8_t current_id  [DLB_PMD_NAME_ARRAY_SIZE];
    uint8_t mixed_content_kind[DLB_PMD_NAME_ARRAY_SIZE];
    uint8_t type_definition[DLB_PMD_NAME_ARRAY_SIZE];
    uint8_t type_label[DLB_PMD_NAME_ARRAY_SIZE];
    uint8_t coordinate_name[DLB_PMD_NAME_ARRAY_SIZE];
    char current_language[4];
    coordinate_type current_coordinate_type;
    unsigned int current_track_id;
    dlb_pmd_bool gain_unit_db;
    block_format_state blkfmt_state;

    dlb_sadm_programme programme;
    dlb_sadm_content content;
    dlb_sadm_object object;
    dlb_sadm_pack_format packfmt;
    dlb_sadm_channel_format chanfmt;
    dlb_sadm_block_format blkfmt;
    dlb_sadm_stream_format streamfmt;
    dlb_sadm_track_format trackfmt;
    dlb_sadm_track_uid track_uid;

    dlb_sadm_programme_label *programme_labels;
    dlb_sadm_idref *programme_contents;
    dlb_sadm_idref *content_objects;
    dlb_sadm_idref *object_objects;
    dlb_sadm_idref *object_track_uids;
    dlb_sadm_idref *packfmt_chanfmts;
    dlb_sadm_idref *chanfmt_blkfmts;
};
    

/**
 * @brief send an error message to the client's error callback
 */
static
void
errmsg
    (dlb_sadm_reader *p    /**< [in] parser state */
    ,const char *fmt       /**< [in] message format */
    ,...                   /**< [in] misc args */
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
 * @brief generate parser tracing messages (if debugging)
 */
static
void
parser_trace
    (dlb_sadm_reader *p
    ,const char *fmt
    ,...
    )
{
#ifdef SADM_XML_DEBUG
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
    dlb_sadm_reader *p = (dlb_sadm_reader *)context;
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

/**
 * @brief copy the ID and ensure same upper case across xml
 * TODO: Implement a much better way to do this
 */
static
void
copy_current_id
    (char   *destination
    ,char   *source         /* must be terminated with NUL! */
    ,size_t  size
    )
{
    char *tmp = source;
    size_t i = 0;

    while (*tmp  && i < size)
    {
        *tmp = (char)toupper((int) *tmp);
        ++tmp;
        ++i;
    }
    if (destination != source)
    {
        if (i < size)
        {
            memset(destination, 0, size);
        }
        else if (i > size)
        {
            i = size;
        }
        memmove(destination, source, i);
    }
}


/* ------------------------ COROUTINE MACROS ------------------------- */
/* The dlb_xml library walks through the XML file and invokes its
 * element_callback whenever it discovers a new tag.  However, we also
 * want to write the recursive descent parser in a manner that reflects
 * the grammar - the shape of the XML we want to parse.  We use a
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

static
int
is_tag_present
    (tag_stack  *stack
    ,const char *tag
    )
{
    tagloc *top = tag_stack_top(stack);
    return !strcasecmp(tag, top->tag);
}


#define next_tag()           COROUTINE_RETURN(0)
#define open_tag(literal)    (is_open_tag(tag, text, literal))
#define closed_tag(literal)  (is_closed_tag(tag, text, literal))
#define error()              { report_error(context, tag, text); return 1; }
#define push_tag()           tag_stack_push(&p->tagstack, tag, p->lineno, NULL)
#define push_action_tag(a)   tag_stack_push(&p->tagstack, tag, p->lineno, a)
#define pop_tag()            if (tag_stack_pop(&p->tagstack, (parser*)p)) error()
#define check_tag_presence() (is_tag_present(&p->tagstack, tag))
    

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


#define if_ignore_optional_tag_block(name)      \
    if (open_tag(name))                         \
    {                                           \
        parser_trace(p, "ignoring body of <%s> tag\n", name); \
        p->ignore_attributes = 1;                             \
        push_tag();                                           \
        next_tag();                                           \
        while(!closed_tag(name))                              \
        {                                                     \
            parser_trace(p, "ignoring body of <%s> sub-tag\n", tag); \
            return 0;                                         \
        }                                                     \
        p->ignore_attributes = 0;                             \

#define end_ignore_optional_tag_block()         \
    }                                           \
    if (check_tag_presence())                   \
    {                                           \
        pop_tag();                              \
        next_tag();                             \
    }                                           \


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
    (dlb_sadm_reader *p  /**< [in] parser state */
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
 * @brief generic parse enumeration routine
 */
static
int                     /** @return -1 on failure, index of name in list on success */
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
 * @brief parse a generic boolean
 */
static
int
decode_bool
    (dlb_sadm_reader *p           /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,pmd_bool *b                  /**< [out] boolean */
    )
{
    static const char *ot[] = { "False", "True" };
    static const char *ot2[] = { "0", "1" };
    int res;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    res = decode_enum(text, ot, 2);
    if (res < 0)
    {
        res = decode_enum(text, ot2, 2);
        if (res < 0)
        {
            errmsg(p, "Error: unknown boolean \"%s\"", text);
            return 0;
        }
    }
    *b = (pmd_bool)res;
    return 1;
}


/**
 * @brief parse the string for an unsigned integer
 *
 * Note that this will not work for very large integers (i.e., those
 * with top bit set).
 */
static 
int                       /** @return 0 on failure, 1 on success */
decode_uint
    (dlb_sadm_reader *p   /**< [in] parser state */
    ,const char *text     /**< [in] text to parse */
    ,const char *name     /**< [in] name of thing being parsed */
    ,unsigned int min     /**< [in] min acceptable value */
    ,unsigned int max     /**< [in] max acceptable value */
    ,unsigned int *val    /**< [out] unsigned integer */
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
    if (v < (long long)min || v > (long long)max || endp == text)
    {
        errmsg(p, "Invalid %s: \"%s\"", name, text);
        return 0;
    }
    *val = (unsigned int)v;
    return 1;
}


static
int
decode_coordinate_type_attribute
    (dlb_sadm_reader *p            /**< [in] parser state */
    ,const char *text              /**< [in] text to parse */
    ,coordinate_type *ty           /**< [out] coordinate type */
    )
{
    int res = decode_enum(
        text, coordinate_type_names, sizeof(coordinate_type_names)/sizeof(coordinate_type_names[0]));

    (void)p;
    if (res >= 0)
    {
        *ty = (coordinate_type)(res + 1);
        return 1;
    }
    return 0;
}


/**
 * @brief parse an ISO 639-1 or ISO 639-2 language code
 */
static
int                               /** @return 0 on failure, 1 on success */
decode_language
    (dlb_sadm_reader *p           /**< [in] parser state */
    ,const char *text             /**< [in] text to parse */
    ,char *code                   /**< [out] language code */
    )
{
    pmd_langcode lc;

    if (NULL == text)
    {
        errmsg(p, "No language code specified");
        return 0;
    }
    else if (pmd_decode_langcode(text, &lc))
    {
        errmsg(p, "unknown language code \"%s\"", text);
        return 0;
    }
    memmove(code, text, 4);
    return 1;    
}


static
int                        /** @return 1 on success, 0 on failure */
decode_programme_label
    (dlb_sadm_reader *p
    ,const char *text
    )
{
    dlb_sadm_programme_label *label;

    if (p->programme.num_labels == p->limits.max_programme_labels)
    {
        errmsg(p, "Too many programme labels");
        return 0;
    }

    label = &p->programme.labels[p->programme.num_labels];
    if (!decode_string(p, text, label->name.data, sizeof(label->name.data)))
    {
        return 0;
    }
    memmove(label->language, p->current_language, sizeof(p->current_language));
    p->current_language[0] = '\0';
    p->programme.num_labels += 1;
    return 1;
}


static
int                        /** @return 1 on success, 0 on failure */
decode_content_label
    (dlb_sadm_reader *p
    ,const char *text
    )
{
    dlb_sadm_content_label *label;

    label = &p->content.label;
    if (!decode_string(p, text, label->name.data, sizeof(label->name.data)))
    {
        return 0;
    }
    memmove(label->language, p->current_language, sizeof(p->current_language));
    p->current_language[0] = '\0';

    return 1;
}


static
int
decode_idref
    (dlb_sadm_reader *p
    ,const char *text
    ,dlb_sadm_idref *idref
    ,dlb_sadm_idref_type type
    )
{
    dlb_sadm_id id;
    
    if (!decode_string(p, text, id.data, sizeof(id.data)))
    {
        return 0;
    }
    copy_current_id((char *)id.data, (char *)id.data, sizeof(id.data));
    
    if (dlb_sadm_lookup_reference(p->model, id.data, type, p->lineno, idref))
    {
        errmsg(p, "too many idrefs");
        return 0;
    }
    return 1;
}


static
int
decode_idref_entry
    (dlb_sadm_reader *p
    ,const char *text
    ,dlb_sadm_idref *idref
    ,dlb_sadm_idref_type type
    ,unsigned int max
    ,unsigned int *num
    )
{
    dlb_sadm_id id;
    
    if (!decode_string(p, text, id.data, sizeof(id.data)))
    {
        return 0;
    }
    copy_current_id((char *)id.data, (char *)id.data, sizeof(id.data));
    
    assert(num);
    if (*num >= max)
    {
        errmsg(p, "too many idrefs, max:%d", max);
        return 0;
    }
    
    if (dlb_sadm_lookup_reference(p->model, id.data, type, p->lineno, &idref[*num]))
    {
        errmsg(p, "too many idrefs, max:%d", max);
        return 0;
    }
    *num = *num + 1;
    return 1;
}


static
int
decode_cartesian
    (dlb_sadm_reader *p
    ,const char *text
    ,pmd_bool *b
    )
{
    int status = decode_bool(p, text, b);

    if (status)
    {
        p->blkfmt_state.got_cartesian = PMD_TRUE;
    }

    return status;
}


static
int
decode_position
    (dlb_sadm_reader *p
    ,const char *text
    ,dlb_sadm_block_format *blkfmt
    )
{
    char *endp;
    float f;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }
    else if (p->current_coordinate_type == COORD_NONE)
    {
        errmsg(p, "coordinate type attribute not set");
        return 0;
    }

    f = (float)strtod(text, &endp);
    switch (p->current_coordinate_type)
    {
    case COORD_X:
        if (f < -1.0f || f > 1.0f || endp == text)
        {
            errmsg(p, "Invalid X coordinate value: \"%s\"", text);
            return 0;
        }
        p->blkfmt_state.got_x = PMD_TRUE;
        blkfmt->azimuth_or_x = f;
        break;

    case COORD_Y:
        if (f < -1.0f || f > 1.0f || endp == text)
        {
            errmsg(p, "Invalid Y coordinate value: \"%s\"", text);
            return 0;
        }
        p->blkfmt_state.got_y = PMD_TRUE;
        blkfmt->elevation_or_y = f;
        break;

    case COORD_Z:
        if (f < -1.0f || f > 1.0f || endp == text)
        {
            errmsg(p, "Invalid Z coordinate value: \"%s\"", text);
            return 0;
        }
        p->blkfmt_state.got_z = PMD_TRUE;
        blkfmt->distance_or_z = f;
        break;

    case COORD_AZIMUTH:
        if (f <= -180.0f || f >= 180.0f || endp == text)
        {
            errmsg(p, "Invalid azimuth coordinate value: \"%s\"", text);
            return 0;
        }
        p->blkfmt_state.got_azimuth = PMD_TRUE;
        blkfmt->azimuth_or_x = f;
        break;

    case COORD_ELEVATION:
        if (f <= -90.0f || f >= 90.0f || endp == text)
        {
            errmsg(p, "Invalid elevation coordinate value: \"%s\"", text);
            return 0;
        }
        p->blkfmt_state.got_elevation = PMD_TRUE;
        blkfmt->elevation_or_y = f;
        break;

    case COORD_DISTANCE:
        if (f < 0.0f || f > 1.0f || endp == text)
        {
            errmsg(p, "Invalid distance coordinate value: \"%s\"", text);
            return 0;
        }
        p->blkfmt_state.got_distance = PMD_TRUE;
        blkfmt->distance_or_z = f;
        break;

    default:
        errmsg(p, "Invalid coordinate type value: %d", (int)p->current_coordinate_type);
        return 0;
    }

    p->current_coordinate_type = COORD_NONE;
    return 1;
}


/**
 * @brief helper function to check if text represents -inf
 */
static inline
pmd_bool
text_is_minf_db
    (const char *text
    )
{
    return !strcasecmp(text, "-999");
}


/**
 * @brief parse the string for a sADM gain value
 */
static
int
decode_gain
    (dlb_sadm_reader *p
    ,const char *text
    ,float *gain
    )
{
    char *endp;
    float gain_value;
    
    if (NULL == text)
    {
        errmsg(p, "Malformed tag");
        return 0;
    }

    gain_value = (float)strtod(text, &endp);
    if (!p->gain_unit_db)
    {
        /* Convert linear gain to dB */
        if (gain_value > 0)
        {
            gain_value = 20 * log10f(gain_value);
        }
        else if (gain_value == 0)
        {
            *gain = -INFINITY;
            return 1;
        }
        else
        {
            errmsg(p, "linear gain out of range %s\n", text);
            return 0;
        }
    }
    else
    {
        /* Process dB gain */
        if (text_is_minf_db(text))
        {
            *gain = -INFINITY;
            return 1;
        }
    }

    p->gain_unit_db = 0;    /* Reset to default */
    
    if (gain_value < -25.0f || gain_value > 6.0f || endp == text)
    {
        errmsg(p, "Invalid gain: \"%s\"",  text);
        return 0;
    }
    
    *gain = gain_value;
    return 1;
}


static
int
decode_dialogue_value
    (dlb_sadm_reader *p
    ,const char *text
    ,unsigned int *dialogue_value
    )
{
    if (!decode_uint(p, text, "dialogue", 0, 2, dialogue_value))
    {
        errmsg(p, "Error: unknown dialogue value \"%s\"\n", text);
        return 0;
    }
    return 1;
}


/** -------------------------- PARSER XML simple tags ------------------------------ */


#define parse_idref(id, ty)           if (!decode_idref(p, text, &id, ty)) error()
#define parse_idref_entry(a, ty)      if (!decode_idref_entry(p, text, a.array, ty, a.max, &a.num)) error()
#define parse_progref(id)             parse_idref(id, DLB_SADM_PROGRAMME)
#define parse_trackref(id)            parse_idref(id, DLB_SADM_TRACKUID)
#define parse_packref(id)             parse_idref(id, DLB_SADM_PACKFMT)
#define parse_chanref(id)             parse_idref(id, DLB_SADM_CHANFMT)
#define parse_objref_entry(arr)       parse_idref_entry(arr, DLB_SADM_OBJECT)
#define parse_packref_entry(arr)      parse_idref_entry(arr, DLB_SADM_PACKFMT)
#define parse_trackref_entry(arr)     parse_idref_entry(arr, DLB_SADM_TRACKUID)
#define parse_chanref_entry(arr)      parse_idref_entry(arr, DLB_SADM_CHANFMT)
#define parse_contentref_entry(arr)   parse_idref_entry(arr, DLB_SADM_CONTENT)

#define parse_programme_label()       if (!decode_programme_label(p, text)) error()
#define parse_content_label()         if (!decode_content_label(p, text)) error()
#define parse_blkfmt_speaker_label(l) if (!decode_string(p, text, l, sizeof(l))) error()
#define parse_blkfmt_gain(g)          if (!decode_gain(p, text, &g)) error()
#define parse_blkfmt_cartesian(c)     if (!decode_cartesian(p, text, &c)) error()
#define parse_blkfmt_position(blkfmt) if (!decode_position(p, text, &blkfmt)) error()
#define parse_blkfmt_width(blkfmt)
#define parse_blkfmt_height(blkfmt)
#define parse_blkfmt_depth(blkfmt)

#define parse_dialogue(d)             if (!decode_dialogue_value(p, text, &d)) error()
#define parse_object_gain(g)          if (!decode_gain(p, text, &g)) error()


/** -------------------------- PARSER TAG ACTIONS ------------------------------ */


/**
 * @brief prepare a new audio program definition for population while parsing audioProgramme tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_programme
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;
    
    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_PROGRAMME, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->programme, '\0', sizeof(p->programme));
        
        /* assign values based on previously parsed attributes */
        memmove(p->programme.id.data,     p->current_id, sizeof(p->current_id));
        memmove(p->programme.name.data,   p->current_name, sizeof(p->current_name));
        p->programme.contents.num = 0;
        p->programme.contents.max = (unsigned int)p->limits.max_programme_contents;
        p->programme.contents.array = p->programme_contents;
        p->programme.labels = p->programme_labels;
        memmove(p->programme.language, p->current_language, sizeof(p->current_language));
        res = PMD_SUCCESS;
    }
    else
    {
        errmsg(p, "programme \"%s\" already defined\n", (char*)p->current_id);
    }
    
    p->current_language[0] = '\0';
    p->current_id[0] = 0;
    p->current_name[0] = 0;
    return res;
}


/**
 * @def alloc_programme(p)
 * @brief call new_programme(p) and report error if any
 */
#define alloc_programme(p) if (new_programme(p)) error()


/**
 * @brief attempt to add a newly constructed audio programme definition to the sADM model
 */
static
dlb_pmd_success
publish_programme
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;
    
    if (dlb_sadm_set_programme(p->model, &p->programme, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a new audio content definition for population while parsing audioContent tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_content
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;
    
    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_CONTENT, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->content, '\0', sizeof(p->content));    
        /* assign values based on previously parsed attributes */
        memmove(p->content.id.data,   p->current_id, sizeof(p->current_id));
        memmove(p->content.name.data, p->current_name, sizeof(p->current_name));
        p->content.type = DLB_SADM_CONTENT_UNSET;
        p->content.objects.num = 0;
        p->content.objects.max = (unsigned int)p->limits.max_content_objects;
        p->content.objects.array = p->content_objects;
        res = PMD_SUCCESS;
    }
    else
    {
        errmsg(p, "content \"%s\" already defined\n", (char*)p->current_id);
    }
    p->current_id[0] = 0;
    p->current_name[0] = 0;
    p->current_language[0] = 0;
    return res;
}


/**
 * @def alloc_content(p)
 * @brief call new_content(p) and report error if any
 */
#define alloc_content(p) if (new_content(p)) error()


/**
 * @brief attempt to add a newly constructed audio content definition to the sADM model
 */
static
dlb_pmd_success
publish_content
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;

    if (dlb_sadm_set_content(p->model, &p->content, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));        
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a new audio object definition for population while parsing audioObject tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_object
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;

    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_OBJECT, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->object, '\0', sizeof(p->object));    
        
        /* assign values based on previously parsed attributes */
        memmove(p->object.id.data,         p->current_id, sizeof(p->current_id));
        memmove(p->object.name.data, p->current_name, sizeof(p->current_name));

        p->object.object_refs.num = 0;
        p->object.object_refs.max = (unsigned int)p->limits.max_object_objects;
        p->object.object_refs.array = p->object_objects;
        
        p->object.track_uids.num = 0;
        p->object.track_uids.max = (unsigned int)p->limits.max_object_track_uids;
        p->object.track_uids.array = p->object_track_uids;
        res = PMD_SUCCESS;        
    }
    else
    {
        errmsg(p, "object \"%s\" has already been defined\n", (char*)p->current_id);        
    }
    
    p->current_id[0] = 0;
    p->current_name[0] = 0;
    return res;
}


/**
 * @def alloc_object(p)
 * @brief call new_object(p) and report error if any
 */
#define alloc_object(p) if (new_object(p)) error()


/**
 * @brief attempt to add a newly constructed audio object definition to the sADM model
 */
static
dlb_pmd_success
publish_object
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;
    if (dlb_sadm_set_object(p->model, &p->object, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));        
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a new audio pack format definition for population while parsing audioPackFormat
 * tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_packfmt
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;
    int type_label;
    char *endp;

    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_PACKFMT, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->packfmt, '\0', sizeof(p->packfmt));    
        
        /* assign values based on previously parsed attributes */
        memmove(p->packfmt.id.data,    p->current_id, sizeof(p->current_id));
        memmove(p->packfmt.name.data,  p->current_name, sizeof(p->current_name));
        
        p->packfmt.chanfmts.num = 0;
        p->packfmt.chanfmts.max = (unsigned int)p->limits.max_packfmt_chanfmts;
        p->packfmt.chanfmts.array = p->packfmt_chanfmts;

        type_label = strtol((char*)p->type_label, &endp, 0);
        if (endp == (char*)p->type_label
            || (type_label != 1 && type_label != 3)
            )
        {
            errmsg(p, "audioPackFormat type label \"%s\" not recognised at line\n", p->type_label);
        }
        else if (  p->type_definition[0] != 0
                && (  (type_label == 1 && strcasecmp((char*)p->type_definition, "DirectSpeakers"))
                   || (type_label == 3 && strcasecmp((char*)p->type_definition, "Objects"))
                   )
                )
        {
            errmsg(p, "audioPackFormat type label \"%s\" and type definition \"%s\" do not agree\n",
                   p->type_label, p->type_definition);
        }
        else
        {
            p->packfmt.type = (dlb_sadm_packfmt_type)type_label;
            res = PMD_SUCCESS;
        }
    }
    else
    {
        errmsg(p, "pack format \"%s\" has already been defined\n", p->current_id);
    }
    
    p->current_id[0] = 0;
    p->current_name[0] = 0;
    p->type_definition[0] = 0;
    p->type_label[0] = 0;
    return res;    
}


/**
 * @def alloc_packfmt(p)
 * @brief call new_packfmt(p) and report error if any
 */
#define alloc_packfmt(p) if (new_packfmt(p)) error()


/**
 * @brief attempt to add a newly constructed audio pack format definition to the sADM model
 */
static
dlb_pmd_success
publish_packfmt
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;
    if (dlb_sadm_set_pack_format(p->model, &p->packfmt, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a new audio channel format definition for population while parsing
 * audioChannelFormat tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_chanfmt
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;
    dlb_pmd_bool got_type_label;
    dlb_pmd_bool got_type_definition;
    dlb_pmd_bool good_type_label = PMD_TRUE;
    dlb_pmd_bool good_type_definition = PMD_TRUE;
    int type_label = 0;
    int type_definition = 0;
    char *endp;

    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_CHANFMT, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->chanfmt, '\0', sizeof(p->chanfmt));
        /* assign values based on previously parsed attributes */
        memmove(p->chanfmt.id.data, p->current_id, sizeof(p->current_id));
        memmove(p->chanfmt.name.data, p->current_name, sizeof(p->current_name));
        p->chanfmt.blkfmts.num = 0;
        p->chanfmt.blkfmts.max = (unsigned int)p->limits.max_chanfmt_blkfmts;
        p->chanfmt.blkfmts.array = p->chanfmt_blkfmts;

        type_label = strtol((char*)p->type_label, &endp, 0);
        got_type_label = (endp != (char*)p->type_label);
        if (got_type_label
            && (type_label != 1 && type_label != 3)
            )
        {
            errmsg(p, "audioChannelFormat type label \"%s\" not recognised at line %u\n", p->type_label, p->lineno);
            good_type_label = PMD_FALSE;
        }

        got_type_definition = (p->type_definition[0] != 0);
        if (got_type_definition)
        {
            if (!strcasecmp((char*)p->type_definition, "DirectSpeakers"))
            {
                type_definition = 1;
            }
            else if (!strcasecmp((char*)p->type_definition, "Objects"))
            {
                type_definition = 3;
            } 
            else
            {
                errmsg(p, "audioChannelFormat type definition \"%s\" not recognised at line %u\n", (char *)p->type_definition, p->lineno);
                good_type_label = PMD_FALSE;
            }
        }

        if (got_type_label && good_type_label && got_type_definition && good_type_definition
              && type_definition != type_label
           )
        {
            errmsg(p, "audioChannelFormat type label \"%s\" and type definition \"%s\" do not agree at line %u\n",
                   p->type_label, (char *)p->type_definition, p->lineno);
        }

        res = PMD_SUCCESS;
    }
    else
    {
        errmsg(p, "channel format \"%s\" has already been defined\n", p->current_id);
    }
    
    p->current_id[0] = 0;
    p->current_name[0] = 0;
    p->type_definition[0] = 0;
    p->type_label[0] = 0;
    return res;
}


/**
 * @def alloc_chanfmt(p)
 * @brief call new_chanfmt(p) and report error if any
 */
#define alloc_chanfmt(p) if (new_chanfmt(p)) error()


/**
 * @brief attempt to add a newly constructed audio channel format definition to the sADM model
 */
static
dlb_pmd_success
publish_chanfmt
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;

    if (dlb_sadm_set_channel_format(p->model, &p->chanfmt, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));
        return 1;
    }
    return 0;
}


/**
 * @brief prepare a new audio block format definition for population while parsing
 * audioBlockFormat tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_blkfmt
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;

    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_BLOCKFMT, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->blkfmt, '\0', sizeof(p->blkfmt));
        /* assign values based on previously parsed attributes */
        memmove(p->blkfmt.id.data, p->current_id, sizeof(p->current_id));
        res = PMD_SUCCESS;
    }
    else
    {
        errmsg(p, "block format \"%s\" has already been defined\n", p->current_id);
    }

    memset(&p->blkfmt_state, 0, sizeof(p->blkfmt_state));
    p->current_id[0] = 0;
    return res;
}


/**
 * @def alloc_blkfmt(p)
 * @brief call new_blkfmt(p) and report error if any
 */
#define alloc_blkfmt(p) if (new_blkfmt(p)) error()


/**
 * @brief attempt to add a newly constructed audio block format definition to the sADM model
 */
static
dlb_pmd_success
publish_blkfmt
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;
    block_format_state *attr = &p->blkfmt_state;
    dlb_sadm_idref idref;

    if (attr->got_x || attr->got_y || attr->got_z || p->blkfmt.cartesian_coordinates)
    {
        if (!attr->got_x || !attr->got_y || !attr->got_z || !attr->got_cartesian || !p->blkfmt.cartesian_coordinates)
        {
            errmsg(p, "Inconsistent or missing cartesian coordinates for block format \"%s\"",
                   p->blkfmt.id.data);
            return 1;
        }
    }
    else if (attr->got_azimuth || attr->got_elevation || attr->got_distance)
    {
        if (!attr->got_azimuth || !attr->got_elevation || !attr->got_distance /*|| p->blkfmt.cartesian_coordinates*/)
        {
            errmsg(p, "Inconsistent or missing spherical coordinates for block format \"%s\"",
                   p->blkfmt.id.data);
            return 1;
        }
    } 
    else
    {
        errmsg(p, "Missing coordinates for block format \"%s\"", p->blkfmt.id.data);
        return 1;
    }

    if (dlb_sadm_set_block_format(p->model, &p->blkfmt, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));
        return 1;
    }
    /* now add to the containing chanfmt */
    if (p->chanfmt.blkfmts.num >= p->chanfmt.blkfmts.max)
    {
        errmsg(p, "Too many block formats in channel format \"%s\"",
               p->chanfmt.id.data);
        return 1;
    }
    
    if (dlb_sadm_lookup_reference(p->model, p->blkfmt.id.data,
                                  DLB_SADM_BLOCKFMT, p->lineno, &idref))
    {
        errmsg(p, "could not find reference \"%s\"", p->blkfmt.id.data);
        return 1;
    }
    p->chanfmt.blkfmts.array[p->chanfmt.blkfmts.num] = idref;
    p->chanfmt.blkfmts.num += 1;
    return 0;
}


/**
 * @brief prepare a new audio stream format definition for population while parsing
 * audioStreamFormat tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_streamfmt
    (dlb_sadm_reader *p
    )
{
    /* TODO: do we need stream format sometimes, and not others? */

    // dlb_sadm_idref idref;
    // dlb_pmd_success res = PMD_FAIL;

    // if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_STREAMFMT, 0, &idref)
    //     || dlb_sadm_idref_defined(idref))
    // {
    //     memset(&p->streamfmt, '\0', sizeof(p->streamfmt));
    //     /* assign values based on previously parsed attributes */
    //     memmove(p->streamfmt.id.data, p->current_id, sizeof(p->current_id));
    //     res = PMD_SUCCESS;
    // }
    // else
    // {
    //     errmsg(p, "stream format \"%s\" has already been defined\n", p->current_id);
    // }
        
    // p->current_id[0] = 0;
    // return res;
    (void)p;
    return PMD_SUCCESS; 
}


/**
 * @def alloc_streamfmt(p)
 * @brief call new_streamfmt(p) and report error if any
 */
#define alloc_streamfmt(p) if (new_streamfmt(p)) error()


/**
 * @brief attempt to add a newly constructed audio block format definition to the sADM model
 */
static
dlb_pmd_success
publish_streamfmt
    (parser *ctx
    )
{
    //TODO: implement correctly
    (void)ctx;
    return PMD_SUCCESS;
}


/**
 * @brief prepare a new audio track format definition for population while parsing
 * audioTrackFormat tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_trackfmt
    (dlb_sadm_reader *p
    )
{
    /* TODO: do we need track format sometimes, and not others? */

    // dlb_sadm_idref idref;
    // dlb_pmd_success res = PMD_FAIL;

    // if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_TRACKFMT, 0, &idref)
    //     || dlb_sadm_idref_defined(idref))
    // {
    //     memset(&p->trackfmt, '\0', sizeof(p->trackfmt));
    //     /* assign values based on previously parsed attributes */
    //     memmove(p->trackfmt.id.data, p->current_id, sizeof(p->current_id));
    //     res = PMD_SUCCESS;
    // }
    // else
    // {
    //     errmsg(p, "block format \"%s\" has already been defined\n", p->current_id);
    // }
        
    // p->current_id[0] = 0;
    // return res;
    (void)p;
    return PMD_SUCCESS;
}


/**
 * @def alloc_trackfmt(p)
 * @brief call new_trackfmt(p) and report error if any
 */
#define alloc_trackfmt(p) if (new_trackfmt(p)) error()


/**
 * @brief attempt to add a newly constructed audio block format definition to the sADM model
 */
static
dlb_pmd_success
publish_trackfmt
    (parser *ctx
    )
{
    //TODO: implement correctly
    (void)ctx;
    return PMD_SUCCESS;
}


/**
 * @brief prepare a new track UID definition for population while parsing
 * audioTrackUID tag
 */
static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
new_track_uid
    (dlb_sadm_reader *p
    )
{
    dlb_sadm_idref idref;
    dlb_pmd_success res = PMD_FAIL;

    if (   dlb_sadm_lookup_reference(p->model, p->current_id, DLB_SADM_TRACKUID, 0, &idref)
        || dlb_sadm_idref_defined(idref))
    {
        memset(&p->track_uid, '\0', sizeof(p->track_uid));
        /* assign values based on previously parsed attributes */
        memmove(p->track_uid.id.data, p->current_id, sizeof(p->current_id));
        res = PMD_SUCCESS;
    }
    else
    {
        errmsg(p, "Track UID \"%s\" has already been defined\n", p->current_id);
    }

    p->current_id[0] = 0;
    return res;
}


/**
 * @def alloc_track_uid(p)
 * @brief call new_track_uid(p) and report error if any
 */
#define alloc_track_uid(p) if (new_track_uid(p)) error()


/**
 * @brief attempt to add a newly constructed track uid definition to the sADM model
 */
static
dlb_pmd_success
publish_track_uid
    (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;
    p->track_uid.channel_idx = 0;
    if (dlb_sadm_set_track_uid(p->model, &p->track_uid, NULL))
    {
        errmsg(p, dlb_sadm_error(p->model));
        return 1;
    }
    return 0;
}


static
dlb_pmd_success
clear_current_track_id
 (parser *ctx
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)ctx;

    p->current_track_id = 0;
    return 0;
}


/**
 * @def clear_track_id(p)
 * @brief call clear_current_track_id(p) and report error if any
 */
#define clear_track_id(p) if (clear_current_track_id(p)) error()


/**
 * @brief attempt to associate a channel number with a given track UID
 */
static
dlb_pmd_success
set_track_uid_channel
    (dlb_sadm_reader *p    /**< [in] parser state */
    ,const char *ref       /**< [in[ Track UID reference newly parsed */
    )
{
    dlb_sadm_track_uid track_uid;
    dlb_sadm_idref idref;
    dlb_sadm_idref idref2;
    char up_buf[13];

    strncpy(up_buf, ref, sizeof(up_buf));
    copy_current_id(up_buf, up_buf, sizeof(up_buf));
    
    if (0 == p->current_track_id)
    {
        errmsg(p, "No trackID attribute specified for audio Track UID Ref");
        return PMD_FAIL;
    }

    if (dlb_sadm_lookup_reference(p->model, (const unsigned char *)up_buf, DLB_SADM_TRACKUID, 0, &idref))
    {
        errmsg(p, "Could not look up track uid \"%s\"", ref);
        return PMD_FAIL;
    }

    if (dlb_sadm_track_uid_lookup(p->model, idref, &track_uid))
    {
        track_uid_to_channel_record *r;

        if (p->channel_assignment_count >= p->limits.num_track_uids)
        {
            errmsg(p, "Too many channel assignments -- could not record track number for track uid \"%s\"", ref);
            return PMD_FAIL;
        }
        r = &p->channel_assignments[p->channel_assignment_count++];
        memmove(&r->track_uid, up_buf, sizeof(r->track_uid));
        r->channel_number = p->current_track_id;
    }
    else
    {
        track_uid.channel_idx = p->current_track_id;

        if (dlb_sadm_set_track_uid(p->model, &track_uid, &idref2))
        {
            return PMD_FAIL;
        }

        if (idref != idref2)
        {
            errmsg(p, "failed to update track UID correctly");
            return PMD_FAIL;
        }
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
publish_nop
    (parser *ctx
    )
{
    (void)ctx;
    return PMD_SUCCESS;
}

static
dlb_pmd_success           /** @return 1 on failure, 0 on success */
alloc_nop
    (dlb_sadm_reader *p
    )
{
    (void)p;
    return PMD_SUCCESS;
}

/**
 * @def process_nop()
 * @brief wrap nop operation
 */
#define process_nop(p) if (alloc_nop(p)) error()

/** -------------------------- PARSER XML LAYOUT ------------------------------ */


/**
 * @brief main XML parser coroutine
 */
static
int
element_callback
    (void *context
    ,char *tag
    ,char *text
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)context;

    COROUTINE_BEGIN;

    if_begin_tag("frame",)
    {
        if_begin_tag("frameHeader",)
        {
            if_begin_tag("frameFormat",)
            {
                if_tag("chunkAdmElement") process_nop(p);
                elif_begin_tag("changedIDs",)
                {
                    if_tag("audioProgrammeIDRef");
                    elif_tag("audioContentIDRef");
                    elif_tag("audioObjectIDRef");
                    elif_tag("audioPackFormatIDRef");
                    elif_tag("audioTrackUIDRef");
                    elif_tag("audioChannelFormatIDRef");
                    elif_tag("audioTrackFormatIDRef");
                    endif();
                }
                elif_begin_tag("transportTrackFormat",)     /* PMDLIB-108: keeping this for backwards compatibility */
                {
                    if_begin_popaction_tag("audioTrack", NULL, clear_current_track_id)
                    {
                        if_tag("audioTrackUIDRef") set_track_uid_channel(p, text);
                        endif();
                    }
                    endif();
                }
                endif();
            }
            elif_begin_tag("transportTrackFormat",)         /* PMDLIB-108: correct placement is here */
            {
                if_begin_popaction_tag("audioTrack", NULL, clear_current_track_id)
                {
                    if_tag("audioTrackUIDRef") set_track_uid_channel(p, text);
                    endif();
                }
                endif();
            }
            endif();
        }
        /* TODO: ebuCoreMain? */
        elif_begin_tag("coreMetadata",)
        {
            if_begin_tag("format",)
            {
                if_begin_tag("audioFormatExtended",)       /* Optional wrapping by coreMetadata and format */
                {
                    if_begin_popaction_tag("audioProgramme", alloc_programme(p), publish_programme)
                    {
                        if_tag("audioProgrammeLabel") parse_programme_label();
                        elif_tag("audioContentIDRef") parse_contentref_entry(p->programme.contents);
                        elif_begin_popaction_tag("loudnessMetadata", process_nop(p), publish_nop)
                        {
                            if_tag("integratedLoudness") process_nop(p);
                            elif_tag("loudnessRange")    process_nop(p);
                            elif_tag("maxTruePeak")      process_nop(p);
                            elif_tag("maxMomentary")     process_nop(p);
                            elif_tag("maxShortTerm")     process_nop(p);
                            elif_tag("dialogueLoudness") process_nop(p);
                            endif();
                        }
                        elif_begin_popaction_tag("authoringInformation", process_nop(p), publish_nop)
                        {
                            if_begin_popaction_tag("referenceLayout", process_nop(p), publish_nop)
                            {
                                if_tag("audioPackFormatIDRef") process_nop(p);
                                endif();
                            }
                            elif_begin_popaction_tag("renderer", process_nop(p), publish_nop)
                            {
                                if_tag("audioPackFormatIDRef") process_nop(p);
                                endif();
                            }
                            endif();
                        }
                        endif();
                    }
                    elif_begin_popaction_tag("audioContent", alloc_content(p), publish_content)
                    {
                        if_tag("audioObjectIDRef")    parse_objref_entry(p->content.objects);
                        elif_tag("dialogue")          parse_dialogue(p->content.dialogue_value);
                        elif_tag("audioContentLabel") parse_content_label();
                        elif_begin_popaction_tag("loudnessMetadata", process_nop(p), publish_nop)
                        {
                            if_tag("integratedLoudness") process_nop(p);
                            elif_tag("loudnessRange")    process_nop(p);
                            elif_tag("maxTruePeak")      process_nop(p);
                            elif_tag("maxMomentary")     process_nop(p);
                            elif_tag("maxShortTerm")     process_nop(p);
                            elif_tag("dialogueLoudness") process_nop(p);
                            endif();
                        }
                        endif();
                    }
                    elif_begin_popaction_tag("audioObject", alloc_object(p), publish_object)
                    {
                        if_tag("gain")                   parse_object_gain(p->object.gain);
                        elif_tag("audioPackFormatIDRef") parse_packref(p->object.pack_format);
                        elif_tag("audioTrackUIDRef")     parse_trackref_entry(p->object.track_uids);
                        elif_tag("audioObjectIDRef")     parse_objref_entry(p->object.object_refs);
                        elif_begin_popaction_tag("audioObjectInteraction", process_nop(p), publish_nop)
                        {
                            if_tag("gainInteractionRange") process_nop(p);
                            endif();
                        }
                        endif();
                    }
                    elif_begin_popaction_tag("audioPackFormat", alloc_packfmt(p), publish_packfmt)
                    {
                        if_tag("audioChannelFormatIDRef") parse_chanref_entry(p->packfmt.chanfmts);
                        endif();
                    }
                    elif_begin_popaction_tag("audioChannelFormat", alloc_chanfmt(p), publish_chanfmt)
                    {
                        if_begin_popaction_tag("audioBlockFormat", alloc_blkfmt(p), publish_blkfmt)
                        {
                            if_tag("speakerLabel")    parse_blkfmt_speaker_label(p->blkfmt.speaker_label);
                            elif_tag("gain")          parse_blkfmt_gain(p->blkfmt.gain);
                            elif_tag("cartesian")     parse_blkfmt_cartesian(p->blkfmt.cartesian_coordinates);
                            elif_tag("position")      parse_blkfmt_position(p->blkfmt);
                            elif_tag("width")         parse_blkfmt_width(p->blkfmt);
                            elif_tag("height")        parse_blkfmt_height(p->blkfmt);
                            elif_tag("depth")         parse_blkfmt_depth(p->blkfmt);
                            endif();
                        }
                        endif();
                    }
                    elif_begin_popaction_tag("audioStreamFormat", alloc_streamfmt(p), publish_streamfmt)
                    {
                        if_tag("audioChannelFormatIDRef");
                        elif_tag("audioTrackFormatIDRef");
                        endif();
                    }
                    elif_begin_popaction_tag("audioTrackFormat", alloc_trackfmt(p), publish_trackfmt)
                    {
                        if_tag("audioStreamFormatIDRef");
                        endif();
                    }
                    elif_begin_popaction_tag("audioTrackUID", alloc_track_uid(p), publish_track_uid)
                    {
                        if_tag("audioChannelFormatIDRef")  parse_chanref(p->track_uid.chanfmt);
                        elif_tag("audioPackFormatIDRef")   parse_packref(p->track_uid.packfmt);
                        elif_tag("audioTrackFormatIDRef");
                        endif();
                    }
                    endif();
                }
                endif();
#ifdef IGNORE_AUDIO_FORMAT_CUSTOM_ELEMENT
                if_ignore_optional_tag_block("audioFormatCustom")
                end_ignore_optional_tag_block();
#else
                elif_begin_tag("audioFormatCustom", )
                {
                    if_begin_tag("audioFormatCustomSet", )
                    {
                        if_begin_tag("admInformation", )
                        {
                            if_begin_tag("profile", )
                            {
                                if_tag("levelID");
                                elif_tag("profileVersion");
                                elif_tag("profileName");
                                endif();
                            }
                            endif();
                        }
                        endif();
                    }
                    endif();
                }
                endif();
#endif
            }
            endif();
        }
        elif_begin_tag("audioFormatExtended",)             /* Non-wrapped version */
        {
            if_begin_popaction_tag("audioProgramme", alloc_programme(p), publish_programme)
            {
                if_tag("audioProgrammeLabel") parse_programme_label();
                elif_tag("audioContentIDRef") parse_contentref_entry(p->programme.contents);
                elif_begin_popaction_tag("loudnessMetadata", process_nop(p), publish_nop)
                {
                    if_tag("integratedLoudness") process_nop(p);
                    elif_tag("loudnessRange") process_nop(p);
                    elif_tag("maxTruePeak") process_nop(p);
                    elif_tag("maxMomentary") process_nop(p);
                    elif_tag("maxShortTerm") process_nop(p);
                    elif_tag("dialogueLoudness") process_nop(p);
                    endif();
                }
                elif_begin_popaction_tag("authoringInformation", process_nop(p), publish_nop)
                {
                    if_begin_popaction_tag("referenceLayout", process_nop(p), publish_nop)
                    {
                        if_tag("audioPackFormatIDRef") process_nop(p);
                        endif();
                    }
                    elif_begin_popaction_tag("renderer", process_nop(p), publish_nop)
                    {
                        if_tag("audioPackFormatIDRef") process_nop(p);
                        endif();
                    }
                    endif();
                }
                endif();
            }
            elif_begin_popaction_tag("audioContent", alloc_content(p), publish_content)
            {
                if_tag("audioObjectIDRef")    parse_objref_entry(p->content.objects);
                elif_tag("dialogue")          parse_dialogue(p->content.dialogue_value);
                elif_tag("audioContentLabel") parse_content_label();
                elif_begin_popaction_tag("loudnessMetadata", process_nop(p), publish_nop)
                {
                    if_tag("integratedLoudness") process_nop(p);
                    elif_tag("loudnessRange")    process_nop(p);
                    elif_tag("maxTruePeak")      process_nop(p);
                    elif_tag("maxMomentary")     process_nop(p);
                    elif_tag("maxShortTerm")     process_nop(p);
                    elif_tag("dialogueLoudness") process_nop(p);
                    endif();
                }
                endif();
            }
            elif_begin_popaction_tag("audioObject", alloc_object(p), publish_object)
            {
                if_tag("gain")                   parse_object_gain(p->object.gain);
                elif_tag("audioPackFormatIDRef") parse_packref(p->object.pack_format);
                elif_tag("audioTrackUIDRef")     parse_trackref_entry(p->object.track_uids);
                elif_tag("audioObjectIDRef")     parse_objref_entry(p->object.object_refs);
                elif_begin_popaction_tag("audioObjectInteraction", process_nop(p), publish_nop)
                {
                    if_tag("gainInteractionRange") process_nop(p);
                    endif();
                }
                endif();
            }
            elif_begin_popaction_tag("audioPackFormat", alloc_packfmt(p), publish_packfmt)
            {
                if_tag("audioChannelFormatIDRef") parse_chanref_entry(p->packfmt.chanfmts);
                endif();
            }
            elif_begin_popaction_tag("audioChannelFormat", alloc_chanfmt(p), publish_chanfmt)
            {
                if_begin_popaction_tag("audioBlockFormat", alloc_blkfmt(p), publish_blkfmt)
                {
                    if_tag("speakerLabel")    parse_blkfmt_speaker_label(p->blkfmt.speaker_label);
                    elif_tag("gain")          parse_blkfmt_gain(p->blkfmt.gain);
                    elif_tag("cartesian")     parse_blkfmt_cartesian(p->blkfmt.cartesian_coordinates);
                    elif_tag("position")      parse_blkfmt_position(p->blkfmt);
                    elif_tag("width")         parse_blkfmt_width(p->blkfmt);
                    elif_tag("height")        parse_blkfmt_height(p->blkfmt);
                    elif_tag("depth")         parse_blkfmt_depth(p->blkfmt);
                    endif();
                }
                endif();
            }
            elif_begin_popaction_tag("audioStreamFormat", alloc_streamfmt(p), publish_streamfmt)
            {
                if_tag("audioChannelFormatIDRef");
                elif_tag("audioTrackFormatIDRef");
                endif();
            }
            elif_begin_popaction_tag("audioTrackFormat", alloc_trackfmt(p), publish_trackfmt)
            {
                if_tag("audioStreamFormatIDRef");
                endif();
            }
            elif_begin_popaction_tag("audioTrackUID", alloc_track_uid(p), publish_track_uid)
            {
                if_tag("audioChannelFormatIDRef")  parse_chanref(p->track_uid.chanfmt);
                elif_tag("audioPackFormatIDRef")   parse_packref(p->track_uid.packfmt);
                elif_tag("audioTrackFormatIDRef");
                endif();
            }
            endif();
        }
        endif();
#ifdef IGNORE_AUDIO_FORMAT_CUSTOM_ELEMENT
        if_ignore_optional_tag_block("audioFormatCustom")
        end_ignore_optional_tag_block();
#else
        elif_begin_tag("audioFormatCustom",)
        {
            if_begin_tag("audioFormatCustomSet",)
            {
                if_begin_tag("admInformation",)
                {
                    if_begin_tag("profile", )
                    {
                        if_tag("levelID");
                        elif_tag("profileVersion");
                        elif_tag("profileName");
                        endif();
                    }
                    endif();
                }
                endif();
            }
            endif();
        }
        endif();
#endif
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
    dlb_sadm_reader *p = (dlb_sadm_reader *)context;
    /*######################### ADM elements #####################################*/
    if (p->ignore_attributes == 1)
    {
        parser_trace(p, "ignoring attribute %s (with value %s) of tag <%s>\n", attribute, value, tag);
        return 0;
    }

    /************************** audioTrackFormat **********************************/
    if (!strcasecmp(tag, "audioTrackFormat"))
    {
        if (!strcasecmp(attribute, "audioTrackFormatID"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "audioTrackFormatName"))
        {
            return 0;
        }
        /* formatLabel - handled by common attribute */
        /* formatDefinition - handled by common attribute */
    }
    /************************** audioStreamFormat *********************************/
    else if (!strcasecmp(tag, "audioStreamFormat"))
    {
        if (!strcasecmp(attribute, "audioStreamFormatID"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "audioStreamFormatName"))
        {
            return 0;
        }
        /* formatLabel - handled by common attribute */
        /* formatDefinition - handled by common attribute */
    }
    /************************** audioChannelFormat **********************************/
    else if (!strcasecmp(tag, "audioChannelFormat"))
    {
        if (!strcasecmp(attribute, "audioChannelFormatName"))
        {
            if (0 != p->current_name[0])
            {
                errmsg(p, "multiple names specified on tag \"%s\"\n", tag);
                return 1;
            }
            if (!decode_string(p, value, p->current_name, sizeof(p->current_name)))
            {
                errmsg(p, "could not decode string value \"%s\" for attribute \"%s\" on tag \"%s\"\n",
                    value, attribute, tag);
                return 1;
            }

            return 0;
        }
        else if (!strcasecmp(attribute, "audioChannelFormatID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple IDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;
        }
        /* typeLabel - handled by common attribute */
        /* typeDefinition - handled by common attribute */
    }
    /************************** audioBlockFormat **********************************/
    else if (!strcasecmp(tag, "audioBlockFormat"))
    {
        if (!strcasecmp(attribute, "audioBlockFormatID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple IDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;
        }
        else if (!strcasecmp(attribute, "rtime"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "duration"))
        {
            return 0;
        }
        /* BS.2125 elements... */
        else if (!strcasecmp(attribute, "initializeBlock"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "lstart"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "lduration"))
        {
            return 0;
        }
        /* ... BS.2125 elements */
    }
    /************************** audioPackFormat ***********************************/
    else if (!strcasecmp(tag, "audioPackFormat"))
    {
        if (!strcasecmp(attribute, "audioPackFormatID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple IDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;
        }
        else if (!strcasecmp(attribute, "audioPackFormatName"))
        {
            if (0 != p->current_name[0])
            {
                errmsg(p, "multiple names specified on tag \"%s\"\n", tag);
                return 1;
            }
            if (!decode_string(p, value, p->current_name, sizeof(p->current_name)))
            {
                errmsg(p, "could not decode string value \"%s\" for attribute \"%s\" on tag \"%s\"\n",
                    value, attribute, tag);
                return 1;
            }
            return 0;
        }
        /* typeLabel - handled by common attribute */
        /* typeDefinition - handled by common attribute */
        /* importance - handled by common attribute */
    }
        /************************** audioObject ***********************************/
    else if (!strcasecmp(tag, "audioObject"))
    {
        if (!strcasecmp(attribute, "audioObjectID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple IDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;
        }
        else if (!strcasecmp(attribute, "audioObjectName"))
        {
            if (0 != p->current_name[0])
            {
                errmsg(p, "multiple names specified on tag \"%s\"\n", tag);
                return 1;
            }
            if (!decode_string(p, value, p->current_name, sizeof(p->current_name)))
            {
                errmsg(p, "could not decode string value \"%s\" for attribute \"%s\" on tag \"%s\"\n",
                    value, attribute, tag);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "dialogue"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, tag, 0, 2, &tmp))
            {
                errmsg(p, "Error: unknown dialogue value \"%s\"\n", value);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "disableDucking"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "interact"))
        {
            return 0;
        }
        /* start - handled by common attribute */
        /* duration - handled by common attribute */
        /* importance - handled by common attribute */
    }
    /************************** audioContent **************************************/
    else if (!strcasecmp(tag, "audioContent"))
    {
        if (!strcasecmp(attribute, "audioContentID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple IDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;        
        }
        else if (!strcasecmp(attribute, "audioContentName"))
        {
            if (0 != p->current_name[0])
            {
                errmsg(p, "multiple names specified on tag \"%s\"\n", tag);
                return 1;
            }
            if (!decode_string(p, value, p->current_name, sizeof(p->current_name)))
            {
                errmsg(p, "could not decode string value \"%s\" for attribute \"%s\" on tag \"%s\"\n",
                    value, attribute, tag);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "audioContentLanguage"))
        {
            if (0 != p->current_language[0])
            {
                errmsg(p, "multiple language attributes specified on tag %s\n", tag);
                return 1;
            }
            if (!decode_language(p, value, p->current_language))
            {
                errmsg(p, "invalid language attributes specified on tag %s\n", tag);
                return 1;
            }
            return 0;
        }
    }
    /************************** audioProgramme ************************************/
    else if (!strcasecmp(tag, "audioProgramme"))
    {
        if (!strcasecmp(attribute, "audioProgrammeID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple IDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;
        }
        else if (!strcasecmp(attribute, "audioProgrammeName"))
        {
            if (0 != p->current_name[0])
            {
                errmsg(p, "multiple names specified on tag \"%s\"\n", tag);
                return 1;
            }
            if (!decode_string(p, value, p->current_name, sizeof(p->current_name)))
            {
                errmsg(p, "could not decode string value \"%s\" for attribute \"%s\" on tag \"%s\"\n",
                    value, attribute, tag);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "audioProgrammeLanguage"))
        {
            if (0 != p->current_language[0])
            {
                errmsg(p, "multiple language attributes specified on tag %s\n", tag);
                return 1;
            }
            if (!decode_language(p, value, p->current_language))
            {
                errmsg(p, "invalid language attributes specified on tag %s\n", tag);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "end"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "maxDuckingDepth"))
        {
            return 0;
        }
        /* start - handled by common attribute */
    }
    /************************** audioTrackUID *************************************/
    else if (!strcasecmp(tag, "audioTrackUID"))
    {
        if (!strcasecmp(attribute, "UID"))
        {
            if (0 != p->current_id[0])
            {
                errmsg(p, "multiple track UIDs specified on tag \"%s\"\n", tag);
                return 1;
            }
            copy_current_id((char *)p->current_id, value, sizeof(p->current_id));
            return 0;
        }
        else if (!strcasecmp(attribute, "sampleRate"))
        {
            unsigned int tmp;
            if (decode_uint(p, value, attribute, 48000, 48000, &tmp))
            {
                return 0;
            }
        }
        else if (!strcasecmp(attribute, "bitDepth"))
        {
            unsigned int tmp;
            if (decode_uint(p, value, attribute, 16, 24, &tmp))
            {
                return 0;
            }
        }
    }
    /************************** audioFormatExtended *******************************/
    else if (!strcasecmp(tag, "audioFormatExtended"))
    {
        if (!strcasecmp(attribute, "version"))
        {
            if (strcasecmp(value, "ITU-R_BS.2076-2"))   /* We support only -2 right now */
            {
                errmsg(p, "unsupported audio format version \"%s\"\n", value);
                return 1;
            }
            return 0;
        }
    }

#ifndef IGNORE_AUDIO_FORMAT_CUSTOM_ELEMENT
    /************************** audioFormatCustomSet ******************************/
    else if (!strcasecmp(tag, "audioFormatCustomSet"))
    {
        if (!strcasecmp(attribute, "audioFormatCustomSetID"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "audioFormatCustomSetName"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "audioFormatCustomSetType"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "audioFormatCustomSetVersion"))
        {
            return 0;
        }
    }

    /************************** profile *******************************************/
    else if (!strcasecmp(tag, "profile"))
    {
        return 0;
    }
#endif

    /*######################### General elements #################################*/
    else if (!strcasecmp(tag, "xml"))
    {
        return 0;
    }

    /************************** ebuCoreMain ***************************************/
    else if (!strcasecmp(tag, "ebuCoreMain"))
    {
        return 0;
    }

    /*######################### ADM sub-elements #################################*/

    /************************** dialogue ******************************************/
    else if (!strcasecmp(tag, "dialogue"))
    {
        if (!strcasecmp(attribute, "nonDialogueContentKind"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, tag, 0, 2, &tmp))
            {
                errmsg(p, "Error: unkown nonDialogueContentKind: \"%s\"", value);
                return 1;
            }
            p->content.type = (dlb_sadm_content_type)(DLB_SADM_CONTENT_NK + tmp);
            return 0;
        }
        else if (!strcasecmp(attribute, "dialogueContentKind"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, tag, 0, 6, &tmp))
            {
                errmsg(p, "Error: unkown dialogueContentKind: \"%s\"", value);
                return 1;
            }
            p->content.type = (dlb_sadm_content_type)(DLB_SADM_CONTENT_DK + tmp);
            return 0;
        }
        else if (!strcasecmp(attribute, "mixedContentKind"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, tag, 0, 3, &tmp))
            {
                errmsg(p, "Error: unkown mixedContentKind: \"%s\"", value);
                return 1;
            }
            p->content.type = (dlb_sadm_content_type)(DLB_SADM_CONTENT_MK + tmp);
            return 0;
        }
    }
    /************************** renderer ******************************************/
    else if (!strcasecmp(tag, "renderer"))
    {
        if (!strcasecmp(attribute, "uri"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "name"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "version"))
        {
            return 0;
        }
    }
    /************************** loudnessMetadata **********************************/
    else if (!strcasecmp(tag, "loudnessMetadata"))
    {
        if (!strcasecmp(attribute, "loudnessMethod"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "loudnessRecType"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "loudnessCorrectionType"))
        {
            return 0;
        }
    }
    /************************** created *******************************************/
    else if (!strcasecmp(tag, "created"))
    {
        if (!strcasecmp(attribute, "startDate"))
        {
            return 0;
        }
        if (!strcasecmp(attribute, "startTime"))
        {
            return 0;
        }
    }
    /************************** frameFormat ***************************************/
    else if (!strcasecmp(tag, "frameFormat"))
    {
        if (!strcasecmp(attribute, "frameFormatID"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "type"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "timeReference"))
        {
            return 0;
        }
        else if (!strcasecmp(attribute, "flowID"))
        {
            dlb_pmd_success success = dlb_sadm_set_flow_id(p->model, value, strlen(value));
            (void)success;
            return 0;
        }
        else if (!strcasecmp(attribute, "countToFull"))
        {
            return 0;
        }
        /* start - handled by common attribute */
        /* duration - handled by common attribute */
    }
    /************************** transportTrackFormat ******************************/
    else if (!strcasecmp(tag, "transportTrackFormat"))
    {
        if (!strcasecmp(attribute, "transportID")
            || !strcasecmp(attribute, "transportName")
            || !strcasecmp(attribute, "numIDs")
            || !strcasecmp(attribute, "numTracks"))
        {
            return 0;
        }
    }
    /************************** gain **********************************************/
    else if (!strcasecmp(tag, "gain"))
    {
        if (!strcasecmp(attribute, "gainUnit"))
        {
            uint8_t tmp[7];
            const char *v = (const char *)tmp;

            if (!decode_string(p, value, tmp, sizeof(tmp)))
            {
                errmsg(p, "Error: invalid gainUnit: \"%s\"", value);
                return 1;
            }

            if (!strcasecmp(v, "linear"))
            {
                p->gain_unit_db = 0;
                return 0;
            }
            else if (!strcasecmp(v, "dB"))
            {
                p->gain_unit_db = 1;
                return 0;
            }
        }
    }
    /************************** audioObjectInteraction **********************************/
    else if (!strcasecmp(tag, "audioObjectInteraction"))
    {
        if (!strcasecmp(attribute, "onOffInteract"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, attribute, 0, 1, &tmp))
            {
                errmsg(p, "Error: invalid onOffInteract: \"%s\"", value);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "gainInteract"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, attribute, 0, 1, &tmp))
            {
                errmsg(p, "Error: invalid gainInteract: \"%s\"", value);
                return 1;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "positionInteract"))
        {
            unsigned int tmp;
            if (!decode_uint(p, value, attribute, 0, 1, &tmp))
            {
                errmsg(p, "Error: invalid gainInteract: \"%s\"", value);
                return 1;
            }
            return 0;
        }
    }
    /************************** gainInteractionRange ************************************/
    else if (!strcasecmp(tag, "gainInteractionRange"))
    {
        if (!strcasecmp(attribute, "gainUnit"))
        {
            uint8_t tmp[7];
            const char *v = (const char *)tmp;

            if (!decode_string(p, value, tmp, sizeof(tmp)))
            {
                errmsg(p, "Error: invalid gainUnit: \"%s\"", value);
                return 1;
            }

            if (!strcasecmp(v, "linear"))
            {
                p->gain_unit_db = 0;
                return 0;
            }
            else if (!strcasecmp(v, "dB"))
            {
                p->gain_unit_db = 1;
                return 0;
            }
            return 0;
        }
        else if (!strcasecmp(attribute, "bound"))
        {
            uint8_t tmp[7];
            const char *v = (const char *)tmp;

            if (!decode_string(p, value, tmp, sizeof(tmp)))
            {
                errmsg(p, "Error: invalid bound: \"%s\"", value);
                return 1;
            }

            if (  strcasecmp(v, "min")
               && strcasecmp(v, "max"))
            {
                errmsg(p, "Error: unsupported bound: \"%s\"", value);
                return 1;
            }
            return 0;
        }
        
    }
    /************************** audioTrack **********************************************/
    else if (!strcasecmp(tag, "audioTrack"))
    {
        if (!strcasecmp(attribute, "trackID"))
        {
            if (decode_uint(p, value, attribute, 1, 255, &p->current_track_id))
            {
                return 0;
            }
        }
        /* formatLabel - handled by common attribute */
        /* formatDefinition - handled by common attribute */
    }

    /*######################### Common attributes ################################*/
    if (!strcasecmp(attribute, "status"))
    {
        if (!strcasecmp(tag, "audioProgrammeIDRef")
            || !strcasecmp(tag, "audioContentIDRef")
            || !strcasecmp(tag, "audioObjectIDRef")
            || !strcasecmp(tag, "audioPackFormatIDRef")
            || !strcasecmp(tag, "audioTrackUIDRef")
            || !strcasecmp(tag, "audioChannelFormatIDRef")
            || !strcasecmp(tag, "audioTrackFormatIDRef")
            )
        {
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "formatLabel"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "formatDefinition"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "typeLabel"))
    {
        memmove(p->type_label, value, sizeof(p->type_label));
        return 0;
    }
    else if (!strcasecmp(attribute, "typeDefinition"))
    {
        memmove(p->type_definition, value, sizeof(p->type_definition));
        return 0;
    }
    else if (!strcasecmp(attribute, "language"))
    {
        if (0 != p->current_language[0])
        {
            errmsg(p, "multiple language attributes specified on tag %s\n", tag);
            return 1;
        }
        if (decode_language(p, value, p->current_language))
        {
            return 0;
        }
    }
    else if (!strcasecmp(attribute, "start"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "duration"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "lstart"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "lduration"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "importance"))
    {
        return 0;
    }
    else if (!strcasecmp(attribute, "coordinate"))
    {
        if (decode_coordinate_type_attribute(p, value, &p->current_coordinate_type))
        {
            return 0;
        }
    }

    errmsg(p, "Unexpected attribute \"%s\" on tag \"%s\" (with value \"%s\")\n",
           attribute, tag, value);
    return 1;
}


static
char*  /** has to return ptr to 1st char of next line, or NULL when no more available */
line_callback
    (void *context
    )
{
    dlb_sadm_reader *p = (dlb_sadm_reader *)context;
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
    dlb_sadm_reader *p = (dlb_sadm_reader *)context;
    if (NULL != p->ecb)
    {
        char report[1024];
        snprintf(report, sizeof(report), "ERROR at line %u: %s", p->lineno, msg);
        p->ecb(report, p->cbarg);
    }
}


static
int
find_channel_assignment
    (dlb_sadm_reader *reader
    ,dlb_sadm_id *track_uid)
{
    const char *t = (const char *)track_uid->data;
    size_t i = 0;
    int a = -1;

    while (i < reader->channel_assignment_count)
    {
        const char *u = (const char *)reader->channel_assignments[i].track_uid.data;

        if (!strncmp(t,  u, DLB_PMD_NAME_ARRAY_SIZE))
        {
            a = (int)reader->channel_assignments[i].channel_number;
            break;
        }
        i++;
    }

    return a;
}


/**
 * @brief check whether every track UID has a channel index
 *
 * Channel indices are assigned in the serial ADM header, under
 * the transportTrackFormat tag.  Each 'audioTrack' entry assigns a
 * channel index via its trackID attribute.
 */
static
dlb_pmd_success
check_track_uids
    (dlb_sadm_reader *reader
    ,dlb_sadm_model *model
    )
{
    dlb_sadm_track_uid_iterator tui;
    dlb_sadm_track_uid track_uid;
    dlb_pmd_success res;

    if (dlb_sadm_track_uid_iterator_init(&tui, model))
    {
        return PMD_FAIL;
    }

    res = PMD_SUCCESS;
    while (!dlb_sadm_track_uid_iterator_next(&tui, &track_uid))
    {
        dlb_pmd_bool is_common;

        if (dlb_sadm_track_uid_is_common_def(model, &track_uid, &is_common))
        {
            return PMD_FAIL;
        }
        if (is_common && track_uid.channel_idx == 0)
        {
            /* If the track uid is a common def and has no definite channel assignment, skip it */
            continue;
        }

        if (0 == track_uid.channel_idx)
        {
            int channel = find_channel_assignment(reader, &track_uid.id);

            if (channel < 1)
            {
                printf("Error: track UID \"%s\" has no assigned channel\n",
                       (char*)track_uid.id.data);
                res = PMD_FAIL;
            }
            else
            {
                track_uid.channel_idx = (unsigned int)channel;
                if (dlb_sadm_set_track_uid(model, &track_uid, NULL))
                {
                    return PMD_FAIL;
                }
            }
        }
    }
    return res;
}


/* --------------------------------- public api ------------------ */


dlb_pmd_bool
dlb_sadm_reader_check_xml
    (const char *buffer
    ,size_t length
    )
{
    static const char *tag1 = "<frame>";
    static const char *tag2 = "<frameheader>";

    const size_t tag1_len = strlen(tag1);
    const size_t tag2_len = strlen(tag2);

    if (length >= tag1_len + tag2_len)
    {
        char tmp[1024];
        unsigned int i;
        char *p;

        memset(tmp, '\0', sizeof(tmp));

        if (length > sizeof(tmp) - 1)
        {
            length = sizeof(tmp) - 1;
        }

        for (i = 0; i != length; ++i)
        {
            tmp[i] = (char)tolower(*buffer++);
        }

        p = strstr(tmp, tag1);
        if (NULL != p)
        {
            if (NULL != strstr(p + tag1_len, tag2))
            {
                return 1;
            }
        }
    }
    return 0;
}


size_t
dlb_sadm_reader_query_memory
    (dlb_sadm_counts *c
    )
{
    if (c)
    {
        return MEMREQ(dlb_sadm_reader,              1)
            +  MEMREQ(track_uid_to_channel_record,  c->num_track_uids)
            +  MEMREQ(dlb_sadm_programme_label,     c->max_programme_labels)
            +  MEMREQ(dlb_sadm_idref,               c->max_programme_contents)
            +  MEMREQ(dlb_sadm_idref,               c->max_content_objects)
            +  MEMREQ(dlb_sadm_idref,               c->max_object_objects)
            +  MEMREQ(dlb_sadm_idref,               c->max_object_track_uids)
            +  MEMREQ(dlb_sadm_idref,               c->max_packfmt_chanfmts)
            +  MEMREQ(dlb_sadm_idref,               c->max_chanfmt_blkfmts)
            ;
    }
    return 0;
}

    
dlb_pmd_success
dlb_sadm_reader_init
    (dlb_sadm_counts *c
    ,void *mem
    ,dlb_sadm_reader **reader
    )
{
    uintptr_t mc = (uintptr_t)mem;
    dlb_sadm_reader *r;
    size_t sz;

    sz = dlb_sadm_reader_query_memory(c);
    memset(mem, '\0', sz);

    r = (dlb_sadm_reader *)mc;
    mc += MEMREQ(dlb_sadm_reader, 1);

    r->channel_assignments = (track_uid_to_channel_record *)mc;
    mc += MEMREQ(track_uid_to_channel_record, c->num_track_uids);

    r->programme_labels = (dlb_sadm_programme_label*)mc;
    mc += MEMREQ(dlb_sadm_programme_label, c->max_programme_labels);

    r->programme_contents = (dlb_sadm_idref *)mc;
    mc += MEMREQ(dlb_sadm_idref, c->max_programme_contents);

    r->content_objects = (dlb_sadm_idref *)mc;
    mc += MEMREQ(dlb_sadm_idref, c->max_content_objects);

    r->object_objects = (dlb_sadm_idref *)mc;
    mc += MEMREQ(dlb_sadm_idref, c->max_object_objects);

    r->object_track_uids = (dlb_sadm_idref *)mc;
    mc += MEMREQ(dlb_sadm_idref, c->max_object_track_uids);

    r->packfmt_chanfmts = (dlb_sadm_idref*)mc;
    mc += MEMREQ(dlb_sadm_idref, c->max_packfmt_chanfmts);

    r->chanfmt_blkfmts = (dlb_sadm_idref*)mc;
    mc += MEMREQ(dlb_sadm_idref, c->max_chanfmt_blkfmts);

    memcpy(&r->limits, c, sizeof(r->limits));

    *reader = r;
    return PMD_SUCCESS;
}


void
dlb_sadm_reader_finish
    (dlb_sadm_reader *reader
    )
{
    (void)reader;
}


dlb_pmd_success
dlb_sadm_reader_read
    (dlb_sadm_reader *reader
    ,dlb_xmlpmd_line_callback lcb
    ,dlb_xmlpmd_error_callback ecb
    ,void *cbarg
    ,dlb_sadm_model *model
    )
{
#define NUM_UNDEFINED (128)    
    dlb_sadm_undefined_ref undefined[NUM_UNDEFINED];
    size_t count = 0;

    reader->coroutine_line = 0;
    reader->lcb = lcb;
    reader->ecb = ecb;
    reader->cbarg = cbarg;
    reader->line = NULL;
    reader->lineno = 0;
    reader->model = model;
    reader->gain_unit_db = 0;
    reader->channel_assignment_count = 0;

    if (dlb_sadm_model_limits(model, &reader->limits))
    {
        return PMD_FAIL;
    }

    tag_stack_init(&reader->tagstack);

    if (dlb_xml_parse2(reader, &line_callback, &element_callback, &attribute_callback,
                       dlb_xml_error_callback))
    {
        return PMD_FAIL;
    }

    count = dlb_sadm_get_undefined_references(model, undefined, NUM_UNDEFINED);
    if (count)
    {
        size_t i;
        for (i = 0; i < count && i < NUM_UNDEFINED; ++i)
        {
            printf("ERROR: undefined %s reference \"%s\" at line %u\n",
                   ref_types_names[undefined[i].type], undefined[i].id, undefined[i].lineno);
        }
        return PMD_FAIL;
    }

    return check_track_uids(reader, model);
}
