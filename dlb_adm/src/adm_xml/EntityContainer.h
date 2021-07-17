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

#ifndef DLB_ADM_ENTITIES_H
#define DLB_ADM_ENTITIES_H

#include "EntityRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    // Entity container

    struct EntityContainer_PK {};

    typedef multi_index_container<
        EntityRecord,
        indexed_by<
            // Primary key (PK)
            ordered_unique<tag<EntityContainer_PK>, identity<EntityRecord> >
        >
    > EntityContainer;

    typedef EntityContainer::index<EntityContainer_PK>::type EntityContainer_PKIndex;

    // Functional object(s) for comparison in searching

    struct EntityIdCompare
    {
        bool operator()(const EntityRecord &lhs, dlb_adm_entity_id   rhs) const;
        bool operator()(dlb_adm_entity_id   lhs, const EntityRecord &rhs) const;
    };

    struct EntityTypeCompare
    {
        bool operator()(const EntityRecord  &lhs, DLB_ADM_ENTITY_TYPE  rhs) const;
        bool operator()(DLB_ADM_ENTITY_TYPE  lhs, const EntityRecord  &rhs) const;
    };

}

#endif
