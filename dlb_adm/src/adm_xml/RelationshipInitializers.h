/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

// This file is intended to be included in exactly one place, inside the
// RelationshipDescriptor.cpp file.  It contains the initializer array for
// the entity relationship descriptors, in one location for easy editing.

static const RelationshipDescriptor initializers[] =
{
    // Toplevel
    {
        DLB_ADM_ENTITY_TYPE_TOPLEVEL,
        DLB_ADM_ENTITY_TYPE_XML,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_TOPLEVEL,
        DLB_ADM_ENTITY_TYPE_ITU_ADM,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_TOPLEVEL,
        DLB_ADM_ENTITY_TYPE_FRAME,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, RelationshipArity::ANY },      // TODO: do we really want to allow multiple?
    },

    // ituADM
    {
        DLB_ADM_ENTITY_TYPE_ITU_ADM,
        DLB_ADM_ENTITY_TYPE_CORE_METADATA,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },

    // coreMetadata
    {
        DLB_ADM_ENTITY_TYPE_CORE_METADATA,
        DLB_ADM_ENTITY_TYPE_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },

    // format
    {
        DLB_ADM_ENTITY_TYPE_FORMAT,
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },

    // frame
    {
        DLB_ADM_ENTITY_TYPE_FRAME,
        DLB_ADM_ENTITY_TYPE_FRAME_HEADER,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_FRAME,
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_FRAME,
        DLB_ADM_ENTITY_TYPE_CORE_METADATA,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },

    // frameHeader
    {
        DLB_ADM_ENTITY_TYPE_FRAME_HEADER,
        DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_FRAME_HEADER,
        DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },   // { 1, 1 } to match the spec (PMDLIB-108)
    },

    // frameFormat
    {
        DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {   // Keeping this for now for backwards compatibility (PMDLIB-108)
        DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
        DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },

    // changedIDs
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_OBJECT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_CONTENT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
        DLB_ADM_ENTITY_TYPE_PROGRAMME,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },

    // transportTrackFormat
    {
        DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT,
        DLB_ADM_ENTITY_TYPE_AUDIO_TRACK,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },

    // audioTrack
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_TRACK,
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },

    // audioFormatExtended
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_PROGRAMME,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_CONTENT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_OBJECT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },

    // audioProgramme
    {
        DLB_ADM_ENTITY_TYPE_PROGRAMME,
        DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_PROGRAMME,
        DLB_ADM_ENTITY_TYPE_CONTENT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 1, RelationshipArity::ANY },
    },

    // audioContent
    {
        DLB_ADM_ENTITY_TYPE_CONTENT,
        DLB_ADM_ENTITY_TYPE_CONTENT_LABEL,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CONTENT,
        DLB_ADM_ENTITY_TYPE_OBJECT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 1, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CONTENT,
        DLB_ADM_ENTITY_TYPE_DIALOGUE,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },

    // audioObject
    {
        DLB_ADM_ENTITY_TYPE_OBJECT,
        DLB_ADM_ENTITY_TYPE_OBJECT_LABEL,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_OBJECT,
        DLB_ADM_ENTITY_TYPE_GAIN,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_OBJECT,
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_OBJECT,
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_OBJECT,
        DLB_ADM_ENTITY_TYPE_OBJECT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },

    // audioTrackUID
    {
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_TRACK_UID,
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, 1 },
    },

    // audioPackFormat
    {
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },

    // audioStreamFormat
    {
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 0, RelationshipArity::ANY },
    },

    // audioChannelFormat
    {
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 1, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
        DLB_ADM_ENTITY_TYPE_FREQUENCY,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 2 },
    },

    // audioTrackFormat
    {
        DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
        DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
        ENTITY_RELATIONSHIP::REFERENCES,
        { 1, 1 },
    },

    // audioBlockFormat
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_GAIN,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_SPEAKER_LABEL,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, RelationshipArity::ANY },
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_CARTESIAN,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_POSITION,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 2, 11 },  // TODO: confirm max
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_EQUATION,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_ORDER,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_DEGREE,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },
    {
        DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
        DLB_ADM_ENTITY_TYPE_NORMALIZATION,
        ENTITY_RELATIONSHIP::CONTAINS,
        { 0, 1 },
    },

};
