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

#include "Source.h"

namespace DlbAdm
{

    Source::Source()
        : ModelEntity()
        , mSourceGroupID(UNKNOWN_SOURCE_GROUP_ID)
        , mChannelNumber(UNKNOWN_CHANNEL_NUMBER)
    {
        // Empty
    }

    Source::Source(dlb_adm_entity_id entityID, ChannelNumber channelNumber, SourceGroupID sourceGroupID /*= DEFAULT_SOURCE_GROUP_ID*/)
        : ModelEntity(entityID)
        , mSourceGroupID(sourceGroupID)
        , mChannelNumber(channelNumber)
    {
        // Empty
    }

    Source::Source(const Source &x)
        : ModelEntity(x)
        , mSourceGroupID(x.mSourceGroupID)
        , mChannelNumber(x.mChannelNumber)
    {
        // Empty
    }

    Source::~Source()
    {
        mSourceGroupID = UNKNOWN_SOURCE_GROUP_ID;
        mChannelNumber = UNKNOWN_CHANNEL_NUMBER;
    }

    Source &Source::operator=(const Source &x)
    {
        (void)ModelEntity::operator=(x);
        mSourceGroupID = x.mSourceGroupID;
        mChannelNumber = x.mChannelNumber;
        return *this;
    }

}
