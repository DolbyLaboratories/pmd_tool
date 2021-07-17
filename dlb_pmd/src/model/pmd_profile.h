/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2020 by Dolby Laboratories,
 *                Copyright (C) 2018-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
#include <string.h>


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
 * @def MAX_PRESENTATION_NAMES
 * @brief max allowed presentation names
 */
#define MAX_PRESENTATION_NAMES (MAX_PRESENTATIONS * DLB_PMD_MAX_PRESENTATION_NAMES)


/**
 * @brief represent profile information in a model
 */
typedef struct
{
    unsigned int profile_number;           /**< current profile number, 0 for none */
    unsigned int profile_level;            /**< current profile level, 0 for none */
    dlb_pmd_model_constraints constraints; /**< model entity constraints */
} pmd_profile;
    

/**
 * @brief initialize/reset profile constraints
 */
static inline
void
pmd_profile_max
    (pmd_profile *p               /**< [in] profile struct to initialize */
    )
{
    p->constraints.max_elements           = MAX_AUDIO_ELEMENTS;
    p->constraints.max_presentation_names = MAX_PRESENTATION_NAMES;
    p->constraints.max.num_signals        = DLB_PMD_MAX_SIGNALS;
    p->constraints.max.num_beds           = MAX_AUDIO_ELEMENTS;
    p->constraints.max.num_objects        = MAX_AUDIO_ELEMENTS;
    p->constraints.max.num_updates        = MAX_UPDATES;
    p->constraints.max.num_presentations  = MAX_PRESENTATIONS;
    p->constraints.max.num_loudness       = MAX_PRESENTATIONS;
    p->constraints.max.num_iat            = 1;
    p->constraints.max.num_ed2_system     = 1;
    p->constraints.max.num_eac3           = MAX_EAC3_ENCODING_PARAMETERS;
    p->constraints.max.num_ed2_turnarounds= MAX_ED2_TURNAROUNDS;
    p->constraints.max.num_headphone_desc = DLB_PMD_MAX_HEADPHONE;
    p->constraints.use_adm_common_defs    = PMD_FALSE;
}


/**
 * @brief initialize/reset profile constraints
 */
static inline
void
pmd_profile_init
    (pmd_profile *p               /**< [in] profile struct to initialize */
    ,dlb_pmd_model_constraints *c /**< [in] max model constraints */
    )
{
    p->profile_number = 0;
    p->profile_level  = 0;
    if (c)
    {
        p->constraints = *c;
    }
    else
    {
        pmd_profile_max(p);
    }
}


/**
 * @brief set a profile 0 constraint
 */
static inline
dlb_pmd_success                     /** @return 0 if successful, 1 otherwise */
pmd_profile_0_set
    (pmd_profile *p                 /**< [in] profile struct to set */
    ,unsigned int profile_level     /**< [in] profile level */
    ,dlb_pmd_model_constraints *max /**< [in] max model constraints */
    )
{
    if (0 == profile_level)
    {
        pmd_profile_init(p, max);
        p->profile_number = 0;
        p->profile_level = profile_level;
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


/**
 * @brief set a profile 1 constraint
 */
static inline
dlb_pmd_success                     /** @return 0 if successful, 1 otherwise */
pmd_profile_1_set
    (pmd_profile *p                 /**< [in] profile struct to set */
    ,unsigned int profile_level     /**< [in] profile level */
    ,dlb_pmd_model_constraints *max /**< [in] max model constraints */
    )
{
    pmd_profile candidate;

    switch (profile_level)
    {
        case 1:
            pmd_profile_init(&candidate, max);
            candidate.constraints.max_elements          = 10;
            candidate.constraints.max.num_signals       = 16;
            candidate.constraints.max.num_beds          = 10;
            candidate.constraints.max.num_objects       = 10;
            candidate.constraints.max.num_presentations = 8;
            break;

        case 2:
            pmd_profile_init(&candidate, max);
            candidate.constraints.max_elements          = 20;
            candidate.constraints.max.num_signals       = 16;
            candidate.constraints.max.num_beds          = 20;
            candidate.constraints.max.num_objects       = 20;
            candidate.constraints.max.num_presentations = 16;
            break;

        case 3:
            pmd_profile_init(&candidate, max);
            candidate.constraints.max_elements          = 50;
            candidate.constraints.max.num_signals       = 16;
            candidate.constraints.max.num_beds          = 50;
            candidate.constraints.max.num_objects       = 50;
            candidate.constraints.max.num_presentations = 48;
            break;

        default:
            return PMD_FAIL;
    }

    if (   candidate.constraints.max_elements    > max->max_elements
        || candidate.constraints.max.num_signals > max->max.num_signals
        || candidate.constraints.max.num_beds    > max->max.num_beds
        || candidate.constraints.max.num_objects > max->max.num_objects
        || candidate.constraints.max.num_presentations > max->max.num_presentations)
    {
        /* profile too large for the model's memory constraints */
        return PMD_FAIL;
    }
    memmove(&p->constraints, &candidate.constraints, sizeof(candidate.constraints));
    p->profile_number = 1;
    p->profile_level = profile_level;
    return PMD_SUCCESS;
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
    ,dlb_pmd_model_constraints *max /**< [in] max model constraints */
    )
{
    switch (profile_number)
    {
        case 0:   return pmd_profile_0_set(p, profile_level, max);
        case 1:   return pmd_profile_1_set(p, profile_level, max);
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
