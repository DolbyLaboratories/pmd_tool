/************************************************************************
 * dlb_pmd
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
