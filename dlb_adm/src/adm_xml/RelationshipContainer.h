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

#ifndef DLB_ADM_RELATIONSHIPS_H
#define DLB_ADM_RELATIONSHIPS_H

#include "RelationshipRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    typedef std::tuple<
        dlb_adm_entity_id,
        ENTITY_RELATIONSHIP,
        DLB_ADM_ENTITY_TYPE,
        dlb_adm_entity_id
    > RelationshipTuple;

    struct RelationshipContainer_PK {};

    typedef composite_key<
        RelationshipRecord,
        const_mem_fun<RelationshipRecord, dlb_adm_entity_id,   &RelationshipRecord::GetFromId>,
        const_mem_fun<RelationshipRecord, ENTITY_RELATIONSHIP, &RelationshipRecord::GetRelationship>,
        const_mem_fun<RelationshipRecord, DLB_ADM_ENTITY_TYPE, &RelationshipRecord::GetToEntityType>,
        const_mem_fun<RelationshipRecord, dlb_adm_entity_id,   &RelationshipRecord::GetToId>
    > RelationshipKey;

    typedef multi_index_container<
        RelationshipRecord,
        indexed_by<
            // Primary Key (PK) index
            ordered_unique<
                tag<RelationshipContainer_PK>,
                RelationshipKey,
                composite_key_compare<
                    std::less<dlb_adm_entity_id>,
                    std::less<ENTITY_RELATIONSHIP>,
                    std::less<DLB_ADM_ENTITY_TYPE>,
                    std::less<dlb_adm_entity_id>
                >
            >
        >
    > RelationshipContainer;

    typedef RelationshipContainer::index<RelationshipContainer_PK>::type RelationshipContainer_PKIndex;

}

#endif /* DLB_ADM_RELATIONSHIPS_H */
