/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#include <string.h>

#include "dlb_pmd_sadm_string.h"


/**
 * @brief helper struct to walk through buffer line-by-line
 */
typedef struct
{
    dlb_xmlpmd_error_callback cberr; /**< client error callback */
    void *cbarg;                     /**< client's error callback user parameter */
    const char *pos;                 /**< current position in string */
    const char *end;                 /**< end of string */
    unsigned int lineno;             /**< line number */
    char line[4096];
} pmd_xml_buf;


/**
 * @brief helper callback to read user-supplied buffer
 */
static
char *                   /** @return start of next line, or NULL */
line_callback
    (void *arg           /**< [in] client argument */
    )
{
    pmd_xml_buf *xbuf = (pmd_xml_buf *)arg;
    size_t size = xbuf->end - xbuf->pos;
    const char *next;
    
    if (xbuf->pos == xbuf->end)
    {
        return NULL;
    }
    else
    {
        memset(xbuf->line, '\0', sizeof(xbuf->line));
        next = memchr(xbuf->pos, '\n', size);
        if (NULL == next)
        {
            next = memchr(xbuf->pos, '\r', size);
        }
        if (NULL == next)
        {
            next = xbuf->end;
        }
        else 
        {
            if (*next == '\n' || *next == '\r') ++next;
            if (*next == '\n') ++next;
        }

        memcpy(xbuf->line, xbuf->pos, next - xbuf->pos);
        xbuf->pos = next;
        xbuf->lineno += 1;
        return xbuf->line;
    }
}


/**
 * @brief error handler callback
 */
static
void
error_callback
    (const char *msg
    ,void *cbarg
    )
{
    pmd_xml_buf *xbuf = (pmd_xml_buf *)cbarg;
    if (NULL != xbuf->cberr)
    {
        xbuf->cberr(msg, xbuf->cbarg);
    }
}


dlb_pmd_success
dlb_pmd_sadm_string_read
   (dlb_pmd_sadm_reader       *rdr
   ,const char                *title
   ,const char                *data
   ,size_t                     size
   ,dlb_pmd_model             *pmd_model
   ,dlb_xmlpmd_error_callback  err
   ,void                      *arg
   ,unsigned int              *errline
   )
{
    pmd_xml_buf xbuf;
        
    xbuf.cberr  = err;
    xbuf.cbarg  = arg;
    xbuf.pos    = data;
    xbuf.end    = data + size;

    xbuf.lineno = 1;

    if (dlb_pmd_sadm_reader_read(rdr, pmd_model, title, line_callback, error_callback, &xbuf))
    {
        if (errline)
        {
            *errline = xbuf.lineno;
        }
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}

