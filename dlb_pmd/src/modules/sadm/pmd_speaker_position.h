/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#ifndef DLB_PMD_SPEAKER_POSITION_H
#define DLB_PMD_SPEAKER_POSITION_H

#include "dlb_pmd_api.h"
#include "dlb_adm/include/dlb_adm_data_types.h"

#include <math.h>

/*
 * symbolic constants used to convert mathematical speaker position
 * coordinates to symbolic coordinates
 */

static const float CARTESIAN_LEFT     =  -1.0f;
static const float CARTESIAN_CENTER   =   0.0f;
static const float CARTESIAN_RIGHT    =   1.0f;

static const float CARTESIAN_FRONT    =   1.0f;
static const float CARTESIAN_WIDE     =   0.677f;
static const float CARTESIAN_MID_SIDE =   0.0f;
static const float CARTESIAN_REAR     =  -1.0f;

static const float CARTESIAN_MIDDLE   =   0.0f;

static const float SPHERICAL_CENTER   =   0.0f;

static const float SPHERICAL_FRONT    =  30.0f;
static const float SPHERICAL_WIDE     =  60.0f;
static const float SPHERICAL_MID_SIDE =  90.0f;
static const float SPHERICAL_REAR     = 110.0f;

static const float SPHERICAL_MIDDLE   =   0.0f;

/**
 * @brief enumeration of symbolic speaker position coordinates
 */
typedef enum
{
    SPEAKER_COORD_NONE,
    /* Left to Right */
    SPEAKER_COORD_LEFT,
    SPEAKER_COORD_CENTER,
    SPEAKER_COORD_RIGHT,
    /* Front to Back */
    SPEAKER_COORD_FRONT,
    SPEAKER_COORD_WIDE,
    SPEAKER_COORD_MID_SIDE,
    SPEAKER_COORD_REAR,
    /* Height */
    SPEAKER_COORD_LOW,
    SPEAKER_COORD_MIDDLE,
    SPEAKER_COORD_HIGH
} SPEAKER_COORD;

/**
 * @brief a speaker position described by three symbolic coordinate values
 */
typedef struct
{
    SPEAKER_COORD   left_to_right;
    SPEAKER_COORD   front_to_back;
    SPEAKER_COORD   height;
} speaker_position;

/**
 * @brief a PMD speaker position described by its three symbolic coordinate values
 */
typedef struct
{
    speaker_position    coords;
    dlb_pmd_speaker     pmd_speaker;
} speaker_table_entry;

/**
 * @brief the PMD speaker positions described using symbolic coordinate values, sorted
 * in ascending order
 */
static speaker_table_entry SPEAKER_TABLE[] =
{
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_FRONT,    SPEAKER_COORD_LOW},    PMD_SPEAKER_LFE},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_FRONT,    SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_L},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_FRONT,    SPEAKER_COORD_HIGH},   PMD_SPEAKER_LTF},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_WIDE,     SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_LFW},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_MID_SIDE, SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_LS},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_MID_SIDE, SPEAKER_COORD_HIGH},   PMD_SPEAKER_LTM},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_REAR,     SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_LRS},
    {{SPEAKER_COORD_LEFT,   SPEAKER_COORD_REAR,     SPEAKER_COORD_HIGH},   PMD_SPEAKER_LTR},
    {{SPEAKER_COORD_CENTER, SPEAKER_COORD_FRONT,    SPEAKER_COORD_LOW},    PMD_SPEAKER_LFE},
    {{SPEAKER_COORD_CENTER, SPEAKER_COORD_FRONT,    SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_C},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_FRONT,    SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_R},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_FRONT,    SPEAKER_COORD_HIGH},   PMD_SPEAKER_RTF},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_WIDE,     SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_RFW},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_MID_SIDE, SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_RS},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_MID_SIDE, SPEAKER_COORD_HIGH},   PMD_SPEAKER_RTM},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_REAR,     SPEAKER_COORD_MIDDLE}, PMD_SPEAKER_RRS},
    {{SPEAKER_COORD_RIGHT,  SPEAKER_COORD_REAR,     SPEAKER_COORD_HIGH},   PMD_SPEAKER_RTR},
};


/**
 * @brief the number of entries in #SPEAKER_TABLE
 */
static const size_t SPEAKER_TABLE_SIZE = (sizeof(SPEAKER_TABLE) / sizeof(speaker_table_entry));

/**
 * @brief given a set of symbolic coordinates, find the corresponding entry in the
 * table (if there is one)
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS if found, PMD_FAIL if not */
lookup_speaker_position
    (dlb_pmd_speaker            *pmd_speaker    /**< [out] PMD speaker position */
    ,const speaker_position     *position       /**< [in]  symbolic coordinate set */
    ,dlb_pmd_bool                alt_spkrs      /**< [in]  use alternative surround speakers? */
    )
{
    dlb_pmd_success success = PMD_FAIL;
    size_t i = 0;

    if ((pmd_speaker == NULL) || (position == NULL))
    {
        return PMD_FAIL;
    }
    *pmd_speaker = PMD_SPEAKER_NULL;

    while (SPEAKER_TABLE[i].coords.left_to_right < position->left_to_right)
    {
        if (++i >= SPEAKER_TABLE_SIZE)
        {
            return PMD_FAIL;
        }
    }

    while (SPEAKER_TABLE[i].coords.front_to_back < position->front_to_back)
    {
        if (++i >= SPEAKER_TABLE_SIZE)
        {
            return PMD_FAIL;
        }
    }

    while (SPEAKER_TABLE[i].coords.height < position->height)
    {
        if (++i >= SPEAKER_TABLE_SIZE)
        {
            return PMD_FAIL;
        }
    }

    if ((position->left_to_right == SPEAKER_TABLE[i].coords.left_to_right) &&
        (position->front_to_back == SPEAKER_TABLE[i].coords.front_to_back) &&
        (position->height        == SPEAKER_TABLE[i].coords.height))
    {
        dlb_pmd_speaker s = SPEAKER_TABLE[i].pmd_speaker;

#if DLB_PMD_USE_ALT_SPKRS
        /* !alt_spkrs means 2.0 - 5.1.4 (i.e., no rear surrounds)
         * when no rear surrounds, Ls and Rs take the Lrs and Rrs
         * speaker positions, plus shorter names
         */
        if (!alt_spkrs)
        {
            switch (s)
            {
            case PMD_SPEAKER_LRS:
                s = PMD_SPEAKER_LS;
                break;
            case PMD_SPEAKER_RRS:
                s = PMD_SPEAKER_RS;
                break;
            default:
                break;
            }
        }
#else
        (void)alt_spkrs;
#endif
        *pmd_speaker = s;

        success = PMD_SUCCESS;
    }

    return success;
}

/**
 * @brief does the given mathematical coordinate correspond to a "wide"
 * speaker position?
 */
static
dlb_pmd_bool                    /** @return PMD_TRUE if it is "wide", PMD_FALSE if not */
is_wide
    (float          y           /**< [in] mathematical coordinate */
    ,dlb_pmd_bool   cartesian   /**< [in] is it cartesian rather than spherical? */
    )
{
    dlb_pmd_bool w = PMD_FALSE;

    if (cartesian)
    {
        if (y == CARTESIAN_WIDE)
        {
            w = PMD_TRUE;
        }
        else if (y < CARTESIAN_FRONT && y > CARTESIAN_MID_SIDE)
        {
            double delta_wide = fabs(CARTESIAN_WIDE - y);
            double delta_front = fabs(CARTESIAN_FRONT - y);
            double delta_side = fabs(CARTESIAN_MID_SIDE - y);

            w = ((delta_wide < delta_front) && (delta_wide < delta_side));
        }
    } 
    else
    {
        w = (fabs(y) == SPHERICAL_WIDE);
    }

    return w;
}

/**
 * @brief analyze the mathematical coordinates in #update and convert them
 * to symbolic coordinates
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS if found, PMD_FAIL if not */
analyze_coordinates
    (speaker_position                   *position   /**< [out] set of symbolic coordinates */
    ,const dlb_adm_data_block_update    *update     /**< [in]  block update record with coordinates to analyze */
    )
{
    dlb_pmd_success l2r_success = PMD_FAIL;
    dlb_pmd_success f2b_success = PMD_FAIL;
    dlb_pmd_success hgt_success = PMD_FAIL;

    if ((position == NULL) || (update == NULL))
    {
        return PMD_FAIL;
    }
    memset(position, 0, sizeof(*position));

    if (update->cartesian)
    {
        float x = update->position[DLB_ADM_COORDINATE_X];
        float y = update->position[DLB_ADM_COORDINATE_Y];
        float z = update->position[DLB_ADM_COORDINATE_Z];

        /* left to right */
        if (x == CARTESIAN_LEFT)
        {
            position->left_to_right = SPEAKER_COORD_LEFT;
            l2r_success = PMD_SUCCESS;
        }
        else if (x == CARTESIAN_CENTER)
        {
            position->left_to_right = SPEAKER_COORD_CENTER;
            l2r_success = PMD_SUCCESS;
        }
        else if (x == CARTESIAN_RIGHT)
        {
            position->left_to_right = SPEAKER_COORD_RIGHT;
            l2r_success = PMD_SUCCESS;
        }

        /* front to back */
        if (y == CARTESIAN_FRONT)
        {
            position->front_to_back = SPEAKER_COORD_FRONT;
            f2b_success = PMD_SUCCESS;
        }
        else if (is_wide(y, PMD_TRUE))
        {
            position->front_to_back = SPEAKER_COORD_WIDE;
            f2b_success = PMD_SUCCESS;
        }
        else if (y == CARTESIAN_MID_SIDE)
        {
            position->front_to_back = SPEAKER_COORD_MID_SIDE;
            f2b_success = PMD_SUCCESS;
        }
        else if (y == CARTESIAN_REAR)
        {
            position->front_to_back = SPEAKER_COORD_REAR;
            f2b_success = PMD_SUCCESS;
        }

        /* height */
        if (z < CARTESIAN_MIDDLE)
        {
            position->height = SPEAKER_COORD_LOW;
            hgt_success = PMD_SUCCESS;
        }
        else if (z > CARTESIAN_MIDDLE)
        {
            position->height = SPEAKER_COORD_HIGH;
            hgt_success = PMD_SUCCESS;
        }
        else
        {
            position->height = SPEAKER_COORD_MIDDLE;
            hgt_success = PMD_SUCCESS;
        }
    }
    else /* spherical */
    {
        float elevation = update->position[DLB_ADM_COORDINATE_ELEVATION];
        float azimuth = update->position[DLB_ADM_COORDINATE_AZIMUTH];
        float abs_azimuth = (float)fabs(azimuth);

        /* left to right */
        if (azimuth > SPHERICAL_CENTER)
        {
            position->left_to_right = SPEAKER_COORD_LEFT;
            l2r_success = PMD_SUCCESS;
        }
        else if (azimuth < SPHERICAL_CENTER)
        {
            position->left_to_right = SPEAKER_COORD_RIGHT;
            l2r_success = PMD_SUCCESS;
        }
        else
        {
            position->left_to_right = SPEAKER_COORD_CENTER;
            l2r_success = PMD_SUCCESS;
        }

        /* front to back */
        if (abs_azimuth <= SPHERICAL_FRONT)
        {
            position->front_to_back = SPEAKER_COORD_FRONT;
            f2b_success = PMD_SUCCESS;
        }
        else if (is_wide(azimuth, PMD_FALSE))
        {
            position->front_to_back = SPEAKER_COORD_WIDE;
            f2b_success = PMD_SUCCESS;
        }
        else if (abs_azimuth == SPHERICAL_MID_SIDE)
        {
            position->front_to_back = SPEAKER_COORD_MID_SIDE;
            f2b_success = PMD_SUCCESS;
        }
        else if (abs_azimuth >= SPHERICAL_REAR)
        {
            position->front_to_back = SPEAKER_COORD_REAR;
            f2b_success = PMD_SUCCESS;
        }

        /* height */
        if (elevation < SPHERICAL_MIDDLE)
        {
            position->height = SPEAKER_COORD_LOW;
            hgt_success = PMD_SUCCESS;
        }
        else if (elevation > SPHERICAL_MIDDLE)
        {
            position->height = SPEAKER_COORD_HIGH;
            hgt_success = PMD_SUCCESS;
        }
        else
        {
            position->height = SPEAKER_COORD_MIDDLE;
            hgt_success = PMD_SUCCESS;
        }
    }

    return (l2r_success || f2b_success || hgt_success);
}

/**
 * @brief find the corresponding PMD speaker position from the mathematical coordinates in #update
 */
static
dlb_pmd_success                 /** @return PMD_SUCCESS if found, PMD_FAIL if not */
find_speaker_position
    (dlb_pmd_speaker                    *pmd_speaker    /**< [out] PMD speaker position */
    ,const dlb_adm_data_block_update    *update         /**< [in]  block update record with coordinates to analyze */
    ,dlb_pmd_bool                        alt_spkrs      /**< [in]  use alternative surround speakers? */
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    speaker_position position;

    if (analyze_coordinates(&position, update) ||
        lookup_speaker_position(pmd_speaker, &position, alt_spkrs))
    {
        success = PMD_FAIL;
    }

    return success;
}


#endif  /* DLB_PMD_SPEAKER_POSITION_H */
