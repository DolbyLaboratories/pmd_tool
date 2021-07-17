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

#ifndef DLB_ADM_XML_CONTAINER_CLASS_H
#define DLB_ADM_XML_CONTAINER_CLASS_H

#include "EntityDB.h"
#include "RelationshipDB.h"

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

        dlb_adm_entity_id GetTopLevelID();

        dlb_adm_entity_id GetGenericID(DLB_ADM_ENTITY_TYPE entityType);

    private:
        int LoadCommonDefs();

        EntityDB mEntityDB;
        RelationshipDB mRelationshipDB;
        std::unique_ptr<AdmIdSequenceMap> mSequenceMap;
    };

}

#endif  // DLB_ADM_XML_CONTAINER_CLASS_H
