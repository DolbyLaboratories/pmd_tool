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

#ifndef DOLBY_E_XML_REF_BUFFER_H
#define DOLBY_E_XML_REF_BUFFER_H

#include <string>
#include <iostream>

/* Test files from Dolby_Repository_File_Format_SDK_Dolby_E_Supplemental_v1.0.3 */

static std::string dolbyE_4x_20_1 = R"(
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
        <dialogueLoudness>-24</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Content 1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-24</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
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
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 2 (Program 2)\">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-25</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Content 2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-25</dialogueLoudness>
      </loudnessMetadata>
      <dialogue nonDialogueContentKind="3">0</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 2" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000003">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
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
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000005</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000006</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000005">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000006">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioProgramme audioProgrammeID="APR_1004" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 4 (Program 4)\">
      <audioContentIDRef>ACO_1004</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-27</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1004" audioContentLanguage="und" audioContentName="Content 4">
      <audioObjectIDRef>AO_1004</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-27</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="0">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1004" audioObjectName="Object 4" interact="0">
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
            <programConfig>6</programConfig>
            <frameRateCode>3</frameRateCode>
            <smpteTimeCode>01:00:00:00</smpteTimeCode>
          </dolbyE>
        </metadataSegment>
        <metadataSegment ID="3">
          <ac3Program ID="0">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>24</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>0</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>20</mixLevel>
              <roomTyp>0</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>4</loRoCMixLev>
              <loRoSurMixLev>3</loRoSurMixLev>
              <ltRtCMixLev>3</ltRtCMixLev>
              <ltRtSurMixLev>4</ltRtSurMixLev>
              <dMixMod>0</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>1</dSurExMod>
              <dHeadPhonMod>0</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">1</compr1>
            <dynRng1 exists="0">2</dynRng1>
            <programDescriptionText>Program 1</programDescriptionText>
          </ac3Program>
          <ac3Program ID="1">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>1</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>1</dSurMod>
            <dialNorm>25</dialNorm>
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
              <loRoCMixLev>3</loRoCMixLev>
              <loRoSurMixLev>4</loRoSurMixLev>
              <ltRtCMixLev>4</ltRtCMixLev>
              <ltRtSurMixLev>3</ltRtSurMixLev>
              <dMixMod>1</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>2</dSurExMod>
              <dHeadPhonMod>1</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">2</compr1>
            <dynRng1 exists="0">3</dynRng1>
            <programDescriptionText>Program 2</programDescriptionText>
          </ac3Program>
          <ac3Program ID="2">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>2</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>2</dSurMod>
            <dialNorm>26</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>0</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>22</mixLevel>
              <roomTyp>0</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>4</loRoCMixLev>
              <loRoSurMixLev>3</loRoSurMixLev>
              <ltRtCMixLev>4</ltRtCMixLev>
              <ltRtSurMixLev>3</ltRtSurMixLev>
              <dMixMod>0</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>2</dSurExMod>
              <dHeadPhonMod>0</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">3</compr1>
            <dynRng1 exists="0">4</dynRng1>
            <programDescriptionText>Program 3</programDescriptionText>
          </ac3Program>
          <ac3Program ID="3">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>3</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>27</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>1</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="0">
              <mixLevel>23</mixLevel>
              <roomTyp>1</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>3</loRoCMixLev>
              <loRoSurMixLev>4</loRoSurMixLev>
              <ltRtCMixLev>3</ltRtCMixLev>
              <ltRtSurMixLev>4</ltRtSurMixLev>
              <dMixMod>1</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>1</dSurExMod>
              <dHeadPhonMod>1</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">4</compr1>
            <dynRng1 exists="0">1</dynRng1>
            <programDescriptionText>Program 4</programDescriptionText>
          </ac3Program>
        </metadataSegment>
        <metadataSegment ID="11">
          <encodeParameters ID="0">
            <hpFOn>0</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="1">
            <hpFOn>1</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="2">
            <hpFOn>0</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="3">
            <hpFOn>1</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
        </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>
)";

static std::string dolbyE_51_20_1 = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="6458aa23-9b3c-4a72-bdab-74f81726d473" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
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
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Content 1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
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
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 2 (Program 2)\">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Content 2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 2" interact="0">
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
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>1</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>29</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>1</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
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
            <compr1 exists="0">2</compr1>
            <dynRng1 exists="0">1</dynRng1>
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

static std::string dolbyE_51_20_2 = R"(
 <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="2571b6b5-21a2-400c-bea8-9da4cdea265c" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="4" numTracks="4" transportID="TP_0001" transportName="X">
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
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 2 (Program 2)\">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-27</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Content 2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-27</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 2" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000003">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
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
              <acMod>2</acMod>
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>1</dSurMod>
            <dialNorm>22</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>1</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="0">
              <mixLevel>24</mixLevel>
              <roomTyp>0</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>3</loRoCMixLev>
              <loRoSurMixLev>4</loRoSurMixLev>
              <ltRtCMixLev>3</ltRtCMixLev>
              <ltRtSurMixLev>4</ltRtSurMixLev>
              <dMixMod>1</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>0</dSurExMod>
              <dHeadPhonMod>1</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">2</compr1>
            <dynRng1 exists="0">1</dynRng1>
            <programDescriptionText>Program 1</programDescriptionText>
          </ac3Program>
          <ac3Program ID="1">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>1</dSurMod>
            <dialNorm>27</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>0</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>23</mixLevel>
              <roomTyp>1</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>4</loRoCMixLev>
              <loRoSurMixLev>3</loRoSurMixLev>
              <ltRtCMixLev>4</ltRtCMixLev>
              <ltRtSurMixLev>3</ltRtSurMixLev>
              <dMixMod>0</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>1</dSurExMod>
              <dHeadPhonMod>0</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">1</compr1>
            <dynRng1 exists="0">2</dynRng1>
            <programDescriptionText>Program 2</programDescriptionText>
          </ac3Program>
        </metadataSegment>
        <metadataSegment ID="11">
          <encodeParameters ID="0">
            <hpFOn>0</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="1">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
        </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>   
)";

static std::string dolbyE_51_1 = R"(
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="0cd8ce4e-71d0-48e2-8d7e-0575b29e6a00" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="6" numTracks="6" transportID="TP_0001" transportName="X">
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
        <dialogueLoudness>-23</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Content 1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-23</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
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
  </audioFormatExtended>
  <audioFormatCustom>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1001" audioFormatCustomSetName="DolbyE DBMD Chunk" audioFormatCustomSetType="CUSTOM_SET_TYPE_DOLBYE_DBMD_CHUNK" audioFormatCustomSetVersion="1">
      <dbmd>
        <metadataSegment ID="1">
          <dolbyE ID="0">
            <programConfig>11</programConfig>
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
            <cMixLev>0</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>23</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>1</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="0">
              <mixLevel>24</mixLevel>
              <roomTyp>0</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>4</loRoCMixLev>
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
            <dynRng1 exists="0">1</dynRng1>
            <programDescriptionText>Program 1</programDescriptionText>
          </ac3Program>
        </metadataSegment>
        <metadataSegment ID="11">
          <encodeParameters ID="0">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
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

static std::string dolbyE_51_2 = R"(
   <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="eb7cd32e-5d29-4834-908e-23e4e2e8c9e6" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="2" numTracks="2" transportID="TP_0001" transportName="X">
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
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 1 (Program 1)\">
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-24</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Content 1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-24</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
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
            <frameRateCode>3</frameRateCode>
            <smpteTimeCode>01:00:00:00</smpteTimeCode>
          </dolbyE>
        </metadataSegment>
        <metadataSegment ID="3">
          <ac3Program ID="0">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>1</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>24</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>0</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>20</mixLevel>
              <roomTyp>1</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>5</loRoCMixLev>
              <loRoSurMixLev>4</loRoSurMixLev>
              <ltRtCMixLev>3</ltRtCMixLev>
              <ltRtSurMixLev>4</ltRtSurMixLev>
              <dMixMod>2</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>2</dSurExMod>
              <dHeadPhonMod>1</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">2</compr1>
            <dynRng1 exists="0">3</dynRng1>
            <programDescriptionText>Program 1</programDescriptionText>
          </ac3Program>
        </metadataSegment>
        <metadataSegment ID="11">
          <encodeParameters ID="0">
            <hpFOn>0</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
        </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>
 
)";

static std::string dolbyE_20_20_1 = R"(
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="4277e02b-8c5c-4aee-abb0-939b2af83096" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
    <transportTrackFormat numIDs="4" numTracks="4" transportID="TP_0001" transportName="X">
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
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1001" audioContentLanguage="und" audioContentName="Content 1">
      <audioObjectIDRef>AO_1001</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
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
    <audioProgramme audioProgrammeID="APR_1002" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 2 (Program 2)\">
      <audioContentIDRef>ACO_1002</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-30</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Content 2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-30</dialogueLoudness>
      </loudnessMetadata>
      <dialogue nonDialogueContentKind="3">0</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1002" audioObjectName="Object 2" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
    </audioObject>
    <audioTrackUID UID="ATU_00000003">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000004">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
  </audioFormatExtended>
  <audioFormatCustom>
    <audioFormatCustomSet audioFormatCustomSetID="AFC_1001" audioFormatCustomSetName="DolbyE DBMD Chunk" audioFormatCustomSetType="CUSTOM_SET_TYPE_DOLBYE_DBMD_CHUNK" audioFormatCustomSetVersion="1">
      <dbmd>
        <metadataSegment ID="1">
          <dolbyE ID="0">
            <programConfig>19</programConfig>
            <frameRateCode>3</frameRateCode>
            <smpteTimeCode>01:00:00:00</smpteTimeCode>
          </dolbyE>
        </metadataSegment>
        <metadataSegment ID="3">
          <ac3Program ID="0">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>0</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>1</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>0</dSurMod>
            <dialNorm>29</dialNorm>
            <copyRightB>1</copyRightB>
            <origBs>0</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="0">
              <mixLevel>21</mixLevel>
              <roomTyp>1</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>5</loRoCMixLev>
              <loRoSurMixLev>3</loRoSurMixLev>
              <ltRtCMixLev>4</ltRtCMixLev>
              <ltRtSurMixLev>5</ltRtSurMixLev>
              <dMixMod>0</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>1</dSurExMod>
              <dHeadPhonMod>0</dHeadPhonMod>
              <adConvTyp>1</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">1</compr1>
            <dynRng1 exists="0">2</dynRng1>
            <programDescriptionText>Program 1</programDescriptionText>
          </ac3Program>
          <ac3Program ID="1">
            <programInfo>
              <acMod>2</acMod>
              <bsMod>1</bsMod>
              <lfeOn>0</lfeOn>
            </programInfo>
            <cMixLev>2</cMixLev>
            <surMixLev>0</surMixLev>
            <dSurMod>1</dSurMod>
            <dialNorm>30</dialNorm>
            <copyRightB>0</copyRightB>
            <origBs>1</origBs>
            <langCode exists="0">
              <langCod>0</langCod>
            </langCode>
            <audioProdInfo exists="1">
              <mixLevel>25</mixLevel>
              <roomTyp>2</roomTyp>
            </audioProdInfo>
            <extBsi1e exists="1">
              <loRoCMixLev>6</loRoCMixLev>
              <loRoSurMixLev>4</loRoSurMixLev>
              <ltRtCMixLev>5</ltRtCMixLev>
              <ltRtSurMixLev>3</ltRtSurMixLev>
              <dMixMod>1</dMixMod>
            </extBsi1e>
            <extBsi2e exists="1">
              <dSurExMod>0</dSurExMod>
              <dHeadPhonMod>1</dHeadPhonMod>
              <adConvTyp>0</adConvTyp>
            </extBsi2e>
            <compr1 exists="0">2</compr1>
            <dynRng1 exists="0">1</dynRng1>
            <programDescriptionText>Program 2</programDescriptionText>
          </ac3Program>
        </metadataSegment>
        <metadataSegment ID="11">
          <encodeParameters ID="0">
            <hpFOn>0</hpFOn>
            <bwLpFOn>0</bwLpFOn>
            <lfeLpFOn>0</lfeLpFOn>
            <sur90On>1</sur90On>
            <surAttOn>0</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
          <encodeParameters ID="1">
            <hpFOn>1</hpFOn>
            <bwLpFOn>1</bwLpFOn>
            <lfeLpFOn>1</lfeLpFOn>
            <sur90On>0</sur90On>
            <surAttOn>1</surAttOn>
            <rfPremphOn>0</rfPremphOn>
          </encodeParameters>
        </metadataSegment>
      </dbmd>
    </audioFormatCustomSet>
  </audioFormatCustom>
</frame>

)";

#endif
