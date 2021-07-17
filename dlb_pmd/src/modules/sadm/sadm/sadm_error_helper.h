/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file sadm_error_helper.h
 * @brief common helper functions for checking args and setting errors
 */

#ifndef DLB_SADM_ERROR_HELPER_INC_
#define DLB_SADM_ERROR_HELPER_INC_

#include "sadm/dlb_sadm_model.h"

#ifdef __cplusplus
extern "C" {
#endif



#if 0
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
    (const dlb_sadm_model *model    /**< [in] model containing error (if any) */
    )
{
    dlb_sadm_model *m = (dlb_sadm_model*)model;
    m->error[0] = '\0';
}
#endif

/**
 * @def FUNCTION_PROLOGUE(model)
 * @brief common API function prologue
 */
#define FUNCTION_PROLOGUE(model) if (!model) return PMD_FAIL; dlb_sadm_error_reset(model)


/**
 * @brief verify an integer argument
 */
static inline
dlb_pmd_success   /** @return 0 on success, non-zero on failure */
check_intarg
    (const dlb_sadm_model *model
    ,const char *name
    ,uint32_t arg
    ,uint32_t min
    ,uint32_t max
    )
{
    if (arg < min || arg > max)
    {
        dlb_sadm_set_error(model, "argument %s has value %u, which is not in range %u - %u\n",
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
    (const dlb_sadm_model *model
    ,const char *name
    ,const void *arg
    )
{
    if (NULL == arg)
    {
        dlb_sadm_set_error(model, "pointer argument %s is NULL\n", name);
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

#endif /* DLB_SADM_ERROR_HELPER_INC_ */
