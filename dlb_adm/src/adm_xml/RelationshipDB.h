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

#ifndef DLB_ADM_RELATIONDHIP_DB_H
#define DLB_ADM_RELATIONDHIP_DB_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "EntityRelationship.h"

#include <boost/core/noncopyable.hpp>
#include <functional>
#include <memory>

namespace DlbAdm
{

    struct RelationshipRecord;
    struct RelationshipDescriptor;
    class RelationshipData;

    class RelationshipDB : public boost::noncopyable
    {
    public:
        RelationshipDB();
        ~RelationshipDB();

        typedef std::function<int (const RelationshipRecord &r)> const& RelationshipCallbackFn;
        typedef std::function<bool(const RelationshipRecord &r)> const& RelationshipFilterFn;

        int GetDescriptor(RelationshipDescriptor &rd, DLB_ADM_ENTITY_TYPE f, DLB_ADM_ENTITY_TYPE t);

        int Add(dlb_adm_entity_id fromID, dlb_adm_entity_id toID);

        int Get(RelationshipRecord &r, dlb_adm_entity_id fromID, dlb_adm_entity_id toID);

        int ForEach(RelationshipCallbackFn callbackFn);

        int ForEach(dlb_adm_entity_id id, ENTITY_RELATIONSHIP r, RelationshipCallbackFn callbackFn, RelationshipFilterFn filterFn = nullptr);

        int ForEach(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE entityType, RelationshipCallbackFn callbackFn, RelationshipFilterFn filterFn = nullptr);

        bool Exists(dlb_adm_entity_id fromId, dlb_adm_entity_id toId);

        bool Exists(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE entityType);

        size_t Count(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE entityType);

    private:
        std::unique_ptr<RelationshipData> mRelationshipData;
    };

}

#endif
