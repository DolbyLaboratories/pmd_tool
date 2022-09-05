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

#include "ComplementaryElement.h"
#include "core_model_defs.h"

namespace DlbAdm
{
    ComplementaryElement::ComplementaryElement()
        : ModelEntity()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ComplementaryElement::ComplementaryElement(const ComplementaryElement &x)
        :ModelEntity(x)
        ,mCompElementId(x.mCompElementId)
        ,mCompLeaderId(x.mCompLeaderId)
    {
        // Empty
    }

    ComplementaryElement::ComplementaryElement(dlb_adm_entity_id id, dlb_adm_entity_id compElementId, dlb_adm_entity_id CompLeaderId)
        :ModelEntity(id)
        ,mCompElementId(compElementId)
        ,mCompLeaderId(CompLeaderId)
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    ComplementaryElement::~ComplementaryElement()
    {
        // Empty for now
    }

    ComplementaryElement &ComplementaryElement::operator=(const ComplementaryElement &x)
    {
        (void)ModelEntity::operator=(x);
        mCompElementId = x.mCompElementId;
        mCompLeaderId = x.mCompLeaderId;
        return *this;
    }

    bool ComplementaryElement::AddLabel(const std::string &name, const std::string &language)
    {
        return ModelEntity::AddLabel(name, language);
    }

}
