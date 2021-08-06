/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
