/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2019 by Dolby Laboratories,
 *                Copyright (C) 2016-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file klv.c
 * @brief SMPTE 336-KLV reader/writer functionality for pmd tool
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "dlb_pmd_api.h"
#include "dlb_pmd_model_combo.h"
#include "dlb_pmd_klv.h"
#include "pmd_tool_klv.h"

#define KLV_BUF_SIZE (102400)


int
klv_read
    (const char                 *filename
    ,dlb_pmd_model_combo        *model
    )
{
    dlb_pmd_model   *pmd_model;
    uint8_t         *buffer;
    unsigned int     length;
    size_t           res;
    long             pos;
    FILE            *f;

    if (dlb_pmd_model_combo_get_writable_pmd_model(model, &pmd_model, PMD_TRUE))
    {
        printf("Could not get writable PMD model\n");
        return 1;
    }

    f = fopen(filename, "rb");
    if (NULL == f)
    {
        printf("Failed to open input file: %s\n", filename);
        return 1;
    }

    if (fseek(f, 0, SEEK_END))
    {
        printf("Failed to seek to end of input file: %d, %s\n", errno,
               strerror(errno));
        fclose(f);
        return 1;
    }

    pos = ftell(f);
    if (pos < 0)
    {
        printf("ftell failed: %d, %s\n", errno, strerror(errno));
        fclose(f);
        return 1;
    }

    length = pos;
    buffer = malloc(length);
    if (NULL == buffer)
    {
        printf("Failed to allocate file buffer: %d, %s\n", errno, strerror(errno));
        return 1;
    }

    rewind(f);
    res = fread(buffer, 1, pos, f);
    fclose(f);
    if ((long)res != pos)
    {
        assert((long)res == pos);
    }

    res = dlb_klvpmd_read_payload(buffer, length, pmd_model, 1, NULL, NULL);
    if (res)
    {
        const char *errmsg = dlb_pmd_error(pmd_model);
        puts(errmsg);
    }

    memset(buffer, '\0', length);
    free(buffer);
    return (int)res;
}


int
klv_write
    (const char                 *filename
    ,dlb_pmd_model_combo        *model
    ,dlb_klvpmd_universal_label  ul
    )
{
    const dlb_pmd_model *pmd_model;
    unsigned char       *buffer  = malloc(KLV_BUF_SIZE);
    int                  length;
    FILE                *f;

    if (dlb_pmd_model_combo_ensure_readable_pmd_model(model, &pmd_model, PMD_TRUE))
    {
        printf("Could not ensure readable PMD model\n");
        return 1;
    }

    f = fopen(filename, "wb");
    if (NULL == f)
    {
        printf("Failed to open output file: %s\n", filename);
        free(buffer);
        return 1;
    }

    length = dlb_klvpmd_write_all((dlb_pmd_model *)pmd_model, 0, buffer, KLV_BUF_SIZE, ul); /* const cast */
    if (length)
    {
        fwrite(buffer, 1, length, f);
    }
    else
    {
        printf("Failed to write MTx(0)\n");
    }

    fclose(f);
    free(buffer);
    return length == 0;
}
