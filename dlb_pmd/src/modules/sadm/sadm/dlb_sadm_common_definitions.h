/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file dlb_sadm_common_definitions.h
 * @brief Common definitions for the audio definition model.
 *        Based on Recommendation  ITU-R  BS.2094-1
 */

#ifndef DLB_SADM_COMMON_DEFINITIONS_INC_
#define DLB_SADM_COMMON_DEFINITIONS_INC_

#include "sadm/dlb_sadm_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_COMMON_AUDIO_CHANNEL_FORMATS    40
#define NUM_COMMON_AUDIO_PACK_FORMATS       22
#define NUM_COMMON_PACKFMT_CHANFMT          24

#define SADM_COMMON_DEFAULT_SILENT_CHANNEL (0)

dlb_pmd_success
dlb_sadm_init_common_definitions
    (dlb_sadm_model *model
    );

#ifdef __cplusplus
}
#endif

#endif /* DLB_SADM_COMMON_DEFINITIONS_INC_ */
