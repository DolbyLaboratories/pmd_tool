/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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

