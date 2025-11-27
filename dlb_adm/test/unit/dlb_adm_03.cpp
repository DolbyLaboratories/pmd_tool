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

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm_api_pvt.h"

#include "AttributeDescriptor.h"
#include "RelationshipDescriptor.h"
#include "EntityDescriptor.h"
#include "EntityRecord.h"
#include "RelationshipRecord.h"
#include "dlb_adm_xml_container.h"
#include "AdmIdTranslator.h"
#include "AdmId.h"
#include "TestUtilities.h"

#include <stdio.h>
#include <fstream>
#include <string>

#include "dlb_adm_data.h"

static const char dolbyReferenceFileName[] = "Dolby_reference.xml";
static const char dolbyReferenceOutFileName[] = "Dolby_reference.out.xml";
static const char dolbyReferenceOutOutFileName[] = "Dolby_reference.out.out.xml";

static const char ebuPart1FileName[] = "EBU_part_1.xml";
static const char ebuPart1OutFileName[] = "EBU_part_1.out.xml";
static const char ebuPart1OutOutFileName[] = "EBU_part_1.out.out.xml";

class DlbAdm03 : public testing::Test
{
protected:

    dlb_adm_container_counts     containerCounts;
    dlb_adm_xml_container       *theContainer;

    void SetUpTestInput(const char *path, const char *content)
    {
        std::ifstream ifs(path);

        if (!ifs.good())
        {
            std::ofstream ofs(path);

            ofs << content;
        }
    }

    virtual void SetUp()
    {
        ::memset(&containerCounts, 0, sizeof(containerCounts));
        theContainer = nullptr;

        SetUpTestInput(dolbyReferenceFileName, dolbyReferenceXML);
        SetUpTestInput(ebuPart1FileName, ebuPart1XML);
    }

    virtual void TearDown()
    {
        if (theContainer != nullptr)
        {
            if (::dlb_adm_container_close(&theContainer))
            {
                theContainer = nullptr;
            }
        }
    }

    void PrintEntityId(const dlb_adm_entity_id &id, bool isReference)
    {
        using namespace DlbAdm;

        EntityRecord e;
        int status = theContainer->GetContainer().GetEntity(e, id);

        if (status)
        {
            printf("<error>");
        } 
        else
        {
            EntityDescriptor ed;

            status = GetEntityDescriptor(ed, static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(id)), isReference);
            if (status)
            {
                printf("<error>");
            }
            else if (ed.hasADMIdOrRef && DLB_ADM_ID_GET_ENTITY_TYPE(id) != DLB_ADM_ENTITY_TYPE_TOPLEVEL)
            {
                char b[99];

                status = dlb_adm_write_entity_id(b, sizeof(b), id);
                if (status)
                {
                    printf("<error>");
                }
                else
                {
                    printf("%s", b);
                }
            }
            else
            {
                printf("%s", ed.name.c_str());
            }
        }
    }

    void PrintRelationship(DlbAdm::ENTITY_RELATIONSHIP r)
    {
        switch (r)
        {
        case DlbAdm::ENTITY_RELATIONSHIP::NONE:
            printf("none");
            break;
        case DlbAdm::ENTITY_RELATIONSHIP::CONTAINS:
            printf("contains");
            break;
        case DlbAdm::ENTITY_RELATIONSHIP::CONTAINED_BY:
            printf("is contained by");
            break;
        case DlbAdm::ENTITY_RELATIONSHIP::REFERENCES:
            printf("references");
            break;
        default:
            printf("<unknown>");
            break;
        }
    }

    void PrintRelationship(DlbAdm::RelationshipRecord rr)
    {
        using namespace DlbAdm;

        PrintEntityId(rr.fromId, rr.relationship == ENTITY_RELATIONSHIP::REFERENCES);
        printf(" ");
        PrintRelationship(rr.relationship);
        printf(" ");
        PrintEntityId(rr.toId, false);
        printf("\n");
    }

    void PrintRelationships()
    {
        if (theContainer != nullptr)
        {
            theContainer->GetContainer().ForEachRelationship
            (
                [&](const DlbAdm::RelationshipRecord &rr)
                {
                    PrintRelationship(rr);
                    return 0;
                }
            );
        }
    }

};

TEST(dlb_adm_test, GetAttributeDescriptorBasic)
{
    using namespace DlbAdm;

    AttributeDescriptor d;
    int status;

    status = GetAttributeDescriptor(d, static_cast<DLB_ADM_TAG>(DLB_ADM_TAG_LAST + 1));
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

    d = nullAttributeDescriptor;
    status = GetAttributeDescriptor(d, DLB_ADM_TAG_PROGRAMME_ID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PROGRAMME, d.entityType);
    EXPECT_EQ(DLB_ADM_TAG_PROGRAMME_ID, d.attributeTag);
    EXPECT_EQ(DLB_ADM_VALUE_TYPE_STRING, d.attributeValueType);
    EXPECT_EQ(std::string("audioProgrammeID"), d.attributeName);

    d = nullAttributeDescriptor;
    status = GetAttributeDescriptor(d, DLB_ADM_ENTITY_TYPE_PROGRAMME, "audioProgrammeID");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PROGRAMME, d.entityType);
    EXPECT_EQ(DLB_ADM_TAG_PROGRAMME_ID, d.attributeTag);
    EXPECT_EQ(DLB_ADM_VALUE_TYPE_STRING, d.attributeValueType);
    EXPECT_EQ(std::string("audioProgrammeID"), d.attributeName);
}

TEST(dlb_adm_test, GetAttributeTag)
{
    DLB_ADM_TAG tag;
    int status;

    tag = DLB_ADM_TAG_UNKNOWN;
    status = ::dlb_adm_get_attribute_tag(NULL, DLB_ADM_ENTITY_TYPE_ILLEGAL, NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_get_attribute_tag(&tag, DLB_ADM_ENTITY_TYPE_ILLEGAL, NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_get_attribute_tag(&tag, DLB_ADM_ENTITY_TYPE_ILLEGAL, "Bratwurst");
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_get_attribute_tag(&tag, DLB_ADM_ENTITY_TYPE_PROGRAMME, "Bratwurst");
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

    tag = DLB_ADM_TAG_UNKNOWN;
    status = ::dlb_adm_get_attribute_tag(&tag, DLB_ADM_ENTITY_TYPE_PROGRAMME, "audioProgrammeID");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TAG_PROGRAMME_ID, tag);

    tag = DLB_ADM_TAG_UNKNOWN;
    status = ::dlb_adm_get_attribute_tag(&tag, DLB_ADM_ENTITY_TYPE_CONTENT, "audioContentID");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TAG_CONTENT_ID, tag);

    tag = DLB_ADM_TAG_UNKNOWN;
    status = ::dlb_adm_get_attribute_tag(&tag, DLB_ADM_ENTITY_TYPE_OBJECT, "audioObjectName");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TAG_OBJECT_NAME, tag);
}

TEST(dlb_adm_test, GetRelationshipDescriptorBasic)
{
    using namespace DlbAdm;

    RelationshipDescriptor d;
    int status;

    status = GetRelationshipDescriptor(d, DLB_ADM_ENTITY_TYPE_ILLEGAL, DLB_ADM_ENTITY_TYPE_ILLEGAL);
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

    d = nullRelationshipDescriptor;
    status = GetRelationshipDescriptor(d, DLB_ADM_ENTITY_TYPE_PROGRAMME, DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PROGRAMME, d.fromType);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, d.toType);
    EXPECT_EQ(ENTITY_RELATIONSHIP::CONTAINS, d.relationship);
    EXPECT_EQ(0, d.arity.minArity);
    EXPECT_EQ(RelationshipArity::ANY, d.arity.maxArity);
}

TEST(dlb_adm_test, GetEntityDescriptorBasic)
{
    using namespace DlbAdm;

    EntityDescriptor d;
    std::string s1("audioProgramme");
    std::string s2("frameFormat");
    int status;

    status = GetEntityDescriptor(d, "");
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

    status = GetEntityDescriptor(d, s1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(s1, d.name);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PROGRAMME, d.entityType);

    status = GetEntityDescriptor(d, DLB_ADM_ENTITY_TYPE_FRAME_FORMAT);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(s2, d.name);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, d.entityType);
}

TEST(dlb_adm_test, OpenCloseContainer)
{
    dlb_adm_container_counts counts;
    dlb_adm_xml_container *container;
    int status;

    ::memset(&counts, 0, sizeof(counts));

    status = ::dlb_adm_container_open(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_open(&container, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_container_open(&container, &counts);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(container, nullptr);

    status = ::dlb_adm_container_close(nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_close(&container);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(container, nullptr);
}

TEST_F(DlbAdm03, AddReferenceBasic)
{
    static const char *programmeIdStr = "APR_1001";
    dlb_adm_entity_id programmeId;
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&programmeId, programmeIdStr, ::strlen(programmeIdStr) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_add_reference(nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_add_reference(theContainer, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_container_add_reference(theContainer, programmeId);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm03, GetSetValue)
{
    static const char *stringValue = "This is a string value";
    static const char *programmeIdStr = "APR_1001";
    dlb_adm_entity_id programmeId;
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&programmeId, programmeIdStr, ::strlen(programmeIdStr) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, programmeId);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_set_string_value(nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_TAG_UNKNOWN, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_set_string_value(theContainer, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_TAG_UNKNOWN, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_UNKNOWN, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_UNKNOWN, stringValue);
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);
    status = ::dlb_adm_container_set_bool_value(theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_VALUE_TYPE_MISMATCH, status);

    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME, stringValue);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_bool bv = DLB_ADM_FALSE;
    char sv1[DLB_ADM_STRING_VALUE_BUFFER_MIN_SIZE - 1];
    char sv2[DLB_ADM_STRING_VALUE_BUFFER_MIN_SIZE * 4];

    status = ::dlb_adm_container_get_bool_value(nullptr, nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_TAG_UNKNOWN);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_get_bool_value(&bv, nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_TAG_UNKNOWN);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_get_bool_value(&bv, theContainer, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_TAG_UNKNOWN);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_container_get_bool_value(&bv, theContainer, programmeId, DLB_ADM_TAG_UNKNOWN);
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);
    status = ::dlb_adm_container_get_bool_value(&bv, theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME);
    EXPECT_EQ(DLB_ADM_STATUS_VALUE_TYPE_MISMATCH, status);
    status = ::dlb_adm_container_get_string_value(sv1, sizeof(sv1), theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_container_get_string_value(sv2, sizeof(sv2), theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, ::strcmp(sv2, stringValue));

    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_LANGUAGE, "en");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm03, SetMutable)
{
    static const char *stringValue1 = "This is a string value";
    static const char *stringValue2 = "This is another string value";
    static const char *programmeIdStr = "APR_1001";
    char sv[DLB_ADM_STRING_VALUE_BUFFER_MIN_SIZE * 4];
    dlb_adm_entity_id programmeId;
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&programmeId, programmeIdStr, ::strlen(programmeIdStr) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, programmeId);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME, stringValue1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_set_mutable(theContainer, programmeId, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME, stringValue2);
    ASSERT_EQ(DLB_ADM_STATUS_ERROR, status);
    status = ::dlb_adm_container_set_mutable(theContainer, programmeId, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_set_string_value(theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME, stringValue2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_get_string_value(sv, sizeof(sv), theContainer, programmeId, DLB_ADM_TAG_PROGRAMME_NAME);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, ::strcmp(sv, stringValue2));
}

TEST_F(DlbAdm03, AddRelationshipBasic)
{
    using namespace DlbAdm;

    AdmIdTranslator translator;

    static const char   *programmeIdStr = "APR_1001";
    static const char   *contentIdStr1  = "ACO_1001";
    static const char   *contentIdStr2  = "ACO_1002";
    static const char   *objectIdStr1   = "AO_1001";
    static const char   *objectIdStr2   = "AO_1002";

    dlb_adm_entity_id    programmeId;
    dlb_adm_entity_id    programmeLabelId1;
    dlb_adm_entity_id    contentId1;
    dlb_adm_entity_id    contentLabelId1a;
    dlb_adm_entity_id    contentId2;
    dlb_adm_entity_id    objectId1;
    dlb_adm_entity_id    objectId2;

    int                  status;

    // Make IDs

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&programmeId, programmeIdStr, ::strlen(programmeIdStr) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    programmeLabelId1 = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, 1);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, programmeLabelId1);

    status = ::dlb_adm_read_entity_id(&contentId1, contentIdStr1, ::strlen(contentIdStr1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&contentId2, contentIdStr2, ::strlen(contentIdStr2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    contentLabelId1a = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, 1);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, contentId1);

    status = ::dlb_adm_read_entity_id(&objectId1, objectIdStr1, ::strlen(objectIdStr1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&objectId2, objectIdStr2, ::strlen(objectIdStr2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Add entities

    status = ::dlb_adm_container_add_reference(theContainer, programmeId);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, programmeLabelId1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, contentId1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, contentId2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, contentLabelId1a);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, objectId1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_reference(theContainer, objectId2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Test adding relationships

    status = ::dlb_adm_container_add_relationship(nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_add_relationship(theContainer, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_container_add_relationship(theContainer, programmeId, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_container_add_relationship(theContainer, programmeId, objectId1);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_RELATIONSHIP, status);

    status = ::dlb_adm_container_add_relationship(theContainer, programmeId, programmeLabelId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_relationship(theContainer, programmeId, programmeLabelId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);   // Repeat should be OK
    status = ::dlb_adm_container_add_relationship(theContainer, programmeId, contentId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_relationship(theContainer, programmeId, contentId2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_relationship(theContainer, contentId1, contentLabelId1a);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_relationship(theContainer, contentId1, objectId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_add_relationship(theContainer, contentId2, objectId2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Allow multiple references

    status = ::dlb_adm_container_add_relationship(theContainer, contentId1, objectId2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Disallow multiple containment

    status = ::dlb_adm_container_add_relationship(theContainer, contentId2, contentLabelId1a);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_RELATIONSHIP, status);

    // But allow repeating the same containment

    status = ::dlb_adm_container_add_relationship(theContainer, contentId1, contentLabelId1a);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm03, ReadXmlFileBasic)
{
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_file(nullptr, nullptr, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_read_xml_file(theContainer, nullptr, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_container_read_xml_file(theContainer, "non_existing_file.xml", DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

    status = ::dlb_adm_container_read_xml_file(theContainer, dolbyReferenceFileName, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(theContainer, dolbyReferenceOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyReferenceFileName, dolbyReferenceOutFileName));

    status = ::dlb_adm_container_close(&theContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_file(theContainer, dolbyReferenceOutFileName, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(theContainer, dolbyReferenceOutOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyReferenceOutFileName, dolbyReferenceOutOutFileName));
}

TEST_F(DlbAdm03, ReadXmlFileEBU)
{
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_file(theContainer, ebuPart1FileName, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(theContainer, ebuPart1OutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(ebuPart1FileName, ebuPart1OutFileName));
    
    status = ::dlb_adm_container_close(&theContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_file(theContainer, ebuPart1OutFileName, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(theContainer, ebuPart1OutOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(ebuPart1OutFileName, ebuPart1OutOutFileName));
}

#ifdef EXTERNAL_ADM_COMMON_DEFINITIONS
TEST_F(DlbAdm03, ReadXmlFileCommonDefs)
{
    const char *path = dlb_adm_get_common_defs_path();
    FILE *f = ::fopen(path, "r");

    if (f != nullptr)
    {
        int status;

        ::fclose(f);
        status = ::dlb_adm_container_open(&theContainer, &containerCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        status = ::dlb_adm_container_read_xml_file(theContainer, path, DLB_ADM_FALSE);
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);

        //PrintRelationships();

#ifdef _WIN32
        status = ::dlb_adm_container_write_xml_file(theContainer, "D:\\data\\PMD\\common_defs.out.xml");
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
#endif
    }
}
#endif

#if 0
TEST_F(DlbAdm03, ReadXmlFileEBUWithCommonDefs)
{
    const char *path = dlb_adm_get_common_defs_path();
    FILE *f = ::fopen(path, "r");

    if (f != nullptr)
    {
        int status;

        ::fclose(f);
        status = ::dlb_adm_container_open(&theContainer, &containerCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        status = ::dlb_adm_container_read_xml_file(theContainer, "D:\\data\\PMD\\ESC19_Netherlands_15chPCMaudio_adm_sadm_dlb_part1_v1.xml", DLB_ADM_TRUE);
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
        status = ::dlb_adm_container_write_xml_file(theContainer, "D:\\data\\PMD\\ESC19_Netherlands_15chPCMaudio_adm_sadm_dlb_part1_v1.common.out.xml");
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    }
}
#endif

TEST_F(DlbAdm03, ReadXmlBufferBasic)
{
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer(nullptr, nullptr, 0, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_container_read_xml_buffer(theContainer, nullptr, 0, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_container_read_xml_buffer(theContainer, dolbyReferenceXML, 0, DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    status = ::dlb_adm_container_read_xml_buffer(theContainer, dolbyReferenceXML, ::strlen(dolbyReferenceXML), DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(theContainer, dolbyReferenceOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyReferenceFileName, dolbyReferenceOutFileName));
}

TEST_F(DlbAdm03, ReadXmlBufferSeveralTimes)
{
    int status;

    status = ::dlb_adm_container_open(&theContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer(theContainer, dolbyReferenceXML, ::strlen(dolbyReferenceXML), DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_container_clear_all(theContainer);

    status = ::dlb_adm_container_read_xml_buffer(theContainer, dolbyReferenceXML, ::strlen(dolbyReferenceXML), DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_container_clear_all(theContainer);

    status = ::dlb_adm_container_read_xml_buffer(theContainer, dolbyReferenceXML, ::strlen(dolbyReferenceXML), DLB_ADM_FALSE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(theContainer, dolbyReferenceOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyReferenceFileName, dolbyReferenceOutFileName));
}

