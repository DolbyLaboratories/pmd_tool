/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "SourceGroup.h"

namespace DlbAdm
{

    static const size_t NAME_LIMIT = 1;

    SourceGroup::SourceGroup()
        : ModelEntity()
        , mSourceGroupID(UNKNOWN_SOURCE_GROUP_ID)
        , mSignalCount(0)
        , mTrackCount(0)
    {
        mNameLimit = NAME_LIMIT;
    }

    SourceGroup::SourceGroup(dlb_adm_entity_id entityID, SourceGroupID sourceGroupID, dlb_adm_uint signalCount, dlb_adm_uint trackCount)
        : ModelEntity(entityID, NAME_LIMIT)
        , mSourceGroupID(sourceGroupID)
        , mSignalCount(signalCount)
        , mTrackCount(trackCount)
    {
        // Empty
    }

    SourceGroup::SourceGroup(const SourceGroup &x)
        : ModelEntity(x)
        , mSourceGroupID(x.mSourceGroupID)
        , mSignalCount(x.mSignalCount)
        , mTrackCount(x.mTrackCount)
    {
        // Empty
    }

    SourceGroup::~SourceGroup()
    {
        mSourceGroupID = UNKNOWN_SOURCE_GROUP_ID;
        mSignalCount = 0;
        mTrackCount = 0;
    }

    SourceGroup &SourceGroup::operator=(const SourceGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mSourceGroupID = x.mSourceGroupID;
        mSignalCount = x.mSignalCount;
        mTrackCount = x.mTrackCount;
        return *this;
    }

}
