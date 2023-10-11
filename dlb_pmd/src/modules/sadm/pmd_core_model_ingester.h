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

#ifndef PMD_CORE_MODEL_INGESTER_H
#define PMD_CORE_MODEL_INGESTER_H

#include "dlb_adm/include/dlb_adm_fwd_type.h"
#include "dlb_pmd_api.h"
#include "pmd_core_model_types.h"

#ifdef TEST_DLL_ENTRY
#undef TEST_DLL_ENTRY
#endif
#define TEST_DLL_ENTRY DLB_PMD_DLL_ENTRY

#ifdef __cplusplus
extern "C" {
#endif

TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_ingester_query_memory_size
    (size_t                     *sz
    );

TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_ingester_open
    (pmd_core_model_ingester   **p_ingester
    ,void                       *memory
    );

/**
 * @brief generate a PMD model from the objects in the core model.
 */
TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_ingester_ingest
    (pmd_core_model_ingester    *ingester
    ,dlb_pmd_model              *pmd_model
    ,const char                 *title
    ,const dlb_adm_core_model   *core_model
    );

TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_ingester_close
    (pmd_core_model_ingester   **p_ingester
    );

#ifdef __cplusplus
}
#endif

#endif  /* PMD_CORE_MODEL_INGESTER_H */
