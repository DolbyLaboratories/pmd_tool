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

#include "XMLContainerFlattener.h"

#include "dlb_adm/src/adm_xml/dlb_adm_xml_container.h"
#include "dlb_adm/src/adm_xml/EntityRecord.h"
#include "dlb_adm/src/adm_xml/RelationshipRecord.h"
#include "dlb_adm/include/dlb_adm_data_types.h"
#include <set>
#include <utility>
#include <functional>
#include <random>
#include <vector>

#ifdef NDEBUG
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)
#else
static int retstat(int s)
{
    return s;   // Put a breakpoint here
}
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return retstat(s)
#endif

namespace DlbAdm
{
    static uint32_t GetIdNumber(dlb_adm_entity_id Id)
    {
        uint32_t number;
        AdmIdTranslator translator;
        translator.DeconstructUntypedId(Id, NULL, &number, NULL);
        return number;
    }

    static int IsCommonDefinition(XMLContainer & container, dlb_adm_entity_id id, bool & isCommon)
    {
        int status;
        EntityRecord e;

        status = container.GetEntity(e, id);
        CHECK_STATUS(status);

        isCommon = e.status == EntityRecord::STATUS::COMMON_DEFINITION;
        return status;
    }

    XMLContainerFlattener::XMLContainerFlattener(dlb_adm_xml_container &container, dlb_adm_xml_container &flattenedContainer)
        : mSourceContainer(container.GetContainer())
        , mFlattenedContainer(flattenedContainer.GetContainer())
        , mTranslator()
        , mCreatedObjectsMap()
        , mFlattenedIdNumber()
        , mRegenerateFlowId(false)
    {
        EntityDB::EntityCallbackFn getMaxEntitySequenceCounter = [&](const EntityRecord &e)
        {
            uint32_t currentId = mFlattenedIdNumber[mTranslator.GetEntityType(e.id)];
            uint32_t nextId = GetIdNumber(e.id);
            if (nextId > currentId)
            {
                mFlattenedIdNumber[mTranslator.GetEntityType(e.id)] = nextId;
            }
            return DLB_ADM_STATUS_OK;
        };

        // numeration starts with 0x1001
        mFlattenedIdNumber[DLB_ADM_ENTITY_TYPE_PROGRAMME] = 0x1000;    // Start counting programms as they are unfold
        mFlattenedIdNumber[DLB_ADM_ENTITY_TYPE_CONTENT] = 0x1000;
        mFlattenedIdNumber[DLB_ADM_ENTITY_TYPE_OBJECT] = 0x1000;
        mSourceContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_CONTENT, getMaxEntitySequenceCounter);
        mSourceContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_OBJECT, getMaxEntitySequenceCounter);

    }

    XMLContainerFlattener::~XMLContainerFlattener()
    {
        /* empty */
    }

    int XMLContainerFlattener::Flatten()
    {
        int status = DLB_ADM_STATUS_OK;
        dlb_adm_entity_id toplevelID = mSourceContainer.GetTopLevelID();
        EntityRecord e;

        status = mFlattenedContainer.GetEntity(e, toplevelID);
        if (status == DLB_ADM_STATUS_NOT_FOUND)
        {
            status = mFlattenedContainer.AddEntity(toplevelID);
            CHECK_STATUS(status);

            status = RecursivelyProcessAndCopyChildEntities(toplevelID, toplevelID);
            CHECK_STATUS(status);

            if(mRegenerateFlowId)
            {
                status = RegenerateFlowId();
                CHECK_STATUS(status);
            }
        }
        else
        {
            status = DLB_ADM_STATUS_ERROR; // flattened container is not empty at start
        }
        return status;
    }

    int XMLContainerFlattener::ProcessAndCopyChildEntities(dlb_adm_entity_id flattenedParentId, const RelationshipRecord &r)
    {
        int status = DLB_ADM_STATUS_OK;

        dlb_adm_entity_id sourceChildId = r.toId;
        DLB_ADM_ENTITY_TYPE childType = mTranslator.GetEntityType(sourceChildId);

        switch (childType)
        {
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
            status = ProcessProgrammeGroup(sourceChildId, flattenedParentId);
            break;

            // audioContent and Object are being copied to new XML container in function ProcessProgrammeGroup
        case DLB_ADM_ENTITY_TYPE_CONTENT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
            // AlternativeValueSet and ComplementaryObjectGroupLabel are not copied at all
        case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
        case DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_GROUP_LABEL:
            break;

            // Some entities may be S-ADM Common Definition - EntityRecord::STATE must be restored for them
        default:
        {
            bool isCommon;
            status = RecursivelyCopyChildEntities(sourceChildId, flattenedParentId);
            CHECK_STATUS(status);
            status = IsCommonDefinition(mSourceContainer, sourceChildId, isCommon);
            CHECK_STATUS(status);
            if (isCommon)
            {
                status = mFlattenedContainer.SetIsCommon(sourceChildId);
            }
        }
        break;
        }
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif

        return status;
    }

    int XMLContainerFlattener::ProcessAndCopyReferencedContentObject
        (dlb_adm_entity_id sourceProgrammeId
        ,dlb_adm_entity_id flattenedParentId
        ,std::map<uint32_t, dlb_adm_entity_id> &AltValMap
        ,const RelationshipRecord &r
        )
    {
        int status = DLB_ADM_STATUS_OK;
        dlb_adm_entity_id sourceContentId = r.GetToId();
        uint32_t contentIdNumber = GetIdNumber(sourceContentId);

        if (!AltValMap.count(contentIdNumber))
        {
            // content do not references AltValSet
            auto key = std::make_pair(sourceContentId, DLB_ADM_NULL_ENTITY_ID);
            if (!mCreatedObjectsMap.count(key))
            {
                // content/object is beeing processed for the first time
                status = RecursivelyCopyChildEntities(sourceContentId, flattenedParentId);
                CHECK_STATUS(status);

                dlb_adm_entity_id sourceObjectId = mTranslator.ConstructUntypedId(DLB_ADM_ENTITY_TYPE_OBJECT, contentIdNumber);
                status = RecursivelyCopyChildEntities(sourceObjectId, flattenedParentId);
                CHECK_STATUS(status);

                mCreatedObjectsMap[key] = sourceContentId;
            }

            status = mFlattenedContainer.AddRelationship(sourceProgrammeId, mCreatedObjectsMap[key]);
            CHECK_STATUS(status);
        }
        else
        {
            // content references AltValSet
            dlb_adm_entity_id flattenedContentId;
            auto key = std::make_pair(sourceContentId, AltValMap[contentIdNumber]);
            if(!mCreatedObjectsMap.count(key))
            {
                // content/object is beeing processed for the first time
                status = GenerateContentObjectWithAVS(sourceContentId, flattenedParentId, AltValMap[contentIdNumber], flattenedContentId);
                CHECK_STATUS(status);
            }
            else
            {
                flattenedContentId = mCreatedObjectsMap[key];
            }

            status = mFlattenedContainer.AddRelationship(sourceProgrammeId, flattenedContentId);
        }
        return status;
    }

    int XMLContainerFlattener::RecursivelyProcessAndCopyChildEntities(dlb_adm_entity_id sourceParentId, dlb_adm_entity_id flattenedParentId, RelationshipDB::RelationshipFilterFn filter)
    {
        return mSourceContainer.ForEachRelationship(sourceParentId
                                                    ,ENTITY_RELATIONSHIP::CONTAINS
                                                    ,std::bind(&XMLContainerFlattener::ProcessAndCopyChildEntities
                                                              ,this
                                                              ,flattenedParentId
                                                              ,std::placeholders::_1)
                                                    ,filter);
    }

    static bool HasADMId(DLB_ADM_ENTITY_TYPE entityType)
    {
        return (entityType > DLB_ADM_ENTITY_TYPE_FIRST && entityType <= DLB_ADM_ENTITY_TYPE_LAST_WITH_ID);
    }

    int XMLContainerFlattener::RecursivelyCopyChildEntities(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedParentId)
    {
        int status = DLB_ADM_STATUS_OK;

        DLB_ADM_ENTITY_TYPE childType = mTranslator.GetEntityType(sourceChildId);
        dlb_adm_entity_id newChildId = HasADMId(childType) ? sourceChildId : mFlattenedContainer.GetGenericID(childType);

        status = RecursivelyCopyEntityWithoutImmediateReferences(sourceChildId, newChildId, flattenedParentId);
        CHECK_STATUS(status);

        status = CopyReferences(sourceChildId, newChildId);
        CHECK_STATUS(status);

        return status;
    }

    int XMLContainerFlattener::RecursivelyCopyEntityWithoutImmediateReferences
        (dlb_adm_entity_id sourceChildId
        ,dlb_adm_entity_id flattenedChildId
        ,dlb_adm_entity_id flattenedParentId
        ,RelationshipDB::RelationshipFilterFn filter
        )
    {
        int status = DLB_ADM_STATUS_OK;

        status = mFlattenedContainer.AddEntity(flattenedChildId);
        CHECK_STATUS(status);

        // copy attributes
        status = CopyAttributes(sourceChildId, flattenedChildId);
        CHECK_STATUS(status);

        // create relationship parent/child
        status = mFlattenedContainer.AddRelationship(flattenedParentId, flattenedChildId);
        CHECK_STATUS(status);

        // for each containing entity, call RecursivelyProcessAndCopyChildEntities
        status = RecursivelyProcessAndCopyChildEntities(sourceChildId, flattenedChildId, filter);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLContainerFlattener::PlainCopyReferencedObject(dlb_adm_entity_id flattenedParentId, const RelationshipRecord &r)
    {
        int status;
        dlb_adm_entity_id sourceContentId = r.GetToId();
        status = RecursivelyCopyChildEntities(sourceContentId, flattenedParentId);
        CHECK_STATUS(status);
        /* copy reference object-object */
        status = mSourceContainer.ForEachRelationship(sourceContentId,
                                                      DLB_ADM_ENTITY_TYPE_OBJECT,
                                                      std::bind(&XMLContainerFlattener::PlainCopyReferencedObject
                                                                ,this
                                                                ,flattenedParentId
                                                                ,std::placeholders::_1));
        CHECK_STATUS(status);

        return status;
    }

    int XMLContainerFlattener::ProcessProgrammeGroup(dlb_adm_entity_id sourceProgrammeId, dlb_adm_entity_id flattenedParentId)
    {
        int status = DLB_ADM_STATUS_OK;

        if(ComplementaryObjectPresent(sourceProgrammeId))
        {
            // create new programme according to Emission Profile alghoritm
            status = ProcessProgrammeWithComplementaryObject(sourceProgrammeId, flattenedParentId);
            mRegenerateFlowId = true;
        }
        else if(AlternativeValueSetPresent(sourceProgrammeId))
        {
            status = ProcessProgrammeWithAVS(sourceProgrammeId, flattenedParentId);
            mRegenerateFlowId = true;
        }
        else
        {
            // plain copy of programme and associated audio content and objects
            status = RecursivelyCopyChildEntities(sourceProgrammeId, flattenedParentId);
            CHECK_STATUS(status);

            RelationshipDB::RelationshipCallbackFn PlainCopyReferencedContentAndObjects = [&](const RelationshipRecord &r)
            {
                int status = DLB_ADM_STATUS_OK;
                dlb_adm_entity_id sourceContentId = r.GetToId();
                auto key = std::make_pair(sourceContentId, DLB_ADM_NULL_ENTITY_ID);
                if(!mCreatedObjectsMap.count(key))
                {
                    status = RecursivelyCopyChildEntities(sourceContentId, flattenedParentId);
                    CHECK_STATUS(status);
                    status = mSourceContainer.ForEachRelationship(sourceContentId, DLB_ADM_ENTITY_TYPE_OBJECT, std::bind(&XMLContainerFlattener::PlainCopyReferencedObject
                        ,this
                        ,flattenedParentId
                        ,std::placeholders::_1));
                    CHECK_STATUS(status);
                }

                mCreatedObjectsMap[key] = sourceContentId;

                return status;
            };
            status = mSourceContainer.ForEachRelationship(sourceProgrammeId, DLB_ADM_ENTITY_TYPE_CONTENT, PlainCopyReferencedContentAndObjects);
        }

        return status;
    }

    int XMLContainerFlattener::ProcessProgrammeWithAVS(dlb_adm_entity_id sourceProgrammeId, dlb_adm_entity_id flattenedParentId)
    {
        int status = DLB_ADM_STATUS_OK;

        status = RecursivelyCopyEntityWithoutImmediateReferences(sourceProgrammeId, sourceProgrammeId, flattenedParentId); // programme Id stays the same
        CHECK_STATUS(status);

        uint32_t objectIdNumber;
        std::map<uint32_t, dlb_adm_entity_id> AltValMap; // key - content/object Id number, value - AltValSet Id

        // map which content/object references which AlternativeValueSet
        status = mSourceContainer.ForEachRelationship(sourceProgrammeId
                                                     ,DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET
                                                     ,[&](const RelationshipRecord &r)
                                                          {
                                                               objectIdNumber = GetIdNumber(r.GetToId());
                                                               AltValMap[objectIdNumber] = r.GetToId();
                                                               return DLB_ADM_STATUS_OK;
                                                          });
        CHECK_STATUS(status);

        // process contents/objects referenced by current programme
        status = mSourceContainer.ForEachRelationship(sourceProgrammeId
                                                     ,DLB_ADM_ENTITY_TYPE_CONTENT
                                                     ,std::bind(&XMLContainerFlattener::ProcessAndCopyReferencedContentObject
                                                               ,this
                                                               ,sourceProgrammeId
                                                               ,flattenedParentId
                                                               ,AltValMap
                                                               ,std::placeholders::_1));

        return status;
    }

    int XMLContainerFlattener::MapComplementaryObjects(const RelationshipRecord &r, std::map<dlb_adm_entity_id, dlb_adm_entity_id> &contentToObject)
    {
        // Add main content and object to map
        contentToObject[r.GetFromId()] = r.GetToId();

        RelationshipDB::RelationshipCallbackFn FindReferencingContent = [&](const RelationshipRecord &r)
        {
            AttributeValue v;
            if (DLB_ADM_STATUS_NOT_FOUND == mFlattenedContainer.GetValue(v, r.GetToId(), DLB_ADM_TAG_CONTENT_ID))
            {
                contentToObject[r.GetToId()] = r.GetFromId();
            }
            return DLB_ADM_STATUS_OK;
        };

        RelationshipDB::RelationshipCallbackFn FindRef = [&](const RelationshipRecord &r)
        {
            AttributeValue v;
            dlb_adm_entity_id id;
            mSourceContainer.GetValue(v, r.GetToId(), DLB_ADM_TAG_COMPLEMENTARY_OBJECT_ID_REF);
            id = boost::get<dlb_adm_entity_id>(v);
            mSourceContainer.ForEachRelationship(id, ENTITY_RELATIONSHIP::REFERENCED_BY, FindReferencingContent);
            return DLB_ADM_STATUS_OK;
        };

        return mSourceContainer.ForEachRelationship(r.GetToId()
                                                   ,DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF
                                                   ,FindRef);
    }

    int XMLContainerFlattener::UnfoldProgramContent(const RelationshipRecord &r, std::vector<std::map<dlb_adm_entity_id, dlb_adm_entity_id>> &contentToObjects)
    {
        std::map<dlb_adm_entity_id, dlb_adm_entity_id> contentToObject;
        contentToObjects.push_back(contentToObject);

        return mSourceContainer.ForEachRelationship(r.GetToId()
                                                   ,DLB_ADM_ENTITY_TYPE_OBJECT
                                                   ,std::bind(&XMLContainerFlattener::MapComplementaryObjects
                                                             ,this
                                                             ,std::placeholders::_1
                                                             ,std::ref(contentToObjects.back())));
    }

    int XMLContainerFlattener::CopyObjectToContainer(dlb_adm_entity_id sourceObjectId, dlb_adm_entity_id flattenedObjectId, dlb_adm_entity_id flattenedParentId)
    {
        int status = DLB_ADM_STATUS_OK;

        status = mFlattenedContainer.AddEntity(flattenedObjectId);
        CHECK_STATUS(status);
        // copy attributes
        status = CopyAttributes(sourceObjectId, flattenedObjectId);
        CHECK_STATUS(status);
        status = CopyReferences(sourceObjectId
                               ,flattenedObjectId
                               ,[&](const RelationshipRecord &r)
                                    {
                                        dlb_adm_entity_id sourceChildId = r.toId;
                                        return DLB_ADM_ENTITY_TYPE_OBJECT != mTranslator.GetEntityType(sourceChildId);
                                    });
        CHECK_STATUS(status);
        // create relationship parent/child
        status = mFlattenedContainer.AddRelationship(flattenedParentId, flattenedObjectId);
        CHECK_STATUS(status);

        return status;
    }

    int XMLContainerFlattener::AddContentToFlattenedProgramme(dlb_adm_entity_id newProgrammeId, dlb_adm_entity_id contentId, dlb_adm_entity_id objectId, dlb_adm_entity_id flattenedParentId)
    {
        /* Add Content to container */
        EntityRecord entity;
        int status = mFlattenedContainer.GetEntity(entity, contentId);
        if (status == DLB_ADM_STATUS_NOT_FOUND)
        {
            status = RecursivelyCopyChildEntities(contentId, flattenedParentId);
        }
        CHECK_STATUS(status);

        /* Add Object to container */
        status = CopyObjectToContainer(objectId, objectId, flattenedParentId);
        CHECK_STATUS(status);

        /* Add Relationship to Programme -> Content */
        status = mFlattenedContainer.AddRelationship(newProgrammeId, contentId);
        CHECK_STATUS(status);
        /* Add Relationship to Content -> Object */
        status = mFlattenedContainer.AddRelationship(contentId, objectId);
        return status;
    }

    int XMLContainerFlattener::AddLabelToFlattenedProgramme(dlb_adm_entity_id newProgrammeId, dlb_adm_entity_id contentId, dlb_adm_entity_id objectId)
    {
        int status = DLB_ADM_STATUS_OK;
        AttributeValue programmeLanguage, labelLanguage, labelText;

        mSourceContainer.GetValue(programmeLanguage, contentId, DLB_ADM_TAG_CONTENT_LANGUAGE);
        mFlattenedContainer.SetValue(newProgrammeId, DLB_ADM_TAG_PROGRAMME_LANGUAGE, programmeLanguage);
        /* Copy new Programme label form complementary object */
        mSourceContainer.ForEachRelationship(objectId
                                            ,DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_GROUP_LABEL
                                            ,[&](const RelationshipRecord &r)   // Get the label for coresponding language
                                            {
                                                mSourceContainer.GetValue(labelLanguage
                                                                         ,r.GetToId()
                                                                         ,DLB_ADM_TAG_COMPLEMENTARY_OBJECT_GROUP_LABEL_LANGUAGE);
                                                return mSourceContainer.GetValue(labelText
                                                                                ,r.GetToId()
                                                                                ,DLB_ADM_TAG_COMPLEMENTARY_OBJECT_GROUP_LABEL_VALUE);
                                            }
                                            ,[&](const RelationshipRecord &r)   // Filter
                                            {
                                                AttributeValue tmpLabelLanguage;
                                                mSourceContainer.GetValue(tmpLabelLanguage
                                                                         ,r.GetToId()
                                                                         ,DLB_ADM_TAG_COMPLEMENTARY_OBJECT_GROUP_LABEL_LANGUAGE);
                                                DlbAdm::attributeString programmLang = boost::get<DlbAdm::attributeString>(programmeLanguage);
                                                DlbAdm::attributeString tempLang = boost::get<DlbAdm::attributeString>(tmpLabelLanguage);                      
                                                return (strncmp(programmLang.data(), tempLang.data(), programmLang.size()) == 0);
                                            });

        dlb_adm_entity_id labelId = mFlattenedContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL);
        status = mFlattenedContainer.AddEntity(labelId);
        CHECK_STATUS(status);
        if (labelLanguage.which() == 0)
        {
            labelLanguage = convertToAttributeString(std::string("und"));
            labelText = convertToAttributeString(std::string("None"));
        }
        status = mFlattenedContainer.SetValue(labelId, DLB_ADM_TAG_PROGRAMME_LABEL_LANGUAGE, labelLanguage);
        CHECK_STATUS(status);
        status = mFlattenedContainer.SetValue(labelId, DLB_ADM_TAG_PROGRAMME_LABEL_VALUE, labelText);
        CHECK_STATUS(status);
        status = mFlattenedContainer.AddRelationship(newProgrammeId, labelId);

        return status;
    }

    int XMLContainerFlattener::ProcessProgrammeWithComplementaryObject(dlb_adm_entity_id sourceProgrammeId, dlb_adm_entity_id flattenedParentId)
    {
        int status = DLB_ADM_STATUS_OK;
        /* For each Content in Program it holds map <Content -> Complementary object> */
        std::vector<std::map<dlb_adm_entity_id, dlb_adm_entity_id>> contentToObjects;

        unsigned int newProgrammeCount = 0;
        unsigned int newProgrammesNeeded = 0;

        mSourceContainer.ForEachRelationship(sourceProgrammeId
                                            ,DLB_ADM_ENTITY_TYPE_CONTENT
                                            ,std::bind(&XMLContainerFlattener::UnfoldProgramContent
                                                      ,this
                                                      ,std::placeholders::_1
                                                      ,std::ref(contentToObjects)));

        // Count how many new programmes are needed
        for (auto map : contentToObjects)
        {
            newProgrammesNeeded = map.size() > newProgrammesNeeded ? map.size() : newProgrammesNeeded;
        }

        do
        {
            /* Create Programm */
            dlb_adm_entity_id newProgrammeId = GenerateNextId(DLB_ADM_ENTITY_TYPE_PROGRAMME);
            status = RecursivelyCopyEntityWithoutImmediateReferences(sourceProgrammeId
                                                                    ,newProgrammeId
                                                                    ,flattenedParentId
                                                                    ,[&](const RelationshipRecord &r)   // Filter - do not copy PROGRAMME_LABEL it will be added later
                                                                    {
                                                                        dlb_adm_entity_id sourceChildId = r.toId;
                                                                        return DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL != mTranslator.GetEntityType(sourceChildId);
                                                                    });
            CHECK_STATUS(status);

            /* Assign content to Programm */
            unsigned int currentContent = 0;
            for (auto map : contentToObjects)
            {
                /* Simple content */
                if (map.size() == 1)
                {
                    status = AddContentToFlattenedProgramme(newProgrammeId, map.begin()->first, map.begin()->second, flattenedParentId);
                    CHECK_STATUS(status);
                }
                /* Complementary Content */
                else
                {
                    for (auto it = map.begin(); it != map.end(); ++it, ++currentContent)
                    {
                        if (currentContent == newProgrammeCount)
                        {
                            status = AddContentToFlattenedProgramme(newProgrammeId, it->first, it->second, flattenedParentId);
                            CHECK_STATUS(status);
                            status = AddLabelToFlattenedProgramme(newProgrammeId, it->first, map.begin()->second);
                            CHECK_STATUS(status);
                            break;
                        }
                    }
                }
            }
            ++newProgrammeCount;
        } while (newProgrammeCount != newProgrammesNeeded);

        return status;
    }

    int XMLContainerFlattener::GenerateContentObjectWithAVS
        (dlb_adm_entity_id sourceContentId
        ,dlb_adm_entity_id flattenedParentId
        ,dlb_adm_entity_id AltValId
        ,dlb_adm_entity_id &newContentId
        )
    {
        int status = DLB_ADM_STATUS_OK;

        uint32_t contentIdNumber = GetIdNumber(sourceContentId);
        dlb_adm_entity_id sourceObjectId = mTranslator.ConstructUntypedId(DLB_ADM_ENTITY_TYPE_OBJECT, contentIdNumber);

        // copy content with new Id applied
        newContentId = GenerateNextId(DLB_ADM_ENTITY_TYPE_CONTENT);
        if(newContentId == DLB_ADM_NULL_ENTITY_ID)
        {
            return DLB_ADM_STATUS_ERROR;
        }
        status = RecursivelyCopyEntityWithoutImmediateReferences(sourceContentId, newContentId, flattenedParentId);
        CHECK_STATUS(status);

        status = mFlattenedContainer.SetValue(newContentId, DLB_ADM_TAG_CONTENT_ID, convertToAttributeString(mTranslator.Translate(newContentId)));
        CHECK_STATUS(status);

        // create new object with new Id and with AVS value applied
        dlb_adm_entity_id newObjectId = GenerateNextId(DLB_ADM_ENTITY_TYPE_OBJECT);
        if(newObjectId == DLB_ADM_NULL_ENTITY_ID)
        {
            return DLB_ADM_STATUS_ERROR;
        }
        status = mFlattenedContainer.AddEntity(newObjectId);
        CHECK_STATUS(status);

        status = CopyAttributes(sourceObjectId, newObjectId);
        CHECK_STATUS(status);

        status = mFlattenedContainer.SetValue(newObjectId, DLB_ADM_TAG_OBJECT_ID, convertToAttributeString(mTranslator.Translate(newObjectId)));
        CHECK_STATUS(status);

        status = mFlattenedContainer.AddRelationship(flattenedParentId, newObjectId);
        CHECK_STATUS(status);

        status = CopyReferences(sourceObjectId ,newObjectId);
        CHECK_STATUS(status);

        // copy sub-elements from AVS to new object
        std::set<DLB_ADM_ENTITY_TYPE> AltValElems;
        RelationshipDB::RelationshipCallbackFn CopyAltValSetAttributes = [&](const RelationshipRecord &r)
        {
            dlb_adm_entity_id elementId = r.GetToId();
            DLB_ADM_ENTITY_TYPE entityType = mTranslator.GetEntityType(elementId);
            AltValElems.insert(entityType);
            return RecursivelyCopyChildEntities(elementId, newObjectId);
        };
        status = mSourceContainer.ForEachRelationship(AltValId, ENTITY_RELATIONSHIP::CONTAINS, CopyAltValSetAttributes);

        // copy sub-elements not affected by AVS to new object
        RelationshipDB::RelationshipCallbackFn CopyNonAltValSetAttributes = [&](const RelationshipRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;
            dlb_adm_entity_id subElementId = r.GetToId();
            DLB_ADM_ENTITY_TYPE subElementType = mTranslator.GetEntityType(subElementId);

            // don't copy elems from AVS and AVS itself
            if  (   !   (  AltValElems.count(subElementType)
                        ||  subElementType == DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET
                        )
                )
            {
                status = RecursivelyCopyChildEntities(subElementId, newObjectId);
            }
            return status;
        };
        status = mSourceContainer.ForEachRelationship(sourceObjectId, ENTITY_RELATIONSHIP::CONTAINS, CopyNonAltValSetAttributes);
        CHECK_STATUS(status);

        // create relationship content-object
        status = mFlattenedContainer.AddRelationship(newContentId, newObjectId);
        CHECK_STATUS(status);

        mCreatedObjectsMap[std::make_pair(sourceContentId, AltValId)] = newContentId;

        return status;
    }

    bool XMLContainerFlattener::ComplementaryObjectPresent(dlb_adm_entity_id programmeId)
    {
        bool complObjPresent = false;

        RelationshipDB::RelationshipCallbackFn detectComplementaryObject = [&](const RelationshipRecord &r)
        {
            complObjPresent |= mSourceContainer.RelationshipExists(r.GetToId(), DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF);
            return DLB_ADM_STATUS_OK;
        };

        RelationshipDB::RelationshipCallbackFn traverseContent = [&](const RelationshipRecord &r)
        {
            return mSourceContainer.ForEachRelationship(r.GetToId(), DLB_ADM_ENTITY_TYPE_OBJECT, detectComplementaryObject);
        };

        mSourceContainer.ForEachRelationship(programmeId, DLB_ADM_ENTITY_TYPE_CONTENT, traverseContent);
        return complObjPresent;
    }

    bool XMLContainerFlattener::AlternativeValueSetPresent(dlb_adm_entity_id programmeId)
    {
        return mSourceContainer.RelationshipExists(programmeId, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET);
    }

    dlb_adm_entity_id XMLContainerFlattener::GenerateNextId(DLB_ADM_ENTITY_TYPE entityType)
    {
        dlb_adm_entity_id nextId = DLB_ADM_NULL_ENTITY_ID;

        switch(entityType)
        {
            case DLB_ADM_ENTITY_TYPE_PROGRAMME:
            case DLB_ADM_ENTITY_TYPE_CONTENT:
            case DLB_ADM_ENTITY_TYPE_OBJECT:
                {
                    nextId = mTranslator.ConstructUntypedId(entityType, ++mFlattenedIdNumber[entityType]);
                }
            default:
                break;
        }
        return nextId;
    }

    int XMLContainerFlattener::CopyAttributes(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId)
    {
        EntityDB::AttributeCallbackFn copyAttributes = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
        {
            return mFlattenedContainer.SetValue(flattenedChildId, tag, value);
        };
        return mSourceContainer.ForEachAttribute(sourceChildId, copyAttributes);
    }

    int XMLContainerFlattener::CopyReferences(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId, RelationshipDB::RelationshipFilterFn filter)
    {
        RelationshipDB::RelationshipCallbackFn copyReferences = [&](const RelationshipRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;
            status = mFlattenedContainer.AddEntity(r.toId);
            CHECK_STATUS(status);
            status = mFlattenedContainer.AddRelationship(flattenedChildId, r.toId);
            return status;
        };
        return mSourceContainer.ForEachRelationship(sourceChildId, ENTITY_RELATIONSHIP::REFERENCES, copyReferences, filter);
    }

    static
    dlb_adm_entity_id
    GetFrameFormatId()
    {
        return AdmIdTranslator().ConstructUntypedId(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, 1, 0);
    }

    static
    int
    generateRandomBytes
        ( std::vector<unsigned char> & bytesContainer
        , size_t count
        )
    {
        constexpr size_t BYTE_MAX_VALUE = 255;

        try
        {
            std::random_device os_seed;
            std::mt19937 generator(os_seed());
            std::uniform_int_distribution<unsigned short> distribute(0, BYTE_MAX_VALUE);

            for(size_t i = 0; i < count; i++)
            {
                bytesContainer.at(i) = distribute(generator);
            }
        }
        catch(...)
        {
            return DLB_ADM_STATUS_ERROR;
        }
        return DLB_ADM_STATUS_OK;
    }

    static
    int
    generateRandomFlowId
        (std::string & generatedFlowId)
    {
        constexpr size_t BYTES_NEEDED_FOR_FLOW_ID = 16;
        int status;

        std::vector<unsigned char> randomBytes = std::vector<unsigned char>(BYTES_NEEDED_FOR_FLOW_ID);
        char uuidChars[DLB_ADM_DATA_FF_UUID_SZ];

        memset(uuidChars, 0, DLB_ADM_DATA_FF_UUID_SZ);

        status = generateRandomBytes(randomBytes, BYTES_NEEDED_FOR_FLOW_ID);
        CHECK_STATUS(status);

        randomBytes[6] = (randomBytes[6] & 0x0f) | 0x40;
        randomBytes[8] = (randomBytes[8] & 0x3f) | 0x80;

        /** Format of UUID/flowId: (8 hex digits)-(4 hex digits)-(4 hex digits)-(4 hex digits)-(12 hex digits)
         *  e.g.: 12345678-abcd-4000-a000-112233445566
         */

        size_t currentByte = 0;
        char  *currentUuidChars = uuidChars;
        size_t i,j;
        for (i = 0; i < 4; i++)
        {
            sprintf(currentUuidChars, "%02hhx", randomBytes[currentByte++]);
            currentUuidChars += 2;
        }
        sprintf(currentUuidChars++, "-");

        for ( i = 0; i < 3; i++)
        {
            for ( j = 0; j < 2; j++)
            {
                sprintf(currentUuidChars, "%02hhx", randomBytes[currentByte++]);
                currentUuidChars += 2;
            }
            sprintf(currentUuidChars++, "-");
        }

        for (i = 0; i < 6; i++)
        {
            sprintf(currentUuidChars, "%02hhx", randomBytes[currentByte++]);
            currentUuidChars += 2;
        }

        generatedFlowId = std::string(uuidChars);

        return DLB_ADM_STATUS_OK;
    }

    int XMLContainerFlattener::RegenerateFlowId()
    {
        std::string newFlowId;
        int status;
        dlb_adm_entity_id frameFormatId = GetFrameFormatId();

        status = generateRandomFlowId(newFlowId);
        CHECK_STATUS(status);

        status = mFlattenedContainer.SetValue(frameFormatId, DLB_ADM_TAG_FRAME_FORMAT_FLOW_ID, convertToAttributeString(newFlowId));

        return status;
    }
}
