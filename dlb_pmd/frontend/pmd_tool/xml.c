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
 * @file xml.c
 * @brief XML reader/writer functionality for pmd tool
 */

#include "xml.h"
#include "dlb_pmd_api.h"
#include "dlb_pmd_model_combo.h"
#include "dlb_pmd_xml_file.h"
#include "dlb_pmd_sadm_file.h"

#include <stdio.h>
#include <string.h>


static
void
error_callback
    (const char *msg
    ,      void *arg
    )
{
    (void)arg;
    puts(msg);
}


int
xml_read
    (const char             *filename
    ,dlb_pmd_model_combo    *model
    ,dlb_pmd_bool            strict
    ,dlb_pmd_bool            use_common_defs
    )
{
    dlb_pmd_bool     is_pmd  = dlb_xmlpmd_file_is_pmd (filename);
    dlb_pmd_bool     is_sadm = dlb_xmlpmd_file_is_sadm(filename);

    if (is_sadm)
    {
        if (dlb_pmd_sadm_file_read(filename, model, use_common_defs, error_callback, NULL))
        {
            printf("XML read sADM file failed\n");
            return 1;
        }
    }
    else
    {
        dlb_pmd_model   *pmd_model;

        if (dlb_pmd_model_combo_get_writable_pmd_model(model, &pmd_model, PMD_TRUE))
        {
            printf("Could not get writable PMD model\n");
            return 1;
        }

        if (is_pmd)
        {
            if (dlb_xmlpmd_file_read(filename, pmd_model, strict, error_callback, NULL))
            {
                printf("XML read file failed: %s\n", dlb_pmd_error(pmd_model));
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }

    return 0;
}


int
xml_write
    (const char             *filename
    ,dlb_pmd_model_combo    *model
    ,dlb_pmd_bool            sadm_out
    )
{
    if (sadm_out)
    {
        if (dlb_pmd_sadm_file_write(filename, model))
        {
            printf("Error writing S-ADM file\n");
            return -1;
        }
    }
    else
    {
        const dlb_pmd_model *pmd_model;

        if (dlb_pmd_model_combo_ensure_readable_pmd_model(model, &pmd_model, PMD_TRUE))
        {
            printf("Could not ensure readable PMD model\n");
            return 1;
        }
        if (dlb_xmlpmd_file_write(filename, pmd_model))
        {
            printf("Error: %s", dlb_pmd_error(pmd_model));
            return -1;
        }
    }

    return 0;
}
