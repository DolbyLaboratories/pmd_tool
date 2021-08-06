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

#ifndef PMD_XYZ_H
#define PMD_XYZ_H

/**
 * @file pmd_xyz.h
 * @brief definitions relating to the Dynamic Update payload (XYZ)
 */


#include "pmd_abd_aod.h"

#include <assert.h>


/**
 * @brief type of PMD audio object update
 *
 * In PMD, the audio objects and presentations are always delivered in
 * entirety at the start of a video frame boundary.  However, the
 * PMD resolution is much higher than frames, allowing object positional
 * updates within the duration of a video frame.
 *
 * This structure details one such positional update for one object.
 */
typedef struct
{
    unsigned int time;          /**< num 32-sample blocks after video sync at which this
                                  *  update applies */
    pmd_element_id obj_idx;     /**< audio object index (within model object array) */
    uint8_t        clusterbin;  /**< working info for OAMDI clustering */
    pmd_position x;             /**< new X position */
    pmd_position y;             /**< new Y position */
    pmd_position z;             /**< new Z position */
} pmd_xyz;
    

/**
 * @def XYZ_BLOCK_SIZE
 * @brief number of samples in one unit of PMD update time 
 */
#define XYZ_BLOCK_SIZE (32)


/**
 * @brief helper function to convert a sample offset into an internal 32-block time
 */
static inline
unsigned int
pmd_xyz_encode_time
    (unsigned int sample_offset
    )
{
    return sample_offset / XYZ_BLOCK_SIZE;
}


/**
 * @brief helper function to convert a sample offset into an internal 32-block time
 */
static inline
unsigned int
pmd_xyz_decode_time
    (unsigned int time
    )
{
    return time * XYZ_BLOCK_SIZE;
}


#endif /* PMD_XYZ_H */
