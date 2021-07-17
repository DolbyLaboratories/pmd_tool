/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2020 by Dolby Laboratories,
 *                Copyright (C) 2018-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file dlb_pmd_generate.h
 * @brief Generate a random model
 */

#ifndef DLB_PMD_GENERATE_H
#define DLB_PMD_GENERATE_H

#include "dlb_pmd_api.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def PMD_GENERATE_RANDOM_NUMBER
 * @brief we specify how many elements of each kind of entity we want.
 * Use this symbolic constant to indicate we want a random number.
 */
#define PMD_GENERATE_RANDOM_NUMBER (~0u)


/**
 * @brief generate a random model with the given number of entities
 *
 * @note this function does not reset the model, it simply adds random
 * data to it.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                      /** @return 0 on success, 1 on failure */
dlb_pmd_generate_random
    (dlb_pmd_model           *model  /**< [in] model to populate randomly */
    ,dlb_pmd_metadata_count  *counts /**< [in] required quantity of each entity.
                                        *      If #PMD_GENERATE_RANDOM_NUMBER is
                                        *      used, generate a random quantity;
                                        *      otherwise, if counts are larger than the
                                        *      largest legal amount, then generate the
                                        *      largest legal amount. Similarly, if
                                        *      counts are lower than the minimum legal
                                        *      amount, generate the minimum.
                                        */
    ,unsigned int seed               /**< [in] seed for internal Pseudo-random
                                      * number generator; use 0 for a random seed.
                                      */
    ,dlb_pmd_bool ascii_strings      /**< [in] if 1, only generate ASCII printable strings */
    ,dlb_pmd_bool sadm               /**< [in] if 1, restrict to generating for
                                      * dlb-serial ADM modesl
                                      */
    );



#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_OAMDI_H */
