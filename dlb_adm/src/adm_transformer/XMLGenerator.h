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

#ifndef DLB_ADM_XML_GENERATOR_H
#define DLB_ADM_XML_GENERATOR_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <boost/core/noncopyable.hpp>
#include <memory>

namespace DlbAdm
{

    class XMLContainer;
    class CoreModel;
    class ModelEntity;
    class Gain;
    class Position;
    class XMLGeneratorData;

    class XMLGenerator : public boost::noncopyable
    {
    public:
        XMLGenerator(XMLContainer &container, const CoreModel &model);
        XMLGenerator(dlb_adm_xml_container &container, const CoreModel &model);
        XMLGenerator(dlb_adm_xml_container *container, const dlb_adm_core_model *model);
        ~XMLGenerator();

        int GenerateFrame();

    private:
        int GenerateTopLevel(dlb_adm_entity_id &topLevelID);

        int GenerateGain(dlb_adm_entity_id parentID, const Gain &gain);
        int GeneratePosition(dlb_adm_entity_id blockFormatID, const Position &position);
        int GenerateSpeakerLabel(dlb_adm_entity_id blockFormatID, dlb_adm_entity_id channelFormatID);
        int GenerateDialogue(dlb_adm_entity_id parentID, DLB_ADM_CONTENT_KIND contentKind);
        int GenerateLabels(const ModelEntity *e, DLB_ADM_ENTITY_TYPE labelType, DLB_ADM_TAG nameTag, DLB_ADM_TAG langTag);

        template <typename T>
        int GenerateObject(dlb_adm_entity_id audioFormatExtendedID, const ModelEntity *e);

        int GenerateBlockFormats();
        int GenerateChannelFormats(dlb_adm_entity_id audioFormatExtendedID);
        int GeneratePackFormats(dlb_adm_entity_id audioFormatExtendedID);
        int GenerateTrackUIDs(dlb_adm_entity_id audioFormatExtendedID);
        int GenerateObjects(dlb_adm_entity_id audioFormatExtendedID);
        int GenerateContents(dlb_adm_entity_id audioFormatExtendedID);
        int GenerateProgrammes(dlb_adm_entity_id audioFormatExtendedID);
        int GenerateAudioFormatExtended(dlb_adm_entity_id frameID);

        int GenerateTracks();
        int GenerateFrameFormat(dlb_adm_entity_id frameHeaderID);
        int GenerateTransportTrackFormat(dlb_adm_entity_id frameHeaderID);
        int GenerateFrameHeader(dlb_adm_entity_id frameID);

        int SetTransportCounts(dlb_adm_entity_id transportID, dlb_adm_uint numSignals, dlb_adm_uint numTracks);

        int GenerateSourceRelationships();
        int GenerateAudioElementRelationships();
        int GeneratePresentationRelationships();

        XMLContainer &mContainer;
        const CoreModel &mModel;
        std::unique_ptr<XMLGeneratorData> mData;
    };

}

#endif  // DLB_ADM_XML_GENERATOR_H
