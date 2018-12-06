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
 * @file pmd_profile.h
 * @brief Profile/Level constraints for PMD models
 *
 * PMD profile and levels constrain the number of 'things' that can belong
 * to that instance of a model. This file encapsulates everything relating
 * to these constraints.
 */

#ifndef PMD_PROFILE_H_
#define PMD_PROFILE_H_


#include "dlb_pmd_api.h"
#include <stdio.h>

/**
 * @def MAX_AUDIO_ELEMENTS
 * @brief upper limit on the number of audio elements we can hold
 */
#define MAX_AUDIO_ELEMENTS DLB_PMD_MAX_AUDIO_ELEMENTS


/**
 * @def MAX_UPDATES
 * @brief upper limit of update metadata within a single video frame
 *
 * Current spec indicates a resolution of 240 Hz for dynamic object
 * metadata.  The PMD spec delivers the entire audio
 * object/presentation at the start of every video frame.  Therefore
 * updates are only valid for the duration of a frame.  The slowest
 * frame rate is 23.98 fps, which means there can be up to 10 updates
 * per video frame, and a max number of 14 objects to update.
 */
#define MAX_UPDATES DLB_PMD_MAX_UPDATES


/**
 * @def MAX_PRESENTATIONS
 * @brief upper limit on the number of audio presentations we can hold
 */
#define MAX_PRESENTATIONS DLB_PMD_MAX_PRESENTATIONS


/**
 * @def MAX_PRESENTATION_ELEMENTS
 * @brief upper limit on number of audio elements in one presentation
 */
#define MAX_PRESENTATION_ELEMENTS (63)


/**
 * @def MAX_EAC3_ENCODING_PARAMETERS
 * @brief upper limit up EAC3 encoding parameters structs within single video frame
 */
#define MAX_EAC3_ENCODING_PARAMETERS DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS


/**
 * @def MAX_ED2_TURNAROUNDS
 * @brief upper limit up ED2 turnaround descriptions within single video frame
 */
#define MAX_ED2_TURNAROUNDS DLB_PMD_MAX_ED2_TURNAROUNDS


/**
 * @def MAX_PROFILE_NUMBER
 * @brief maximum allowed profile number 
 */
#define MAX_PROFILE_NUMBER (65535)

/**
 * @def MAX_PROFILE_LEVEL
 * @brief maximum allowed profile level
 */
#define MAX_PROFILE_LEVEL  (65535)


/**
 * @brief represent profile information in a model
 */
typedef struct
{
    uint16_t profile_number;               /**< current profile number, 0 for none */
    uint16_t profile_level;                /**< current profile level, 0 for none */
    unsigned int max_elements;             /**< max number of beds + objects */
    dlb_pmd_metadata_count constraints;    /**< repurpose count struct to mean 'max' count */
} pmd_profile;
    

/**
 * @brief initialize/reset profile constraints
 */
static inline
void
pmd_profile_init
    (pmd_profile *p          /**< [in] profile struct to initialize */
    )
{
    p->profile_number                 = 0;
    p->profile_level                  = 0;
    p->max_elements                   = MAX_AUDIO_ELEMENTS;
    p->constraints.num_signals        = DLB_PMD_MAX_SIGNALS;
    p->constraints.num_beds           = MAX_AUDIO_ELEMENTS;
    p->constraints.num_objects        = MAX_AUDIO_ELEMENTS;
    p->constraints.num_updates        = MAX_UPDATES;
    p->constraints.num_presentations  = MAX_PRESENTATIONS;
    p->constraints.num_loudness       = MAX_PRESENTATIONS;
    p->constraints.num_iat            = 1;
    p->constraints.num_eac3           = MAX_EAC3_ENCODING_PARAMETERS;
    p->constraints.num_ed2_turnarounds= MAX_ED2_TURNAROUNDS;
    p->constraints.num_headphone_desc = DLB_PMD_MAX_HEADPHONE;
}


/**
 * @brief set a profile 0 constraint
 */
static inline
dlb_pmd_success                  /** @return 0 if successful, 1 otherwise */
pmd_profile_0_set
    (pmd_profile *p              /**< [in] profile struct to set */
    ,unsigned int profile_level  /**< [in] profile level */
    )
{
    if (0 == profile_level)
    {
        pmd_profile_init(p);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


/**
 * @brief set a profile 1 constraint
 */
static inline
dlb_pmd_success                  /** @return 0 if successful, 1 otherwise */
pmd_profile_1_set
    (pmd_profile *p              /**< [in] profile struct to set */
    ,unsigned int profile_level  /**< [in] profile level */
    )
{
    switch (profile_level)
    {
        case 1:
            pmd_profile_init(p);
            p->max_elements                  = 10;
            p->constraints.num_signals       = 16;
            p->constraints.num_beds          = 10;
            p->constraints.num_objects       = 10;
            p->constraints.num_presentations = 8;
            return PMD_SUCCESS;

        case 2:
            pmd_profile_init(p);
            p->max_elements                  = 20;
            p->constraints.num_signals       = 16;
            p->constraints.num_beds          = 20;
            p->constraints.num_objects       = 20;
            p->constraints.num_presentations = 16;
            return PMD_SUCCESS;

        case 3:
            pmd_profile_init(p);
            p->max_elements                  = 50;
            p->constraints.num_signals       = 16;
            p->constraints.num_beds          = 50;
            p->constraints.num_objects       = 50;
            p->constraints.num_presentations = 48;
            return PMD_SUCCESS;

        default:
            return PMD_FAIL;
    }
}


/**
 * @brief set a profile constraint
 */
static inline
dlb_pmd_success                  /** @return 0 if successful, 1 otherwise */
pmd_profile_set
    (pmd_profile *p              /**< [in] profile struct to set */
    ,unsigned int profile_number /**< [in] profile number */
    ,unsigned int profile_level  /**< [in] profile level */
    )
{
    switch (profile_number)
    {
        case 0:   return pmd_profile_0_set(p, profile_level);
        case 1:   return pmd_profile_1_set(p, profile_level);
        default:  return 1;
    }
}


/**
 * @brief helper function to add profiling info (if it exists) to error message
 */
static inline
void
pmd_profile_error_info
    (pmd_profile *p
    ,char *str
    ,size_t capacity
    )
{
    *str = '\0';
    if (p->profile_number)
    {
        snprintf(str, capacity, "in profile %u, level %u", p->profile_number, p->profile_level);
    }
}


#endif /* PMD_PROFILE_H_ */
