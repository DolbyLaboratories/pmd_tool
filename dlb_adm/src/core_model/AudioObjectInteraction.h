/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef DLB_ADM_AUDIO_OBJECT_INTERACTION_H
#define DLB_ADM_AUDIO_OBJECT_INTERACTION_H

#include "ModelEntity.h"
#include "Gain.h"
#include "Position.h"

#include "dlb_adm/include/dlb_adm_data_types.h"

#include <map>

namespace DlbAdm
{

    class AudioObjectInteraction : public ModelEntity
    {
    public:
        AudioObjectInteraction();

        explicit AudioObjectInteraction(dlb_adm_bool onOffInteract
                                       ,dlb_adm_bool gainInteract
                                       ,dlb_adm_bool positionInteract
                                       ,const Gain &minGain
                                       ,const Gain &maxGain
                                       ,const std::map<Position::COORDINATE, float> &minPositions =
                                              std::map<Position::COORDINATE, float>()
                                       ,const std::map<Position::COORDINATE, float> &maxPositions =
                                              std::map<Position::COORDINATE, float>()
                                       );

        AudioObjectInteraction(const dlb_adm_data_audio_object_interaction &audioObjectInteraction);

        AudioObjectInteraction(const AudioObjectInteraction &x);

        virtual ~AudioObjectInteraction();

        AudioObjectInteraction &operator=(const AudioObjectInteraction &x);

        Gain GetMinGainRange() const { return mMinGainRange; }

        Gain GetMaxGainRange() const { return mMaxGainRange; }

        std::map<Position::COORDINATE, float> GetMinPositionRange() const { return mMinPositionRange; }

        std::map<Position::COORDINATE, float> GetMaxPositionRange() const { return mMaxPositionRange; }

        dlb_adm_bool GetOnOfInteract() const { return mOnOffInteract; }

        dlb_adm_bool GetGainInteract() const { return mGainInteract; }

        dlb_adm_bool GetPositionInteract() const { return mPositionInteract; }


    private:

        void Clear();

        Gain EnsureGainRange(const Gain &gain, float min, float max) const;

        float EnsurePositionRange(float value, float min, float max) const;

        void AssignPositionRange(Position::COORDINATE coordinate, float min, float max);

        dlb_adm_bool mOnOffInteract;
        dlb_adm_bool mGainInteract;
        dlb_adm_bool mPositionInteract;

        Gain mMinGainRange;
        Gain mMaxGainRange;
        std::map<Position::COORDINATE, float> mMinPositionRange;
        std::map<Position::COORDINATE, float> mMaxPositionRange;
    };

}

#endif // DLB_ADM_AUDIO_OBJECT_INTERACTION_H
