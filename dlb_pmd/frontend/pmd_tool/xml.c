/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2021 by Dolby Laboratories,
 *                Copyright (C) 2016-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
