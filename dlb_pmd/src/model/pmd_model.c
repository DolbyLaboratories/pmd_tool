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
 * @file pmd_model.c
 * @brief model
 */

#include "pmd_model.h"


void
pmd_model_new_frame
    (dlb_pmd_model *model
    )
{
    model->version_avail = 0;
    model->version_maj = 0;
    model->version_min = 0;
    model->num_signals = 0;
    model->num_elements = 0;
    model->num_abd = 0;
    model->num_apd = 0;
    model->num_pld = 0;
    model->num_xyz = 0;
    model->num_hed = 0;
    model->iat_read_this_frame = 0;
    
    /* initialize audio signal bitmap to 0 */
    pmd_signals_init(&model->signals);

    /* initialize the data to \xff, because 0 is a valid
     * signal/object/presentation identifier (internally,
     * it is the index into the required array)
     */
    memset(model->element_list, '\xff', sizeof(*model->element_list) * model->limits.max_elements);
    memset(model->xyz_list, '\xff', sizeof(*model->xyz_list) * model->limits.max.num_updates);

    pmd_iat_init(model->iat);

    model->write_state.iat_written = 0;
    model->write_state.esd_written = 0;
    pmd_xyz_set_init(&model->write_state.xyz_written);

    pmd_smpte2109_init(&model->smpte2109);
}


void
pmd_model_init
    (dlb_pmd_model *model
    )
{
    pmd_model_new_frame(model);
    model->coordinate_print_precision = DLB_PMD_DEFAULT_COORDINATE_PRECISION;

    pmd_profile_init(&model->profile, &model->limits);

    snprintf((char*)model->title, sizeof(model->title), PMD_UNTITLED_MODEL_TITLE);

    model->esd_present = 0;
    if (model->limits.max.num_ed2_system)
    {
        memset(model->esd, '\0', sizeof(*model->esd));
        model->esd->count = 1;
    }

    memset(model->eep_list, '\xff', sizeof(*model->eep_list) * model->limits.max.num_eac3);
    memset(model->etd_list, '\xff', sizeof(*model->etd_list) * model->limits.max.num_ed2_turnarounds);
    memset(model->apd_list, '\xff', sizeof(*model->apd_list) * model->limits.max.num_presentations);

    model->num_eep = 0;
    model->num_etd = 0;
    model->num_aen = 0;
    pmd_apn_list_init(&model->apn_list, model->limits.max_presentation_names);

    pmd_idmap_init(&model->element_ids);
    pmd_idmap_init(&model->apd_ids);
    pmd_idmap_init(&model->eep_ids);
    pmd_idmap_init(&model->etd_ids);
    pmd_idmap_init(&model->aen_ids);

    model->write_state.bed_write_index = 0;
    model->write_state.obj_write_index = 0;
    model->write_state.abd_written = 0;
    model->write_state.aod_written = 0;
    model->write_state.apd_written = 0;
    model->write_state.pld_written = 0;
    model->write_state.eep_written = 0;
    model->write_state.etd_written = 0;
    model->write_state.hed_written = 0;
    pmd_xyz_set_init(&model->write_state.xyz_written);

    model->write_state.apn_written = 0;
    model->write_state.aen_written = 0;
    model->write_state.esn_written = 0;
    model->write_state.esn_bitmap = 0;

    pmd_apn_list_iterator_init(&model->write_state.apni, &model->apn_list);
}


int                            /** @return 0 on success, 1 on error */
pmd_model_add_update
    (dlb_pmd_model *model      /**< [in] model to look up */
    ,pmd_xyz *new_update       /**< [in] update to add */
    )
{
    if (model->num_xyz < model->limits.max.num_updates)
    {
        /* perform binary chop to find insertion point */
        unsigned int time;
        unsigned int pos = 0;
        unsigned int end = model->num_xyz;
        unsigned int mid = 0;
        while (pos != end)
        {
            mid = (pos + end)/2;
            time = model->xyz_list[mid].time;
            if      (time == new_update->time) break;
            else if (time > new_update->time) end = mid;
            else pos = mid + 1;
        }

        /* pos will now point to an insertion point */
        if (pos < model->num_xyz)
        {
            unsigned int count = model->num_xyz - pos;
            memmove(&model->xyz_list[pos+1], &model->xyz_list[pos], sizeof(pmd_xyz) * count);
        }
        model->xyz_list[pos] = *new_update;
        model->num_xyz += 1;
        return 0;
    }
    return 1;
}


pmd_apd *    /** @return pointer to presentation or NULL if not found */
pmd_find_presentation
    (dlb_pmd_model *model   /**< [in] model to look up */
    ,unsigned int   pres    /**< [in] presentation identifier to find */
    )
{
    unsigned int i;
    pmd_apd *p;
    
    p = model->apd_list;
    for (i = 0; i != model->num_apd; ++i)
    {
        if (pres == p->id)
        {
            return p;
        }
        ++p;
    }

    /* no such presentation id */
    return NULL;
}


pmd_bool                          /** @return 1 if object in presentation, 0 otherwise */
pmd_object_in_presentation
    (pmd_apd *p                   /**< [in] PMD presentation */
    ,unsigned int idx             /**< [in] PMD object index */
    )
{
    return pmd_elements_test(&p->elements, idx);
}

    
pmd_bool                          /** @return 1 if bed in presentation, 0 otherwise */
pmd_bed_in_presentation
    (pmd_apd *p                   /**< [in] PMD presentation */
    ,unsigned int idx             /**< [in] PMD bed index */
    )
{
    return pmd_elements_test(&p->elements, idx);
}



void
pmd_remap_channels
    (dlb_pmd_model *model   /**< [in] model to remap */
    ,int *map
    )
{
    pmd_element *e = model->element_list;
    pmd_object_metadata *omd;
    pmd_track_metadata *tmd;
    unsigned int i;
    unsigned int j;
    
    /* note that the PCM reorder is 0-based, but the
     * signal identifiers in the model are 1-based */
    for (i = 0; i != model->num_elements; ++i)
    {
        switch (e->mode)
        {
            case PMD_MODE_CHANNEL:
                tmd = e->md.channel.metadata;
                for (j = 0; j != e->md.channel.num_tracks; ++j)
                {
                    tmd->source = (uint8_t)map[tmd->source];
                    assert(255 != tmd->source);
                    ++tmd;
                }
                break;
            case PMD_MODE_OBJECT:
                omd = &e->md.object;
                omd->source = (uint8_t)map[omd->source];
                assert(255 != omd->source);
                break;
            default:
                break;
        }
        ++e;
    }
}


const uint8_t *
pmd_model_lookup_element_name
    (const dlb_pmd_model *model
    ,dlb_pmd_element_id element_id
    )
{
    uint16_t idx;
    
    if (pmd_idmap_lookup(&model->aen_ids, element_id, &idx))
    {
        return model->aen_list[idx].name;
    }
    return NULL;
}


void
pmd_model_apply_update
   (dlb_pmd_model *m
   ,pmd_xyz *update
   )
{
    pmd_element *e;

    e = &m->element_list[update->obj_idx];
    assert(e->mode == PMD_MODE_OBJECT);
    e->md.object.x = update->x;
    e->md.object.y = update->y;
    e->md.object.z = update->z;
}
