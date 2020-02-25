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
 * @file dlb_sadm_string.h
 * @brief process serial ADM XML embedded within a string
 */

#ifndef DLB_PMD_SADM_STRING_H
#define DLB_PMD_SADM_STRING_H

#include "sadm/dlb_sadm_model.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief type of error callback
 */
typedef
void
(*dlb_sadm_error_callback)
    (const char *msg
    ,void *arg
    );


/**
 * @brief helper routine to write PMD SADM to a string
 */
dlb_pmd_success                /** @return 0 if string written successfully, 1 otherwise */
dlb_sadm_string_write
   (const dlb_sadm_model *model     /**< [in] PMD model struct to write */
   ,char                 *data      /**< [in/out] data buffer to hold written SADM */
   ,size_t               *size      /**< [in/out] in: capacity of buffer, out: size of SADM */
   );



/**
 * @brief helper routine to read and parse PMD SADM from a string
 *
 * Upon parse error, this function will return 1, and the error message
 * can be retrieved via dlb_sadm_error().
 */
dlb_pmd_success                /** @return 0 if string read and parsed successfully, 1 otherwise */
dlb_sadm_string_read
   (const char               *data          /**< [in] data to read */
   ,size_t                    size          /**< [in] length of data */
   ,dlb_sadm_model           *model         /**< [in] PMD model struct to populate */
   ,dlb_sadm_error_callback   err           /**< [in] error callback */
   ,void                     *arg           /**< [in] user-parameter for err callback */
   ,unsigned int             *error_line    /**< [in] error line */
   );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_SADM_STRING_H */
