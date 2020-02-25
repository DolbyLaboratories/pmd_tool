/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @file pmd_sadm_write_string.c
 * @brief helper function to write serial ADM XML to a string
 */

#include <stdio.h>
#include <string.h>

#include "dlb_pmd_api.h"
#include "dlb_pmd_sadm_string.h"


typedef struct
{
    char *buffer;
    size_t size;
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

    if (NULL == buf)
    {
        /* end */
        xbuf->size = pos - xbuf->buffer;
    }
    else if (pos)
    {
        /* pos will only be non-zero if we have already written something into
         * the buffer.  Since we only have one buffer, this means we've already
         * exhausted it, so we must return 0;
         */
        return 0;
    }
    else
    {
        *buf = xbuf->buffer;
        *capacity = xbuf->size;
    }
    return 1;
}


dlb_pmd_success
dlb_pmd_sadm_string_write
   (dlb_pmd_sadm_writer *w
   ,const dlb_pmd_model *model
   ,char *buffer
   ,size_t *size
   )
{
    xml_buffer xbuf;
    dlb_pmd_success ret;

    xbuf.buffer = buffer;
    xbuf.size = *size;
    
    ret = dlb_pmd_sadm_writer_write(w, model, get_buffer, 0, &xbuf);
    *size = xbuf.size;
    return ret;
}


