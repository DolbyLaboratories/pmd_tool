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
