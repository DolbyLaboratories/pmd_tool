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


#include "dlb_pmd_sadm_file.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_pmd_model_combo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK_NULL(s, m)    if ((s) == NULL)              { error_msg = (m); goto finish; }
#define CHECK_STATUS(s, m)  if ((s) != DLB_ADM_STATUS_OK) { error_msg = (m); goto finish; }
#define CHECK_SUCCESS(s, m) if ((s) != PMD_SUCCESS)       { error_msg = (m); goto finish; }

#ifdef DIRECTORY_SEPARATOR_CHAR
#undef DIRECTORY_SEPARATOR_CHAR
#endif
#ifdef _WIN32
#define DIRECTORY_SEPARATOR_CHAR '\\'
#else
#define DIRECTORY_SEPARATOR_CHAR '/'
#endif

static
const char *
remove_path
   (const char *filename
   )
{
    size_t len = strlen(filename);
    const char *p = filename + len;

    while (*p != DIRECTORY_SEPARATOR_CHAR)
    {
        if (p == filename)
        {
            break;
        }
        p--;
    }

    if (*p == DIRECTORY_SEPARATOR_CHAR)
    {
        p++;
    }

    return p;
}


/** ------------------------------ public API ------------------------- */


dlb_pmd_success
dlb_pmd_sadm_file_read
   (const char                  *filename
   ,dlb_pmd_model_combo         *model
   ,dlb_pmd_bool                 use_common_defs
   ,dlb_pmd_sadm_error_callback  error_callback
   ,void                        *error_callback_arg
   )
{
    dlb_pmd_success              ultimate_success = PMD_FAIL;
    dlb_pmd_success              success;
    const char                  *error_msg = NULL;
    dlb_adm_container_counts     counts;
    dlb_adm_container_counts     flattened_counts;
    dlb_adm_xml_container       *container = NULL;
    dlb_adm_xml_container       *flattened_container = NULL;
    dlb_adm_core_model          *core_model;
    int                          status;

    memset(&counts, 0, sizeof(counts));
    memset(&flattened_counts, 0, sizeof(flattened_counts));

    status = dlb_adm_container_open(&container, &counts);
    CHECK_STATUS(status, "dlb_pmd_sadm_file_read(): failed to open XML container");
    status = dlb_adm_container_open(&flattened_container, &flattened_counts);
    CHECK_STATUS(status, "dlb_pmd_sadm_file_read(): failed to open XML container");

    status = dlb_adm_container_read_xml_file(container, filename, use_common_defs);
    CHECK_STATUS(status, "dlb_pmd_sadm_file_read(): failed to read XML file");

    status = dlb_adm_container_flatten(container, flattened_container);
    CHECK_STATUS(status, "dlb_adm_container_flatten(): failed to flatten XML container");

    success = dlb_pmd_model_combo_get_writable_core_model(model, &core_model);
    CHECK_SUCCESS(success, "dlb_pmd_sadm_file_read(): could not get writable core model");
    
    status = dlb_adm_core_model_ingest_xml_container(core_model, flattened_container);
    CHECK_STATUS(status, "dlb_pmd_sadm_file_read(): failed to ingest core model from XML container");

    status = dlb_adm_container_close(&container);
    CHECK_STATUS(status, "dlb_pmd_sadm_file_read(): failed to close XML container");
    status = dlb_adm_container_close(&flattened_container);
    CHECK_STATUS(status, "dlb_pmd_sadm_file_read(): failed to close XML container");

    ultimate_success = PMD_SUCCESS;

finish:
    if ((error_msg != NULL) && (error_callback != NULL))
    {
        (*error_callback)(error_msg, error_callback_arg);
    }
    if (container != NULL)
    {
        (void)dlb_adm_container_close(&container);
    }
    if (flattened_container != NULL)
    {
        (void)dlb_adm_container_close(&flattened_container);
    }

    return ultimate_success;
}


dlb_pmd_bool
dlb_xmlpmd_file_is_sadm
     (const char *filename
     )
{
    dlb_pmd_bool is_sadm = PMD_FALSE;

    (void)dlb_adm_file_is_sadm_xml(&is_sadm, filename);

    return is_sadm;
}
