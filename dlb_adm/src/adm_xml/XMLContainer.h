/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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

#ifndef DLB_ADM_XML_CONTAINER_CLASS_H
#define DLB_ADM_XML_CONTAINER_CLASS_H

#include "EntityDB.h"
#include "RelationshipDB.h"
#include <boost/interprocess/managed_heap_memory.hpp>

namespace DlbAdm
{

    class AdmIdSequenceMap;

    class XMLContainer : public boost::noncopyable
    {
    public:
        XMLContainer();
        ~XMLContainer();

        int AddEntity(const dlb_adm_entity_id &id);

        int GetEntity(EntityRecord &e, const dlb_adm_entity_id &id);

        int AddRelationship(const dlb_adm_entity_id &fromId, const dlb_adm_entity_id &toId);

        int AddEntityWithRelationship(const dlb_adm_entity_id &parentID, const dlb_adm_entity_id &id);

        int SetValue(const dlb_adm_entity_id &id, DLB_ADM_TAG tag, const AttributeValue &value);

        int GetValue(AttributeValue &value, const dlb_adm_entity_id &id, DLB_ADM_TAG tag) const;

        int SetMutable(const dlb_adm_entity_id &id, dlb_adm_bool isMutable);

        int SetIsCommon(const dlb_adm_entity_id &id);

        int ForEachEntity(DLB_ADM_ENTITY_TYPE entityType, EntityDB::EntityCallbackFn callbackFn, EntityDB::EntityFilterFn filterFn = nullptr);

        int ForEachAttribute(const dlb_adm_entity_id &id, EntityDB::AttributeCallbackFn callbackFn);

        int ForEachRelationship(RelationshipDB::RelationshipCallbackFn callbackFn);

        int ForEachRelationship(const dlb_adm_entity_id &id, ENTITY_RELATIONSHIP r, RelationshipDB::RelationshipCallbackFn callbackFn, RelationshipDB::RelationshipFilterFn filterFn = nullptr);

        int ForEachRelationship(const dlb_adm_entity_id &id, DLB_ADM_ENTITY_TYPE entityType, RelationshipDB::RelationshipCallbackFn callbackFn, RelationshipDB::RelationshipFilterFn filterFn = nullptr);

        bool RelationshipExists(const dlb_adm_entity_id &fromId, const dlb_adm_entity_id &toId);

        bool RelationshipExists(const dlb_adm_entity_id &id, DLB_ADM_ENTITY_TYPE entityType);

        size_t RelationshipCount(const dlb_adm_entity_id &id, DLB_ADM_ENTITY_TYPE entityType);

        int ReadXmlBuffer(const char *xmlBuffer, size_t characterCount, dlb_adm_bool useCommonDefs);

        int ReadXmlFile(const char *filePath, dlb_adm_bool useCommonDefs);

        int WriteXmlBuffer(dlb_adm_write_buffer_callback bufferCallback, void *callbackArg);

        int WriteXmlFile(const char *filePath);

        int LoadCommonDefs();

        int Clear();

        dlb_adm_entity_id GetTopLevelID();

        dlb_adm_entity_id GetGenericID(DLB_ADM_ENTITY_TYPE entityType);

    private:

        std::unique_ptr<EntityDB> mEntityDB;
        std::unique_ptr<RelationshipDB> mRelationshipDB;
        std::unique_ptr<AdmIdSequenceMap> mSequenceMap;
        std::unique_ptr<boost::interprocess::managed_heap_memory> mSharedMemory;
    };

}

#endif  // DLB_ADM_XML_CONTAINER_CLASS_H
