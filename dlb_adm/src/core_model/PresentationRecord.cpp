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
                                           dlb_adm_entity_id elementGrpID /* = DLB_ADM_NULL_ENTITY_ID */)
        : presentationID(presID)
        , contentGroupID(contentGrpID)
        , elementGroupID(elementGrpID)
        , audioElementID(elementID)
    {
        // Empty
    }

    PresentationRecord::PresentationRecord(const PresentationRecord &x)
        : presentationID(x.presentationID)
        , contentGroupID(x.contentGroupID)
        , elementGroupID(x.elementGroupID)
        , audioElementID(x.audioElementID)
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

        return *this;
    }

    bool PresentationRecord::operator<(const PresentationRecord &x) const
    {
        return
            std::tie(  presentationID,   contentGroupID,   elementGroupID,   audioElementID) <
            std::tie(x.presentationID, x.contentGroupID, x.elementGroupID, x.audioElementID);
    }

    PresentationRecord &PresentationRecord::Clear()
    {
        presentationID = DLB_ADM_NULL_ENTITY_ID;
        contentGroupID = DLB_ADM_NULL_ENTITY_ID;
        elementGroupID = DLB_ADM_NULL_ENTITY_ID;
        audioElementID = DLB_ADM_NULL_ENTITY_ID;

        return *this;
    }

    bool PresentationRecord::IsNull() const
    {
        return
            presentationID == DLB_ADM_NULL_ENTITY_ID &&
            contentGroupID == DLB_ADM_NULL_ENTITY_ID &&
            elementGroupID == DLB_ADM_NULL_ENTITY_ID &&
            audioElementID == DLB_ADM_NULL_ENTITY_ID;
    }

    bool PresentationRecord::Validate(bool nullOK /* = false */) const
    {
        bool nullPresentation = (presentationID == DLB_ADM_NULL_ENTITY_ID);
        bool nullElementGroup = (elementGroupID == DLB_ADM_NULL_ENTITY_ID);
        AdmIdTranslator translator;

        return
            ((nullPresentation || translator.GetEntityType(presentationID) == DLB_ADM_ENTITY_TYPE_PROGRAMME) &&
             (translator.GetEntityType(contentGroupID) == DLB_ADM_ENTITY_TYPE_CONTENT) &&
             (nullElementGroup || translator.GetEntityType(elementGroupID) == DLB_ADM_ENTITY_TYPE_OBJECT) &&
             (translator.GetEntityType(audioElementID) == DLB_ADM_ENTITY_TYPE_OBJECT)) ||
            (nullOK && IsNull());
    }

}
