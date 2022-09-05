/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
