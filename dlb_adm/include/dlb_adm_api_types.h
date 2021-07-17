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

#ifndef DLB_ADM_API_TYPES_H
#define DLB_ADM_API_TYPES_H

#include <string.h>

#include "dlb_adm_entity_id.h"
#include "dlb_adm_fwd_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* API status codes */

typedef enum
{
    DLB_ADM_STATUS_OK,
    DLB_ADM_STATUS_ERROR,
    DLB_ADM_STATUS_NOT_IMPLEMENTED,
    DLB_ADM_STATUS_NULL_POINTER,
    DLB_ADM_STATUS_INVALID_ARGUMENT,
    DLB_ADM_STATUS_OUT_OF_RANGE,
    DLB_ADM_STATUS_OUT_OF_MEMORY,
    DLB_ADM_STATUS_EXCEPTION,
    DLB_ADM_STATUS_NOT_FOUND,
    DLB_ADM_STATUS_NOT_UNIQUE,
    DLB_ADM_STATUS_VALUE_TYPE_MISMATCH,
    DLB_ADM_STATUS_INVALID_RELATIONSHIP,
}  DLB_ADM_STATUS;


/* Boolean type and values */

typedef uint8_t dlb_adm_bool;

enum
{
    DLB_ADM_FALSE,
    DLB_ADM_TRUE
};


/* Library configuration */

typedef struct 
{
    const char      *path_to_common_defs;
} dlb_adm_library_config;


/* Attribute value numeric types */

typedef uint32_t    dlb_adm_uint;
typedef int32_t     dlb_adm_int;
typedef float       dlb_adm_float;


/* ADM audio types */

typedef enum
{
    DLB_ADM_AUDIO_TYPE_NONE,
    DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS,
    DLB_ADM_AUDIO_TYPE_MATRIX,
    DLB_ADM_AUDIO_TYPE_OBJECTS,
    DLB_ADM_AUDIO_TYPE_HOA,
    DLB_ADM_AUDIO_TYPE_BINAURAL,
    DLB_ADM_AUDIO_TYPE_LAST_STD = DLB_ADM_AUDIO_TYPE_BINAURAL,
    DLB_ADM_AUDIO_TYPE_FIRST_CUSTOM = 0x1000,
    DLB_ADM_AUDIO_TYPE_LAST_CUSTOM = 0xffff
} DLB_ADM_AUDIO_TYPE;


/* ADM entities */

typedef enum
{
    DLB_ADM_ENTITY_TYPE_ILLEGAL = -1,
    DLB_ADM_ENTITY_TYPE_VOID,
    DLB_ADM_ENTITY_TYPE_TOPLEVEL,
    DLB_ADM_ENTITY_TYPE_XML,
    DLB_ADM_ENTITY_TYPE_FIRST = DLB_ADM_ENTITY_TYPE_XML,

    /* Entities with ADM identifiers */
    DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
    DLB_ADM_ENTITY_TYPE_FIRST_WITH_ID = DLB_ADM_ENTITY_TYPE_FRAME_FORMAT,
    DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT,
    DLB_ADM_ENTITY_TYPE_PROGRAMME,
    DLB_ADM_ENTITY_TYPE_CONTENT,
    DLB_ADM_ENTITY_TYPE_OBJECT,
    DLB_ADM_ENTITY_TYPE_PACK_FORMAT,
    DLB_ADM_ENTITY_TYPE_STREAM_FORMAT,
    DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT,
    DLB_ADM_ENTITY_TYPE_TRACK_FORMAT,
    DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT,
    DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET,
    DLB_ADM_ENTITY_TYPE_TRACK_UID,
    DLB_ADM_ENTITY_TYPE_LAST_WITH_ID = DLB_ADM_ENTITY_TYPE_TRACK_UID,

    /* Component entities (no ADM identifier) */
    DLB_ADM_ENTITY_TYPE_ITU_ADM,
    DLB_ADM_ENTITY_TYPE_CORE_METADATA,
    DLB_ADM_ENTITY_TYPE_FORMAT,
    DLB_ADM_ENTITY_TYPE_FRAME,
    DLB_ADM_ENTITY_TYPE_FRAME_HEADER,
    DLB_ADM_ENTITY_TYPE_CHANGED_IDS,
    DLB_ADM_ENTITY_TYPE_AUDIO_TRACK,
    DLB_ADM_ENTITY_TYPE_AUDIO_FORMAT_EXTENDED,
    DLB_ADM_ENTITY_TYPE_PROGRAMME_LABEL,
    DLB_ADM_ENTITY_TYPE_CONTENT_LABEL,
    DLB_ADM_ENTITY_TYPE_OBJECT_LABEL,
    DLB_ADM_ENTITY_TYPE_SPEAKER_LABEL,
    DLB_ADM_ENTITY_TYPE_GAIN,
    DLB_ADM_ENTITY_TYPE_DIALOGUE,
    DLB_ADM_ENTITY_TYPE_CARTESIAN,
    DLB_ADM_ENTITY_TYPE_POSITION,
    DLB_ADM_ENTITY_TYPE_EQUATION,
    DLB_ADM_ENTITY_TYPE_DEGREE,
    DLB_ADM_ENTITY_TYPE_ORDER,
    DLB_ADM_ENTITY_TYPE_NORMALIZATION,
    DLB_ADM_ENTITY_TYPE_FREQUENCY,
    DLB_ADM_ENTITY_TYPE_LAST = DLB_ADM_ENTITY_TYPE_FREQUENCY,

    /* How many distinct values in this enum? */
    DLB_ADM_ENTITY_TYPE_COUNT

} DLB_ADM_ENTITY_TYPE;


/* ADM entity attributes */

typedef enum
{
    DLB_ADM_TAG_UNKNOWN,

    /* xml */
    DLB_ADM_TAG_XML_VERSION,
    DLB_ADM_TAG_XML_ENCODING,

    /* ituADM */
    DLB_ADM_TAG_ITU_ADM_XMLNS,

    /* frameFormat */
    DLB_ADM_TAG_FRAME_FORMAT_ID,
    DLB_ADM_TAG_FIRST = DLB_ADM_TAG_FRAME_FORMAT_ID,
    DLB_ADM_TAG_FRAME_FORMAT_TYPE,
    DLB_ADM_TAG_FRAME_FORMAT_START,
    DLB_ADM_TAG_FRAME_FORMAT_DURATION,
    DLB_ADM_TAG_FRAME_FORMAT_TIME_REFERENCE,
    DLB_ADM_TAG_FRAME_FORMAT_FLOW_ID,

    /* transportTrackFormat */
    DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_ID,
    DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NAME,
    DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_IDS,
    DLB_ADM_TAG_TRANSPORT_TRACK_FORMAT_NUM_TRACKS,

    /* audioProgramme */
    DLB_ADM_TAG_PROGRAMME_ID,
    DLB_ADM_TAG_PROGRAMME_NAME,
    DLB_ADM_TAG_PROGRAMME_LANGUAGE,

    /* audioContent */
    DLB_ADM_TAG_CONTENT_ID,
    DLB_ADM_TAG_CONTENT_NAME,
    DLB_ADM_TAG_CONTENT_LANGUAGE,

    /* audioObject */
    DLB_ADM_TAG_OBJECT_ID,
    DLB_ADM_TAG_OBJECT_NAME,
    DLB_ADM_TAG_OBJECT_L_START,
    DLB_ADM_TAG_OBJECT_L_DURATION,

    /* audioTrackUID */
    DLB_ADM_TAG_TRACK_UID_UID,
    DLB_ADM_TAG_TRACK_UID_SAMPLE_RATE,
    DLB_ADM_TAG_TRACK_UID_BIT_DEPTH,

    /* audioPackFormat */
    DLB_ADM_TAG_PACK_FORMAT_ID,
    DLB_ADM_TAG_PACK_FORMAT_NAME,
    DLB_ADM_TAG_PACK_FORMAT_TYPE_LABEL,
    DLB_ADM_TAG_PACK_FORMAT_TYPE_DEFINITION,

    /* audioStreamFormat */
    DLB_ADM_TAG_STREAM_FORMAT_ID,
    DLB_ADM_TAG_STREAM_FORMAT_NAME,
    DLB_ADM_TAG_STREAM_FORMAT_FORMAT_LABEL,
    DLB_ADM_TAG_STREAM_FORMAT_FORMAT_DEFINITION,

    /* audioChannelFormat */
    DLB_ADM_TAG_CHANNEL_FORMAT_ID,
    DLB_ADM_TAG_CHANNEL_FORMAT_NAME,
    DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_LABEL,
    DLB_ADM_TAG_CHANNEL_FORMAT_TYPE_DEFINITION,

    /* audioTrackFormat */
    DLB_ADM_TAG_TRACK_FORMAT_ID,
    DLB_ADM_TAG_TRACK_FORMAT_NAME,
    DLB_ADM_TAG_TRACK_FORMAT_FORMAT_LABEL,
    DLB_ADM_TAG_TRACK_FORMAT_FORMAT_DEFINITION,

    /* audioBlockFormat */
    DLB_ADM_TAG_BLOCK_FORMAT_ID,
    DLB_ADM_TAG_BLOCK_FORMAT_RTIME,
    DLB_ADM_TAG_BLOCK_FORMAT_DURATION,

    /* alternativeValueSet */
    DLB_ADM_TAG_ALT_VALUE_SET_ID,

    /* audioTrack */
    DLB_ADM_TAG_AUDIO_TRACK_ID,

    /* audioFormatExtended */
    DLB_ADM_TAG_AUDIO_FORMAT_EXT_VERSION,

    /* audioProgrammeLabel */
    DLB_ADM_TAG_PROGRAMME_LABEL_VALUE,
    DLB_ADM_TAG_PROGRAMME_LABEL_LANGUAGE,

    /* audioContentLabel */
    DLB_ADM_TAG_CONTENT_LABEL_VALUE,
    DLB_ADM_TAG_CONTENT_LABEL_LANGUAGE,

    /* audioObjectLabel */
    DLB_ADM_TAG_OBJECT_LABEL_VALUE,
    DLB_ADM_TAG_OBJECT_LABEL_LANGUAGE,

    /* gain */
    DLB_ADM_TAG_SPEAKER_GAIN_VALUE,
    DLB_ADM_TAG_SPEAKER_GAIN_UNIT,

    /* speakerLabel */
    DLB_ADM_TAG_SPEAKER_LABEL_VALUE,

    /* dialogue */
    DLB_ADM_TAG_DIALOGUE_VALUE,
    DLB_ADM_TAG_DIALOGUE_NON_DIALOGUE_KIND,
    DLB_ADM_TAG_DIALOGUE_DIALOGUE_KIND,
    DLB_ADM_TAG_DIALOGUE_MIXED_KIND,

    /* cartesian */
    DLB_ADM_TAG_CARTESIAN_VALUE,

    /* position */
    DLB_ADM_TAG_POSITION_VALUE,
    DLB_ADM_TAG_POSITION_COORDINATE,
    DLB_ADM_TAG_POSITION_SCREEN_EDGE_LOCK,

    /* equation */
    DLB_ADM_TAG_EQUATION_VALUE,

    /* degree */
    DLB_ADM_TAG_DEGREE_VALUE,

    /* order */
    DLB_ADM_TAG_ORDER_VALUE,

    /* normalization */
    DLB_ADM_TAG_NORMALIZATION_VALUE,
        
    /* frequency */
    DLB_ADM_TAG_FREQUENCY_VALUE,
    DLB_ADM_TAG_FREQUENCY_TYPE_DEFINITION,

    DLB_ADM_TAG_LAST = DLB_ADM_TAG_FREQUENCY_TYPE_DEFINITION

} DLB_ADM_TAG;

typedef enum
{
    DLB_ADM_SPEAKER_CONFIG_NONE,
    DLB_ADM_SPEAKER_CONFIG_MONO,
    DLB_ADM_SPEAKER_CONFIG_2_0,
    DLB_ADM_SPEAKER_CONFIG_3_0,
    DLB_ADM_SPEAKER_CONFIG_5_1,
    DLB_ADM_SPEAKER_CONFIG_5_1_2,
    DLB_ADM_SPEAKER_CONFIG_5_1_4,
    DLB_ADM_SPEAKER_CONFIG_7_0_4,
    DLB_ADM_SPEAKER_CONFIG_7_1_4,
    DLB_ADM_SPEAKER_CONFIG_9_1_6,
    DLB_ADM_SPEAKER_CONFIG_CUSTOM,
    DLB_ADM_SPEAKER_CONFIG_LAST_BED = DLB_ADM_SPEAKER_CONFIG_CUSTOM,
    DLB_ADM_SPEAKER_CONFIG_PORTABLE,
    DLB_ADM_SPEAKER_CONFIG_HEADPHONE,
    DLB_ADM_SPEAKER_CONFIG_LAST = DLB_ADM_SPEAKER_CONFIG_HEADPHONE,
    DLB_ADM_SPEAKER_CONFIG_COUNT
} DLB_ADM_SPEAKER_CONFIG;

typedef enum
{
    DLB_ADM_OBJECT_CLASS_NONE,
    DLB_ADM_OBJECT_CLASS_DIALOG,
    DLB_ADM_OBJECT_CLASS_VDS,
    DLB_ADM_OBJECT_CLASS_VOICEOVER,
    DLB_ADM_OBJECT_CLASS_GENERIC,
    DLB_ADM_OBJECT_CLASS_SUBTITLE,
    DLB_ADM_OBJECT_CLASS_EMERGENCY_ALERT,
    DLB_ADM_OBJECT_CLASS_EMERGENCY_INFO,
    DLB_ADM_OBJECT_CLASS_LAST = DLB_ADM_OBJECT_CLASS_EMERGENCY_INFO,
    DLB_ADM_OBJECT_CLASS_COUNT
} DLB_ADM_OBJECT_CLASS;

typedef enum
{
    DLB_ADM_CONTENT_KIND_NK,                    /* non-dialogue kind */
    DLB_ADM_CONTENT_KIND_NK_UNDEFINED = DLB_ADM_CONTENT_KIND_NK,
    DLB_ADM_CONTENT_KIND_NK_MUSIC,
    DLB_ADM_CONTENT_KIND_NK_EFFECTS,
    DLB_ADM_CONTENT_KIND_DK = 10,               /* dialogue kind */
    DLB_ADM_CONTENT_KIND_DK_UNDEFINED = DLB_ADM_CONTENT_KIND_DK,
    DLB_ADM_CONTENT_KIND_DK_DIALOGUE,
    DLB_ADM_CONTENT_KIND_DK_VOICEOVER,
    DLB_ADM_CONTENT_KIND_DK_SUBTITLE,
    DLB_ADM_CONTENT_KIND_DK_DESCRIPTION,
    DLB_ADM_CONTENT_KIND_DK_COMMENTARY,
    DLB_ADM_CONTENT_KIND_DK_EMERGENCY,
    DLB_ADM_CONTENT_KIND_MK = 20,               /* mixed kind */
    DLB_ADM_CONTENT_KIND_MK_UNDEFINED = DLB_ADM_CONTENT_KIND_MK,
    DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN,
    DLB_ADM_CONTENT_KIND_MK_MIXED,
    DLB_ADM_CONTENT_KIND_MK_HEARING_IMPAIRED,

    DLB_ADM_CONTENT_KIND_UNKNOWN

} DLB_ADM_CONTENT_KIND;

typedef enum
{
    DLB_ADM_VALUE_TYPE_BOOL,
    DLB_ADM_VALUE_TYPE_UINT,
    DLB_ADM_VALUE_TYPE_INT,
    DLB_ADM_VALUE_TYPE_FLOAT,
    DLB_ADM_VALUE_TYPE_AUDIO_TYPE,
    DLB_ADM_VALUE_TYPE_TIME,
    DLB_ADM_VALUE_TYPE_STRING,
} DLB_ADM_VALUE_TYPE;

typedef struct
{
    uint8_t          hours;
    uint8_t          minutes;
    uint8_t          seconds;

    uint32_t         fraction_numerator;
    uint32_t         fraction_denominator;

} dlb_adm_time;

#define DLB_ADM_STRING_VALUE_BUFFER_MIN_SIZE (24)


/* ADM container counts */

typedef struct
{
    int                  placeholder;
} dlb_adm_container_counts;


/* ADM core model counts */

typedef struct
{
    int                  placeholder;
} dlb_adm_core_model_counts;


/* XML Writer */

/**
 * @brief callback that is intended to retrieve more write buffer
 *
 * This will be invoked by the xml writing routine whenever it needs
 * more buffer, and also to deliver the final buffer for writing.  In
 * the End of File case, buf should be NULL indicating that no new
 * buffers are required.
 */
typedef
int  /** @return 1 on success, 0 on failure */
(*dlb_adm_write_buffer_callback)
    (void    *arg           /**< [in] client-supplied parameter */
    ,char    *pos           /**< [in] current write position of previous buffer */
    ,char   **buf           /**< [out] start of next buffer position, NULL for final write */
    ,size_t  *capacity      /**< [out] capacity of next buffer */
    );


/* API data types */

typedef enum
{
    DLB_ADM_COORDINATE_X,
    DLB_ADM_COORDINATE_AZIMUTH = DLB_ADM_COORDINATE_X,
    DLB_ADM_COORDINATE_Y,
    DLB_ADM_COORDINATE_ELEVATION = DLB_ADM_COORDINATE_Y,
    DLB_ADM_COORDINATE_Z,
    DLB_ADM_COORDINATE_DISTANCE = DLB_ADM_COORDINATE_Z,
    DLB_ADM_COORDINATE_LAST = DLB_ADM_COORDINATE_DISTANCE,
    DLB_ADM_COORDINATE_COUNT
} DLB_ADM_COORDINATE;

typedef enum
{
    DLB_ADM_GAIN_UNIT_LINEAR,
    DLB_ADM_GAIN_UNIT_FIRST = DLB_ADM_GAIN_UNIT_LINEAR,
    DLB_ADM_GAIN_UNIT_DB,
    DLB_ADM_GAIN_UNIT_LAST = DLB_ADM_GAIN_UNIT_DB,
    DLB_ADM_GAIN_UNIT_COUNT
} DLB_ADM_GAIN_UNIT;

typedef float       dlb_adm_gain_value;

typedef uint16_t    dlb_adm_source_group_id;
typedef uint8_t     dlb_adm_channel_number;

typedef uint32_t    dlb_adm_sample_rate;
typedef uint8_t     dlb_adm_bit_depth;


#ifdef __cplusplus
}
#endif

#endif /* DLB_ADM_API_TYPES_H */
