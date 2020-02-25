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
 * @file model.h
 * @brief helper data struct to encapsulate use of dlb_pmd_model
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include "dlb_pmd_api.h"
#include "dlb_pmd_xml_file.h"
#include "dlb_pmd_sadm_file.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief encapsulate model and its creation
 */
typedef struct
{
    size_t size;
    void *mem;
    dlb_pmd_model *model;
} model;
    

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


/**
 * @brief create a PMD model
 */
static inline
dlb_pmd_success
model_init
    (model *m
    )
{
    m->size = dlb_pmd_query_mem();
    m->mem = malloc(m->size);
    if (NULL == m->mem)
    {
        printf("could not allocate memory\n");
        return PMD_FAIL;
    }
    dlb_pmd_init(&m->model, m->mem);
    return PMD_SUCCESS;
}


/**
 * @brief destroy a PMD model
 */
static inline
void
model_finish
    (model *m
    )
{
    if (m->model)
    {
        dlb_pmd_finish(m->model);
        free(m->mem);
        m->model = NULL;
        m->mem = NULL;
    }
}


/**
 * @brief populate a model from an XML file
 */
static inline
dlb_pmd_success
model_populate
    (model *m
    ,const char *filename
    )

{
    if (filename)
    {
        if (dlb_xmlpmd_file_is_pmd(filename))
        {
            if (dlb_xmlpmd_file_read(filename, m->model, !DLB_PMD_XML_STRICT, error_callback, NULL))
            {
                printf("XML read file failed: %s\n", dlb_pmd_error(m->model));
                return PMD_FAIL;
            }
        }
        else if (dlb_xmlpmd_file_is_sadm(filename))
        {
            if (dlb_pmd_sadm_file_read(filename, m->model, error_callback, NULL))
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


/**
 * @brief dump an XML representation of the model
 */
static inline
dlb_pmd_success
model_dump
    (model *m
    ,const char *filename
    )
{
    if (filename)
    {
        if (dlb_xmlpmd_file_write(filename, m->model))
        {
            printf("XML read file failed: %s\n", dlb_pmd_error(m->model));
            return PMD_FAIL;
        }
    }
    return PMD_SUCCESS;
}


#endif /* __MODEL_H__ */
