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

#include "XMLIngester.h"
#include "dlb_adm/src/dlb_adm_api_pvt.h"
#include "dlb_adm/src/adm_xml/dlb_adm_xml_container.h"
#include "dlb_adm/src/adm_xml/EntityRecord.h"
#include "dlb_adm/src/adm_xml/RelationshipRecord.h"
#include "dlb_adm/src/adm_xml/ContentContainer.h"

#include "dlb_adm/src/core_model/CoreModel.h"
#include "dlb_adm/src/core_model/FrameFormat.h"
#include "dlb_adm/src/core_model/BlockUpdate.h"
#include "dlb_adm/src/core_model/Source.h"
#include "dlb_adm/src/core_model/SourceGroup.h"
#include "dlb_adm/src/core_model/Target.h"
#include "dlb_adm/src/core_model/TargetGroup.h"
#include "dlb_adm/src/core_model/AudioTrack.h"
#include "dlb_adm/src/core_model/AudioElement.h"
#include "dlb_adm/src/core_model/ElementGroup.h"
#include "dlb_adm/src/core_model/ContentGroup.h"
#include "dlb_adm/src/core_model/Presentation.h"

#include "dlb_adm/src/core_model/PresentationRecord.h"
#include "dlb_adm/src/core_model/ElementRecord.h"
#include "dlb_adm/src/core_model/SourceRecord.h"
#include "dlb_adm/src/core_model/UpdateRecord.h"

#include "dlb_adm/src/adm_identity/AdmId.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <boost/algorithm/string.hpp>
#include <map>

//#define PRINT_ELEMENT_RECORDS
#ifdef PRINT_ELEMENT_RECORDS
#ifdef _MSC_VER
#define PRIX64 "I64X"
#else
#include <cinttypes>
#endif
#endif

#ifdef NDEBUG
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)
#define CHECK_OPTIONAL(s) if ((s) != DLB_ADM_STATUS_OK && (s) != DLB_ADM_STATUS_NOT_FOUND) return (s)
#else
static int retstat(int s)
{
    return s;   // Put a breakpoint here
}
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return retstat(s)
#define CHECK_OPTIONAL(s) if ((s) != DLB_ADM_STATUS_OK && (s) != DLB_ADM_STATUS_NOT_FOUND) return retstat(s)
#endif

namespace DlbAdm
{

    static const char *coordinateNames[]
    {
        "X",
        "Y",
        "Z",
        "AZIMUTH",
        "ELEVATION",
        "DISTANCE"
    };

    static const Position::COORDINATE coordinateValues[] =
    {
        Position::COORDINATE::X,
        Position::COORDINATE::Y,
        Position::COORDINATE::Z,
        Position::COORDINATE::AZIMUTH,
        Position::COORDINATE::ELEVATION,
        Position::COORDINATE::DISTANCE
    };

    typedef std::map<std::string, Position::COORDINATE> CoordinateMap;
    static CoordinateMap coordinateMap;

    static void InitCoordinateMap()
    {
        if (coordinateMap.empty())
        {
            size_t i = 0;

            for (Position::COORDINATE c : coordinateValues)
            {
                coordinateMap[std::string(coordinateNames[i++])] = c;
            }
        }
    }

    static int GetPositionCoordinate(Position::COORDINATE &c, const std::string &name)
    {
        std::string upName = boost::to_upper_copy(name);
        auto it = coordinateMap.find(upName);
        int status = DLB_ADM_STATUS_NOT_FOUND;

        if (it != coordinateMap.end())
        {
            status = DLB_ADM_STATUS_OK;
            c = it->second;
        }

        return status;
    }

    class XMLIngesterData
    {
    public:
        XMLIngesterData() : mContentContainer()
        {
            InitCoordinateMap();
        }

        ContentContainer &GetContentContainer() { return mContentContainer; }

    private:
        ContentContainer mContentContainer;
    };

    template <class T>
    int GetAttributeValue(T &v, const AttributeValue &av)
    {
        int status = DLB_ADM_STATUS_OK;
        const T *pv = boost::get<T>(&av);

        if (pv == nullptr)
        {
            status = DLB_ADM_STATUS_VALUE_TYPE_MISMATCH;
        }
        else
        {
            v = *pv;
        }

        return status;
    }

    template <class T>
    int CheckRange(T v, T h)
    {
        int status = DLB_ADM_STATUS_OK;

        if (v > h)
        {
            status = DLB_ADM_STATUS_ERROR;
        }

        return status;
    }

    template <class T>
    int CheckRange(T v, T l, T h)
    {
        int status = DLB_ADM_STATUS_OK;

        if (v < l || v > h)
        {
            status = DLB_ADM_STATUS_ERROR;
        }

        return status;
    }

    static int IngestGain(Gain &ingestedGain, XMLContainer &container, dlb_adm_entity_id parentId)
    {
        /*
            The gain element is found on multiple entities:

            <audioObject audioObjectID="AO_1002" audioObjectName="English Dialog">
                <gain gainUnit="dB">0.000000</gain>
                ...
            </audioObject>
    
            <audioBlockFormat audioBlockFormatID="AB_00031002_00000001">
              <gain gainUnit="dB">0.000000</gain>
              ...
            </audioBlockFormat>

            Units for gain may be expressed as dB or linear:

            <gain gainUnit="dB">0.000000</gain>
            <gain gainUnit="linear">1.000000</gain>

         */

        int status = DLB_ADM_STATUS_OK;
        bool gainFound = false;

        RelationshipDB::RelationshipCallbackFn ingestGain = [&](const RelationshipRecord &r)
        {
            Gain::GAIN_UNIT gainUnit = Gain::GAIN_UNIT::LINEAR;
            float gainValue = 1.0f;

            // Make certain there is at most one gain element (TODO: arity checking)
            if (gainFound)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }
            else
            {
                gainFound = true;
            }

            EntityDB::AttributeCallbackFn ingestGainValue = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
            {
                int status = DLB_ADM_STATUS_OK;

                if (tag == DLB_ADM_TAG_SPEAKER_GAIN_VALUE)
                {
                    status = GetAttributeValue(gainValue, value);
                }
                else if (tag == DLB_ADM_TAG_SPEAKER_GAIN_UNIT)
                {
                    std::string unit;

                    status = GetAttributeValue(unit, value);
                    CHECK_STATUS(status);
                    boost::to_upper(unit);
                    if (unit == "DB")
                    {
                        gainUnit = Gain::GAIN_UNIT::DECIBELS;
                    }
                    else if (unit == "LINEAR")
                    {
                        gainUnit = Gain::GAIN_UNIT::LINEAR;
                    }
                    else
                    {
                        status = DLB_ADM_STATUS_ERROR;
                    }
                }

                return status;
            };
            status = container.ForEachAttribute(r.GetToId(), ingestGainValue);
            CHECK_STATUS(status);
            ingestedGain = Gain(gainValue, gainUnit);
            return status;
        };
        status = container.ForEachRelationship(parentId, DLB_ADM_ENTITY_TYPE_GAIN, ingestGain);
        CHECK_STATUS(status);

        return status;
    }

    static int CheckCoordinate(dlb_adm_float &coord, AttributeValue &value, bool &gotIt)
    {
        if (gotIt)
        {
            return DLB_ADM_STATUS_NOT_UNIQUE;
        }
        gotIt = true;

        return GetAttributeValue(coord, value);
    }

    static int IngestPosition(Position &position, dlb_adm_entity_id blockFormatID, XMLContainer &container)
    {
        /*
            Ingest the cartesian and position values for an audioBlockFormat element:

            <audioBlockFormat audioBlockFormatID="AB_00031002_00000001">
              ...
              <cartesian>1</cartesian>
              <position coordinate="X">0.000000</position>
              <position coordinate="Y">1.000000</position>
              <position coordinate="Z">0.000000</position>
            </audioBlockFormat>

         */

        int status = DLB_ADM_STATUS_OK;
        dlb_adm_bool cartesian = DLB_ADM_FALSE;
        dlb_adm_float coord1 = 0.0f;
        dlb_adm_float coord2 = 0.0f;
        dlb_adm_float coord3 = 0.0f;
        bool gotCartesian = false;
        bool got1 = false;
        bool got2 = false;
        bool got3 = false;

        // Cartesian
        RelationshipDB::RelationshipCallbackFn ingestCartesian = [&](const RelationshipRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;

            // Only one "cartesian" element allowed (TODO: arity checking)
            if (gotCartesian)
            {
                status = DLB_ADM_STATUS_ERROR;
            } 
            else
            {
                gotCartesian = true;
            }
            CHECK_STATUS(status);

            dlb_adm_entity_id cartesianID = r.toId;
            AttributeValue value;

            status = container.GetValue(value, cartesianID, DLB_ADM_TAG_CARTESIAN_VALUE);
            CHECK_STATUS(status);
            status = GetAttributeValue(cartesian, value);
            CHECK_STATUS(status);

            return status;
        };
        status = container.ForEachRelationship(blockFormatID, DLB_ADM_ENTITY_TYPE_CARTESIAN, ingestCartesian);
        CHECK_STATUS(status);

        // Position
        RelationshipDB::RelationshipCallbackFn ingestPosition = [&](const RelationshipRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;
            dlb_adm_entity_id positionID = r.toId;
            std::string coordinateName;
            Position::COORDINATE coordinateValue;
            AttributeValue value;

            status = container.GetValue(value, positionID, DLB_ADM_TAG_POSITION_COORDINATE);
            CHECK_STATUS(status);
            status = GetAttributeValue(coordinateName, value);
            CHECK_STATUS(status);
            status = GetPositionCoordinate(coordinateValue, coordinateName);
            CHECK_STATUS(status);
            status = container.GetValue(value, positionID, DLB_ADM_TAG_POSITION_VALUE);
            CHECK_STATUS(status);

            if (cartesian)
            {
                switch (coordinateValue)
                {
                case Position::COORDINATE::X:
                    status = CheckCoordinate(coord1, value, got1);
                    break;
                case Position::COORDINATE::Y:
                    status = CheckCoordinate(coord2, value, got2);
                    break;
                case Position::COORDINATE::Z:
                    status = CheckCoordinate(coord3, value, got3);
                    break;
                default:
                    status = DLB_ADM_STATUS_ERROR;
                    break;
                }
            }
            else
            {
                switch (coordinateValue)
                {
                case Position::COORDINATE::AZIMUTH:
                    status = CheckCoordinate(coord1, value, got1);
                    break;
                case Position::COORDINATE::ELEVATION:
                    status = CheckCoordinate(coord2, value, got2);
                    break;
                case Position::COORDINATE::DISTANCE:
                    status = CheckCoordinate(coord3, value, got3);
                    break;
                default:
                    status = DLB_ADM_STATUS_ERROR;
                    break;
                }
            }
            CHECK_STATUS(status);

            return status;
        };
        status = container.ForEachRelationship(blockFormatID, DLB_ADM_ENTITY_TYPE_POSITION, ingestPosition);
        CHECK_STATUS(status);

        if (!(got1 && got2 && got3))
        {
            status = DLB_ADM_STATUS_ERROR;
        }
        CHECK_STATUS(status);

        position = Position(coord1, coord2, coord3, cartesian ? true : false);

        return status;
    }

    static int IngestName(ModelEntity &entity, XMLContainer &container, DLB_ADM_TAG nameTag, DLB_ADM_TAG langTag)
    {
        /*
            Multiple entities have name and language attributes:

            <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="English" audioProgrammeLanguage="en">...
            <audioContent audioContentID="ACO_1001" audioContentName="Main Stereo Bed" audioContentLanguage="en">...

         */

        dlb_adm_entity_id id = entity.GetEntityID();
        int status = DLB_ADM_STATUS_OK;
        bool gotName = false;
        std::string name;
        std::string lang;

        EntityDB::AttributeCallbackFn ingestName = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
        {
            int status = DLB_ADM_STATUS_OK;

            if (tag == nameTag)
            {
                status = GetAttributeValue(name, value);
                CHECK_STATUS(status);
                gotName = true;
            }
            else if (tag == langTag)
            {
                status = GetAttributeValue(lang, value);
                CHECK_STATUS(status);
            }

            return status;
        };
        status = container.ForEachAttribute(id, ingestName);
        CHECK_STATUS(status);
        if (gotName)
        {
            if (!entity.AddName(name, lang))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
        }
        CHECK_STATUS(status);

        return status;
    }

    template <class T>
    int IngestLabels(T &parent, XMLContainer &container, DLB_ADM_ENTITY_TYPE labelType, DLB_ADM_TAG nameTag, DLB_ADM_TAG langTag)
    {
        /*
            Multiple entities may have label elements:

            <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="English" audioProgrammeLanguage="en">
              <audioProgrammeLabel language="en">English</audioProgrammeLabel>
              <audioProgrammeLabel language="fr">Anglais</audioProgrammeLabel>
              ...
            </audioProgramme>

            <audioContent audioContentID="ACO_1001" audioContentName="Main Stereo Bed">
              <audioContentLabel language="en">Main Stereo Bed</audioContentLabel>
              ...
            </audioContent>

         */

        int status = DLB_ADM_STATUS_OK;
        dlb_adm_entity_id parentId = parent.GetEntityID();

        RelationshipDB::RelationshipCallbackFn ingestLabel = [&](const RelationshipRecord &r)
        {
            int status = DLB_ADM_STATUS_OK;
            std::string name;
            std::string lang;

            EntityDB::AttributeCallbackFn ingestName = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
            {
                int status = DLB_ADM_STATUS_OK;

                if (tag == nameTag)
                {
                    status = GetAttributeValue(name, value);
                    CHECK_STATUS(status);
                }
                else if (tag == langTag)
                {
                    status = GetAttributeValue(lang, value);
                    CHECK_STATUS(status);
                }

                return status;
            };
            status = container.ForEachAttribute(r.GetToId(), ingestName);
            CHECK_STATUS(status);
            if (!parent.AddLabel(name, lang))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
            CHECK_STATUS(status);

            return status;
        };
        status = container.ForEachRelationship(parentId, labelType, ingestLabel);
        CHECK_STATUS(status);

        return status;
    }

    XMLIngester::XMLIngester(CoreModel &model, XMLContainer &container)
        : mModel(model)
        , mContainer(container)
        , mData(new XMLIngesterData())
    {
        // Empty
    }

    XMLIngester::XMLIngester(CoreModel &model, dlb_adm_xml_container &container)
        : mModel(model)
        , mContainer(container.GetContainer())
        , mData(new XMLIngesterData())
    {
        // Empty
    }

    XMLIngester::~XMLIngester()
    {
        // Empty
    }

    int XMLIngester::Ingest()
    {
        int status;

        // Step 1: ingest the model entities

        // Presentations

        status = IngestPresentations();
        CHECK_STATUS(status);
        status = IngestContentGroups();
        CHECK_STATUS(status);
        status = IngestAudioObjects();  // ElementGroup and AudioElement
        CHECK_STATUS(status);

        status = AnalyzeContent();
        CHECK_STATUS(status);

        // Audio elements

        status = IngestAudioTracks();
        CHECK_STATUS(status);
        status = IngestTargetGroups();
        CHECK_STATUS(status);
        status = IngestTargets();
        CHECK_STATUS(status);

        // Sources

        status = IngestSources();
        CHECK_STATUS(status);

        // Frame format

        status = IngestFrameFormat();
        CHECK_STATUS(status);

        // Step 2: build the model tables

        status = IngestContentTables();
        CHECK_STATUS(status);
        status = IngestSourceTable();
        CHECK_STATUS(status);
        status = IngestUpdateTable();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLIngester::AnalyzeContent()
    {
        /*
            We want to link the dialogue content kind values from audioContent to the bed or
            object (audioPackFormat).  The first case is a hop from audioContent to audioObject
            to audioPackFormat:

            <audioContent audioContentID="ACO_1001" audioContentName="Main Stereo Bed">
                <audioObjectIDRef>AO_1001</audioObjectIDRef>
                <dialogue mixedContentKind="2">2</dialogue>
            </audioContent>
            <audioObject audioObjectID="AO_1001" audioObjectName="Main Stereo Bed">
                <gain gainUnit="dB">0.000000</gain>
                <audioPackFormatIDRef>AP_00011001</audioPackFormatIDRef>
                <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
                <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
            </audioObject>
            <audioPackFormat audioPackFormatID="AP_00011001" audioPackFormatName="RoomCentric_2.0" typeLabel="0001" typeDefinition="DirectSpeakers">
                <audioChannelFormatIDRef>AC_00011001</audioChannelFormatIDRef>
                <audioChannelFormatIDRef>AC_00011002</audioChannelFormatIDRef>
            </audioPackFormat>

            The second case is a hop from audioContent to an audioObject that is a grouping of
            other audioObjects (we call this an ElementGroup in the model), to a single audioObject
            in the group, then to audioPackFormat:

            TODO: add XML example

            Note: at present we are ignoring the value of the "dialogue" attribute on audioObject.
            TODO: perhaps we should make certain it is compatible?

         */

        ContentContainer &container = mData->GetContentContainer();
        container.clear();

        CoreModel::EntityCallbackFn contentFn = [&](const ModelEntity *e)
        {
            const ContentGroup *contentGroup = dynamic_cast<const ContentGroup *>(e);

            if (contentGroup == nullptr)
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            RelationshipDB::RelationshipCallbackFn contentObjectFn = [&](const RelationshipRecord &r)
            {
                dlb_adm_entity_id contentID = r.GetFromId();
                dlb_adm_entity_id objectID = r.GetToId();

                RelationshipDB::RelationshipCallbackFn elementFn = [&](const RelationshipRecord &r)
                {
                    dlb_adm_entity_id targetGroupID = r.GetToId();
                    ContentRecord cr(targetGroupID, contentID, contentGroup->GetContentKind());
                    bool inserted = cr.Validate() && container.insert(cr).second;
                    int status = inserted ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR;
                    return status;
                };

                RelationshipDB::RelationshipCallbackFn groupFn = [&](const RelationshipRecord &r)
                {
                    return mContainer.ForEachRelationship(r.GetToId(), DLB_ADM_ENTITY_TYPE_PACK_FORMAT, elementFn);
                };

                return
                    mContainer.ForEachRelationship(objectID, DLB_ADM_ENTITY_TYPE_OBJECT,      groupFn) ||
                    mContainer.ForEachRelationship(objectID, DLB_ADM_ENTITY_TYPE_PACK_FORMAT, elementFn);
            };
            return mContainer.ForEachRelationship(e->GetEntityID(), DLB_ADM_ENTITY_TYPE_OBJECT, contentObjectFn);
        };
        return mModel.ForEach(DLB_ADM_ENTITY_TYPE_CONTENT, contentFn);
    }

    int XMLIngester::GetSpeakerConfig(DLB_ADM_SPEAKER_CONFIG &config, dlb_adm_entity_id id) const
    {
        size_t channelCount = mContainer.RelationshipCount(id, DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT);
        int status = DLB_ADM_STATUS_OK;

        // TODO: confirm config with actual speaker positions

        config = DLB_ADM_SPEAKER_CONFIG_NONE;
        switch (channelCount)
        {
        case 0:
            status = DLB_ADM_STATUS_ERROR;
            break;

        case 1:
            config = DLB_ADM_SPEAKER_CONFIG_MONO;
            break;

        case 2:
            config = DLB_ADM_SPEAKER_CONFIG_2_0;
            break;

        case 3:
            config = DLB_ADM_SPEAKER_CONFIG_3_0;
            break;

        case 6:
            config = DLB_ADM_SPEAKER_CONFIG_5_1;
            break;

        case 8:
            config = DLB_ADM_SPEAKER_CONFIG_5_1_2;
            break;

        case 10:
            config = DLB_ADM_SPEAKER_CONFIG_5_1_4;
            break;

        case 11:
            config = DLB_ADM_SPEAKER_CONFIG_7_0_4;
            break;

        case 12:
            config = DLB_ADM_SPEAKER_CONFIG_7_1_4;
            break;

        case 16:
            config = DLB_ADM_SPEAKER_CONFIG_9_1_6;
            break;

        default:
            config = DLB_ADM_SPEAKER_CONFIG_CUSTOM;
            break;
        }

        return status;
    }

    int XMLIngester::GetObjectClass(DLB_ADM_OBJECT_CLASS &objectClass, dlb_adm_entity_id id) const
    {
        ContentContainer &container = mData->GetContentContainer();
        auto itPair = container.equal_range(id, TargetGroupIdCompare());
        auto it = itPair.first;

        DLB_ADM_OBJECT_CLASS c = DLB_ADM_OBJECT_CLASS_NONE;
        DLB_ADM_CONTENT_KIND contentKind = DLB_ADM_CONTENT_KIND_UNKNOWN;
        int status = DLB_ADM_STATUS_OK;
        bool found = false;

        objectClass = DLB_ADM_OBJECT_CLASS_GENERIC;
        while (it != itPair.second)
        {
            if (found)
            {
                if (it->mContentKind != contentKind)
                {
                    status = DLB_ADM_STATUS_ERROR;
                    break;
                }
            }
            else
            {
                contentKind = it->mContentKind;
                c = dlb_adm_translate_content_kind(contentKind);
                found = true;
            }
            ++it;
        }
        CHECK_STATUS(status);

        if (found)
        {
            objectClass = c;
        }
#if 0
        else
        {
            Dump(container, "ContentContainer.csv");
        }
#endif

        return DLB_ADM_STATUS_OK;
    }

    int XMLIngester::IngestFrameFormat()
    {
        /*
            <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.02000"
                         flowID="12345678-abcd-4000-a000-112233445566">
            </frameFormat>

            Note: for now we are doing only type "full"

            TODO: this breaks for a regular non-Serial ADM file...
         */

        int status = DLB_ADM_STATUS_ERROR;
        bool found = false;

        EntityDB::EntityCallbackFn ingestFrameFormat = [&](const EntityRecord &e)
        {
            int status;

            if (found)
            {
                status = DLB_ADM_STATUS_ERROR;
            }
            else
            {
                dlb_adm_entity_id id = e.id;
                AttributeValue attrValue;
                std::string type;
                std::string start;
                std::string duration;
                std::string flowID;

                found = true;
                status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_FRAME_FORMAT_TYPE);
                CHECK_STATUS(status);
                status = GetAttributeValue(type, attrValue);
                CHECK_STATUS(status);
                status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_FRAME_FORMAT_START);
                CHECK_STATUS(status);
                status = GetAttributeValue(start, attrValue);
                CHECK_STATUS(status);
                status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_FRAME_FORMAT_DURATION);
                CHECK_STATUS(status);
                status = GetAttributeValue(duration, attrValue);
                CHECK_STATUS(status);
                status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_FRAME_FORMAT_FLOW_ID);
                CHECK_OPTIONAL(status);
                if (status == DLB_ADM_STATUS_NOT_FOUND)
                {
                    status = DLB_ADM_STATUS_OK;
                }
                else
                {
                    status = GetAttributeValue(flowID, attrValue);
                    CHECK_STATUS(status);
                }

                FrameFormat frameFormat(id, type, start, duration, flowID);

                if (!mModel.AddEntity(frameFormat))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
#ifndef NDEBUG
                CHECK_STATUS(status);
#endif
            }

            return status;
        };
        status = mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, ingestFrameFormat);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLIngester::IngestSources()
    {
        /*
              <audioTrack trackID="1">
                <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
              </audioTrack>
         */

        EntityDB::EntityCallbackFn ingestAudioTrack = [&](const EntityRecord &e)
        {
            dlb_adm_entity_id id = e.id;
            AttributeValue attrValue;
            dlb_adm_uint channel;
            int status;

            status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_AUDIO_TRACK_ID);
            CHECK_STATUS(status);
            status = GetAttributeValue(channel, attrValue);
            CHECK_STATUS(status);

            if (!mModel.AddEntity(Source(id, static_cast<ChannelNumber>(channel))))
            {
                status = DLB_ADM_STATUS_ERROR;
            }

            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, ingestAudioTrack);
    }

    int XMLIngester::IngestTargets()
    {
        /*
            Bed channel target (note the presence of speakerLabel):

            <audioChannelFormat audioChannelFormatID="AC_00011001" audioChannelFormatName="RoomCentricLeft" typeLabel="0001" typeDefinition="DirectSpeakers">
              <audioBlockFormat audioBlockFormatID="AB_00011001_00000001">
                <speakerLabel>RC_L</speakerLabel>
                <gain gainUnit="dB">0.000000</gain>
                <cartesian>1</cartesian>
                <position coordinate="X">-1.000000</position>
                <position coordinate="Y">1.000000</position>
                <position coordinate="Z">0.000000</position>
              </audioBlockFormat>
            </audioChannelFormat>
            
            Object channel target (note the absence of speakerLabel):

            <audioChannelFormat audioChannelFormatID="AC_00031002" audioChannelFormatName="English Dialog" typeLabel="0003" typeDefinition="Objects">
              <audioBlockFormat audioBlockFormatID="AB_00031002_00000001">
                <gain gainUnit="dB">0.000000</gain>
                <cartesian>1</cartesian>
                <position coordinate="X">0.000000</position>
                <position coordinate="Y">1.000000</position>
                <position coordinate="Z">0.000000</position>
              </audioBlockFormat>
            </audioChannelFormat>
         */

        EntityDB::EntityCallbackFn ingestChannelFormat = [&](const EntityRecord &e)
        {
            dlb_adm_entity_id channelFormatID = e.id;
            AttributeValue attrValue;
            DLB_ADM_AUDIO_TYPE audioType;
            std::string speakerLabel;
            std::string name;
            int status;

            status = mContainer.GetValue(attrValue, channelFormatID, DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_LABEL);
            CHECK_STATUS(status);
            status = GetAttributeValue(audioType, attrValue);
            CHECK_STATUS(status);

            RelationshipDB::RelationshipCallbackFn ingestBlockFormat = [&](const RelationshipRecord &r)
            {
                int status = DLB_ADM_STATUS_OK;
                dlb_adm_entity_id blockFormatId = r.toId;
                Position position;
                Gain gain;

                status = IngestPosition(position, blockFormatId, mContainer);
                CHECK_STATUS(status);
                status = IngestGain(gain, mContainer, blockFormatId);
                CHECK_STATUS(status);

                // TODO: start, duration

                BlockUpdate update(blockFormatId, position, gain);  // TODO: start, duration

                if (!mModel.AddEntity(update))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
                CHECK_STATUS(status);

                if (audioType == DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS)
                {
                    RelationshipDB::RelationshipCallbackFn ingestSpeakerLabel = [&](const RelationshipRecord &r)
                    {
                        int status = DLB_ADM_STATUS_OK;
                        dlb_adm_entity_id speakerLabelID = r.toId;

                        status = mContainer.GetValue(attrValue, speakerLabelID, DLB_ADM_TAG_SPEAKER_LABEL_VALUE);
                        CHECK_STATUS(status);
                        status = GetAttributeValue(speakerLabel, attrValue);
#ifndef NDEBUG
                        CHECK_STATUS(status);
#endif
                        return status;
                    };
                    status = mContainer.ForEachRelationship(blockFormatId, DLB_ADM_ENTITY_TYPE_SPEAKER_LABEL, ingestSpeakerLabel);
                }

                return status;
            };
            status = mContainer.ForEachRelationship(channelFormatID, DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, ingestBlockFormat);
            CHECK_STATUS(status);

            Target target(channelFormatID, audioType, speakerLabel);

            status = mContainer.GetValue(attrValue, channelFormatID, DLB_ADM_TAG_CHANNEL_FORMAT_NAME);
            CHECK_STATUS(status);
            status = GetAttributeValue(name, attrValue);
            CHECK_STATUS(status);
            target.AddName(name.data());
            if (!mModel.AddEntity(target))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, ingestChannelFormat);
    }

    int XMLIngester::IngestTargetGroups()
    {
        /*
            audioPackFormat for a bed:

            <audioPackFormat audioPackFormatID="AP_00011001" audioPackFormatName="RoomCentric_2.0" typeLabel="0001" typeDefinition="DirectSpeakers">
              <audioChannelFormatIDRef>AC_00011001</audioChannelFormatIDRef>
              <audioChannelFormatIDRef>AC_00011002</audioChannelFormatIDRef>
            </audioPackFormat>

            audioPackFormat for an object:

            <audioPackFormat audioPackFormatID="AP_00031002" audioPackFormatName="English Dialog" typeLabel="0003" typeDefinition="Objects">
              <audioChannelFormatIDRef>AC_00031002</audioChannelFormatIDRef>
            </audioPackFormat>
         */

        EntityDB::EntityCallbackFn ingestPackFormat = [&](const EntityRecord &e)
        {
            dlb_adm_entity_id id = e.id;
            AttributeValue attrValue;
            DLB_ADM_AUDIO_TYPE audioType;
            DLB_ADM_SPEAKER_CONFIG speakerConfig;
            DLB_ADM_OBJECT_CLASS objectClass;
            std::string name;
            int status;

            status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_PACK_FORMAT_NAME);
            CHECK_STATUS(status);
            status = GetAttributeValue(name, attrValue);
            CHECK_STATUS(status);

            status = mContainer.GetValue(attrValue, id, DLB_ADM_TAG_PACK_FORMAT_TYPE_LABEL);
            CHECK_STATUS(status);
            status = GetAttributeValue(audioType, attrValue);
            CHECK_STATUS(status);

            switch (audioType)
            {
            case DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS:
            {
                status = GetSpeakerConfig(speakerConfig, id);
                CHECK_STATUS(status);

                TargetGroup targetGroup(id, speakerConfig);

                if (!name.empty())
                {
                    targetGroup.AddName(name.data());
                }
                if (!mModel.AddEntity(targetGroup))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
                break;
            }

            case DLB_ADM_AUDIO_TYPE_OBJECTS:
            {
                status = GetObjectClass(objectClass, id);
                CHECK_STATUS(status);

                bool isDynamic = false;     // TODO: how to set this correctly?
                TargetGroup targetGroup(id, objectClass, isDynamic);
                
                if (!name.empty())
                {
                    targetGroup.AddName(name.data());
                }
                if (!mModel.AddEntity(targetGroup))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
                break;
            }

            default:
                status = DLB_ADM_STATUS_ERROR;
                break;
            }

            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_PACK_FORMAT, ingestPackFormat);
    }

    int XMLIngester::IngestAudioTracks()
    {
        /*
            <audioTrackUID UID="ATU_00000001" sampleRate="48000" bitDepth="24">
              <audioChannelFormatIDRef>AC_00011001</audioChannelFormatIDRef>
              <audioPackFormatIDRef>AP_00011001</audioPackFormatIDRef>
            </audioTrackUID>
         */

        EntityDB::EntityCallbackFn ingestTrackUID = [&](const EntityRecord &e)
        {
            dlb_adm_entity_id id = e.id;
            SampleRate sampleRate = UNKNOWN_SAMPLE_RATE;
            BitDepth bitDepth = UNKNOWN_BIT_DEPTH;
            int status;

            EntityDB::AttributeCallbackFn ingestAttributes = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
            {
                int status = DLB_ADM_STATUS_OK;
                dlb_adm_uint v;

                if (tag == DLB_ADM_TAG_TRACK_UID_SAMPLE_RATE)
                {
                    status = GetAttributeValue(v, value);
                    CHECK_STATUS(status);
                    sampleRate = static_cast<SampleRate>(v);
                } 
                else if (tag == DLB_ADM_TAG_TRACK_UID_BIT_DEPTH)
                {
                    status = GetAttributeValue(v, value);
                    CHECK_STATUS(status);
                    bitDepth = static_cast<BitDepth>(v);
                }

                return status;
            };
            status = mContainer.ForEachAttribute(id, ingestAttributes);
            CHECK_STATUS(status);

            if (!mModel.AddEntity(AudioTrack(id, sampleRate, bitDepth)))
            {
                status = DLB_ADM_STATUS_ERROR;
            }

            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_TRACK_UID, ingestTrackUID);
    }

    int XMLIngester::IngestAudioObjects()
    {
        EntityDB::EntityCallbackFn ingestObject = [&](const EntityRecord &e)
        {
            Gain gain;
            dlb_adm_entity_id id = e.id;
            AttributeValue attributeValue;
            std::string name;
            int status;

            status = IngestGain(gain, mContainer, id);
            CHECK_STATUS(status);
            status = mContainer.GetValue(attributeValue, id, DLB_ADM_TAG_OBJECT_NAME);
            CHECK_STATUS(status);
            status = GetAttributeValue(name, attributeValue);
            CHECK_STATUS(status);
            
            // Is this object:
            // 1) a container for a group of other audio objects (ElementGroup)?
            // 2) a container for a pack format (AudioElement)?
            //
            if (mContainer.RelationshipExists(id, DLB_ADM_ENTITY_TYPE_OBJECT))
            {
                ElementGroup elementGroup(id, gain);

                if (!name.empty())
                {
                    elementGroup.AddName(name.data());
                }
                if (!mModel.AddEntity(elementGroup))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            } 
            else
            {
                AudioElement audioElement(id, gain);

                if (!name.empty())
                {
                    audioElement.AddName(name.data());
                }
                if (!mModel.AddEntity(audioElement))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_OBJECT, ingestObject);
    }

    int XMLIngester::IngestContentGroups()
    {
        /*
            To ingest a content group, we must also find the optional dialogue sub-element
            and interpret its values:

            <audioContent audioContentID="ACO_1001" audioContentName="Main Stereo Bed">
                ...
                <dialogue mixedContentKind="2">2</dialogue>
            </audioContent>

         */

        EntityDB::EntityCallbackFn ingestContent = [&](const EntityRecord &e)
        {
            dlb_adm_entity_id id = e.id;
            DLB_ADM_CONTENT_KIND contentKind = DLB_ADM_CONTENT_KIND_UNKNOWN;
            bool dialogueFound = false;
            int status = DLB_ADM_STATUS_OK;

            RelationshipDB::RelationshipCallbackFn ingestDialogue = [&](const RelationshipRecord &r)
            {
                dlb_adm_uint dialogueValue = 0;
                dlb_adm_uint nonDialogueContentKind = 0;
                dlb_adm_uint dialogueContentKind = 0;
                dlb_adm_uint mixedContentKind = 0;

                bool dialogueValueFound = false;
                bool nonDialogueContentKindFound = false;
                bool dialogueContentKindFound = false;
                bool mixedContentKindFound = false;

                int status;

                if (dialogueFound)
                {
                    return static_cast<int>(DLB_ADM_STATUS_ERROR);
                } 
                else
                {
                    dialogueFound = true;
                }

                EntityDB::AttributeCallbackFn ingestDialogAttribute = [&](dlb_adm_entity_id /*id*/, DLB_ADM_TAG tag, const AttributeValue &value)
                {
                    int status = DLB_ADM_STATUS_OK;

                    switch (tag)
                    {
                    case DLB_ADM_TAG_DIALOGUE_VALUE:
                        dialogueValueFound = true;
                        status = GetAttributeValue(dialogueValue, value);
                        break;

                    case DLB_ADM_TAG_DIALOGUE_NON_DIALOGUE_KIND:
                        nonDialogueContentKindFound = true;
                        status = GetAttributeValue(nonDialogueContentKind, value);
                        break;

                    case DLB_ADM_TAG_DIALOGUE_DIALOGUE_KIND:
                        dialogueContentKindFound = true;
                        status = GetAttributeValue(dialogueContentKind, value);
                        break;

                    case DLB_ADM_TAG_DIALOGUE_MIXED_KIND:
                        mixedContentKindFound = true;
                        status = GetAttributeValue(mixedContentKind, value);
                        break;

                    default:
                        break;
                    }

                    return status;
                };
                status = mContainer.ForEachAttribute(r.GetToId(), ingestDialogAttribute);
                CHECK_STATUS(status);

                // Validation
                size_t n = 0;

                if (nonDialogueContentKindFound)
                {
                    ++n;
                }
                if (dialogueContentKindFound)
                {
                    ++n;
                }
                if (mixedContentKindFound)
                {
                    ++n;
                }
                if (n != 1 || !dialogueValueFound)
                {
                    return static_cast<int>(DLB_ADM_STATUS_ERROR);
                }

                // Interpretation
                switch (dialogueValue)
                {
                case 0:
                    if (nonDialogueContentKindFound)
                    {
                        contentKind = static_cast<DLB_ADM_CONTENT_KIND>(DLB_ADM_CONTENT_KIND_NK + nonDialogueContentKind);
                        status = CheckRange(nonDialogueContentKind, 2u);
                    }
                    else
                    {
                        status = DLB_ADM_STATUS_ERROR;
                    }
                    break;

                case 1:
                    if (dialogueContentKindFound)
                    {
                        contentKind = static_cast<DLB_ADM_CONTENT_KIND>(DLB_ADM_CONTENT_KIND_DK + dialogueContentKind);
                        status = CheckRange(dialogueContentKind, 6u);
                    }
                    else
                    {
                        status = DLB_ADM_STATUS_ERROR;
                    }
                    break;

                case 2:
                    if (mixedContentKindFound)
                    {
                        contentKind = static_cast<DLB_ADM_CONTENT_KIND>(DLB_ADM_CONTENT_KIND_MK + mixedContentKind);
                        status = CheckRange(mixedContentKind, 3u);
                    }
                    else
                    {
                        status = DLB_ADM_STATUS_ERROR;
                    }
                    break;

                default:
                    status = DLB_ADM_STATUS_ERROR;
                    break;
                }

                return status;
            };
            status = mContainer.ForEachRelationship(id, DLB_ADM_ENTITY_TYPE_DIALOGUE, ingestDialogue);
            CHECK_STATUS(status);

            // Create the ContentGroup instance
            ContentGroup cg(id, contentKind);

            // Ingest the name and language attributes
            status = IngestName(cg, mContainer, DLB_ADM_TAG_CONTENT_NAME, DLB_ADM_TAG_CONTENT_LANGUAGE);
            CHECK_STATUS(status);

            // Ingest the content label elements
            status = IngestLabels(cg, mContainer, DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, DLB_ADM_TAG_CONTENT_LABEL_VALUE, DLB_ADM_TAG_CONTENT_LABEL_LANGUAGE);
            CHECK_STATUS(status);

            // Add the ContentGroup to the model
            if (!mModel.AddEntity(cg))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
            CHECK_STATUS(status);

            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_CONTENT, ingestContent);
    }

    int XMLIngester::IngestPresentations()
    {
        /*
            <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="English" audioProgrammeLanguage="en">
              <audioProgrammeLabel language="en">English</audioProgrammeLabel>
              <audioContentIDRef>ACO_1001</audioContentIDRef>
              <audioContentIDRef>ACO_1002</audioContentIDRef>
            </audioProgramme>
         */

        EntityDB::EntityCallbackFn ingestPresentation = [&](const EntityRecord &e)
        {
            int status = DLB_ADM_STATUS_OK;
            Presentation presentation(e.id);

            status = IngestName(presentation, mContainer, DLB_ADM_TAG_PROGRAMME_NAME, DLB_ADM_TAG_PROGRAMME_LANGUAGE);
            CHECK_STATUS(status);
            status = IngestLabels(presentation, mContainer, DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, DLB_ADM_TAG_PROGRAMME_LABEL_VALUE, DLB_ADM_TAG_PROGRAMME_LABEL_LANGUAGE);
            CHECK_STATUS(status);

            if (!mModel.AddEntity(presentation))
            {
                status = DLB_ADM_STATUS_ERROR;
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        return mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_PROGRAMME, ingestPresentation);
    }

    int XMLIngester::IngestPresentationTable()
    {
        // Build the presentation table.  We start with each AudioElement entity and do recursive
        // backward-chaining through ElementGroup, ContentGroup and finally, Presentation entities,
        // keeping track of each entity ID along the way.  When we get to Presentation, we create
        // the PresentationRecord and add it to the table.

        // TODO: would forward-chaining also work?  If so, we might be able to drop representation
        // of inverse relationships altogether.

        dlb_adm_entity_id presentationID = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id contentGroupID = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id elementGroupID = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id audioElementID = DLB_ADM_NULL_ENTITY_ID;
        int status = DLB_ADM_STATUS_OK;

        EntityDB::EntityFilterFn isAudioElement = [&](const EntityRecord &e)
        {
            // If an audioObject instance refers to an audioPackFormat, it is an AudioElement
            // and not an ElementGroup.
            return mContainer.RelationshipExists(e.id, DLB_ADM_ENTITY_TYPE_PACK_FORMAT);
        };

        RelationshipDB::RelationshipFilterFn isElementGroup = [&](const RelationshipRecord &r)
        {
            // If both the 'fromType' and 'toType' of the relatonship are audioObject instances, we
            // have an ElementGroup and not an AudioElement.
            DLB_ADM_ENTITY_TYPE toType = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(r.toId));
            return (toType == DLB_ADM_ENTITY_TYPE_OBJECT);
        };

        EntityDB::EntityCallbackFn audioElementCallback = [&](const EntityRecord &e)
        {
            int status = DLB_ADM_STATUS_OK;

            RelationshipDB::RelationshipCallbackFn contentGroupCallback = [&](const RelationshipRecord &r)
            {
                int status = DLB_ADM_STATUS_OK;
                bool gotPres = false;

                RelationshipDB::RelationshipCallbackFn presentationCallback = [&](const RelationshipRecord &r)
                {
                    int status = DLB_ADM_STATUS_OK;

                    gotPres = true;
                    presentationID = r.toId;
                    PresentationRecord p(presentationID, contentGroupID, audioElementID, elementGroupID);
                    if (!mModel.AddRecord(p))
                    {
                        status = DLB_ADM_STATUS_ERROR;
                    }
                    CHECK_STATUS(status);
                    presentationID = DLB_ADM_NULL_ENTITY_ID;

                    return status;
                };

                contentGroupID = r.toId;
                status = mContainer.ForEachRelationship(contentGroupID, DLB_ADM_ENTITY_TYPE_PROGRAMME, presentationCallback);
                CHECK_STATUS(status);

                if (!gotPres)
                {
                    PresentationRecord p(DLB_ADM_NULL_ENTITY_ID, contentGroupID, audioElementID, elementGroupID);
                    if (!mModel.AddRecord(p))
                    {
                        status = DLB_ADM_STATUS_ERROR;
                    }
                    CHECK_STATUS(status);
                }
                contentGroupID = DLB_ADM_NULL_ENTITY_ID;

                return status;
            };

            RelationshipDB::RelationshipCallbackFn elementGroupCallback = [&](const RelationshipRecord &r)
            {
                int status = DLB_ADM_STATUS_OK;

                elementGroupID = r.toId;
                status = mContainer.ForEachRelationship(elementGroupID, DLB_ADM_ENTITY_TYPE_CONTENT, contentGroupCallback);
                CHECK_STATUS(status);
                elementGroupID = DLB_ADM_NULL_ENTITY_ID;

                return status;
            };

            audioElementID = e.id;
            status = mContainer.ForEachRelationship(audioElementID, DLB_ADM_ENTITY_TYPE_CONTENT, contentGroupCallback);
            CHECK_STATUS(status);
            status = mContainer.ForEachRelationship(audioElementID, ENTITY_RELATIONSHIP::REFERENCED_BY, elementGroupCallback, isElementGroup);
            CHECK_STATUS(status);
            audioElementID = DLB_ADM_NULL_ENTITY_ID;

            return status;
        };
        status = mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_OBJECT, audioElementCallback, isAudioElement);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLIngester::IngestElementTable()
    {
        // Build the audio element table.  We start with each Target and do recursive backward-
        // chaining through TargetGroup, AudioTrack and finally, AudioElement.  When we get to
        // AudioElement, we create the ElementRecord and add it to the table.

        // TODO: would forward-chaining also work?  If so, we might be able to drop representation
        // of inverse relationships altogether.
        
        dlb_adm_entity_id audioElementID = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id audioTrackID   = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id targetGroupID  = DLB_ADM_NULL_ENTITY_ID;
        dlb_adm_entity_id targetID       = DLB_ADM_NULL_ENTITY_ID;
        int status = DLB_ADM_STATUS_OK;

        EntityDB::EntityCallbackFn targetCallback = [&](const EntityRecord &e)
        {
            int status = DLB_ADM_STATUS_OK;

            RelationshipDB::RelationshipCallbackFn targetGroupCallback = [&](const RelationshipRecord &r)
            {
                int status = DLB_ADM_STATUS_OK;

                RelationshipDB::RelationshipCallbackFn audioTrackCallback = [&](const RelationshipRecord &r)
                {
                    int status = DLB_ADM_STATUS_OK;

                    RelationshipDB::RelationshipCallbackFn audioElementCallback = [&](const RelationshipRecord &r)
                    {
                        int status = DLB_ADM_STATUS_OK;

                        audioElementID = r.toId;
                        ElementRecord er(audioElementID, targetGroupID, targetID, audioTrackID);
#ifdef PRINT_ELEMENT_RECORDS
                        printf("%" PRIX64 ",%" PRIX64 ",%" PRIX64 ",%" PRIX64 "\n", audioElementID, targetGroupID, targetID, audioTrackID);
#endif
                        if (!mModel.AddRecord(er))
                        {
                            status = DLB_ADM_STATUS_ERROR;
                        }
                        audioElementID = DLB_ADM_NULL_ENTITY_ID;
#ifndef NDEBUG
                        CHECK_STATUS(status);
#endif
                        return status;
                    };

                    audioTrackID = r.toId;
                    status = mContainer.ForEachRelationship(audioTrackID, DLB_ADM_ENTITY_TYPE_OBJECT, audioElementCallback);
                    CHECK_STATUS(status);
                    audioTrackID = DLB_ADM_NULL_ENTITY_ID;
                
                    return status;
                };

                RelationshipDB::RelationshipFilterFn filterAudioTrack = [&](const RelationshipRecord &r)
                {
                    // The TargetGroup may have several Targets, each with an associated AudioTrack, we only want
                    // the AudioTrack that matches the current Target.
                    return mContainer.RelationshipExists(r.toId, targetID);
                };

                // TODO: test the situation where multiple AudioElements use the same TargetGroup/Targets, but with different
                // AudioTrack entities (which is an intended ADM use case).  Does the backward chaining add exactly the correct
                // rows to the element table?

                targetGroupID = r.toId;
                status = mContainer.ForEachRelationship(targetGroupID, DLB_ADM_ENTITY_TYPE_TRACK_UID, audioTrackCallback, filterAudioTrack);
                CHECK_STATUS(status);
                targetGroupID = DLB_ADM_NULL_ENTITY_ID;

                return status;
            };

            targetID = e.id;
            status = mContainer.ForEachRelationship(targetID, DLB_ADM_ENTITY_TYPE_PACK_FORMAT, targetGroupCallback);
            CHECK_STATUS(status);
            targetID = DLB_ADM_NULL_ENTITY_ID;

            return status;
        };
        status = mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, targetCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLIngester::IngestContentTables()
    {
        int status;

        // TODO: this is a placeholder for re-writing content ingestion;
        // for now, call the old implementations

        status = IngestPresentationTable();
        CHECK_STATUS(status);
        status = IngestElementTable();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLIngester::IngestSourceTable()
    {
        /*
            <transportTrackFormat transportID="TP_0001" transportName="X" numIDs="4" numTracks="4">
              <audioTrack trackID="1">
                <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
              </audioTrack>
              <audioTrack trackID="2">
                <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
              </audioTrack>
              ...
            </transportTrackFormat>

            Note: the audioTrack elements are ingested by IngestSources().
        */

        int status;

        EntityDB::EntityCallbackFn ingestSourceGroup = [&](const EntityRecord &e)
        {
            dlb_adm_entity_id sourceGroupID = e.id;
            DLB_ADM_ENTITY_TYPE entityType;
            uint32_t groupID;
            AttributeValue attrValue;
            std::string name;
            dlb_adm_uint numTracks;
            dlb_adm_uint numIDs;
            int status;

            status = mContainer.GetValue(attrValue, sourceGroupID, DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NAME);
            CHECK_STATUS(status);
            status = GetAttributeValue(name, attrValue);
            CHECK_STATUS(status);
            status = mContainer.GetValue(attrValue, sourceGroupID, DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_TRACKS);
            CHECK_STATUS(status);
            status = GetAttributeValue(numTracks, attrValue);
            CHECK_STATUS(status);
            status = mContainer.GetValue(attrValue, sourceGroupID, DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_IDS);
            CHECK_STATUS(status);
            status = GetAttributeValue(numIDs, attrValue);
            CHECK_STATUS(status);

            AdmIdTranslator().DeconstructUntypedId(sourceGroupID, &entityType, &groupID, nullptr);
            if (entityType == DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT)
            {
                SourceGroup sourceGroup(sourceGroupID, static_cast<SourceGroupID>(groupID), numIDs, numTracks);
                bool added = true;

                if (!name.empty())
                {
                    added = sourceGroup.AddName(name, "");
                }
                if (added)
                {
                    added = mModel.AddEntity(sourceGroup);
                }
                if (added)
                {
                    RelationshipDB::RelationshipCallbackFn handleSource = [&](const RelationshipRecord &sourceRecord)
                    {
                        dlb_adm_entity_id sourceID = sourceRecord.toId;
                        int status;

                        RelationshipDB::RelationshipCallbackFn handleAudioTrack = [&](const RelationshipRecord &trackRecord)
                        {
                            dlb_adm_entity_id audioTrackID = trackRecord.toId;
                            SourceRecord r(sourceGroupID, sourceID, audioTrackID);
                            bool added = mModel.AddRecord(r);
                            return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
                        };
                        status = mContainer.ForEachRelationship(sourceID, DLB_ADM_ENTITY_TYPE_TRACK_UID, handleAudioTrack);
#ifndef NDEBUG
                        CHECK_STATUS(status);
#endif
                        return status;
                    };
                    status = mContainer.ForEachRelationship(sourceGroupID, DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, handleSource);
                }
                else
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            }
            else
            {
                status = DLB_ADM_STATUS_ERROR;
            }
#ifndef NDEBUG
            CHECK_STATUS(status);
#endif
            return status;
        };
        status = mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, ingestSourceGroup);

        return status;
    }

    int XMLIngester::IngestUpdateTable()
    {
        int status = DLB_ADM_STATUS_OK;

        EntityDB::EntityCallbackFn updateCallback = [&](const EntityRecord &e)
        {
            UpdateRecord ur(e.id);
            bool added = mModel.AddRecord(ur);
            int status = (added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
            return status;
        };
        status = mContainer.ForEachEntity(DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, updateCallback);
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

}
