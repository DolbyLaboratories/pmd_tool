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
// EntityDescriptor.cpp file.  It contains the initializer array for
// the entity descriptors, in one location for easy editing.

static const EntityDescriptor initializers[] =
{
    {
        "___void___",
        DLB_ADM_ENTITY_TYPE_VOID,
        false,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "___toplevel___",
        DLB_ADM_ENTITY_TYPE_TOPLEVEL,
        false,
        true,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "xml",
        DLB_ADM_ENTITY_TYPE_XML,
        false,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },

    // IDRef entities...
    {
        "audioProgrammeIDRef",
        DLB_ADM_ENTITY_TYPE_PROGRAMME,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioContentIDRef",
        DLB_ADM_ENTITY_TYPE_CONTENT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioObjectIDRef",
        DLB_ADM_ENTITY_TYPE_OBJECT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioPackFormatIDRef",
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioTrackUIDRef",
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioChannelFormatIDRef",
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioStreamFormatIDRef",
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioTrackFormatIDRef",
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioComplementaryObjectIDRef",
        DLB_ADM_ENTITY_TYPE_OBJECT,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "alternativeValueSetIDRef",
        DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET,
        false,
        true,
        true,
        DLB_ADM_TAG_UNKNOWN
    },
    // ... IDRef entities

    // Entities with ADM identifiers...
    {
        "frameFormat",
        DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_FRAME_FORMAT_ID
    },
    {
        "transportTrackFormat",
        DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_ID
    },
    {
        "audioProgramme",
        DLB_ADM_ENTITY_TYPE_PROGRAMME,
        true,
        true,
        false,
        DLB_ADM_TAG_PROGRAMME_ID
    },
    {
        "audioContent",
        DLB_ADM_ENTITY_TYPE_CONTENT,
        true,
        true,
        false,
        DLB_ADM_TAG_CONTENT_ID
    },
    {
        "audioObject",
        DLB_ADM_ENTITY_TYPE_OBJECT,
        true,
        true,
        false,
        DLB_ADM_TAG_OBJECT_ID
    },
    {
        "audioTrackUID",
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        true,
        true,
        false,
        DLB_ADM_TAG_TRACK_UID_UID
    },
    {
        "audioPackFormat",
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_PACK_FORMAT_ID
    },
    {
        "audioStreamFormat",
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_STREAM_FORMAT_ID
    },
    {
        "audioChannelFormat",
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_CHANNEL_FORMAT_ID
    },
    {
        "audioTrackFormat",
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_TRACK_FORMAT_ID
    },
    {
        "audioBlockFormat",
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        true,
        true,
        false,
        DLB_ADM_TAG_BLOCK_FORMAT_ID
    },
    {
        "alternativeValueSet",
        DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET,
        true,
        true,
        false,
        DLB_ADM_TAG_ALT_VALUE_SET_ID
    },
    // ... entities with ADM identifiers

    // Component entities (no ADM identifier)...
    {
        "ituADM",
        DLB_ADM_ENTITY_TYPE_ITU_ADM,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "coreMetadata",
        DLB_ADM_ENTITY_TYPE_CORE_METADATA,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "format",
        DLB_ADM_ENTITY_TYPE_FORMAT,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "frame",
        DLB_ADM_ENTITY_TYPE_FRAME,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "frameHeader",
        DLB_ADM_ENTITY_TYPE_FRAME_HEADER,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "changedIDs",
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioTrack",
        DLB_ADM_ENTITY_TYPE_AUDIO_TRACK,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioFormatExtended",
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        true,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    },
    {
        "audioProgrammeLabel",
        DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL,
        false,
        false,
        false,
        DLB_ADM_TAG_PROGRAMME_LABEL_VALUE
    },
    {
        "audioContentLabel",
        DLB_ADM_ENTITY_TYPE_CONTENT_LABEL,
        false,
        false,
        false,
        DLB_ADM_TAG_CONTENT_LABEL_VALUE
    },
    {
        "audioObjectLabel",
        DLB_ADM_ENTITY_TYPE_OBJECT_LABEL,
        false,
        false,
        false,
        DLB_ADM_TAG_OBJECT_LABEL_VALUE
    },
    {
        "gain",
        DLB_ADM_ENTITY_TYPE_GAIN,
        false,
        false,
        false,
        DLB_ADM_TAG_SPEAKER_GAIN_VALUE
    },
    {
        "speakerLabel",
        DLB_ADM_ENTITY_TYPE_SPEAKER_LABEL,
        false,
        false,
        false,
        DLB_ADM_TAG_SPEAKER_LABEL_VALUE
    },
    {
        "dialogue",
        DLB_ADM_ENTITY_TYPE_DIALOGUE,
        false,
        false,
        false,
        DLB_ADM_TAG_DIALOGUE_VALUE
    },
    {
        "cartesian",
        DLB_ADM_ENTITY_TYPE_CARTESIAN,
        false,
        false,
        false,
        DLB_ADM_TAG_CARTESIAN_VALUE
    },
    {
        "position",
        DLB_ADM_ENTITY_TYPE_POSITION,
        false,
        false,
        false,
        DLB_ADM_TAG_POSITION_VALUE
    },
    {
        "equation",
        DLB_ADM_ENTITY_TYPE_EQUATION,
        false,
        false,
        false,
        DLB_ADM_TAG_EQUATION_VALUE
    },
    {
        "degree",
        DLB_ADM_ENTITY_TYPE_DEGREE,
        false,
        false,
        false,
        DLB_ADM_TAG_DEGREE_VALUE
    },
    {
        "order",
        DLB_ADM_ENTITY_TYPE_ORDER,
        false,
        false,
        false,
        DLB_ADM_TAG_ORDER_VALUE
    },
    {
        "normalization",
        DLB_ADM_ENTITY_TYPE_NORMALIZATION,
        false,
        false,
        false,
        DLB_ADM_TAG_NORMALIZATION_VALUE
    },
    {
        "frequency",
        DLB_ADM_ENTITY_TYPE_FREQUENCY,
        false,
        false,
        false,
        DLB_ADM_TAG_FREQUENCY_VALUE
    },
    // ... component entities (no ADM identifier)
};
