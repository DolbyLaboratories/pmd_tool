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
        BlockUpdate(dlb_adm_entity_id id, const Position &position, const Gain &gain, const dlb_adm_time *start = nullptr, const dlb_adm_time *duration = nullptr, bool isCommon = false);
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
