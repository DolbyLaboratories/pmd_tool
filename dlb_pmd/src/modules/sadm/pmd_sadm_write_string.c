/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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


