/************************************************************************
 * dlb_adm
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

#ifndef DLB_ADM_ENTITY_DB_H
#define DLB_ADM_ENTITY_DB_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "AttributeValue.h"

#include <boost/core/noncopyable.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
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
        EntityDB(boost::interprocess::managed_heap_memory &memory);
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

        void Clear();

    private:
        std::unique_ptr<EntityData> mEntityData;
    };

}

#endif
