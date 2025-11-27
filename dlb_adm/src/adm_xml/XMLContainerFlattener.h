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

#ifndef DLB_XML_CONTAINER_FLATTENER_H
#define DLB_XML_CONTAINER_FLATTENER_H

#include "XMLContainer.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include <map>
#include <vector>

namespace DlbAdm
{
    /**
     * @brief Class for "flattening" content of XML Container with Emission Profile S-ADM.
     * It "unfolds" complementary and AlternateValueSets objects into new audioProgrammes, audioContents and audioObjects.
     */
    class XMLContainerFlattener : public boost::noncopyable
    {
        public:

            /**
             * @param container reference to input container
             * @param flattenedContainer reference to output container. Must be empty at start
             */
            XMLContainerFlattener
                (dlb_adm_xml_container &container
                ,dlb_adm_xml_container &flattenedContainer
                );

            ~XMLContainerFlattener();

            /**
             * @brief Flatten the S-ADM stored in container and copy it to flattenedContainer.
             * flattenedContainer must be empty at start.
             * @return DLB_ADM_STATUS_OK on success
             * */
            int Flatten();

        private:

            /**
             * @brief
             *
             * @param flattenedParentId Id of "parent" entity in mFlattenedContainer. That entity must be present in mFlattenedContainer.
             * @param r Relationship record from source container
             * @return DLB_ADM_STATUS_OK on success
             */
            int ProcessAndCopyChildEntities(dlb_adm_entity_id flattenedParentId, const RelationshipRecord &r);

            int ProcessAndCopyReferencedContentObject(dlb_adm_entity_id sourceProgrammeId
                                                     ,dlb_adm_entity_id flattenedParentId
                                                     ,std::map<uint32_t, dlb_adm_entity_id> &AltValMap
                                                     ,const RelationshipRecord &r);

            /**
             * @brief Recursively copy "child" entities contained in "parent" entity from mSourceContainer to mFlattnedContainer.
             * Recursion ends, when currently processed "parent" entity do not contain any more "child" entity.
             * audioProgramme, audioContent and audioObjects entities are being "unfold" from ComplementaryGroups and AlternativeValueSets
             * before being placed to mFlattnedContainer.
             * @param sourceParentId Id of "parent" entity in mSourceContainer.
             * @param flattenedParentId Id of "parent" entity in mFlattenedContainer. That entity must be present in mFlattenedContainer.
             * @param filter function filtering elements to copy
             * @return DLB_ADM_STATUS_OK on success
             */
            int RecursivelyProcessAndCopyChildEntities(dlb_adm_entity_id sourceParentId, dlb_adm_entity_id flattenedParentId, RelationshipDB::RelationshipFilterFn filter = nullptr);

            /**
             * @brief Copy labels, references and attributes of specified "child" entity. Creates relationship with "parent" object,
             * then recursively process and copies entities contained in "child" entity.
             * @return DLB_ADM_STATUS_OK on success
             */
            int RecursivelyCopyChildEntities(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedParentId);

            /**
             * @brief Copy labels and attributes of specified "child" entity. Creates relationship with "parent" object,
             * then recursively process and copies entities contained in "child" entity.
             * Relationships not directly contained in child entity ARE copied to flattenedContainer.
             * @param sourceChildId Id of "child" entity in mSourceContainer.
             * @param sourceChildId Id of "child" to be added to in mFlattenedContainer.
             * @param flattenedParentId Id of "parent" entity in mFlattenedContainer. That entity must be present in mFlattenedContainer.
             * @param filter function filtering elements to copy
             * @return DLB_ADM_STATUS_OK on success
             */
            int RecursivelyCopyEntityWithoutImmediateReferences
                (dlb_adm_entity_id                      sourceChildId
                ,dlb_adm_entity_id                      flattenedChildId
                ,dlb_adm_entity_id                      flattenedParentId
                ,RelationshipDB::RelationshipFilterFn   filter = nullptr);

            /**
             * @brief Copy "child "object labels, references and attributes and create relationship between child
             * and parent. Recursively process relationship to other objects if exist any.
             * @param flattenedParentId Id of "parent" entity in mFlattenedContainer: content or object.
             *        The entity must be present in mFlattenedContainer.
             * @param r Relationship record from source container
             * @return DLB_ADM_STATUS_OK on success
             */
            int PlainCopyReferencedObject(dlb_adm_entity_id flattenedParentId, const RelationshipRecord &r);

            /**
             * @brief "Unfold" audioProgramme and referenced audioContents and audioObjects from ComplementaryGroups and
             * AlternativeValueSets, and copy result to mFlattenedContainer. New programmes, contents and objects may be
             * created in mFlattenedContainer based on presence of ComplementaryGroups and AlternativeValueSets.
             * @param sourceProgrammeId Id of audioProgramme to process from mSourceContainer
             * @param flattenedParentId Id of entity that will contain copied audioProgramme, Content and Object in mFlattenedContainer
             * @return DLB_ADM_STATUS_OK on success
             */
            int ProcessProgrammeGroup(dlb_adm_entity_id sourceProgrammeId, dlb_adm_entity_id flattenedParentId);

            /**
             * @brief Process audioProgramme witch refers to AlternativeValueSets. Copy attributes/labels, then create references
             * to audioContents. If audioContent references Object affected by AlternativeValueSet, new Content/Object are created.
             * @param sourceProgrammeId Id of audioProgramme to process from mSourceContainer
             * @param flattenedParentId Id of entity that will contain copied audioProgramme, Content and Object in mFlattenedContainer
             * @return DLB_ADM_STATUS_OK on success
             */
            int ProcessProgrammeWithAVS(dlb_adm_entity_id sourceProgrammeId, dlb_adm_entity_id flattenedParentId);

            /**
             * @brief Process complementaty objects by creating new programmes for each object variant
             * @param sourceProgrammeId Id of audioProgramme to process from mSourceContainer
             * @param flattenedParentId Id of entity that will contain copied or created audioProgramme, Content and Object in mFlattenedContainer
             * @return DLB_ADM_STATUS_OK on success or error code
             */
            int ProcessProgrammeWithComplementaryObject(dlb_adm_entity_id sourceProgrammeId, dlb_adm_entity_id flattenedParentId);

            /**
             * @brief Unfolds Complementary objects to content - object map for further processing
             *
             * @param r Relationship record from source container
             * @param contentToObjects container to store map for each content
             * @return DLB_ADM_STATUS_OK on success or error code
             */
            int UnfoldProgramContent(const RelationshipRecord &r, std::vector<std::map<dlb_adm_entity_id, dlb_adm_entity_id>> &contentToObjects);

            /**
             * @brief checks the raletion content - object and fill in the map
             *
             * @param r Relationship record from source container
             * @param contentToObject container to store pairs content - object
             * @return DLB_ADM_STATUS_OK on success or error code
             */
            int MapComplementaryObjects(const RelationshipRecord &r, std::map<dlb_adm_entity_id, dlb_adm_entity_id> &contentToObject);

            /**
             * @brief Copy unfolded Complementary object to flattentd container
             *
             * @param sourceObjectId Id of audioObject to process from mSourceContainer
             * @param flattenedObjectId Id of audioObject to be added to mFlattenedContainer
             * @param flattenedParentId Id of entity that will contain copied or created audioProgramme, Content and Object in mFlattenedContainer
             * @return DLB_ADM_STATUS_OK on success or error code
             */
            int CopyObjectToContainer(dlb_adm_entity_id sourceObjectId, dlb_adm_entity_id flattenedObjectId, dlb_adm_entity_id flattenedParentId);

            /**
             * @brief Copy unfolded audioContents and audioObjects to flattentd container
             *
             * @param programmeId Program Id to which new content and object needs to be added
             * @param contentId audioContent Id to be added
             * @param objectId audioObject Id to be added
             * @param flattenedParentId Id of entity that will contain copied or created audioProgramme, Content and Object in mFlattenedContainer
             * @return DLB_ADM_STATUS_OK on success or error code
             */
            int AddContentToFlattenedProgramme(dlb_adm_entity_id programmeId, dlb_adm_entity_id contentId, dlb_adm_entity_id objectId, dlb_adm_entity_id flattenedParentId);

            /**
             * @brief Add label to newly created audioProgramme
             *
             * @param newProgrammeId Program Id to which new label needs to be added
             * @param contentId audioContent Id to deduct the label language
             * @param objectId audioObject Id to to deduct the label text
             * @return DLB_ADM_STATUS_OK on success or error code
             */
            int AddLabelToFlattenedProgramme(dlb_adm_entity_id newProgrammeId, dlb_adm_entity_id contentId, dlb_adm_entity_id objectId);

            /**
             * @brief Creates new Content/Object with specific AlternateValueSet applied and copy them to mFlattenedContainer.
             * @param[in] sourceContentId Id of source audioContent
             * @param[in] sourceContentId Id of entity that will contain copied Content and Object in mFlattenedContainer
             * @param[in] sourceContentId Id of AlternativeValueSet which will be used to create new Object
             * @param[out] newContentId output reference for Id of new Content in mFlattenedContainer
             */
            int GenerateContentObjectWithAVS
                (dlb_adm_entity_id sourceContentId
                ,dlb_adm_entity_id flattenedParentId
                ,dlb_adm_entity_id AVSId
                ,dlb_adm_entity_id &newContentId
                );

            /**
             * @return True if specified audioProgramme references any complementary object. False otherwise.
             */
            bool ComplementaryObjectPresent(dlb_adm_entity_id programmeId);

            /**
             * @return True if specified audioProgramme references any AlternativeValueSet. False otherwise.
             */
            bool AlternativeValueSetPresent(dlb_adm_entity_id programmeId);

            /**
             * @brief Return next Id and increases count in mFlattenedIdNumber of specified type
             * (either audioProgramme, Content or Object).
             */
            dlb_adm_entity_id GenerateNextId(DLB_ADM_ENTITY_TYPE entityType);

            int CopyAttributes(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId);

            int CopyReferences(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId, RelationshipDB::RelationshipFilterFn filter = nullptr);

            int RegenerateFlowId();

            XMLContainer &mSourceContainer;
            XMLContainer &mFlattenedContainer;

            AdmIdTranslator mTranslator;

            /**
             * @param Key pair of source audioContentId and AlternativeValueSetId that were used to create new
             * audioContent and audioObject
             * @param Value Id of audioContent in mFlattenedContainer created from source audioContent,
             * to whose audioObject was applied specific AlternativeValueSet. Null Id of AltValSet means no AltValSet was
             * used to create mapped entity.
             */
            std::map<std::pair<dlb_adm_entity_id, dlb_adm_entity_id>, dlb_adm_entity_id> mCreatedObjectsMap;

            /**
             * @param Key DLB_ADM_ENTITY_TYPE of either audioProgramme, audioContent or audioObject
             * @param Value number used to generate next Id of specific entity type
             */
            std::map<DLB_ADM_ENTITY_TYPE, uint32_t> mFlattenedIdNumber;

            bool mRegenerateFlowId;
    };
}

#endif
