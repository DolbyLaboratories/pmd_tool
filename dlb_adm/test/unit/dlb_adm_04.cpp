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

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "AdmIdTranslator.h"
#include "AudioElement.h"
#include "AlternativeValueSet.h"
#include "AudioObjectInteraction.h"
#include "AudioTrack.h"
#include "Source.h"
#include "TargetGroup.h"
#include "Target.h"
#include "ElementRecord.h"
#include "Presentation.h"
#include "ContentGroup.h"
#include "ElementGroup.h"
#include "PresentationRecord.h"
#include "ModelEntityContainer.h"
#include "CoreModel.h"

#include <cmath>
#include <memory>
#include <boost/interprocess/managed_heap_memory.hpp>

using namespace DlbAdm;

static const ChannelNumber ch1 = 1u;
static const ChannelNumber ch2 = 2u;
static const ChannelNumber ch3 = 3u;
static const ChannelNumber ch4 = 4u;

static const char *apr1String = "APR_1001";
static const char *apr2String = "APR_1002";
static const char *apr3String = "APR_1003";     // Not added to model
static const char *aco1String = "ACO_1001";
static const char *aco2String = "ACO_1002";
static const char *aco3String = "ACO_1003";
static const char *ao1String  = "AO_1001";
static const char *ao2String  = "AO_1002";
static const char *ao3String  = "AO_1003";
static const char *atu1String = "ATU_00000001";
static const char *atu2String = "ATU_00000002";
static const char *atu3String = "ATU_00000003";
static const char *atu4String = "ATU_00000004";
static const char *ap1String  = "AP_00011001";
static const char *ap2String  = "AP_00031002";
static const char *ap3String  = "AP_00031003";
static const char *ac1String  = "AC_00011001";
static const char *ac2String  = "AC_00011002";
static const char *ac3String  = "AC_00031002";
static const char *ac4String  = "AC_00031003";
static const char *avs1String  = "AVS_1001_0001";
static const char *avs2String  = "AVS_1001_0002";
static const char *avs3String  = "AVS_1002_0001";
static const char *avs4String  = "AVS_1002_0002";

class DlbAdm04 : public testing::Test
{
protected:

    dlb_adm_entity_id mPres1ID;
    dlb_adm_entity_id mPres2ID;
    dlb_adm_entity_id mPres3ID;
    dlb_adm_entity_id mContent1ID;
    dlb_adm_entity_id mContent2ID;
    dlb_adm_entity_id mContent3ID;
    dlb_adm_entity_id mElement1ID;
    dlb_adm_entity_id mElement2ID;
    dlb_adm_entity_id mElement3ID;
    dlb_adm_entity_id mTrack1ID;
    dlb_adm_entity_id mTrack2ID;
    dlb_adm_entity_id mTrack3ID;
    dlb_adm_entity_id mTrack4ID;
    dlb_adm_entity_id mTargetGrp1ID;
    dlb_adm_entity_id mTargetGrp2ID;
    dlb_adm_entity_id mTargetGrp3ID;
    dlb_adm_entity_id mTarget1ID;
    dlb_adm_entity_id mTarget2ID;
    dlb_adm_entity_id mTarget3ID;
    dlb_adm_entity_id mTarget4ID;
    dlb_adm_entity_id mSource1ID;
    dlb_adm_entity_id mSource2ID;
    dlb_adm_entity_id mSource3ID;
    dlb_adm_entity_id mSource4ID;
    dlb_adm_entity_id mAltValSet1ID;
    dlb_adm_entity_id mAltValSet2ID;
    dlb_adm_entity_id mAltValSet3ID;
    dlb_adm_entity_id mAltValSet4ID;

    dlb_adm_data_loudness mPres1Loudness;
    dlb_adm_data_loudness mPres2Loudness;
    dlb_adm_data_loudness mContent1Loudness;
    dlb_adm_data_loudness mContent2Loudness;

    bool mIDsGood;

    void CreateIDs()
    {
        AdmIdTranslator translator;

        mPres1ID = translator.Translate(apr1String);
        mPres2ID = translator.Translate(apr2String);
        mPres3ID = translator.Translate(apr3String);
        mContent1ID = translator.Translate(aco1String);
        mContent2ID = translator.Translate(aco2String);
        mContent3ID = translator.Translate(aco3String);
        mElement1ID = translator.Translate(ao1String);
        mElement2ID = translator.Translate(ao2String);
        mElement3ID = translator.Translate(ao3String);
        mTrack1ID = translator.Translate(atu1String);
        mTrack2ID = translator.Translate(atu2String);
        mTrack3ID = translator.Translate(atu3String);
        mTrack4ID = translator.Translate(atu4String);
        mTargetGrp1ID = translator.Translate(ap1String);
        mTargetGrp2ID = translator.Translate(ap2String);
        mTargetGrp3ID = translator.Translate(ap3String);
        mTarget1ID = translator.Translate(ac1String);
        mTarget2ID = translator.Translate(ac2String);
        mTarget3ID = translator.Translate(ac3String);
        mTarget4ID = translator.Translate(ac4String);
        mSource1ID = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, static_cast<uint32_t>(ch1));
        mSource2ID = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, static_cast<uint32_t>(ch2));
        mSource3ID = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, static_cast<uint32_t>(ch3));
        mSource4ID = translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, static_cast<uint32_t>(ch4));
        mAltValSet1ID = translator.Translate(avs1String);
        mAltValSet2ID = translator.Translate(avs2String);
        mAltValSet3ID = translator.Translate(avs3String);
        mAltValSet4ID = translator.Translate(avs4String);
    }

    bool CheckIDs()
    {
        return
            mPres1ID != DLB_ADM_NULL_ENTITY_ID &&
            mPres2ID != DLB_ADM_NULL_ENTITY_ID &&
            mPres3ID != DLB_ADM_NULL_ENTITY_ID &&
            mContent1ID != DLB_ADM_NULL_ENTITY_ID &&
            mContent2ID != DLB_ADM_NULL_ENTITY_ID &&
            mContent3ID != DLB_ADM_NULL_ENTITY_ID &&
            mElement1ID != DLB_ADM_NULL_ENTITY_ID &&
            mElement2ID != DLB_ADM_NULL_ENTITY_ID &&
            mElement3ID != DLB_ADM_NULL_ENTITY_ID &&
            mTrack1ID != DLB_ADM_NULL_ENTITY_ID &&
            mTrack2ID != DLB_ADM_NULL_ENTITY_ID &&
            mTrack3ID != DLB_ADM_NULL_ENTITY_ID &&
            mTrack4ID != DLB_ADM_NULL_ENTITY_ID &&
            mTargetGrp1ID != DLB_ADM_NULL_ENTITY_ID &&
            mTargetGrp2ID != DLB_ADM_NULL_ENTITY_ID &&
            mTargetGrp3ID != DLB_ADM_NULL_ENTITY_ID &&
            mTarget1ID != DLB_ADM_NULL_ENTITY_ID &&
            mTarget2ID != DLB_ADM_NULL_ENTITY_ID &&
            mTarget3ID != DLB_ADM_NULL_ENTITY_ID &&
            mTarget4ID != DLB_ADM_NULL_ENTITY_ID &&
            mSource1ID != DLB_ADM_NULL_ENTITY_ID &&
            mSource2ID != DLB_ADM_NULL_ENTITY_ID &&
            mSource3ID != DLB_ADM_NULL_ENTITY_ID &&
            mSource4ID != DLB_ADM_NULL_ENTITY_ID &&
            mAltValSet1ID != DLB_ADM_NULL_ENTITY_ID &&
            mAltValSet2ID != DLB_ADM_NULL_ENTITY_ID &&
            mAltValSet3ID != DLB_ADM_NULL_ENTITY_ID &&
            mAltValSet4ID != DLB_ADM_NULL_ENTITY_ID ;
    }

    virtual void SetUp()
    {
        CreateIDs();
        mPres1Loudness = {-0.5f, DLB_ADM_LOUDNESS_TYPE_INTEGRATED};
        mPres2Loudness = {2.0f, DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED};
        mContent1Loudness = {-35.6f, DLB_ADM_LOUDNESS_TYPE_INTEGRATED};
        mContent2Loudness = {24.9f, DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED};
        mIDsGood = CheckIDs();
    }

    virtual void TearDown()
    {
    }

};

TEST(dlb_adm_test, GainConversion)
{
    float f, g, h;

    g = Gain::LinearToDecibels(1.0f);
    EXPECT_EQ(0.0f, g);
    g = Gain::DecibelsToLinear(g);
    EXPECT_EQ(1.0f, g);

    g = Gain::LinearToDecibels(0.0f);
    EXPECT_TRUE(std::isinf(g));
    g = Gain::DecibelsToLinear(g);
    EXPECT_EQ(0.0f, g);

    g = Gain::LinearToDecibels(2.0f);
    f = ::floor(g);
    h = ::floor(g + 0.5f);
    EXPECT_EQ(6.0f, f);
    EXPECT_EQ(6.0f, h);
    g = Gain::DecibelsToLinear(g);
    h = ::floor(g + 0.5f);
    EXPECT_EQ(2.0f, h);

    Gain gain(1.0f, Gain::GAIN_UNIT::LINEAR);

    gain.Convert(Gain::GAIN_UNIT::DECIBELS);
    EXPECT_EQ(0.0f, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::DECIBELS, gain.GetGainUnit());
    gain.Convert(Gain::GAIN_UNIT::LINEAR);
    EXPECT_EQ(1.0f, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
}

TEST(dlb_adm_test, PositionConversion)
{
    float CENTER_POS = 0.0f;
    float FRONT_POS = 1.0f;
    float FLOR_POS = 0.0f;

    Position posCartesian(CENTER_POS, FRONT_POS, FLOR_POS, true);
    Position posSpherical = Position::CartesianToSpherical(posCartesian);
    Position posCartesian2 = Position::SphericalToCartesian(posSpherical);

    EXPECT_EQ(posCartesian.GetCoordinate1(), posCartesian2.GetCoordinate1());
    EXPECT_EQ(posCartesian.GetCoordinate2(), posCartesian2.GetCoordinate2());
    EXPECT_EQ(posCartesian.GetCoordinate3(), posCartesian2.GetCoordinate3());
}


TEST(dlb_adm_test, AudioObjectInteractionBasic)
{
    bool onOfInteract = false;
    bool gainInteract = true;
    bool positionInteract = true;
    float MIN_GAIN = -std::numeric_limits<float>::infinity();
    float MAX_GAIN = 30.0f;
    float MIN_POS = -1;
    float MAX_POS = 1;
    float SOME_POS = 0.4f;

    Gain minGain(MIN_GAIN, Gain::GAIN_UNIT::DECIBELS);
    Gain maxGain(MAX_GAIN, Gain::GAIN_UNIT::DECIBELS);
    std::map<Position::COORDINATE, float> minPositions;
    std::map<Position::COORDINATE, float> maxPositions;

    minPositions[Position::COORDINATE::X] = MIN_POS;
    maxPositions[Position::COORDINATE::X] = SOME_POS;
    minPositions[Position::COORDINATE::AZIMUTH] = 0.0f;
    maxPositions[Position::COORDINATE::Y] = MAX_POS;

    // constructors
    AudioObjectInteraction aoi(onOfInteract, gainInteract, positionInteract, minGain, maxGain, minPositions, maxPositions);
    EXPECT_FLOAT_EQ(aoi.GetMinGainRange().GetGainValue(), MIN_GAIN);
    EXPECT_FLOAT_EQ(aoi.GetMaxPositionRange().at(Position::COORDINATE::X), SOME_POS);
    EXPECT_EQ(aoi.GetMinPositionRange().size(), 1u);

    // check parameters validation
    gainInteract = false;
    AudioObjectInteraction aoi2(onOfInteract, gainInteract, positionInteract, minGain, maxGain, minPositions);
    EXPECT_NE(aoi2.GetMinGainRange(), minGain);
    EXPECT_EQ(aoi2.GetMaxPositionRange().size(), 0u);
    EXPECT_EQ(aoi2.GetMinPositionRange().size(), 0u);
}

TEST(dlb_adm_test, EntityNameBasics)
{
    static const char *name1 = "First Name";
    static const char *name2 = "Second Name";
    static const char *lang = "en";

    EntityName firstName(name1);
    EntityName secondName(name2, lang);

    EXPECT_FALSE(firstName.HasLanguage());
    EXPECT_TRUE(secondName.HasLanguage());

    EXPECT_EQ(std::string(name1), firstName.GetName());
    EXPECT_EQ(std::string(name2), secondName.GetName());
    EXPECT_NE(firstName.GetName(), secondName.GetName());
    EXPECT_NE(std::string(lang), firstName.GetLanguage());
    EXPECT_EQ(std::string(lang), secondName.GetLanguage());
    EXPECT_NE(firstName.GetLanguage(), secondName.GetLanguage());
}

TEST(dlb_adm_test, AudioElementBasic)
{
    static const char *idString = "AO_1001";
    static const float gainValue = 1.0f;
    static const dlb_adm_data_position_offset positionOffsetValue = {60, DLB_ADM_FALSE};

    AdmIdTranslator translator;
    dlb_adm_entity_id elementID = translator.Translate(idString);
    dlb_adm_entity_id checkId;

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, elementID);

    // constructor

    AudioElement element1(elementID, gainValue, Gain::GAIN_UNIT::LINEAR, positionOffsetValue);
    Gain gain = element1.GetGain();
    size_t n = element1.GetNameLimit();
    Position positionOffset = element1.GetPositionOffset();

    checkId = element1.GetEntityID();
    EXPECT_EQ(elementID, checkId);
    EXPECT_EQ(gainValue, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ (positionOffset.GetCoordinate1(), positionOffsetValue.offset_value);
    EXPECT_EQ (positionOffset.IsCartesian(), positionOffsetValue.cartesian);

    // copy constructor

    AudioElement element2(element1);

    gain = element2.GetGain();
    checkId = element2.GetEntityID();
    n = element2.GetNameLimit();
    positionOffset = element2.GetPositionOffset();
    EXPECT_EQ(elementID, checkId);
    EXPECT_EQ(gainValue, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ (positionOffset.GetCoordinate1(), positionOffsetValue.offset_value);
    EXPECT_EQ (positionOffset.IsCartesian(), positionOffsetValue.cartesian);

    // assignment operator

    AudioElement element3;

    element3 = element1;
    gain = element3.GetGain();
    checkId = element3.GetEntityID();
    n = element3.GetNameLimit();
    positionOffset = element3.GetPositionOffset();
    EXPECT_EQ(elementID, checkId);
    EXPECT_EQ(gainValue, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ (positionOffset.GetCoordinate1(), positionOffsetValue.offset_value);
    EXPECT_EQ (positionOffset.IsCartesian(), positionOffsetValue.cartesian);
}

TEST(dlb_adm_test, AlternativeValueSetBasic)
{
    int status;

    static const char *idString = "AVS_1001_0001";

    AdmIdTranslator translator;
    const dlb_adm_entity_id avsID = translator.Translate(idString);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, avsID);

    const Position sourcePositionOffset(0.3, true);
    const Gain     sourceGain(-10.1, Gain::GAIN_UNIT::DECIBELS);
    const std::string sourceName1("English");
    const std::string sourceLang1("eng");

    dlb_adm_entity_id checkId;
    Gain checkGain;
    Position checkPositionOffset;
    EntityName checkName;

    // constructor from struct dlb_adm_data_alt_value_set

    const dlb_adm_data_alt_value_set avsStruct0{avsID, DLB_ADM_FALSE, DLB_ADM_FALSE, {0.0, 0.0, 0.0}, DLB_ADM_FALSE, {0.0, DLB_ADM_GAIN_UNIT_LINEAR}};
    AlternativeValueSet avsFromStruct0(avsStruct0);

    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, avsFromStruct0.GetGain(checkGain));
    EXPECT_EQ(false, avsFromStruct0.HasGain());

    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, avsFromStruct0.GetPositionOffset(checkPositionOffset));
    EXPECT_EQ(false, avsFromStruct0.HasPositionOffset());

    size_t n = avsFromStruct0.GetNameLimit();
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_EQ(0u, avsFromStruct0.GetLabelCount());

    const dlb_adm_data_alt_value_set avsStruct1{avsID, DLB_ADM_TRUE, DLB_ADM_TRUE, {0.3, 0.0, 0.0}, DLB_ADM_TRUE, {-10.1, DLB_ADM_GAIN_UNIT_DB}};
    AlternativeValueSet avsFromStruct1(avsStruct1);

    n = avsFromStruct1.GetNameLimit();
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_EQ(0u, avsFromStruct1.GetLabelCount());

    EXPECT_TRUE(avsFromStruct1.HasPositionOffset());
    status = avsFromStruct1.GetPositionOffset(checkPositionOffset);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkPositionOffset.GetCoordinate1(), sourcePositionOffset.GetCoordinate1());
    EXPECT_EQ(checkPositionOffset.IsCartesian(), sourcePositionOffset.IsCartesian());

    EXPECT_TRUE(avsFromStruct1.HasGain());
    status = avsFromStruct1.GetGain(checkGain);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkGain.GetGainValue(), sourceGain.GetGainValue());
    EXPECT_EQ(checkGain.GetGainUnit(), sourceGain.GetGainUnit());

    // constructors

    AlternativeValueSet avs0(avsID);

    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, avs0.GetGain(checkGain));
    EXPECT_EQ(false, avs0.HasGain());

    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, avs0.GetPositionOffset(checkPositionOffset));
    EXPECT_EQ(false, avs0.HasPositionOffset());

    n = avs0.GetNameLimit();
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_EQ(0u, avs0.GetLabelCount());

    checkId = avs0.GetEntityID();
    EXPECT_EQ(avsID, checkId);

    AlternativeValueSet avs1(avsID, sourcePositionOffset, sourceGain);
    EXPECT_TRUE(avs1.AddLabel(sourceName1, sourceLang1));

    n = avs1.GetNameLimit();
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FALSE(avs1.HasName());
    EXPECT_EQ(avs1.GetLabelCount(), 1u);
    EXPECT_TRUE(avs1.GetName(checkName, 0));
    EXPECT_EQ(checkName.GetName(), sourceName1);
    EXPECT_TRUE(checkName.HasLanguage());
    EXPECT_EQ(checkName.GetLanguage(), sourceLang1);

    EXPECT_TRUE(avs1.HasPositionOffset());
    status = avs1.GetPositionOffset(checkPositionOffset);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkPositionOffset.GetCoordinate1(), sourcePositionOffset.GetCoordinate1());
    EXPECT_EQ(checkPositionOffset.IsCartesian(), sourcePositionOffset.IsCartesian());

    EXPECT_TRUE(avs1.HasGain());
    status = avs1.GetGain(checkGain);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkGain.GetGainValue(), sourceGain.GetGainValue());
    EXPECT_EQ(checkGain.GetGainUnit(), sourceGain.GetGainUnit());

    // copy constructor

    AlternativeValueSet avs2(avs1);

    n = avs2.GetNameLimit();
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FALSE(avs2.HasName());
    EXPECT_EQ(avs2.GetLabelCount(), 1u);
    EXPECT_TRUE(avs2.GetName(checkName, 0));
    EXPECT_EQ(checkName.GetName(), sourceName1);
    EXPECT_TRUE(checkName.HasLanguage());
    EXPECT_EQ(checkName.GetLanguage(), sourceLang1);

    EXPECT_TRUE(avs2.HasPositionOffset());
    status = avs2.GetPositionOffset(checkPositionOffset);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkPositionOffset.GetCoordinate1(), sourcePositionOffset.GetCoordinate1());
    EXPECT_EQ(checkPositionOffset.IsCartesian(), sourcePositionOffset.IsCartesian());

    EXPECT_TRUE(avs2.HasGain());
    status = avs2.GetGain(checkGain);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkGain.GetGainValue(), sourceGain.GetGainValue());
    EXPECT_EQ(checkGain.GetGainUnit(), sourceGain.GetGainUnit());

    // assignment operator

    AlternativeValueSet avs3;
    avs3 = avs2;

    n = avs3.GetNameLimit();
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FALSE(avs3.HasName());
    EXPECT_EQ(avs3.GetLabelCount(), 1u);
    EXPECT_TRUE(avs3.GetName(checkName, 0));
    EXPECT_EQ(checkName.GetName(), sourceName1);
    EXPECT_TRUE(checkName.HasLanguage());
    EXPECT_EQ(checkName.GetLanguage(), sourceLang1);

    EXPECT_TRUE(avs3.HasPositionOffset());
    status = avs3.GetPositionOffset(checkPositionOffset);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkPositionOffset.GetCoordinate1(), sourcePositionOffset.GetCoordinate1());
    EXPECT_EQ(checkPositionOffset.IsCartesian(), sourcePositionOffset.IsCartesian());

    EXPECT_TRUE(avs3.HasGain());
    status = avs3.GetGain(checkGain);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FLOAT_EQ(checkGain.GetGainValue(), sourceGain.GetGainValue());
    EXPECT_EQ(checkGain.GetGainUnit(), sourceGain.GetGainUnit());
}

TEST(dlb_adm_test, AudioTrackBasic)
{
    static const char *idString = "ATU_00000001";
    static const SampleRate sampleRate = 48000;
    static const BitDepth bitDepth = 24;

    AdmIdTranslator translator;
    dlb_adm_entity_id trackID = translator.Translate(idString);
    dlb_adm_entity_id checkId;

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, trackID);

    // constructor

    AudioTrack audioTrack1(trackID, sampleRate, bitDepth);
    SampleRate sr = audioTrack1.GetSampleRate();
    BitDepth bd = audioTrack1.GetBitDepth();
    size_t n = audioTrack1.GetNameLimit();

    checkId = audioTrack1.GetEntityID();
    EXPECT_EQ(trackID, checkId);
    EXPECT_EQ(sampleRate, sr);
    EXPECT_EQ(bitDepth, bd);
    EXPECT_EQ(0u, n);

    // copy constructor

    AudioTrack audioTrack2(audioTrack1);

    checkId = audioTrack2.GetEntityID();
    sr = audioTrack2.GetSampleRate();
    bd = audioTrack2.GetBitDepth();
    n = audioTrack2.GetNameLimit();
    EXPECT_EQ(trackID, checkId);
    EXPECT_EQ(sampleRate, sr);
    EXPECT_EQ(bitDepth, bd);
    EXPECT_EQ(0u, n);

    // assignment operator

    AudioTrack audioTrack3;

    audioTrack3 = audioTrack1;
    checkId = audioTrack3.GetEntityID();
    sr = audioTrack3.GetSampleRate();
    bd = audioTrack3.GetBitDepth();
    n = audioTrack3.GetNameLimit();
    EXPECT_EQ(trackID, checkId);
    EXPECT_EQ(sampleRate, sr);
    EXPECT_EQ(bitDepth, bd);
    EXPECT_EQ(0u, n);
}

TEST(dlb_adm_test, SourceBasic)
{
    static const ChannelNumber ch5 = 5u;

    AdmIdTranslator translator;
    dlb_adm_entity_id sourceID =
        translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, static_cast<uint32_t>(ch5));
    dlb_adm_entity_id checkId;

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, sourceID);

    // constructor

    Source source1(sourceID, ch5);
    SourceGroupID groupID = source1.GetSourceGroupID();
    ChannelNumber ch = source1.GetChannelNumber();
    size_t n = source1.GetNameLimit();

    checkId = source1.GetEntityID();
    EXPECT_EQ(sourceID, checkId);
    EXPECT_EQ(DEFAULT_SOURCE_GROUP_ID, groupID);
    EXPECT_EQ(ch5, ch);
    EXPECT_EQ(0u, n);

    // copy constructor

    Source source2(source1);

    groupID = source2.GetSourceGroupID();
    ch = source2.GetChannelNumber();
    n = source2.GetNameLimit();
    checkId = source2.GetEntityID();
    EXPECT_EQ(sourceID, checkId);
    EXPECT_EQ(DEFAULT_SOURCE_GROUP_ID, groupID);
    EXPECT_EQ(ch5, ch);
    EXPECT_EQ(0u, n);

    // assignment operator

    Source source3;

    source3 = source1;
    groupID = source3.GetSourceGroupID();
    ch = source3.GetChannelNumber();
    n = source3.GetNameLimit();
    checkId = source3.GetEntityID();
    EXPECT_EQ(sourceID, checkId);
    EXPECT_EQ(DEFAULT_SOURCE_GROUP_ID, groupID);
    EXPECT_EQ(ch5, ch);
    EXPECT_EQ(0u, n);
}

TEST(dlb_adm_test, TargetGroupBasic)
{
    static const char *bedIDString = "AP_00011001";
    static const char *objIDString = "AP_00031002";
    static const DLB_ADM_SPEAKER_CONFIG spkrCfg = DLB_ADM_SPEAKER_CONFIG_5_1_2;
    static const DLB_ADM_AUDIO_TYPE audioType = DLB_ADM_AUDIO_TYPE_OBJECTS;

    AdmIdTranslator translator;
    dlb_adm_entity_id bedID = translator.Translate(bedIDString);
    dlb_adm_entity_id objID = translator.Translate(objIDString);
    dlb_adm_entity_id checkId;

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, bedID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, objID);

    // constructor (bed)

    TargetGroup bed1(bedID, spkrCfg);
    DLB_ADM_SPEAKER_CONFIG checkCfg = bed1.GetSpeakerConfig();
    DLB_ADM_AUDIO_TYPE checkAudioType = bed1.GetAudioType();
    size_t n = bed1.GetNameLimit();

    checkId = bed1.GetEntityID();
    EXPECT_EQ(bedID, checkId);
    EXPECT_EQ(spkrCfg, checkCfg);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, checkAudioType);
    EXPECT_EQ(1u, n);
    EXPECT_TRUE(bed1.IsBed());
    EXPECT_FALSE(bed1.IsObject());

    // copy constructor (bed)

    TargetGroup bed2(bed1);

    checkId = bed2.GetEntityID();
    checkCfg = bed2.GetSpeakerConfig();
    checkAudioType = bed2.GetAudioType();
    n = bed2.GetNameLimit();
    EXPECT_EQ(bedID, checkId);
    EXPECT_EQ(spkrCfg, checkCfg);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, checkAudioType);
    EXPECT_EQ(1u, n);
    EXPECT_TRUE(bed2.IsBed());
    EXPECT_FALSE(bed2.IsObject());

    // assignment operator (bed)

    TargetGroup bed3;

    bed3 = bed1;
    checkId = bed3.GetEntityID();
    checkCfg = bed3.GetSpeakerConfig();
    checkAudioType = bed3.GetAudioType();
    n = bed3.GetNameLimit();
    EXPECT_EQ(bedID, checkId);
    EXPECT_EQ(spkrCfg, checkCfg);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, checkAudioType);
    EXPECT_EQ(1u, n);
    EXPECT_TRUE(bed3.IsBed());
    EXPECT_FALSE(bed3.IsObject());

    // constructor (object)

    TargetGroup obj1(objID, audioType, false);

    checkId = obj1.GetEntityID();
    checkCfg = obj1.GetSpeakerConfig();
    checkAudioType = obj1.GetAudioType();
    n = obj1.GetNameLimit();
    EXPECT_EQ(objID, checkId);
    EXPECT_EQ(DLB_ADM_SPEAKER_CONFIG_NONE, checkCfg);
    EXPECT_EQ(audioType, checkAudioType);
    EXPECT_EQ(1u, n);
    EXPECT_FALSE(obj1.IsBed());
    EXPECT_TRUE(obj1.IsObject());

    // copy constructor (object)

    TargetGroup obj2(obj1);

    checkId = obj2.GetEntityID();
    checkCfg = obj2.GetSpeakerConfig();
    checkAudioType = obj2.GetAudioType();
    n = obj2.GetNameLimit();
    EXPECT_EQ(objID, checkId);
    EXPECT_EQ(DLB_ADM_SPEAKER_CONFIG_NONE, checkCfg);
    EXPECT_EQ(audioType, checkAudioType);
    EXPECT_EQ(1u, n);
    EXPECT_FALSE(obj2.IsBed());
    EXPECT_TRUE(obj2.IsObject());

    // assignment operator (object)

    TargetGroup obj3;

    obj3 = obj1;
    checkId = obj3.GetEntityID();
    checkCfg = obj3.GetSpeakerConfig();
    checkAudioType = obj3.GetAudioType();
    n = obj3.GetNameLimit();
    EXPECT_EQ(objID, checkId);
    EXPECT_EQ(DLB_ADM_SPEAKER_CONFIG_NONE, checkCfg);
    EXPECT_EQ(audioType, checkAudioType);
    EXPECT_EQ(1u, n);
    EXPECT_FALSE(obj3.IsBed());
    EXPECT_TRUE(obj3.IsObject());
}

TEST(dlb_adm_test, TargetBasic)
{
    static const char *idString = "AC_00011002";
    static const DLB_ADM_AUDIO_TYPE audioType = DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS;

    AdmIdTranslator translator;
    dlb_adm_entity_id targetID = translator.Translate(idString);
    dlb_adm_entity_id checkId;

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, targetID);

    // constructor

    Target target1(targetID, audioType, "Left");
    DLB_ADM_AUDIO_TYPE at = target1.GetAudioType();
    size_t n = target1.GetNameLimit();

    checkId = target1.GetEntityID();
    EXPECT_EQ(targetID, checkId);
    EXPECT_EQ(audioType, at);
    EXPECT_EQ(1u, n);

    // copy constructor

    Target target2(target1);

    checkId = target2.GetEntityID();
    at = target2.GetAudioType();
    n = target2.GetNameLimit();
    EXPECT_EQ(targetID, checkId);
    EXPECT_EQ(audioType, at);
    EXPECT_EQ(1u, n);

    // assignment operator

    Target target3;

    target3 = target1;
    checkId = target3.GetEntityID();
    at = target3.GetAudioType();
    n = target3.GetNameLimit();
    EXPECT_EQ(targetID, checkId);
    EXPECT_EQ(audioType, at);
    EXPECT_EQ(1u, n);
}

TEST(dlb_adm_test, ElementRecordBasic)
{
    static const char *aoString = "AO_1001";
    static const char *atuString = "ATU_00000001";
    static const char *apString = "AP_00011001";
    static const char *acString = "AC_00011002";

    AdmIdTranslator translator;
    dlb_adm_entity_id audioElementID = translator.Translate(aoString);
    dlb_adm_entity_id audioTrackID = translator.Translate(atuString);
    dlb_adm_entity_id targetGroupID = translator.Translate(apString);
    dlb_adm_entity_id targetID = translator.Translate(acString);
    dlb_adm_entity_id sourceID =
        translator.ConstructGenericId(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, static_cast<uint32_t>(ch1));

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, audioElementID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, audioTrackID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, targetGroupID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, targetID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, sourceID);

    // default constructor

    ElementRecord r0;

    EXPECT_TRUE(r0.IsNull());
    EXPECT_FALSE(r0.Validate());
    EXPECT_TRUE(r0.Validate(true));

    r0.audioElementID = audioElementID;
    EXPECT_FALSE(r0.IsNull());
    EXPECT_FALSE(r0.Validate());
    EXPECT_FALSE(r0.Validate(true));

    // constructor

    ElementRecord r1(audioElementID, targetGroupID, targetID, audioTrackID);

    EXPECT_FALSE(r1.IsNull());
    EXPECT_TRUE(r1.Validate());
    EXPECT_TRUE(r1.Validate(true));

    r1.audioTrackID = targetGroupID;
    EXPECT_FALSE(r1.Validate());
    r1.audioTrackID = audioTrackID;
    EXPECT_TRUE(r1.Validate());

    // copy constructor

    ElementRecord r2(r1);

    EXPECT_EQ(audioElementID, r2.audioElementID);
    EXPECT_EQ(targetGroupID, r2.targetGroupID);
    EXPECT_EQ(targetID, r2.targetID);
    EXPECT_EQ(audioTrackID, r2.audioTrackID);
    EXPECT_FALSE(r2.IsNull());
    EXPECT_TRUE(r2.Validate());
    EXPECT_TRUE(r2.Validate(true));

    // assignment operator

    ElementRecord r3;

    r3 = r1;
    EXPECT_EQ(audioElementID, r3.audioElementID);
    EXPECT_EQ(targetGroupID, r3.targetGroupID);
    EXPECT_EQ(targetID, r3.targetID);
    EXPECT_EQ(audioTrackID, r3.audioTrackID);
    EXPECT_FALSE(r3.IsNull());
    EXPECT_TRUE(r3.Validate());
    EXPECT_TRUE(r3.Validate(true));

    // Clear()

    r3.Clear();
    EXPECT_TRUE(r3.IsNull());
    EXPECT_FALSE(r3.Validate());
    EXPECT_TRUE(r3.Validate(true));
}

TEST(dlb_adm_test, PresentationBasic)
{
    static const char *idString = "APR_1001";

    AdmIdTranslator translator;
    dlb_adm_entity_id presentationID = translator.Translate(idString);
    dlb_adm_entity_id checkId;

    LoudnessMetadata loudness(-0.5, DLB_ADM_LOUDNESS_TYPE_INTEGRATED);

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, presentationID);

    // constructor

    Presentation p1(presentationID, loudness);
    size_t n = p1.GetNameLimit();

    checkId = p1.GetEntityID();
    EXPECT_EQ(presentationID, checkId);
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ(p1.GetLoudnessMetadata().GetLoudnessValue(), loudness.GetLoudnessValue());
    EXPECT_EQ(p1.GetLoudnessMetadata().GetLoudnessType(), loudness.GetLoudnessType());

    // copy constructor

    Presentation p2(p1);

    checkId = p2.GetEntityID();
    n = p2.GetNameLimit();
    EXPECT_EQ(presentationID, checkId);
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ(p2.GetLoudnessMetadata().GetLoudnessValue(), loudness.GetLoudnessValue());
    EXPECT_EQ(p2.GetLoudnessMetadata().GetLoudnessType(), loudness.GetLoudnessType());

    // assignment operator

    Presentation p3;

    p3 = p1;
    checkId = p3.GetEntityID();
    n = p3.GetNameLimit();
    EXPECT_EQ(presentationID, checkId);
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ(p3.GetLoudnessMetadata().GetLoudnessValue(), loudness.GetLoudnessValue());
    EXPECT_EQ(p3.GetLoudnessMetadata().GetLoudnessType(), loudness.GetLoudnessType());
}

TEST(dlb_adm_test, ContentGroupBasic)
{
    static const char *idString = "ACO_1001";
    static const DLB_ADM_CONTENT_KIND contentKind = DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN;

    AdmIdTranslator translator;
    dlb_adm_entity_id groupID = translator.Translate(idString);
    dlb_adm_entity_id checkId;

    const LoudnessMetadata loudness(3.5, DLB_ADM_LOUDNESS_TYPE_INTEGRATED);

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, groupID);

    // constructor

    ContentGroup group1(groupID, contentKind, loudness);
    DLB_ADM_CONTENT_KIND ck = group1.GetContentKind();
    size_t n = group1.GetNameLimit();

    checkId = group1.GetEntityID();
    EXPECT_EQ(groupID, checkId);
    EXPECT_EQ(contentKind, ck);
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ(group1.GetLoudnessMetadata().GetLoudnessValue(), loudness.GetLoudnessValue());
    EXPECT_EQ(group1.GetLoudnessMetadata().GetLoudnessType(), loudness.GetLoudnessType());

    // copy constructor

    ContentGroup group2(group1);

    checkId = group2.GetEntityID();
    ck = group2.GetContentKind();
    n = group2.GetNameLimit();
    EXPECT_EQ(groupID, checkId);
    EXPECT_EQ(contentKind, ck);
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ(group2.GetLoudnessMetadata().GetLoudnessValue(), loudness.GetLoudnessValue());
    EXPECT_EQ(group2.GetLoudnessMetadata().GetLoudnessType(), loudness.GetLoudnessType());

    // assignment operator

    ContentGroup group3;

    group3 = group1;
    checkId = group3.GetEntityID();
    ck = group3.GetContentKind();
    n = group3.GetNameLimit();
    EXPECT_EQ(groupID, checkId);
    EXPECT_EQ(contentKind, ck);
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
    EXPECT_FLOAT_EQ(group3.GetLoudnessMetadata().GetLoudnessValue(), loudness.GetLoudnessValue());
    EXPECT_EQ(group3.GetLoudnessMetadata().GetLoudnessType(), loudness.GetLoudnessType());
}

TEST(dlb_adm_test, ElementGroupBasic)
{
    static const char *idString = "AO_1003";
    static const float gainValue = 1.0f;

    AdmIdTranslator translator;
    dlb_adm_entity_id elementID = translator.Translate(idString);
    dlb_adm_entity_id checkId;

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, elementID);

    // constructor

    ElementGroup element1(elementID, gainValue);
    Gain gain = element1.GetGain();
    size_t n = element1.GetNameLimit();

    checkId = element1.GetEntityID();
    EXPECT_EQ(elementID, checkId);
    EXPECT_EQ(gainValue, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);

    // copy constructor

    ElementGroup element2(element1);

    gain = element2.GetGain();
    checkId = element2.GetEntityID();
    n = element2.GetNameLimit();
    EXPECT_EQ(elementID, checkId);
    EXPECT_EQ(gainValue, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);

    // assignment operator

    ElementGroup element3;

    element3 = element1;
    gain = element3.GetGain();
    checkId = element3.GetEntityID();
    n = element3.GetNameLimit();
    EXPECT_EQ(elementID, checkId);
    EXPECT_EQ(gainValue, gain.GetGainValue());
    EXPECT_EQ(Gain::GAIN_UNIT::LINEAR, gain.GetGainUnit());
    EXPECT_EQ(DEFAULT_NAME_LIMIT, n);
}

TEST(dlb_adm_test, PresentationRecordBasic)
{
    static const char *aprString = "APR_1001";
    static const char *acoString = "ACO_1001";
    static const char *aoString = "AO_1001";
    static const char *avsString = "AVS_1001_0001";

    AdmIdTranslator translator;
    const dlb_adm_entity_id presentationID = translator.Translate(aprString);
    const dlb_adm_entity_id contentGroupID = translator.Translate(acoString);
    const dlb_adm_entity_id elementGroupID = DLB_ADM_NULL_ENTITY_ID;
    const dlb_adm_entity_id audioElementID = translator.Translate(aoString);
    const dlb_adm_entity_id altValSetID = translator.Translate(avsString);

    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, presentationID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroupID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, audioElementID);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, altValSetID);

    // default constructor

    PresentationRecord p0;

    EXPECT_TRUE(p0.IsNull());
    EXPECT_FALSE(p0.Validate());
    EXPECT_TRUE(p0.Validate(true));

    p0.audioElementID = audioElementID;
    EXPECT_FALSE(p0.IsNull());
    EXPECT_FALSE(p0.Validate());
    EXPECT_FALSE(p0.Validate(true));

    // constructor

    PresentationRecord p1(presentationID, contentGroupID, audioElementID, elementGroupID, altValSetID);

    EXPECT_FALSE(p1.IsNull());
    EXPECT_TRUE(p1.Validate());
    EXPECT_TRUE(p1.Validate(true));

    p1.elementGroupID = contentGroupID;
    EXPECT_FALSE(p1.Validate());
    p1.elementGroupID = elementGroupID;
    EXPECT_TRUE(p1.Validate());

    p1.altValueSetID = audioElementID;
    EXPECT_FALSE(p1.Validate());
    p1.altValueSetID = altValSetID;
    EXPECT_TRUE(p1.Validate());
    p1.altValueSetID = DLB_ADM_NULL_ENTITY_ID;
    EXPECT_TRUE(p1.Validate());
    p1.altValueSetID = altValSetID;
    EXPECT_TRUE(p1.Validate());

    // copy constructor

    PresentationRecord p2(p1);

    EXPECT_EQ(presentationID, p2.presentationID);
    EXPECT_EQ(contentGroupID, p2.contentGroupID);
    EXPECT_EQ(elementGroupID, p2.elementGroupID);
    EXPECT_EQ(audioElementID, p2.audioElementID);
    EXPECT_EQ(altValSetID, p2.altValueSetID);
    EXPECT_FALSE(p2.IsNull());
    EXPECT_TRUE(p2.Validate());
    EXPECT_TRUE(p2.Validate(true));

    // assignment operator

    PresentationRecord p3;

    p3 = p1;
    EXPECT_EQ(presentationID, p3.presentationID);
    EXPECT_EQ(contentGroupID, p3.contentGroupID);
    EXPECT_EQ(elementGroupID, p3.elementGroupID);
    EXPECT_EQ(audioElementID, p3.audioElementID);
    EXPECT_EQ(altValSetID, p3.altValueSetID);
    EXPECT_FALSE(p3.IsNull());
    EXPECT_TRUE(p3.Validate());
    EXPECT_TRUE(p3.Validate(true));

    // Clear()

    p3.Clear();
    EXPECT_TRUE(p3.IsNull());
    EXPECT_FALSE(p3.Validate());
    EXPECT_TRUE(p3.Validate(true));

    EXPECT_EQ(p3.altValueSetID, DLB_ADM_NULL_ENTITY_ID);
}

TEST_F(DlbAdm04, ModelEntityContainerBasic)
{
    ASSERT_TRUE(mIDsGood);

    // container insertion
    std::unique_ptr<boost::interprocess::managed_heap_memory> memory = std::unique_ptr<boost::interprocess::managed_heap_memory>(new boost::interprocess::managed_heap_memory(13000));
    ModelEntityContainer *container = memory->construct<ModelEntityContainer>("ModelEntityContainer")(ModelEntityContainer::ctor_args_list(), memory->get_allocator<ModelEntityContainer>());;

    // presentations
    auto inserted = container->insert(ModelEntityRecord(memory->construct<Presentation>("Pres1")(mPres1ID, mPres1Loudness)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Presentation>("Pres2")(mPres2ID, mPres2Loudness)));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(2u, container->size());

    // content groups
    inserted = container->insert(ModelEntityRecord(memory->construct<ContentGroup>("Cont1")(mContent1ID, DLB_ADM_CONTENT_KIND_MK_MIXED, mContent1Loudness)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<ContentGroup>("Cont2")(mContent2ID, DLB_ADM_CONTENT_KIND_DK_DIALOGUE, mContent2Loudness)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<ContentGroup>("Cont3")(mContent3ID, DLB_ADM_CONTENT_KIND_DK_DIALOGUE)));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(5u, container->size());

    // audio elements
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioElement>("Elem1")(mElement1ID)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioElement>("Elem2")(mElement2ID)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioElement>("Elem3")(mElement3ID)));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(8u, container->size());

    // tracks
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioTrack>("Track1")(mTrack1ID)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioTrack>("Track2")(mTrack2ID)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioTrack>("Track3")(mTrack3ID)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AudioTrack>("Track4")(mTrack4ID)));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(12u, container->size());

    // target groups
    inserted = container->insert(ModelEntityRecord(memory->construct<TargetGroup>("TargetGrp1")(mTargetGrp1ID, DLB_ADM_SPEAKER_CONFIG_2_0)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<TargetGroup>("TargetGrp2")(mTargetGrp2ID, DLB_ADM_AUDIO_TYPE_OBJECTS, false)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<TargetGroup>("TargetGrp3")(mTargetGrp3ID, DLB_ADM_AUDIO_TYPE_OBJECTS, false)));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(15u, container->size());

    // targets
    inserted = container->insert(ModelEntityRecord(memory->construct<Target>("Target1")(mTarget1ID, DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, "Left")));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Target>("Target2")(mTarget2ID, DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, "Right")));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Target>("Target3")(mTarget3ID, DLB_ADM_AUDIO_TYPE_OBJECTS, "")));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Target>("Target4")(mTarget4ID, DLB_ADM_AUDIO_TYPE_OBJECTS, "")));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(19u, container->size());

    // sources
    inserted = container->insert(ModelEntityRecord(memory->construct<Source>("Source1")(mSource1ID, ch1)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Source>("Source2")(mSource2ID, ch2)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Source>("Source3")(mSource3ID, ch3)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<Source>("Source4")(mSource4ID, ch4)));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(23u, container->size());

    // alternativeValueSets
    inserted = container->insert(ModelEntityRecord(memory->construct<AlternativeValueSet>("AltValSet1")(mAltValSet1ID)));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AlternativeValueSet>("AltValSet2")(mAltValSet2ID, Position(0.9, true))));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AlternativeValueSet>("AltValSet3")(mAltValSet3ID, boost::none, Gain(-5.1))));
    EXPECT_TRUE(inserted.second);
    inserted = container->insert(ModelEntityRecord(memory->construct<AlternativeValueSet>("AltValSet4")(mAltValSet4ID, Position(-0.25, true), Gain(3.2, Gain::GAIN_UNIT::DECIBELS))));
    EXPECT_TRUE(inserted.second);
    EXPECT_EQ(27u, container->size());

    // queries
    ModelEntityContainer_PKIndex &index = container->get<ModelEntityContainer_PK>();
    ModelEntityContainer_PKIndex::iterator indexEnd = index.end();

    // Ask for non-existent Presentation 3
    ModelEntityContainer_PKIndex::iterator it = index.find(mPres3ID, ModelEntityIdCompare());
    EXPECT_EQ(indexEnd, it);

    // Find Presentation 1
    it = index.find(mPres1ID, ModelEntityIdCompare());
    EXPECT_NE(indexEnd, it);
    EXPECT_EQ(mPres1ID, it->GetReference().GetEntityID());

    // Find all Stream Format records (there should be none)
    auto r = index.equal_range(DLB_ADM_ENTITY_TYPE_STREAM_FORMAT, ModelEntityTypeCompare());
    EXPECT_EQ(r.first, r.second);

    // Find all presentation records (there should be two)
    r = index.equal_range(DLB_ADM_ENTITY_TYPE_PROGRAMME, ModelEntityTypeCompare());
    EXPECT_NE(r.first, r.second);

    size_t n = 0;
    it = r.first;
    while (it != r.second)
    {
        ++it;
        ++n;
    }
    EXPECT_EQ(2u, n);

    // Find Presentation 1 using equal_range and make certain there is only one result
    r = index.equal_range(mPres1ID, ModelEntityIdCompare());
    EXPECT_NE(r.first, r.second);
    n = 0;
    it = r.first;
    while (it != r.second)
    {
        ++it;
        ++n;
    }
    EXPECT_EQ(1u, n);

    memory->destroy<ModelEntityContainer>("ModelEntityContainer");
}

TEST_F(DlbAdm04, CoreModelBasic)
{
    ASSERT_TRUE(mIDsGood);

    CoreModel model;
    bool ok;

    // Add a model entity
    ok = model.AddEntity(Presentation(mPres1ID, mPres1Loudness));
    EXPECT_TRUE(ok);

    // Disallow insertion of duplicates
    ok = model.AddEntity(Presentation(mPres1ID, mPres1Loudness));
    EXPECT_FALSE(ok);

    // Check that it can be retrieved
    const ModelEntity *entityPtr;

    ok = model.GetEntity(mPres1ID, &entityPtr);
    EXPECT_TRUE(ok);
    EXPECT_NE(nullptr, entityPtr);
    EXPECT_EQ(mPres1ID, entityPtr->GetEntityID());

    const Presentation *presPtr = dynamic_cast<const Presentation*>(entityPtr);
    ASSERT_NE(nullptr, presPtr);
    EXPECT_FLOAT_EQ(presPtr->GetLoudnessMetadata().GetLoudnessValue(), mPres1Loudness.loudness_value);
    EXPECT_EQ(presPtr->GetLoudnessMetadata().GetLoudnessType(), mPres1Loudness.loudness_type);

    // Check that we don't find one that isn't there
    ok = model.GetEntity(mPres2ID, &entityPtr);
    EXPECT_FALSE(ok);
    EXPECT_EQ(nullptr, entityPtr);

    // Add some others
    ok = model.AddEntity(Presentation(mPres2ID, mPres2Loudness));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(ContentGroup(mContent1ID, DLB_ADM_CONTENT_KIND_MK_MIXED, mContent1Loudness));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(AudioElement(mElement1ID));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(AudioTrack(mTrack1ID));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(TargetGroup(mTargetGrp2ID, DLB_ADM_AUDIO_TYPE_OBJECTS, false));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(Target(mTarget3ID, DLB_ADM_AUDIO_TYPE_OBJECTS, ""));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(Source(mSource3ID, ch3));
    EXPECT_TRUE(ok);
    ok = model.AddEntity(AlternativeValueSet(mAltValSet1ID));
    EXPECT_TRUE(ok);

    // Add a couple of table records
    ok = model.AddRecord(PresentationRecord(mPres1ID, mContent1ID, mElement1ID, DLB_ADM_NULL_ENTITY_ID, mAltValSet1ID));
    EXPECT_TRUE(ok);
    ok = model.AddRecord(PresentationRecord(mPres2ID, mContent1ID, mElement1ID));
    EXPECT_TRUE(ok);
    ok = model.AddRecord(ElementRecord(mElement1ID, mTargetGrp2ID, mTarget3ID, mTrack1ID));
    EXPECT_TRUE(ok);

    // Disallow insertion of duplicates
    ok = model.AddRecord(PresentationRecord(mPres1ID, mContent1ID, mElement1ID, DLB_ADM_NULL_ENTITY_ID, mAltValSet1ID));
    EXPECT_FALSE(ok);
    ok = model.AddRecord(ElementRecord(mElement1ID, mTargetGrp2ID, mTarget3ID, mTrack1ID));
    EXPECT_FALSE(ok);

    // Try to add invalid records
    ok = model.AddRecord(PresentationRecord(mPres1ID, mContent1ID, DLB_ADM_NULL_ENTITY_ID));
    EXPECT_FALSE(ok);
    ok = model.AddRecord(PresentationRecord(mPres2ID, mContent1ID, mElement1ID));
    EXPECT_FALSE(ok);
    ok = model.AddRecord(ElementRecord(mElement1ID, DLB_ADM_NULL_ENTITY_ID, mTarget3ID, mTrack1ID));
    EXPECT_FALSE(ok);
    ok = model.AddRecord(ElementRecord(mElement2ID, mTargetGrp2ID, mTarget3ID, mTrack1ID));
    EXPECT_FALSE(ok);
}

TEST(dlb_adm_test, CoreModel_Profiles)
{
    CoreModel model;

    EXPECT_FALSE(model.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE));

    model.AddProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_TRUE(model.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE));

    model.Clear();
    EXPECT_FALSE(model.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE));

    model.AddProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_TRUE(model.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE));

}
