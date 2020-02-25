/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @file pmd_samd_ingester.h
 * @brief convert an sADM model into a PMD model
 */


#include "pmd_sadm_limits.h"
#include <string.h>


/**
 * @brief abstract type of sADM ingester
 */
typedef struct pmd_sadm_ingester pmd_sadm_ingester;


/**
 * @brief determine memory requirements
 */
size_t                       /** @return size of memory required by ingester in bytes */
pmd_sadm_ingester_query_mem
    (dlb_sadm_counts *sc     /**< [in] model size constraints */
    );


/**
 * @brief initialize the ingester
 */
dlb_pmd_success                /** @return PMD_SUCCESS if ok, PMD_FAIL otherwise */
pmd_sadm_ingester_init
    (pmd_sadm_ingester **cptr  /**< [out] the ingester */
    ,void *mem                 /**< [in] memory to use when initializing */
    ,dlb_sadm_model *sadm      /**< [in] the sADM model */
    );


void
pmd_sadm_ingester_finish
   (pmd_sadm_ingester *c
   );


dlb_pmd_success
pmd_sadm_ingester_ingest
    (pmd_sadm_ingester  *c
    ,const char *title
    ,dlb_pmd_model *pmd
    );



