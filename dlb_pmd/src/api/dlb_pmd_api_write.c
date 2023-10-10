/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

/**
 * @file dlb_pmd_api_write.c
 * @brief implementation of write API for Dolby Professional Metadata library
 */

#include "dlb_pmd_api.h"
#include "pmd_model.h"
#include "pmd_error_helper.h"
#include "xml_hex.h"
#include "xml_uuid.h"
#include "xml_eidr.h"
#include "xml_ad_id.h"

#include <stddef.h>
#include <stdarg.h>


/**
 * @def SIGNAL_TO_CHANNEL_INDEX(s)
 * @brief convert a signal id (1-255) to a 0-based channel index
 *
 * The API numbers audio signals 1 - 255, where 1 is the first channel
 * in the PCM sample set. The internal model numbers these from 0.
 */
#define SIGNAL_TO_CHANNEL_INDEX(s) ((s)-1)


/**
 * @brief check and encode a PMD co-ordinate
 *
 * accepted values: -1.0 - 1.0
 * X: -1.0 = left, 1.0 = right
 * Y: -1.0 = front, 1.0 = back
 * Z: -1.0 = floor, 0.0 = horizon, 1.0 = ceiling
 *
 * output range 1 - 0x3ff, in units of 1/1024
 */
static 
int                       /** @return 0 on failure, 1 on success */
encode_coordinate
    (dlb_pmd_model *model /**< [in] model */
    ,float f              /**< [in] co-ordinate in range -1.0 - 1.0 */
    ,pmd_position *pos    /**< [out] co-ordinate */
    )
{
    if (f < -1.0f || f > 1.0f)
    {
        error(model, "x,y,z co-ordinates must be in range -1.0 - 1.0. not %g", f);
        return 0;
    }
    *pos = pmd_encode_position(f);
    return 1;
}


/**
 * @brief encode db to a PMD gain value
 *
 * Allowed range: 0 - 0x3f, in steps of 0.5 dB, where
 *   0x00 =  -inf dB
 *   0x01 = -25.0 dB
 *   0x33 =   0.0 dB
 *   0x3F =   6.0 dB
 */
static 
int                         /** @return 0 on failure, 1 on success */
encode_gain
    (dlb_pmd_model *model  /**< [in] model */
    ,float db              /**< [in] gain in dBFS, -25.0 to +6.0 */
    ,pmd_gain *gain        /**< [out] PMD gain */
    )
{
    if ((isinf(db) && db < 0) ||
        (db >= PMD_MIN_FINITE_GAIN_DB && db <= PMD_MAX_GAIN_DB))
    {
        *gain = pmd_db_to_gain(db);
        return 1;
    }
    error(model, "gain must be -inf or in range %f to %f, not %f",
          PMD_MIN_FINITE_GAIN_DB,
          PMD_MAX_GAIN_DB,
          db);
    return 0;
}


/**
 * @brief convert a float to a PMD size value
 *
 * Range: 0 - 31, encoding 0.0 - 1.0
 *    where 0 means 'point source'
 *    and 31 means entire field.
 */
static 
int                       /** @return 0 on failure, 1 on success */
encode_size
    (dlb_pmd_model *model /**< [in] model */
    ,float size           /**< [in] size as float 0.0 - 1.0 */
    ,pmd_size *sz         /**< [out] PMD size value */
    )
{
    if (size < 0 || size > 1)
    {
        error(model, "object sizes must be in range 0 - 1, not %g", size);
        return 0;
    }
    *sz = (pmd_size)((size * 31)+0.5);
    return 1;
}


/** ----------------------------- public api ----------------------- */


const char *
dlb_pmd_error
    (const dlb_pmd_model *model
    )
{
    if (model->error[0])
    {
        return &model->error[0];
    }
    return NULL;
}


dlb_pmd_success
dlb_pmd_set_smpte2109_sample_offset
    (dlb_pmd_model *model
    ,uint16_t sample_offset
    )
{
    FUNCTION_PROLOGUE(model);
    
    model->smpte2109.sample_offset = sample_offset;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_remap_local_tag
    (dlb_pmd_model *model
    ,uint16_t localtag
    ,const uint8_t *ul
    )
{
    pmd_smpte2109 *smpte2109;
    pmd_dynamic_tag *dtag;

    FUNCTION_PROLOGUE(model);
    smpte2109 = &model->smpte2109;
    if (smpte2109->num_dynamic_tags >= PMD_MAX_DYNAMIC_TAGS)
    {
        error(model, "too many dynamic tags");
        return PMD_FAIL;
    }

    dtag = &smpte2109->dynamic_tags[smpte2109->num_dynamic_tags];
    dtag->local_tag = localtag;
    memcpy(dtag->universal_label, ul, sizeof(dtag->universal_label));
    smpte2109->num_dynamic_tags += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_title
    (dlb_pmd_model *model
    ,const char *title
    )
{
    FUNCTION_PROLOGUE(model);
    /* use snprintf to convert C escape codes to UTF-8 */
    snprintf((char*)model->title, sizeof(model->title), "%s", title);

    if (!pmd_string_valid((char*)model->title))
    {
        error(model, "invalid character in text string");
        memset(model->title, '\0', sizeof(model->title));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_signal
    (dlb_pmd_model *model
    ,dlb_pmd_signal signal
    )
{
    dlb_pmd_metadata_count *con;
    unsigned int idx;    

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, signal, 1, 255);
    
    idx = SIGNAL_TO_CHANNEL_INDEX(signal);
    con = &model->profile.constraints.max;

    if (pmd_signals_test(&model->signals, idx))
    {
        /* already added */
        return PMD_SUCCESS;
    }
    
    if (model->num_signals >= con->num_signals)
    {
        error(model, "too many signals");
        return PMD_FAIL;
    }
    pmd_signals_add(&model->signals, idx);
    model->num_signals += 1;
    return PMD_SUCCESS;
}


/**
 * @brief helper function to check an individual constraint, and report the
 * error if failed
 */
static inline
dlb_pmd_success                 /** @return 1 on failure, 0 on success */
set_profile
    (dlb_pmd_model *model       /**< [in] model for error reporting */
    ,pmd_profile *p             /**< [in] profile structure to adjust */
    ,unsigned int profile       /**< [in] profile number */
    ,unsigned int level         /**< [in] profile level */
    )
{
    if (pmd_profile_set(p, profile, level, &model->limits))
    {
        error(model, "unknown profile/level combination: %u/%u", profile, level);
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief helper function to check an individual constraint, and report the
 * error if failed
 */
static inline
dlb_pmd_success                 /** @return 1 on failure, 0 on success */
check_entity_constraint
    (dlb_pmd_model *model       /**< [in] model to check */
    ,const char *name           /**< [in] name of entity (for error report, if any) */
    ,unsigned int count         /**< [in] actual quantity of entity */
    ,unsigned int limit         /**< [in] maximum quantity of entity as specified by constraint */
    )
{
    if (count > limit)
    {
        error(model, "Profile constraint failure: model has %u %s, "
              "but profile limit is %u",
              count, name, limit);
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


/**
 * @def CHECK_ENTITY(m, a, c, f, n)
 * @brief helper macro to cut down syntactic clutter for constraint checking
 */
#define CHECK_ENTITY(m, a, c, f, n) check_entity_constraint(m, n, a.f, c->f)


/**
 * @brief helper function to verify that the number of beds and objects fit within the constraint
 */
static inline
dlb_pmd_success                 /** @return 1 on failure, 0 on success */
check_elements
    (dlb_pmd_model *model                /**< [in] model for error report */
    ,dlb_pmd_metadata_count *actual      /**< [in] actual entity count */
    ,unsigned int max_elements           /**< [in] maximum element count */
    )
{
    if (actual->num_beds + actual->num_objects > max_elements)
    {
        error(model, "Profile constraint failure: model has %u elements (%u beds, %u objects), "
              "but profile limit is %u",
              actual->num_beds + actual->num_objects,
              actual->num_beds,
              actual->num_objects,
              max_elements);
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_profile
    (dlb_pmd_model *model
    ,unsigned int   profile
    ,unsigned int   level
    )
{
    dlb_pmd_metadata_count *max;
    dlb_pmd_metadata_count count;
    pmd_profile p;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, profile, 0, MAX_PROFILE_NUMBER);
    CHECK_INTARG(model, level,   0, MAX_PROFILE_LEVEL);
    
    max = &p.constraints.max;
    if (   set_profile(model, &p, profile, level)
        || dlb_pmd_count_entities(model, &count)
        || CHECK_ENTITY  (model,  count, max, num_signals,        "signals")
        || check_elements(model, &count, p.constraints.max_elements)
        || CHECK_ENTITY  (model,  count, max, num_updates,        "updates")
        || CHECK_ENTITY  (model,  count, max, num_presentations,  "presentations")
        || CHECK_ENTITY  (model,  count, max, num_loudness,       "loudness")
        || CHECK_ENTITY  (model,  count, max, num_eac3,           "EAC-3 encoding parameters")
        || CHECK_ENTITY  (model,  count, max, num_ed2_turnarounds,"ED2 turnarounds")
        || CHECK_ENTITY  (model,  count, max, num_headphone_desc, "Headphone descriptions")
       )
    {
        return PMD_FAIL;
    }
    
    memmove(&model->profile, &p, sizeof(pmd_profile));
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_signals
    (dlb_pmd_model *model
    ,unsigned int num_signals
    )
{
    unsigned int limit;
    uint16_t max_sigid;
    uint16_t i;
    uint16_t num;

    FUNCTION_PROLOGUE(model);
    limit = model->profile.constraints.max.num_signals;
    if (num_signals + model->num_signals > limit)
    {
        num_signals = limit - model->num_signals;
        if (!num_signals)
        {
            char suffix[128];
            pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
            error(model, "too many signals, only %u permitted%s", limit, suffix);
            return PMD_FAIL;
        }
    }
    
    num = model->num_signals;
    i = 0;
    max_sigid = 0;
    while (num)
    {
        if (pmd_signals_test(&model->signals, i))
        {
            max_sigid = i+1;
            --num;
        }
        ++i;
    }

    for (i = 0; i != num_signals; ++i)
    {
        pmd_signals_add(&model->signals, max_sigid);
        ++max_sigid;
    }

    model->num_signals += (uint16_t)num_signals;
    return PMD_SUCCESS;
}



static
dlb_pmd_success
add_element_name
    (dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,const char *name
    )
{
    pmd_aen *aen;
    uint16_t idx;

    if (!pmd_string_valid(name))
    {
        error(model, "name contains non-unicode characters");           
        return PMD_FAIL;
    }       

    idx = model->num_aen;
    aen = &model->aen_list[idx];
    model->num_aen += 1;
    aen->id = id;

    /* snprintf will convert C escape codes */
    snprintf((char*)aen->name, sizeof(aen->name), "%s", name);

    pmd_idmap_insert(&model->aen_ids, id, idx);
    return PMD_SUCCESS;
}



dlb_pmd_success
dlb_pmd_add_bed
    (dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,const char *name
    ,dlb_pmd_speaker_config cfg
    ,unsigned int first_signal
    ,int origin
    )
{
    unsigned int limit;    
    pmd_element *e;
    pmd_channel_metadata *cmd;
    pmd_track_metadata *tmd;
    unsigned int bit;
    unsigned int i;
    uint16_t idx;
    static uint8_t SPEAKER_COUNTS[NUM_PMD_SPEAKER_CONFIGS] =  {0};
    static pmd_channel_set SPEAKERS[NUM_PMD_SPEAKER_CONFIGS] = {0};
    int cfg_idx = (int)(cfg);

    switch (cfg)
    {
        case DLB_PMD_SPEAKER_CONFIG_2_0:        /* fall through */
        case DLB_PMD_SPEAKER_CONFIG_PORTABLE:   /* fall through */
        case DLB_PMD_SPEAKER_CONFIG_HEADPHONE:           
            SPEAKER_COUNTS[cfg_idx] = 2;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_STEREO;
            break;

        case DLB_PMD_SPEAKER_CONFIG_3_0:
            SPEAKER_COUNTS[cfg_idx] = 3;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_3_0;
            break;

        case DLB_PMD_SPEAKER_CONFIG_5_1:
            SPEAKER_COUNTS[cfg_idx] = 6;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_5_1;
            break;

        case DLB_PMD_SPEAKER_CONFIG_5_1_2:
            SPEAKER_COUNTS[cfg_idx] = 8;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_5_1_2;
            break;

        case DLB_PMD_SPEAKER_CONFIG_5_1_4:
            SPEAKER_COUNTS[cfg_idx] = 10;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_5_1_4;
            break;

        case DLB_PMD_SPEAKER_CONFIG_7_1_4:
            SPEAKER_COUNTS[cfg_idx] = 12;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_7_1_4;
            break;

        case DLB_PMD_SPEAKER_CONFIG_9_1_6:
            SPEAKER_COUNTS[cfg_idx] = 16;
            SPEAKERS[cfg_idx] = PMD_CHANNELSET_9_1_6;
            break;

        default:
            break;
    }
    
    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, DLB_PMD_MAX_AUDIO_ELEMENTS);

    assert(first_signal > 0);

    limit = model->profile.constraints.max_elements;

    /* check whether the ID has already been claimed */
    if (pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        error(model, "element id %u already in use", id);
        return PMD_FAIL;
    }
    else if (model->num_elements >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many elements, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }

    if (origin > 0)
    {
        uint16_t oidx;
        pmd_element *oe;
        if (!pmd_idmap_lookup(&model->element_ids, (dlb_pmd_element_id)origin, &oidx))
        {
            error(model, "bed %u original element id %d not found", id, origin);
            return PMD_FAIL;
        }
        oe = &model->element_list[oidx];
        if (oe->mode != PMD_MODE_CHANNEL)
        {
            error(model, "bed %u original element %d is not a bed", id, origin);
            return PMD_FAIL;
        }
    }

    /* if the id hasn't been used, then there must be a free slot */
    assert(model->num_elements < limit);

    e = &model->element_list[model->num_elements];
    e->id = id;
    e->mode = PMD_MODE_CHANNEL;
    e->hed_idx = 0xffff;

    cmd = &e->md.channel;
    cmd->config = cfg;
    cmd->derived = origin > 0;
    cmd->origin = (pmd_element_id)origin;
    cmd->num_tracks = SPEAKER_COUNTS[cfg];
    tmd = cmd->metadata;
    
    bit = 0;
    first_signal = SIGNAL_TO_CHANNEL_INDEX(first_signal);
    for (i = 0; i != SPEAKER_COUNTS[cfg]; ++i)
    {
        tmd->source = (uint8_t)first_signal;
        if (!encode_gain(model, 0.0, &tmd->gain)) return PMD_FAIL;
        while (!((1ul << bit) & SPEAKERS[cfg])) ++bit;
        tmd->target = (pmd_speaker)(bit+1);
        ++bit;
        ++tmd;
        ++first_signal;
    }
    pmd_bed_set_normal_form(cmd);
    pmd_idmap_insert(&model->element_ids, e->id, model->num_elements);
    model->num_elements += 1;
    model->num_abd += 1;

    /* now add element name */
    if (name && name[0])
    {
        return add_element_name(model, id, name);
    }
    else
    {
        char tmp[256];
        snprintf(tmp, sizeof(tmp), "Bed %u", id);
        return add_element_name(model, id, tmp);
    }
}


dlb_pmd_success
dlb_pmd_set_bed
    (      dlb_pmd_model *model
    ,const dlb_pmd_bed *bed
    )
{
    pmd_channel_metadata *cmd;
    pmd_track_metadata *tmd;
    dlb_pmd_bool added = 0;
    dlb_pmd_source *src;
    unsigned int limit;
    pmd_element *e;
    unsigned int i;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, bed);

    limit = model->profile.constraints.max_elements;

    /* check whether the ID has already been claimed */
    if (pmd_idmap_lookup(&model->element_ids, bed->id, &idx))
    {
        e = &model->element_list[idx];
    }
    else if (model->num_elements >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many elements, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    else
    {
        /* this is a new object */
        e = &model->element_list[model->num_elements];
        pmd_idmap_insert(&model->element_ids, bed->id, model->num_elements);
        added = 1;        
    }

    if (bed->bed_type)
    {
        uint16_t oidx = 0;
        pmd_element *oe;
        if (!pmd_idmap_lookup(&model->element_ids, bed->source_id, &oidx))
        {
            error(model, "bed %u has unknown source id %u", bed->id, bed->source_id);
            return PMD_FAIL;
        }
        oe = &model->element_list[oidx];
        if (oe->mode != PMD_MODE_CHANNEL)
        {
            error(model, "bed %u's source id %u is not a bed", bed->id, bed->source_id);
            return PMD_FAIL;
        }
    }

    e->id = bed->id;
    e->mode = PMD_MODE_CHANNEL;
    e->hed_idx = 0xffff;

    cmd = &e->md.channel;
    cmd->config = bed->config;
    cmd->derived = bed->bed_type;
    cmd->origin = bed->source_id;
    cmd->num_tracks = bed->num_sources;
    tmd = cmd->metadata;

    if (PMD_FAIL == add_element_name(model, bed->id, bed->name))
    {
        return PMD_FAIL;
    }

    src = bed->sources;
    for (i = 0; i != bed->num_sources; ++i)
    {
        tmd->source = src->source - 1;
        if (!encode_gain(model, src->gain, &tmd->gain)) return PMD_FAIL;
        tmd->target = src->target;
        ++src;
        ++tmd;
    }
    model->num_elements += added;
    model->num_abd += added;
    pmd_bed_set_normal_form(cmd);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_object
    (dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,const char *name
    ,dlb_pmd_object_class cls
    ,unsigned int signal
    ,dlb_pmd_coordinate x
    ,dlb_pmd_coordinate y
    ,dlb_pmd_coordinate z
    ,dlb_pmd_gain gain
    ,dlb_pmd_size size
    ,dlb_pmd_bool size_vertical
    ,dlb_pmd_bool dynamic_update
    ,dlb_pmd_bool diverge
    )
{
    pmd_object_metadata *omd;
    unsigned int limit;
    pmd_element *e;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, DLB_PMD_MAX_AUDIO_ELEMENTS);

    limit = model->profile.constraints.max_elements;
    
    assert(signal > 0);

    /* check whether the ID has already been claimed */
    if (pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        error(model, "element id %u already in use", id);
        return PMD_FAIL;
    }
    else if (model->num_elements >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many elements, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }

    e = &model->element_list[model->num_elements];
    e->id = id;
    e->mode = PMD_MODE_OBJECT;
    e->hed_idx = 0xffff;
    omd = &e->md.object;
    if (!encode_coordinate(model, x, &omd->x)) return PMD_FAIL;
    if (!encode_coordinate(model, diverge ? 1.0f : y, &omd->y)) return PMD_FAIL;
    if (!encode_coordinate(model, diverge ? 0.0f : z, &omd->z)) return PMD_FAIL;
    if (!encode_size(model, size, &omd->size)) return PMD_FAIL;
    if (!encode_gain(model, gain, &omd->gain)) return PMD_FAIL;
    omd->oclass = cls;
    omd->source = (pmd_track_index)SIGNAL_TO_CHANNEL_INDEX(signal);
    omd->size_vertical   = size_vertical;
    omd->dynamic_updates = dynamic_update;
    omd->diverge         = diverge;
    pmd_idmap_insert(&model->element_ids, e->id, model->num_elements);
    model->num_elements += 1;

    if (name && name[0])
    {
        return add_element_name(model, id, name);
    }
    else
    {
        char tmp[256];
        snprintf(tmp, sizeof(tmp), "Object %u", id);
        return add_element_name(model, id, tmp);
    }
}


dlb_pmd_success
dlb_pmd_set_object
    (      dlb_pmd_model *model
    ,const dlb_pmd_object *object
    )
{
    pmd_object_metadata *omd;
    dlb_pmd_bool added = 0;
    unsigned int limit;
    pmd_element *e;
    uint16_t idx;
    
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, object);

    limit = model->profile.constraints.max_elements;
    /* check whether the ID has already been claimed */
    if (pmd_idmap_lookup(&model->element_ids, object->id, &idx))
    {
        e = &model->element_list[idx];
    }
    else if (model->num_elements >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many elements, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    else
    {
        /* this is a new object */
        e = &model->element_list[model->num_elements];
        pmd_idmap_insert(&model->element_ids, object->id, model->num_elements);
        added = 1;
    }
    
    e->id = object->id;
    e->mode = PMD_MODE_OBJECT;
    e->hed_idx = 0xffff;

    if (PMD_FAIL == add_element_name(model, object->id, object->name))
    {
        return PMD_FAIL;
    }

    omd = &e->md.object;
    if (!encode_coordinate(model, object->x, &omd->x)) return PMD_FAIL;
    if (!encode_coordinate(model, object->y, &omd->y)) return PMD_FAIL;
    if (!encode_coordinate(model, object->z, &omd->z)) return PMD_FAIL;
    if (!encode_size(model, object->size, &omd->size)) return PMD_FAIL;
    if (!encode_gain(model, object->source_gain, &omd->gain)) return PMD_FAIL;
    omd->oclass = object->object_class;
    omd->source = object->source - 1;
    omd->size_vertical   = object->size_3d;
    omd->dynamic_updates = object->dynamic_updates;
    omd->diverge         = object->diverge;
        
    model->num_elements += added;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_presentation
    (dlb_pmd_model *model
    ,dlb_pmd_presentation_id id
    ,const char *lang
    ,const char *name
    ,const char *namelang
    ,dlb_pmd_speaker_config cfg
    ,int num_elements
    ,dlb_pmd_element_id *elements
    )
{
    unsigned int limit;
    pmd_apd *pres;
    uint16_t idx;
    int i;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, DLB_PMD_MAX_PRESENTATIONS);

    limit = model->profile.constraints.max.num_presentations;

    if (pmd_idmap_lookup(&model->apd_ids, id, &idx))
    {
        error(model, "presentation id %u already exists", id);
        return PMD_FAIL;
    }    

    if (model->num_apd >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many presentations, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    
    pres = &model->apd_list[model->num_apd];
    pres->num_elements = 0;
    pres->num_names = 0;
    pmd_elements_init(&pres->elements);
    pres->id = id;

    if (pmd_decode_langcode(lang, &pres->pres_lang))
    {
        error(model, "unrecognized language code \"%s\"", lang);
        return PMD_FAIL;
    }

    /* need to convert object ids into array indices */
    pres->config =  cfg;
    for (i = 0; i != num_elements; ++i)
    {
        if (!pmd_idmap_lookup(&model->element_ids, elements[i], &idx))
        {
            error(model, "unknown element %u!", elements[i]);
            return PMD_FAIL;
        }
        pmd_elements_add(&pres->elements, idx);
        pres->num_elements += 1;
    }
    
    pmd_idmap_insert(&model->apd_ids, pres->id, model->num_apd);
    model->num_apd += 1;

    return dlb_pmd_add_presentation_name(model, pres->id, namelang, name);
}


dlb_pmd_success
dlb_pmd_set_presentation
    (      dlb_pmd_model *model
    ,const dlb_pmd_presentation *p
    )
{
    dlb_pmd_bool added = 0;
    unsigned int limit;
    unsigned int i;
    pmd_apd *pres;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, p);
    CHECK_INTARG(model, p->id, 1, DLB_PMD_MAX_PRESENTATIONS);

    limit = model->profile.constraints.max.num_presentations;
    if (pmd_idmap_lookup(&model->apd_ids, p->id, &idx))
    {
        pres = &model->apd_list[idx];
    }
    else if (model->num_apd >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many presentations, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    else
    {
        pres = &model->apd_list[model->num_apd];
        pmd_idmap_insert(&model->apd_ids, p->id, model->num_apd);
        idx = model->num_apd;
        added = 1;
    }
    
    pres->num_elements = 0;
    pmd_elements_init(&pres->elements);

    pres->id = p->id;
    pres->num_names = p->num_names;
    pres->config = p->config;

    if (p->num_names >= DLB_PMD_MAX_PRESENTATION_NAMES)
    {
        error(model, "too many presentation names for presentation %u", p->id);
        return PMD_FAIL;
    }

    if (pmd_decode_langcode(p->audio_language, &pres->pres_lang))
    {
        error(model, "unrecognized language code \"%s\"", p->audio_language);
        return PMD_FAIL;
    }

    for (i = 0; i != p->num_names; ++i)
    {
        pmd_langcode langcode;
        pmd_apn *pname;
        
        if (pmd_decode_langcode(p->names[i].language, &langcode))
        {
            error(model, "unrecognized language code \"%s\"", p->names[i].language);
            return PMD_FAIL;
        }

        pname = pmd_apn_list_find(&model->apn_list, pres->id, langcode);
        if (pname)
        {
            error(model, "presentation %u cannot have two names with same language \"%s\"",
                  pres->id, p->names[i].language);
            return PMD_FAIL;
        }

        pname = pmd_apn_list_add(&model->apn_list);
        if (!pname)
        {
            error(model, "too many presentation names");
            return PMD_FAIL;
        }
        
        pname->presid = pres->id;
        pname->lang = langcode;
        memcpy(pname->text, p->names[i].text, sizeof(pname->text));
        pres->names[i] = pname->idx;
    }

    for (i = 0; i != p->num_elements; ++i)
    {
        if (!pmd_idmap_lookup(&model->element_ids, p->elements[i], &idx))
        {
            error(model, "unknown element id %u!", p->elements[i]);
            return PMD_FAIL;
        }
        pmd_elements_add(&pres->elements, idx);
        pres->num_elements += 1;
    }
        
    model->num_apd += added;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_update
    (dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,unsigned int time
    ,dlb_pmd_coordinate x
    ,dlb_pmd_coordinate y
    ,dlb_pmd_coordinate z
    )
{
    unsigned int limit;
    pmd_xyz *update;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, DLB_PMD_MAX_AUDIO_ELEMENTS);

    if (time < 5)
    {
        error(model, "update times less than 5 are ignored");
        return PMD_SUCCESS;
    }

    limit = model->profile.constraints.max.num_updates;
    if (!pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        error(model, "object %u does not exist", id);
        return PMD_FAIL;
    }
    if (model->element_list[idx].mode != PMD_MODE_OBJECT)
    {
        error(model, "object is not generic");
        return PMD_FAIL;
    }
    if (!model->element_list[idx].md.object.dynamic_updates)
    {
        error(model, "specifying an update for non updating object");
        return PMD_FAIL;
    }
    if (model->num_xyz >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many updates, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    update = &model->xyz_list[model->num_xyz];
    update->obj_idx = idx;
    update->time = time;
    if (!encode_coordinate(model, x, &update->x)) return PMD_FAIL;
    if (!encode_coordinate(model, y, &update->y)) return PMD_FAIL;
    if (!encode_coordinate(model, z, &update->z)) return PMD_FAIL;
    model->num_xyz += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_update
    (      dlb_pmd_model *model
    ,const dlb_pmd_update *u
    )
{
    unsigned int update_time;
    dlb_pmd_bool added = 0;
    unsigned int limit;
    pmd_xyz *update;
    unsigned int i;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, u);
    CHECK_INTARG(model, u->id, 1, DLB_PMD_MAX_AUDIO_ELEMENTS);
    CHECK_INTARG(model, u->sample_offset, 0, DLB_PMD_MAX_UPDATE_TIME);

    limit = model->profile.constraints.max.num_updates;
    if (!pmd_idmap_lookup(&model->element_ids, u->id, &idx))
    {
        error(model, "object %u does not exist", u->id);
        return PMD_FAIL;
    }
    if (model->element_list[idx].mode != PMD_MODE_OBJECT)
    {
        error(model, "object is not generic");
        return PMD_FAIL;
    }
    if (!model->element_list[idx].md.object.dynamic_updates)
    {
        error(model, "specifying an update for non updating object");
        return PMD_FAIL;
    }

    update_time = pmd_xyz_encode_time(u->sample_offset);
    update = model->xyz_list;
    for (i = 0; i != model->num_xyz; ++i)
    {
        if (update->obj_idx == idx && update->time == update_time)
        {
            break;
        }
    }
    
    if (i == model->num_xyz)
    {
        if (model->num_xyz >= limit)
        {
            char suffix[128];
            pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
            error(model, "too many updates, only %u permitted%s", limit, suffix);
            return PMD_FAIL;
        }
        update = &model->xyz_list[model->num_xyz];
        added = 1;
    }
    update->obj_idx = idx;
    update->time = update_time;
    
    if (!encode_coordinate(model, u->x, &update->x)) return PMD_FAIL;
    if (!encode_coordinate(model, u->y, &update->y)) return PMD_FAIL;
    if (!encode_coordinate(model, u->z, &update->z)) return PMD_FAIL;
        
    model->num_xyz += added;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_eac3_encoding_parameters
    (dlb_pmd_model *model
    ,int id
    )
{
    unsigned int limit;
    pmd_eep *eep;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);

    limit = model->profile.constraints.max.num_eac3;
    
    if (pmd_idmap_lookup(&model->eep_ids, (uint16_t)id, &idx))
    {
        error(model, "EAC3 encoding parameters id %u already exists", id);
        return PMD_FAIL;
    }
    if (model->num_eep >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many EAC3 encoding parameters, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    
    eep = &model->eep_list[model->num_eep];
    eep->id = (uint16_t)id;
    eep->options  = 0;
    eep->num_presentations = 0;
    
    pmd_idmap_insert(&model->eep_ids, eep->id, model->num_eep);
    model->num_eep += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_eep_add_encoder_params
    (dlb_pmd_model *model
    ,unsigned int   id
    ,dlb_pmd_compr  dynrng_prof
    ,dlb_pmd_compr  compr_prof
    ,dlb_pmd_bool   surround90
    ,unsigned char  hmixlev
    )
{
    pmd_eep *eep;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);

    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)id, &idx))
    {
        error(model, "EAC3 encoding parameters %u does not exist", id);
        return PMD_FAIL;
    }
    eep = &model->eep_list[idx];
    eep->options |= PMD_EEP_ENCODER_PRESENT;
    eep->dynrng_prof = dynrng_prof;
    eep->compr_prof = compr_prof;
    eep->surround90 = surround90;
    eep->hmixlev = hmixlev;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_eep_add_bitstream_params
    (dlb_pmd_model    *model
    ,unsigned int      id
    ,dlb_pmd_bsmod     bsmod
    ,dlb_pmd_surmod    dsurmod
    ,dlb_pmd_dialnorm  dialnorm
    ,dlb_pmd_prefdmix  dmixmod
    ,dlb_pmd_cmixlev   ltrtcmixlev
    ,dlb_pmd_surmixlev ltrtsurmixlev
    ,dlb_pmd_cmixlev   lorocmixlev
    ,dlb_pmd_surmixlev lorosurmixlev
    )
{
    pmd_eep *eep;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);

    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)id, &idx))
    {
        error(model, "EAC3 encoding parameters %u does not exist", id);
        return PMD_FAIL;
    }
    eep = &model->eep_list[idx];
    eep->options |= PMD_EEP_BITSTREAM_PRESENT;
    eep->bsmod = bsmod;
    eep->dsurmod = dsurmod;
    eep->dialnorm = dialnorm;
    eep->dmixmod = dmixmod;
    eep->ltrtcmixlev = ltrtcmixlev;
    eep->ltrtsurmixlev = ltrtsurmixlev;
    eep->lorocmixlev = lorocmixlev;
    eep->lorosurmixlev = lorosurmixlev;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_eep_add_drc_params
    (dlb_pmd_model *model
    ,unsigned int   id
    ,dlb_pmd_compr  port_spkr
    ,dlb_pmd_compr  port_hp
    ,dlb_pmd_compr  flat_panl
    ,dlb_pmd_compr  home_thtr
    ,dlb_pmd_compr  ddplus
    )
{
    pmd_eep *eep;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);
    
    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)id, &idx))
    {
        error(model, "EAC3 encoding parameters %u does not exist", id);
        return PMD_FAIL;
    }
    eep = &model->eep_list[idx];
    eep->options |= PMD_EEP_DRC_PRESENT;
    eep->drc_port_spkr   = port_spkr;
    eep->drc_port_hphone = port_hp;
    eep->drc_flat_panl   = flat_panl;
    eep->drc_home_thtr   = home_thtr;
    eep->drc_ddplus      = ddplus;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_eep_add_presentation
    (dlb_pmd_model *model
    ,unsigned int   id
    ,unsigned int   pres_id
    )
{
    pmd_eep *eep;
    uint16_t presidx;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);
    CHECK_INTARG(model, pres_id, 1, DLB_PMD_MAX_PRESENTATIONS);

    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)id, &idx))
    {
        error(model, "EAC3 encoding parameters %u does not exist", id);
        return PMD_FAIL;
    }
    if (!pmd_idmap_lookup(&model->apd_ids, (uint16_t)pres_id, &presidx))
    {
        error(model, "presentation %u does not exist", pres_id);
        return PMD_FAIL;
    }

    eep = &model->eep_list[idx];
    if (eep->num_presentations >= PMD_EEP_MAX_PRESENTATIONS)
    {
        error(model, "too many presentations in EAC3 encoder parameters");
        return PMD_FAIL;
    }

    eep->presentations[eep->num_presentations] = presidx;
    eep->num_presentations += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_eac3
    (      dlb_pmd_model *model
    ,const dlb_pmd_eac3 *eac3
    )
{
    dlb_pmd_bool added = 0;
    unsigned int limit;
    uint16_t presidx;
    unsigned int i;
    pmd_eep *eep;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, eac3);
    CHECK_INTARG(model, eac3->id, 1, 255);

    limit = model->profile.constraints.max.num_eac3;
    if (pmd_idmap_lookup(&model->eep_ids, (uint16_t)eac3->id, &idx))
    {
        eep = &model->eep_list[idx];
    }
    else if (model->num_eep >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many EAC3 encoder parameters, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    else
    {
        eep = &model->eep_list[model->num_eep];
        pmd_idmap_insert(&model->eep_ids, (uint16_t)eac3->id, model->num_eep);
        added = 1;
    }

    memset(eep, '\0', sizeof(*eep));

    eep->id = (uint16_t)eac3->id;
    if (eac3->b_encoder_params)
    {
        eep->options |= PMD_EEP_ENCODER_PRESENT;
        eep->dynrng_prof = eac3->dynrng_prof;
        eep->compr_prof = eac3->compr_prof;
        eep->surround90 = eac3->surround90;
        eep->hmixlev = eac3->hmixlev;
    }
    if (eac3->b_bitstream_params)
    {
        eep->options |= PMD_EEP_BITSTREAM_PRESENT;
        eep->bsmod = eac3->bsmod;
        eep->dsurmod = eac3->dsurmod;
        eep->dialnorm = eac3->dialnorm;
        eep->dmixmod = eac3->dmixmod;
        eep->ltrtcmixlev = eac3->ltrtcmixlev;
        eep->ltrtsurmixlev = eac3->ltrtsurmixlev;
        eep->lorocmixlev = eac3->lorocmixlev;
        eep->lorosurmixlev = eac3->lorosurmixlev;
    }
    if (eac3->b_drc_params)
    {
        eep->options |= PMD_EEP_DRC_PRESENT;
        eep->drc_port_spkr = eac3->drc_port_spkr;
        eep->drc_port_hphone = eac3->drc_port_hphone;
        eep->drc_home_thtr = eac3->drc_home_thtr;
        eep->drc_flat_panl = eac3->drc_flat_panl;
        eep->drc_ddplus = eac3->drc_ddplus;
    }

    for (i = 0; i != eac3->num_presentations; ++i)
    {
        dlb_pmd_presentation_id id = eac3->presentations[i];
        if (!pmd_idmap_lookup(&model->apd_ids, id, &presidx))
        {
            error(model, "presentation %u does not exist", id);
            return PMD_FAIL;
        }
        else if (eep->num_presentations >= PMD_EEP_MAX_PRESENTATIONS)
        {
            error(model, "too many presentations in EAC3 encoder parameters");
            return PMD_FAIL;
        }

        eep->presentations[eep->num_presentations] = presidx;
        eep->num_presentations += 1;
    }
        
    model->num_eep += added;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_etd
    (dlb_pmd_model *model
    ,int id
    )
{
    unsigned int limit;
    pmd_etd *etd;
    uint16_t idx;
    
    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);

    limit = model->profile.constraints.max.num_ed2_turnarounds;
    if (pmd_idmap_lookup(&model->etd_ids, (uint16_t)id, &idx))
    {
        error(model, "ED2 turnaround id %u already in use", id);
        return PMD_FAIL;
    }

    if (model->num_etd >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many turnarounds, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    
    etd = &model->etd_list[model->num_etd];
    etd->id = (uint8_t)id;
    etd->ed2_presentations = 0;
    etd->de_presentations = 0;

    pmd_idmap_insert(&model->etd_ids, etd->id, model->num_etd);
    model->num_etd += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_etd_add_ed2
    (dlb_pmd_model     *model
    ,int                id
    ,dlb_pmd_frame_rate framerate
    )
{
    pmd_etd *etd;
    uint16_t idx;
    
    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);

    if (!pmd_idmap_lookup(&model->etd_ids, (uint16_t)id, &idx))
    {
        error(model, "ED2 turnaround %u does not exist", id);
        return PMD_FAIL;
    }
    etd = &model->etd_list[idx];
    etd->ed2_presentations = 0;
    etd->ed2_framerate = framerate;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_etd_add_ed2_presentation
    (dlb_pmd_model *model
    ,unsigned int   id
    ,unsigned int   pres_id
    ,unsigned int   apm_id
    )
{
    pmd_etd *etd;
    uint16_t idx;
    turnaround *t;
    
    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);
    CHECK_INTARG(model, pres_id, 1, DLB_PMD_MAX_PRESENTATIONS);
    CHECK_INTARG(model, apm_id, 1, 255);

    if (!pmd_idmap_lookup(&model->etd_ids, (uint16_t)id, &idx))
    {
        error(model, "ED2 turnaround %u does not exist", id);
        return PMD_FAIL;
    }

    etd = &model->etd_list[idx];
    if (etd->ed2_presentations >= PMD_ETD_MAX_PRESENTATIONS)
    {
        error(model, "too many presentations for ED2 Turnaround");
        return PMD_FAIL;
    }
        
    t = &etd->ed2_turnaround[etd->ed2_presentations];
    if (!pmd_idmap_lookup(&model->apd_ids, (uint16_t)pres_id, &idx))
    {
        error(model, "presentation %u does not exist", pres_id);
        return PMD_FAIL;
    }
    t->presid = idx;

    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)apm_id, &idx))
    {
        error(model, "AC3 program metadata id %u does not exist", apm_id);
        return PMD_FAIL;
    }
    t->eepid = (uint8_t)idx;
    etd->ed2_presentations += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_etd_add_de
    (dlb_pmd_model            *model
    ,int                       id
    ,dlb_pmd_frame_rate        framerate
    ,dlb_pmd_de_program_config pgm_config
    )
{
    pmd_etd *etd;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);    
    CHECK_INTARG(model, id, 1, 255);

    if (!pmd_idmap_lookup(&model->etd_ids, (uint16_t)id, &idx))
    {
        error(model, "ED2 turnaround %u does not exist", id);
        return PMD_FAIL;
    }
    etd = &model->etd_list[idx];
    etd->de_presentations = 0;
    etd->de_framerate = framerate;
    etd->pgm_config = pgm_config;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_etd_add_de_presentation
    (dlb_pmd_model *model
    ,unsigned int  id
    ,unsigned int  pres_id
    ,unsigned int  apm_id
    )
{
    pmd_etd *etd;
    turnaround *t;
    uint16_t idx;
    
    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, 255);
    CHECK_INTARG(model, pres_id, 1, DLB_PMD_MAX_PRESENTATIONS);
    CHECK_INTARG(model, apm_id, 1, 255);

    if (!pmd_idmap_lookup(&model->etd_ids, (uint16_t)id, &idx))
    {
        error(model, "ED2 turnaround %u does not exist", id);
        return PMD_FAIL;
    }
    etd = &model->etd_list[idx];
    if (etd->de_presentations >= PMD_ETD_MAX_PRESENTATIONS)
    {
        error(model, "too many presentations for ED2 Turnaround");
        return PMD_FAIL;
    }
        
    t = &etd->de_turnaround[etd->de_presentations];
    if (!pmd_idmap_lookup(&model->apd_ids, (uint16_t)pres_id, &idx))
    {
        error(model, "presentation %u does not exist", pres_id);
        return PMD_FAIL;
    }
    t->presid = idx;
    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)apm_id, &idx))
    {
        error(model, "AC3 program metadata id %u does not exist", apm_id);
        return PMD_FAIL;
    }
    t->eepid = (uint8_t)idx;
    etd->de_presentations += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_ed2_turnaround
    (      dlb_pmd_model *model
    ,const dlb_pmd_ed2_turnaround *etd
    )
{
    const dlb_pmd_turnaround *turn;
    dlb_pmd_bool added = 0;
    unsigned int limit;
    unsigned int i;
    uint16_t pidx;
    uint16_t aidx;
    uint16_t idx;
    pmd_etd *e;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, etd);
    CHECK_INTARG(model, etd->id, 1, 255);

    limit = model->profile.constraints.max.num_ed2_turnarounds;
    if (pmd_idmap_lookup(&model->etd_ids, (uint16_t)etd->id, &idx))
    {
        e = &model->etd_list[idx];
    }
    else if (model->num_etd >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many turnarounds, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    else
    {
        e = &model->etd_list[model->num_etd];        
        pmd_idmap_insert(&model->etd_ids, (uint16_t)etd->id, model->num_etd);
        added = 1;
    }

    memset(e, '\0', sizeof(*e));

    e->id = (uint8_t)etd->id;
    e->ed2_presentations = etd->ed2_presentations;
    if (etd->ed2_presentations > 0)
    {
        if (etd->ed2_framerate > DLB_PMD_FRAMERATE_3000)
        {
            error(model, "ED2 turnarounds only concern framerates 23.98 - 30 fps");
            return PMD_FAIL;
        }
        
        e->ed2_framerate = etd->ed2_framerate;
        turn = etd->ed2_turnarounds;
        for (i = 0; i != etd->ed2_presentations; ++i)
        {
            if (!pmd_idmap_lookup(&model->apd_ids, turn->presid, &pidx))
            {
                error(model, "presentation %u does not exist", turn->presid);
                return PMD_FAIL;
            }
            if (!pmd_idmap_lookup(&model->eep_ids, turn->eepid, &aidx))
            {
                error(model, "EAC3 encoder parameters id %u does not exist", turn->eepid);
                return PMD_FAIL;
            }
            e->ed2_turnaround[i].presid = pidx;
            e->ed2_turnaround[i].eepid = (uint8_t)aidx;
            ++turn;
        }
    }
    
    if (etd->de_presentations > 0)
    {
        if (etd->de_framerate > DLB_PMD_FRAMERATE_3000)
        {
            error(model, "ED2 turnarounds only concern framerates 23.98 - 30 fps");
            return PMD_FAIL;
        }
        
        e->de_framerate = etd->de_framerate;
        e->pgm_config = etd->pgm_config;
        e->de_presentations = etd->de_presentations;
        turn = etd->de_turnarounds;
        for (i = 0; i != etd->de_presentations; ++i)
        {
            if (!pmd_idmap_lookup(&model->apd_ids, turn->presid, &pidx))
            {
                error(model, "presentation %u does not exist", turn->presid);
                return PMD_FAIL;
            }
            if (!pmd_idmap_lookup(&model->eep_ids, turn->eepid, &aidx))
            {
                error(model, "EAC3 encoder parameters id %u does not exist", turn->eepid);
                return PMD_FAIL;
            }
            e->de_turnaround[i].presid = pidx;
            e->de_turnaround[i].eepid = (uint8_t)aidx;
            ++turn;
        }
    }
    model->num_etd += added;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_add
    (dlb_pmd_model *model
    ,uint64_t       timestamp
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat) return PMD_FAIL;
    
    memset(iat, '\0', sizeof(*iat));
    iat->options = PMD_IAT_PRESENT;
    iat->timestamp = timestamp;
    iat->user_data_size = 0;
    iat->extension_size = 0;
    iat->content_id_size = 0;
    iat->distribution_id_size = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_content_id_uuid
    (dlb_pmd_model *model
    ,const char    *uuid
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    
    if (!read_uuid(uuid, iat->content_id))
    {
        error(model, "Error: could not parse UUID: \"%s\"", uuid);
        return PMD_FAIL;
    }
    
    iat->content_id_size = 16;
    iat->content_id_type = PMD_IAT_CONTENT_ID_UUID;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_content_id_eidr
    (dlb_pmd_model *model
    ,const char    *eidr
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    
    if (!read_eidr(eidr, iat->content_id))
    {
        error(model, "Error: could not parse EIDR: \"%s\"", eidr);
        return PMD_FAIL;
    }

    iat->content_id_size = 12;
    iat->content_id_type = PMD_IAT_CONTENT_ID_EIDR;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_content_id_ad_id
    (dlb_pmd_model *model
    ,const char    *ad_id
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    if (!read_ad_id(ad_id, iat->content_id))
    {
        error(model, "Error: could not parse Ad-ID: \"%s\"", ad_id);
        return PMD_FAIL;
    }
    iat->content_id_size = (uint8_t)strlen(ad_id);
    iat->content_id_type = PMD_IAT_CONTENT_ID_AD_ID;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_content_id_raw
    (dlb_pmd_model          *model
    ,dlb_pmd_content_id_type type
    ,size_t                  len
    ,uint8_t                *data
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    if (len >= PMD_IAT_CONTENT_ID_SPACE)
    {
        error(model, "Error: content Id length too long: %u", len);
        return PMD_FAIL;
    }
    if ((int)type > 0x1e || (int)type < 3)
    {
        error(model, "Error: raw content id type incorrect (3-0x1e): %u", type);
        return PMD_FAIL;
    }

    iat->content_id_size = (uint8_t)len;
    iat->content_id_type = type;
    memcpy(iat->content_id, data, len);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_distribution_id_atsc3
    (dlb_pmd_model *model
    ,uint16_t       bsid
    ,uint16_t       majno
    ,uint16_t       minno
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    if (majno > 1023)
    {
        error(model, "Error: major channel number %u too large (0-1023)", majno);
        return PMD_FAIL;
    }
    if (minno > 1023)
    {
        error(model, "Error: minor channel number %u too large (0-1023)", minno);
        return PMD_FAIL;
    }
    iat->distribution_id_type = 0;
    iat->distribution_id_size = 5;
    iat->distribution_id[0] = (bsid >> 8) & 0xff;
    iat->distribution_id[1] = bsid & 0xff;
    iat->distribution_id[2] = 0xf0 | ((majno >> 6) & 0x0f);
    iat->distribution_id[3] = ((majno & 0x3f) << 2) | ((minno >> 8) & 0x3);
    iat->distribution_id[4] = (minno & 0xff);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_distribution_id_raw
    (dlb_pmd_model *model
    ,dlb_pmd_distribution_id_type type
    ,size_t len
    ,uint8_t *data
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    if (len >= PMD_IAT_DISTRIBUTION_ID_SPACE)
    {
        error(model, "Error: distribution id length too long: %u", len);
        return PMD_FAIL;
    }
    if ((int)type > 6 || (int)type < 1)
    {
        error(model, "Error: raw distribution id type incorrect (1-6): %u", type);
        return PMD_FAIL;
    }
    iat->distribution_id_size = (uint8_t)len;
    iat->distribution_id_type = type;
    memcpy(iat->distribution_id, data, len);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_set_offset
    (dlb_pmd_model *model
    ,uint16_t       offset
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, offset, 0, (1u<<11)-1);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    
    iat->options |= PMD_IAT_OFFSET_PRESENT;
    iat->offset = offset;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_set_validity_duration
    (dlb_pmd_model *model
    ,uint16_t       vdur
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, vdur, 0, (1<<11)-1);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    
    iat->options |= PMD_IAT_VALIDITY_DUR_PRESENT;
    iat->validity_duration = vdur;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_set_user_data
    (dlb_pmd_model *model
    ,size_t         size
    ,uint8_t       *data
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    if (size > (PMD_IAT_USER_DATA_SPACE-1))
    {
        error(model, "IAT user data too long");
        return PMD_FAIL;
    }
    iat->user_data_size = (uint8_t)size;
    memcpy(iat->user_data, data, size);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_iat_set_extension
    (dlb_pmd_model *model
    ,size_t         size
    ,uint8_t       *data
    )
{
    pmd_iat *iat;

    FUNCTION_PROLOGUE(model);

    iat = model->iat;
    if (!iat || !(iat->options & PMD_IAT_PRESENT))
    {
        error(model, "Error: IAT not present");
        return PMD_FAIL;
    }
    if (size > (PMD_IAT_EXTENSION_SPACE-1))
    {
        error(model, "IAT extension data too long");
        return PMD_FAIL;
    }

    iat->extension_size = (uint8_t)size;
    memcpy(iat->extension_data, data, size);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_iat
    (dlb_pmd_model *model
    ,dlb_pmd_identity_and_timing *iat
    )
{
    pmd_iat *miat;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, iat);

    miat = model->iat;
    if (!miat)
    {
        error(model, "IAT not included in model constraint");
        return PMD_FAIL;
    }

    memset(miat, '\0', sizeof(*miat));
    if (iat)
    {
        miat->options = PMD_IAT_PRESENT;
        
        if (iat->content_id.size)
        {
            if (iat->content_id.size > PMD_CONTENT_ID_MAX_BYTES)
            {
                error(model, "IAT content id size too large");
                return PMD_FAIL;
            }
            
            miat->content_id_size = (uint8_t)iat->content_id.size;
            miat->content_id_type = iat->content_id.type;
            memcpy(miat->content_id, iat->content_id.data, iat->content_id.size);
        }
        
        if (iat->distribution_id.size)
        {
            if (iat->distribution_id.size > PMD_DISTRIBUTION_ID_MAX_BYTES)
            {
                error(model, "IAT distribution id size too large");
                return PMD_FAIL;
            }
            
            miat->distribution_id_size = (uint8_t)iat->distribution_id.size;
            miat->distribution_id_type = iat->distribution_id.type;
            memcpy(miat->distribution_id, iat->distribution_id.data,
                   iat->distribution_id.size);
        }
        miat->timestamp = iat->timestamp;
        if (iat->offset.present)
        {
            miat->options |= PMD_IAT_OFFSET_PRESENT;
            miat->offset = iat->offset.offset;
        }
        if (iat->validity_duration.present)
        {
            miat->options |= PMD_IAT_VALIDITY_DUR_PRESENT;
            miat->validity_duration = iat->validity_duration.vdur;
        }
        
        if (iat->user_data.size)
        {
            if (iat->user_data.size > PMD_IAT_USER_DATA_SPACE)
            {
                error(model, "IAT user data size too large");
                return PMD_FAIL;
            }
            miat->user_data_size = (uint8_t)iat->user_data.size;
            memcpy(miat->user_data, iat->user_data.data, miat->user_data_size);
        }
        
        if (iat->extension.size)
        {
            size_t bytes = (iat->extension.size + 7)/8;
            if (bytes > PMD_IAT_EXTENSION_SPACE)
            {
                error(model, "IAT extension size too large");
                return PMD_FAIL;
            }
            miat->extension_size = (uint8_t)bytes;
            memcpy(miat->extension_data, iat->extension.data, bytes);
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief helper function to verify a LUFS value is in the correct range
 */
static inline
dlb_pmd_success
VERIFY_LUFS
    (dlb_pmd_model *model
    ,dlb_pmd_lufs lufs
    ,const char *fieldname
    )
{
    if (lufs < DLB_PMD_LUFS_MIN || lufs > DLB_PMD_LUFS_MAX)
    {
        error(model, "%s should be between %f and %f, not %f",
              fieldname, DLB_PMD_LUFS_MIN, DLB_PMD_LUFS_MAX, lufs);
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_loudness
    (      dlb_pmd_model    *model
    ,const dlb_pmd_loudness *p
    )
{
    unsigned int limit;
    uint16_t presidx;
    pmd_pld *pld;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, p);

    limit = model->profile.constraints.max.num_loudness;
    if (!pmd_idmap_lookup(&model->apd_ids, p->presid, &presidx))
    {
        error(model, "presentation %u does not exist", p->presid);
        return PMD_FAIL;
    }
    if (model->num_pld >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many loudness payloads, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }
    pld = &model->pld_list[model->num_pld];

    memset(pld, '\0', sizeof(*pld));
    pld->presid = presidx;

    if (p->loud_prac_type > PMD_PLD_LOUDNESS_PRACTICE_CONSUMER_LEVELLER)
    {
        error(model, "Unknown loudness practice type: %u", p->loud_prac_type);
        return PMD_FAIL;
    }
    pld->lpt = (pmd_loudness_practice)p->loud_prac_type;

    if (p->loudcorr_type > PMD_PLD_CORRECTION_REALTIME)
    {
        error(model, "Unknown loudness correction type: %u", p->loudcorr_type);
        return PMD_FAIL;
    }
    pld->options |= PMD_PLD_OPT_LOUDCORR_TYPE;
    pld->corrty = (pmd_correction_type)p->loudcorr_type;

    if (p->b_loudcorr_gating)
    {
        if (p->loudcorr_gating > PMD_PLD_GATING_PRACTICE_RESERVED_07)
        {
            error(model, "Unknown loudness correction gating: %u", p->loudcorr_gating);
            return PMD_FAIL;
        }
        pld->options |= PMD_PLD_OPT_LOUDCORR_DIALGATE;
        pld->dpt = (pmd_dialgate_practice)p->loudcorr_gating;
    }

    if (p->b_loudrelgat)
    {
        if (VERIFY_LUFS(model, p->loudrelgat, "relative-gated loudness")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_LOUDRELGAT;
        pld->lrg = pmd_encode_lufs(p->loudrelgat);
    }

    if (p->b_loudspchgat)
    {
        if (VERIFY_LUFS(model, p->loudspchgat, "speech-gated loudness")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_LOUDSPCHGAT;
        pld->lsg = pmd_encode_lufs(p->loudspchgat);
        pld->sdpt = (pmd_dialgate_practice)p->loudspch_gating;
    }

    if (p->b_loudstrm3s)
    {
        if (VERIFY_LUFS(model, p->loudstrm3s, "3-second loudness")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_LOUDSTRM3S;
        pld->l3g = pmd_encode_lufs(p->loudstrm3s);
    }

    if (p->b_max_loudstrm3s)
    {
        if (VERIFY_LUFS(model, p->max_loudstrm3s, "max 3-secondloudness")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_MAX_LOUDSTRM3S;        
        pld->l3g_max = pmd_encode_lufs(p->max_loudstrm3s);
    }

    if (p->b_truepk)
    {
        if (VERIFY_LUFS(model, p->truepk, "true-peak")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_TRUEPK;
        pld->tpk = pmd_encode_lufs(p->truepk);
    }

    if (p->b_max_truepk)
    {
        if (VERIFY_LUFS(model, p->max_truepk, "maximum true-peak")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_MAX_TRUEPK;
        pld->tpk_max = pmd_encode_lufs(p->max_truepk);        
    }

    if (p->b_prgmbndy)
    {
        if (!(p->prgmbndy >= 1 && p->prgmbndy <= 9)
            && !(p->prgmbndy >= -9 && p->prgmbndy <= -1))
        {
            error(model, "loudness program boundary value should be +/-[1-9], not %u",
                  p->prgmbndy);
            return PMD_FAIL;
        }

        pld->options |= PMD_PLD_OPT_PRGMBNDY;
        pld->prgmbndy = p->prgmbndy;

        if (p->b_prgmbndy_offset)
        {
            if (p->prgmbndy_offset > 2048)
            {
                error(model, "loudness program boundary offset can be no larger than 2048, not %u",
                      p->prgmbndy_offset);
                return PMD_FAIL;
            }
            pld->options |= PMD_PLD_OPT_PRGMBNDY_OFFSET;
            pld->prgmbndy_offset = p->prgmbndy_offset;
        }
    }

    if (p->b_lra)
    {
        if (p->lra < DLB_PMD_LU_MIN || p->lra > DLB_PMD_LU_MAX)
        {
            error(model, "Loudness range must be between %f and %f, not %f",
                  DLB_PMD_LU_MIN, DLB_PMD_LU_MAX, p->lra);
            return PMD_FAIL;
        }

        pld->options |= PMD_PLD_OPT_LRA;
        pld->lra = pmd_encode_lra(p->lra);
        pld->lrap = (pmd_loudness_range_practice)p->lra_prac_type;
    }

    if (p->b_loudmntry)
    {
        if (VERIFY_LUFS(model, p->loudmntry, "momentary loudness")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_LOUDMNTRY;
        pld->ml = pmd_encode_lufs(p->loudmntry);
    }

    if (p->b_max_loudmntry)
    {
        if (VERIFY_LUFS(model, p->max_loudmntry, "maximum momentary loudness")) return PMD_FAIL;
        pld->options |= PMD_PLD_OPT_MAX_LOUDMNTRY;        
        pld->ml_max = pmd_encode_lufs(p->max_loudmntry);
    }
    
    pld->extension_bits = (uint8_t)p->extension.size;
    memcpy(pld->extension, p->extension.data, sizeof(pld->extension));

    model->num_pld += 1;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_ed2_system
    (      dlb_pmd_model *model 
    ,const dlb_pmd_ed2_system *sys
    )
{
    pmd_esd *esd;
    unsigned int i;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, sys);
    
    esd = model->esd;
    if (!esd)
    {
        error(model, "ED2 system excluded from current model constraints");
        return PMD_FAIL;
    }
           
    memset(esd, '\0', sizeof(*esd));

    esd->count = sys->count;
    esd->rate = sys->rate;
    for (i = 0; i != sys->count; ++i)
    {
        esd->streams[i].config = sys->streams[i].config;
        esd->streams[i].compression = sys->streams[i].compression;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_add_presentation_name
    (dlb_pmd_model           *model
    ,dlb_pmd_presentation_id  id
    ,const char              *lang
    ,const char              *name
    )
{
    pmd_apn *pname;
    pmd_apd *pres;
    pmd_langcode langcode;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_INTARG(model, id, 1, DLB_PMD_MAX_PRESENTATIONS);

    if (!pmd_string_valid(name))
    {
        error(model, "invalid character in presentation name");
        return PMD_FAIL;
    }

    if (!pmd_idmap_lookup(&model->apd_ids, id, &idx))
    {
        error(model, "presentation id %u does not exist", id);
        return PMD_FAIL;
    }    

    pres = &model->apd_list[idx];
    if (pres->num_names >= DLB_PMD_MAX_PRESENTATION_NAMES)
    {
        error(model, "too many presentation names for presentation %u", id);
        return PMD_FAIL;
    }

    if (pmd_decode_langcode(lang, &langcode))
    {
        error(model, "unrecognized language code \"%s\"", lang);
        return PMD_FAIL;
    }

    pname = pmd_apn_list_find(&model->apn_list, id, langcode);
    if (!pname)
    {
        pname = pmd_apn_list_add(&model->apn_list);
        if (!pname)
        {
            return PMD_FAIL;
        }
        pres->names[pres->num_names] = pname->idx;
        pres->num_names += 1;
    }

    pname->presid = id;
    pname->lang = langcode;
    /* snprintf will convert C escape codes */
    snprintf((char*)pname->text, sizeof(pname->text), "%s", (char*)name);
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_headphone_element
    (      dlb_pmd_model *model
    ,const dlb_pmd_headphone *hed
    )
{
    dlb_pmd_headphone *target;
    unsigned int limit;
    pmd_element *e;
    uint16_t idx;
    
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, hed);

    limit = model->profile.constraints.max.num_headphone_desc;
    if (!pmd_idmap_lookup(&model->element_ids, hed->audio_element_id, &idx))
    {
        error(model, "headphone audio element id %u does not exist",
              hed->audio_element_id);
        return PMD_FAIL;
    }
    else if (model->num_hed >= limit)
    {
        char suffix[128];
        pmd_profile_error_info(&model->profile, suffix, sizeof(suffix));
        error(model, "too many headphone descriptions, only %u permitted%s", limit, suffix);
        return PMD_FAIL;
    }

    e = &model->element_list[idx];
    if (e->hed_idx != 0xffff)
    {
        error(model, "element id %u already has a headphone description", e->id);
        return PMD_FAIL;
    }

    e->hed_idx = model->num_hed;
    target = &model->hed_list[model->num_hed];
    model->num_hed += 1;
    target->audio_element_id = idx;
    target->head_tracking_enabled = hed->head_tracking_enabled;
    target->render_mode = hed->render_mode;
    assert(hed->channel_mask <= 65535);
    target->channel_mask = hed->channel_mask;
    return PMD_SUCCESS;
}


