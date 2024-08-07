/************************************************************************
 * dlb_pmd
 * Copyright (c) 2017-2019, Dolby Laboratories Inc.
 * Copyright (c) 2017-2019, Dolby International AB.
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

#include <stdio.h>
#include <string.h>

#include "dlb_pmd_xml_file.h"
#include "pmd_error_helper.h"


/**
 * @brief helper struct used to read parsers
 */
typedef struct
{
    dlb_xmlpmd_error_callback cberr; /**< client error callback */
    void *cbarg;                     /**< client's error callback user parameter */
    FILE *fp;                        /**< currently open file pointer */
    char line[4096];                 /**< current read line */
} xml_buffer;


/**
 * @brief helper callback to read another line from file
 */
static
char *                   /** @return start of next line, or NULL */
line_callback
    (void *arg           /**< [in] client argument */
    )
{
    xml_buffer *xbuf = (xml_buffer *)arg;
    char *line;

    if (feof(xbuf->fp))
    {
        return NULL;
    }

    line = fgets(xbuf->line, sizeof(xbuf->line), xbuf->fp);
    return line;
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
    xml_buffer *xbuf = (xml_buffer *)cbarg;
    if (NULL != xbuf->cberr)
    {
        xbuf->cberr(msg, xbuf->cbarg);
    }
}


/** ------------------------------ public API ------------------------- */


dlb_pmd_success
dlb_xmlpmd_file_read
   (const char                *filename
   ,dlb_pmd_model             *model
   ,dlb_pmd_bool               strict
   ,dlb_xmlpmd_error_callback  err
   ,void                      *arg
   )
{
    xml_buffer xbuf;
    dlb_pmd_success ret;

    xbuf.cberr = err;
    xbuf.cbarg = arg;
    xbuf.fp = fopen(filename, "r");
    if (NULL == xbuf.fp)
    {
        error(model, "Failed to open input file: %s\n", filename);
        return PMD_FAIL;
    }

    ret = (dlb_pmd_success)dlb_xmlpmd_parse(line_callback, error_callback, &xbuf, model, strict);
    fclose(xbuf.fp);
    return ret;
}


dlb_pmd_bool
dlb_xmlpmd_file_is_pmd
     (const char *filename
     )
{
    FILE *f = fopen(filename, "r");
    dlb_pmd_bool is_pmd = PMD_FALSE;
    if (NULL != f)
    {
        char tmp[1024];
        size_t sz;

        memset(tmp, '\0', sizeof(tmp));
        sz = fread(tmp, sizeof(tmp)-1, 1, f);
        (void)sz;
        if (dlb_xmlpmd_is_pmd(tmp, sizeof(tmp)))
        {
            is_pmd = PMD_TRUE;
        }
        fclose(f);
    }
    return is_pmd;
}
