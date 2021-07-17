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

#include "ElementTable.h"

namespace DlbAdm
{

    bool AudioElementIdCompare::operator()(const ElementRecord &lhs, dlb_adm_entity_id rhs) const
    {
        return lhs.audioElementID < rhs;
    }

    bool AudioElementIdCompare::operator()(dlb_adm_entity_id lhs, const ElementRecord &rhs) const
    {
        return lhs < rhs.audioElementID;
    }

}
