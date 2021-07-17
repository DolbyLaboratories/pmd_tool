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

#include "EntityRecord.h"

namespace DlbAdm
{

    EntityRecord::EntityRecord()
        : id(DLB_ADM_NULL_ENTITY_ID)
        , attributesIndex(TableIndex_NIL)
        , status(STATUS::UNINITALIZED)
    {
        // Empty
    }

    EntityRecord::EntityRecord(const EntityRecord & x)
        : id(x.id)
        , attributesIndex(x.attributesIndex)
        , status(x.status)
    {
        // Empty
    }

    EntityRecord::~EntityRecord()
    {
        id = DLB_ADM_NULL_ENTITY_ID;
        attributesIndex = TableIndex_NIL;
        status = STATUS::DESTROYED;
    }

    EntityRecord &EntityRecord::operator=(const EntityRecord &x)
    {
        id = x.id;
        attributesIndex = x.attributesIndex;
        status = x.status;

        return *this;
    }

    bool EntityRecord::operator<(const EntityRecord &x) const
    {
        return id < x.id;
    }

}
