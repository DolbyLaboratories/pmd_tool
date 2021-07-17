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

#ifndef DLB_ADM_XML_INGESTER_H
#define DLB_ADM_XML_INGESTER_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <boost/core/noncopyable.hpp>
#include <memory>

namespace DlbAdm
{

    class XMLContainer;
    class CoreModel;
    class XMLIngesterData;

    class XMLIngester : public boost::noncopyable
    {
    public:
        XMLIngester(CoreModel &model, XMLContainer &container);
        XMLIngester(CoreModel &model, dlb_adm_xml_container &container);
        ~XMLIngester();

        int Ingest();

    private:
        int AnalyzeContent();

        int GetSpeakerConfig(DLB_ADM_SPEAKER_CONFIG &config, dlb_adm_entity_id id) const;
        int GetObjectClass(DLB_ADM_OBJECT_CLASS &objectClass, dlb_adm_entity_id id) const;

        int IngestFrameFormat();

        int IngestSources();
        int IngestTargets();
        int IngestTargetGroups();
        int IngestAudioTracks();
        int IngestAudioObjects();
        int IngestContentGroups();
        int IngestPresentations();

        int IngestPresentationTable();
        int IngestElementTable();
        int IngestContentTables();
        int IngestSourceTable();
        int IngestUpdateTable();

        CoreModel &mModel;
        XMLContainer &mContainer;
        std::unique_ptr<XMLIngesterData> mData;
    };

}

#endif  // DLB_ADM_XML_INGESTER_H
