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

#include <string.h>
#include <sstream>
#include <cstdio>
#include <cmath>

#include "AttributeValue.h"
#include "AdmIdTranslator.h"
#include "AdmId.h"

using namespace DlbAdm;

#ifdef EXTERNAL_ADM_COMMON_DEFINITIONS
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
#endif

TEST(dlb_adm_test, Translate_Good_FF_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "FF_00000001";
    const char *s2 = "FF_00000001_01";
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

TEST(dlb_adm_test, Translate_11_FF_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "FF_00000000001";
    const char *s2 = "FF_00000000001_01";
    const char *expected1 = "FF_00000001";
    const char *expected2 = "FF_00000001_01";    
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
    compare = strcmp(expected1, s.data());
    EXPECT_EQ(0, compare);

    id = translator.Translate(s2);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, entityType);
    EXPECT_EQ(0x1, id & MASK_32);
    EXPECT_EQ(0x1, (id >> FRAME_PART_SHIFT) & MASK_08);
    s = translator.Translate(id);
    compare = strcmp(expected2, s.data());
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

TEST(dlb_adm_test, Translate_Good_AFC_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AFC_1001";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);
    entityType = translator.GetEntityType(id);
    EXPECT_EQ(DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET, entityType);
    EXPECT_EQ(0x1001, (id >> X_W_SHIFT) & MASK_16);
    s = translator.Translate(id);
    compare = strcmp(s1, s.data());
    EXPECT_EQ(0, compare);
}

TEST(dlb_adm_test, Translate_All_Good_AFC_Id)
{
    char id_string[9] = "AFC_1001";
    
    AdmIdTranslator translator;
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    int compare;
    
    for(uint32_t hex_digits = 0x1001; hex_digits <= 0xFFFF; hex_digits++)
    {
        sprintf(id_string+4, "%04X", hex_digits);
    
        id = translator.Translate(id_string);
        EXPECT_NE(0, id);
        entityType = translator.GetEntityType(id);
        EXPECT_EQ(DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET, entityType);
        EXPECT_EQ(hex_digits, (id >> X_W_SHIFT) & MASK_16);
        s = translator.Translate(id);
        compare = strcmp(id_string, s.data());
        EXPECT_EQ(0, compare);
    }
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

TEST(dlb_adm_test, Construct_Good_AVS_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AVS_1001_0001";
    dlb_adm_entity_id translator_id;
    dlb_adm_entity_id construct_id;

    translator_id = translator.Translate(s1);
    EXPECT_NE(0, translator_id);

    construct_id = translator.ConstructUntypedId(DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, 0x1001, 1);
    EXPECT_EQ(construct_id, translator_id);
}

TEST(dlb_adm_test, Deconstruct_Good_AVS_Id)
{
    AdmIdTranslator translator;
    const char *s1 = "AVS_1001_0001";
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    uint32_t xw;
    uint32_t z;

    id = translator.Translate(s1);
    EXPECT_NE(0, id);

    translator.DeconstructUntypedId(id, &entityType, &xw, &z);
    EXPECT_EQ(entityType, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET);
    EXPECT_EQ(xw, 0x1001);
    EXPECT_EQ(z, 0x0001);
}

TEST(dlb_adm_test, Deconstruct_Initial_Combinations_Good_AVS_Id)
{
    char id_string[14] = "AVS_1001_0001";
    
    AdmIdTranslator translator;
    dlb_adm_entity_id id;
    DLB_ADM_ENTITY_TYPE entityType;
    std::string s;
    
    uint32_t dec_xw;
    uint32_t dec_z;

    for(uint32_t xw = 0x1001; xw <= 0x100F; xw++)
    {    
        for(uint32_t z = 0x0001; z <= 0x000F; z++)
        {
            sprintf(id_string+4, "%04X_%04X",xw, z);

            id = translator.Translate(id_string);
            EXPECT_NE(0, id);

            translator.DeconstructUntypedId(id, &entityType, &dec_xw, &dec_z);
            EXPECT_EQ(entityType, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET);
            EXPECT_EQ(xw, dec_xw);
            EXPECT_EQ(z, dec_z);           
        }
    }
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

TEST(dlb_adm_test, SubcomponentIdReferencesComponent_audioObject)
{
    AdmIdTranslator translator;
    const char *obj1String = "AO_1001";
    const char *obj2String = "AO_1002";
    const char *avs11String = "AVS_1001_0001";
    const char *avs12String = "AVS_1001_0002";
    const char *programmeString = "APR_1001";

    dlb_adm_entity_id obj1Id;
    dlb_adm_entity_id obj2Id;
    dlb_adm_entity_id avs11Id;
    dlb_adm_entity_id avs12Id;
    dlb_adm_entity_id programmeId;

    obj1Id      = translator.Translate(obj1String);
    obj2Id      = translator.Translate(obj2String);
    avs11Id     = translator.Translate(avs11String);
    avs12Id     = translator.Translate(avs12String);
    programmeId = translator.Translate(programmeString);
    EXPECT_NE(0, obj1Id);
    EXPECT_NE(0, obj2Id);
    EXPECT_NE(0, avs11Id);
    EXPECT_NE(0, avs12Id);
    EXPECT_NE(0, programmeId);

    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(obj1Id, DLB_ADM_NULL_ENTITY_ID));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(DLB_ADM_NULL_ENTITY_ID, avs11Id));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(avs11Id, obj1Id));

    EXPECT_TRUE(translator.SubcomponentIdReferencesComponent(obj1Id, avs11Id));
    EXPECT_TRUE(translator.SubcomponentIdReferencesComponent(obj1Id, avs12Id));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(obj2Id, avs11Id));

    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(programmeId, programmeId));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(obj1Id, obj1Id));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(programmeId, obj1Id));
    EXPECT_FALSE(translator.SubcomponentIdReferencesComponent(programmeId, avs11Id));
}

TEST(dlb_adm_test, ParseValue_String_to_dlb_adm_time_1)
{
    std::string timeString("12:34:56.23456S78901");
    dlb_adm_time timeConversionResult;

    int status;

    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(timeConversionResult.hours, 12);
    EXPECT_EQ(timeConversionResult.minutes, 34);
    EXPECT_EQ(timeConversionResult.seconds, 56);
    EXPECT_EQ(timeConversionResult.fraction_numerator, 23456);
    EXPECT_EQ(timeConversionResult.fraction_denominator, 78901);

    timeString = "12:34:56.1234567S234567890";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(timeConversionResult.hours, 12);
    EXPECT_EQ(timeConversionResult.minutes, 34);
    EXPECT_EQ(timeConversionResult.seconds, 56);
    EXPECT_EQ(timeConversionResult.fraction_numerator, 1234567);
    EXPECT_EQ(timeConversionResult.fraction_denominator, 234567890);
}

TEST(dlb_adm_test, ParseValue_String_to_dlb_adm_time_2)
{
    std::string timeString("12:34:56.78901");
    dlb_adm_time timeConversionResult;

    int status;

    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(timeConversionResult.hours, 12);
    EXPECT_EQ(timeConversionResult.minutes, 34);
    EXPECT_EQ(timeConversionResult.seconds, 56);
    EXPECT_EQ(timeConversionResult.fraction_numerator, 78901);
    EXPECT_EQ(timeConversionResult.fraction_denominator, 0);

    timeString = "12:34:56.1234567";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(timeConversionResult.hours, 12);
    EXPECT_EQ(timeConversionResult.minutes, 34);
    EXPECT_EQ(timeConversionResult.seconds, 56);
    EXPECT_EQ(timeConversionResult.fraction_numerator, 1234567);
    EXPECT_EQ(timeConversionResult.fraction_denominator, 0);
}

TEST(dlb_adm_test, ParseValue_String_to_dlb_adm_time_3)
{
    std::string timeString("12345S67890");
    dlb_adm_time timeConversionResult;

    int status;

    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(timeConversionResult.hours, 0);
    EXPECT_EQ(timeConversionResult.minutes, 0);
    EXPECT_EQ(timeConversionResult.seconds, 0);
    EXPECT_EQ(timeConversionResult.fraction_numerator, 12345);
    EXPECT_EQ(timeConversionResult.fraction_denominator, 67890);

    timeString = "1234567S234567890";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(timeConversionResult.hours, 0);
    EXPECT_EQ(timeConversionResult.minutes, 0);
    EXPECT_EQ(timeConversionResult.seconds, 0);
    EXPECT_EQ(timeConversionResult.fraction_numerator, 1234567);
    EXPECT_EQ(timeConversionResult.fraction_denominator, 234567890);
}

TEST(dlb_adm_test, ParseValue_String_to_dlb_adm_time_bad_string)
{
    dlb_adm_time timeConversionResult;

    int status;

    // various combinations of bad strings
    std::string timeString("12:34:56.78901S234a6");
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "123:34:56.78901S23406";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "12:340:56.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "12:34:560.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "12:34:0.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "12:3:50.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "1:33:50.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "12:67:50.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    
    timeString = "01:57:90.78901S23416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);

    timeString = "11:57:20.78901S416";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);

    timeString = "11:57:20.789S41611";
    status = ParseValue(timeConversionResult, timeString);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST(dlb_adm_test, ParseValue_float)
{
    dlb_adm_float value;
    EXPECT_EQ(ParseValue(value, std::string("-inf")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, -INFINITY);

    EXPECT_EQ(ParseValue(value, std::string("-INF")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, -INFINITY);

    EXPECT_EQ(ParseValue(value, std::string("inf")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, INFINITY);

    EXPECT_EQ(ParseValue(value, std::string("INF")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, INFINITY);

    EXPECT_EQ(ParseValue(value, std::string("0")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, 0.0);
    EXPECT_EQ(ParseValue(value, std::string("0.1")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, 0.1);
    EXPECT_EQ(ParseValue(value, std::string("1")),DLB_ADM_STATUS_OK);
    EXPECT_FLOAT_EQ(value, 1.0);
}

TEST(dlb_adm_test, WriteValue_float)
{
    std::stringstream ss;
    dlb_adm_float val = -INFINITY;
    DlbAdm::operator<<(ss, val);
    EXPECT_STREQ(ss.str().c_str(), "-INF");

    ss.str(std::string());
    val = INFINITY;
    DlbAdm::operator<<(ss, val);
    EXPECT_STREQ(ss.str().c_str(), "INF");

    ss.str(std::string());
    val = 0.5;
    DlbAdm::operator<<(ss, val);
    EXPECT_STREQ(ss.str().c_str(), "0.50");

    ss.str(std::string());
    val = -10.25;
    DlbAdm::operator<<(ss, val);
    EXPECT_STREQ(ss.str().c_str(), "-10.25");
}
