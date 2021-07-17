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

#ifndef DLB_ADM_CORE_MODEL_DEFS_H
#define DLB_ADM_CORE_MODEL_DEFS_H

#include "dlb_adm/include/dlb_adm_api_types.h"

namespace DlbAdm
{

    typedef dlb_adm_sample_rate SampleRate;
    typedef dlb_adm_bit_depth BitDepth;

    static const SampleRate UNKNOWN_SAMPLE_RATE = 0u;
    static const SampleRate DEFAULT_SAMPLE_RATE = 48000u;
    static const BitDepth UNKNOWN_BIT_DEPTH = 0u;
    static const BitDepth DEFAULT_BIT_DEPTH = 24u;

    typedef dlb_adm_source_group_id SourceGroupID;
    typedef dlb_adm_channel_number ChannelNumber;

    static const SourceGroupID UNKNOWN_SOURCE_GROUP_ID = 0u;
    static const SourceGroupID DEFAULT_SOURCE_GROUP_ID = 1u;
    static const ChannelNumber UNKNOWN_CHANNEL_NUMBER = 0u;

    static const size_t DEFAULT_NAME_LIMIT = 32u;

}

#endif  // DLB_ADM_CORE_MODEL_DEFS_H
