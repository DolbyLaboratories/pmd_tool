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

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "dlb_adm/include/dlb_adm_api.h"

#include "EntityContainer.h"
#include "AttributeDescriptor.h"
#include "RelationshipContainer.h"

#include "AdmIdTranslator.h"

TEST(dlb_adm_test, RelationshipQueryBasic)
{
    using namespace DlbAdm;

    AdmIdTranslator translator;

    static const char   *programmeIdStr = "APR_1001";
    static const char   *contentIdStr1  = "ACO_1001";
    static const char   *contentIdStr2  = "ACO_1002";
    static const char   *contentIdStr3  = "ACO_1003";
    static const char   *objectIdStr1   = "AO_1001";
    static const char   *objectIdStr2   = "AO_1002";

    uint32_t programmeLabelCount = 0;
    uint32_t contentLabelCount = 0;

    dlb_adm_entity_id    programmeId;
    dlb_adm_entity_id    programmeLabelId1;
    dlb_adm_entity_id    programmeLabelId2;
    dlb_adm_entity_id    programmeLabelId3;

    dlb_adm_entity_id    contentId1;
    dlb_adm_entity_id    contentLabelId1a;
    dlb_adm_entity_id    contentLabelId1b;
    dlb_adm_entity_id    contentLabelId1c;

    dlb_adm_entity_id    contentId2;
    dlb_adm_entity_id    contentId3;    // Note: not added to container

    dlb_adm_entity_id    objectId1;
    dlb_adm_entity_id    objectId2;

    int                  status;

    status = ::dlb_adm_read_entity_id(&programmeId, programmeIdStr, ::strlen(programmeIdStr) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    programmeLabelId1 = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, ++programmeLabelCount);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, programmeLabelId1);
    programmeLabelId2 = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, ++programmeLabelCount);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, programmeLabelId2);
    programmeLabelId3 = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, ++programmeLabelCount);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, programmeLabelId3);

    status = ::dlb_adm_read_entity_id(&contentId1, contentIdStr1, ::strlen(contentIdStr1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    contentLabelId1a = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, ++contentLabelCount);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, contentLabelId1a);
    contentLabelId1b = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, ++contentLabelCount);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, contentLabelId1b);
    contentLabelId1c = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, ++contentLabelCount);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, contentLabelId1c);

    status = ::dlb_adm_read_entity_id(&contentId2, contentIdStr2, ::strlen(contentIdStr2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_read_entity_id(&contentId3, contentIdStr3, ::strlen(contentIdStr3) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_read_entity_id(&objectId1, objectIdStr1, ::strlen(objectIdStr1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_read_entity_id(&objectId2, objectIdStr2, ::strlen(objectIdStr2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    try
    {
        RelationshipContainer container;
        RelationshipRecord record;
	    const bool isFalse = false;

        // Add audio programme relationships

        record.fromId = programmeId;
        record.toId = contentId1;
        record.relationship = ENTITY_RELATIONSHIP::REFERENCES;

        auto insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);
        insertResult = container.insert(record);
        EXPECT_EQ(isFalse, insertResult.second); // No duplicates allowed

        record.toId = contentId2;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = programmeLabelId1;
        record.relationship = ENTITY_RELATIONSHIP::CONTAINS;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = programmeLabelId2;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = programmeLabelId3;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = programmeId;
        record.relationship = ENTITY_RELATIONSHIP::CONTAINED_BY;
        record.fromId = programmeLabelId1;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.fromId = programmeLabelId2;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.fromId = programmeLabelId3;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        // Add audio content relationships

        record.fromId = contentId1;
        record.toId = objectId1;
        record.relationship = ENTITY_RELATIONSHIP::REFERENCES;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = contentLabelId1a;
        record.relationship = ENTITY_RELATIONSHIP::CONTAINS;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = contentLabelId1b;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = contentLabelId1c;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.fromId = contentId2;
        record.toId = objectId2;
        record.relationship = ENTITY_RELATIONSHIP::REFERENCES;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.toId = contentId1;
        record.relationship = ENTITY_RELATIONSHIP::CONTAINED_BY;
        record.fromId = contentLabelId1a;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.fromId = contentLabelId1b;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        record.fromId = contentLabelId1c;
        insertResult = container.insert(record);
        EXPECT_EQ(true, insertResult.second);

        // Test relationship queries

        RelationshipContainer_PKIndex &index = container.get<RelationshipContainer_PK>();
        RelationshipContainer_PKIndex::iterator indexEnd = index.end();

        // Ask for non-existent relationship between the programme and content 3
        RelationshipContainer_PKIndex::iterator it =
            index.find(RelationshipTuple(programmeId, ENTITY_RELATIONSHIP::REFERENCES, translator.GetEntityType(contentId3), contentId3));
        EXPECT_EQ(indexEnd, it);

        // Find the specific relationship between the programme and content 1
        it = index.find(RelationshipTuple(programmeId, ENTITY_RELATIONSHIP::REFERENCES, translator.GetEntityType(contentId1), contentId1));
        EXPECT_NE(indexEnd, it);

        // Find all the content entities referred to by the programme
        auto contentTuple = std::make_tuple(programmeId, ENTITY_RELATIONSHIP::REFERENCES, DLB_ADM_ENTITY_TYPE_CONTENT);
        RelationshipContainer_PKIndex::iterator lowerIt = index.lower_bound(contentTuple);
        RelationshipContainer_PKIndex::iterator upperIt = index.upper_bound(contentTuple);
        size_t n = 0;

        while (lowerIt != upperIt)
        {
            lowerIt++;
            n++;
        }
        EXPECT_EQ(2u, n);

        // Find all the content label entities contained by content 1
        auto contentLableTuple1 = std::make_tuple(contentId1, ENTITY_RELATIONSHIP::CONTAINS, DLB_ADM_ENTITY_TYPE_CONTENT_LABEL);
        lowerIt = index.lower_bound(contentLableTuple1);
        upperIt = index.upper_bound(contentLableTuple1);
        n = 0;
        while (lowerIt != upperIt)
        {
            lowerIt++;
            n++;
        }
        EXPECT_EQ(3u, n);

        // Find all the content label entities contained by content 2 (i.e., none)
        auto contentLableTuple2 = std::make_tuple(contentId2, ENTITY_RELATIONSHIP::CONTAINS, DLB_ADM_ENTITY_TYPE_CONTENT_LABEL);
        lowerIt = index.lower_bound(contentLableTuple2);
        upperIt = index.upper_bound(contentLableTuple2);
        n = 0;
        while (lowerIt != upperIt)
        {
            lowerIt++;
            n++;
        }
        EXPECT_EQ(0u, n);

        // Find all entities contained by content 1
        auto contentContainsTuple1 = std::make_tuple(contentId1, ENTITY_RELATIONSHIP::CONTAINS);
        lowerIt = index.lower_bound(contentContainsTuple1);
        upperIt = index.upper_bound(contentContainsTuple1);
        n = 0;
        while (lowerIt != upperIt)
        {
            lowerIt++;
            n++;
        }
        EXPECT_EQ(3u, n);

        // Test inverse relationship queries --

        // Ask for non-existent relationship between content 3 and the first content label
        it = index.find(RelationshipTuple(contentLabelId1a, ENTITY_RELATIONSHIP::CONTAINED_BY, translator.GetEntityType(contentId3), contentId3));
        EXPECT_EQ(indexEnd, it);

        // Find the specific relationship between content 1 and the first content label
        it = index.find(RelationshipTuple(contentLabelId1a, ENTITY_RELATIONSHIP::CONTAINED_BY, translator.GetEntityType(contentId1), contentId1));
        EXPECT_NE(indexEnd, it);

        // Find the specific relationship between content 1 and the first content label with a wildcard query
        auto wildcardTuple = std::make_tuple(contentLabelId1a, ENTITY_RELATIONSHIP::CONTAINED_BY);
        lowerIt = index.lower_bound(wildcardTuple);
        upperIt = index.upper_bound(wildcardTuple);
        n = 0;
        while (lowerIt != upperIt)
        {
            lowerIt++;
            n++;
        }
        EXPECT_EQ(1u, n);
    }
    catch (...)
    {
        FAIL();
    }
}