/************************************************************************
 * dlb_adm
 * Copyright (c) 2023-2025, Dolby Laboratories Inc.
 * Copyright (c) 2023-2025, Dolby International AB.
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

#include "XMLContainerComplementaryFlattener.h"

#include "dlb_adm/src/adm_xml/dlb_adm_xml_container.h"
#include "dlb_adm/src/adm_xml/EntityRecord.h"
#include "dlb_adm/src/adm_xml/RelationshipRecord.h"
#include "dlb_adm/include/dlb_adm_data_types.h"
#include <set>
#include <utility>
#include <functional>
#include <random>
#include <vector>
#include <algorithm>

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
    XMLContainerComplementaryFlattener::XMLContainerComplementaryFlattener(dlb_adm_xml_container &container, dlb_adm_xml_container &flattenedContainer)
        : mSourceContainer(container.GetContainer())
        , mFlattenedContainer(flattenedContainer.GetContainer())
        , mTranslator()
        , mFlattenedProgrammeIdNumber{0}
        , mSourceObjectToContent()
        , mSourceComplGroups()
        , mRegenerateFlowId(false)
    {
    }

    XMLContainerComplementaryFlattener::~XMLContainerComplementaryFlattener()
    {
        /* empty */
    }

    int XMLContainerComplementaryFlattener::InitFlattening()
    {
        RelationshipDB::RelationshipCallbackFn MapContentToObject = [&](const RelationshipRecord &r)
        {
            if(mSourceObjectToContent.count(r.toId))
            {
                return DLB_ADM_STATUS_NOT_UNIQUE;
            }
            mSourceObjectToContent[r.toId] = r.fromId;
            return DLB_ADM_STATUS_OK;
        };
        EntityDB::EntityCallbackFn TraverseContents = [&](const EntityRecord &e)
        {
            return mSourceContainer.ForEachRelationship(e.id, DLB_ADM_ENTITY_TYPE_OBJECT, MapContentToObject);
        };
        int status = mSourceContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_CONTENT, TraverseContents);
        CHECK_STATUS(status);


        RelationshipDB::RelationshipCallbackFn ListComplementaryGroupMembers = [&](const RelationshipRecord &r)
        {
            AttributeValue v;
            dlb_adm_entity_id id;
            mSourceContainer.GetValue(v, r.GetToId(), DLB_ADM_TAG_COMPLEMENTARY_OBJECT_ID_REF);
            id = boost::get<dlb_adm_entity_id>(v);
            mSourceComplGroups.back().push_back(id);
            return DLB_ADM_STATUS_OK;
        };
        EntityDB::EntityCallbackFn ListComplementaryGroups = [&](const EntityRecord &e)
        {
            int status = DLB_ADM_STATUS_OK;
            if(mSourceContainer.RelationshipExists(e.id, DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF))
            {
                mSourceComplGroups.push_back(std::vector<dlb_adm_entity_id>());
                mSourceComplGroups.back().push_back(e.id);
                status = mSourceContainer.ForEachRelationship(e.id, DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF, ListComplementaryGroupMembers);
            }
            return status; 
        };
        
        return mSourceContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_OBJECT, ListComplementaryGroups);
    }

    int XMLContainerComplementaryFlattener::Flatten()
    {
        int status = DLB_ADM_STATUS_OK;
        dlb_adm_entity_id toplevelID = mSourceContainer.GetTopLevelID();
        EntityRecord e;


        status = mFlattenedContainer.GetEntity(e, toplevelID);
        if (status == DLB_ADM_STATUS_NOT_FOUND)
        {
            status = InitFlattening();
            CHECK_STATUS(status);
            
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

    static int IsCommonDefinition(XMLContainer & container, dlb_adm_entity_id id, bool & isCommon)
    {
        int status;
        EntityRecord e;

        status = container.GetEntity(e, id);
        CHECK_STATUS(status);

        isCommon = e.status == EntityRecord::STATUS::COMMON_DEFINITION;
        return status;
    }

    int XMLContainerComplementaryFlattener::ProcessAndCopyChildEntities(dlb_adm_entity_id flattenedParentId, const RelationshipRecord &r)
    {
        int status = DLB_ADM_STATUS_OK;

        dlb_adm_entity_id sourceChildId = r.toId;
        DLB_ADM_ENTITY_TYPE childType = mTranslator.GetEntityType(sourceChildId);

        switch (childType)
        {
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
            status = ProcessProgrammeGroup(sourceChildId, flattenedParentId);
            break;

            // ComplementaryObjectGroupLabel is not copied at all
        case DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_GROUP_LABEL:
        case DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF:
            break;

        default:
        {
            // Some entities may be S-ADM Common Definition - EntityRecord::STATE must be restored for them
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

    int XMLContainerComplementaryFlattener::RecursivelyProcessAndCopyChildEntities(dlb_adm_entity_id sourceParentId, dlb_adm_entity_id flattenedParentId, RelationshipDB::RelationshipFilterFn filter)
    {
        return mSourceContainer.ForEachRelationship(sourceParentId
                                                    ,ENTITY_RELATIONSHIP::CONTAINS
                                                    ,std::bind(&XMLContainerComplementaryFlattener::ProcessAndCopyChildEntities
                                                              ,this
                                                              ,flattenedParentId
                                                              ,std::placeholders::_1)
                                                    ,filter);
    }

    static bool HasADMId(DLB_ADM_ENTITY_TYPE entityType)
    {
        return (entityType > DLB_ADM_ENTITY_TYPE_FIRST && entityType <= DLB_ADM_ENTITY_TYPE_LAST_WITH_ID);
    }

    int XMLContainerComplementaryFlattener::RecursivelyCopyChildEntities(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedParentId)
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

    int XMLContainerComplementaryFlattener::RecursivelyCopyEntityWithoutImmediateReferences
        (dlb_adm_entity_id sourceChildId
        ,dlb_adm_entity_id flattenedChildId
        ,dlb_adm_entity_id flattenedParentId
        ,RelationshipDB::RelationshipFilterFn filter
        ,std::function<bool(const DLB_ADM_TAG tag)> const& attributFilter
        )
    {
        int status = DLB_ADM_STATUS_OK;

        status = mFlattenedContainer.AddEntity(flattenedChildId);
        CHECK_STATUS(status);

        status = CopyAttributes(sourceChildId, flattenedChildId, attributFilter);
        CHECK_STATUS(status);

        status = mFlattenedContainer.AddRelationship(flattenedParentId, flattenedChildId);
        CHECK_STATUS(status);

        status = RecursivelyProcessAndCopyChildEntities(sourceChildId, flattenedChildId, filter);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static void SortObjIntoCategories( const std::vector<dlb_adm_entity_id>& objectsInProgramme
                                     , const std::vector<std::vector<dlb_adm_entity_id>>& sourceComplGroups
                                     , std::vector<dlb_adm_entity_id>& standaloneObjects
                                     , std::vector<size_t>& complGroupIndexes
                                     )
    {
        std::map<dlb_adm_entity_id, bool> IsObjectAnalyzed;
        for(const auto o : objectsInProgramme)
        {
            IsObjectAnalyzed.emplace(o, false);
        }

        for(const auto programmeObject : objectsInProgramme)
        {
            if(IsObjectAnalyzed[programmeObject])
            {
                continue;
            }

            bool foundInCmplGroup = false;
            for(size_t i = 0; i < sourceComplGroups.size(); i++)
            {
                const auto& sourceComplGroup = sourceComplGroups[i];
                foundInCmplGroup = std::find(sourceComplGroup.begin(), sourceComplGroup.end(), programmeObject) != sourceComplGroup.end();
                if(foundInCmplGroup)
                {
                    const dlb_adm_entity_id cmplLeader = sourceComplGroup[0];
                    bool cmplGroupInProgramme = false;
                    if(cmplLeader == programmeObject)
                    {
                        // check if any other cmpl group member is present in programme
                        const dlb_adm_entity_id cmplMember = sourceComplGroup[1];
                        cmplGroupInProgramme = std::find(objectsInProgramme.begin(), objectsInProgramme.end(), cmplMember) != objectsInProgramme.end();
                    }
                    else
                    {
                        cmplGroupInProgramme = std::find(objectsInProgramme.begin(), objectsInProgramme.end(), cmplLeader) != objectsInProgramme.end();
                    }

                    if(cmplGroupInProgramme)
                    {
                        complGroupIndexes.push_back(i);
                        for(const auto srcCmplObj : sourceComplGroup)
                        {
                            IsObjectAnalyzed[srcCmplObj] = true;
                        }
                    }
                    else
                    {
                        standaloneObjects.push_back(programmeObject);
                        IsObjectAnalyzed[programmeObject] = true;
                    }

                    break;
                }
            }

            if(!foundInCmplGroup)
            {
                standaloneObjects.push_back(programmeObject);
                IsObjectAnalyzed[programmeObject] = true;
            }
        }
    }

    int XMLContainerComplementaryFlattener::ProcessProgrammeGroup(const dlb_adm_entity_id sourceProgrammeId, const dlb_adm_entity_id flattenedParentId)
    {
        int status = DLB_ADM_STATUS_OK;

        std::vector<dlb_adm_entity_id> objectsInProgramme;
        RelationshipDB::RelationshipCallbackFn listObjInProgramme = [&](const RelationshipRecord &r)
        {
            objectsInProgramme.push_back(r.toId);
            return DLB_ADM_STATUS_OK;
        };
        RelationshipDB::RelationshipCallbackFn TraverseObject = [&](const RelationshipRecord &r)
        {
            return mSourceContainer.ForEachRelationship(r.toId, DLB_ADM_ENTITY_TYPE_OBJECT, listObjInProgramme);
        };
        status = mSourceContainer.ForEachRelationship(sourceProgrammeId, DLB_ADM_ENTITY_TYPE_CONTENT, TraverseObject);
        CHECK_STATUS(status);

        std::map<dlb_adm_entity_id, dlb_adm_entity_id> objToAvsInProgramme;
        if(mSourceContainer.RelationshipExists(sourceProgrammeId, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET))
        {
            RelationshipDB::RelationshipCallbackFn listAvsInProgramme = [&](const RelationshipRecord &r)
            {
                bool found = false;
                for(const auto obj : objectsInProgramme)
                {
                    if(mSourceContainer.RelationshipExists(obj, r.toId))
                    {
                        found = true;
                        objToAvsInProgramme[obj] = r.toId;
                        break;
                    }
                }

                return found ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_NOT_FOUND;
            };
            status = mSourceContainer.ForEachRelationship(sourceProgrammeId, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, listAvsInProgramme);
            CHECK_STATUS(status);
        }

        std::vector<dlb_adm_entity_id> standaloneObjects;
        std::vector<size_t> complGroupIndexes;

        SortObjIntoCategories(objectsInProgramme, mSourceComplGroups, standaloneObjects, complGroupIndexes);

        std::vector<std::vector<dlb_adm_entity_id>::iterator> cmplObjGroupCurrentCombination;
        if(complGroupIndexes.size())
        {
            mRegenerateFlowId = true;
            // generete combination of objects for each combination of complementary groups
            // an "odometer" will be used to generate combinations
            // init odometer
            std::vector<std::pair<std::vector<dlb_adm_entity_id>::iterator, std::vector<dlb_adm_entity_id>::iterator>> cmplGroupsIteratorRanges;
            for(const auto idx : complGroupIndexes)
            {
                cmplObjGroupCurrentCombination.push_back(mSourceComplGroups[idx].begin());
                cmplGroupsIteratorRanges.push_back(std::make_pair(mSourceComplGroups[idx].begin(), mSourceComplGroups[idx].end()));
            }

            // start odometer
            const size_t maxCmplGrpsIdx = complGroupIndexes.size() - 1;
            while (cmplObjGroupCurrentCombination[0] != std::get<1>(cmplGroupsIteratorRanges[0])) // if odometer wraps around - stop
            {
                status = GenerateNewProgramme(sourceProgrammeId, flattenedParentId, standaloneObjects, cmplObjGroupCurrentCombination, objToAvsInProgramme);
                CHECK_STATUS(status);

                // advance odometer
                cmplObjGroupCurrentCombination[maxCmplGrpsIdx]++;
                // if single "digit" wraps around - reset it and advance next one. It may cause a cascade of reseting and advancing
                for(int i = maxCmplGrpsIdx; i > 0 && cmplObjGroupCurrentCombination[i] == std::get<1>(cmplGroupsIteratorRanges[i]); i--)
                {
                    cmplObjGroupCurrentCombination[i] = std::get<0>(cmplGroupsIteratorRanges[i]);
                    cmplObjGroupCurrentCombination[i-1]++;
                }
            }
        }
        else
        {
            // No complementary groups in programme, plain copy programme
            const dlb_adm_entity_id newProgrammeId = GenerateNextProgrammeId();
            status = RecursivelyCopyEntityWithoutImmediateReferences(sourceProgrammeId, newProgrammeId, flattenedParentId);
            CHECK_STATUS(status);

            status = CopyReferences(sourceProgrammeId, newProgrammeId);
            CHECK_STATUS(status);
        }

        return status;
    }

    int XMLContainerComplementaryFlattener::AddLanguageToFlattenedProgramme(const dlb_adm_entity_id newProgrammeId)
    {
        int status = DLB_ADM_STATUS_OK;

        std::vector<DlbAdm::attributeString>contentLanguages;

        RelationshipDB::RelationshipCallbackFn ListContentLanguages = [&](const RelationshipRecord &r)
        {
            AttributeValue attr;
            
            int status = mSourceContainer.GetValue(attr, r.GetToId(), DLB_ADM_TAG_CONTENT_LANGUAGE);
            if (status == DLB_ADM_STATUS_OK)
            {
                contentLanguages.push_back(boost::get<DlbAdm::attributeString>(attr));
            }
            return status;
        };
        status = mFlattenedContainer.ForEachRelationship(newProgrammeId, DLB_ADM_ENTITY_TYPE_CONTENT, ListContentLanguages);
        CHECK_STATUS(status);
        
        DlbAdm::attributeString programmeLanguage{"und"};
        const DlbAdm::attributeString undefinedLanguage{"und"};
        for(const auto contentLang : contentLanguages)
        {
            if(strncmp(contentLang.data(), undefinedLanguage.data(), contentLang.size()))
            {
                programmeLanguage = contentLang;
                break; // there is no check wheter all languages in programme are the same or "und"...
            }
        }

        status = mFlattenedContainer.SetValue(newProgrammeId, DLB_ADM_TAG_PROGRAMME_LANGUAGE, programmeLanguage);
        CHECK_STATUS(status);        

        return status;
    }

    static bool IsNotProgrammeLanguage(const DLB_ADM_TAG tag)
    {
        return tag != DLB_ADM_TAG_PROGRAMME_LANGUAGE;
    }


    int XMLContainerComplementaryFlattener::GenerateNewProgramme( const dlb_adm_entity_id sourceProgrammeId
                                                                , const dlb_adm_entity_id flattenedParentId
                                                                , const std::vector<dlb_adm_entity_id>& simpleObjects
                                                                , const std::vector<std::vector<dlb_adm_entity_id>::iterator>& cmplObjGroupCurrentCombination
                                                                , const std::map<dlb_adm_entity_id, dlb_adm_entity_id>& objToAvsInProgramme)
    {
        int status = DLB_ADM_STATUS_OK;

        const dlb_adm_entity_id newProgrammeId = GenerateNextProgrammeId();
        status = RecursivelyCopyEntityWithoutImmediateReferences(sourceProgrammeId
                                                                ,newProgrammeId
                                                                ,flattenedParentId
                                                                ,nullptr
                                                                ,IsNotProgrammeLanguage);
        CHECK_STATUS(status);

        for(const auto object : simpleObjects)
        {
            status = mFlattenedContainer.AddRelationship(newProgrammeId, mSourceObjectToContent[object]);
            CHECK_STATUS(status);
            if(objToAvsInProgramme.count(object))
            {
                status = mFlattenedContainer.AddRelationship(newProgrammeId, objToAvsInProgramme.at(object));
                CHECK_STATUS(status);
            }
        }

        for(const auto objectIt : cmplObjGroupCurrentCombination)
        {
            status = mFlattenedContainer.AddRelationship(newProgrammeId, mSourceObjectToContent[*objectIt]);
            CHECK_STATUS(status);
            if(objToAvsInProgramme.count(*objectIt))
            {
                status = mFlattenedContainer.AddRelationship(newProgrammeId, objToAvsInProgramme.at(*objectIt));
                CHECK_STATUS(status);
            }
        }

        status = AddLanguageToFlattenedProgramme(newProgrammeId);
        CHECK_STATUS(status);

        return status;
    }

    dlb_adm_entity_id XMLContainerComplementaryFlattener::GenerateNextProgrammeId()
    {
        dlb_adm_entity_id nextId = DLB_ADM_NULL_ENTITY_ID;
        nextId = mTranslator.ConstructUntypedId(DLB_ADM_ENTITY_TYPE_PROGRAMME, 0x1000 + (++mFlattenedProgrammeIdNumber)); // numeration starts with 0x1001
        return nextId;
    }

    int XMLContainerComplementaryFlattener::CopyAttributes(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId, std::function<bool(const DLB_ADM_TAG tag)> const& attributFilter)
    {
        EntityDB::AttributeCallbackFn copyAttributes = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
        {
            int status = DLB_ADM_STATUS_OK;
            if(attributFilter == nullptr || attributFilter(tag))
            {
                status = mFlattenedContainer.SetValue(flattenedChildId, tag, value);
            }
            return status;
        };
        return mSourceContainer.ForEachAttribute(sourceChildId, copyAttributes);
    }

    int XMLContainerComplementaryFlattener::CopyReferences(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId, RelationshipDB::RelationshipFilterFn filter)
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

    int XMLContainerComplementaryFlattener::RegenerateFlowId()
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
