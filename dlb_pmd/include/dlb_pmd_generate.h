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
