/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2019 by Dolby Laboratories,
 *                Copyright (C) 2018-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
