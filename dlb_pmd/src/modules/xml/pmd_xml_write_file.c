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
 * @file pmd_xml_write_file.c
 * @brief helper function to write XML to a file
 */

#include <stdio.h>
#include <string.h>

#include "dlb_pmd_api.h"
#include "dlb_pmd_xml_file.h"
#include "pmd_error_helper.h"

typedef struct xml_buffer
{
    FILE *fp;
    dlb_pmd_model *model;
    char line[4096];
    int length;
    int indent;
} xml_buffer;


static
int
get_buffer
    (void *arg
    ,char *pos
    ,char **buf
    ,size_t *capacity
    )
{
    xml_buffer *xbuf = (xml_buffer *)arg;

    if (NULL == xbuf->fp)
    {
        return 0;
    }

    if (NULL != pos)
    {
        ptrdiff_t len = pos - xbuf->line;
        if (len < 0 || len > (ptrdiff_t)sizeof(xbuf->line))
        {
            error(xbuf->model, "Bad write-pointer returned by xml writer to get_buffer\n");
            return 0;
        }

        fwrite(xbuf->line, 1, len, xbuf->fp);
    }

    if (NULL != buf)
    {
        *buf = xbuf->line;
        *capacity = sizeof(xbuf->line);
    }
    return 1;
}


dlb_pmd_success
dlb_xmlpmd_file_write
   (const char          *filename
   ,const dlb_pmd_model *model
   )
{
    static xml_buffer xbuf;
    dlb_pmd_success ret;

    xbuf.fp = fopen(filename, "w");
    if (NULL == xbuf.fp)
    {
        error(model, "Failed to open output file: %s\n", filename);
        return PMD_FAIL;
    }

    ret = dlb_xmlpmd_write(get_buffer, 0, &xbuf, model);
    fclose(xbuf.fp);
    return ret;
}
