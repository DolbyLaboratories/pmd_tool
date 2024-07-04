/************************************************************************
 * dlb_adm
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#include "AlternativeValueSet.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include "core_model_defs.h"

namespace DlbAdm
{
    AlternativeValueSet::AlternativeValueSet()
        : ModelEntity()
        , mPositionOffset()
        , mGain()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AlternativeValueSet::AlternativeValueSet
        ( dlb_adm_entity_id id
        , const boost::optional<Position> position /* = boost::none */
        , const boost::optional<Gain> gain /* = boost::none */
        )
        : ModelEntity(id)
        , mPositionOffset(position)
        , mGain(gain)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AlternativeValueSet::AlternativeValueSet(const dlb_adm_data_alt_value_set &avs)
        : ModelEntity    (avs.id)
    {
        mPositionOffset = boost::make_optional(avs.has_position_offset, Position(avs.position[0], avs.cartesian));
        mGain = boost::make_optional(avs.has_gain, Gain(avs.gain));
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AlternativeValueSet::AlternativeValueSet(const AlternativeValueSet &x)
        : ModelEntity(x)
        , mPositionOffset(x.mPositionOffset)
        , mGain(x.mGain)
    {
        // empty
    }

    AlternativeValueSet::~AlternativeValueSet()
    {
        mPositionOffset = boost::none;
        mGain = boost::none;
    }

    int AlternativeValueSet::GetPositionOffset(Position &position) const
    {
        int status;

        if(mPositionOffset.has_value())
        {
            position = mPositionOffset.value();
            status = DLB_ADM_STATUS_OK;
        }
        else
        {
            status = DLB_ADM_STATUS_NOT_FOUND;
        }
        
        return status;
    }

    int AlternativeValueSet::GetGain(Gain &gain) const
    {
        int status;

        if(mGain.has_value())
        {
            gain = mGain.value();
            status = DLB_ADM_STATUS_OK;
        }
        else
        {
            status = DLB_ADM_STATUS_NOT_FOUND;
        }
        
        return status;
    }

    dlb_adm_entity_id AlternativeValueSet::GetParentId() const
    { 
        AdmIdTranslator translator;
        uint32_t sequenceNumber;
        translator.DeconstructUntypedId(this->GetEntityID(), nullptr, &sequenceNumber, nullptr);
        
        return translator.ConstructUntypedId(DLB_ADM_ENTITY_TYPE_OBJECT, sequenceNumber);
    }

    AlternativeValueSet & AlternativeValueSet::operator=(const AlternativeValueSet &x)
    {
        (void)ModelEntity::operator=(x);
        mPositionOffset = x.mPositionOffset;
        mGain = x.mGain;
        return *this;
    }

    bool AlternativeValueSet::AddLabel(const char *name, const char *language)
    {
        return ModelEntity::AddLabel(name, language);
    }

    bool AlternativeValueSet::AddLabel(const std::string &name, const std::string &language)
    {
        return ModelEntity::AddLabel(name, language);
    }
}

