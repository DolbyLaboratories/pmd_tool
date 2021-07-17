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

#include "FrameFormat.h"

namespace DlbAdm
{

    FrameFormat::FrameFormat()
        : ModelEntity()
        , mType()
        , mStart()
        , mDuration()
        , mFlowID()
    {
        // Empty
    }

    FrameFormat::FrameFormat(
        dlb_adm_entity_id entityID,
        const std::string &type,
        const std::string &start,
        const std::string &duration,
        const std::string &flowID
    )
        : ModelEntity(entityID)
        , mType(type)
        , mStart(start)
        , mDuration(duration)
        , mFlowID(flowID)
    {
        // Empty
    }

    FrameFormat::FrameFormat(const FrameFormat &x)
        : ModelEntity(x)
        , mType(x.mType)
        , mStart(x.mStart)
        , mDuration(x.mDuration)
        , mFlowID(x.mFlowID)
    {
        // Empty
    }

    FrameFormat::~FrameFormat()
    {
        // Empty
    }

    FrameFormat &FrameFormat::operator=(const FrameFormat &x)
    {
        ModelEntity::operator=(x);
        mType = x.mType;
        mStart = x.mStart;
        mDuration = x.mDuration;
        mFlowID = x.mFlowID;
        return *this;
    }

}
