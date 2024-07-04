/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
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

#ifndef DLB_XML_CONTAINER_COMPLEMENTARY_FLATTENER_H
#define DLB_XML_CONTAINER_COMPLEMENTARY_FLATTENER_H

#include "XMLContainer.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include <map>
#include <vector>
#include <set>

namespace DlbAdm
{
    /**
     * @brief Class for "flattening" content of XML Container with Emission Profile S-ADM.
     * It "unfolds" only complementary objects into new audioProgrammes.
     */
    class XMLContainerComplementaryFlattener : public boost::noncopyable
    {
        public:

            /**
             * @param container reference to input container
             * @param flattenedContainer reference to output container. Must be empty at start
             */
            XMLContainerComplementaryFlattener
                (dlb_adm_xml_container &container
                ,dlb_adm_xml_container &flattenedContainer
                );

            ~XMLContainerComplementaryFlattener();

            /**
             * @brief Flatten the S-ADM stored in container and copy it to flattenedContainer.
             * flattenedContainer must be empty at start.
             * @return DLB_ADM_STATUS_OK on success
             * */
            int Flatten();

        private:

            int InitFlattening();

            /**
             * @brief
             *
             * @param flattenedParentId Id of "parent" entity in mFlattenedContainer. That entity must be present in mFlattenedContainer.
             * @param r Relationship record from source container
             * @return DLB_ADM_STATUS_OK on success
             */
            int ProcessAndCopyChildEntities(dlb_adm_entity_id sourceParentId, dlb_adm_entity_id flattenedParentId, const RelationshipRecord &r);

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
             * @param attributFilter function filtering attributes to copy
             * @return DLB_ADM_STATUS_OK on success
             */
            int RecursivelyCopyEntityWithoutImmediateReferences
                (dlb_adm_entity_id                      sourceChildId
                ,dlb_adm_entity_id                      flattenedChildId
                ,dlb_adm_entity_id                      flattenedParentId
                ,RelationshipDB::RelationshipFilterFn   filter = nullptr
                ,std::function<bool(const DLB_ADM_TAG tag)> const& attributFilter = nullptr);

            int ProcessProgrammeGroup(const dlb_adm_entity_id sourceProgrammeId, const dlb_adm_entity_id flattenedParentId);

            int GenerateNewProgramme( const dlb_adm_entity_id sourceProgrammeId
                                    , const dlb_adm_entity_id flattenedParentId
                                    , const std::vector<dlb_adm_entity_id>& simpleObjects
                                    , const std::vector<std::vector<dlb_adm_entity_id>::iterator>& cmplObjGroupCurrentCombination
                                    , const std::map<dlb_adm_entity_id, dlb_adm_entity_id>& objToAvsInProgramme);

            int AddLanguageToFlattenedProgramme(const dlb_adm_entity_id newProgrammeId);

            /**
             * @brief Return next Porgramme Id and increases count in mFlattenedProgrammeIdNumber.
             */
            dlb_adm_entity_id GenerateNextProgrammeId();

            int CopyAttributes(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId, std::function<bool(const DLB_ADM_TAG tag)> const& attributFilter = nullptr);

            int CopyReferences(dlb_adm_entity_id sourceChildId, dlb_adm_entity_id flattenedChildId, RelationshipDB::RelationshipFilterFn filter = nullptr);

            int RegenerateFlowId();

            XMLContainer &mSourceContainer;
            XMLContainer &mFlattenedContainer;

            AdmIdTranslator mTranslator;

            /**
             * @brief Number used to generate next Id of flattened programme.
             */
            size_t mFlattenedProgrammeIdNumber;

            std::map<dlb_adm_entity_id, dlb_adm_entity_id> mSourceObjectToContent;
            std::vector<std::vector<dlb_adm_entity_id>> mSourceComplGroups;

            bool mRegenerateFlowId;
    };
}

#endif