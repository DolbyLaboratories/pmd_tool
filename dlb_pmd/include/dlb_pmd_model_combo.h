/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * Copyright (c) 2021, Dolby International AB.
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
