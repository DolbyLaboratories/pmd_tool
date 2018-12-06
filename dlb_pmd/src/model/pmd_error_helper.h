/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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
 * @file pmd_error_helper.h
 * @brief common helper functions for checking args and setting errors
 */

#ifndef DLB_PMD_ERROR_HELPER_INC_
#define DLB_PMD_ERROR_HELPER_INC_

#include "pmd_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief auxiliary function to record an error string
 *
 * @note that even for const models, the error field is mutable 
 */
static inline
void
error
    (const dlb_pmd_model *model /**< [in] model */
    ,const char *fmt            /**< [in] error message format */
    ,...                        /**< [in] error message varargs */
    )
{
    dlb_pmd_model *m = (dlb_pmd_model*)model;
    va_list  args;

    va_start(args, fmt);
    (void)vsnprintf(m->error, sizeof(m->error), fmt, args);
    va_end(args);
}


/**
 * @brief helper to remove error information, if any
 *
 * @note we use the first byte of the error string to indicate whether
 * an error is present or not.
 *
 * @note that even for const models, the error field is mutable 
 */
static inline
void
error_reset
    (const dlb_pmd_model *model    /**< [in] model containing error (if any) */
    )
{
    dlb_pmd_model *m = (dlb_pmd_model*)model;
    m->error[0] = '\0';
}


/**
 * @def FUNCTION_PROLOGUE(model)
 * @brief common API function prologue
 */
#define FUNCTION_PROLOGUE(model) if (!model) return PMD_FAIL; error_reset(model)


/**
 * @brief verify an integer argument
 */
static inline
dlb_pmd_success   /** @return 0 on success, non-zero on failure */
check_intarg
    (const dlb_pmd_model *model
    ,const char *name
    ,uint32_t arg
    ,uint32_t min
    ,uint32_t max
    )
{
    if (arg < min || arg > max)
    {
        error(model, "argument %s has value %u, which is not in range %u - %u\n",
              name, arg, min, max);
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief verify a pointer argument
 */
static inline
dlb_pmd_success   /** @return 0 on success, non-zero on failure */
check_ptrarg
    (const dlb_pmd_model *model
    ,const char *name
    ,const void *arg
    )
{
    if (NULL == arg)
    {
        error(model, "pointer argument %s is NULL\n", name);
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


/**
 * @def CHECK_INTARG(model,arg,min,max)
 * @brief verify an integer argument is within limits
 */
#define CHECK_INTARG(m,arg,min,max)                                     \
    if (check_intarg(m,#arg,(uint32_t)arg,(uint32_t)min, (uint32_t)max)) return PMD_FAIL
    

/**
 * @def CHECK_PTRARG(model,ptr)
 * @brief verify a pointer is valid
 */
#define CHECK_PTRARG(m,arg) \
    if (check_ptrarg(m,#arg,arg)) return PMD_FAIL


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_ERROR_HELPER_INC_ */
