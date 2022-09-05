/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#include "dlb_pmd/include/dlb_pmd_pcm.h"
#include "dlb_pmd_sadm_file.h"
#include "sadm_bitstream_encoder.h"
#include "sadm_bitstream_decoder.h"
#include "pmd_profile.h"
#include "test_data.h"

#include <string.h>
#include <stdio.h>

class DlbPmdSadm02 : public testing::Test
{
protected:
    static const size_t FRAME_SAMPLES = 1920;

    dlb_pmd_model_constraints limits;
    dlb_pmd_model *pmdModel;
    dlb_pcmpmd_augmentor *augmentor;
    dlb_pcmpmd_extractor *extractor;

    uint8_t *pmdModelMemory;
    uint8_t *encoderMemory;
    uint8_t *decoderMemory;
    uint8_t *augmentorMemory;
    uint8_t *extractorMemory;

    uint8_t             *mPmdModelComboMemory;
    dlb_pmd_model_combo *mPmdModelCombo;

    uint8_t binaryBuffer[MAX_DATA_BYTES];
    char decodedXml[MAX_DATA_BYTES * 4];
    char compareXml[MAX_DATA_BYTES * 4];

    uint32_t pcmBuffer[FRAME_SAMPLES * 2];

    void SetLimits()
    {
        pmd_profile p;

        pmd_profile_set(&p, 0, 0, NULL);
        limits = p.constraints;
    }

    bool InitComboModel(dlb_pmd_model *existing_pmd_model, dlb_adm_core_model *existing_core_model)
    {
        bool good = true;

        try
        {
            size_t sz = ::dlb_pmd_model_combo_query_mem(existing_pmd_model, existing_core_model);
            dlb_pmd_success success;

            if (sz == 0) throw false;
            mPmdModelComboMemory = new uint8_t[sz];
            if (mPmdModelComboMemory == nullptr) throw false;
            success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, existing_pmd_model, existing_core_model, PMD_FALSE, mPmdModelComboMemory);
            if ((success == PMD_FAIL) || (mPmdModelCombo == nullptr)) throw false;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    virtual void SetUp()
    {
        SetLimits();

        pmdModel = nullptr;
        pmdModelMemory = nullptr;
        augmentor = nullptr;
        extractor = nullptr;

        encoderMemory = nullptr;
        decoderMemory = nullptr;
        augmentorMemory = nullptr;
        extractorMemory = nullptr;

        mPmdModelCombo = nullptr;
        mPmdModelComboMemory = nullptr;

        ::memset(binaryBuffer, 0, sizeof(binaryBuffer));
        ::memset(decodedXml,   0, sizeof(decodedXml));
        ::memset(compareXml,   0, sizeof(compareXml));
        ::memset(pcmBuffer,    0, sizeof(pcmBuffer));
    }

    virtual void TearDown()
    {
        if (mPmdModelCombo != nullptr)
        {
            (void)dlb_pmd_model_combo_destroy(&mPmdModelCombo);
        }
        if (mPmdModelComboMemory != nullptr)
        {
            delete[] mPmdModelComboMemory;
            mPmdModelComboMemory = nullptr;
        }
        if (extractor != nullptr)
        {
            dlb_pcmpmd_extractor_finish(extractor);
        }
        if (extractorMemory != nullptr)
        {
            delete[] extractorMemory;
            extractorMemory = nullptr;
        }

        if (augmentor != nullptr)
        {
            dlb_pcmpmd_augmentor_finish(augmentor);
            augmentor = nullptr;
        }
        if (augmentorMemory != nullptr)
        {
            delete[] augmentorMemory;
            augmentorMemory = nullptr;
        }

        if (decoderMemory != nullptr)
        {
            delete[] decoderMemory;
            decoderMemory = nullptr;
        }
        if (encoderMemory != nullptr)
        {
            delete[] encoderMemory;
            encoderMemory = nullptr;
        }

        if (pmdModel != nullptr)
        {
            dlb_pmd_finish(pmdModel);
            pmdModel = nullptr;
        }
        if (pmdModelMemory != nullptr)
        {
            delete[] pmdModelMemory;
            pmdModelMemory = nullptr;
        }
    }

    dlb_pmd_success WriteStringToFile(const char *fileName, const char *str)
    {
        FILE *f = ::fopen(fileName, "w");
        dlb_pmd_success success = PMD_FAIL;

        if (f != nullptr)
        {
            ::fputs(str, f);
            ::fclose(f);
            success = PMD_SUCCESS;
        }

        return success;
    }

    dlb_pmd_success ReadStringFromFile(char *str, size_t sz, const char *fileName)
    {
        FILE *f = ::fopen(fileName, "r");
        dlb_pmd_success success = PMD_FAIL;

        if (f != nullptr)
        {
            size_t n, m;

            ::fseek(f, 0, SEEK_END);
            n = ::ftell(f);
            ::fseek(f, 0, SEEK_SET);
            if (n > sz)
            {
                n = sz;
            }
            m = ::fread(str, 1, n, f);
            ::fclose(f);
            if (m > 0)
            {
                success = PMD_SUCCESS;
            }
        }

        return success;
    }

    static void errorCallback(const char *msg, void *arg)
    {
        (void)arg;
        ::puts(msg);
    }

};

TEST_F(DlbPmdSadm02, BitstreamCompressDecompress)
{
    sadm_bitstream_encoder *encoder = nullptr;
    sadm_bitstream_decoder *decoder = nullptr;
    dlb_pmd_success success;
    int encodedCount;
    int decodedCount;
    int comp;
    size_t len;
    size_t sz;

    sz = ::sadm_bitstream_encoder_query_mem();
    encoderMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, encoderMemory);

    success = ::sadm_bitstream_encoder_init(encoderMemory, &encoder);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    ASSERT_LE(sizeof(smallXML1), sizeof(encoder->xmlbuf));

    ::strcpy(encoder->xmlbuf, smallXML1);
    len = ::strlen(encoder->xmlbuf);
    encoder->size = len;
    encodedCount = ::compress_sadm_xml(encoder, binaryBuffer);
    EXPECT_LT(0, encodedCount);

    sz = ::sadm_bitstream_decoder_query_mem();
    decoderMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, decoderMemory);

    success = ::sadm_bitstream_decoder_init(decoderMemory, &decoder);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    decodedCount = ::decompress_sadm_xml(decoder, binaryBuffer, encodedCount);
    EXPECT_EQ(static_cast<int>(len), decodedCount);
    ASSERT_LT(decodedCount, static_cast<int>(sizeof(decodedXml)));
    decodedXml[decodedCount] = '\0';
    ::strncpy(decodedXml, decoder->xmlbuf, decodedCount);
    comp = ::strcmp(smallXML1, decodedXml);
    EXPECT_EQ(0, comp);
}

TEST_F(DlbPmdSadm02, BitstreamEncodeDecodeSubframeMode)
{
    //
    // NOTE: because this does file i/o, we may need to move it over into the integration
    // testing, as it may be too slow for developer unit testing.
    //

    const char *inputXMLFileName   = "stereo_sadm_02_subframe_input.xml";
    const char *compareXMLFileName = "stereo_sadm_02_subframe_compare.xml";
    const char *outputXMLFileName  = "stereo_sadm_02_subframe_output.xml";
    dlb_pmd_success success;
    const dlb_pmd_model *cpm;
    int compare;
    size_t sz;

    // Write test input file
    success = WriteStringToFile(inputXMLFileName, smallXML1);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Create the PMD model
    sz = ::dlb_pmd_query_mem_constrained(&limits);
    pmdModelMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, pmdModelMemory);
    ::dlb_pmd_init_constrained(&pmdModel, &limits, pmdModelMemory);

    // Ingest test XML into the PMD model
    ASSERT_TRUE(InitComboModel(pmdModel, nullptr));
    success = ::dlb_pmd_sadm_file_read(inputXMLFileName, mPmdModelCombo, PMD_FALSE, errorCallback, NULL);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Write comparison XML to file
    success = ::dlb_pmd_sadm_file_write(compareXMLFileName, mPmdModelCombo);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Encode to binary
    sz = ::dlb_pcmpmd_augmentor_query_mem(PMD_TRUE);
    augmentorMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, augmentorMemory);
    ::dlb_pcmpmd_augmentor_init2
    (
        &augmentor,
        mPmdModelCombo,
        augmentorMemory,
        DLB_PMD_FRAMERATE_2500,
        DLB_PMD_KLV_UL_ST2109,
        PMD_FALSE,
        1,
        1,
        PMD_FALSE,
        0,
        PMD_TRUE
    );
    ::dlb_pcmpmd_augment(augmentor, pcmBuffer, FRAME_SAMPLES, 0);
    EXPECT_NE(0u, pcmBuffer[GUARDBAND]);

    // Reinitialize the model
    success = dlb_pmd_model_combo_clear(mPmdModelCombo);
    ASSERT_EQ(PMD_SUCCESS, success);

    // Decode from binary
    sz = ::dlb_pcmpmd_extractor_query_mem(PMD_TRUE);
    extractorMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, extractorMemory);
    dlb_pcmpmd_extractor_init2
    (
        &extractor,
        extractorMemory,
        DLB_PMD_FRAMERATE_2500,
        0,
        1,
        PMD_FALSE,
        mPmdModelCombo,
        nullptr,
        PMD_TRUE
    );
    success = ::dlb_pcmpmd_extract(extractor, pcmBuffer, FRAME_SAMPLES, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Convert to PMD
    success = dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, inputXMLFileName, &cpm);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    ASSERT_EQ(pmdModel, cpm);

    // Write decoded XML to file
    success = ::dlb_pmd_sadm_file_write(outputXMLFileName, mPmdModelCombo);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Compare XML files
    success = ReadStringFromFile(decodedXml, sizeof(decodedXml) - 1, outputXMLFileName);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = ReadStringFromFile(compareXml, sizeof(compareXml) - 1, compareXMLFileName);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    compare = ::strcmp(decodedXml, compareXml);
    EXPECT_EQ(0, compare);
}

TEST_F(DlbPmdSadm02, BitstreamEncodeDecodeFrameMode)
{
    //
    // NOTE: because this does file i/o, we may need to move it over into the integration
    // testing, as it may be too slow for developer unit testing.
    //

    const char *inputXMLFileName   = "stereo_sadm_02_frame_input.xml";
    const char *compareXMLFileName = "stereo_sadm_02_frame_compare.xml";
    const char *outputXMLFileName  = "stereo_sadm_02_frame_output.xml";
    dlb_pmd_success success;
    const dlb_pmd_model *cpm;
    int compare;
    size_t sz;

    // Write test input file
    success = WriteStringToFile(inputXMLFileName, smallXML1);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Create the PMD model
    sz = ::dlb_pmd_query_mem_constrained(&limits);
    pmdModelMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, pmdModelMemory);
    ::dlb_pmd_init_constrained(&pmdModel, &limits, pmdModelMemory);

    // Ingest test XML into the PMD model
    ASSERT_TRUE(InitComboModel(pmdModel, nullptr));
    success = ::dlb_pmd_sadm_file_read(inputXMLFileName, mPmdModelCombo, PMD_FALSE, errorCallback, NULL);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Write comparison XML to file
    success = ::dlb_pmd_sadm_file_write(compareXMLFileName, mPmdModelCombo);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Encode to binary
    sz = ::dlb_pcmpmd_augmentor_query_mem(PMD_TRUE);
    augmentorMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, augmentorMemory);
    ::dlb_pcmpmd_augmentor_init2
    (
        &augmentor,
        mPmdModelCombo,
        augmentorMemory,
        DLB_PMD_FRAMERATE_2500,
        DLB_PMD_KLV_UL_ST2109,
        PMD_FALSE,
        2,
        2,
        PMD_TRUE,
        0,
        PMD_TRUE
    );
    ::dlb_pcmpmd_augment(augmentor, pcmBuffer, FRAME_SAMPLES, 0);
    EXPECT_NE(0u, pcmBuffer[GUARDBAND * 2]);

    // Reinitialize the model
    success = dlb_pmd_model_combo_clear(mPmdModelCombo);
    ASSERT_EQ(PMD_SUCCESS, success);

    // Decode from binary
    sz = ::dlb_pcmpmd_extractor_query_mem(PMD_TRUE);
    extractorMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, extractorMemory);
    dlb_pcmpmd_extractor_init2
    (
        &extractor,
        extractorMemory,
        DLB_PMD_FRAMERATE_2500,
        0,
        2,
        PMD_TRUE,
        mPmdModelCombo,
        nullptr,
        PMD_TRUE
    );
    success = ::dlb_pcmpmd_extract(extractor, pcmBuffer, FRAME_SAMPLES, 0);
    EXPECT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Convert to PMD
    success = dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, inputXMLFileName, &cpm);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    ASSERT_EQ(pmdModel, cpm);

    // Write decoded XML to file
    success = ::dlb_pmd_sadm_file_write(outputXMLFileName, mPmdModelCombo);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Compare XML files
    success = ReadStringFromFile(decodedXml, sizeof(decodedXml) - 1, outputXMLFileName);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = ReadStringFromFile(compareXml, sizeof(compareXml) - 1, compareXMLFileName);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    compare = ::strcmp(decodedXml, compareXml);
    EXPECT_EQ(0, compare);
}

TEST_F(DlbPmdSadm02, InOutAndCompareADM)
{
    const char *inputXMLFileName = "stereo_2D_ADM_sadm_02_input.xml";
    const char *outputXMLFileName = "stereo_2D_ADM_sadm_02_output.xml";
    dlb_pmd_success success;
    int compare;

    // Write test input file
    success = WriteStringToFile(inputXMLFileName, stereo_2D_ADM);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    // In
    success = ::dlb_pmd_sadm_file_read(inputXMLFileName, mPmdModelCombo, PMD_FALSE, errorCallback, NULL);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Out
    success = ::dlb_pmd_sadm_file_write(outputXMLFileName, mPmdModelCombo);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    // Compare
    success = ReadStringFromFile(decodedXml, sizeof(decodedXml) - 1, outputXMLFileName);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    success = ReadStringFromFile(compareXml, sizeof(compareXml) - 1, inputXMLFileName);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    compare = ::strcmp(decodedXml, compareXml);
    EXPECT_EQ(0, compare);  // v2.0.0 no longer takes S-ADM through the PMD model, so they are the same
}
