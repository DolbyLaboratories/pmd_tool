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
