/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#include "gtest/gtest.h"

#include "sadm/dlb_sadm_model.h"

#include <string.h>

#define CHECK_SUCCESS(S) if ((S) == PMD_FAIL) return PMD_FAIL

static const char *objectNames[] =
{
    "Object01",
    "Object02",
    "Object03",
    "Object04",
    "Object05",
    "Object06",
    "Object07",
    "Object08",
    "Object09",
    "Object0A",
    "Object0B",
    "Object0C",
    "Object0D",
    "Object0E",
    "Object0F",
    "Object10",
};

static const char *objectBlockFormatIDs[] =
{
    "AB_00031001_00000001",
    "AB_00031002_00000001",
    "AB_00031003_00000001",
    "AB_00031004_00000001",
    "AB_00031005_00000001",
    "AB_00031006_00000001",
    "AB_00031007_00000001",
    "AB_00031008_00000001",
    "AB_00031009_00000001",
    "AB_0003100A_00000001",
    "AB_0003100B_00000001",
    "AB_0003100C_00000001",
    "AB_0003100D_00000001",
    "AB_0003100E_00000001",
    "AB_0003100F_00000001",
    "AB_00031010_00000001",
};

static const char *objectChannelFormatIDs[] =
{
    "AC_00031001",
    "AC_00031002",
    "AC_00031003",
    "AC_00031004",
    "AC_00031005",
    "AC_00031006",
    "AC_00031007",
    "AC_00031008",
    "AC_00031009",
    "AC_0003100A",
    "AC_0003100B",
    "AC_0003100C",
    "AC_0003100D",
    "AC_0003100E",
    "AC_0003100F",
    "AC_00031010",
};

static const char *objectPackFormatIDs[] =
{
    "AP_00031001",
    "AP_00031002",
    "AP_00031003",
    "AP_00031004",
    "AP_00031005",
    "AP_00031006",
    "AP_00031007",
    "AP_00031008",
    "AP_00031009",
    "AP_0003100A",
    "AP_0003100B",
    "AP_0003100C",
    "AP_0003100D",
    "AP_0003100E",
    "AP_0003100F",
    "AP_00031010",
};

static const char *trackUIDIDs[] =
{
    "ATU_00000001",
    "ATU_00000002",
    "ATU_00000003",
    "ATU_00000004",
    "ATU_00000005",
    "ATU_00000006",
    "ATU_00000007",
    "ATU_00000008",
    "ATU_00000009",
    "ATU_0000000A",
    "ATU_0000000B",
    "ATU_0000000C",
    "ATU_0000000D",
    "ATU_0000000E",
    "ATU_0000000F",
    "ATU_00000010",
};

static const char *audioObjectIDs[] =
{
    "AO_1001",
    "AO_1002",
    "AO_1003",
    "AO_1004",
    "AO_1005",
    "AO_1006",
    "AO_1007",
    "AO_1008",
    "AO_1009",
    "AO_100A",
    "AO_100B",
    "AO_100C",
    "AO_100D",
    "AO_100E",
    "AO_100F",
    "AO_1010",
};

static const char *audioContentIDs[] =
{
    "ACO_1001",
    "ACO_1002",
    "ACO_1003",
    "ACO_1004",
    "ACO_1005",
    "ACO_1006",
    "ACO_1007",
    "ACO_1008",
    "ACO_1009",
    "ACO_100A",
    "ACO_100B",
    "ACO_100C",
    "ACO_100D",
    "ACO_100E",
    "ACO_100F",
    "ACO_1010",
};

static const char *audioProgrammeIDs[] =
{
    "APR_1001",
    "APR_1002",
    "APR_1003",
    "APR_1004",
    "APR_1005",
    "APR_1006",
    "APR_1007",
    "APR_1008",
    "APR_1009",
    "APR_100A",
    "APR_100B",
    "APR_100C",
    "APR_100D",
    "APR_100E",
    "APR_100F",
    "APR_1010",
};

class DlbPmdSadm01 : public testing::Test
{
protected:

    char *modelMemory1;
    char *modelMemory2;

    void Clear(dlb_sadm_idref_array &idref_array)
    {
        ::memset(idref_array.array, 0, sizeof(dlb_sadm_idref) * idref_array.max);
        idref_array.num = 0;
    }

    void Clear(dlb_sadm_idref_array &idref_array, dlb_sadm_idref *idrefs, unsigned int n)
    {
        idref_array.array = idrefs;
        idref_array.max = n;
        Clear(idref_array);
    }

    dlb_pmd_success Push(dlb_sadm_idref_array &idref_array, dlb_sadm_idref &idref)
    {
        dlb_pmd_success pushed = PMD_FAIL;

        if (idref_array.num < idref_array.max)
        {
            ::memcpy(&idref_array.array[idref_array.num++], &idref, sizeof(idref));
            pushed = PMD_SUCCESS;
        }

        return pushed;
    }

    void Clear(dlb_sadm_pack_format &pf, dlb_sadm_packfmt_type type, dlb_sadm_idref *idrefs, unsigned int n)
    {
        ::memset(&pf, 0, sizeof(pf));
        pf.type = type;
        Clear(pf.chanfmts, idrefs, n);
    }

    void Clear(dlb_sadm_block_format &block_format, bool cartesian)
    {
        ::memset(&block_format, 0, sizeof(block_format));
        if (cartesian)
        {
            block_format.cartesian_coordinates = PMD_TRUE;
        }
    }

    void Clear(dlb_sadm_channel_format &cf, dlb_sadm_idref *idrefs, unsigned int n)
    {
        ::memset(&cf, 0, sizeof(cf));
        Clear(cf.blkfmts, idrefs, n);
    }

    void Clear(dlb_sadm_track_uid &track_uid)
    {
        ::memset(&track_uid, 0, sizeof(track_uid));
    }

    void Clear(dlb_sadm_object &object, dlb_sadm_idref *idrefs, unsigned int n)
    {
        ::memset(&object, 0, sizeof(object));
        Clear(object.track_uids, idrefs, n);
    }

    void Clear(dlb_sadm_content &content, dlb_sadm_content_type type, dlb_sadm_idref *idrefs, unsigned int n)
    {
        ::memset(&content, 0, sizeof(content));
        content.type = type;
        ::strcpy(content.label.language, "en");
        Clear(content.objects, idrefs, n);
    }

    void MakeSadmID(dlb_sadm_id &id, const char *name)
    {
        ::strcpy(reinterpret_cast<char *>(id.data), name);
    }

    void MakeSadmName(dlb_sadm_name &sadm_name, const char *name)
    {
        ::strcpy(reinterpret_cast<char *>(sadm_name.data), name);
    }

    dlb_pmd_success AddObjectContent(dlb_sadm_model *model, unsigned int objectIndex, unsigned int objectChannel, dlb_sadm_idref &content_idref)
    {
        const char *object_name = objectNames[objectIndex];

        dlb_sadm_block_format block_format;
        dlb_sadm_channel_format channel_format;
        dlb_sadm_pack_format pack_format;
        dlb_sadm_track_uid track_uid;
        dlb_sadm_object object;
        dlb_sadm_content content;

        dlb_sadm_idref block_idref;
        dlb_sadm_idref channel_idref;
        dlb_sadm_idref pack_idref;
        dlb_sadm_idref track_idref;
        dlb_sadm_idref object_idref;

        dlb_pmd_success success;

        Clear(block_format, true);
        Clear(channel_format, &block_idref, 1);
        Clear(pack_format, DLB_SADM_PACKFMT_TYPE_OBJECT, &channel_idref, 1);
        Clear(track_uid);
        Clear(object, &track_idref, 1);
        Clear(content, DLB_SADM_CONTENT_DK_DIALOGUE, &object_idref, 1);

        MakeSadmID(block_format.id, objectBlockFormatIDs[objectIndex]);
        success = dlb_sadm_set_block_format(model, &block_format, &block_idref);
        CHECK_SUCCESS(success);

        MakeSadmID(channel_format.id, objectChannelFormatIDs[objectIndex]);
        MakeSadmName(channel_format.name, object_name);
        success = Push(channel_format.blkfmts, block_idref);
        CHECK_SUCCESS(success);
        success = dlb_sadm_set_channel_format(model, &channel_format, &channel_idref);
        CHECK_SUCCESS(success);

        MakeSadmID(pack_format.id, objectPackFormatIDs[objectIndex]);
        MakeSadmName(pack_format.name, object_name);
        success = Push(pack_format.chanfmts, channel_idref);
        CHECK_SUCCESS(success);
        success = dlb_sadm_set_pack_format(model, &pack_format, &pack_idref);
        CHECK_SUCCESS(success);

        MakeSadmID(track_uid.id, trackUIDIDs[objectIndex]);
        track_uid.chanfmt = channel_idref;
        track_uid.packfmt = pack_idref;
        track_uid.channel_idx = objectChannel;
        success = dlb_sadm_set_track_uid(model, &track_uid, &track_idref);
        CHECK_SUCCESS(success);

        MakeSadmID(object.id, audioObjectIDs[objectIndex]);
        MakeSadmName(object.name, object_name);
        object.pack_format = pack_idref;
        success = Push(object.track_uids, track_idref);
        CHECK_SUCCESS(success);
        success = dlb_sadm_set_object(model, &object, &object_idref);
        CHECK_SUCCESS(success);

        MakeSadmID(content.id, audioContentIDs[objectIndex]);
        MakeSadmName(content.label.name, object_name);
        MakeSadmName(content.name, object_name);
        success = Push(content.objects, object_idref);
        CHECK_SUCCESS(success);
        success = dlb_sadm_set_content(model, &content, &content_idref);

        return success;
    }

    virtual void SetUp()
    {
        modelMemory1 = NULL;
        modelMemory2 = NULL;
    }

    virtual void TearDown()
    {
        if (modelMemory2 != NULL)
        {
            delete[] modelMemory2;
            modelMemory2 = NULL;
        }
        if (modelMemory1 != NULL)
        {
            delete[] modelMemory1;
            modelMemory1 = NULL;
        }
    }

};

TEST_F(DlbPmdSadm01, FlowID)
{
    static const size_t UUID_SIZE = 36;
    static const char *good_uuid = "123e4567-e89b-12d3-a456-426655440000";
    static const char *random_string = "this is random";
    static const char *long_random_string = "this is a much longer random string that is longer than a UUID";
    static const char *prefix_uuid = "123e4567-e89b-12d3-a456-426655440000 is the UUID we want";

    char test_uuid[UUID_SIZE + 1];
    char bad_uuid[UUID_SIZE / 2];
    dlb_pmd_success success;
    dlb_sadm_counts limits;
    dlb_sadm_model *m;
    size_t sz;

    // Setup
    dlb_sadm_get_default_counts(&limits);
    sz = dlb_sadm_query_memory(&limits);
    modelMemory1 = new char[sz];
    ASSERT_NE(nullptr, modelMemory1);
    success = dlb_sadm_init(&limits, modelMemory1, &m);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // dlb_sadm_get_flow_id() bad arguments
    success = dlb_sadm_get_flow_id(NULL, NULL, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL), success);
    success = dlb_sadm_get_flow_id(m, NULL, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL),  success);
    success = dlb_sadm_get_flow_id(m, bad_uuid, sizeof(bad_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL),  success);

    // dlb_sadm_get_flow_id() returns blank UUID
    strcpy(test_uuid, good_uuid);
    success = dlb_sadm_get_flow_id(m, test_uuid, sizeof(test_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    EXPECT_EQ(0, ::strcmp("", test_uuid));

    // dlb_sadm_set_flow_id() bad arguments
    success = dlb_sadm_set_flow_id(NULL, NULL, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL),  success);
    success = dlb_sadm_set_flow_id(m, NULL, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = dlb_sadm_set_flow_id(m, random_string, ::strlen(random_string));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL),  success);
    success = dlb_sadm_set_flow_id(m, long_random_string, ::strlen(long_random_string));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL), success);

    // Set a good UUID
    success = dlb_sadm_set_flow_id(m, good_uuid, ::strlen(good_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    test_uuid[0] = '\0';
    success = dlb_sadm_get_flow_id(m, test_uuid, sizeof(test_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    EXPECT_EQ(0, ::strcmp(good_uuid, test_uuid));

    // Does NULL input string clear the UUID in the model?
    success = dlb_sadm_set_flow_id(m, NULL, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = dlb_sadm_get_flow_id(m, test_uuid, sizeof(test_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    EXPECT_EQ(0, ::strcmp("", test_uuid));

    // Does empty input string clear the UUID in the model?
    success = dlb_sadm_set_flow_id(m, good_uuid, ::strlen(good_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = dlb_sadm_set_flow_id(m, "", 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    strcpy(test_uuid, good_uuid);
    success = dlb_sadm_get_flow_id(m, test_uuid, sizeof(test_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    EXPECT_EQ(0, ::strcmp("", test_uuid));

    // Set the flow ID from a string starting with a good UUID
    success = dlb_sadm_set_flow_id(m, prefix_uuid, ::strlen(prefix_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    test_uuid[0] = '\0';
    success = dlb_sadm_get_flow_id(m, test_uuid, sizeof(test_uuid));
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    EXPECT_EQ(0, ::strncmp(test_uuid, prefix_uuid, UUID_SIZE));
}

TEST_F(DlbPmdSadm01, CopyBadArgs)
{
    static const size_t NUM_OBJECTS = 8;

    dlb_pmd_success success;
    dlb_sadm_counts limits;
    dlb_sadm_model *m1;
    dlb_sadm_model *m2;
    size_t sz;

    // Setup
    dlb_sadm_get_default_counts(&limits);
    ASSERT_EQ(NUM_OBJECTS, limits.num_objects);
    sz = dlb_sadm_query_memory(&limits);
    modelMemory1 = new char[sz];
    ASSERT_NE(nullptr, modelMemory1);
    success = dlb_sadm_init(&limits, modelMemory1, &m1);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    limits.num_programmes -= 2;
    limits.num_contents -= 2;
    limits.num_objects -= 2;
    sz = dlb_sadm_query_memory(&limits);
    modelMemory2 = new char[sz];
    ASSERT_NE(nullptr, modelMemory2);
    success = dlb_sadm_init(&limits, modelMemory2, &m2);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Add some objects to the source model
    dlb_sadm_idref content_idref;
    dlb_sadm_idref content_idrefs[NUM_OBJECTS];
    dlb_sadm_programme_label label;
    dlb_sadm_programme programme;
    unsigned int i, ch;

    ::memset(&programme, 0, sizeof(programme));
    Clear(programme.contents, content_idrefs, NUM_OBJECTS);
    for (i = 0, ch = 1; i < NUM_OBJECTS; ++i, ++ch)
    {
        success = AddObjectContent(m1, i, ch, content_idref);
        ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
        success = Push(programme.contents, content_idref);
        ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    }

    // Add programme

    ::strcpy(programme.language, "en");
    MakeSadmID(programme.id, audioProgrammeIDs[0]);
    ::strcpy(label.language, "en");
    MakeSadmName(label.name, "Programme 1");
    programme.num_labels = 1;
    programme.labels = &label;
    success = dlb_sadm_set_programme(m1, &programme, NULL);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Test NULL pointer arguments
    success = dlb_sadm_copy(nullptr, nullptr);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL), success);
    success = dlb_sadm_copy(m2, nullptr);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL), success);

    // Try to copy too many objects to the other model
    success = dlb_sadm_copy(m2, m1);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_FAIL), success);
}

TEST_F(DlbPmdSadm01, CopyBasic)
{
    static const size_t NUM_OBJECTS = 8;

    dlb_pmd_success success;
    dlb_sadm_counts limits;
    dlb_sadm_model *m1;
    dlb_sadm_model *m2;
    size_t sz;

    // Setup
    dlb_sadm_get_default_counts(&limits);
    ASSERT_EQ(NUM_OBJECTS, limits.num_objects);
    sz = dlb_sadm_query_memory(&limits);
    modelMemory1 = new char[sz];
    ASSERT_NE(nullptr, modelMemory1);
    success = dlb_sadm_init(&limits, modelMemory1, &m1);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    modelMemory2 = new char[sz];
    ASSERT_NE(nullptr, modelMemory2);
    success = dlb_sadm_init(&limits, modelMemory2, &m2);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Add some objects to the source model
    dlb_sadm_idref content_idref;
    dlb_sadm_idref content_idrefs[NUM_OBJECTS];
    dlb_sadm_programme_label label;
    dlb_sadm_programme programme;
    unsigned int i, ch;

    ::memset(&programme, 0, sizeof(programme));
    Clear(programme.contents, content_idrefs, NUM_OBJECTS);
    for (i = 0, ch = 1; i < NUM_OBJECTS; ++i, ++ch)
    {
        success = AddObjectContent(m1, i, ch, content_idref);
        ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
        success = Push(programme.contents, content_idref);
        ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    }

    // Add programme

    ::strcpy(programme.language, "en");
    MakeSadmID(programme.id, audioProgrammeIDs[0]);
    ::strcpy(label.language, "en");
    MakeSadmName(label.name, "Programme 1");
    programme.num_labels = 1;
    programme.labels = &label;
    success = dlb_sadm_set_programme(m1, &programme, NULL);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Test
    success = dlb_sadm_copy(m2, m1);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    dlb_pmd_bool eq = dlb_sadm_eq(m1, m2);
    EXPECT_TRUE(eq);
}

TEST_F(DlbPmdSadm01, EqBasic)
{
    static const size_t NUM_OBJECTS = 8;

    dlb_pmd_success success;
    dlb_sadm_counts limits;
    dlb_sadm_model *m1;
    dlb_sadm_model *m2;
    size_t sz;

    // Setup
    dlb_sadm_get_default_counts(&limits);
    ASSERT_EQ(NUM_OBJECTS, limits.num_objects);
    sz = dlb_sadm_query_memory(&limits);
    modelMemory1 = new char[sz];
    ASSERT_NE(nullptr, modelMemory1);
    success = dlb_sadm_init(&limits, modelMemory1, &m1);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    modelMemory2 = new char[sz];
    ASSERT_NE(nullptr, modelMemory2);
    success = dlb_sadm_init(&limits, modelMemory2, &m2);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Add some objects to the source model
    dlb_sadm_idref content_idref;
    dlb_sadm_idref content_idrefs[NUM_OBJECTS];
    dlb_sadm_programme_label label;
    dlb_sadm_programme programme;
    unsigned int i, ch;

    ::memset(&programme, 0, sizeof(programme));
    Clear(programme.contents, content_idrefs, NUM_OBJECTS);
    for (i = 0, ch = 1; i < NUM_OBJECTS; ++i, ++ch)
    {
        success = AddObjectContent(m1, i, ch, content_idref);
        ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
        success = Push(programme.contents, content_idref);
        ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    }

    // Add programme

    ::strcpy(programme.language, "en");
    MakeSadmID(programme.id, audioProgrammeIDs[0]);
    ::strcpy(label.language, "en");
    MakeSadmName(label.name, "Programme 1");
    programme.num_labels = 1;
    programme.labels = &label;
    success = dlb_sadm_set_programme(m1, &programme, NULL);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Test NULL pointer arguments
    dlb_pmd_bool eq;

    eq = dlb_sadm_eq(nullptr, nullptr);
    EXPECT_TRUE(eq);
    eq = dlb_sadm_eq(m1, nullptr);
    EXPECT_FALSE(eq);
    eq = dlb_sadm_eq(nullptr, m2);
    EXPECT_FALSE(eq);

    // Test same pointer arguments
    eq = dlb_sadm_eq(m1, m1);
    EXPECT_TRUE(eq);

    // Test against empty model
    eq = dlb_sadm_eq(m1, m2);
    EXPECT_FALSE(eq);

    // Copy the model and test again
    success = dlb_sadm_copy(m2, m1);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    eq = dlb_sadm_eq(m1, m2);
    EXPECT_TRUE(eq);

    // Set the gain on an object to a different value, then test for equality
    dlb_sadm_idref object_ref;
    dlb_sadm_idref track_ref;
    dlb_sadm_idref_array object_ref_array;
    dlb_sadm_idref_array track_ref_array;
    dlb_sadm_object_iterator it;
    dlb_sadm_object object;

    Clear(object_ref_array, &object_ref, 1);
    Clear(track_ref_array,  &track_ref,  1);
    success = dlb_sadm_object_iterator_init(&it, m2);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = dlb_sadm_object_iterator_next(&it, &object, &object_ref_array, &track_ref_array);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = dlb_sadm_object_iterator_next(&it, &object, &object_ref_array, &track_ref_array);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = dlb_sadm_object_iterator_next(&it, &object, &object_ref_array, &track_ref_array);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    object.gain = -1.0;
    success = dlb_sadm_set_object(m2, &object, nullptr);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    eq = dlb_sadm_eq(m1, m2);
    EXPECT_FALSE(eq);
}
