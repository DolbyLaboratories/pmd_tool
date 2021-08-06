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

#ifndef DLB_ADM_DATA_TYPES_H
#define DLB_ADM_DATA_TYPES_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#define DLB_ADM_DATA_NAME_LEN_MIN (3)
#define DLB_ADM_DATA_NAME_SZ_MIN (DLB_ADM_DATA_NAME_LEN_MIN + 1)

#define DLB_ADM_DATA_NAME_LEN (67)
#define DLB_ADM_DATA_NAME_SZ (DLB_ADM_DATA_NAME_LEN + 1)

#define DLB_ADM_DATA_LANG_LEN (3)
#define DLB_ADM_DATA_LANG_SZ (DLB_ADM_DATA_LANG_LEN + 1)

#define DLB_ADM_DATA_SPEAKER_LABEL_LEN (31)
#define DLB_ADM_DATA_SPEAKER_LABEL_SZ (DLB_ADM_DATA_SPEAKER_LABEL_LEN + 1)

#define DLB_ADM_DATA_FF_UUID_LEN (36)
#define DLB_ADM_DATA_FF_UUID_SZ (DLB_ADM_DATA_FF_UUID_LEN + 1)

#define DLB_ADM_DATA_FF_TYPE_LEN (12)
#define DLB_ADM_DATA_FF_TYPE_SZ (DLB_ADM_DATA_FF_TYPE_LEN + 1)

/*
 * Time format is hh:mm:ss.<samples>S<rate> where samples and rate are five to nine characters each:
 *
 * 00:03:45.01536S48000     zero hours, three minutes, forty-five seconds, 1536 samples at 48kHz sample rate
 * 01:59:00.048000S192000   one hour, fifty-nine minutes, zero seconds, 48,000 samples at 192kHz sample rate
 *   3 +3 +3  +9 +1  +9
 */
#define DLB_ADM_DATA_FF_TIME_LEN (3 + 3 + 3 + 9 + 1 + 9)
#define DLB_ADM_DATA_FF_TIME_SZ (DLB_ADM_DATA_FF_TIME_LEN + 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t    dlb_adm_channel_count;  /* We need at least [0, 256] so uint8_t won't work */
typedef uint8_t     dlb_adm_element_count;  /* The number of audio elements in a presentation */

typedef struct
{
    dlb_adm_gain_value       gain_value;
    DLB_ADM_GAIN_UNIT        gain_unit;
} dlb_adm_data_gain;

typedef struct 
{
    char                   **names;
    char                   **langs;
    unsigned int             name_limit;
    unsigned int             name_count;
    unsigned int             label_count;
    unsigned int             max_name_size;

    unsigned int             string_storage_size;
    char                    *string_storage;
} dlb_adm_data_names;

typedef struct
{
    dlb_adm_entity_id        id;
    dlb_adm_bool             cartesian;
    dlb_adm_float            position[DLB_ADM_COORDINATE_COUNT];
    dlb_adm_data_gain        gain;
    dlb_adm_bool             has_time;
    dlb_adm_time             start_time;
    dlb_adm_time             duration;
} dlb_adm_data_block_update;

typedef struct
{
    dlb_adm_entity_id        id;
    dlb_adm_source_group_id  group_id;
    dlb_adm_channel_number   channel;
} dlb_adm_data_source;

typedef struct
{
    dlb_adm_entity_id        id;
    dlb_adm_source_group_id  group_id;
    char                     name[DLB_ADM_DATA_NAME_SZ];
} dlb_adm_data_source_group;

typedef struct
{
    dlb_adm_entity_id        id;
    DLB_ADM_AUDIO_TYPE       audio_type;
    char                     speaker_label[DLB_ADM_DATA_SPEAKER_LABEL_SZ];
} dlb_adm_data_target;

typedef struct
{
    dlb_adm_entity_id        id;
    DLB_ADM_SPEAKER_CONFIG   speaker_config;        /**< DLB_ADM_SPEAKER_CONFIG_NONE for an object */
    DLB_ADM_OBJECT_CLASS     object_class;          /**< DLB_ADM_OBJECT_CLASS_NONE for a bed */
    dlb_adm_bool             is_dynamic;            /**< For an object, are dynamic updates allowed? */
} dlb_adm_data_target_group;

typedef struct
{
    dlb_adm_entity_id        id;
    dlb_adm_sample_rate      sample_rate;
    dlb_adm_bit_depth        bit_depth;
} dlb_adm_data_audio_track;

typedef struct
{
    dlb_adm_entity_id        id;
    dlb_adm_data_gain        gain;
} dlb_adm_data_audio_element;

typedef dlb_adm_data_audio_element dlb_adm_data_element_group;

typedef struct
{
    dlb_adm_entity_id        id;
    DLB_ADM_CONTENT_KIND     content_kind;
} dlb_adm_data_content_group;

typedef struct
{
    dlb_adm_entity_id        id;
} dlb_adm_data_presentation;

typedef struct
{
    dlb_adm_entity_id        id;
    char                     type    [DLB_ADM_DATA_FF_TYPE_SZ];
    char                     start   [DLB_ADM_DATA_FF_TIME_SZ];
    char                     duration[DLB_ADM_DATA_FF_TIME_SZ];
    char                     flow_id [DLB_ADM_DATA_FF_UUID_SZ];
} dlb_adm_data_frame_format;

/**
 * @brief All the parts for one audio element.
 */
typedef struct
{
    dlb_adm_data_audio_element   audio_element;
    dlb_adm_data_target_group    target_group;
    dlb_adm_data_target         *targets;
    dlb_adm_data_audio_track    *audio_tracks;
    dlb_adm_data_source_group    source_group;
    dlb_adm_data_source         *sources;
    dlb_adm_data_block_update   *block_updates;

    dlb_adm_channel_count        channel_count;
    dlb_adm_channel_count        channel_capacity;
    uint8_t                     *array_storage;

} dlb_adm_data_audio_element_data;


/**
 * @brief All the parts for one presentation.
 */
typedef struct
{
    dlb_adm_data_presentation    presentation;
    dlb_adm_data_content_group  *content_groups;
    dlb_adm_data_element_group  *element_groups;
    dlb_adm_data_audio_element  *audio_elements;

    dlb_adm_element_count        element_count;
    dlb_adm_element_count        element_capacity;
    uint8_t                     *array_storage;

} dlb_adm_data_presentation_data;

#ifdef __cplusplus
}
#endif

#endif /* DLB_ADM_DATA_TYPES_H */
