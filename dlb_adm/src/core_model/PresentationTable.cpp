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

#include "PresentationTable.h"

namespace DlbAdm
{

    bool PresentationIdCompare::operator()(const PresentationRecord &lhs, dlb_adm_entity_id rhs) const
    {
        return lhs.presentationID < rhs;
    }

    bool PresentationIdCompare::operator()(dlb_adm_entity_id lhs, const PresentationRecord &rhs) const
    {
        return lhs < rhs.presentationID;
    }

}
