/************************************************************************
 * dlb_adm
 * Copyright (c) 2025, Dolby Laboratories Inc.
 * Copyright (c) 2025, Dolby International AB.
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

#ifndef DOLBY_E_XML_BUFFER_H
#define DOLBY_E_XML_BUFFER_H

#include <string>
#include <iostream>

static std::string dolbyE_51_20 = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="32644eb7-9bac-48fc-81b2-f9cdfa3fac2f" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="8" numTracks="8" transportID="TP_0001" transportName="X">
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="1">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="2">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="3">
        <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="4">
        <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="5">
        <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="6">
        <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="7">
        <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="8">
        <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission</profile>
      <profile profileName="Super Good Profile" profileVersion="1234.56" profileLevel="over 9000">Unknown Publisher: Document 4682</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission</profile>
      <profile profileName="Super Good Profile" profileVersion="1234.56" profileLevel="over 9000">Unknown Publisher: Document 4682</profile>
    </profileList>
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeLanguage="und" audioProgrammeName="Dolby E Programme-1">
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Dolby E Programme-1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="2">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Dolby E Programme-1" interact="0">
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000003">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
      <audioChannelFormatIDRef>AC_00010004</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000005">
      <audioChannelFormatIDRef>AC_00010005</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000006">
      <audioChannelFormatIDRef>AC_00010006</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010003</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName="Dolby E Programme-2">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Dolby E Programme-2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="2">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Dolby E Programme-2" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000007">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000008">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
  </audioFormatExtended>
  <audioFormatCustom>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1001" audioFormatCustomSetName="DolbyE DBMD Chunk" audioFormatCustomSetType="CUSTOM_SET_TYPE_DOLBYE_DBMD_CHUNK" audioFormatCustomSetVersion="1">
      <dbmd>
          <metadataSegment ID="1">
            <dolbyE ID="0">
              <programConfig>0</programConfig>
              <frameRateCode>3</frameRateCode>
              <smpteTimeCode>01:00:00:00</smpteTimeCode>
            </dolbyE>
          </metadataSegment>
          <metadataSegment ID="3">
            <ac3Program ID="0">
              <programInfo>
                <acMod>7</acMod>
                <bsMod>0</bsMod>
                <lfeOn>1</lfeOn>
              </programInfo>
              <cMixLev>1</cMixLev>
              <surMixLev>0</surMixLev>
              <dSurMod>0</dSurMod>
              <dialNorm>20</dialNorm>
              <copyRightB>0</copyRightB>
              <origBs>0</origBs>
              <langCode exists="0">
                <langCod>0</langCod>
              </langCode>
              <audioProdInfo exists="1">
                <mixLevel>21</mixLevel>
                <roomTyp>1</roomTyp>
              </audioProdInfo>
              <extBsi1e exists="1">
                <loRoCMixLev>5</loRoCMixLev>
                <loRoSurMixLev>4</loRoSurMixLev>
                <ltRtCMixLev>4</ltRtCMixLev>
                <ltRtSurMixLev>4</ltRtSurMixLev>
                <dMixMod>1</dMixMod>
              </extBsi1e>
              <extBsi2e exists="1">
                <dSurExMod>1</dSurExMod>
                <dHeadPhonMod>0</dHeadPhonMod>
                <adConvTyp>0</adConvTyp>
              </extBsi2e>
              <compr1 exists="0">1</compr1>
              <dynRng1 exists="0">2</dynRng1>
              <programDescriptionText>Program 1</programDescriptionText>
            </ac3Program>
            <ac3Program ID="1">
              <programInfo>
                <acMod>2</acMod>
                <bsMod>2</bsMod>
                <lfeOn>0</lfeOn>
              </programInfo>
              <cMixLev>0</cMixLev>
              <surMixLev>1</surMixLev>
              <dSurMod>0</dSurMod>
              <dialNorm>29</dialNorm>
              <copyRightB>1</copyRightB>
              <origBs>1</origBs>
              <langCode exists="1">
                <langCod>15</langCod>
              </langCode>
              <audioProdInfo exists="0">
                <mixLevel>25</mixLevel>
                <roomTyp>0</roomTyp>
              </audioProdInfo>
              <extBsi1e exists="1">
                <loRoCMixLev>4</loRoCMixLev>
                <loRoSurMixLev>5</loRoSurMixLev>
                <ltRtCMixLev>3</ltRtCMixLev>
                <ltRtSurMixLev>5</ltRtSurMixLev>
                <dMixMod>0</dMixMod>
              </extBsi1e>
              <extBsi2e exists="1">
                <dSurExMod>1</dSurExMod>
                <dHeadPhonMod>1</dHeadPhonMod>
                <adConvTyp>1</adConvTyp>
              </extBsi2e>
              <compr1 exists="1">126</compr1>
              <dynRng1 exists="1">44</dynRng1>
            </ac3Program>
          </metadataSegment>
          <metadataSegment ID="11">
            <encodeParameters ID="0">
              <hpFOn>1</hpFOn>
              <bwLpFOn>0</bwLpFOn>
              <lfeLpFOn>1</lfeLpFOn>
              <sur90On>0</sur90On>
              <surAttOn>1</surAttOn>
              <rfPremphOn>0</rfPremphOn>
            </encodeParameters>
            <encodeParameters ID="1">
              <hpFOn>0</hpFOn>
              <bwLpFOn>1</bwLpFOn>
              <lfeLpFOn>0</lfeLpFOn>
              <sur90On>1</sur90On>
              <surAttOn>0</surAttOn>
              <rfPremphOn>0</rfPremphOn>
            </encodeParameters>
          </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>
)";

static std::string dolbyE_51_20_cartesian = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="32644eb7-9bac-48fc-81b2-f9cdfa3fac2f" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="8" numTracks="8" transportID="TP_0001" transportName="X">
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="1">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="2">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="3">
        <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="4">
        <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="5">
        <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="6">
        <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="7">
        <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="8">
        <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission v1.0</profile>
      <profile profileName="Super Good Profile" profileVersion="1234.56" profileLevel="over 9000">Unknown Publisher: Document 4682</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission v1.0</profile>
      <profile profileName="Super Good Profile" profileVersion="1234.56" profileLevel="over 9000">Unknown Publisher: Document 4682</profile>
    </profileList>
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeLanguage="und" audioProgrammeName="Dolby E Programme-1">
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Dolby E Programme-1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="2">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Dolby E Programme-1" interact="0">
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioChannelFormatIDRef>AC_00010801</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioChannelFormatIDRef>AC_00010802</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000003">
      <audioChannelFormatIDRef>AC_00010803</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
      <audioChannelFormatIDRef>AC_00010804</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000005">
      <audioChannelFormatIDRef>AC_00010805</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000006">
      <audioChannelFormatIDRef>AC_00010806</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010803</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName="Dolby E Programme-2">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Dolby E Programme-2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="2">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Dolby E Programme-2" interact="0">
      <audioPackFormatIDRef>AP_00010802</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000007">
      <audioChannelFormatIDRef>AC_00010801</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010802</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000008">
      <audioChannelFormatIDRef>AC_00010802</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010802</audioPackFormatIDRef>
    </audioTrackUID>
  </audioFormatExtended>
  <audioFormatCustom>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1001" audioFormatCustomSetName="DolbyE DBMD Chunk" audioFormatCustomSetType="CUSTOM_SET_TYPE_DOLBYE_DBMD_CHUNK" audioFormatCustomSetVersion="1">
      <dbmd>
          <metadataSegment ID="1">
            <dolbyE ID="0">
              <programConfig>0</programConfig>
              <frameRateCode>3</frameRateCode>
              <smpteTimeCode>01:00:00:00</smpteTimeCode>
            </dolbyE>
          </metadataSegment>
          <metadataSegment ID="3">
            <ac3Program ID="0">
              <programInfo>
                <acMod>7</acMod>
                <bsMod>0</bsMod>
                <lfeOn>1</lfeOn>
              </programInfo>
              <cMixLev>1</cMixLev>
              <surMixLev>0</surMixLev>
              <dSurMod>0</dSurMod>
              <dialNorm>20</dialNorm>
              <copyRightB>0</copyRightB>
              <origBs>0</origBs>
              <langCode exists="0">
                <langCod>0</langCod>
              </langCode>
              <audioProdInfo exists="1">
                <mixLevel>21</mixLevel>
                <roomTyp>1</roomTyp>
              </audioProdInfo>
              <extBsi1e exists="1">
                <loRoCMixLev>5</loRoCMixLev>
                <loRoSurMixLev>4</loRoSurMixLev>
                <ltRtCMixLev>4</ltRtCMixLev>
                <ltRtSurMixLev>4</ltRtSurMixLev>
                <dMixMod>1</dMixMod>
              </extBsi1e>
              <extBsi2e exists="1">
                <dSurExMod>1</dSurExMod>
                <dHeadPhonMod>0</dHeadPhonMod>
                <adConvTyp>0</adConvTyp>
              </extBsi2e>
              <compr1 exists="0">1</compr1>
              <dynRng1 exists="0">2</dynRng1>
              <programDescriptionText>Program 1</programDescriptionText>
            </ac3Program>
            <ac3Program ID="1">
              <programInfo>
                <acMod>2</acMod>
                <bsMod>2</bsMod>
                <lfeOn>0</lfeOn>
              </programInfo>
              <cMixLev>0</cMixLev>
              <surMixLev>1</surMixLev>
              <dSurMod>0</dSurMod>
              <dialNorm>29</dialNorm>
              <copyRightB>1</copyRightB>
              <origBs>1</origBs>
              <langCode exists="1">
                <langCod>15</langCod>
              </langCode>
              <audioProdInfo exists="0">
                <mixLevel>25</mixLevel>
                <roomTyp>0</roomTyp>
              </audioProdInfo>
              <extBsi1e exists="1">
                <loRoCMixLev>4</loRoCMixLev>
                <loRoSurMixLev>5</loRoSurMixLev>
                <ltRtCMixLev>3</ltRtCMixLev>
                <ltRtSurMixLev>5</ltRtSurMixLev>
                <dMixMod>0</dMixMod>
              </extBsi1e>
              <extBsi2e exists="1">
                <dSurExMod>1</dSurExMod>
                <dHeadPhonMod>1</dHeadPhonMod>
                <adConvTyp>1</adConvTyp>
              </extBsi2e>
              <compr1 exists="1">126</compr1>
              <dynRng1 exists="1">44</dynRng1>
              <programDescriptionText>Program 2</programDescriptionText>
            </ac3Program>
          </metadataSegment>
          <metadataSegment ID="11">
            <encodeParameters ID="0">
              <hpFOn>1</hpFOn>
              <bwLpFOn>0</bwLpFOn>
              <lfeLpFOn>1</lfeLpFOn>
              <sur90On>0</sur90On>
              <surAttOn>1</surAttOn>
              <rfPremphOn>0</rfPremphOn>
            </encodeParameters>
            <encodeParameters ID="1">
              <hpFOn>0</hpFOn>
              <bwLpFOn>1</bwLpFOn>
              <lfeLpFOn>0</lfeLpFOn>
              <sur90On>1</sur90On>
              <surAttOn>0</surAttOn>
              <rfPremphOn>0</rfPremphOn>
            </encodeParameters>
          </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>
)";

static std::string dolbyE_51_selected = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="32644eb7-9bac-48fc-81b2-f9cdfa3fac2f" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="8" numTracks="8" transportID="TP_0001" transportName="X">
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="1">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="2">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission</profile>
    </profileList>
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeLanguage="eng" audioProgrammeName="Dolby E Programme-1">
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Dolby E Programme-1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-23</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="4">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Dolby E Programme-1" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
  </audioFormatExtended>
  <audioFormatCustom>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1001" audioFormatCustomSetName="DolbyE DBMD Chunk" audioFormatCustomSetType="CUSTOM_SET_TYPE_DOLBYE_DBMD_CHUNK" audioFormatCustomSetVersion="1">
      <dbmd>
          <metadataSegment ID="1">
            <dolbyE ID="0">
              <programConfig>11</programConfig>
              <frameRateCode>4</frameRateCode>
              <smpteTimeCode>21:20:19:18</smpteTimeCode>
            </dolbyE>
          </metadataSegment>
          <metadataSegment ID="3">
            <ac3Program ID="2">
              <programInfo>
                <acMod>2</acMod>
                <bsMod>2</bsMod>
                <lfeOn>0</lfeOn>
              </programInfo>
              <cMixLev>1</cMixLev>
              <surMixLev>0</surMixLev>
              <dSurMod>0</dSurMod>
              <dialNorm>23</dialNorm>
              <copyRightB>0</copyRightB>
              <origBs>0</origBs>
              <langCode exists="1">
                <langCod>128</langCod>
              </langCode>
              <audioProdInfo exists="1">
                <mixLevel>21</mixLevel>
                <roomTyp>1</roomTyp>
              </audioProdInfo>
              <extBsi1e exists="1">
                <loRoCMixLev>5</loRoCMixLev>
                <loRoSurMixLev>4</loRoSurMixLev>
                <ltRtCMixLev>4</ltRtCMixLev>
                <ltRtSurMixLev>4</ltRtSurMixLev>
                <dMixMod>1</dMixMod>
              </extBsi1e>
              <extBsi2e exists="1">
                <dSurExMod>1</dSurExMod>
                <dHeadPhonMod>0</dHeadPhonMod>
                <adConvTyp>0</adConvTyp>
              </extBsi2e>
              <compr1 exists="0">1</compr1>
              <dynRng1 exists="0">2</dynRng1>
              <programDescriptionText>Program 1</programDescriptionText>
            </ac3Program>
          </metadataSegment>
          <metadataSegment ID="11">
            <encodeParameters ID="2">
              <hpFOn>1</hpFOn>
              <bwLpFOn>1</bwLpFOn>
              <lfeLpFOn>0</lfeLpFOn>
              <sur90On>0</sur90On>
              <surAttOn>1</surAttOn>
              <rfPremphOn>1</rfPremphOn>
            </encodeParameters>
          </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1002" audioFormatCustomSetName="Unknown Extension" audioFormatCustomSetType="UNKNOWN_TYPE" audioFormatCustomSetVersion="unkown.1.alpha-preliminary">
      <unknownMetadata1 unkownAttribute1="1">
        <unknownMetadata2 darkAttribute1="one" darkAttribute2="two">
          <unknown>test</unknown>
        </unknownMetadata2>
      </unknownMetadata1>
      <moreUnknownMetadata unkownAttribute1="1">
        <unknownMetadata2 darkAttribute1="one" darkAttribute2="two">
          <unknown>test</unknown>
        </unknownMetadata2>
      </moreUnknownMetadata>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>
)";

// String splitter due to limit of 16380 in single string(2 strings can still be concatenated)
static std::string dolbyE_8x_10 = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="83ffa106-81f7-45f1-aa13-190e3cf7ec67" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="8" numTracks="8" transportID="TP_0001" transportName="X">
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="1">
        <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="2">
        <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="3">
        <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="4">
        <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="5">
        <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="6">
        <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="7">
        <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
      </audioTrack>
      <audioTrack formatDefinition="PCM" formatLabel="0001" trackID="8">
        <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
      </audioTrack>
    </transportTrackFormat>
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission</profile>
    </profileList>
  </frameHeader>
  <audioFormatExtended version="ITU-R_BS.2076-3">
    <profileList>
      <profile profileLevel="1" profileName="Advanced sound system: ADM and S-ADM profile for emission" profileVersion="1">ITU-R BS.2168</profile>
      <profile profileLevel="1" profileName="Dolby E ADM and S-ADM Profile for emission" profileVersion="1">Dolby E ADM and S-ADM Profile for emission</profile>
    </profileList>
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 1 (Program 1)\">
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-22</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Content 1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-22</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000001">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 2 (Program 2)\">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-23</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Content 2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-23</dialogueLoudness>
      </loudnessMetadata>
      <dialogue nonDialogueContentKind="3">0</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 2" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000002">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1003" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 3 (Program 3)\">
      <audioContentIDRef>ACO_1003</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-26</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1003" audioContentLanguage="und" audioContentName="Content 3">
      <audioObjectIDRef>AO_1003</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-26</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="4">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1003" audioObjectName="Object 3" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000003">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1004" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 4 (Program 4)\">
      <audioContentIDRef>ACO_1004</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-28</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1004" audioContentLanguage="und" audioContentName="Content 4">
      <audioObjectIDRef>AO_1004</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-28</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="3">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1004" audioObjectName="Object 4" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000004">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1005" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 5 (Program 5)\">
      <audioContentIDRef>ACO_1005</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-31</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1005" audioContentLanguage="und" audioContentName="Content 5">
      <audioObjectIDRef>AO_1005</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-31</dialogueLoudness>
      </loudnessMetadata>
      <dialogue dialogueContentKind="1">1</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1005" audioObjectName="Object 5" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000005">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1006" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 6 (Program 6)\">
      <audioContentIDRef>ACO_1006</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>0</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1006" audioContentLanguage="und" audioContentName="Content 6">
      <audioObjectIDRef>AO_1006</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>0</dialogueLoudness>
      </loudnessMetadata>
      <dialogue dialogueContentKind="5">1</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1006" audioObjectName="Object 6" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000006">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1007" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 7 (Program 7)\">
      <audioContentIDRef>ACO_1007</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-1</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1007" audioContentLanguage="und" audioContentName="Content 7">
      <audioObjectIDRef>AO_1007</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-1</dialogueLoudness>
      </loudnessMetadata>
      <dialogue dialogueContentKind="6">1</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1007" audioObjectName="Object 7" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000007</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000007">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1008" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 8 (Program 8)\">
      <audioContentIDRef>ACO_1008</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-10</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1008" audioContentLanguage="und" audioContentName="Content 8">
      <audioObjectIDRef>AO_1008</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-10</dialogueLoudness>
      </loudnessMetadata>
      <dialogue dialogueContentKind="2">1</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1008" audioObjectName="Object 8" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000008">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>
  </audioFormatExtended>
  <audioFormatCustom>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1001" audioFormatCustomSetName="DolbyE DBMD Chunk" audioFormatCustomSetType="CUSTOM_SET_TYPE_DOLBYE_DBMD_CHUNK" audioFormatCustomSetVersion="1">
      <dbmd>
        <metadataSegment ID="1">
          <dolbyE ID="0">
            <programConfig>10</programConfig>
            <frameRateCode>5</frameRateCode>
            <smpteTimeCode>01:00:00:00</smpteTimeCode>
          </dolbyE>
        </metadataSegment>
        <metadataSegment ID="3">
          <ac3Program ID="0">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>1</surMixLev>
            <dSurMod>2</dSurMod>
            <dialNorm>22</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>0</origBs>
            <langCode exists="0">
            </langCode>
            <audioProdInfo exists="0">
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>1</loRoCMixLev>
              <loRoSurMixLev>1</loRoSurMixLev>
              <ltRtCMixLev>2</ltRtCMixLev>
              <ltRtSurMixLev>3</ltRtSurMixLev>
              <dMixMod>0</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>0</dSurExMod>
              <dHeadPhonMod>1</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="1">128</compr1>
            <dynRng1 exists="0">0</dynRng1>
            <programDescriptionText>Program 1 (Ice Hockey)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="1">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>1</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>1</cMixLev>
            <surMixLev>2</surMixLev>
            <dSurMod>3</dSurMod>
            <dialNorm>23</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>1</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="0">
              <mixLevel>21</mixLevel>
              <roomTyp>1</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>1</loRoCMixLev>
              <loRoSurMixLev>2</loRoSurMixLev>
              <ltRtCMixLev>3</ltRtCMixLev>
              <ltRtSurMixLev>4</ltRtSurMixLev>
              <dMixMod>1</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>1</dSurExMod>
              <dHeadPhonMod>2</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="1">255</compr1>
            <dynRng1 exists="0">1</dynRng1>
            <programDescriptionText>Program 2 (Hockey sobre hielo)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="2">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>2</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>2</cMixLev>
            <surMixLev>3</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>26</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>1</origBs>
            <langCode exists="1">)"
R"(               <langCod>20</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>1</mixLevel>
              <roomTyp>0</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>3</loRoCMixLev>
              <loRoSurMixLev>4</loRoSurMixLev>
              <ltRtCMixLev>5</ltRtCMixLev>
              <ltRtSurMixLev>6</ltRtSurMixLev>
              <dMixMod>2</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>2</dSurExMod>
              <dHeadPhonMod>3</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">5</compr1>
            <dynRng1 exists="0">2</dynRng1>
            <programDescriptionText>Program 3 (Hockey sur glace)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="3">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>3</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>3</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>1</dSurMod>
            <dialNorm>28</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>0</origBs>
            <langCode exists="1">
              <langCod>100</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>10</mixLevel>
              <roomTyp>1</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>4</loRoCMixLev>
              <loRoSurMixLev>5</loRoSurMixLev>
              <ltRtCMixLev>6</ltRtCMixLev>
              <ltRtSurMixLev>7</ltRtSurMixLev>
              <dMixMod>3</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>3</dSurExMod>
              <dHeadPhonMod>0</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">4</compr1>
            <dynRng1 exists="0">3</dynRng1>
            <programDescriptionText>Program 4 (Calcio)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="4">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>4</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>1</surMixLev>
            <dSurMod>2</dSurMod>
            <dialNorm>31</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>0</origBs>
            <langCode exists="1">
              <langCod>150</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>23</mixLevel>
              <roomTyp>2</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>5</loRoCMixLev>
              <loRoSurMixLev>6</loRoSurMixLev>
              <ltRtCMixLev>7</ltRtCMixLev>
              <ltRtSurMixLev>0</ltRtSurMixLev>
              <dMixMod>0</dMixMod>
            </extBsi1e>
            <extBsi2e exists="0">
            </extBsi2e>
            <compr1 exists="0">3</compr1>
            <dynRng1 exists="0">4</dynRng1>
            <programDescriptionText>Program 5 (Gimnasia rítmica)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="5">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>5</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>1</cMixLev>
            <surMixLev>2</surMixLev>
            <dSurMod>3</dSurMod>
            <dialNorm>0</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>1</origBs>
            <langCode exists="1">
              <langCod>255</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>31</mixLevel>
              <roomTyp>3</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>6</loRoCMixLev>
              <loRoSurMixLev>7</loRoSurMixLev>
              <ltRtCMixLev>0</ltRtCMixLev>
              <ltRtSurMixLev>1</ltRtSurMixLev>
              <dMixMod>1</dMixMod>
            </extBsi1e>
            <extBsi2e exists="0">
              <dSurExMod>2</dSurExMod>
              <dHeadPhonMod>2</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">2</compr1>
            <dynRng1 exists="0">5</dynRng1>
            <programDescriptionText>Program 6 (Gymnastique rythmique)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="6">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>6</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>2</cMixLev>
            <surMixLev>3</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>1</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>1</origBs>
            <langCode exists="1">
              <langCod>200</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>15</mixLevel>
              <roomTyp>2</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>7</loRoCMixLev>
              <loRoSurMixLev>0</loRoSurMixLev>
              <ltRtCMixLev>1</ltRtCMixLev>
              <ltRtSurMixLev>2</ltRtSurMixLev>
              <dMixMod>3</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>3</dSurExMod>
              <dHeadPhonMod>3</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">1</compr1>
            <dynRng1 exists="1">255</dynRng1>
            <programDescriptionText>Program 7 (Piłka nożna)</programDescriptionText>
          </ac3Program>
          <ac3Program ID="7">
            <programInfo>
              <acMod>1</acMod>
              <bsMod>7</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>3</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>1</dSurMod>
            <dialNorm>10</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>0</origBs>
            <langCode exists="1">
              <langCod>44</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>28</mixLevel>
              <roomTyp>0</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="0">
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>0</dSurExMod>
              <dHeadPhonMod>0</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">0</compr1>
            <dynRng1 exists="1">128</dynRng1>
            <programDescriptionText>Program 8 (Football)</programDescriptionText>
          </ac3Program>
        </metadataSegment>
        <metadataSegment ID="11">
          <encodeParameters ID="0">
            <hpFOn>0</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="1">
            <hpFOn>1</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="2">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="3">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="4">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="5">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="6">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>1</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="7">
            <hpFOn>0</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>1</rfPremphOn>
          </encodeParameters>
        </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>
)";

#endif
