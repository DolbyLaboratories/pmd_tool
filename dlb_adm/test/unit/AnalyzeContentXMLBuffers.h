/************************************************************************
 * dlb_adm
 * Copyright (c) 2023-2025, Dolby Laboratories Inc.
 * Copyright (c) 2023-2025, Dolby International AB.
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

static std::string altValSet = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_00000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="a7ac074b-2e21-4f7f-b08b-3c1b59d61959">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="PMD Audio Interface" numIDs="10" numTracks="10">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="3" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="4" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="5" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="6" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="7" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="8" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="9" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000009</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="10" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_0000000A</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="English" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">English</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <audioContentIDRef>ACO_1004</audioContentIDRef>
    </audioProgramme>
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeName="Spanish" audioProgrammeLanguage="spa">
      <audioProgrammeLabel language="spa">Spanish</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <audioContentIDRef>ACO_1005</audioContentIDRef>
      <alternativeValueSetIDRef>AVS_1001_0001</alternativeValueSetIDRef>
    </audioProgramme>
    <audioProgramme audioProgrammeID="APR_1003" audioProgrammeName="Chinese" audioProgrammeLanguage="und">
      <audioProgrammeLabel language="und">Chinese</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <audioContentIDRef>ACO_1006</audioContentIDRef>
      <alternativeValueSetIDRef>AVS_1001_0002</alternativeValueSetIDRef>
    </audioProgramme>
    <audioProgramme audioProgrammeID="APR_1004" audioProgrammeName="English VDS" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">English VDS</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <audioContentIDRef>ACO_1004</audioContentIDRef>
      <audioContentIDRef>ACO_1007</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="English" audioContentLanguage="eng">
      <dialogue mixedContentKind="2">2</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1004" audioContentName="English Dialog" audioContentLanguage="eng">
      <dialogue dialogueContentKind="1">1</dialogue>
      <loudnessMetadata>
        <dialogueLoudness>-23.00</dialogueLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1004</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1005" audioContentName="Spanish Dialog" audioContentLanguage="spa">
      <dialogue dialogueContentKind="1">1</dialogue>
      <loudnessMetadata>
        <dialogueLoudness>-23.00</dialogueLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1005</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1006" audioContentName="Chinese Dialog" audioContentLanguage="und">
      <dialogue dialogueContentKind="1">1</dialogue>
      <loudnessMetadata>
        <dialogueLoudness>-23.00</dialogueLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1006</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1007" audioContentName="English VDS" audioContentLanguage="eng">
      <dialogue dialogueContentKind="4">1</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1007</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="English" interact="0">
      <alternativeValueSet alternativeValueSetID="AVS_1001_0001">
        <gain gainUnit="dB">0.00</gain>
      </alternativeValueSet>
      <alternativeValueSet alternativeValueSetID="AVS_1001_0002">
        <gain gainUnit="dB">0.00</gain>
      </alternativeValueSet>
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1004" audioObjectName="English Dialog" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00031004</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1005" audioObjectName="Spanish Dialog" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00031005</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1006" audioObjectName="Chinese Dialog" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00031006</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000009</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1007" audioObjectName="English VDS" interact="0">
      <gain gainUnit="dB">0.00</gain>
      <audioPackFormatIDRef>AP_00031007</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_0000000A</audioTrackUIDRef>
    </audioObject>
    <audioPackFormat audioPackFormatID="AP_00031004" audioPackFormatName="English Dialog" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031004</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioPackFormat audioPackFormatID="AP_00031005" audioPackFormatName="Spanish Dialog" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031005</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioPackFormat audioPackFormatID="AP_00031006" audioPackFormatName="Chinese Dialog" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031006</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioPackFormat audioPackFormatID="AP_00031007" audioPackFormatName="English VDS" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031007</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031004" audioChannelFormatName="English Dialog" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031004_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
        <gain gainUnit="dB">0.00</gain>
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031005" audioChannelFormatName="Spanish Dialog" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031005_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
        <gain gainUnit="dB">0.00</gain>
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031006" audioChannelFormatName="Chinese Dialog" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031006_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
        <gain gainUnit="dB">0.00</gain>
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031007" audioChannelFormatName="English VDS" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031007_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
        <gain gainUnit="dB">0.00</gain>
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000003">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010004</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000005">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010005</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000006">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010006</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000007">
      <audioPackFormatIDRef>AP_00031004</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00031004</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000008">
      <audioPackFormatIDRef>AP_00031005</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00031005</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000009">
      <audioPackFormatIDRef>AP_00031006</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00031006</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_0000000A">
      <audioPackFormatIDRef>AP_00031007</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00031007</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="AdvSS Emission S-ADM Profile" profileVersion="1.0.0" profileLevel="1">ITU-R BS.[ADM-NGA-EMISSION]-X</profile>
    </profileList>
  </audioFormatExtended>
</frame>
)";

static std::string xml_ME_D_AD = R"(
<?xml version="1.0" encoding="UTF-8"?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat frameFormatID="FF_000000001" type="full" start="00:00:00.00000" duration="00:00:00.01920S48000" timeReference="local" flowID="be8a0b8a-8162-4a6d-882b-4a4797784ed9">
    </frameFormat>
    <transportTrackFormat transportID="TP_0001" transportName="Audio Interface" numIDs="12" numTracks="12">
      <audioTrack trackID="1" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="2" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="3" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="4" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="5" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="6" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="7" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="8" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="9" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_00000009</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="10" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_0000000A</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="11" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_0000000B</audioTrackUIDRef>
      </audioTrack>
      <audioTrack trackID="12" formatLabel="0001" formatDefinition="PCM">
        <audioTrackUIDRef>ATU_0000000C</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1" profileLevel="1">ITU-R BS.2168</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeName="Presentation 1" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">English Commentary</audioProgrammeLabel>
      <audioProgrammeLabel language="fre">Commentaire en anglais</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <audioContentIDRef>ACO_1002</audioContentIDRef>
    </audioProgramme>
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeName="Presentation 2" audioProgrammeLanguage="eng">
      <audioProgrammeLabel language="eng">Radio Commentator</audioProgrammeLabel>
      <audioProgrammeLabel language="fre">Commentateur radio</audioProgrammeLabel>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <audioContentIDRef>ACO_1003</audioContentIDRef>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentName="Bed 1" audioContentLanguage="und">
      <audioContentLabel language="und">Bed 5.1.4</audioContentLabel>
      <dialogue nonDialogueContentKind="3">0</dialogue>
      <loudnessMetadata>
        <integratedLoudness>-23.00</integratedLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1002" audioContentName="Object 1" audioContentLanguage="eng">
      <audioContentLabel language="eng">Dialogue eng</audioContentLabel>
      <dialogue dialogueContentKind="1">1</dialogue>
      <loudnessMetadata>
        <dialogueLoudness>-23.00</dialogueLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
    </audioContent>
    <audioContent audioContentID="ACO_1003" audioContentName="Object 2" audioContentLanguage="eng">
      <audioContentLabel language="eng">RadioCommentator</audioContentLabel>
      <dialogue dialogueContentKind="4">1</dialogue>
      <loudnessMetadata>
        <dialogueLoudness>-23.00</dialogueLoudness>
      </loudnessMetadata>
      <audioObjectIDRef>AO_1003</audioObjectIDRef>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Bed 1" interact="0">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000009</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_0000000A</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 1" interact="0">
      <audioPackFormatIDRef>AP_00031002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_0000000B</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1003" audioObjectName="Object 2" interact="0">
      <audioPackFormatIDRef>AP_00031003</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_0000000C</audioTrackUIDRef>
    </audioObject>
    <audioPackFormat audioPackFormatID="AP_00031002" audioPackFormatName="Object 1" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031002</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioPackFormat audioPackFormatID="AP_00031003" audioPackFormatName="Object 2" typeLabel="0003" typeDefinition="Objects">
      <audioChannelFormatIDRef>AC_00031003</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031002" audioChannelFormatName="Object 1" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031002_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031003" audioChannelFormatName="Object 2" typeLabel="0003" typeDefinition="Objects">
      <audioBlockFormat audioBlockFormatID="AB_00031003_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
        <cartesian>1</cartesian>
        <position coordinate="X">0.00</position>
        <position coordinate="Y">1.00</position>
        <position coordinate="Z">0.00</position>
      </audioBlockFormat>
    </audioChannelFormat>
    <audioTrackUID UID="ATU_00000001">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010801</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010802</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000003">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010803</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010804</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000005">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010805</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000006">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010806</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000007">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_0001080D</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000008">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_0001080F</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000009">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010810</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_0000000A">
      <audioPackFormatIDRef>AP_00010805</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00010812</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_0000000B">
      <audioPackFormatIDRef>AP_00031002</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00031002</audioChannelFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_0000000C">
      <audioPackFormatIDRef>AP_00031003</audioPackFormatIDRef>
      <audioChannelFormatIDRef>AC_00031003</audioChannelFormatIDRef>
    </audioTrackUID>
    <profileList>
      <profile profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1" profileLevel="1">ITU-R BS.2168</profile>
    </profileList>
  </audioFormatExtended>
</frame>
)";
