/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021-2024, Dolby Laboratories Inc.
 * Copyright (c) 2021-2024, Dolby International AB.
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

#include "pmd_core_model_ingester.h"
#include "pmd_speaker_position.h"
#include "dlb_adm/include/dlb_adm_api.h"

#include <stdio.h>

#ifdef NDEBUG
#define FAILURE PMD_FAIL
#define CHECK_STATUS(s)         if ((s) != DLB_ADM_STATUS_OK) return (s)
#else
static int ret_status(int s)
{
    return s;           // Put a breakpoint here
}
static dlb_pmd_success ret_fail()
{
    return PMD_FAIL;    // Put a breakpoint here
}
#define FAILURE ret_fail()
#define CHECK_STATUS(s)         if ((s) != DLB_ADM_STATUS_OK) return ret_status(s)
#endif

#define CHECK_STATUS_SUCCESS(s) if ((s) != DLB_ADM_STATUS_OK) return FAILURE
#define CHECK_SUCCESS(s)        if ((s) != PMD_SUCCESS) return FAILURE

#define PMD_MAX_BED_CHANNELS (16)


/**
 * @brief table of speaker counts for ADM speaker configs
 */
static size_t SPEAKER_CONFIG_COUNT[] =
{
    /* DLB_ADM_SPEAKER_CONFIG_NONE */        0,
    /* DLB_ADM_SPEAKER_CONFIG_MONO */        1,
    /* DLB_ADM_SPEAKER_CONFIG_2_0 */         2,
    /* DLB_ADM_SPEAKER_CONFIG_3_0 */         3,
    /* DLB_ADM_SPEAKER_CONFIG_5_1 */         6,
    /* DLB_ADM_SPEAKER_CONFIG_5_1_2 */       8,
    /* DLB_ADM_SPEAKER_CONFIG_5_1_4 */      10,
    /* DLB_ADM_SPEAKER_CONFIG_7_0_4 */      11,
    /* DLB_ADM_SPEAKER_CONFIG_7_1_4 */      12,
    /* DLB_ADM_SPEAKER_CONFIG_9_1_6 */      16,
    /* DLB_ADM_SPEAKER_CONFIG_CUSTOM */      0,     /* "Custom" could be any number, we can't predict */
    /* DLB_ADM_SPEAKER_CONFIG_PORTABLE */    2,
    /* DLB_ADM_SPEAKER_CONFIG_HEADPHONE */   2
};

struct pmd_core_model_ingester
{
    const dlb_adm_core_model            *core_model;
    dlb_pmd_model                       *pmd_model;
    dlb_adm_data_presentation_data       presentation_data;
    dlb_adm_data_audio_element_data      element_data;
    dlb_adm_data_names                   names;
};

static
int
ingest_signal
    (const dlb_adm_core_model   *core_model
    ,const dlb_adm_data_source  *source
    ,void                       *callback_arg
    )
{
    dlb_pmd_model *pmd_model = (dlb_pmd_model *)callback_arg;
    int status = DLB_ADM_STATUS_OK;
    dlb_pmd_success success;

    if ((core_model == NULL) || (source == NULL) || (pmd_model == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    success = dlb_pmd_add_signal(pmd_model, source->channel);
    if (success != PMD_SUCCESS)
    {
        status = DLB_ADM_STATUS_ERROR;
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
    }

    return status;
}

static
dlb_pmd_success
ingest_signals
    (pmd_core_model_ingester    *ingester
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    int status;

    status = dlb_adm_core_model_for_each_source(ingester->core_model, ingest_signal, ingester->pmd_model);
    if (status != DLB_ADM_STATUS_OK)
    {
        success = PMD_FAIL;
#ifndef NDEBUG
        CHECK_SUCCESS(success);
#endif
    }

    return success;
}

static
void
translate_coordinates(dlb_pmd_coordinate *x
                     ,dlb_pmd_coordinate *y
                     ,dlb_pmd_coordinate *z
                     ,const dlb_adm_data_block_update *update
                     ,const dlb_adm_data_position_offset *offset
                     )
{
    static const double PI_OVER_180 = 3.1415926535897932384626433832795 / 180.0;
    if (update->cartesian)
    {
        *x = update->position[DLB_ADM_COORDINATE_X];
        *y = update->position[DLB_ADM_COORDINATE_Y];
        *z = update->position[DLB_ADM_COORDINATE_Z];
    }
    else
    {
        double xy_plane;
        float distance;

        if (update->position[DLB_ADM_COORDINATE_DISTANCE] == 0.0f)
        {
            /* If the distance is zero it means it was not set
             * so we should set it to default which is 1
             */
            distance = 1.0f;
        }
        else
        {
            distance = update->position[DLB_ADM_COORDINATE_DISTANCE];
        }
        xy_plane = fabs(distance * cos(update->position[DLB_ADM_COORDINATE_ELEVATION] * PI_OVER_180));

        *x = (float)(xy_plane * (sin(update->position[DLB_ADM_COORDINATE_AZIMUTH]   * PI_OVER_180) * -1.0));
        *y = (float)(xy_plane *  cos(update->position[DLB_ADM_COORDINATE_AZIMUTH]   * PI_OVER_180));
        *z = (float)(distance *  sin(update->position[DLB_ADM_COORDINATE_ELEVATION] * PI_OVER_180));
    }

    if (*x == 0 && *y == 1 && *z ==0)
    {
        *x = offset->cartesian ? offset->offset_value : (sin(offset->offset_value   * PI_OVER_180) * -1.0);
    }
}

static
void
translate_object_class
    (dlb_pmd_object_class   *pmd_object_class
    ,DLB_ADM_OBJECT_CLASS    adm_object_class
    )
{
    switch (adm_object_class)
    {
        case DLB_ADM_OBJECT_CLASS_DIALOG:
            *pmd_object_class = PMD_CLASS_DIALOG;
            break;
        case DLB_ADM_OBJECT_CLASS_VDS:
            *pmd_object_class = PMD_CLASS_VDS;
            break;
        case DLB_ADM_OBJECT_CLASS_VOICEOVER:
            *pmd_object_class = PMD_CLASS_VOICEOVER;
            break;
        case DLB_ADM_OBJECT_CLASS_SUBTITLE:
            *pmd_object_class = PMD_CLASS_SUBTITLE;
            break;
        case DLB_ADM_OBJECT_CLASS_EMERGENCY_ALERT:
            *pmd_object_class = PMD_CLASS_EMERGENCY_ALERT;
            break;
        case DLB_ADM_OBJECT_CLASS_EMERGENCY_INFO:
            *pmd_object_class = PMD_CLASS_EMERGENCY_INFO;
            break;
        default:
            *pmd_object_class = PMD_CLASS_GENERIC;
            break;
    }
}

/**
 * @brief Translate an ADM AudioElement entity ID to be used as a PMD element ID
 */
static
dlb_pmd_success
translate_audio_element_id
    (dlb_pmd_element_id     *pmd_element_id         /**< [out] PMD element ID */
    ,dlb_adm_entity_id       adm_audio_element_id   /**< [in]  ADM AudioElement entity ID */
    )
{
    char id_string[32];
    unsigned int tmp;
    int status;

    status = dlb_adm_write_entity_id(id_string, sizeof(id_string), adm_audio_element_id);
    CHECK_STATUS_SUCCESS(status);
    if ((1 != sscanf(id_string, "AO_%x", &tmp)) || (tmp < 0x1001) || (tmp > 0x1fff))
    {
        printf("Illegal object id \"%s\"\n", id_string);
        return FAILURE;
    }
    *pmd_element_id = (dlb_pmd_element_id)(tmp - 0x1000u);

    return PMD_SUCCESS;
}

static
dlb_pmd_success
generate_pmd_object
    (pmd_core_model_ingester    *ingester
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    dlb_adm_data_audio_element_data *data = &ingester->element_data;
    dlb_pmd_object obj;
    int status;

    memset(&obj, 0, sizeof(obj));
    status = dlb_adm_core_model_get_names(ingester->core_model, &ingester->names, data->audio_element.id);
    CHECK_STATUS_SUCCESS(status);
    success = translate_audio_element_id(&obj.id, data->audio_element.id);
    CHECK_SUCCESS(success);

    /*
     * Note: for a mono bed, which we translate into a PMD object, we won't
     * have an object class to translate.  Theoretically, we could translate
     * the "content kind" from a ContentGroup containing the AudioElement,
     * but that is not easily available to us here, and may produce different
     * results for different presentations.  Therefore, we assume that a mono
     * bed is of class "dialog".  TODO: is this adequate?
     *
     * One use case where this might not be correct is for an old program with
     * mono audio, where we would want a "complete main" object class, if such
     * existed.  But it is not clear we can actually describe such a program
     * in PMD.
     */
    if (data->targets[0].audio_type == DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS)
    {
        obj.object_class = PMD_CLASS_DIALOG;
    } 
    else
    {
        translate_object_class(&obj.object_class, data->audio_element.object_class);
    }

    translate_coordinates(&obj.x, &obj.y, &obj.z, &data->block_updates[0], &data->audio_element.position_offset);

    obj.dynamic_updates = (dlb_pmd_bool)data->target_group.is_dynamic;
    obj.size            = 0.0f;
    obj.size_3d         = 0;
    obj.diverge         = 0;
    obj.source          = (dlb_pmd_signal)data->sources[0].channel;
    obj.source_gain     = (dlb_pmd_gain)dlb_adm_add_gains_in_decibels(data->block_updates[0].gain, data->audio_element.gain); /* TODO: gain from audio_element or from block_update?  or both?? */

    if (ingester->names.name_count > 0)
    {
        strncpy(obj.name, ingester->names.names[0], sizeof(obj.name));
    }
    /* TODO: we will lose any labels -- is there a way to fix that? */

    if (dlb_pmd_set_object(ingester->pmd_model, &obj))
    {
        printf("Error: failed to generate PMD object for audioObject \"%s\"\n", (char*)obj.name);
        return FAILURE;
    }

    return success;
}

static
dlb_pmd_success
translate_speaker_config
    (dlb_pmd_speaker_config     *pmd_config
    ,DLB_ADM_SPEAKER_CONFIG      adm_config
    )
{
    *pmd_config = DLB_PMD_SPEAKER_CONFIG_NONE;

    switch (adm_config)
    {
    case DLB_ADM_SPEAKER_CONFIG_2_0:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_2_0;
        break;

    case DLB_ADM_SPEAKER_CONFIG_3_0:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_3_0;
        break;

    case DLB_ADM_SPEAKER_CONFIG_5_1:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_5_1;
        break;

    case DLB_ADM_SPEAKER_CONFIG_5_1_2:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_5_1_2;
        break;

    case DLB_ADM_SPEAKER_CONFIG_5_1_4:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_5_1_4;
        break;

    case DLB_ADM_SPEAKER_CONFIG_7_0_4:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_7_1_4;
        break;

    case DLB_ADM_SPEAKER_CONFIG_7_1_4:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_7_1_4;
        break;

    case DLB_ADM_SPEAKER_CONFIG_9_1_6:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_9_1_6;
        break;

    case DLB_ADM_SPEAKER_CONFIG_PORTABLE:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_PORTABLE;
        break;

    case DLB_ADM_SPEAKER_CONFIG_HEADPHONE:
        *pmd_config = DLB_PMD_SPEAKER_CONFIG_HEADPHONE;
        break;

    default:
        return FAILURE;
    }

    return PMD_SUCCESS;
}

static
dlb_pmd_success
generate_pmd_bed
    (pmd_core_model_ingester    *ingester
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    dlb_adm_data_audio_element_data *data = &ingester->element_data;
    dlb_pmd_source sources[PMD_MAX_BED_CHANNELS];
    dlb_pmd_bool alt_spkrs;
    dlb_pmd_bed bed;
    int status;
    size_t i;

    if (data->channel_count != SPEAKER_CONFIG_COUNT[data->target_group.speaker_config])
    {
        return FAILURE;
    }

    memset(&bed, 0, sizeof(bed));
    memset(&sources, 0, sizeof(sources));
    status = dlb_adm_core_model_get_names(ingester->core_model, &ingester->names, data->audio_element.id);
    CHECK_STATUS_SUCCESS(status);
    success = translate_audio_element_id(&bed.id, data->audio_element.id);
    CHECK_SUCCESS(success);
    success = translate_speaker_config(&bed.config, data->target_group.speaker_config);
    CHECK_SUCCESS(success);
    alt_spkrs = (bed.config > DLB_PMD_SPEAKER_CONFIG_5_1_4);

    bed.bed_type = PMD_BED_ORIGINAL;
    bed.num_sources = (uint8_t)data->channel_count;
    bed.sources = sources;

    if (ingester->names.name_count > 0)
    {
        strncpy(bed.name, ingester->names.names[0], sizeof(bed.name));
    }
    /* TODO: we will lose any labels -- is there a way to fix that? */

    for (i = 0; i < data->channel_count; i++)
    {
        success = find_speaker_position(&sources[i].target, &data->block_updates[i], alt_spkrs);
        CHECK_SUCCESS(success);
        sources[i].source = (dlb_pmd_signal)data->sources[i].channel;
        /*check later if that correct to add gains from block update and audio element*/
        sources[i].gain = (dlb_pmd_gain)dlb_adm_add_gains_in_decibels(data->block_updates[i].gain, data->audio_element.gain);
    }

    if (dlb_pmd_set_bed(ingester->pmd_model, &bed))
    {
        printf("Error: failed to generate PMD bed for audioObject \"%s\"\n", bed.name);
        return FAILURE;
    }

    return PMD_SUCCESS;
}

static
int
audio_element_callback
    (const dlb_adm_core_model   *model
    ,dlb_adm_entity_id           entity_id
    ,void                       *callback_arg
    )
{
    pmd_core_model_ingester *ingester = (pmd_core_model_ingester *)callback_arg;
    int status;

    status = dlb_adm_core_model_get_element_data(&ingester->element_data, model, entity_id);
    CHECK_STATUS(status);
    if (ingester->element_data.channel_count == 0)
    {
        status = DLB_ADM_STATUS_ERROR;
    }
    else
    {
        switch (ingester->element_data.targets[0].audio_type)
        {
        case DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS:
            if (ingester->element_data.channel_count == 1)
            {
                if (generate_pmd_object(ingester))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            } 
            else
            {
                if (generate_pmd_bed(ingester))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            }
            break;

        case DLB_ADM_AUDIO_TYPE_OBJECTS:
            if (ingester->element_data.channel_count == 1)
            {
                if (generate_pmd_object(ingester))
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            }
            else
            {
                status = DLB_ADM_STATUS_ERROR;
            }
            break;

        default:
            status = DLB_ADM_STATUS_ERROR;
            break;
        }
    }
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif

    return status;
}

static
dlb_pmd_success
ingest_content
    (pmd_core_model_ingester    *ingester
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    int status;

    status = dlb_adm_core_model_for_each_audio_element_id(ingester->core_model, audio_element_callback, ingester);
    if (status != DLB_ADM_STATUS_OK)
    {
        success = PMD_FAIL;
#ifndef NDEBUG
        CHECK_SUCCESS(success);
#endif
    }

    return success;
}

/**
 * @brief Translate an ADM Presentation entity ID to be used as a PMD presentation ID
 */
static
dlb_pmd_success
translate_presentation_id
    (dlb_pmd_presentation_id    *pmd_presentation_id    /**< [out] PMD presentation ID */
    ,dlb_adm_entity_id           adm_presentation_id    /**< [in]  ADM Presentation entity ID */
    )
{
    char id_string[32];
    unsigned int tmp;
    int status;

    status = dlb_adm_write_entity_id(id_string, sizeof(id_string), adm_presentation_id);
    CHECK_STATUS_SUCCESS(status);
    if ((1 != sscanf(id_string, "APR_%x", &tmp)) || (tmp < 0x1001) || (tmp > 0x11ff))
    {
        printf("Illegal object id \"%s\"\n", id_string);
        return FAILURE;
    }
    *pmd_presentation_id = (dlb_pmd_presentation_id)(tmp - 0x1000u);

    return PMD_SUCCESS;
}

static
dlb_pmd_bool
config_is_bed
    (dlb_pmd_speaker_config config
    )
{
    return config <= DLB_PMD_SPEAKER_CONFIG_LAST_BED;
}

static
void
reconcile_presentation_config
    (pmd_core_model_ingester    *ingester
    ,dlb_pmd_presentation       *presentation
    ,size_t                      element_idx
    )
{
    dlb_pmd_bed bed;
    dlb_pmd_source sources[PMD_MAX_BED_CHANNELS];

    if ((!dlb_pmd_bed_lookup(ingester->pmd_model, presentation->elements[element_idx], &bed, PMD_MAX_BED_CHANNELS, sources)) &&
        (presentation->config != bed.config))
    {
        dlb_pmd_bool use_larger = PMD_FALSE;

        /* presentation config is uninitialized, use the bed config */
        if (presentation->config == DLB_PMD_SPEAKER_CONFIG_NONE)
        {
            presentation->config = bed.config;
        }
        /* if both are beds, or both are not beds, use the larger value */
        else if (( config_is_bed(presentation->config) &&  config_is_bed(bed.config)) ||
                 (!config_is_bed(presentation->config) && !config_is_bed(bed.config)))
        {
            use_larger = PMD_TRUE;
        }
        /* if the current object's format is a bed, and the presentation's is not, use the bed */
        else if (config_is_bed(bed.config))
        {
            presentation->config = bed.config;
        }

        if (use_larger && bed.config > presentation->config)
        {
            /* TODO: this is a little bothersome for portable v. headphones... */
            presentation->config = bed.config;
        }
    }
}

static
const char *
reconcile_language
    (const char *language
    )
{
    const char *l = language;

    if (language == NULL || language[0] == '\0')
    {
        l = "und";
    }

    return l;
}

static
dlb_pmd_bool
presentations_name_language_conflicts_labels_language(dlb_adm_data_names *names)
{
    dlb_pmd_bool ret = PMD_FALSE;

    if(names == NULL)
    {
        return ret;
    }

    for(size_t i = 1; i <= names->label_count; i++)
    {
        if(strcmp(names->langs[0], names->langs[i]) == 0)
        {
            ret = PMD_TRUE;
            break;
        }
    }

    return ret;
}

static
dlb_pmd_success
generate_pmd_presentation
    (pmd_core_model_ingester    *ingester
    )
{
    dlb_pmd_success success;
    const char *audio_language = NULL;
    dlb_adm_data_presentation_data *data = &ingester->presentation_data;
    dlb_pmd_element_id elements[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_presentation pres;
    dlb_adm_bool has_name;
    size_t i, j, n;
    int status;

    memset(&pres, 0, sizeof(pres));
    memset(elements, 0, sizeof(elements));
    success = translate_presentation_id(&pres.id, ingester->presentation_data.presentation.id);
    CHECK_SUCCESS(success);

    pres.config = DLB_PMD_SPEAKER_CONFIG_NONE;  /* as we add elements, we'll set this to a specific config */
    pres.num_elements = data->element_count;
    pres.elements = elements;
    for (i = 0; i < data->element_count; i++)
    {
        success = translate_audio_element_id(&elements[i], data->audio_elements[i].id);
        CHECK_SUCCESS(success);
        reconcile_presentation_config(ingester, &pres, i);
    }

    status = dlb_adm_core_model_get_names(ingester->core_model, &ingester->names, data->presentation.id);
    CHECK_STATUS_SUCCESS(status);
    n = ingester->names.name_count;
    if (n > DLB_PMD_MAX_PRESENTATION_NAMES)
    {
        n = DLB_PMD_MAX_PRESENTATION_NAMES;
    }
    pres.num_names = (unsigned int)n;
    i = 0;
    if  (presentations_name_language_conflicts_labels_language(&ingester->names))
    {
        pres.num_names--;
        i++;
    }
    for (j = 0; j < pres.num_names; i++, j++)
    {
        strncpy(pres.names[j].text,                        ingester->names.names[i],  sizeof(pres.names[j].text));
        strncpy(pres.names[j].language, reconcile_language(ingester->names.langs[i]), sizeof(pres.names[j].language));
    }

    status = dlb_adm_core_model_has_name(&has_name, &ingester->names);
    if (has_name)
    {
        audio_language = ingester->names.langs[0];
    }
    strncpy(pres.audio_language, reconcile_language(audio_language), sizeof(pres.audio_language));

    if (dlb_pmd_set_presentation(ingester->pmd_model, &pres))
    {
        printf("Error: failed to add presentation\n");
        return FAILURE;
    }

    return success;
}

static
int
presentation_callback
    (const dlb_adm_core_model   *model
    ,dlb_adm_entity_id           entity_id
    ,void                       *callback_arg
    )
{
    pmd_core_model_ingester *ingester = (pmd_core_model_ingester *)callback_arg;
    int status;

    status = dlb_adm_core_model_get_presentation_data(&ingester->presentation_data, model, entity_id);
    CHECK_STATUS(status);
    if ((ingester->presentation_data.element_count == 0) ||
        (generate_pmd_presentation(ingester)))
    {
        status = DLB_ADM_STATUS_ERROR;
    }

    return status;
}

static
dlb_pmd_success
ingest_programmes
    (pmd_core_model_ingester    *ingester
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    int status;

    status = dlb_adm_core_model_for_each_entity_id(
        ingester->core_model, DLB_ADM_ENTITY_TYPE_PROGRAMME, presentation_callback, ingester);
    if (status != DLB_ADM_STATUS_OK)
    {
        success = PMD_FAIL;
#ifndef NDEBUG
        CHECK_SUCCESS(success);
#endif
    }

    return success;
}

static
dlb_pmd_success
ingest_frame_format
    (pmd_core_model_ingester    *ingester
    )
{
    char uuid[DLB_ADM_DATA_FF_UUID_SZ];
    dlb_pmd_success success = PMD_SUCCESS;
    int status;

    status = dlb_adm_core_model_get_flow_id(ingester->core_model, uuid, sizeof(uuid));
    CHECK_STATUS_SUCCESS(status);
    if (uuid[0] != '\0')
    {
        success =
            dlb_pmd_iat_add(ingester->pmd_model, 0) ||
            dlb_pmd_iat_content_id_uuid(ingester->pmd_model, uuid);
    }

    return success;
}

dlb_pmd_success
pmd_core_model_ingester_query_memory_size
    (size_t                     *sz
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    size_t total_sz = sizeof(pmd_core_model_ingester);
    size_t memory_sz;
    int status;

    if (sz == NULL)
    {
        return PMD_FAIL;
    }
    *sz = 0;

    status = dlb_adm_core_model_query_presentation_data_memory_size(&memory_sz, DLB_PMD_MAX_PRESENTATION_ELEMENTS);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    total_sz += memory_sz;

    status = dlb_adm_core_model_query_element_data_memory_size(&memory_sz, PMD_MAX_BED_CHANNELS, 0);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    total_sz += memory_sz;

    status = dlb_adm_core_model_query_names_memory_size(&memory_sz, DLB_PMD_NAME_ARRAY_SIZE, DLB_PMD_MAX_PRESENTATION_NAMES);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    total_sz += memory_sz;
    *sz = total_sz;

    return success;
}

dlb_pmd_success
pmd_core_model_ingester_open
    (pmd_core_model_ingester   **p_ingester
    ,void                       *memory
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    pmd_core_model_ingester *ingester;
    uint8_t *p = (uint8_t *)memory;
    size_t memory_sz;
    int status;

    if ((p_ingester == NULL) || (memory == NULL))
    {
        return PMD_FAIL;
    }
    *p_ingester = NULL;

    ingester = (pmd_core_model_ingester *)p;
    memset(ingester, 0, sizeof(*ingester));
    p += sizeof(*ingester);

    status = dlb_adm_core_model_query_presentation_data_memory_size(&memory_sz, DLB_PMD_MAX_PRESENTATION_ELEMENTS);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    status = dlb_adm_core_model_configure_presentation_data(&ingester->presentation_data, DLB_PMD_MAX_PRESENTATION_ELEMENTS, p);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    p += memory_sz;

    status = dlb_adm_core_model_query_element_data_memory_size(&memory_sz, PMD_MAX_BED_CHANNELS, 0);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    status = dlb_adm_core_model_configure_element_data(&ingester->element_data, PMD_MAX_BED_CHANNELS, 0, p);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    p += memory_sz;
    
    status = dlb_adm_core_model_query_names_memory_size(&memory_sz, DLB_PMD_NAME_ARRAY_SIZE, DLB_PMD_MAX_PRESENTATION_NAMES);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    status = dlb_adm_core_model_configure_names(&ingester->names, DLB_PMD_MAX_PRESENTATION_NAMES, (char *)p, memory_sz);
    if (status != DLB_ADM_STATUS_OK)
    {
        return FAILURE;
    }
    /* p += memory_sz; */

    *p_ingester = ingester;

    return success;
}

dlb_pmd_success
pmd_core_model_ingester_ingest
    (pmd_core_model_ingester    *ingester
    ,dlb_pmd_model              *pmd_model
    ,const char                 *title
    ,const dlb_adm_core_model   *core_model
    )
{
    dlb_pmd_success success = PMD_SUCCESS;

    if ((ingester == NULL) || (pmd_model  == NULL) || (title == NULL) || (core_model == NULL))
    {
        return PMD_FAIL;
    }
    ingester->core_model = core_model;
    ingester->pmd_model = pmd_model;

    if (dlb_pmd_reset(pmd_model)            ||
        dlb_pmd_set_title(pmd_model, title) ||
        ingest_signals(ingester)            ||
        ingest_content(ingester)            ||
        ingest_programmes(ingester)         ||
        ingest_frame_format(ingester))
    {
        success = PMD_FAIL;
    }

    ingester->pmd_model = NULL;
    ingester->core_model = NULL;

    return success;
}

dlb_pmd_success
pmd_core_model_ingester_close
    (pmd_core_model_ingester   **p_ingester
    )
{
    pmd_core_model_ingester *ingester;

    if ((p_ingester == NULL) || (*p_ingester == NULL))
    {
        return PMD_FAIL;
    }
    ingester = *p_ingester;
    *p_ingester = NULL;

    (void)dlb_adm_core_model_clear_element_data(&ingester->element_data);
    memset(ingester, 0, sizeof(*ingester));

    return PMD_SUCCESS;
}
