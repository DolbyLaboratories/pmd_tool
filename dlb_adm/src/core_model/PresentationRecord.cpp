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

#include "PresentationRecord.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <tuple>

namespace DlbAdm
{

    PresentationRecord::PresentationRecord()
    {
        Clear();
    }

    PresentationRecord::PresentationRecord(dlb_adm_entity_id presID,
                                           dlb_adm_entity_id contentGrpID,
                                           dlb_adm_entity_id elementID,
                                           dlb_adm_entity_id elementGrpID /* = DLB_ADM_NULL_ENTITY_ID */,
                                           dlb_adm_entity_id altValSetID  /* = DLB_ADM_NULL_ENTITY_ID */,
                                           dlb_adm_entity_id compRefID /* = DLB_ADM_NULL_ENTITY_ID */)
        :presentationID(presID)
        ,contentGroupID(contentGrpID)
        ,elementGroupID(elementGrpID)
        ,audioElementID(elementID)
        ,altValueSetID (altValSetID)
        ,complementaryRefID(compRefID)
    {
        // Empty
    }

    PresentationRecord::PresentationRecord(const PresentationRecord &x)
        :presentationID(x.presentationID)
        ,contentGroupID(x.contentGroupID)
        ,elementGroupID(x.elementGroupID)
        ,audioElementID(x.audioElementID)
        ,altValueSetID (x.altValueSetID)
        ,complementaryRefID(x.complementaryRefID)
    {
        // Empty
    }

    PresentationRecord::~PresentationRecord()
    {
        Clear();
    }

    PresentationRecord &PresentationRecord::operator=(const PresentationRecord &x)
    {
        presentationID = x.presentationID;
        contentGroupID = x.contentGroupID;
        elementGroupID = x.elementGroupID;
        audioElementID = x.audioElementID;
        altValueSetID  = x.altValueSetID;
        complementaryRefID = x.complementaryRefID;

        return *this;
    }

    bool PresentationRecord::operator<(const PresentationRecord &x) const
    {
        return
            std::tie(  presentationID,   contentGroupID,   elementGroupID,   audioElementID,   altValueSetID, complementaryRefID) <
            std::tie(x.presentationID, x.contentGroupID, x.elementGroupID, x.audioElementID, x.altValueSetID, x.complementaryRefID);
    }

    PresentationRecord &PresentationRecord::Clear()
    {
        presentationID = DLB_ADM_NULL_ENTITY_ID;
        contentGroupID = DLB_ADM_NULL_ENTITY_ID;
        elementGroupID = DLB_ADM_NULL_ENTITY_ID;
        audioElementID = DLB_ADM_NULL_ENTITY_ID;
        altValueSetID  = DLB_ADM_NULL_ENTITY_ID;
        complementaryRefID = DLB_ADM_NULL_ENTITY_ID;

        return *this;
    }

    bool PresentationRecord::IsNull() const
    {
        return
            presentationID == DLB_ADM_NULL_ENTITY_ID &&
            contentGroupID == DLB_ADM_NULL_ENTITY_ID &&
            elementGroupID == DLB_ADM_NULL_ENTITY_ID &&
            audioElementID == DLB_ADM_NULL_ENTITY_ID &&
            altValueSetID  == DLB_ADM_NULL_ENTITY_ID &&
            complementaryRefID == DLB_ADM_NULL_ENTITY_ID;
    }

    bool PresentationRecord::Validate(bool nullOK /* = false */) const
    {
        bool nullPresentation = (presentationID == DLB_ADM_NULL_ENTITY_ID);
        bool nullElementGroup = (elementGroupID == DLB_ADM_NULL_ENTITY_ID);
        bool nullAltValSet    = (altValueSetID  == DLB_ADM_NULL_ENTITY_ID);
        bool nullCompObject   = (complementaryRefID == DLB_ADM_NULL_ENTITY_ID);
        AdmIdTranslator translator;

        return
            ((nullPresentation || translator.GetEntityType(presentationID) == DLB_ADM_ENTITY_TYPE_PROGRAMME) &&
             (translator.GetEntityType(contentGroupID) == DLB_ADM_ENTITY_TYPE_CONTENT) &&
             (nullElementGroup || translator.GetEntityType(elementGroupID) == DLB_ADM_ENTITY_TYPE_OBJECT) &&
             (nullCompObject || translator.GetEntityType(complementaryRefID) == DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF) &&
             (translator.GetEntityType(audioElementID) == DLB_ADM_ENTITY_TYPE_OBJECT) &&
             (  nullAltValSet
             || (   !nullPresentation
                &&  translator.GetEntityType(altValueSetID) == DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET
                &&  translator.SubcomponentIdReferencesComponent(audioElementID, altValueSetID)
                )
             )) ||
            (nullOK && IsNull());
    }

}
