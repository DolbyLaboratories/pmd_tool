/************************************************************************
 * dlb_xmllib
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

/** @file dlb_xml.c */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "dlb_xml/include/dlb_xml.h"

/* Boolean type */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
typedef int dlb_xml_bool;

/** Context for each recursive level (one element) */
#define MAX_TAG_LEN 34    /**< Maximum significant characters in element and attribute tags */
#define MAX_TEXT_LEN 2048 /**< Maximum text length per comment, element or attribute */
struct dlb_xml_level_context 
{
    int level;                      /**< Level depth */
    /* Tag name */
    char tag[MAX_TAG_LEN + 1];      /**< Stores current tag name */
    char *p_tag;                    /**< Processing pointer for tag name */
    int tag_len;                    /**< Number of characters in tag name */
    /* Element text */
    char text[MAX_TEXT_LEN + 1];    /**< Stores current element text */
    char *p_text;                   /**< Processing pointer for element text */
    int text_len;                   /**< Number of characters in element text */
    /* Comment */
    char comment[MAX_TEXT_LEN + 1]; /**< Stores current comment */
    char *p_comment;                /**< Processing pointer for comment */
    int comment_len;                /**< Number of characters in comment */
    /* Attribute name */
    char attrib[MAX_TAG_LEN + 1];   /**< Stores current attribute name */
    char *p_attrib;                 /**< Processing pointer for attribute name */
    int attrib_len;                 /**< Number of characters in attribute name */
    /* Attribute value */
    char value[MAX_TEXT_LEN + 1];   /**< Stores current element value */
    char *p_value;                  /**< Processing pointer for element value */
    int value_len;                  /**< Number of characters in element value */
    char value_delimiter;           /**< Quote character around the attribute value, ' or " */
    /* Operation control flags */
    dlb_xml_bool b_prev_open;       /**< Open caret of tag has been processed already */
    dlb_xml_bool b_have_element;    /**< Open tag of an element has been processed */
    dlb_xml_bool b_tag_first_char;  /**< Expecting first chararacter after open caret of a tag */
    dlb_xml_bool b_inside_tag;      /**< Within tag (after open caret, before closing caret) */
    dlb_xml_bool b_closing_tag;     /**< Current tag is a closing tag */
    dlb_xml_bool b_declaration;     /**< Current element is actually the XML declaration */
    dlb_xml_bool b_tag_ended;       /**< Within tag, after element name (expecting attributes) */
    int comment_match;              /**< Element is a comment */
    dlb_xml_bool b_attrib_ended;    /**< Attribute name has ended */
    dlb_xml_bool b_attrib_val_start;/**< Attribute value is starting */
    dlb_xml_bool b_entity;          /**< Within entity */
    dlb_xml_bool b_sub_entity;      /**< Within additional level of entity */
};

/** Context for the overall XML processing */
struct dlb_xml_context 
{
    char *p_char;   /**< Pointer to next character to be parsed (or to NUL char for line loading) */
    char prev_char; /**< Most recent parsed character */
    /* Callbacks */
    void *p_context; /**< Callback context */
    char *(*line_callback) /**< Callback to load the next line */
        ( void *p_context
        );
    int (*element_callback) /**< Called when element is opened (text is NULL) or closed (with text enclosed by element) */
        ( void *p_context
        , char *tag
        , char *text
        );
    int (*attribute_callback) /**< Called for each attribute inside an element's open tag or declaration */
        ( void *p_context
        , char *tag
        , char *attribute
        , char *value
        );
    void (*error_callback)  /**< called to report parsing errors */
        ( void *p_context
        , char *msg
        );
};


/* Forward declaration of local function */
int 
dlb_xml_parse_level
    ( struct dlb_xml_context *p_context
    , int level
    );

/**
 * @brief invoke XML parser error callback, if any
 */
static
void
dlb_xml_report_error
    ( struct dlb_xml_context *p_context
    , const char *fmt
    , ...
    )
{
    if (NULL != p_context->error_callback)
    {
        char message[256];
        va_list args;
        
        va_start(args, fmt);
        vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);
        p_context->error_callback(p_context->p_context, message);
    }
}


/**
 * @brief Load next line if necessary
 *
 * If a new line has to be loaded, p_context->p_char will point at
 * the first character of the new line, or be NULL on EOF
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @return: DLB_XML_SUCCESS: processed normally (including EOF encounter), DLB_XML_ERROR: encountered error
 */
static 
int 
dlb_xml_advance_line
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    )
{
    char *eol;
    
    /* Load next line if necessary */
    while ((p_context->p_char == NULL) || (*(p_context->p_char) == '\0')) 
    {
        /* Get next line chunk */
        p_context->p_char = (*(p_context->line_callback))
            ( p_context->p_context
            );
        
        /* End of File/Stream? */
        if (p_context->p_char == NULL) 
        {
            if (
                p_level_context->b_inside_tag       || 
                (! p_level_context->b_closing_tag)  || 
                (p_level_context->level > 0)
            ) 
            {
                dlb_xml_report_error(p_context, "XML ERROR[%u] Unterminated element: <%s>", p_level_context->level, p_level_context->tag);
                return DLB_XML_ERROR;
            }
            break;
        }
        
        /* Skip leading whitespaces */
        while (
            (*(p_context->p_char) != '\0') && 
            (
                (*(p_context->p_char) == ' ') || 
                (*(p_context->p_char) == '\t')
            )
        ) 
        {
            p_context->p_char++;
        }
        
        /* Remove EOL, add space */
        eol = p_context->p_char;
        while (*eol != '\0') 
        {
            if (
                (*eol == '\n') || 
                (*eol == '\r')
            ) 
            {
                /* Append space, unless unnecessary */
                if (
                    (eol != p_context->p_char) && 
                    (*(eol - 1) != '>')
                ) 
                {
                    /* Add space in place of first char of EOL */
                    *eol++ = ' ';
                }
                /* Assure string termination */
                *eol = '\0';
                break;
            }
            eol++;
        }
    }
    
    return DLB_XML_SUCCESS;
}

/**
 * @brief XML processing of one character when not within the Carets of a Tag
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @return: FALSE: processed normally, TRUE: encountered "virtual" '<', skip ahead in loop without advancing in source string
 */
static 
dlb_xml_bool 
dlb_xml_process_outside_tag
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    )
{
    /* Examine character, Look for tag start (open or close tag) */
    if (
        p_level_context->b_prev_open ||  /* virtual '<' */
        (*(p_context->p_char) == '<')    /* actual '<' */
    )
    {
        p_level_context->b_tag_first_char   = TRUE;     /* Initial part after open caret */
        p_level_context->b_inside_tag       = TRUE;     /* Now inside the tag */
        p_level_context->b_tag_ended        = FALSE;    /* Expect tag first, not attribute */
        
        /* Skip ahead after virtual caret */
        if (p_level_context->b_prev_open) 
        {
            p_level_context->b_prev_open = FALSE;
            return TRUE;
        }
    } 
    else 
    if (p_level_context->text_len < MAX_TEXT_LEN) /* Assure there is enough space */
    {
        /* Copy character into text string */
        *p_level_context->p_text++ = *(p_context->p_char);
        p_level_context->text_len++;
    }
    
    return FALSE;
}

/**
 * @brief XML processing of one character of an entity
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_entity
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    /* Examine character */
    if (*(p_context->p_char) == '>') /* entity ends */
    {
        p_level_context->b_entity = FALSE;
        p_level_context->b_inside_tag = FALSE;
    }
    else
    if (*(p_context->p_char) == '[') /* sub-level of entities */
    {
        /* recurse */
        *p_ret = dlb_xml_parse_level
            ( p_context
            , p_level_context->level + 1
            );
        if (*p_ret != DLB_XML_SUCCESS) 
        {
            return TRUE;
        }
    }
    /* Discard anything in entities */
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}

/**
 * @brief XML processing of one character of a comment
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_comment
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    /* Examine character */
    if (*(p_context->p_char) == '-') /* match either of the 4 '-' in the comment tag */
    {
        p_level_context->comment_match++;
        if (p_level_context->comment_match == 3) /* matched comment open '<!--', reset comment buffer */
        {
            p_level_context->p_comment = &(p_level_context->comment[0]);
            p_level_context->comment_len = 0;
        }
    }
    else
    if (
        (p_level_context->comment_match == 5) && 
        (*(p_context->p_char) == '>')
    )
    {
        /* match closing caret of comment, after two '-', for the '-->' */
        *p_level_context->p_comment = '\0';
        p_level_context->comment_match = 0; /* Comment ends */
        p_level_context->b_inside_tag = FALSE;
    }
    else 
    if (p_level_context->comment_match == 1) /* no initial '-', assume entity */
    {
        p_level_context->b_entity = TRUE; /* switch to entity processing */
        p_level_context->comment_match = 0;
    }
    else 
    if (p_level_context->comment_match < 3) /* neither '-' nor '>' when expected */
    {
        dlb_xml_report_error(p_context, "XML ERROR[%u] malformed comment!", p_level_context->level);
        *p_ret = DLB_XML_ERROR;
        return TRUE;
    }
    else 
    {
        while (p_level_context->comment_match > 3) 
        {
            if (p_level_context->comment_len < MAX_TEXT_LEN) /* Assure there is enough space */
            {
                /* Copy comment into comment string */
                *p_level_context->p_comment++ = '-';
                p_level_context->comment_len++;
            }
            p_level_context->comment_match--;
        }
        if (p_level_context->comment_len < MAX_TEXT_LEN) /* Assure there is enough space */
        {
            /* Copy comment into comment string */
            *p_level_context->p_comment++ = *(p_context->p_char);
            p_level_context->comment_len++;
        }
    }
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}

/**
 * @brief XML processing on end of element name
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_finalize_tag_name
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    /* Finalize tag name */
    if (p_level_context->b_closing_tag) 
    {
        /* Assure closing tag is same length as opening tag */
        if (
            (*p_level_context->p_tag != '\0') && 
            (! p_level_context->b_tag_ended)
        ) 
        {
            dlb_xml_report_error(p_context, "XML ERROR[%u] closing tag mismatch! %c instead of EOL", p_level_context->level, *p_level_context->p_tag);
            *p_ret = DLB_XML_ERROR;
            return TRUE;
        }
    } 
    else 
    {
        /* Terminate tag name */
        *p_level_context->p_tag = '\0';
        
        /* Callback on element's open tag */
        if (! p_level_context->b_declaration) 
        {
            if (p_context->element_callback != NULL) 
            {
                *p_ret = (*(p_context->element_callback))
                    ( p_context->p_context
                    , p_level_context->tag
                    , NULL
                    );
                if (*p_ret != DLB_XML_SUCCESS) 
                {
                    return TRUE;
                }
            }
        }
    }
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}


/**
 * @brief verify XML string contains a valid escape (exluding leading '&' symbol)
 *
 * @param p_text: Text to verify  
 * @param p_unicode: output code
 * @return TRUE is escape is well-formed, FALSE: otherwise
 */
static
dlb_xml_bool
dlb_xml_decode_escape
    (const char **pp_text
    ,unsigned int *p_unicode
    )
{
    const char *p_text = *pp_text;
    const char *p_end = strchr(p_text, ';');
    
    if (NULL == p_end)
    {
        return FALSE;
    }
    
    *pp_text = p_end + 1;
    if (1 == sscanf(p_text, "#%u;", p_unicode))
    {
        return TRUE;
    }
    if (1 == sscanf(p_text, "#x%x;", p_unicode))
    {
        return TRUE;
    }
    if (0 == strncmp(p_text, "amp;", 4))
    {
        *p_unicode = '&';
        return TRUE;
    }
    if (0 == strncmp(p_text, "lt;", 3))
    {
        *p_unicode = '<';
        return TRUE;
    }
    if (0 == strncmp(p_text, "gt;", 3))
    {
        *p_unicode = '>';
        return TRUE;
    }
    if (0 == strncmp(p_text, "quot;", 5))
    {
        *p_unicode = '\"';
        return TRUE;
    }
    if (0 == strncmp(p_text, "apos;", 5))
    {
        *p_unicode = '\'';
        return TRUE;
    }
    return FALSE;
}


/**
 * @brief verify XML string contains a valid utf-8 encoded char
 *
 * @param c0: first byte of encoding
 * @param p_text: remainder of utf8 encoding
 * @param p_end: end of text string
 * @param p_unicode: output unicode char code
 * @return TRUE is escape is well-formed, FALSE: otherwise
 */
static
dlb_xml_bool
dlb_xml_decode_utf8char
    (unsigned char c0
    ,const char **pp_text
    ,const char *p_end
    ,unsigned int *p_unicode
    )
{
    unsigned char c1;
    unsigned char c2;
    unsigned char c3;
    const char *p_text = *pp_text;
            
    if (0xC0 == (c0 & 0xE0))
    {
        /* one continuation byte */
        if (p_text >= p_end) return FALSE;
        c1 = p_text[0];
        if ((c1 & 0xC0) != 0x80) return FALSE;
        *p_unicode = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
        p_text += 1;
    }
    else if (0xE0 == (c0 & 0xF0))
    {
        /* two continuation bytes */
        if (p_text+1 >= p_end) return FALSE;
        c1 = p_text[0];
        c2 = p_text[1];
        if ((c1 & 0xC0) != 0x80) return FALSE;                
        if ((c2 & 0xC0) != 0x80) return FALSE;
        *p_unicode = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        p_text += 2;
    }
    else if (0xF0 == (c0 & 0xF8)) 
    {
        /* three continuation bytes */
        if (p_text+2 >= p_end) return FALSE;
        c1 = p_text[0];
        c2 = p_text[1];
        c3 = p_text[2];
        if ((c1 & 0xC0) != 0x80) return FALSE;                
        if ((c2 & 0xC0) != 0x80) return FALSE;
        if ((c3 & 0xC0) != 0x80) return FALSE;
        *p_unicode = ((c0 & 0x07) << 18) | ((c1 & 0x3f) << 12) | ((c2 & 0x3f) << 6) | (c3 & 0x3f);
        p_text += 3;
    }
    else  /* illegal UTF-8 code */
    {
        return FALSE;
    }
    *pp_text = p_text;
    return TRUE;
}


/**
 * @brief verify XML string is well-formed (with respect to escape characters)
 *
 * @param p_text: Text to verify  
 * @param text_len: length of text to verify
 * @return TRUE is string is well-formed, FALSE: otherwise
 */
static
dlb_xml_bool
dlb_xml_check_text
    (const char *p_text
    ,int text_len
    )
{
    const char *end = p_text + text_len;
    unsigned int unicode = 0;
    unsigned char c;

    while (p_text < end)
    {
        c = *p_text;
        ++p_text;
        if ('&' == c)
        {
            if (!dlb_xml_decode_escape(&p_text, &unicode))
            {
                return FALSE;
            }            
        }
        else if (c < ' ' && c != '\t' && c != '\r' && c != '\n')
        {
            return FALSE;
        }
        else if (c > 127)
        {
            if (!dlb_xml_decode_utf8char(c, &p_text, end, &unicode))
            {
                return FALSE;
            }
        }
    
        /* now check valid ranges: legal chars are Unicode and ISO/IEC 10646 */
        if (!((unicode < 0xd800) ||
              (unicode >= 0xe000 && unicode <= 0xfffd) ||
              (unicode >= 0x10000 && unicode <= 0x10ffff)))
        {
            return FALSE;
        }
    }
    return TRUE;
}


/**
 * @brief XML processing on closing caret of a tag
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_tag_exit
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    p_level_context->b_inside_tag = FALSE;
    
    /* Finalize tag name, if not already done */
    if (! p_level_context->b_tag_ended) 
    {
        if (dlb_xml_finalize_tag_name(p_context, p_level_context, p_ret)) 
        {
            return TRUE;
        }
    }
    
    /* Process element */
    if (p_level_context->b_declaration) 
    {
        /* Validate declaration tag */
        if (
            (p_level_context->tag_len > 0) && 
            (p_context->prev_char != '?')
        ) 
        {
            dlb_xml_report_error(p_context, "XML WARNING[%u] declaration not closed correctly: '%s'", p_level_context->level, p_level_context->tag);
        }
    }
    else
    {
        /* Now we have an element */
        p_level_context->b_have_element = TRUE;
        
        /* Singular tag? */
        if (
            (p_level_context->tag_len > 0) && 
            (p_context->prev_char == '/')
        ) 
        {
            p_level_context->b_closing_tag = TRUE; /* Tag is open and close: <xxx/> */
        }
        
        /* Process completed element */
        if (p_level_context->b_closing_tag) 
        {
            /* Finalize text */
            *p_level_context->p_text = '\0';
            
            if (!dlb_xml_check_text(p_level_context->text, p_level_context->text_len))
            {
                dlb_xml_report_error(p_context,
                                     "XML ERROR[%u] malformed text \"%s\": <%s>",
                                     p_level_context->level, p_level_context->text,
                                     p_level_context->tag);

                *p_ret = DLB_XML_ERROR;
                return TRUE;
            }

            if (p_context->element_callback != NULL) 
            {
                /* Callback on close tag, with text enclosed in element */
                *p_ret = (*(p_context->element_callback))
                    ( p_context->p_context
                    , p_level_context->tag
                    , p_level_context->text
                    );
            } 
            else 
            {
                *p_ret = DLB_XML_SUCCESS;
            }
            
            /* Reset text buffer */
            p_level_context->p_text = &p_level_context->text[0];
            p_level_context->text_len = 0;
            
            return TRUE; /* Normal end of this element level */
        }
    }
    
    /* Reset tag buffer */
    p_level_context->p_tag = &p_level_context->tag[0];
    p_level_context->tag_len = 0;
    /* Reset flag */
    p_level_context->b_declaration = FALSE;
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}

/**
 * @brief XML processing of one character of the element name in a tag
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_tag_name
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    /* Check if name ends */
    if (
        (*(p_context->p_char) == ' ')   || 
        (*(p_context->p_char) == '\t')  || 
        (*(p_context->p_char) == '/')   || 
        (*(p_context->p_char) == '?')
    ) 
    {
        if (p_level_context->tag_len > 0) 
        {
            p_level_context->b_tag_ended = TRUE; /* Tag name has ended before closing caret, potentially attributes left */
            
            /* Finalize tag name and perform open callback before parsing attributes in this tag */
            if (dlb_xml_finalize_tag_name(p_context, p_level_context, p_ret)) 
            {
                return TRUE;
            }
            
            /* Reset attribute name buffer */
            p_level_context->p_attrib = &(p_level_context->attrib[0]);
            p_level_context->attrib_len = 0;
        }
    } 
    else 
    
    /* Process name character */
    if (p_level_context->tag_len < MAX_TAG_LEN) /* Assure there is enough space */
    {
        if (p_level_context->b_closing_tag) 
        {
            /* Compare closing tag against opening tag */
            if (*p_level_context->p_tag != *(p_context->p_char)) 
            {
                if (*p_level_context->p_tag == '\0') 
                {
                    dlb_xml_report_error(p_context, "XML ERROR[%u] closing tag mismatch! %c instead of EOL after '%s'", p_level_context->level, *(p_context->p_char), p_level_context->tag);
                } 
                else 
                {
                    dlb_xml_report_error(p_context, "XML ERROR[%u] closing tag mismatch! %c instead of %c in '%s'", p_level_context->level, *(p_context->p_char), *p_level_context->p_tag, p_level_context->tag);
                }
                p_level_context->b_tag_ended = TRUE; /* No further comparison, declare tag as ended */
                *p_ret = DLB_XML_ERROR;
                return TRUE;
            }
            p_level_context->p_tag++;
        } 
        else 
        {
            /* Copy tag name into tag string */
            *p_level_context->p_tag++ = *(p_context->p_char);
        }
        p_level_context->tag_len++;
    }
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}

/**
 * @brief XML processing of one character of an attribute name in a tag
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_attribute_name
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    if (*(p_context->p_char) == '=') /* Attribute name ended, value about to start */
    {
        *p_level_context->p_attrib = '\0';
        p_level_context->b_attrib_ended = TRUE;
        p_level_context->b_attrib_val_start = TRUE;
        p_level_context->p_value = &(p_level_context->value[0]);
        p_level_context->value_len = 0;
    }
    else 
    if (
        ((*(p_context->p_char) != ' ') && (*(p_context->p_char) != '\t')) && /* Skip whitespace */
        (p_level_context->attrib_len < MAX_TAG_LEN) /* Assure there is enough space */
    )
    {
        /* Copy attribute name into attribute string */
        *p_level_context->p_attrib++ = *(p_context->p_char);
        p_level_context->attrib_len++;
    }
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}

/**
 * @brief XML processing of one character of the value of an attribute
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_attribute_value
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    if (p_level_context->b_attrib_val_start) /* Opening quote of attribute value */
    {
        p_level_context->value_delimiter = *(p_context->p_char);
        p_level_context->b_attrib_val_start = FALSE;
    } 
    else 
    if (*(p_context->p_char) == p_level_context->value_delimiter) /* Closing quote of attribute value */
    {
        *p_level_context->p_value = '\0';
        p_level_context->b_attrib_ended = FALSE;

        if (!dlb_xml_check_text(p_level_context->value, p_level_context->value_len))
        {
            dlb_xml_report_error(p_context,
                                 "XML ERROR[%u] malformed attribute text \"%s\": <%s %s>",
                                 p_level_context->level, p_level_context->value,
                                 p_level_context->tag,
                                 p_level_context->attrib);
            *p_ret = DLB_XML_ERROR;
            return TRUE;
        }

        if (p_context->attribute_callback != NULL) 
        {
            /* Callback for each attribute within an element's open tag */
            *p_ret = (*(p_context->attribute_callback))
                ( p_context->p_context
                , p_level_context->tag
                , p_level_context->attrib
                , p_level_context->value);
            if (*p_ret != DLB_XML_SUCCESS) 
            {
                return TRUE;
            }
        }
        p_level_context->p_attrib = &(p_level_context->attrib[0]);
        p_level_context->attrib_len = 0;
    }
    else 
    if (p_level_context->value_len < MAX_TEXT_LEN) /* Assure there is enough space */
    {
        /* Copy attribute value into value string */
        *p_level_context->p_value++ = *(p_context->p_char);
        p_level_context->value_len++;
    }
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}

/**
 * @brief XML processing of one character when within the Carets of a Tag
 *
 * @param p_context: Global XML parsing context
 * @param p_level_context: Local context of the current level
 * @param p_ret [out]: Error code on non-zero return (DLB_XML_SUCCESS: OK, otherwise NG)
 * @return: FALSE: continue processing, TRUE: stop processing XML
 */
static 
dlb_xml_bool 
dlb_xml_process_inside_tag
    ( struct dlb_xml_context *p_context
    , struct dlb_xml_level_context *p_level_context
    , int *p_ret
    )
{
    /* Examine first character of Tag */
    if (p_level_context->b_tag_first_char && (*(p_context->p_char) == '/')) /* "</" */
    {
        p_level_context->b_closing_tag = TRUE; /* Following is a closing tag */
    } 
    else 
    if (p_level_context->b_tag_first_char && (*(p_context->p_char) == '?')) /* "<?" */ 
    {
        p_level_context->b_declaration = TRUE; /* Following is a declaration */
    } 
    else 
    if (p_level_context->b_tag_first_char && (*(p_context->p_char) == '!'))  /* "<!" */
    {
        p_level_context->comment_match = TRUE; /* Following might be a comment, start matching */
    } 
    else 
    
    /* Recurse to next element? */
    if (
        p_level_context->b_tag_first_char && 
        p_level_context->b_have_element
    ) 
    {
        /* Another element is opening (we already have one), so recurse for the new one */
        *p_ret = dlb_xml_parse_level(p_context, p_level_context->level + 1);
        if (*p_ret != DLB_XML_SUCCESS) 
        {
            return TRUE;
        }
        p_level_context->b_inside_tag = FALSE; /* Therefore, no longer inside a tag */
    } 
    else 
    
    /* Process entity */
    if (p_level_context->b_entity) 
    {
        if (dlb_xml_process_entity(p_context, p_level_context, p_ret)) 
        {
            return TRUE;
        }
    }
    else 
    
    /* Process comment */
    if (p_level_context->comment_match > 0) 
    {
        if (dlb_xml_process_comment(p_context, p_level_context, p_ret)) 
        {
            return TRUE;
        }
    }
    else 
    
    /* Processing element tag content */
    {
        /* Update open/close tag flag */
        if (p_level_context->b_tag_first_char) 
        {
            p_level_context->b_closing_tag = FALSE; /* First char of tag was no '/', so no closing tag */
        }
        
        /* Check whether tag is done */
        if (*(p_context->p_char) == '>') /* Tag ends */
        {
            if (dlb_xml_process_tag_exit(p_context, p_level_context, p_ret)) 
            {
                return TRUE;
            }
        } 
        else 
        
        /* Process element name in tag */
        if (! p_level_context->b_tag_ended) 
        {
            if (dlb_xml_process_tag_name(p_context, p_level_context, p_ret)) 
            {
                return TRUE;
            }
        } 
        else 
        
        /* Process attribute name in tag */
        if (! p_level_context->b_attrib_ended) 
        {
            if (dlb_xml_process_attribute_name(p_context, p_level_context, p_ret)) 
            {
                return TRUE;
            }
        } 
        else 
        
        /* Process attribute value */
        if (dlb_xml_process_attribute_value(p_context, p_level_context, p_ret)) 
        {
            return TRUE;
        }
    }
    p_level_context->b_tag_first_char = FALSE; /* First char of tag has been processed */
    
    *p_ret = DLB_XML_SUCCESS;
    return FALSE;
}


/**
 * @brief Simple XML parser
 * 
 * Limited error and syntax checking
 * 
 * @param p_context: Pointer to processing context
 * @param level: Current hierarchy depth level
 */
int 
dlb_xml_parse_level
    ( struct dlb_xml_context *p_context
    , int level
    )
{
    int ret = DLB_XML_SUCCESS;
    struct dlb_xml_level_context lc;
    
    /* Sanity check */
    if ((p_context == NULL) || (p_context->line_callback == NULL)) 
    {
#ifndef NDEBUG
        fprintf(stderr, "XML ERROR[%u] Invalid pointer!\n", level);
#endif /* NDEBUG */
        return DLB_XML_INVALID_POINTER;
    }
    
    /* Init local context (Reset text buffers) */
    memset(&lc, 0, sizeof(lc));
    lc.level = level;
    lc.p_tag = &lc.tag[0];
    lc.p_text = &lc.text[0];
    
    /* Check whether tag has already been opened */
    if ((level > 0) && (p_context->prev_char == '<')) 
    {
        lc.b_prev_open = TRUE;
    } 
    else 
    if ((level > 0) && (*(p_context->p_char) == '[')) 
    {
        lc.b_sub_entity = TRUE;
    }
    
    /* Process XML, one character at a time */
    do 
    {
        /* Load next line if necessary */
        ret = dlb_xml_advance_line(p_context, &lc);
        if ((ret != DLB_XML_SUCCESS) || (p_context->p_char == NULL)) 
        {
            return ret;
        }
        
        /* Processing inside and outside of the carets of a tag */
        if (! lc.b_inside_tag) 
        {
            /* Check if entity sub level ends */
            if (lc.b_sub_entity && (*(p_context->p_char) == ']')) 
            {
                lc.b_sub_entity = FALSE;
                break;
            } 
            else 
            if (dlb_xml_process_outside_tag(p_context, &lc)) 
            {
                continue;
            }
        } 
        else 
        {
            if (dlb_xml_process_inside_tag(p_context, &lc, &ret)) 
            {
                break;
            }
        }
        
        /* Advance to in string to next character */
        p_context->prev_char = *(p_context->p_char); /* Remember most recent character */
        (p_context->p_char)++; /* Advance in string */
    } 
    while (1);
    
    return ret;
}

/* Parse XML */
int 
dlb_xml_parse2
    ( void *p_context
    , char *(*line_callback)   (void *p_context)
    , int (*element_callback)  (void *p_context, char *tag, char *text)
    , int (*attribute_callback)(void *p_context, char *tag, char *attribute, char *value)
    , void  (*error_callback)  (void *p_context, char *msg)
    )
{
    struct dlb_xml_context context;
    
    /* Prepare context */
    memset(&context, 0, sizeof(context));
    context.p_context           = p_context;
    context.line_callback       = line_callback;
    context.element_callback    = element_callback;
    context.attribute_callback  = attribute_callback;
    context.error_callback      = error_callback;
    
    /* Recursively process XML elements */
    return dlb_xml_parse_level(&context, 0);
}

/* Return library version */
int 
dlb_xml_query_version
    ( dlb_xml_version *p_version
    )
{
    if (p_version == NULL) 
    {
        return -1;
    }
    
    p_version->version_major  = DLB_XML_VERSION_MAJOR;
    p_version->version_minor  = DLB_XML_VERSION_MINOR;
    p_version->version_update = DLB_XML_VERSION_UPDATE;
    
    return 0;
}

