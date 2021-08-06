/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#include <string.h>
#include <sstream>

#include "AttributeValue.h"
#include "AdmIdTranslator.h"
#include "AdmId.h"

using namespace DlbAdm;

TEST(dlb_adm_test, ConfigureTheLibrary)
{
    dlb_adm_library_config config;
#ifdef _WIN32
    std::string path = "C:\\temp\\";
#else
    std::string path = "/opt/dlb_adm/";
#endif
    const char *currentPath = ::dlb_adm_get_common_defs_path();
    char defaultPath[99];
    int status;

    ::strcpy(defaultPath, currentPath);
    path += currentPath;
    ::memset(&config, 0, sizeof(config));
    config.path_to_common_defs = path.c_str();

    status = ::dlb_adm_configure(&config);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    currentPath = ::dlb_adm_get_common_defs_path();
    EXPECT_EQ(0, ::strcmp(currentPath, path.c_str()));
    EXPECT_NE(0, ::strcmp(defaultPath, currentPath));

    config.path_to_common_defs = defaultPath;
    status = ::dlb_adm_configure(&config);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST(dlb_adm_test, Translate_Good_FF_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "FF_00000000001";
    const char *s2 = "FF_00000000001_01";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, entityType);
    EXPECT_EQ(0x1, id & MASK_32);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);

    id = translator.Translate(s2);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, entityType);
    EXPECT_EQ(0x1, id & MASK_32);
    EXPECT_EQ(0x1, (id >> FRAME_PART_SHIFT) & MASK_08);
    s = translator.Translate(id);
    compare = strcmp(s2, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_TP_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "TP_1003";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, entityType);
    EXPECT_EQ(0x1003, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_APR_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "APR_1001";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PROGRAMME, entityType);
    EXPECT_EQ(0x1001, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_ACO_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "ACO_1002";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_CONTENT, entityType);
    EXPECT_EQ(0x1002, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AO_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AO_1003";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_OBJECT, entityType);
    EXPECT_EQ(0x1003, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_ATU_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "ATU_00011002";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_TRACK_UID, entityType);
    EXPECT_EQ(0x00011002, id & MASK_32);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AP_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AP_00010003";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    DLB_ADM_AUDIO_TYPE audioType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_PACK_FORMAT, entityType);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, audioType);
    EXPECT_EQ(0x3, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AS_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AS_00010003";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    DLB_ADM_AUDIO_TYPE audioType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_STREAM_FORMAT, entityType);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, audioType);
    EXPECT_EQ(0x3, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AC_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AC_00010003";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    DLB_ADM_AUDIO_TYPE audioType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, entityType);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, audioType);
    EXPECT_EQ(0x3, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AT_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AT_00010003_01";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    DLB_ADM_AUDIO_TYPE audioType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_TRACK_FORMAT, entityType);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, audioType);
    EXPECT_EQ(0x3, (id >> X_W_SHIFT) & MASK_16);
    EXPECT_EQ(0x1, id & MASK_08);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AB_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AB_00010003_0012EF99";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    DLB_ADM_AUDIO_TYPE audioType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, entityType);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, audioType);
    EXPECT_EQ(0x3, (id >> X_W_SHIFT) & MASK_16);
    EXPECT_EQ(0x0012EF99, id & MASK_32);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_Good_AVS_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AVS_1001_0001";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, entityType);
    EXPECT_EQ(0x1001, (id >> X_W_SHIFT) & MASK_16);
    EXPECT_EQ(0x1, id & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Write_Good_Time_Value)
{
    using namespace DlbAdm;

    std::string s;
    std::ostringstream ss1;
    std::ostringstream ss2;
    std::ostringstream ss3;
    AttributeValue v;
    dlb_adm_time t;

    t.hours = 2;
    t.minutes = 34;
    t.seconds = 56;
    t.fraction_numerator = 25000;
    t.fraction_denominator = 0;

    v = t;
    ss1 << v;
    s = ss1.str();
    EXPECT_EQ(std::string("02:34:56.25000"), s);

    t.fraction_numerator = 12000;
    t.fraction_denominator = 48000;
    v = t;
    ss2 << v;
    s = ss2.str();
    EXPECT_EQ(std::string("02:34:56.12000S48000"), s);

    t.fraction_numerator = 100;
    t.fraction_denominator = 48000;
    v = t;
    ss3 << v;
    s = ss3.str();
    EXPECT_EQ(std::string("02:34:56.00100S48000"), s);
}

TEST(dlb_adm_test, ConstructGenericID)
{
    dlb_adm_entity_id id;
    int status;
    size_t i;

    status = ::dlb_adm_construct_generic_entity_id(nullptr, DLB_ADM_ENTITY_TYPE_VOID, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_construct_generic_entity_id(&id, DLB_ADM_ENTITY_TYPE_VOID, 0);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // n == 0
    status = ::dlb_adm_construct_generic_entity_id(&id, DLB_ADM_ENTITY_TYPE_VOID, 1);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);

    for (i = DLB_ADM_ENTITY_TYPE_FIRST; i < DLB_ADM_ENTITY_TYPE_FIRST_WITH_ID; ++i)
    {
        DLB_ADM_ENTITY_TYPE type = static_cast<DLB_ADM_ENTITY_TYPE>(i);

        status = ::dlb_adm_construct_generic_entity_id(&id, type, 1);
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    }

    for (; i <= DLB_ADM_ENTITY_TYPE_LAST_WITH_ID; ++i)
    {
        DLB_ADM_ENTITY_TYPE type = static_cast<DLB_ADM_ENTITY_TYPE>(i);

        if (type == DLB_ADM_ENTITY_TYPE_TRACK_UID)
        {
            continue;
        }
        status = ::dlb_adm_construct_generic_entity_id(&id, type, 1);
        EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    }

    status = ::dlb_adm_construct_generic_entity_id(&id, DLB_ADM_ENTITY_TYPE_TRACK_UID, 1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    for (; i < DLB_ADM_ENTITY_TYPE_LAST; ++i)
    {
        DLB_ADM_ENTITY_TYPE type = static_cast<DLB_ADM_ENTITY_TYPE>(i);

        status = ::dlb_adm_construct_generic_entity_id(&id, type, 1);
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    }
}
