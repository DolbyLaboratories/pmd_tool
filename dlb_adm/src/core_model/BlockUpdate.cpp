/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "BlockUpdate.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

#include <cstring>

namespace DlbAdm
{

    const dlb_adm_entity_id BlockUpdate::PARENT_ID_MASK = 0x00ffffff00000000;

    BlockUpdate::BlockUpdate()
        : ModelEntity()
    {
        Clear();
    }

    BlockUpdate::BlockUpdate(dlb_adm_entity_id id, const Position &position, const Gain &gain, const dlb_adm_time *start, const dlb_adm_time *duration)
        : ModelEntity(id)
        , mPosition(position)
        , mGain(gain)
        , mHasTime((start != nullptr) || (duration != nullptr))
    {
        if (start != nullptr)
        {
            mStart = *start;
        }
        else
        {
            ::memset(&mStart, 0, sizeof(mStart));
        }

        if (duration != nullptr)
        {
            mDuration = *duration;
        }
        else
        {
            ::memset(&mDuration, 0, sizeof(mDuration));
        }
    }

    BlockUpdate::BlockUpdate(const BlockUpdate &x)
        : ModelEntity(x)
        , mPosition(x.mPosition)
        , mGain(x.mGain)
        , mHasTime(x.mHasTime)
        , mStart(x.mStart)
        , mDuration(x.mDuration)
    {
        // Empty
    }

    BlockUpdate::~BlockUpdate()
    {
        Clear(true);
    }

    BlockUpdate &BlockUpdate::operator=(const BlockUpdate &x)
    {
        ModelEntity::operator=(x);
        mPosition = x.mPosition;
        mGain = x.mGain;
        mHasTime = x.mHasTime;
        mStart = x.mStart;
        mDuration = x.mDuration;
        return *this;
    }

    bool BlockUpdate::GetStart(dlb_adm_time &start) const
    {
        start = mStart;
        return mHasTime;
    }

    bool BlockUpdate::GetDuration(dlb_adm_time &duration) const
    {
        duration = mDuration;
        return mHasTime;
    }

    dlb_adm_entity_id BlockUpdate::GetParentID() const
    {
        dlb_adm_entity_id parentIDType = static_cast<dlb_adm_entity_id>(DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT) << ENTITY_TYPE_SHIFT;
        dlb_adm_entity_id parentID = (mEntityID & PARENT_ID_MASK) | parentIDType;
        return parentID;
    }

    DLB_ADM_AUDIO_TYPE BlockUpdate::GetAudioType() const
    {
        DLB_ADM_AUDIO_TYPE audioType = static_cast<DLB_ADM_AUDIO_TYPE>((mEntityID >> AUDIO_TYPE_SHIFT) & MASK_08);
        return audioType;
    }

    void BlockUpdate::Clear(bool zeroGain)
    {
        // TODO: should we clear mEntityID?
        mPosition.Clear();
        mGain.Clear(zeroGain);
        mHasTime = false;
        ::memset(&mStart, 0, sizeof(mStart));
        ::memset(&mDuration, 0, sizeof(mDuration));
    }

    dlb_adm_entity_id BlockUpdate::GetTargetID(dlb_adm_entity_id blockUpdateID)
    {
        dlb_adm_entity_id targetID = DLB_ADM_NULL_ENTITY_ID;
        DLB_ADM_ENTITY_TYPE entityType = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(blockUpdateID));

        if (entityType == DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT)
        {
            dlb_adm_entity_id parentIDType = static_cast<dlb_adm_entity_id>(DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT) << ENTITY_TYPE_SHIFT;
            targetID = (blockUpdateID & PARENT_ID_MASK) | parentIDType;
        }

        return targetID;
    }

}
