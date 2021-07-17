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
 * @file dlb_sadm_write_string.c
 * @brief helper function to write seial ADM XML to a string
 */

#include <stdio.h>
#include <string.h>

#include "sadm/dlb_sadm_string.h"
#include "sadm/dlb_sadm_writer.h"


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
dlb_sadm_string_write
   (const dlb_sadm_model *model
   ,char *buffer
   ,size_t *size
   )
{
    xml_buffer xbuf;
    dlb_pmd_success ret;

    xbuf.buffer = buffer;
    xbuf.size = *size;
    
    ret = dlb_sadm_write(get_buffer, 0, &xbuf, model);
    *size = xbuf.size;
    return ret;
}


