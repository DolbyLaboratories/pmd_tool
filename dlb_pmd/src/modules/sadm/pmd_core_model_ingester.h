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
