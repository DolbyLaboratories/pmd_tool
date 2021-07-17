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

#ifndef DLB_ADM_ENTITY_DB_H
#define DLB_ADM_ENTITY_DB_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "AttributeValue.h"

#include <boost/core/noncopyable.hpp>
#include <functional>
#include <memory>

namespace DlbAdm
{

    struct EntityRecord;
    struct AttributeDescriptor;
    class EntityData;

    class EntityDB : public boost::noncopyable
    {
    public:
        EntityDB();
        ~EntityDB();

        typedef std::function<int (const EntityRecord &e)> const& EntityCallbackFn;
        typedef std::function<bool(const EntityRecord &e)> const& EntityFilterFn;
        typedef std::function<int (dlb_adm_entity_id id, DLB_ADM_TAG tag, const AttributeValue &value)> const& AttributeCallbackFn;

        int Get(EntityRecord &e, dlb_adm_entity_id id);

        int GetDescriptor(AttributeDescriptor &d, DLB_ADM_TAG tag);

        int GetDescriptor(AttributeDescriptor &d, DLB_ADM_ENTITY_TYPE entityType, const std::string &name);

        int Add(dlb_adm_entity_id id);

        int SetValue(dlb_adm_entity_id id, DLB_ADM_TAG tag, const AttributeValue &value);

        int GetValue(AttributeValue &value, dlb_adm_entity_id id, DLB_ADM_TAG tag) const;

        int SetMutable(dlb_adm_entity_id id, dlb_adm_bool isMutable);

        int SetIsCommon(dlb_adm_entity_id id);

        int ForEach(DLB_ADM_ENTITY_TYPE entityType, EntityCallbackFn callbackFn, EntityFilterFn filterFn = nullptr);

        int ForEach(dlb_adm_entity_id id, AttributeCallbackFn callbackFn);

    private:
        std::unique_ptr<EntityData> mEntityData;
    };

}

#endif
