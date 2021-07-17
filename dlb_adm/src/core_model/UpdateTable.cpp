/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "UpdateTable.h"

#include <tuple>

namespace DlbAdm
{

    bool TargetIdCompare::operator()(const UpdateKeyResult &lhs, dlb_adm_entity_id rhs) const
    {
        dlb_adm_entity_id lhsID = lhs.value.GetTargetID();
        return lhsID < rhs;
    }

    bool TargetIdCompare::operator()(dlb_adm_entity_id lhs, const UpdateKeyResult &rhs) const
    {
        dlb_adm_entity_id rhsID = rhs.value.GetTargetID();
        return lhs < rhsID;
    }

}
