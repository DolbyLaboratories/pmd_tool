/************************************************************************
 * dlb_pmd
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

/**
 * @file dlb_pmd_api_read.c
 * @brief implementation of read API for Dolby Professional Metadata library
 */

#include "dlb_pmd_api.h"

#include "pmd_model.h"
#include "pmd_error_helper.h"
#include <stdlib.h>

/**
 * @brief helper function to ensure output arrays of ids are in ascending order
 *
 * This creates a 'normal form' for API output, making equality checking simpler.
 */
static inline
void
presentation_element_insert
    (dlb_pmd_element_id *array      /**< array being inserted into */
    ,unsigned int element_num       /**< how many elements in array before latest inserted */
    ,dlb_pmd_element_id new_element /**< latest element to insert */
    )
{
    array[element_num] = new_element;
    if (element_num)
    {
        unsigned int pos = element_num;
        unsigned int shift = 0;
        while (pos && new_element < array[pos-1])
        {
            --pos;
            ++shift;
        }
        if (shift)
        {
            memmove(&array[pos+1], &array[pos], sizeof(array[pos])*shift);
            array[pos] = new_element;
        }
    }
}


dlb_pmd_success
dlb_pmd_smpte2109_sample_offset
    (const dlb_pmd_model *model
    ,      uint16_t *so
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, so);

    *so = model->smpte2109.sample_offset;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_version
    (const dlb_pmd_model *model
    ,      unsigned char *maj
    ,      unsigned char *min
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, maj);
    CHECK_PTRARG(model, min);

    if (model->version_avail != 0xff)
    {
        *maj = model->version_maj;
        *min = model->version_min;
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_title
    (const dlb_pmd_model *model
    ,const char **title
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, title);
    
    *title = (char*)model->title;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_profile
    (const dlb_pmd_model *model
    ,      unsigned int  *profile
    ,      unsigned int  *level
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, profile);
    CHECK_PTRARG(model, level);    

    *profile = model->profile.profile_number;
    *level   = model->profile.profile_level;
    return PMD_SUCCESS;
}


unsigned int
dlb_pmd_num_signals
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return pmd_signals_count(&model->signals);
}


unsigned int
dlb_pmd_num_beds
   (const dlb_pmd_model *model
   )
{
    FUNCTION_PROLOGUE(model);
    return model->num_abd;
}


unsigned int
dlb_pmd_num_objects
   (const dlb_pmd_model *model
   )
{
    FUNCTION_PROLOGUE(model);
    return model->num_elements - model->num_abd;
}


unsigned int
dlb_pmd_num_updates
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->num_xyz;
}


unsigned int
dlb_pmd_num_presentations
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->num_apd;
}


unsigned int
dlb_pmd_num_loudness
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->num_pld;
}


unsigned int
dlb_pmd_num_iat
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->iat && ((model->iat->options & PMD_IAT_PRESENT) != 0);
}


unsigned int
dlb_pmd_num_eac3
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->num_eep;
}


unsigned int
dlb_pmd_num_ed2_system
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->esd_present;
}


unsigned int
dlb_pmd_num_ed2_turnarounds
    (const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    return model->num_etd;
}


dlb_pmd_success
dlb_pmd_count_entities
    (const dlb_pmd_model *model
    ,dlb_pmd_metadata_count *count
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, count);

    count->num_signals            = dlb_pmd_num_signals(model);
    count->num_beds               = dlb_pmd_num_beds(model);
    count->num_objects            = dlb_pmd_num_objects(model);
    count->num_presentations      = dlb_pmd_num_presentations(model);
    count->num_loudness           = dlb_pmd_num_loudness(model);
    count->num_iat                = dlb_pmd_num_iat(model);
    count->num_eac3               = dlb_pmd_num_eac3(model);
    count->num_ed2_system         = dlb_pmd_num_ed2_system(model);
    count->num_ed2_turnarounds    = dlb_pmd_num_ed2_turnarounds(model);
    count->num_updates            = dlb_pmd_num_updates(model);
    count->num_headphone_desc     = dlb_pmd_num_headphone_element_desc(model);
    return PMD_SUCCESS;
}


static inline
dlb_pmd_success
populate_api_bed
    (const dlb_pmd_model *model
    ,const pmd_element *e
    ,dlb_pmd_bed *bed
    ,unsigned int num_sources
    ,dlb_pmd_source *sources
    )
{
    const pmd_track_metadata *tmd;
    const uint8_t *name;
    unsigned int i;

    memset(bed, '\0', sizeof(*bed));
    
    if (num_sources < e->md.channel.num_tracks)
    {
        return PMD_FAIL;
    }

    bed->id          = e->id;
    bed->config      = e->md.channel.config;
    bed->bed_type    = e->md.channel.derived;
    bed->source_id   = bed->bed_type ? e->md.channel.origin : 0;
    bed->num_sources = e->md.channel.num_tracks;
    bed->sources     = sources;

    memset(sources, '\0', sizeof(*sources) * bed->num_sources);

    name = pmd_model_lookup_element_name(model, e->id);
    if (NULL != name)
    {
        memcpy(bed->name, name, sizeof(bed->name));
    }

    tmd = e->md.channel.metadata;
    for (i = 0; i != bed->num_sources; ++i)
    {
        sources->target = tmd->target;
        sources->source = tmd->source + 1;
        sources->gain   = pmd_gain_to_db(tmd->gain);
        ++sources;
        ++tmd;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_bed_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,dlb_pmd_bed *bed
    ,unsigned int num_sources
    ,dlb_pmd_source *sources
    )
{
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, bed);

    if (pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        const pmd_element *e = &model->element_list[idx];
        if (e->mode == PMD_MODE_CHANNEL)
        {
            return populate_api_bed(model, e, bed, num_sources, sources);
        }
        /* otherwise, not a bed! */
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_signal_iterator_init
    (dlb_pmd_signal_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->pos   = 0;
    it->count = pmd_signals_count(&model->signals);
    return PMD_SUCCESS;
}
    

dlb_pmd_success
dlb_pmd_signal_iterator_next
    (dlb_pmd_signal_iterator *it
    ,dlb_pmd_signal *sig
    )
{
    const dlb_pmd_model *model;
    unsigned int id;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, sig);

    if (it->count)
    {
        model = it->model;
        id = it->pos;
        while (id < DLB_PMD_MAX_SIGNALS)
        {
            if (pmd_signals_test(&model->signals, id))
            {
                *sig = (id + 1) & 0xff;
                it->pos = id + 1;
                return PMD_SUCCESS;
            }
            ++id;
        }
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_bed_iterator_init
    (dlb_pmd_bed_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}
    

dlb_pmd_success
dlb_pmd_bed_iterator_next
    (dlb_pmd_bed_iterator *it
    ,dlb_pmd_bed *bed
    ,unsigned int num_sources
    ,dlb_pmd_source *sources
    )
{
    const dlb_pmd_model *model;
    const pmd_idmap *map;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, bed);

    model = it->model;

    map = &model->element_ids;
    idx = (uint16_t)it->count;
    while (pmd_idmap_iterate_next(map, MAX_AUDIO_ELEMENTS, &it->count, &idx))
    {
        const pmd_element *e = &model->element_list[idx];
        if (PMD_MODE_CHANNEL == e->mode)
        {
            return populate_api_bed(model, e, bed, num_sources, sources);
        }
    }
    return PMD_FAIL;
}


static inline
void
populate_api_object
    (const dlb_pmd_model *model
    ,const pmd_element *e
    ,dlb_pmd_object *obj
    )
{
    const uint8_t *name;

    memset(obj, '\0', sizeof(*obj));

    obj->id              = e->id;
    obj->object_class    = e->md.object.oclass;
    obj->dynamic_updates = e->md.object.dynamic_updates;
    obj->x               = pmd_decode_position(e->md.object.x);
    obj->y               = pmd_decode_position(e->md.object.y);
    obj->z               = pmd_decode_position(e->md.object.z);
    obj->size            = pmd_decode_size(e->md.object.size);
    obj->size_3d         = e->md.object.size_vertical;
    obj->diverge         = e->md.object.diverge;
    obj->source          = e->md.object.source + 1;
    obj->source_gain     = pmd_gain_to_db(e->md.object.gain);

    name = pmd_model_lookup_element_name(model, e->id);
    if (NULL != name)
    {
        memcpy(obj->name, name, sizeof(obj->name));
    }
}


dlb_pmd_success
dlb_pmd_object_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,dlb_pmd_object *object
    )
{
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, object);

    if (pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        const pmd_element *e = &model->element_list[idx];
        if (e->mode == PMD_MODE_OBJECT)
        {
            populate_api_object(model, e, object);
            return PMD_SUCCESS;
        }
        /* otherwise, not an object! */
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_object_iterator_init
    (dlb_pmd_object_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_object_iterator_next
   (dlb_pmd_object_iterator *it
   ,dlb_pmd_object *object
   )
{
    const dlb_pmd_model *model;
    const pmd_idmap *map;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, object);

    model = it->model;

    idx = (uint16_t)it->count;
    map = &model->element_ids;
    while (pmd_idmap_iterate_next(map, MAX_AUDIO_ELEMENTS, &it->count, &idx))
    {
        const pmd_element *e = &model->element_list[idx];
        if (PMD_MODE_OBJECT == e->mode)
        {
            populate_api_object(model, e, object);
            return PMD_SUCCESS;
        }
    }
    return PMD_FAIL;
}


static inline
void
populate_api_update
    (const pmd_xyz *u
    ,pmd_element_id id
    ,dlb_pmd_update *update
    )
{
    update->sample_offset = pmd_xyz_decode_time(u->time);
    update->id            = id;
    update->x             = pmd_decode_position(u->x);
    update->y             = pmd_decode_position(u->y);
    update->z             = pmd_decode_position(u->z);
}


dlb_pmd_success
dlb_pmd_update_lookup
    (const dlb_pmd_model *model
    ,unsigned int sample_offset
    ,dlb_pmd_element_id id
    ,dlb_pmd_update *update
    )
{
    const pmd_xyz *u;
    unsigned int time = pmd_xyz_encode_time(sample_offset);
    unsigned int i;
    uint16_t idx;
    
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, update);

    if (!pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        return PMD_FAIL;
    }
    
    u = model->xyz_list;
    for (i = 0; i != model->num_xyz; ++i)
    {
        if (u->time == time && u->obj_idx == idx)
        {
            populate_api_update(u, id, update);
            return PMD_SUCCESS;
        }
        ++u;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_update_iterator_init
    (dlb_pmd_update_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_update_iterator_next
   (dlb_pmd_update_iterator *it
   ,dlb_pmd_update *update
   )
{
    const dlb_pmd_model *model;
    dlb_pmd_element_id id;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, update);

    model = it->model;
    idx = (uint16_t)it->count;
    while (idx < model->num_xyz)
    {
        const pmd_xyz *u = &model->xyz_list[idx];
        id = model->element_list[u->obj_idx].id;
        
        populate_api_update(u, id, update);
        ++idx;
        it->count = idx;
        return PMD_SUCCESS;
    }
    it->count = idx;
    return PMD_FAIL;
}


static inline
dlb_pmd_success
populate_api_presentation
    (const dlb_pmd_model *model
    ,const pmd_apd *p
    ,dlb_pmd_presentation *api
    ,unsigned int num_elements
    ,dlb_pmd_element_id *elements
    )
{
    const pmd_apn *pnam;
    unsigned int i;
    
    memset(api, '\0', sizeof(*api));

    api->id             = p->id;
    api->config         = p->config;
    api->num_elements   = p->num_elements;
    api->elements       = elements;
    api->num_names      = p->num_names;

    pmd_langcode_string(p->pres_lang, &api->audio_language);

    for (i = 0; i != p->num_names; ++i)
    {
        pnam = pmd_apn_list_lookup(&model->apn_list, p->names[i]);
        pmd_langcode_string(pnam->lang, &api->names[i].language);
        memcpy(api->names[i].text, pnam->text, sizeof(pnam->text));
    }

    if (num_elements >= api->num_elements)
    {
        pmd_apd_iterator pi;
        const pmd_element *e;
        unsigned int j;

        pmd_apd_iterator_init(&pi, p);
        
        for (j = 0; j != api->num_elements; ++j)
        {
            unsigned int idx;
            if (!pmd_apd_iterator_next(&pi, &idx))
            {
                return PMD_FAIL;
            }
            e = &model->element_list[idx];
            presentation_element_insert(api->elements, j, e->id);
        }
        return PMD_SUCCESS;
    }

    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_presentation_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_presentation_id id
    ,dlb_pmd_presentation *presentation
    ,unsigned int num_elements
    ,dlb_pmd_element_id *elements
    )
{
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, presentation);

    if (pmd_idmap_lookup(&model->apd_ids, id, &idx))
    {
        const pmd_apd *p = &model->apd_list[idx];
        return populate_api_presentation(model, p, presentation, num_elements, elements);
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_presentation_iterator_init
    (dlb_pmd_presentation_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_presentation_iterator_next
    (dlb_pmd_presentation_iterator *it
    ,dlb_pmd_presentation *presentation
    ,unsigned int num_elements
    ,dlb_pmd_element_id *elements
    )
{
    const dlb_pmd_model *model;
    const pmd_idmap *map;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, presentation);

    model = it->model;
    map = &model->apd_ids;
    idx = (uint16_t)it->count;    
    if (pmd_idmap_iterate_next(map, MAX_PRESENTATIONS, &it->count, &idx))
    {
        const pmd_apd *p = &model->apd_list[idx];
        populate_api_presentation(model, p, presentation, num_elements, elements);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


static
void
populate_api_loudness
    (const dlb_pmd_model *model
    ,const pmd_pld *pld
    ,dlb_pmd_loudness *loudness
    )
{
    pmd_loudness_options options = pld->options;
    const pmd_apd *p;
    memset(loudness, '\0', sizeof(*loudness));
    
    p = &model->apd_list[pld->presid];
    loudness->presid = p->id;
    loudness->loud_prac_type = (dlb_pmd_loudness_practice)pld->lpt;
    if (loudness->loud_prac_type != PMD_PLD_LOUDNESS_PRACTICE_NOT_INDICATED)
    {
        if (options & PMD_PLD_OPT_LOUDCORR_DIALGATE)
        {
            loudness->b_loudcorr_gating = 1;
            loudness->loudcorr_gating = (dlb_pmd_dialgate_practice)pld->dpt;
        }
        loudness->loudcorr_type = (dlb_pmd_correction_type)pld->corrty;
    }
    if (options & PMD_PLD_OPT_LOUDRELGAT)
    {
        loudness->b_loudrelgat = 1;
        loudness->loudrelgat = pmd_decode_lufs(pld->lrg);
    }
    if (options & PMD_PLD_OPT_LOUDSPCHGAT)
    {
        loudness->b_loudspchgat = 1;
        loudness->loudspchgat = pmd_decode_lufs(pld->lsg);
        loudness->loudspch_gating = (dlb_pmd_dialgate_practice)pld->sdpt;
    }
    if (options & PMD_PLD_OPT_LOUDSTRM3S)
    {
        loudness->b_loudstrm3s = 1;
        loudness->loudstrm3s = pmd_decode_lufs(pld->l3g);
    }
    if (options & PMD_PLD_OPT_MAX_LOUDSTRM3S)
    {
        loudness->b_max_loudstrm3s = 1;
        loudness->max_loudstrm3s = pmd_decode_lufs(pld->l3g_max);
    }
    if (options & PMD_PLD_OPT_TRUEPK)
    {
        loudness->b_truepk = 1;
        loudness->truepk = pmd_decode_lufs(pld->tpk);
    }
    if (options & PMD_PLD_OPT_MAX_TRUEPK)
    {
        loudness->b_max_truepk = 1;
        loudness->max_truepk = pmd_decode_lufs(pld->tpk_max);
    }
    if (options & PMD_PLD_OPT_PRGMBNDY)
    {
        loudness->b_prgmbndy = 1;
        loudness->prgmbndy = pld->prgmbndy;
    }
    if (options & PMD_PLD_OPT_PRGMBNDY_OFFSET)
    {
        loudness->b_prgmbndy_offset = 1;
        loudness->prgmbndy_offset = pld->prgmbndy_offset;
    }
    if (options & PMD_PLD_OPT_LRA)
    {
        loudness->b_lra = 1;
        loudness->lra = pmd_decode_lra(pld->lra);
        loudness->lra_prac_type = (dlb_pmd_loudness_range_practice)pld->lrap;
    }
    if (options & PMD_PLD_OPT_LOUDMNTRY)
    {
        loudness->b_loudmntry = 1;
        loudness->loudmntry = pmd_decode_lufs(pld->ml);
    }
    if (options & PMD_PLD_OPT_MAX_LOUDMNTRY)
    {
        loudness->b_max_loudmntry = 1;
        loudness->max_loudmntry = pmd_decode_lufs(pld->ml_max);
    }
    if (options & PMD_PLD_OPT_EXTENSION)
    {
        loudness->extension.size = pld->extension_bits;
        memcpy(loudness->extension.data, pld->extension,
               (pld->extension_bits + 7)/8);
    }
}


dlb_pmd_success
dlb_pmd_loudness_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_presentation_id id
    ,dlb_pmd_loudness *loudness
    )
{
    const pmd_pld *pld;
    unsigned int i;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, loudness);

    if (!pmd_idmap_lookup(&model->apd_ids, id, &idx))
    {
        return PMD_FAIL;
    }

    pld = model->pld_list;
    for (i = 0; i != model->num_pld; ++i)
    {
        if (pld->presid == idx)
        {
            populate_api_loudness(model, pld, loudness);
            return PMD_SUCCESS;
        }
        ++pld;
    }
    return PMD_FAIL;
}

   
dlb_pmd_success
dlb_pmd_loudness_iterator_init
    (dlb_pmd_loudness_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);    
    CHECK_PTRARG(model, it);

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_loudness_iterator_next
   (dlb_pmd_loudness_iterator *it
   ,dlb_pmd_loudness *loudness
   )
{
    const dlb_pmd_model *model;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, loudness);

    model = it->model;
    idx = (uint16_t)it->count;
    while (idx < model->num_pld)
    {
        const pmd_pld *pld = &model->pld_list[idx];
        populate_api_loudness(model, pld, loudness);
        ++idx;
        it->count = idx;
        return PMD_SUCCESS;
    }
    it->count = idx;
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_iat_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_identity_and_timing *api
    )
{
    const pmd_iat *iat;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, api);

    memset(api, '\0', sizeof(*api));
    iat = model->iat;

    if (iat->options & PMD_IAT_PRESENT)
    {
        if (iat->content_id_size)
        {
            api->content_id.type = iat->content_id_type;
            api->content_id.size = iat->content_id_size;
            memcpy(api->content_id.data, iat->content_id, sizeof(iat->content_id));
        }
        if (iat->distribution_id_size)
        {
            api->distribution_id.type = iat->distribution_id_type;
            api->distribution_id.size = iat->distribution_id_size;
            memcpy(api->distribution_id.data, iat->distribution_id, sizeof(iat->distribution_id));
        }
        api->timestamp = iat->timestamp;
        if (iat->options & PMD_IAT_OFFSET_PRESENT)
        {
            api->offset.present = 1;
            api->offset.offset = iat->offset;
        }
        if (iat->options & PMD_IAT_VALIDITY_DUR_PRESENT)
        {
            api->validity_duration.present = 1;
            api->validity_duration.vdur = iat->validity_duration;
        }
        if (iat->user_data_size)
        {
            api->user_data.size = iat->user_data_size;
            memcpy(api->user_data.data, iat->user_data, sizeof(iat->user_data));
        }
        if (iat->extension_size)
        {
            api->extension.size = iat->extension_size*8;
            memcpy(api->extension.data, iat->extension_data, sizeof(iat->extension_data));
        }
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


static inline
void
populate_api_eac3
    (const dlb_pmd_model *model
    ,const pmd_eep *eep
    ,dlb_pmd_eac3 *eac3
    )
{
    unsigned int i;
    
    memset(eac3, '\0', sizeof(*eac3));
    eac3->id = eep->id;
    if (eep->options & PMD_EEP_ENCODER_PRESENT)
    {
        eac3->b_encoder_params = 1;
        eac3->dynrng_prof      = eep->dynrng_prof;
        eac3->compr_prof       = eep->compr_prof;
        eac3->surround90       = eep->surround90;
        eac3->hmixlev          = eep->hmixlev;
    }
    if (eep->options & PMD_EEP_BITSTREAM_PRESENT)
    {
        eac3->b_bitstream_params = 1;
        eac3->bsmod              = eep->bsmod;
        eac3->dsurmod            = eep->dsurmod;
        eac3->dialnorm           = eep->dialnorm;
        eac3->dmixmod            = eep->dmixmod;
        eac3->ltrtcmixlev        = eep->ltrtcmixlev;
        eac3->ltrtsurmixlev      = eep->ltrtsurmixlev;
        eac3->lorocmixlev        = eep->lorocmixlev;
        eac3->lorosurmixlev      = eep->lorosurmixlev;
    }
    if (eep->options & PMD_EEP_DRC_PRESENT)
    {
        eac3->b_drc_params     = 1;
        eac3->drc_port_spkr    = eep->drc_port_spkr;
        eac3->drc_port_hphone  = eep->drc_port_hphone;
        eac3->drc_flat_panl    = eep->drc_flat_panl;
        eac3->drc_home_thtr    = eep->drc_home_thtr;
        eac3->drc_ddplus       = eep->drc_ddplus;
    }
    eac3->num_presentations = eep->num_presentations;
    for (i = 0; i != eac3->num_presentations; ++i)
    {
        const pmd_apd *apd = model->apd_list;
        eac3->presentations[i] = apd[eep->presentations[i]].id;
    }
}


dlb_pmd_success
dlb_pmd_eac3_lookup
    (const dlb_pmd_model *model
    ,unsigned int id
    ,dlb_pmd_eac3 *eac3
    )
{
    const pmd_eep *eep;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, eac3);

    if (!pmd_idmap_lookup(&model->eep_ids, (uint16_t)id, &idx))
    {
        return PMD_FAIL;
    }

    eep = &model->eep_list[idx];
    populate_api_eac3(model, eep, eac3);
    return PMD_SUCCESS;
}

   
dlb_pmd_success
dlb_pmd_eac3_iterator_init
    (dlb_pmd_eac3_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);    

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_eac3_iterator_next
   (dlb_pmd_eac3_iterator *it
   ,dlb_pmd_eac3 *eac3
   )
{
    const pmd_eep *eep;
    const dlb_pmd_model *model;
    const pmd_idmap *map;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, eac3);

    model = it->model;
    map = &model->eep_ids;
    idx = (uint16_t)it->count;
    if (pmd_idmap_iterate_next(map, MAX_EAC3_ENCODING_PARAMETERS, &it->count, &idx))
    {
        eep = &model->eep_list[idx];
        populate_api_eac3(model, eep, eac3);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


static inline
void
populate_api_etd
    (const dlb_pmd_model *model
    ,const pmd_etd *etd
    ,dlb_pmd_ed2_turnaround *api
    )
{
    const pmd_apd *apd = model->apd_list;
    const pmd_apd *p;
    const pmd_eep *eep = model->eep_list;
    const pmd_eep *e;
    unsigned int i;

    memset(api, '\0', sizeof(*api));
    api->id = etd->id;
    api->ed2_presentations = etd->ed2_presentations;
    api->ed2_framerate = etd->ed2_framerate;
    for (i = 0; i != api->ed2_presentations; ++i)
    {
        p = &apd[etd->ed2_turnaround[i].presid];
        e = &eep[etd->ed2_turnaround[i].eepid];
        api->ed2_turnarounds[i].presid = p->id;
        api->ed2_turnarounds[i].eepid = e->id;
    }
    
    api->de_presentations = etd->de_presentations;
    api->de_framerate = etd->de_framerate;
    api->pgm_config = etd->pgm_config;
    for (i = 0; i != api->de_presentations; ++i)
    {
        p = &apd[etd->de_turnaround[i].presid];
        e = &eep[etd->de_turnaround[i].eepid];

        api->de_turnarounds[i].presid = p->id;
        api->de_turnarounds[i].eepid = e->id;
    }
}


dlb_pmd_success
dlb_pmd_ed2_turnaround_lookup
    (const dlb_pmd_model *model
    ,unsigned int id
    ,dlb_pmd_ed2_turnaround *ed2_turnaround
    )
{
    const pmd_etd *etd;
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, ed2_turnaround);

    if (!pmd_idmap_lookup(&model->etd_ids, (uint16_t)id, &idx))
    {
        return PMD_FAIL;
    }

    etd = &model->etd_list[idx];
    populate_api_etd(model, etd, ed2_turnaround);
    return PMD_SUCCESS;
}

   
dlb_pmd_success
dlb_pmd_ed2_turnaround_iterator_init
    (dlb_pmd_ed2_turnaround_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);    

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_ed2_turnaround_iterator_next
   (dlb_pmd_ed2_turnaround_iterator *it
   ,dlb_pmd_ed2_turnaround *ed2_turnaround
   )
{
    const dlb_pmd_model *model;
    const pmd_idmap *map;
    const pmd_etd *etd;
    uint16_t idx;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, ed2_turnaround);

    model = it->model;
    map = &model->etd_ids;
    idx = (uint16_t)it->count;
    if (pmd_idmap_iterate_next(map, MAX_ED2_TURNAROUNDS, &it->count, &idx))
    {
        etd = &model->etd_list[idx];
        populate_api_etd(model, etd, ed2_turnaround);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_ed2_system_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_ed2_system *api
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, api);

    if (model->esd_present)
    {
        const pmd_esd *esd = model->esd;
        unsigned int i;
        memset(api, '\0', sizeof(*api));
        api->count = esd->count;
        api->rate = esd->rate;
        for (i = 0; i != api->count; ++i)
        {
            api->streams[i].config = esd->streams[i].config;
            api->streams[i].compression = esd->streams[i].compression;
        }
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}


static inline
dlb_pmd_success
populate_api_headphone
    (const dlb_pmd_model *model
    ,const dlb_pmd_headphone *hed
    ,dlb_pmd_headphone *api
    )
{
    const pmd_element *e = &model->element_list[hed->audio_element_id];
    api->audio_element_id = e->id;
    api->head_tracking_enabled = hed->head_tracking_enabled;
    api->render_mode = hed->render_mode;
    api->channel_mask = hed->channel_mask;
    return PMD_SUCCESS;
}


unsigned int
dlb_pmd_num_headphone_element_desc
    (const dlb_pmd_model *model
    )
{
    if (!model) return 0;
    return model->num_hed;
}


dlb_pmd_success
dlb_pmd_hed_lookup
    (const dlb_pmd_model *model
    ,dlb_pmd_element_id id
    ,dlb_pmd_headphone *api
    )
{
    uint16_t idx;

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, api);

    if (pmd_idmap_lookup(&model->element_ids, id, &idx))
    {
        const pmd_element *e = &model->element_list[idx];
        if (e->hed_idx != 0xffff)
        {
            const pmd_hed *hed = &model->hed_list[e->hed_idx];
            return populate_api_headphone(model, hed, api);
        }
    }
    return PMD_FAIL;
}

   
dlb_pmd_success
dlb_pmd_hed_iterator_init
    (dlb_pmd_hed_iterator *it
    ,const dlb_pmd_model *model
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, it);

    it->model = model;
    it->count = 0;
    return PMD_SUCCESS;
}

    
dlb_pmd_success
dlb_pmd_hed_iterator_next
   (dlb_pmd_hed_iterator *it
   ,dlb_pmd_headphone *api
   )
{
    const dlb_pmd_model *model;

    if (!it) return PMD_FAIL;
    CHECK_PTRARG(it->model, api);

    model = it->model;

    if (it->count < model->num_hed)
    {
        const pmd_hed *hed = &model->hed_list[it->count];
        it->count += 1;
        return populate_api_headphone(model, hed, api);
    }
    return PMD_FAIL;
}

