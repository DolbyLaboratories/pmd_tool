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

#ifndef PMD_CORE_MODEL_GENERATOR_H
#define PMD_CORE_MODEL_GENERATOR_H

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

/**
 * @brief Return the size of memory needed to create a core model generator instance.
 */
TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_generator_query_memory_size
    (size_t                     *sz
    );

/**
 * @brief Open a core model generator instance.
 */
TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_generator_open
    (pmd_core_model_generator  **p_generator
    ,void                       *memory
    );

/**
 * @brief Generate a core model representation of the objects in the PMD model.
 *
 * For best results, #core_model should be empty on entry to this function.
 */
TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_generator_generate
    (pmd_core_model_generator   *generator
    ,dlb_adm_core_model         *core_model
    ,const dlb_pmd_model        *pmd_model
    );

/**
 * @brief Close a core model generator instance.
 */
TEST_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
pmd_core_model_generator_close
    (pmd_core_model_generator  **p_generator
    );

#ifdef __cplusplus
}
#endif

#endif  /* PMD_CORE_MODEL_GENERATOR_H */
