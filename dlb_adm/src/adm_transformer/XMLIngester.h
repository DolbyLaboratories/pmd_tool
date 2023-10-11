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

        int IngestCommonDefs();

    private:

        int AnalyzeContent();

        int GetSpeakerConfig(DLB_ADM_SPEAKER_CONFIG &config, dlb_adm_entity_id id) const;

        int IngestFrameFormat();
        int IngestProfileList();

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
    };

}

#endif  // DLB_ADM_XML_INGESTER_H
