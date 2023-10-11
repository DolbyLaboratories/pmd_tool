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

#ifndef TEST_XML_BUFFER_H
#define TEST_XML_BUFFER_H

#include <string>
#include <iostream>

static std::string object_ref = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="d2e41700-ec49-445e-85a3-17409a2b2174">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="2" numTracks="2">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 1</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Object 1" audioContentLanguage="eng">
      <dialogue dialogueContentKind="1">1</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 1" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00031001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
    </audioObject>
    <audioPackFormat audioPackFormatID="AP_00031001" audioPackFormatName="audioPackFormat_1" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031001</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031001" audioChannelFormatName="audioChannelFormat_1" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031001_00000001" lstart="00:00:00.00000S48000" lduration="00:00:00.00960S48000">
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>)";

static std::string xml_20 = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="d2e41700-ec49-445e-85a3-17409a2b2174">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="2" numTracks="2">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 1</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Bed 1" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Bed 1" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>)";

static std::string object_ref_pack_format_wrong = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="d2e41700-ec49-445e-85a3-17409a2b2174">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="2" numTracks="2">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 1</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Bed 1" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Bed 1" interact="0">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1002" audioObjectName="Bed 1" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>)";

static std::string object_ref_wrong = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="d2e41700-ec49-445e-85a3-17409a2b2174">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="2" numTracks="2">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 1</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Bed 1" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Bed 1" interact="0">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1002" audioObjectName="Bed 1" interact="0">
      <audioObjectIDRef>AO_1003</audioObjectIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1003" audioObjectName="Bed 1" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>)";
#endif /* TEST_XML_BUFFER_H */

static std::string content_wrong = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="d2e41700-ec49-445e-85a3-17409a2b2174">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="2" numTracks="2">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 1</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
    </audioProgramme>
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeName="Presentation 2" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 2</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1002</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Bed 1" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1002" audioContentName="Bed 1" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Bed 1" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>)";


static std::string packformat_wrong = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="d2e41700-ec49-445e-85a3-17409a2b2174">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="2" numTracks="2">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Presentation 1</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Bed 1" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Bed 1" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1002" audioObjectName="Bed 1" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>)";
