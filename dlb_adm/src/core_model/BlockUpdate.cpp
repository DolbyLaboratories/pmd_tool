/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2021, Dolby Laboratories Inc.
 * Copyright (c) 2020-2021, Dolby International AB.
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

    BlockUpdate::BlockUpdate(dlb_adm_entity_id id, const Position &position, const Gain &gain, const dlb_adm_time *start, const dlb_adm_time *duration, bool isCommon /*= false*/)
        : ModelEntity(id, 0, isCommon)
        , mPosition(position)
        , mGain(gain)
        , mHasTime((start != nullptr) && (duration != nullptr))
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
        mIsCommon = false;
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
