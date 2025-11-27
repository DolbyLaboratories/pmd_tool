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

#ifndef TEST_INCORRECT_DOLBYE_XML_BUFFER_H
#define TEST_INCORRECT_DOLBYE_XML_BUFFER_H

#include <string>
#include <iostream>

static std::string dolbyE_51_altvalset = R"(
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<frame version="ITU-R_BS.2125-1">
  <frameHeader>
    <frameFormat duration="00:00:00.01920S48000" flowID="32644eb7-9bac-48fc-81b2-f9cdfa3fac2f" frameFormatID="FF_00000001" start="00:00:00.00000S48000" timeReference="local" type="full"/>
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
    <audioProgramme audioProgrammeID="APR_1001" audioProgrammeLanguage="eng" audioProgrammeName="Dolby E Programme-1">
      <audioContentIDRef>ACO_1001</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-20</dialogueLoudness>
      </loudnessMetadata>
      <alternativeValueSetIDRef>AVS_1001_0001</alternativeValueSetIDRef>
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
      <alternativeValueSet alternativeValueSetID="AVS_1001_0001">
        <gain gainUnit="dB">-3.00</gain>
        <positionOffset coordinate="X">0.00</positionOffset>
      </alternativeValueSet>      
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

static std::string dolbyE_2x20_complementary = R"(
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
      <audioContentIDRef>ACO_1002</audioContentIDRef>
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
    <audioContent audioContentID="ACO_1002" audioContentLanguage="und" audioContentName="Content 2">
      <audioObjectIDRef>AO_1002</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-29</dialogueLoudness>
      </loudnessMetadata>
      <dialogue mixedContentKind="1">2</dialogue>
    </audioContent>    
    <audioObject audioObjectID="AO_1001" audioObjectName="Object 1" interact="0">
      <audioComplementaryObjectIDRef>AO_1002</audioComplementaryObjectIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000001</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000002</audioTrackUIDRef>
    </audioObject>
    <audioObject audioObjectID="AO_1002" audioObjectName="audioObject_2" interact="0">
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000003</audioTrackUIDRef>
      <audioTrackUIDRef>ATU_00000004</audioTrackUIDRef>
    </audioObject>    
    <audioTrackUID UID="ATU_00000001">
      <audioChannelFormatIDRef>AC_00010001</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
    <audioTrackUID UID="ATU_00000002">
      <audioChannelFormatIDRef>AC_00010002</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>
    </audioTrackUID>
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
