/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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

#include "AudioElement.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    AudioElement::AudioElement()
        : ModelEntity()
        , mGain()
        , mPositionOffset()
        , mObjectInteraction()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
        mObjectClass = DLB_ADM_OBJECT_CLASS_NONE;
        mInteract = 0u;
    }

    AudioElement::AudioElement(const AudioElement &x)
        : ModelEntity(x)
        , mGain(x.mGain)
        , mPositionOffset(x.mPositionOffset)
        , mObjectClass(x.mObjectClass)
        , mInteract(x.mInteract)
        , mObjectInteraction(x.mObjectInteraction)
    {
        // Empty
    }

    AudioElement::AudioElement(dlb_adm_entity_id id
                              ,const Gain &gain
                              ,const Position &positionOffset
                              ,const DLB_ADM_OBJECT_CLASS objectClass
                              ,dlb_adm_bool interact /*= 0 */
                              ,const AudioObjectInteraction &objectIteraction /*= default */
                              )
        : ModelEntity(id)
        , mGain(gain)
        , mPositionOffset(positionOffset)
        , mObjectClass(objectClass)
        , mInteract(interact)
        , mObjectInteraction(objectIteraction)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AudioElement::AudioElement
        (dlb_adm_entity_id id
        ,float gainValue /*= 1.0f*/
        ,Gain::GAIN_UNIT gainUnit /*= Gain::GAIN_UNIT::LINEAR*/
        ,const dlb_adm_data_position_offset offset /*= {0.0, DLB_ADM_FALSE}*/
        ,const DLB_ADM_OBJECT_CLASS objectClass /*= DLB_ADM_OBJECT_CLASS_NONE */
        ,dlb_adm_bool interact /*= 0 */
        ,const AudioObjectInteraction objectIteraction /*= default */
        )
        : ModelEntity(id)
        , mGain(gainValue, gainUnit)
        , mPositionOffset(offset.offset_value, offset.cartesian)
        , mObjectClass(objectClass)
        , mInteract(interact)
        , mObjectInteraction(objectIteraction)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AudioElement::~AudioElement()
    {
        // Empty for now
    }

    AudioElement &AudioElement::operator=(const AudioElement &x)
    {
        (void)ModelEntity::operator=(x);
        mGain = x.mGain;
        mPositionOffset = x.mPositionOffset;
        mObjectClass = x.mObjectClass;
        mInteract = x.mInteract;
        return *this;
    }

    bool AudioElement::AddLabel(const char *name, const char *language)
    {
        return ModelEntity::AddLabel(name, language);
    }

    bool AudioElement::AddLabel(const std::string &name, const std::string &language)
    {
        return ModelEntity::AddLabel(name, language);
    }

}
