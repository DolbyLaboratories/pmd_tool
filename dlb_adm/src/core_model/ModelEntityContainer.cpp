/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "ModelEntityContainer.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <tuple>

namespace DlbAdm
{

    ModelEntityRecord::ModelEntityRecord(ConstModelEntityPtr p)
        : modelEntityPtr(p)
    {
        // Empty
    }

    const ModelEntity *ModelEntityRecord::GetPointer() const
    {
        return modelEntityPtr.get();
    }

    const ModelEntity &ModelEntityRecord::GetReference() const
    {
        return *modelEntityPtr.get();
    }

    bool ModelEntityIdCompare::operator()(const ModelEntity &lhs, dlb_adm_entity_id rhs) const
    {
        AdmIdTranslator translator;
        DLB_ADM_ENTITY_TYPE lhsType = lhs.GetEntityType();
        DLB_ADM_ENTITY_TYPE rhsType = translator.GetEntityType(rhs);
        dlb_adm_entity_id lhsID = lhs.GetEntityID();

        return
            std::tie(lhsType, lhsID) <
            std::tie(rhsType, rhs);
    }

    bool ModelEntityIdCompare::operator()(dlb_adm_entity_id lhs, const ModelEntity &rhs) const
    {
        AdmIdTranslator translator;
        DLB_ADM_ENTITY_TYPE lhsType = translator.GetEntityType(lhs);
        DLB_ADM_ENTITY_TYPE rhsType = rhs.GetEntityType();
        dlb_adm_entity_id rhsID = rhs.GetEntityID();

        return
            std::tie(lhsType, lhs) <
            std::tie(rhsType, rhsID);
    }

    bool ModelEntityTypeCompare::operator()(const ModelEntity &lhs, DLB_ADM_ENTITY_TYPE rhs) const
    {
        DLB_ADM_ENTITY_TYPE lhsType = lhs.GetEntityType();

        return lhsType < rhs;
    }

    bool ModelEntityTypeCompare::operator()(DLB_ADM_ENTITY_TYPE lhs, const ModelEntity &rhs) const
    {
        DLB_ADM_ENTITY_TYPE rhsType = rhs.GetEntityType();

        return lhs < rhsType;
    }

}
