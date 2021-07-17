/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2019 by Dolby Laboratories,
 *                Copyright (C) 2018-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_XYZ_SET_INC_
#define PMD_XYZ_SET_INC_

#include <string.h>


/**
 * @brief datatype representing the list of written updates
 *
 * bit positions correspond to indices in the model's update array 
 */
typedef struct
{
    uint8_t bitmap[(MAX_UPDATES+7) / 8];
} pmd_xyz_set;


/**
 * @brief initialize signals datatype
 */
static inline
void
pmd_xyz_set_init
    (pmd_xyz_set *set
    )
{
    memset(set->bitmap, '\0', sizeof(set->bitmap));
}


/**
 * @brief copy set
 */
static inline
void
pmd_xyz_set_copy
    (pmd_xyz_set *dest
    ,pmd_xyz_set *src
    )
{
    memcpy(dest, src, sizeof(src->bitmap));
}


/**
 * @brief add an index to the list of known set
 */
static inline
void
pmd_xyz_set_add
    (pmd_xyz_set *set
    ,unsigned int idx
    )
{
    set->bitmap[(idx)/8] |= (1<<((idx) & 7));
}


/**
 * @brief remove an index from the list of known set
 */
static inline
void
pmd_xyz_set_remove
    (pmd_xyz_set *set
    ,unsigned int idx
    )
{
    set->bitmap[(idx)/8] &= ~(1<<((idx) & 7));
}


/**
 * @brief check whether or not a given index is in the set
 */
static inline
pmd_bool
pmd_xyz_set_test
    (pmd_xyz_set *set
    ,unsigned int idx
    )
{
    return set->bitmap[(idx)/8] & (1<<((idx) & 7));
}


#endif /* PMD_XYZ_SET_INC_ */
