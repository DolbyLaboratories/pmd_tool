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

#include "RelationshipDescriptor.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>

#include <tuple>

namespace DlbAdm
{
    using namespace boost::multi_index;

    const int RelationshipArity::ANY = -1;

    ENTITY_RELATIONSHIP Inverse(ENTITY_RELATIONSHIP r)
    {
        ENTITY_RELATIONSHIP inverse = ENTITY_RELATIONSHIP::NONE;

        switch (r)
        {
        case ENTITY_RELATIONSHIP::CONTAINS:
            inverse = ENTITY_RELATIONSHIP::CONTAINED_BY;
            break;
        case ENTITY_RELATIONSHIP::CONTAINED_BY:
            inverse = ENTITY_RELATIONSHIP::CONTAINS;
            break;
        case ENTITY_RELATIONSHIP::REFERENCES:
            inverse = ENTITY_RELATIONSHIP::REFERENCED_BY;
            break;
        case ENTITY_RELATIONSHIP::REFERENCED_BY:
            inverse = ENTITY_RELATIONSHIP::REFERENCES;
            break;
        default:
            break;
        }

        return inverse;
    }

    RelationshipDescriptor nullRelationshipDescriptor =
    {
        DLB_ADM_ENTITY_TYPE_ILLEGAL,
        DLB_ADM_ENTITY_TYPE_ILLEGAL,
        ENTITY_RELATIONSHIP::NONE,
        { 0, 0 }
    };

    bool RelationshipDescriptor::operator<(const RelationshipDescriptor &x) const
    {
        return
            std::tie(  fromType,   toType) <
            std::tie(x.fromType, x.toType);
    }

    struct RelationshipIndex_PK {};

    typedef multi_index_container<
        RelationshipDescriptor,
        indexed_by<
            ordered_unique<tag<RelationshipIndex_PK>, identity<RelationshipDescriptor>  >
        >
    > RelationshipIndex;

    typedef RelationshipIndex::index<RelationshipIndex_PK>::type RelationshipIndex_PKIndex;

    struct DescriptorKey
    {
        DLB_ADM_ENTITY_TYPE  fromType;
        DLB_ADM_ENTITY_TYPE  toType;
    };

    struct DescriptorKeyCompare
    {
        bool operator()(const RelationshipDescriptor &lhs, const DescriptorKey &rhs) const;
        bool operator()(const DescriptorKey &lhs, const RelationshipDescriptor &rhs) const;
    };

    bool DescriptorKeyCompare::operator()(const RelationshipDescriptor &lhs, const DescriptorKey &rhs) const
    {
        return
            std::tie(lhs.fromType, lhs.toType) <
            std::tie(rhs.fromType, rhs.toType);
    }

    bool DescriptorKeyCompare::operator()(const DescriptorKey &lhs, const RelationshipDescriptor &rhs) const
    {
        return
            std::tie(lhs.fromType, lhs.toType) <
            std::tie(rhs.fromType, rhs.toType);
    }

#include "RelationshipInitializers.h"

    static const size_t RELATIONSHIP_COUNT = sizeof(initializers) / sizeof(RelationshipDescriptor);

    static RelationshipIndex theAdmRelationshipIndex;

    void InitializeRelationshipIndex()
    {
        if (theAdmRelationshipIndex.size() == 0)
        {
            for (size_t i = 0; i < RELATIONSHIP_COUNT; i++)
            {
                const RelationshipDescriptor *rd = &initializers[i];
                theAdmRelationshipIndex.insert(*rd);

                RelationshipDescriptor inverse;
                inverse.fromType = rd->toType;
                inverse.toType = rd->fromType;
                inverse.relationship = Inverse(rd->relationship);
                theAdmRelationshipIndex.insert(inverse);
            }
        }
    }

    int GetRelationshipDescriptor(RelationshipDescriptor &rd, DLB_ADM_ENTITY_TYPE f, DLB_ADM_ENTITY_TYPE t)
    {
        if (theAdmRelationshipIndex.size() == 0)
        {
            InitializeRelationshipIndex();
        }

        int status = DLB_ADM_STATUS_NOT_FOUND;

        DescriptorKey key;
        key.fromType = f;
        key.toType = t;

        RelationshipIndex_PKIndex &index = theAdmRelationshipIndex.get<RelationshipIndex_PK>();
        RelationshipIndex_PKIndex::iterator it = index.find(key, DescriptorKeyCompare());

        if (it != index.end())
        {
            rd = *it;
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

}
