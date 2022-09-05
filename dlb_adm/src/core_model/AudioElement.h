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

#ifndef DLB_ADM_AUDIO_ELEMENT_H
#define DLB_ADM_AUDIO_ELEMENT_H

#include "ModelEntity.h"
#include "Gain.h"
#include "Position.h"
#include "AudioObjectInteraction.h"

namespace DlbAdm
{

    class AudioElement : public ModelEntity
    {
    public:
        AudioElement();
        AudioElement(dlb_adm_entity_id id
                    ,const Gain &gain
                    ,const Position &positionOffset
                    ,const DLB_ADM_OBJECT_CLASS objectClass
                    ,dlb_adm_bool interact = 0
                    ,const AudioObjectInteraction &objectIteraction = AudioObjectInteraction());

        explicit AudioElement
            (dlb_adm_entity_id id
            ,float gainValue = 1.0f
            ,Gain::GAIN_UNIT gainUnit = Gain::GAIN_UNIT::LINEAR
            ,const dlb_adm_data_position_offset offset = {0.0, DLB_ADM_FALSE}
            ,DLB_ADM_OBJECT_CLASS objectClass = DLB_ADM_OBJECT_CLASS_NONE
            ,dlb_adm_bool interact = 0
            ,const AudioObjectInteraction objectIteraction = AudioObjectInteraction()
            );

        AudioElement(const AudioElement &x);
        virtual ~AudioElement();

        AudioElement &operator=(const AudioElement &x);

        Gain GetGain() const { return mGain; }

        Position GetPositionOffset() const { return mPositionOffset; }

        DLB_ADM_OBJECT_CLASS GetObjectClass() const { return mObjectClass; }

        dlb_adm_bool IsInteractive() const { return mInteract; }

        AudioObjectInteraction GetInteractionBoundreies() const { return mObjectInteraction; }

        virtual bool AddLabel(const char *name, const char *language = "");

        virtual bool AddLabel(const std::string &name, const std::string &language);

    private:
        Gain mGain;
        Position mPositionOffset; /* only stores coordinate "X" or "azimuth"  */
        DLB_ADM_OBJECT_CLASS mObjectClass;
        dlb_adm_bool mInteract;
        AudioObjectInteraction mObjectInteraction;
    };

}

#endif // DLB_ADM_AUDIO_ELEMENT_H
