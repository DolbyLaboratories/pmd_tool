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

#ifndef DLB_ADM_BLOCK_UPDATE_H
#define DLB_ADM_BLOCK_UPDATE_H

#include "ModelEntity.h"
#include "Position.h"
#include "Gain.h"

namespace DlbAdm
{

    class BlockUpdate : public ModelEntity
    {
    public:
        BlockUpdate();
        BlockUpdate(dlb_adm_entity_id id, const Position &position, const Gain &gain, const dlb_adm_time *start = nullptr, const dlb_adm_time *duration = nullptr);
        BlockUpdate(const BlockUpdate &x);
        ~BlockUpdate();

        BlockUpdate &operator=(const BlockUpdate &x);

        Position GetPosition() const { return mPosition; }

        Gain GetGain() const { return mGain; }

        bool HasTime() const { return mHasTime; }

        bool GetStart(dlb_adm_time &start) const;

        bool GetDuration(dlb_adm_time &duration) const;

        dlb_adm_entity_id GetParentID() const;

        DLB_ADM_AUDIO_TYPE GetAudioType() const;

        void Clear(bool zeroGain = false);

        static dlb_adm_entity_id GetTargetID(dlb_adm_entity_id blockUpdateID);

    private:
        static const dlb_adm_entity_id PARENT_ID_MASK;

        Position mPosition;
        Gain mGain;
        
        bool mHasTime;
        dlb_adm_time mStart;
        dlb_adm_time mDuration;
    };

}

#endif  // DLB_ADM_BLOCK_UPDATE_H
