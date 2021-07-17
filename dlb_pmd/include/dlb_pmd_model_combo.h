/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_CORE_MODEL_CARRIER_H
#define PMD_CORE_MODEL_CARRIER_H

#include "dlb_pmd_types.h"
#include "dlb_pmd_lib_dll.h"
#include "dlb_adm/include/dlb_adm_fwd_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY,
    DLB_PMD_MODEL_COMBO_STATE_HAS_CONTENT,
    DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED,
    DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY
} DLB_PMD_MODEL_COMBO_STATE;

DLB_PMD_DLL_ENTRY
size_t                  /** @return size of memory to allocate in bytes, or 0 if there was an error */
dlb_pmd_model_combo_query_mem
    (dlb_pmd_model          *existing_pmd_model         /**< [in] pre-existing PMD model instance to use -- may be NULL */
    ,dlb_adm_core_model     *existing_core_model        /**< [in] pre-existing core model instance to use -- may be NULL */
    );

/**
 * @brief Initialize a combo model instance.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_init
    (dlb_pmd_model_combo   **model_combo                /**< [out] model combo instance pointer */
    ,dlb_pmd_model          *existing_pmd_model         /**< [in]  pre-existing PMD model instance to use -- may be NULL */
    ,dlb_adm_core_model     *existing_core_model        /**< [in]  pre-existing core model instance to use -- may be NULL */
    ,dlb_pmd_bool            use_adm_common_defs        /**< [in]  use ADM common definitions */
    ,void                   *memory                     /**< [in]  memory to use for initialization; if NULL, uses malloc() */
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_get_readable_pmd_model
    (const dlb_pmd_model_combo  *model_combo
    ,const dlb_pmd_model       **pmd_model
    ,dlb_pmd_bool                reset_write_state
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_ensure_readable_pmd_model
    (dlb_pmd_model_combo    *model_combo
    ,const dlb_pmd_model   **pmd_model
    ,dlb_pmd_bool            reset_write_state
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_get_writable_pmd_model
    (dlb_pmd_model_combo    *model_combo
    ,dlb_pmd_model         **pmd_model
    ,dlb_pmd_bool            reset_write_state
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_get_readable_core_model
    (const dlb_pmd_model_combo  *model_combo
    ,const dlb_adm_core_model  **core_model
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_ensure_readable_core_model
    (dlb_pmd_model_combo        *model_combo
    ,const dlb_adm_core_model  **core_model
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_get_writable_core_model
    (dlb_pmd_model_combo        *model_combo
    ,dlb_adm_core_model        **core_model
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_convert_to_pmd_model
    (dlb_pmd_model_combo        *model_combo
    ,const char                 *title
    ,const dlb_pmd_model       **pmd_model
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_convert_to_core_model
    (dlb_pmd_model_combo        *model_combo
    ,const dlb_adm_core_model  **core_model
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_has_content
    (dlb_pmd_model_combo    *model_combo
    ,dlb_pmd_bool           *pmd_model_has_content
    ,dlb_pmd_bool           *core_model_has_content
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_get_state
    (dlb_pmd_model_combo        *model_combo
    ,DLB_PMD_MODEL_COMBO_STATE  *pmd_model_state
    ,DLB_PMD_MODEL_COMBO_STATE  *core_model_state
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_use_adm_common_defs
    (const dlb_pmd_model_combo  *model_combo
    ,dlb_pmd_bool               *use_common_defs
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_clear
    (dlb_pmd_model_combo    *model_combo
    );

DLB_PMD_DLL_ENTRY
dlb_pmd_success
dlb_pmd_model_combo_destroy
    (dlb_pmd_model_combo   **model_combo
    );

#ifdef __cplusplus
}
#endif

#endif  /* PMD_CORE_MODEL_CARRIER_H */
