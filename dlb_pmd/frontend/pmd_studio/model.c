/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
  * @file model.c
  * @brief helper data struct to encapsulate use of dlb_pmd_model
  */

#include "model.h"
#include "dlb_pmd_api.h"
#include "dlb_pmd_model_combo.h"
#include "dlb_pmd_xml_file.h"
#include "dlb_pmd_sadm_file.h"
#include "pmd_studio_common_defs.h"
#include "dlb_pmd/frontend/pmd_tool/xml.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
#define FAILURE PMD_FAIL
#else
static dlb_pmd_success ret_fail()
{
    return PMD_FAIL;    // Put a breakpoint here
}
#define FAILURE ret_fail()
#endif

#define CHECK_SUCCESS(s) if ((s) != PMD_SUCCESS) return FAILURE

    
/**
 * @brief helper function to print errors
 */
static
void
error_callback
    (const char *msg
    ,void *arg
    )
{
    (void)arg;
    puts(msg);
}


dlb_pmd_success
model_init
    (model *m
    )
{
    dlb_pmd_bool use_common_defs = PMD_FALSE;   /* TODO: this should be a parameter */

    memset(m, 0, sizeof(*m));
    m->size = dlb_pmd_model_combo_query_mem(NULL, NULL);
    m->mem = malloc(m->size);
    if (NULL == m->mem)
    {
        printf("could not allocate memory\n");
        return PMD_FAIL;
    }
    if (dlb_pmd_model_combo_init(&m->model, NULL, NULL, use_common_defs, m->mem))
    {
        printf("could not initialize model\n");
        return PMD_FAIL;
    }

    return PMD_SUCCESS;
}


void
model_finish
    (model *m
    )
{
    if (m->model)
    {
        (void)dlb_pmd_model_combo_destroy(&m->model);
    }
    if (m->mem)
    {
        free(m->mem);
    }
    memset(m, 0, sizeof(*m));
}


dlb_pmd_success
model_populate
    (model      *m
    ,const char *filename
    )
{
    dlb_pmd_bool use_common_defs = PMD_FALSE;   /* TODO: this should come from somewhere else */

    if (filename)
    {
        if (dlb_xmlpmd_file_is_pmd(filename))
        {
            dlb_pmd_model *pmd_model;

            if (dlb_pmd_model_combo_get_writable_pmd_model(m->model, &pmd_model, PMD_FALSE))
            {
                printf("Call to dlb_pmd_model_combo_get_writable_pmd_model() failed\n");
                return PMD_FAIL;
            }
            if (dlb_xmlpmd_file_read(filename, pmd_model, PMD_FALSE, error_callback, NULL))
            {
                printf("XML read file failed: %s\n", dlb_pmd_error(pmd_model));
                return PMD_FAIL;
            }
        }
        else if (dlb_xmlpmd_file_is_sadm(filename))
        {
            if (dlb_pmd_sadm_file_read(filename, m->model, use_common_defs, error_callback, NULL))
            {
                printf("XML read sADM file failed\n");
                return PMD_FAIL;
            }
        }
        else
        {
            printf("Could not determine XML format\n");
            return PMD_FAIL;
        }
    }

    return PMD_SUCCESS;
}


dlb_pmd_success
model_dump
    (model      *m
    ,const char *filename
    )
{
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_bool sadm_out = PMD_FALSE;
    dlb_pmd_success success;

    success = dlb_pmd_model_combo_get_state(m->model, &pmd_model_state, &core_model_state);
    CHECK_SUCCESS(success);
    if (pmd_model_state == DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY)
    {
        sadm_out = PMD_FALSE;
    }
    else if (core_model_state == DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY)
    {
        sadm_out = PMD_TRUE;
    }
    else
    {
        if (pmd_model_state == DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT &&
            core_model_state == DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT)
        {
            success = PMD_FAIL;
        }
        else if (pmd_model_state == DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT)
        {
            sadm_out = PMD_FALSE;
        }
        else if (core_model_state == DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT)
        {
            sadm_out = PMD_TRUE;
        }
        else
        {
            success = PMD_FAIL;
        }
    }
    CHECK_SUCCESS(success);

    if (xml_write(filename, m->model, sadm_out))
    {
        success = PMD_FAIL;
    }

    return success;
}
