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

#include <stdlib.h>


#define CHECK_STATUS(S) if ((S) != DLB_ADM_STATUS_OK) goto finish
#define CHECK_SUCCESS(S) if ((S) != PMD_SUCCESS) goto finish
#define CHECK_NULL(P) if ((P) == NULL) goto finish


/** ------------------------------ public API ------------------------- */


dlb_pmd_success
dlb_pmd_sadm_file_write
   (const char              *filename
   ,dlb_pmd_model_combo     *model
   )
{
    dlb_pmd_success ultimate_success = PMD_FAIL;
    const dlb_adm_core_model *core_model = NULL;
    dlb_adm_xml_container *container = NULL;
    dlb_pmd_success success;
    int status;

    success = dlb_pmd_model_combo_ensure_readable_core_model(model, &core_model);
    CHECK_SUCCESS(success);
    status = dlb_adm_container_open_from_core_model(&container, core_model);
    CHECK_STATUS(status);
    status = dlb_adm_container_write_xml_file(container, filename);
    CHECK_STATUS(status);

    ultimate_success = PMD_SUCCESS;

finish:
    if (container != NULL)
    {
        (void)dlb_adm_container_close(&container);
    }

    return ultimate_success;
}

