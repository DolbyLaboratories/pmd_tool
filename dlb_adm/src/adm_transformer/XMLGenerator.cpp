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

#include "XMLGenerator.h"
#include "XMLConstants.h"
#include "dlb_adm/src/adm_xml/dlb_adm_xml_container.h"
#include "dlb_adm/src/core_model/dlb_adm_core_model.h"

#include "dlb_adm/src/core_model/CoreModel.h"
#include "dlb_adm/src/core_model/FrameFormat.h"
#include "dlb_adm/src/core_model/BlockUpdate.h"
#include "dlb_adm/src/core_model/Source.h"
#include "dlb_adm/src/core_model/SourceGroup.h"
#include "dlb_adm/src/core_model/Target.h"
#include "dlb_adm/src/core_model/TargetGroup.h"
#include "dlb_adm/src/core_model/AudioTrack.h"
#include "dlb_adm/src/core_model/AudioElement.h"
#include "dlb_adm/src/core_model/AlternativeValueSet.h"
#include "dlb_adm/src/core_model/ComplementaryElement.h"
#include "dlb_adm/src/core_model/ElementGroup.h"
#include "dlb_adm/src/core_model/ContentGroup.h"
#include "dlb_adm/src/core_model/Presentation.h"
#include "dlb_adm/src/core_model/SourceRecord.h"
#include "dlb_adm/src/core_model/ElementRecord.h"
#include "dlb_adm/src/core_model/PresentationRecord.h"

#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#define GENERATE_GAIN_FOR_BLOCK_UPDATE_DIRECT_SPEAKERS (0)

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

    XMLGenerator::XMLGenerator(XMLContainer &container, const CoreModel &model)
        : mContainer(container)
        , mModel(model)
    {
        // Empty
    }

    XMLGenerator::XMLGenerator(dlb_adm_xml_container &container, const CoreModel &model)
        : mContainer(container.GetContainer())
        , mModel(model)
    {
        // Empty
    }

    XMLGenerator::XMLGenerator(dlb_adm_xml_container *container, const dlb_adm_core_model *model)
        : mContainer(container->GetContainer())
        , mModel(model->GetCoreModel())
    {
        // Empty
    }

    XMLGenerator::~XMLGenerator()
    {
        // Empty
    }

    int XMLGenerator::GenerateFrame()
    {
        dlb_adm_entity_id topLevelID;
        dlb_adm_entity_id frameID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_FRAME);
        int status;

        status = GenerateTopLevel(topLevelID);
        CHECK_STATUS(status);
        status = mContainer.AddEntity(frameID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(frameID, DLB_ADM_TAG_FRAME_VERSION, convertToAttributeString(S_ADM_SPEC_VERSION));
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(topLevelID, frameID);
        CHECK_STATUS(status);

        status = GenerateAudioFormatExtended(frameID);
        CHECK_STATUS(status);
        status = GenerateFrameHeader(frameID);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateTopLevel(dlb_adm_entity_id &topLevelID)
    {
        dlb_adm_entity_id xmlID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_XML);
        int status;

        topLevelID = mContainer.GetTopLevelID();
        status = mContainer.AddEntity(topLevelID);
        CHECK_STATUS(status);

        status = mContainer.AddEntity(xmlID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(xmlID, DLB_ADM_TAG_XML_VERSION, convertToAttributeString(XML_VERSION));
        CHECK_STATUS(status);
        status = mContainer.SetValue(xmlID, DLB_ADM_TAG_XML_ENCODING, convertToAttributeString(XML_ENCODING));
        CHECK_STATUS(status);

        status = mContainer.AddRelationship(topLevelID, xmlID);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static std::string TranslateGainUnit(Gain::GAIN_UNIT gainUnit) // TODO: possibly this should be more global
    {
        std::string unit("linear");

        if (gainUnit == Gain::GAIN_UNIT::DECIBELS)
        {
            unit = "dB";
        }

        return unit;
    }

    int XMLGenerator::GenerateGain(dlb_adm_entity_id parentID, const Gain &gain)
    {
        dlb_adm_entity_id gainID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_GAIN);
        Gain::GAIN_UNIT gainUnit = gain.GetGainUnit();
        int status;

        status = mContainer.AddEntity(gainID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(parentID, gainID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(gainID, DLB_ADM_TAG_SPEAKER_GAIN_VALUE, AttributeValue(gain.GetGainValue()));
        CHECK_STATUS(status);
        DlbAdm::attributeString gainUnitAtrr;
        status = ParseValue(gainUnitAtrr, TranslateGainUnit(gainUnit));
        CHECK_STATUS(status);
        status = mContainer.SetValue(gainID, DLB_ADM_TAG_SPEAKER_GAIN_UNIT, gainUnitAtrr);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static int Generate1Position(XMLContainer &container, dlb_adm_entity_id blockFormatID, const char *label, dlb_adm_float value)
    {
        dlb_adm_entity_id positionID = container.GetGenericID(DLB_ADM_ENTITY_TYPE_POSITION);
        int status;

        status = container.AddEntity(positionID);
        CHECK_STATUS(status);
        status = container.AddRelationship(blockFormatID, positionID);
        CHECK_STATUS(status);
        status = container.SetValue(positionID, DLB_ADM_TAG_POSITION_VALUE, AttributeValue(value));
        CHECK_STATUS(status);
        status = container.SetValue(positionID, DLB_ADM_TAG_POSITION_COORDINATE, convertToAttributeString(std::string(label)));
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GeneratePosition(dlb_adm_entity_id blockFormatID, const Position &position)
    {
        dlb_adm_bool cartesian = position.IsCartesian();
        dlb_adm_entity_id cartesianID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_CARTESIAN);
        int status;

        // cartesian
        status = mContainer.AddEntity(cartesianID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(blockFormatID, cartesianID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(cartesianID, DLB_ADM_TAG_CARTESIAN_VALUE, AttributeValue(cartesian));
        CHECK_STATUS(status);

        // position
        if (cartesian)
        {
            status = Generate1Position(mContainer, blockFormatID, "X", position.GetCoordinate1());
            CHECK_STATUS(status);
            status = Generate1Position(mContainer, blockFormatID, "Y", position.GetCoordinate2());
            CHECK_STATUS(status);
            status = Generate1Position(mContainer, blockFormatID, "Z", position.GetCoordinate3());
            CHECK_STATUS(status);
        }
        else
        {
            status = Generate1Position(mContainer, blockFormatID, "azimuth", position.GetCoordinate1());
            CHECK_STATUS(status);
            status = Generate1Position(mContainer, blockFormatID, "elevation", position.GetCoordinate2());
            CHECK_STATUS(status);
            status = Generate1Position(mContainer, blockFormatID, "distance", position.GetCoordinate3());
            CHECK_STATUS(status);
        }
        // TODO: screen edge lock

        return status;
    }

    static int Generate1PositionOffset(XMLContainer &container, dlb_adm_entity_id audioElementID, const char *label, dlb_adm_float value)
    {
        /* TODO: finish implementation for positions Y, Z / elevation, distance (if needed). Maybe expand function GeneratePosition? */
	dlb_adm_entity_id positionID = container.GetGenericID(DLB_ADM_ENTITY_TYPE_POSITION_OFFSET);
        int status;

        status = container.AddEntity(positionID);
        CHECK_STATUS(status);
        status = container.AddRelationship(audioElementID, positionID);
        CHECK_STATUS(status);
        status = container.SetValue(positionID, DLB_ADM_TAG_POSITION_OFFSET_VALUE, AttributeValue(value));
        CHECK_STATUS(status);
        status = container.SetValue(positionID, DLB_ADM_TAG_POSITION_OFFSET_COORDINATE, convertToAttributeString(std::string(label)));
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static int GenerateGainRange(XMLContainer &container, dlb_adm_entity_id aoiID, const Gain &gain, std::string bound)
    {
        int status;
        dlb_adm_entity_id gainRangeID = container.GetGenericID(DLB_ADM_ENTITY_TYPE_GAIN_INTERACTION_RANGE);

        status = container.AddEntity(gainRangeID);
        CHECK_STATUS(status);
        status = container.AddRelationship(aoiID, gainRangeID);
        CHECK_STATUS(status);
        status = container.SetValue(gainRangeID, DLB_ADM_TAG_GAIN_INTERACTION_RANGE_BOUND, convertToAttributeString(bound));
        CHECK_STATUS(status);
        status = container.SetValue(gainRangeID, DLB_ADM_TAG_GAIN_INTERACTION_RANGE_UNIT, convertToAttributeString(TranslateGainUnit(gain.GetGainUnit())));
        CHECK_STATUS(status);
        status = container.SetValue(gainRangeID, DLB_ADM_TAG_GAIN_INTERACTION_RANGE_VALUE, AttributeValue(gain.GetGainValue()));
        CHECK_STATUS(status);
        return status;
    }

    static int GeneratePositionRange(XMLContainer &container, dlb_adm_entity_id aoiID, Position::COORDINATE coordinate, float value, std::string bound)
    {
        int status;
        dlb_adm_entity_id positionRangeID = container.GetGenericID(DLB_ADM_ENTITY_TYPE_POSITION_INTERACTION_RANGE);
        std::string coordinateName;

        status = Position::PositionCoordinateToName(coordinate, coordinateName);
        CHECK_STATUS(status);
        status = container.AddEntity(positionRangeID);
        CHECK_STATUS(status);
        status = container.AddRelationship(aoiID, positionRangeID);
        CHECK_STATUS(status);
        status = container.SetValue(positionRangeID, DLB_ADM_TAG_POSITION_INTERACTION_RANGE_BOUND, convertToAttributeString(bound));
        CHECK_STATUS(status);
        status = container.SetValue(positionRangeID, DLB_ADM_TAG_POSITION_INTERACTION_RANGE_COORDINATE, convertToAttributeString(coordinateName));
        CHECK_STATUS(status);
        status = container.SetValue(positionRangeID, DLB_ADM_TAG_POSITION_INTERACTION_RANGE_VALUE, AttributeValue(value));
        CHECK_STATUS(status);
        return status;
    }

    int XMLGenerator::GeneratePositionOffset(dlb_adm_entity_id entityID, const Position &position)
    {
        const char *label = position.IsCartesian() ? "X" : "azimuth";

        return Generate1PositionOffset(mContainer, entityID, label, position.GetCoordinate1());
    }

    int XMLGenerator::GenerateAudioObjectInteraction(dlb_adm_entity_id audioElementID, const AudioObjectInteraction &aoi)
    {
        int status;
        dlb_adm_entity_id aoiID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_OBJECT_INTERACTION);
        dlb_adm_bool gainInteract = aoi.GetGainInteract();
        dlb_adm_bool positionInteract = aoi.GetPositionInteract();
        dlb_adm_bool positionRangesPresent = !aoi.GetMinPositionRange().empty();

        status = mContainer.AddEntity(aoiID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(audioElementID, aoiID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(aoiID, DLB_ADM_TAG_OBJECT_INTERACTION_ON_OFF, AttributeValue(aoi.GetOnOfInteract()));
        CHECK_STATUS(status);
        status = mContainer.SetValue(aoiID, DLB_ADM_TAG_OBJECT_INTERACTION_GAIN, AttributeValue(gainInteract));
        CHECK_STATUS(status);
        status = mContainer.SetValue(aoiID, DLB_ADM_TAG_OBJECT_INTERACTION_POSITION, AttributeValue(positionInteract));
        CHECK_STATUS(status);

        if (gainInteract)
        {
            status = GenerateGainRange(mContainer, aoiID, aoi.GetMinGainRange(), std::string("min"));
            CHECK_STATUS(status);
            status = GenerateGainRange(mContainer, aoiID, aoi.GetMaxGainRange(), std::string("max"));
            CHECK_STATUS(status);
        }

        if (positionRangesPresent)
        {
            for (auto range : aoi.GetMinPositionRange())
            {
                status = GeneratePositionRange(mContainer, aoiID, range.first, range.second, std::string("min"));
                CHECK_STATUS(status);
            }

            for (auto range : aoi.GetMaxPositionRange())
            {
                status = GeneratePositionRange(mContainer, aoiID, range.first, range.second, std::string("max"));
                CHECK_STATUS(status);
            }
        }
        return DLB_ADM_STATUS_OK;
    }

    int XMLGenerator::GenerateLoudnessMetadata(dlb_adm_entity_id parentID, const LoudnessMetadata &loudness)
    {
        int status = DLB_ADM_STATUS_OK;

        if(loudness.IsInitialized())
        {
            dlb_adm_entity_id loundessMetadataID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_LOUDNESS_METADATA);
            DLB_ADM_ENTITY_TYPE loudnessType = DLB_ADM_ENTITY_TYPE_INTEGRATED_LOUDNESS;
            DLB_ADM_TAG loudnessValueTag = DLB_ADM_TAG_INTEGRATED_LOUDNESS_VALUE;

            switch (loudness.GetLoudnessType())
            {
            case DLB_ADM_LOUDNESS_TYPE_INTEGRATED:
                loudnessType = DLB_ADM_ENTITY_TYPE_INTEGRATED_LOUDNESS;
                loudnessValueTag = DLB_ADM_TAG_INTEGRATED_LOUDNESS_VALUE;
                break;
            case DLB_ADM_LOUDNESS_TYPE_DIALOGUE:
                loudnessType = DLB_ADM_ENTITY_TYPE_DIALOGUE_LOUDNESS;
                loudnessValueTag = DLB_ADM_TAG_DIALOGUE_LOUDNESS_VALUE;
                break;
            default:
                status = DLB_ADM_STATUS_ERROR; // unknown Loudness Type
#ifndef NDEBUG
                CHECK_STATUS(status);
#endif
                break;
            }

            dlb_adm_entity_id loundessMetadataTypeID = mContainer.GetGenericID(loudnessType);

            status = mContainer.AddEntity(loundessMetadataID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(parentID, loundessMetadataID);
            CHECK_STATUS(status);
            if (!mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE))
            {
                status = mContainer.SetValue(loundessMetadataID, DLB_ADM_TAG_LOUDNESS_METHOD, convertToAttributeString("ITU-R BS.1770"));
                CHECK_STATUS(status);
            }

            status = mContainer.AddEntity(loundessMetadataTypeID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(loundessMetadataID, loundessMetadataTypeID);
            CHECK_STATUS(status);
            status = mContainer.SetValue(loundessMetadataTypeID, loudnessValueTag, AttributeValue(loudness.GetLoudnessValue()));
            CHECK_STATUS(status);
        }
        return status;
    }

    int XMLGenerator::GenerateSpeakerLabel(dlb_adm_entity_id blockFormatID, dlb_adm_entity_id channelFormatID)
    {
        const Target *target;
        int status;

        status = mModel.GetEntity(channelFormatID, &target);
        CHECK_STATUS(status);
        if (target->GetAudioType() == DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS)
        {
            std::string label = target->GetSpeakerLabel();

            if (!label.empty())
            {
                dlb_adm_entity_id speakerLabelID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_SPEAKER_LABEL);

                status = mContainer.AddEntity(speakerLabelID);
                CHECK_STATUS(status);
                status = mContainer.AddRelationship(blockFormatID, speakerLabelID);
                CHECK_STATUS(status);
                status = mContainer.SetValue(speakerLabelID, DLB_ADM_TAG_SPEAKER_LABEL_VALUE, convertToAttributeString(label));
#ifndef NDEBUG
                CHECK_STATUS(status);
#endif
            }
        }

        return status;
    }

    int XMLGenerator::GenerateDialogue(dlb_adm_entity_id parentID, DLB_ADM_CONTENT_KIND contentKind)
    {
        int status = DLB_ADM_STATUS_OK;

        if (contentKind != DLB_ADM_CONTENT_KIND_UNKNOWN)
        {
            dlb_adm_entity_id dialogueID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_DIALOGUE);
            dlb_adm_uint dialogueValue;
            dlb_adm_uint attributeValue;
            DLB_ADM_TAG attributeTag;

            if (contentKind >= DLB_ADM_CONTENT_KIND_MK)
            {
                dialogueValue = 2;
                attributeValue = static_cast<dlb_adm_uint>(contentKind) - static_cast<dlb_adm_uint>(DLB_ADM_CONTENT_KIND_MK);
                attributeTag = DLB_ADM_TAG_DIALOGUE_MIXED_KIND;
            }
            else if (contentKind >= DLB_ADM_CONTENT_KIND_DK)
            {
                dialogueValue = 1;
                attributeValue = static_cast<dlb_adm_uint>(contentKind) - static_cast<dlb_adm_uint>(DLB_ADM_CONTENT_KIND_DK);
                attributeTag = DLB_ADM_TAG_DIALOGUE_DIALOGUE_KIND;
            }
            else
            {
                dialogueValue = 0;
                attributeValue = static_cast<dlb_adm_uint>(contentKind);
                attributeTag = DLB_ADM_TAG_DIALOGUE_NON_DIALOGUE_KIND;
            }

            status = mContainer.AddEntity(dialogueID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(parentID, dialogueID);
            CHECK_STATUS(status);
            status = mContainer.SetValue(dialogueID, DLB_ADM_TAG_DIALOGUE_VALUE, AttributeValue(dialogueValue));
            CHECK_STATUS(status);
            status = mContainer.SetValue(dialogueID, attributeTag, AttributeValue(attributeValue));
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
        }

        return status;
    }

    int XMLGenerator::GenerateLabels(const ModelEntity *e, DLB_ADM_ENTITY_TYPE labelType, DLB_ADM_TAG nameTag, DLB_ADM_TAG langTag)
    {
        dlb_adm_entity_id parentID = e->GetEntityID();
        size_t firstLabel = e->GetNameCount() - e->GetLabelCount();
        int status = DLB_ADM_STATUS_OK;

        for (size_t i = firstLabel; i < e->GetNameCount(); ++i)
        {
            dlb_adm_entity_id labelID = mContainer.GetGenericID(labelType);
            EntityName entityName;
            std::string name;
            std::string lang;

            status = mContainer.AddEntity(labelID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(parentID, labelID);
            CHECK_STATUS(status);

            if (!e->GetName(entityName, i))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
            CHECK_STATUS(status);
            name = entityName.GetName();
            lang = entityName.GetLanguage();
            status = mContainer.SetValue(labelID, nameTag, convertToAttributeString(name));
            CHECK_STATUS(status);
            if (!lang.empty())
            {
                status = mContainer.SetValue(labelID, langTag, convertToAttributeString(lang));
                CHECK_STATUS(status);
            }
        }

        return status;
    }

    template<typename T>
    int DlbAdm::XMLGenerator::GenerateObject(dlb_adm_entity_id audioFormatExtendedID, const ModelEntity *e)
    {
        const T *object = dynamic_cast<const T *>(e);

        if (object == nullptr)
        {
            return static_cast<int>(DLB_ADM_STATUS_ERROR);
        }

        dlb_adm_entity_id objectID = object->GetEntityID();
        Gain gain = object->GetGain();
        EntityName name;
        int status = DLB_ADM_STATUS_OK;

        status = mContainer.AddEntity(objectID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(audioFormatExtendedID, objectID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(objectID, DLB_ADM_TAG_OBJECT_INTERACT, AttributeValue(OBJECT_INTERACT));
        CHECK_STATUS(status);
        status = GenerateGain(objectID, gain);
        CHECK_STATUS(status);
        status = GenerateLabels(e, DLB_ADM_ENTITY_TYPE_OBJECT_LABEL, DLB_ADM_TAG_OBJECT_LABEL_VALUE, DLB_ADM_TAG_OBJECT_LABEL_LANGUAGE);
        CHECK_STATUS(status);
        if (object->GetName(name, 0))
        {
            status = mContainer.SetValue(objectID, DLB_ADM_TAG_OBJECT_NAME, convertToAttributeString(name.GetName()));
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
        }

        return status;
    }

    template<>
    int DlbAdm::XMLGenerator::GenerateObject<AudioElement>(dlb_adm_entity_id audioFormatExtendedID, const ModelEntity *e)
    {
        const AudioElement *object = dynamic_cast<const AudioElement *>(e);

        if (object == nullptr)
        {
            return static_cast<int>(DLB_ADM_STATUS_ERROR);
        }

        dlb_adm_entity_id objectID = object->GetEntityID();
        Gain gain = object->GetGain();
        EntityName name;
        int status = DLB_ADM_STATUS_OK;

        status = mContainer.AddEntity(objectID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(audioFormatExtendedID, objectID);
        CHECK_STATUS(status);
        if (mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE))
        {
            status = mContainer.SetValue(objectID, DLB_ADM_TAG_OBJECT_INTERACT, object->IsInteractive());
            CHECK_STATUS(status);
            if (object->IsInteractive())
            {
                status = GenerateAudioObjectInteraction(objectID, object->GetInteractionBoundreies());
                CHECK_STATUS(status);
            }
        }
        status = GenerateGain(objectID, gain);
        CHECK_STATUS(status);
        status = GenerateLabels(e, DLB_ADM_ENTITY_TYPE_OBJECT_LABEL, DLB_ADM_TAG_OBJECT_LABEL_VALUE, DLB_ADM_TAG_OBJECT_LABEL_LANGUAGE);
        CHECK_STATUS(status);
        if (object->GetName(name, 0))
        {
            status = mContainer.SetValue(objectID, DLB_ADM_TAG_OBJECT_NAME, convertToAttributeString(name.GetName()));
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
        }

        return status;
    }

    int XMLGenerator::GenerateAltValSets()
    {
        int status;

        CoreModel::EntityCallbackFn altValSetCallback = [&](const ModelEntity *e)
        {
            const AlternativeValueSet *avs = dynamic_cast<const AlternativeValueSet *>(e);

            if (avs == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id avsId = avs->GetEntityID();
            dlb_adm_entity_id objectId = avs->GetParentId();

            status = mContainer.AddEntity(avsId);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(objectId, avsId);
            CHECK_STATUS(status);

            if(avs->HasPositionOffset())
            {
                Position positionOffset;
                status = avs->GetPositionOffset(positionOffset);
                CHECK_STATUS(status);
                status = GeneratePositionOffset(avsId, positionOffset);
                CHECK_STATUS(status);
            }

            if(avs->HasGain())
            {
                Gain gain;
                status = avs->GetGain(gain);
                CHECK_STATUS(status);
                status = GenerateGain(avsId, gain);
                CHECK_STATUS(status);
            }

            status = GenerateLabels(e, DLB_ADM_ENTITY_TYPE_OBJECT_LABEL, DLB_ADM_TAG_OBJECT_LABEL_VALUE, DLB_ADM_TAG_OBJECT_LABEL_LANGUAGE);

#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, altValSetCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static
    int GenerateComplementaryGroupLabels(XMLContainer& container, const ComplementaryElement *e, dlb_adm_entity_id leader_id)
    {
        int status = DLB_ADM_STATUS_OK;

        for (size_t i = 0; i < e->GetLabelCount(); ++i)
        {
            dlb_adm_entity_id labelID = container.GetGenericID(DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_GROUP_LABEL);
            EntityName entityName;
            std::string name;
            std::string lang;

            status = container.AddEntity(labelID);
            CHECK_STATUS(status);
            status = container.AddRelationship(leader_id, labelID);
            CHECK_STATUS(status);

            if (!e->GetName(entityName, i))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
            CHECK_STATUS(status);
            name = entityName.GetName();
            lang = entityName.GetLanguage();
            status = container.SetValue(labelID, DLB_ADM_TAG_COMPLEMENTARY_OBJECT_GROUP_LABEL_VALUE, convertToAttributeString(name));
            CHECK_STATUS(status);
            if (!lang.empty())
            {
                status = container.SetValue(labelID, DLB_ADM_TAG_COMPLEMENTARY_OBJECT_GROUP_LABEL_LANGUAGE, convertToAttributeString(lang));
                CHECK_STATUS(status);
            }
        }

        return status;
    }

    int XMLGenerator::GenerateComplementaryObjects()
    {
        int status;

        CoreModel::EntityCallbackFn ComplementaryObjectCallback = [&](const ModelEntity *e)
        {
            const ComplementaryElement *comp = dynamic_cast<const ComplementaryElement *>(e);

            if (comp == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id compId = comp->GetEntityID();
            dlb_adm_entity_id objectId = comp->GetComplementaryObjectId();

            if (comp->IsComplementaryLeader())
            {
                /*Complementary Leader object (objectId == LeaderId) contains label for audioComplementary ObjectGroupLabel*/
                status = GenerateComplementaryGroupLabels(mContainer
                        ,comp
                        ,comp->GetComplementaryLeaderId());
                CHECK_STATUS(status);
            }
            else
            {
                status = mContainer.AddEntity(compId);
                CHECK_STATUS(status);

                status = mContainer.SetValue(compId, DLB_ADM_TAG_COMPLEMENTARY_OBJECT_ID_REF, AttributeValue(objectId));
                CHECK_STATUS(status);
            }

            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF, ComplementaryObjectCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static bool UpdateHasGain(const BlockUpdate *update)
    {
        bool hasGain = true;

#if GENERATE_GAIN_FOR_BLOCK_UPDATE_DIRECT_SPEAKERS
        // TODO: Figure out whether we're supposed to generate gain for direct speakers audio type,
        // whether conditioned on unity gain or not...  And whether there are conditions for other
        // audio types.
        if (update->GetAudioType() == DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS)
        {
            hasGain = (!update->GetGain().IsUnity());
        }
#else
        (void)update;
#endif
        return hasGain;
    }

    int XMLGenerator::GenerateBlockFormats()
    {
        int status;

        CoreModel::EntityCallbackFn blockFormatCallback = [&](const ModelEntity *e)
        {
            const BlockUpdate *update = dynamic_cast<const BlockUpdate *>(e);

            if (update == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id blockFormatID = update->GetEntityID();
            dlb_adm_entity_id channelFormatID = update->GetParentID();
            Position position = update->GetPosition();
            Gain gain = update->GetGain();
            dlb_adm_time start;
            dlb_adm_time duration;
            bool hasStart = update->GetStart(start);
            bool hasDuration = update->GetDuration(duration);
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(blockFormatID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(channelFormatID, blockFormatID);
            CHECK_STATUS(status);

            if (!hasStart || !hasDuration)
            {
                status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
                                        [&](const ModelEntity *e)
                                        {
                                            const FrameFormat *frameFormat = dynamic_cast<const FrameFormat *>(e);
                                            start = frameFormat->GetStart();
                                            duration = frameFormat->GetDuration();
                                            return status;
                                        }
                                        );
            }
            status = mContainer.SetValue(blockFormatID, DLB_ADM_TAG_BLOCK_FORMAT_LSTART, AttributeValue(start));
            CHECK_STATUS(status);
            status = mContainer.SetValue(blockFormatID, DLB_ADM_TAG_BLOCK_FORMAT_LDURATION, AttributeValue(duration));
            CHECK_STATUS(status);

            if (UpdateHasGain(update))
            {
                if (  !mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE)
                   && (  (gain.GetGainUnit() == Gain::GAIN_UNIT::LINEAR && gain.GetGainValue() != 1)
                      || (gain.GetGainUnit() == Gain::GAIN_UNIT::DECIBELS && gain.GetGainValue() != 0)
                      )
                   )
                {
                    status = GenerateGain(blockFormatID, gain);
                    CHECK_STATUS(status);
                }
            }

            status = GeneratePosition(blockFormatID, position);
            CHECK_STATUS(status);
            status = GenerateSpeakerLabel(blockFormatID, channelFormatID);
            CHECK_STATUS(status);
            if(update->IsCommon())
            {
                status = mContainer.SetIsCommon(blockFormatID);
#ifndef NDEBUG
                CHECK_STATUS(status);
#endif
            }

            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, blockFormatCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateChannelFormats(dlb_adm_entity_id audioFormatExtendedID)
    {
        int status;

        CoreModel::EntityCallbackFn channelFormatCallback = [&](const ModelEntity *e)
        {
            const Target *target = dynamic_cast<const Target *>(e);

            if (target == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id channelFormatID = target->GetEntityID();
            EntityName name;
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(channelFormatID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(audioFormatExtendedID, channelFormatID);
            CHECK_STATUS(status);
            status = mContainer.SetValue(channelFormatID, DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_LABEL, AttributeValue(target->GetAudioType()));
            CHECK_STATUS(status);
            status = mContainer.SetValue(channelFormatID, DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_DEFINITION, convertToAttributeString(target->GetAudioTypeString()));
            CHECK_STATUS(status);
            if (target->GetName(name, 0))
            {
                status = mContainer.SetValue(channelFormatID, DLB_ADM_TAG_CHANNEL_FORMAT_NAME, convertToAttributeString(name.GetName()));
                CHECK_STATUS(status);
            }
            if (target->IsCommon())
            {
                status = mContainer.SetIsCommon(channelFormatID);
                CHECK_STATUS(status);
            }

            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, channelFormatCallback);
        CHECK_STATUS(status);

        status = GenerateBlockFormats();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GeneratePackFormats(dlb_adm_entity_id audioFormatExtendedID)
    {
        int status;

        CoreModel::EntityCallbackFn packFormatCallback = [&](const ModelEntity *e)
        {
            const TargetGroup *targetGroup = dynamic_cast<const TargetGroup *>(e);

            if (targetGroup == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id packFormatID = targetGroup->GetEntityID();
            EntityName name;
            DLB_ADM_AUDIO_TYPE audioType = (targetGroup->IsBed() ? DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS : targetGroup->GetAudioType());
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(packFormatID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(audioFormatExtendedID, packFormatID);
            CHECK_STATUS(status);
            status = mContainer.SetValue(packFormatID, DLB_ADM_TAG_PACK_FORMAT_TYPE_LABEL, AttributeValue(audioType));
            CHECK_STATUS(status);
            status = mContainer.SetValue(packFormatID, DLB_ADM_TAG_PACK_FORMAT_TYPE_DEFINITION, convertToAttributeString(ModelEntity::TranslateAudioType(audioType)));
            CHECK_STATUS(status);
            if (targetGroup->GetName(name, 0))
            {
                status = mContainer.SetValue(packFormatID, DLB_ADM_TAG_PACK_FORMAT_NAME, convertToAttributeString(name.GetName()));
                CHECK_STATUS(status);
            }
            if (targetGroup->IsCommon())
            {
                status = mContainer.SetIsCommon(packFormatID);
                CHECK_STATUS(status);
            }

            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_PACK_FORMAT, packFormatCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateTrackUIDs(dlb_adm_entity_id audioFormatExtendedID)
    {
        int status;

        CoreModel::EntityCallbackFn trackUIDCallback = [&](const ModelEntity *e)
        {
            const AudioTrack *audioTrack = dynamic_cast<const AudioTrack *>(e);

            if (audioTrack == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id trackUIDID = audioTrack->GetEntityID();
            SampleRate rate = audioTrack->GetSampleRate();
            BitDepth depth = audioTrack->GetBitDepth();
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(trackUIDID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(audioFormatExtendedID, trackUIDID);
            CHECK_STATUS(status);
            if (rate != UNKNOWN_SAMPLE_RATE)
            {
                status = mContainer.SetValue(trackUIDID, DLB_ADM_TAG_TRACK_UID_SAMPLE_RATE, AttributeValue(rate));
                CHECK_STATUS(status);
            }
            if (depth != UNKNOWN_BIT_DEPTH)
            {
                status = mContainer.SetValue(trackUIDID, DLB_ADM_TAG_TRACK_UID_BIT_DEPTH, AttributeValue(depth));
                CHECK_STATUS(status);
            }
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_TRACK_UID, trackUIDCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    bool IsAudioElement(const ModelEntity *e)
    {
        const AudioElement *elementGroup = dynamic_cast<const AudioElement *>(e);
        return (elementGroup != nullptr);
    }

    bool IsElementGroup(const ModelEntity *e)
    {
        const ElementGroup *elementGroup = dynamic_cast<const ElementGroup *>(e);
        return (elementGroup != nullptr);
    }

    int XMLGenerator::GenerateObjects(dlb_adm_entity_id audioFormatExtendedID)
    {
        int status;

        // AudioElement
        CoreModel::EntityCallbackFn audioElementCallback = [&](const ModelEntity *e)
        {
            int status = GenerateObject<AudioElement>(audioFormatExtendedID, e);
            CHECK_STATUS(status);

            const AudioElement *element = dynamic_cast<const AudioElement *>(e);

            if (element == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            /* TODO: Move this part inside GenerateObject... */
            if (  mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE)
               && element->GetPositionOffset().GetCoordinate1() != 0.0f)
            {
                status = GeneratePositionOffset(element->GetEntityID(),element->GetPositionOffset());
            }
            /* ...TODO: Move this part inside GenerateObject */
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;

        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_OBJECT, audioElementCallback, IsAudioElement);
        CHECK_STATUS(status);

        // ElementGroup
        CoreModel::EntityCallbackFn elementGroupCallback = [&](const ModelEntity *e)
        {
            return GenerateObject<ElementGroup>(audioFormatExtendedID, e);
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_OBJECT, elementGroupCallback, IsElementGroup);
        CHECK_STATUS(status);

        status = GenerateAltValSets();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif

        status = GenerateComplementaryObjects();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif

        return status;
    }

    static int GetPresentationLanguage(const CoreModel &model, dlb_adm_entity_id contentGroupID, std::string &lang)
    {
        int status = DLB_ADM_STATUS_OK;
        
        CoreModel::PresentationCallbackFn presentationCallback = [&](const PresentationRecord &r)
        {
            if ((r.contentGroupID == contentGroupID) && (r.presentationID != DLB_ADM_NULL_ENTITY_ID))
            {
                const ModelEntity *program;
                const ModelEntity *content;
                EntityName entityName;
                model.GetEntity(r.presentationID, &program);
                model.GetEntity(contentGroupID, &content);
                const ContentGroup *contentGroup = dynamic_cast<const ContentGroup *>(content);
                
                if (DLB_ADM_CONTENT_KIND_NK_MUSIC == contentGroup->GetContentKind() ||
                    DLB_ADM_CONTENT_KIND_NK_EFFECTS == contentGroup->GetContentKind() ||
                    !program->HasName()
                   )
                {
                    lang = std::string("und");
                }
                else
                {
                    if (program->GetName(entityName, 0))
                    {
                        lang = entityName.GetLanguage();
                    }
                }
            }
            return status;
        };
        
        return model.ForEach(presentationCallback);
    }

    int XMLGenerator::GenerateContents(dlb_adm_entity_id audioFormatExtendedID)
    {
        int status;

        CoreModel::EntityCallbackFn contentCallback = [&](const ModelEntity *e)
        {
            const ContentGroup *contentGroup = dynamic_cast<const ContentGroup *>(e);

            if (contentGroup == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id contentGroupID = contentGroup->GetEntityID();
            DLB_ADM_CONTENT_KIND contentKind = contentGroup->GetContentKind();
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(contentGroupID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(audioFormatExtendedID, contentGroupID);
            CHECK_STATUS(status);

            if (  mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE)
               && !contentGroup->GetLoudnessMetadata().IsInitialized())
            {
                LoudnessMetadata loudness = LoudnessMetadata(-23, DLB_ADM_LOUDNESS_TYPE_INTEGRATED);
                if (contentKind == DLB_ADM_CONTENT_KIND_DK_DIALOGUE)
                {
                    loudness = LoudnessMetadata(-23, DLB_ADM_LOUDNESS_TYPE_DIALOGUE);
                }
                /* Emission profile requires Loudness. If it is unavailable generate default */
                status = GenerateLoudnessMetadata(contentGroupID, loudness);
            }
            else
            {
                status = GenerateLoudnessMetadata(contentGroupID, contentGroup->GetLoudnessMetadata());
            }
            CHECK_STATUS(status);

            if (e->HasName())
            {
                EntityName entityName;
                std::string name;
                std::string lang;

                if (!e->GetName(entityName, 0))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
                CHECK_STATUS(status);
                name = entityName.GetName();
                lang = entityName.GetLanguage();
                status = mContainer.SetValue(contentGroupID, DLB_ADM_TAG_CONTENT_NAME, convertToAttributeString(name));
                CHECK_STATUS(status);

                if (lang.empty() && mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE))
                {
                    status = GetPresentationLanguage(mModel, contentGroupID, lang);
                    CHECK_STATUS(status);
                }

                if (!lang.empty())
                {
                    status = mContainer.SetValue(contentGroupID, DLB_ADM_TAG_CONTENT_LANGUAGE, convertToAttributeString(lang));
                    CHECK_STATUS(status);
                }
            }
            status = GenerateLabels(e, DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, DLB_ADM_TAG_CONTENT_LABEL_VALUE, DLB_ADM_TAG_CONTENT_LABEL_LANGUAGE);
            CHECK_STATUS(status);
            status = GenerateDialogue(contentGroupID, contentKind);
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_CONTENT, contentCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateProgrammes(dlb_adm_entity_id audioFormatExtendedID)
    {
        int status;

        CoreModel::EntityCallbackFn programmeCallback = [&](const ModelEntity *e)
        {
            const Presentation *presentation = dynamic_cast<const Presentation *>(e);

            if (presentation == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id presentationID = presentation->GetEntityID();
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(presentationID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(audioFormatExtendedID, presentationID);
            CHECK_STATUS(status);

            if (  mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE)
               && !presentation->GetLoudnessMetadata().IsInitialized())
            {
                /* Emission profile requires Loudness. If it is unavailable generate default */
                status = GenerateLoudnessMetadata(presentationID, LoudnessMetadata(-23, DLB_ADM_LOUDNESS_TYPE_INTEGRATED));
            }
            else
            {
                status = GenerateLoudnessMetadata(presentationID, presentation->GetLoudnessMetadata());
            }
            CHECK_STATUS(status);

            if (e->HasName())
            {
                EntityName entityName;
                std::string name;
                std::string lang;

                if (!e->GetName(entityName, 0))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
                CHECK_STATUS(status);
                name = entityName.GetName();
                lang = entityName.GetLanguage();
                status = mContainer.SetValue(presentationID, DLB_ADM_TAG_PROGRAMME_NAME, convertToAttributeString(name));
                CHECK_STATUS(status);
                if (!lang.empty())
                {
                    status = mContainer.SetValue(presentationID, DLB_ADM_TAG_PROGRAMME_LANGUAGE, convertToAttributeString(lang));
                    CHECK_STATUS(status);
                }
            }
            status = GenerateLabels(e, DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, DLB_ADM_TAG_PROGRAMME_LABEL_VALUE, DLB_ADM_TAG_PROGRAMME_LABEL_LANGUAGE);
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_PROGRAMME, programmeCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateAudioFormatExtended(dlb_adm_entity_id frameID)
    {
        dlb_adm_entity_id audioFormatExtendedID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED);
        int status;

        status = mContainer.AddEntity(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = mContainer.SetValue(audioFormatExtendedID, DLB_ADM_TAG_AUDIO_FORMAT_EXT_VERSION, convertToAttributeString(ADM_SPEC_VERSION));
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(frameID, audioFormatExtendedID);
        CHECK_STATUS(status);

        status = GenerateChannelFormats(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = GeneratePackFormats(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = GenerateTrackUIDs(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = GenerateObjects(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = GenerateContents(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = GenerateProgrammes(audioFormatExtendedID);
        CHECK_STATUS(status);
        status = GenerateProfileList(audioFormatExtendedID);
        CHECK_STATUS(status);

        status = GenerateAudioElementRelationships();
        CHECK_STATUS(status);
        status = GeneratePresentationRelationships();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateTracks()
    {
        int status;

        CoreModel::EntityCallbackFn trackCallback = [&](const ModelEntity *e)
        {
            const Source *source = dynamic_cast<const Source *>(e);

            if (source == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id sourceID = source->GetEntityID();
            dlb_adm_uint channel = static_cast<dlb_adm_uint>(source->GetChannelNumber());
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(sourceID);
            CHECK_STATUS(status);
            status = mContainer.SetValue(sourceID, DLB_ADM_TAG_AUDIO_TRACK_ID, AttributeValue(channel));
            CHECK_STATUS(status);
            if (mModel.HasProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE))
            {
                status = mContainer.SetValue(sourceID, DLB_ADM_TAG_AUDIO_TRACK_FORMAT_LABEL, convertToAttributeString(AUDIO_TRACK_FORMAT_LABEL));
                CHECK_STATUS(status);
                status = mContainer.SetValue( sourceID
                                            , DLB_ADM_TAG_AUDIO_TRACK_FORMAT_DEFINITION
                                            , convertToAttributeString(AUDIO_TRACK_FORMAT_DEFINITION));
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, trackCallback);

        return status;
    }

    int XMLGenerator::GenerateFrameFormat(dlb_adm_entity_id frameHeaderID)
    {
        int status;

        CoreModel::EntityCallbackFn frameFormatCallback = [&](const ModelEntity *e)
        {
            const FrameFormat *frameFormat = dynamic_cast<const FrameFormat *>(e);

            if (frameFormat == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id frameFormatID = frameFormat->GetEntityID();
            std::string type = frameFormat->GetType();
            dlb_adm_time start = frameFormat->GetStart();
            dlb_adm_time duration = frameFormat->GetDuration();
            std::string timeRef = frameFormat->GetTimeReference();
            std::string flowID = frameFormat->GetFlowID();
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddEntity(frameFormatID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(frameHeaderID, frameFormatID);
            CHECK_STATUS(status);
            status = mContainer.SetValue(frameFormatID, DLB_ADM_TAG_FRAME_FORMAT_TYPE, convertToAttributeString(type));
            CHECK_STATUS(status);
            status = mContainer.SetValue(frameFormatID, DLB_ADM_TAG_FRAME_FORMAT_START, AttributeValue(start));
            CHECK_STATUS(status);
            status = mContainer.SetValue(frameFormatID, DLB_ADM_TAG_FRAME_FORMAT_DURATION, AttributeValue(duration));
            CHECK_STATUS(status);
            status = mContainer.SetValue(frameFormatID, DLB_ADM_TAG_FRAME_FORMAT_TIME_REFERENCE, convertToAttributeString(timeRef));
            CHECK_STATUS(status);

            if (!flowID.empty())
            {
                status = mContainer.SetValue(frameFormatID, DLB_ADM_TAG_FRAME_FORMAT_FLOW_ID, convertToAttributeString(flowID));
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, frameFormatCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateTransportTrackFormat(dlb_adm_entity_id frameHeaderID)
    {
        int status;

        CoreModel::EntityCallbackFn transportCallback = [&](const ModelEntity *e)
        {
            const SourceGroup *sourceGroup = dynamic_cast<const SourceGroup *>(e);

            if (sourceGroup == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            dlb_adm_entity_id transportID = sourceGroup->GetEntityID();

            status = mContainer.AddEntity(transportID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(frameHeaderID, transportID);
            CHECK_STATUS(status);
            if (sourceGroup->HasName())
            {
                EntityName name;
                bool got = sourceGroup->GetName(name, 0);

                if (got)
                {
                    status = mContainer.SetValue(transportID, DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NAME, convertToAttributeString(name.GetName()));
                }
                else
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, transportCallback);
        CHECK_STATUS(status);

        status = GenerateTracks();

        return status;
    }

    int XMLGenerator::GenerateFrameHeader(dlb_adm_entity_id frameID)
    {
        dlb_adm_entity_id frameHeaderID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_FRAME_HEADER);
        int status;

        status = mContainer.AddEntity(frameHeaderID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(frameID, frameHeaderID);
        CHECK_STATUS(status);

        status = GenerateProfileList(frameHeaderID);
        CHECK_STATUS(status);
        status = GenerateTransportTrackFormat(frameHeaderID);
        CHECK_STATUS(status);
        status = GenerateSourceRelationships();
        CHECK_STATUS(status);
        status = GenerateFrameFormat(frameHeaderID);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    static int GetProfileDescriptorIndex(const DLB_ADM_PROFILE type, size_t & index)
    {
        int status = DLB_ADM_STATUS_OK;

        for(index = 0; index < SUPPORTED_PROFILES.size(); index++)
        {
            if(SUPPORTED_PROFILES[index].type == type)
            {
                break;
            }
        }

        if(index == SUPPORTED_PROFILES.size())
        {
            status = DLB_ADM_STATUS_ERROR; // not implemented profile type
        }

        return status;
    }

    int XMLGenerator::GenerateProfileList(dlb_adm_entity_id parentID)
    {
        int status = DLB_ADM_STATUS_OK;
        const std::set<DLB_ADM_PROFILE> profiles = mModel.GetProfiles();

        if(profiles.size() == 0)
        {
            return status;
        }

        const dlb_adm_entity_id profileListID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_PROFILE_LIST);
        status = mContainer.AddEntity(profileListID);
        CHECK_STATUS(status);
        status = mContainer.AddRelationship(parentID, profileListID);
        CHECK_STATUS(status);

        for (auto & profile : profiles)
        {
            size_t index;
            status = GetProfileDescriptorIndex(profile, index);
            CHECK_STATUS(status);

            const dlb_adm_entity_id profileID = mContainer.GetGenericID(DLB_ADM_ENTITY_TYPE_PROFILE_LIST_SPECIFICATION);
            status = mContainer.AddEntity(profileID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(profileListID, profileID);
            CHECK_STATUS(status);

            status = mContainer.SetValue(profileID, DLB_ADM_TAG_PROFILE_LIST_SPECIFICATION_NAME,    convertToAttributeString(SUPPORTED_PROFILES[index].name));
            CHECK_STATUS(status);
            status = mContainer.SetValue(profileID, DLB_ADM_TAG_PROFILE_LIST_SPECIFICATION_VERSION, convertToAttributeString(SUPPORTED_PROFILES[index].version));
            CHECK_STATUS(status);
            status = mContainer.SetValue(profileID, DLB_ADM_TAG_PROFILE_LIST_SPECIFICATION_LEVEL,   AttributeValue(SUPPORTED_PROFILES[index].level));
            CHECK_STATUS(status);
            status = mContainer.SetValue(profileID, DLB_ADM_TAG_PROFILE_LIST_SPECIFICATION_VALUE,   convertToAttributeString(SUPPORTED_PROFILES[index].value));
            CHECK_STATUS(status);
        }

        return status;
    }

    int XMLGenerator::SetTransportCounts(dlb_adm_entity_id transportID, dlb_adm_uint numSignals, dlb_adm_uint numTracks)
    {
        int status;

        // "NUM_IDS" is the number of virtual tracks
        status = mContainer.SetValue(transportID, DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_IDS, AttributeValue(numTracks));
        CHECK_STATUS(status);
        // "NUM_TRACKS" is the number of actual tracks, or signals
        status = mContainer.SetValue(transportID, DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_TRACKS, AttributeValue(numSignals));
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLGenerator::GenerateSourceRelationships()
    {
        dlb_adm_entity_id curSourceGroupID = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id curSourceID = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_uint sourceCount = 0;
        dlb_adm_uint trackCount = 0;
        int status;

        CoreModel::SourceCallbackFn sourceCallback = [&](const SourceRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;

            if (r.sourceGroupID != curSourceGroupID)
            {
                if (curSourceGroupID != DLB_ADM_NULL_ENTITY_ID)
                {
                    status = SetTransportCounts(curSourceGroupID, sourceCount, trackCount);
                    CHECK_STATUS(status);
                    sourceCount = 0;
                    trackCount = 0;
                }
                curSourceGroupID = r.sourceGroupID;
            }

            if (r.sourceID != curSourceID)
            {
                status = mContainer.AddRelationship(r.sourceGroupID, r.sourceID);
                CHECK_STATUS(status);
                curSourceID = r.sourceID;
                ++sourceCount;
            }
            ++trackCount;

            status = mContainer.AddRelationship(r.sourceID, r.audioTrackID);
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(sourceCallback);
        CHECK_STATUS(status);

        if (curSourceGroupID != DLB_ADM_NULL_ENTITY_ID)     // Add counts to the last source group
        {
            status = SetTransportCounts(curSourceGroupID, sourceCount, trackCount);
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
        }

        return status;
    }

    int XMLGenerator::GenerateAudioElementRelationships()
    {
        int status;

        CoreModel::ElementCallbackFn elementCallback = [&](const ElementRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;

            status = mContainer.AddRelationship(r.audioElementID, r.audioTrackID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(r.audioElementID, r.targetGroupID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(r.audioTrackID, r.targetGroupID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(r.audioTrackID, r.targetID);
            CHECK_STATUS(status);
            status = mContainer.AddRelationship(r.targetGroupID, r.targetID);
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mModel.ForEach(elementCallback);

        return status;
    }

    int XMLGenerator::GeneratePresentationRelationships()
    {
        int status;

        CoreModel::PresentationCallbackFn presentationCallback = [&](const PresentationRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;

            if (r.presentationID != DLB_ADM_NULL_ENTITY_ID)
            {
                status = mContainer.AddRelationship(r.presentationID, r.contentGroupID);
                CHECK_STATUS(status);
                if(r.altValueSetID != DLB_ADM_NULL_ENTITY_ID)
                {
                    status = mContainer.AddRelationship(r.presentationID, r.altValueSetID);
                    CHECK_STATUS(status);
                }
            }
            if (r.complementaryRefID != DLB_ADM_NULL_ENTITY_ID)
            {
                const ComplementaryElement *compElement;

                status = mModel.GetEntity(r.complementaryRefID, &compElement);

                if (status == DLB_ADM_STATUS_OK)
                {
                    status = mContainer.AddRelationship(compElement->GetComplementaryLeaderId(), r.complementaryRefID);
                    CHECK_STATUS(status);
                }
            }
            if (r.elementGroupID != DLB_ADM_NULL_ENTITY_ID)
            {
                status = mContainer.AddRelationship(r.contentGroupID, r.elementGroupID);
                CHECK_STATUS(status);
                status = mContainer.AddRelationship(r.elementGroupID, r.audioElementID);
                CHECK_STATUS(status);
            }
            else
            {
                status = mContainer.AddRelationship(r.contentGroupID, r.audioElementID);
                CHECK_STATUS(status);
            }

            return status;
        };
        status = mModel.ForEach(presentationCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

}
