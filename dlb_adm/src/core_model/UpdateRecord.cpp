/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#include "UpdateRecord.h"
#include "BlockUpdate.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

namespace DlbAdm
{

    UpdateRecord::UpdateRecord()
        : updateID(DLB_ADM_NULL_ENTITY_ID)
    {
        // Empty
    }

    UpdateRecord::UpdateRecord(dlb_adm_entity_id updateID)
        : updateID(updateID)
    {
        // Empty
    }

    UpdateRecord::UpdateRecord(const UpdateRecord &x)
        : updateID(x.updateID)
    {
        // Empty
    }

    UpdateRecord::~UpdateRecord()
    {
        updateID = DLB_ADM_NULL_ENTITY_ID;
    }

    UpdateRecord &UpdateRecord::operator=(const UpdateRecord &x)
    {
        updateID = x.updateID;
        return *this;
    }

    bool UpdateRecord::operator<(const UpdateRecord &x) const
    {
        return updateID < x.updateID;
    }

    dlb_adm_entity_id UpdateRecord::GetTargetID() const
    {
        return BlockUpdate::GetTargetID(updateID);
    }

    bool UpdateRecord::IsNull() const
    {
        return (updateID == DLB_ADM_NULL_ENTITY_ID);
    }

    bool UpdateRecord::Validate(bool nullOK) const
    {
        return
            (AdmIdTranslator().GetEntityType(updateID) == DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT) ||
            (nullOK && updateID == DLB_ADM_NULL_ENTITY_ID);
    }

}
