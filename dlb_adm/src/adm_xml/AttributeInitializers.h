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

// This file is intended to be included in exactly one place, inside the
// AttributeDescriptor.cpp file.  It contains the initializer array for
// the attribute descriptors, in one location for easy editing.

static const AttributeInitializer initializers[] =
{
    /* xml */
    { DLB_ADM_ENTITY_TYPE_XML, "version",  DLB_ADM_TAG_XML_VERSION,  DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_XML, "encoding", DLB_ADM_TAG_XML_ENCODING, DLB_ADM_VALUE_TYPE_STRING },

    /* ituADM */
    { DLB_ADM_ENTITY_TYPE_ITU_ADM, "xmlns", DLB_ADM_TAG_ITU_ADM_XMLNS, DLB_ADM_VALUE_TYPE_STRING },

    /* frameFormat */
    { DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, "frameFormatID", DLB_ADM_TAG_FRAME_FORMAT_ID,             DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, "type",          DLB_ADM_TAG_FRAME_FORMAT_TYPE,           DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, "start",         DLB_ADM_TAG_FRAME_FORMAT_START,          DLB_ADM_VALUE_TYPE_STRING /*DLB_ADM_VALUE_TYPE_TIME*/ },     // TODO
    { DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, "duration",      DLB_ADM_TAG_FRAME_FORMAT_DURATION,       DLB_ADM_VALUE_TYPE_STRING /*DLB_ADM_VALUE_TYPE_TIME*/ },     // TODO
    { DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, "timeReference", DLB_ADM_TAG_FRAME_FORMAT_TIME_REFERENCE, DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, "flowID",        DLB_ADM_TAG_FRAME_FORMAT_FLOW_ID,        DLB_ADM_VALUE_TYPE_STRING },

    /* transportTrackFormat */
    { DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, "transportID",   DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_ID,         DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, "transportName", DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NAME,       DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, "numIDs",        DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_IDS,    DLB_ADM_VALUE_TYPE_UINT },
    { DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, "numTracks",     DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_TRACKS, DLB_ADM_VALUE_TYPE_UINT },

    /* audioProgramme */
    { DLB_ADM_ENTITY_TYPE_PROGRAMME, "audioProgrammeID",       DLB_ADM_TAG_PROGRAMME_ID,       DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_PROGRAMME, "audioProgrammeName",     DLB_ADM_TAG_PROGRAMME_NAME,     DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_PROGRAMME, "audioProgrammeLanguage", DLB_ADM_TAG_PROGRAMME_LANGUAGE, DLB_ADM_VALUE_TYPE_STRING },

    /* audioContent */
    { DLB_ADM_ENTITY_TYPE_CONTENT, "audioContentID",       DLB_ADM_TAG_CONTENT_ID,       DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_CONTENT, "audioContentName",     DLB_ADM_TAG_CONTENT_NAME,     DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_CONTENT, "audioContentLanguage", DLB_ADM_TAG_CONTENT_LANGUAGE, DLB_ADM_VALUE_TYPE_STRING },

    /* audioObject */
    { DLB_ADM_ENTITY_TYPE_OBJECT, "audioObjectID",   DLB_ADM_TAG_OBJECT_ID,         DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_OBJECT, "audioObjectName", DLB_ADM_TAG_OBJECT_NAME,       DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_OBJECT, "lstart",          DLB_ADM_TAG_OBJECT_L_START,    DLB_ADM_VALUE_TYPE_STRING /*DLB_ADM_VALUE_TYPE_TIME*/ },     // TODO
    { DLB_ADM_ENTITY_TYPE_OBJECT, "lduration",       DLB_ADM_TAG_OBJECT_L_DURATION, DLB_ADM_VALUE_TYPE_STRING /*DLB_ADM_VALUE_TYPE_TIME*/ },     // TODO

    /* audioTrackUID */
    { DLB_ADM_ENTITY_TYPE_TRACK_UID, "UID",        DLB_ADM_TAG_TRACK_UID_UID,         DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_TRACK_UID, "sampleRate", DLB_ADM_TAG_TRACK_UID_SAMPLE_RATE, DLB_ADM_VALUE_TYPE_UINT },
    { DLB_ADM_ENTITY_TYPE_TRACK_UID, "bitDepth",   DLB_ADM_TAG_TRACK_UID_BIT_DEPTH,   DLB_ADM_VALUE_TYPE_UINT },

    /* audioPackFormat */
    { DLB_ADM_ENTITY_TYPE_PACK_FORMAT, "audioPackFormatID",   DLB_ADM_TAG_PACK_FORMAT_ID,              DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_PACK_FORMAT, "audioPackFormatName", DLB_ADM_TAG_PACK_FORMAT_NAME,            DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_PACK_FORMAT, "typeLabel",           DLB_ADM_TAG_PACK_FORMAT_TYPE_LABEL,      DLB_ADM_VALUE_TYPE_AUDIO_TYPE },
    { DLB_ADM_ENTITY_TYPE_PACK_FORMAT, "typeDefinition",      DLB_ADM_TAG_PACK_FORMAT_TYPE_DEFINITION, DLB_ADM_VALUE_TYPE_STRING },

    /* audioStreamFormat */
    { DLB_ADM_ENTITY_TYPE_STREAM_FORMAT, "audioStreamFormatID",   DLB_ADM_TAG_STREAM_FORMAT_ID,                DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_STREAM_FORMAT, "audioStreamFormatName", DLB_ADM_TAG_STREAM_FORMAT_NAME,              DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_STREAM_FORMAT, "formatLabel",           DLB_ADM_TAG_STREAM_FORMAT_FORMAT_LABEL,      DLB_ADM_VALUE_TYPE_AUDIO_TYPE },
    { DLB_ADM_ENTITY_TYPE_STREAM_FORMAT, "formatDefinition",      DLB_ADM_TAG_STREAM_FORMAT_FORMAT_DEFINITION, DLB_ADM_VALUE_TYPE_STRING },

    /* audioChannelFormat */
    { DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, "audioChannelFormatID",   DLB_ADM_TAG_CHANNEL_FORMAT_ID,              DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, "audioChannelFormatName", DLB_ADM_TAG_CHANNEL_FORMAT_NAME,            DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, "typeLabel",              DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_LABEL,      DLB_ADM_VALUE_TYPE_AUDIO_TYPE },
    { DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, "typeDefinition",         DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_DEFINITION, DLB_ADM_VALUE_TYPE_STRING },

    /* audioTrackFormat */
    { DLB_ADM_ENTITY_TYPE_TRACK_FORMAT, "audioTrackFormatID",   DLB_ADM_TAG_TRACK_FORMAT_ID,                DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_TRACK_FORMAT, "audioTrackFormatName", DLB_ADM_TAG_TRACK_FORMAT_NAME,              DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_TRACK_FORMAT, "formatLabel",          DLB_ADM_TAG_TRACK_FORMAT_FORMAT_LABEL,      DLB_ADM_VALUE_TYPE_AUDIO_TYPE },
    { DLB_ADM_ENTITY_TYPE_TRACK_FORMAT, "formatDefinition",     DLB_ADM_TAG_TRACK_FORMAT_FORMAT_DEFINITION, DLB_ADM_VALUE_TYPE_STRING },

    /* audioBlockFormat */
    { DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, "audioBlockFormatID", DLB_ADM_TAG_BLOCK_FORMAT_ID,       DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, "rtime",              DLB_ADM_TAG_BLOCK_FORMAT_RTIME,    DLB_ADM_VALUE_TYPE_STRING /*DLB_ADM_VALUE_TYPE_TIME*/ },     // TODO
    { DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, "duration",           DLB_ADM_TAG_BLOCK_FORMAT_DURATION, DLB_ADM_VALUE_TYPE_STRING /*DLB_ADM_VALUE_TYPE_TIME*/ },     // TODO

    /* alternativeValueSet */
    { DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, "alternativeValueSetID", DLB_ADM_TAG_ALT_VALUE_SET_ID, DLB_ADM_VALUE_TYPE_STRING },
    // TODO...

    /* audioTrack */
    { DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, "trackID", DLB_ADM_TAG_AUDIO_TRACK_ID, DLB_ADM_VALUE_TYPE_UINT },

    /* audioFormatExtended */
    { DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED, "version", DLB_ADM_TAG_AUDIO_FORMAT_EXT_VERSION, DLB_ADM_VALUE_TYPE_STRING },

    /* audioProgrammeLabel */
    { DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, "audioProgrammeLabelValue", DLB_ADM_TAG_PROGRAMME_LABEL_VALUE,    DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL, "language",                 DLB_ADM_TAG_PROGRAMME_LABEL_LANGUAGE, DLB_ADM_VALUE_TYPE_STRING },

    /* audioContentLabel */
    { DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, "audioContentLabelValue", DLB_ADM_TAG_CONTENT_LABEL_VALUE,    DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_CONTENT_LABEL, "language",               DLB_ADM_TAG_CONTENT_LABEL_LANGUAGE, DLB_ADM_VALUE_TYPE_STRING },

    /* audioObjectLabel */
    { DLB_ADM_ENTITY_TYPE_OBJECT_LABEL, "audioObjectLabelValue", DLB_ADM_TAG_OBJECT_LABEL_VALUE,    DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_OBJECT_LABEL, "language",              DLB_ADM_TAG_OBJECT_LABEL_LANGUAGE, DLB_ADM_VALUE_TYPE_STRING },

    /* gain */
    { DLB_ADM_ENTITY_TYPE_GAIN, "gainValue", DLB_ADM_TAG_SPEAKER_GAIN_VALUE, DLB_ADM_VALUE_TYPE_FLOAT },
    { DLB_ADM_ENTITY_TYPE_GAIN, "gainUnit",  DLB_ADM_TAG_SPEAKER_GAIN_UNIT,  DLB_ADM_VALUE_TYPE_STRING },

    /* speakerLabel */
    { DLB_ADM_ENTITY_TYPE_SPEAKER_LABEL, "speakerLabelValue", DLB_ADM_TAG_SPEAKER_LABEL_VALUE, DLB_ADM_VALUE_TYPE_STRING },

    /* dialogue */
    { DLB_ADM_ENTITY_TYPE_DIALOGUE, "dialogueValue",          DLB_ADM_TAG_DIALOGUE_VALUE,             DLB_ADM_VALUE_TYPE_UINT },
    { DLB_ADM_ENTITY_TYPE_DIALOGUE, "nonDialogueContentKind", DLB_ADM_TAG_DIALOGUE_NON_DIALOGUE_KIND, DLB_ADM_VALUE_TYPE_UINT },
    { DLB_ADM_ENTITY_TYPE_DIALOGUE, "dialogueContentKind",    DLB_ADM_TAG_DIALOGUE_DIALOGUE_KIND,     DLB_ADM_VALUE_TYPE_UINT },
    { DLB_ADM_ENTITY_TYPE_DIALOGUE, "mixedContentKind",       DLB_ADM_TAG_DIALOGUE_MIXED_KIND,        DLB_ADM_VALUE_TYPE_UINT },

    /* cartesian */
    { DLB_ADM_ENTITY_TYPE_CARTESIAN, "cartesianValue", DLB_ADM_TAG_CARTESIAN_VALUE, DLB_ADM_VALUE_TYPE_BOOL },

    /* position */
    { DLB_ADM_ENTITY_TYPE_POSITION, "positionValue",  DLB_ADM_TAG_POSITION_VALUE,            DLB_ADM_VALUE_TYPE_FLOAT },
    { DLB_ADM_ENTITY_TYPE_POSITION, "coordinate",     DLB_ADM_TAG_POSITION_COORDINATE,       DLB_ADM_VALUE_TYPE_STRING },
    { DLB_ADM_ENTITY_TYPE_POSITION, "screenEdgeLock", DLB_ADM_TAG_POSITION_SCREEN_EDGE_LOCK, DLB_ADM_VALUE_TYPE_STRING },

    /* equation */
    { DLB_ADM_ENTITY_TYPE_EQUATION, "equationValue", DLB_ADM_TAG_EQUATION_VALUE, DLB_ADM_VALUE_TYPE_STRING },

    /* degree */
    { DLB_ADM_ENTITY_TYPE_DEGREE, "degreeValue", DLB_ADM_TAG_DEGREE_VALUE, DLB_ADM_VALUE_TYPE_INT },

    /* order */
    { DLB_ADM_ENTITY_TYPE_ORDER, "orderValue", DLB_ADM_TAG_ORDER_VALUE, DLB_ADM_VALUE_TYPE_UINT },

    /* normalization */
    { DLB_ADM_ENTITY_TYPE_NORMALIZATION, "normalizationValue", DLB_ADM_TAG_NORMALIZATION_VALUE, DLB_ADM_VALUE_TYPE_STRING },

    /* frequency */
    { DLB_ADM_ENTITY_TYPE_FREQUENCY, "frequencyValue", DLB_ADM_TAG_FREQUENCY_VALUE,           DLB_ADM_VALUE_TYPE_FLOAT },
    { DLB_ADM_ENTITY_TYPE_FREQUENCY, "typeDefinition", DLB_ADM_TAG_FREQUENCY_TYPE_DEFINITION, DLB_ADM_VALUE_TYPE_STRING },

};