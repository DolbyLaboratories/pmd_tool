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

#include "dlb_adm/include/dlb_adm_fast_api.h"
#include "dlb_adm/include/dlb_adm_api.h"

#include "dlb_adm/src/core_model/Presentation.h"
#include "dlb_adm/src/core_model/dlb_adm_core_model.h"

static const char CONTENT_GROUP_TEMPLATE[DLB_ADM_DATA_NAME_SZ] = "audioContent_%u";
static const char AUDIO_ELEMENT_TEMPLATE[DLB_ADM_DATA_NAME_SZ] = "audioObject_%u";
static const char TARGET_GROUP_TEMPLATE[DLB_ADM_DATA_NAME_SZ] = "TargetGroup";
static const char SOURCE_GROUP_TEMPLATE[DLB_ADM_DATA_NAME_SZ] = "serialized ADM";
static const unsigned int SOURCE_GROUP_ID = 1;

static
int
source_group_count_callback
    (const dlb_adm_core_model   *model
    ,dlb_adm_entity_id           entity_id
    ,void                       *callback_arg
    )
{
    dlb_adm_entity_id *group_id = (dlb_adm_entity_id *)callback_arg;
    
    *group_id = entity_id;

    return DLB_ADM_STATUS_OK;
}

static
DLB_ADM_OBJECT_CLASS
translate_content_kind_to_obj_class
    (DLB_ADM_CONTENT_KIND        content_kind
    )
{
    DLB_ADM_OBJECT_CLASS c = DLB_ADM_OBJECT_CLASS_GENERIC;

    switch (content_kind)
    {
    case DLB_ADM_CONTENT_KIND_NK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_NK_MUSIC:
    case DLB_ADM_CONTENT_KIND_NK_EFFECTS:
        c = DLB_ADM_OBJECT_CLASS_GENERIC;
        break;

    case DLB_ADM_CONTENT_KIND_DK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_DK_DIALOGUE:
    case DLB_ADM_CONTENT_KIND_DK_COMMENTARY:
        c = DLB_ADM_OBJECT_CLASS_DIALOG;
        break;

    case DLB_ADM_CONTENT_KIND_DK_VOICEOVER:
        c = DLB_ADM_OBJECT_CLASS_VOICEOVER;
        break;

    case DLB_ADM_CONTENT_KIND_DK_SUBTITLE:
        c = DLB_ADM_OBJECT_CLASS_SUBTITLE;
        break;

    case DLB_ADM_CONTENT_KIND_DK_DESCRIPTION:
        c = DLB_ADM_OBJECT_CLASS_VDS;
        break;

    case DLB_ADM_CONTENT_KIND_DK_EMERGENCY:
        c = DLB_ADM_OBJECT_CLASS_EMERGENCY_ALERT;
        break;

    case DLB_ADM_CONTENT_KIND_MK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN:
    case DLB_ADM_CONTENT_KIND_MK_MIXED:
    case DLB_ADM_CONTENT_KIND_MK_HEARING_IMPAIRED:
    case DLB_ADM_CONTENT_KIND_MK_VISUALLY_IMPAIRED:
        c = DLB_ADM_OBJECT_CLASS_GENERIC;
        break;

    default:
        break;
    }

    return c;
}

static
int
add_entity_name
    (const char          *entity_name_template
    ,unsigned int         index
    ,const char          *language_tag
    ,dlb_adm_data_names  *names
    )
{
    char name[DLB_ADM_DATA_NAME_SZ];
    memset(&name, 0, sizeof(name));
    snprintf(name, sizeof(name), entity_name_template, index);
    int status = dlb_adm_core_model_clear_names(names);
    if (DLB_ADM_STATUS_OK != status)
    {
        return DLB_ADM_STATUS_ERROR;
    }
    status = dlb_adm_core_model_add_name(names, name, language_tag);
    return status;   
}

/* Get Audio Pack Format from Common definition */
static
dlb_adm_entity_id
get_common_def_target_group
    (DLB_ADM_SPEAKER_CONFIG  speaker_config
    )
{
    switch(speaker_config)
    {
        case DLB_ADM_SPEAKER_CONFIG_2_0:
            return 0x801000200000000; /* AP_00010002 */
        case DLB_ADM_SPEAKER_CONFIG_5_1:
            return 0x801000300000000;  /* AP_00010003 */                  
        case DLB_ADM_SPEAKER_CONFIG_5_1_4:
            return 0x801000500000000; /* AP_00010005 */
        default:
            return DLB_ADM_NULL_ENTITY_ID;
    }
}

static
dlb_adm_entity_id
get_common_def_target
    (DLB_ADM_SPEAKER_CONFIG  speaker_config
    ,unsigned int            channel_idx
    )
{

    /* AC_00010001, AC_00010002 */
    static const dlb_adm_entity_id ids_20[2] = {0xA01000100000000, 0xA01000200000000};

    /* AC_00010001, AC_00010002, AC_00010003, AC_00010004, AC_00010005, AC_00010006  */
    static const dlb_adm_entity_id ids_51[6] = {0xA01000100000000
                                               ,0xA01000200000000
                                               ,0xA01000300000000
                                               ,0xA01000400000000
                                               ,0xA01000500000000
                                               ,0xA01000600000000
                                               };

    /* AC_00010001, AC_00010002, AC_00010003, AC_00010004, AC_00010005
    , AC_00010006, AC_0001000D, AC_0001000F, AC_00010010, AC_00010012  */
    static const dlb_adm_entity_id ids_514[10] = {0xA01000100000000
                                                 ,0xA01000200000000
                                                 ,0xA01000300000000
                                                 ,0xA01000400000000
                                                 ,0xA01000500000000
                                                 ,0xA01000600000000
                                                 ,0xA01000D00000000
                                                 ,0xA01000F00000000
                                                 ,0xA01001000000000
                                                 ,0xA01001200000000
                                                 };  

    switch(speaker_config)
    {
        case DLB_ADM_SPEAKER_CONFIG_2_0:
            if ((channel_idx) >= 2)
            {
                return DLB_ADM_NULL_ENTITY_ID;
            }
            return ids_20[channel_idx];
        case DLB_ADM_SPEAKER_CONFIG_5_1:
            if ((channel_idx) >= 6)
            {
                return DLB_ADM_NULL_ENTITY_ID;
            }
            return ids_51[channel_idx];                  
        case DLB_ADM_SPEAKER_CONFIG_5_1_4:
            if ((channel_idx) >= 10)
            {
                return DLB_ADM_NULL_ENTITY_ID;
            }
            return ids_514[channel_idx];
        default:
            return DLB_ADM_STATUS_NOT_FOUND;
    }
    
    return DLB_ADM_STATUS_OK;
}

static
int
add_target_group
    (dlb_adm_core_model        *model
    ,DLB_ADM_SPEAKER_CONFIG     speaker_config
    ,bool                       is_bed
    ,char                       language_tag[DLB_ADM_DATA_LANG_SZ]
    ,dlb_adm_data_target_group *target_group
    ,dlb_adm_data_names         *names    
    )
{
    target_group->speaker_config = is_bed ? speaker_config : DLB_ADM_SPEAKER_CONFIG_NONE;
    target_group->audio_type = is_bed ? DLB_ADM_AUDIO_TYPE_NONE : DLB_ADM_AUDIO_TYPE_OBJECTS;
    int status = dlb_adm_core_model_add_target_group(model, target_group, names);
    return status;
}

static
int
add_target
    (dlb_adm_core_model         *model
    ,bool                       is_bed
    ,char                       language_tag[DLB_ADM_DATA_LANG_SZ]
    ,dlb_adm_data_target        *target
    ,dlb_adm_data_names         *names    
    )
{
    target->audio_type = is_bed ? DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS : DLB_ADM_AUDIO_TYPE_OBJECTS;
    int status = dlb_adm_core_model_add_target(model, target, names);
    if (DLB_ADM_STATUS_OK != status)
    {
        return status;
    }

    dlb_adm_data_block_update block_update;
    memset(&block_update, 0, sizeof(block_update));
    block_update.cartesian = DLB_ADM_TRUE;
    block_update.position[DLB_ADM_COORDINATE_X] = 0;
    block_update.position[DLB_ADM_COORDINATE_Y] = 1;
    block_update.position[DLB_ADM_COORDINATE_Z] = 0;
    block_update.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    block_update.gain.gain_value = 0.0;
    status = dlb_adm_core_model_add_block_update(model, target->id, &block_update);

    return status;
}


dlb_adm_element_ids
dlb_adm_add_audio_element
    (dlb_adm_core_model         *model
    ,DLB_ADM_SPEAKER_CONFIG      speaker_config
    ,DLB_ADM_CONTENT_KIND        content_kind
    ,char                        language_tag[DLB_ADM_DATA_LANG_SZ]
    ,unsigned int               *sources
    ,unsigned int                num_sources
    ,dlb_adm_data_names         *names  
    )
{
    dlb_adm_element_ids element_ids {DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID};
    if ((model == nullptr) || (sources == nullptr) || (names == nullptr))
    {
        return element_ids;
    }

    /* TODO num sources and speaker config check */

    size_t number_of_audio_elements = 0;
    int status = dlb_adm_core_model_count_entities(model, DLB_ADM_ENTITY_TYPE_OBJECT, &number_of_audio_elements);
    if (DLB_ADM_STATUS_OK != status)
    {
        return element_ids;
    }

    bool is_bed = (num_sources > 1);

    dlb_adm_data_content_group content_group;
    memset(&content_group, 0, sizeof(content_group));
    content_group.content_kind = content_kind;

    status = add_entity_name(CONTENT_GROUP_TEMPLATE, number_of_audio_elements + 1, language_tag, names);
    if (DLB_ADM_STATUS_OK != status)
    {
        return element_ids;
    }
    status = dlb_adm_core_model_add_content_group(model, &content_group, names);
    if (DLB_ADM_STATUS_OK != status)
    {
        return element_ids;
    }

    dlb_adm_data_audio_element audio_element;
    memset(&audio_element, 0, sizeof(audio_element));
    audio_element.object_class = translate_content_kind_to_obj_class(content_kind);
    audio_element.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    audio_element.gain.gain_value = 0;  
    status = add_entity_name(AUDIO_ELEMENT_TEMPLATE, number_of_audio_elements + 1, language_tag, names);
    if (DLB_ADM_STATUS_OK != status)
    {
        return element_ids;
    }

    status = dlb_adm_core_model_add_audio_element(model, &audio_element, names);
    if (DLB_ADM_STATUS_OK != status)
    {
        return element_ids;
    }

    /* add AudioPackFormat*/
    dlb_adm_data_target_group target_group;
    memset(&target_group, 0, sizeof(target_group));
    target_group.id = is_bed ? get_common_def_target_group(speaker_config) : DLB_ADM_NULL_ENTITY_ID;

    /* if AudioPackFormat is not from common definitions it should be added */
    if (DLB_ADM_NULL_ENTITY_ID == target_group.id)
    {
        status = add_target_group(model, speaker_config, is_bed, language_tag, &target_group, names);
        if (DLB_ADM_STATUS_OK != status)
        {
            return element_ids;
        }        
    }

    /* Add FrameFormat */
    dlb_adm_data_source_group source_group;
    memset(&source_group, 0, sizeof(source_group));
    source_group.group_id = SOURCE_GROUP_ID;
    status = dlb_adm_core_model_for_each_entity_id(model, DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, source_group_count_callback, &source_group.id);
    if (source_group.id == 0)
    {
        snprintf(source_group.name, sizeof(source_group.name), "%s", SOURCE_GROUP_TEMPLATE);
        status = dlb_adm_core_model_add_source_group(model, &source_group);
        if (DLB_ADM_STATUS_OK != status)
        {
            return element_ids;
        } 
    }

    /* TODO what if already exists*/
    for (unsigned int i = 0; i < num_sources; ++i)
    {
        /* AudioChannelFormat */
        dlb_adm_data_target target;
        memset(&target, 0, sizeof(target));
        target.id = is_bed ? get_common_def_target(speaker_config, i) : DLB_ADM_NULL_ENTITY_ID;

        if (DLB_ADM_NULL_ENTITY_ID == target.id)
        {
            status = add_target(model, is_bed, language_tag, &target, names);
            if (DLB_ADM_STATUS_OK != status)
            {
                return element_ids;
            }        
        }  

        dlb_adm_data_audio_track audio_track;
        memset(&audio_track, 0, sizeof(audio_track));
        status = dlb_adm_core_model_add_audio_track(model, &audio_track);
        if (DLB_ADM_STATUS_OK != status)
        {
            return element_ids;
        }

        dlb_adm_data_source source;
        memset(&source, 0, sizeof(source));
        source.channel = sources[i];
        source.group_id = source_group.group_id;
        status = dlb_adm_core_model_add_source(model, &source);
        if (DLB_ADM_STATUS_OK != status)
        {
            return element_ids;
        }

        status = dlb_adm_core_model_add_source_relation(model, source_group.id, source.id, audio_track.id);    
        if (DLB_ADM_STATUS_OK != status)
        {
            return element_ids;
        }

        status = dlb_adm_core_model_add_element_relation(model, audio_element.id, target_group.id, target.id, audio_track.id);
        if (DLB_ADM_STATUS_OK != status)
        {
            return element_ids;
        }

    }

    element_ids.element_id = audio_element.id;
    element_ids.content_group_id = content_group.id;
    
    return element_ids;
}  

dlb_adm_entity_id
dlb_adm_add_audio_programme
    (dlb_adm_core_model         *model
    ,char                        programme_name[DLB_ADM_DATA_NAME_SZ]
    ,char                        language_tag[DLB_ADM_DATA_LANG_SZ]
    ,dlb_adm_presentation_label *labels
    ,unsigned int                num_labels
    ,dlb_adm_data_names         *names
    )
{
    if  (   model == nullptr
        ||  labels == nullptr
        ||  names == nullptr
        )
    {
        return DLB_ADM_NULL_ENTITY_ID;
    }

    dlb_adm_core_model_clear_names(names);
    dlb_adm_core_model_add_name(names, programme_name, language_tag);

    for (unsigned int i = 0; i < num_labels; i++)
    {
        dlb_adm_core_model_add_label(names, labels[i].presentation_label, labels[i].language_tag);
    }

    dlb_adm_data_presentation presentation;
    memset(&presentation, 0, sizeof(presentation));
    presentation.id = DLB_ADM_NULL_ENTITY_ID;

    int status = dlb_adm_core_model_add_presentation(model, &presentation, names);
    return status == DLB_ADM_STATUS_OK ? presentation.id : DLB_ADM_NULL_ENTITY_ID;
}

int
dlb_adm_add_element_to_programme
    (dlb_adm_core_model         *model
    ,dlb_adm_entity_id           audio_programm_id
    ,dlb_adm_entity_id           audio_content_id
    ,dlb_adm_entity_id           audio_element_id
    )
{
    return dlb_adm_core_model_add_presentation_relation
        ( model
        , audio_programm_id
        , audio_content_id
        , DLB_ADM_NULL_ENTITY_ID
        , audio_element_id
        , DLB_ADM_NULL_ENTITY_ID
        , DLB_ADM_NULL_ENTITY_ID
        );
}    
