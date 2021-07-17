/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2019 by Dolby Laboratories,
 *                Copyright (C) 2016-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_AEN_H
#define PMD_AEN_H


/**
 * @file pmd_aen.h
 * @brief model of Audio Element Names (AEN)
 */


#include "pmd_abd_aod.h"
#include <assert.h>


/**
 * @brief type of an element name
 */
typedef struct
{
    pmd_element_id id;                      /**< audio element identifier */
    uint8_t name[DLB_PMD_NAME_ARRAY_SIZE];  /**< audio element name */
} pmd_aen;
    

#endif /* PMD_AEN_H */
