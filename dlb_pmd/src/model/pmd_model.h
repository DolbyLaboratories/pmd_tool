/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @file pmd_model.h
 * @brief model
 */

#ifndef PMD_MODEL_H_
#define PMD_MODEL_H_

#if defined(_MSC_VER) 
#  if _MSC_VER < 1900
#    define snprintf _snprintf
#    if !defined(inline)
#      define inline __inline
#    endif
#  endif
#endif

#include "pmd_profile.h"
#include "pmd_strings.h"
#include "pmd_signals.h"
#include "pmd_xyz_set.h"
#include "pmd_os.h"
#include "pmd_idmap.h"

#include "pmd_smpte2109.h"
#include "pmd_abd_aod.h"
#include "pmd_aen.h"
#include "pmd_apd.h"
#include "pmd_apn.h"
#include "pmd_eep.h"
#include "pmd_esd.h"
#include "pmd_etd.h"
#include "pmd_hed.h"
#include "pmd_iat.h"
#include "pmd_pld.h"
#include "pmd_xyz.h"

#include <string.h>
#include <stdio.h>


/**
 * @def PMD_ERROR_SIZE
 * @brief size of space in bytes reserved for error string
 */
#define PMD_ERROR_SIZE (128)


/**
 * @def MAX_UPDATE_BITS
 * @brief number of bits available to store update times
 */
#define MAX_UPDATE_BITS (6)


/**
 * @def MAX_UPDATE_TIME
 * @brief maximum sample time after update
 *
 * sample times are listed in blocks of 32 
 */
#define MAX_UPDATE_TIME ((1<<MAX_UPDATE_BITS)-1)


 /**
 * @def PMD_UNTITLED_MODEL_TITLE
 * @brief default value for an untitled model's title
 * @note don't use this value for actual model titles!
 */
#define PMD_UNTITLED_MODEL_TITLE "\t\t\tUntitled\t\t\t"


 /**
 * @brief model write state
 */
typedef struct
{
    /**
     * Information on how much has been written
     *
     * In PCM workflows, we have to distribute the various payloads
     * over successive blocks, if there is not enough space to put
     * them all in the first block.
     */
    unsigned int    esd_written;        /**< Number of ESD streams written (used to make sure we write
                                         *   the next one in correct order */
    pmd_bool        iat_written;        /**< Has IAT been written this frame? */
    unsigned int    abd_written;        /**< How many Audio Bed descriptions have been written? */
    unsigned int    aod_written;        /**< How many Audio Object descriptions have been written? */
    unsigned int    bed_write_index;    /**< index of next bed to write */
    unsigned int    obj_write_index;    /**< index of next object to write */
    unsigned int    apd_written;        /**< How many Audio Presentation descriptions have been written? */
    unsigned int    pld_written;        /**< How many Presentation Loudness descriptions have been written? */
    unsigned int    eep_written;        /**< How many EAC3 encoder parameter structs have been written? */
    unsigned int    etd_written;        /**< How many ED2 turnaround descriptions have been written? */
    unsigned int    hed_written;        /**< How many headphone descriptions have been written? */
    pmd_xyz_set     xyz_written;        /**< Which XYZ payloads have been written? */

    unsigned int    apn_written;        /**< How many presentation names have been written? */
    unsigned int    aen_written;        /**< How many element names have been written? */
    unsigned int    esn_written;        /**< How many ED2 stream names have been written? */
    unsigned int    esn_bitmap;         /**< Which ED2 substreams have written their name? */

    pmd_apn_list_iterator apni;         /**< presentation name iterator */

} pmd_model_write_state;


/**
 * @brief main model
 */
struct dlb_pmd_model
{
    pmd_mutex lock;             /**< protect concurrent access */
    pmd_bool  mallocated;       /**< true if memory allocated by malloc() */

    char error[PMD_ERROR_SIZE]; /**< error string, if any */
    dlb_pmd_model_error_callback error_callback;
    dlb_pmd_model_error_callback_arg error_cbarg;

    uint8_t title[DLB_PMD_TITLE_SIZE]; /**< title of overall content */
    
    uint8_t version_avail;      /**< version information available in bitstream? */
    uint8_t version_maj;        /**< if available, bitstream major version number */
    uint8_t version_min;        /**< if available, bitstream minor version number */

    dlb_pmd_model_constraints limits; /**< max entities allowed */
    pmd_profile profile;              /**< profile constraints, if any */

    uint16_t num_signals;
    uint16_t num_elements;
    uint16_t num_abd;
    uint16_t num_apd;
    uint16_t num_pld;
    uint16_t num_xyz;
    uint16_t num_eep;
    uint16_t num_etd;
    uint16_t num_aen;
    uint16_t num_hed;

    pmd_bool esd_present;               /**< True only for ED2 encoding */

    pmd_signals signals;
    pmd_element *element_list;
    pmd_apd *apd_list;
    pmd_pld *pld_list;
    pmd_iat *iat;
    pmd_bool iat_read_this_frame;
    
    /**
     * audio object updates are expected to be ordered temporally, i.e.,
     * earlier updates first.
     */
    pmd_xyz *xyz_list;
    pmd_apn_list apn_list;
    pmd_aen *aen_list;
    pmd_eep *eep_list;
    pmd_etd *etd_list;
    pmd_esd *esd;
    pmd_hed *hed_list;

    /**
     * Information keeping track of what has been read
     */
    pmd_idmap element_ids;    /**< converts element id attributes to array indices */
    pmd_idmap apd_ids;        /**< converts presentation id attributes to array indices */
    pmd_idmap eep_ids;        /**< converts eac3 encoding parameter id attrs to array indices */
    pmd_idmap etd_ids;        /**< converts ED2 turnarounds id attrs to array indices */
    pmd_idmap aen_ids;        /**< converts element id to element name array index */

    /**
     * Information on how much has been written
     */
    pmd_model_write_state write_state;

    /**
     * SMPTE 2109
     */
    pmd_smpte2109 smpte2109;    /**< SMPTE 2109 specific information */

    /**
     * Miscellaneous
     */
    int coordinate_print_precision;
};


/**
 * @brief prepare model to receive information from a new frame
 *
 * We need to selectively update model information, because things
 * like APN and ECD payloads may persist between frames (i.e., it may
 * take multiple frames to accumulate all information).
 */
static inline
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


/**
 * @brief first-time initialization of model
 */
static inline
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


/**
 * @brief helper function to add an update to the model
 *
 * updates are stored in chronological order, so we may need to insert
 * into the middle of the array
 */
static inline
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


/**
 * @brief helper function to look up a presentation identifier
 */
static inline
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


/**
 * @brief determine if a given PMD object index occurs in the given presentation
 */
static inline
pmd_bool                          /** @return 1 if object in presentation, 0 otherwise */
pmd_object_in_presentation
    (pmd_apd *p                   /**< [in] PMD presentation */
    ,unsigned int idx             /**< [in] PMD object index */
    )
{
    return pmd_elements_test(&p->elements, idx);
}

    
/**
 * @brief determine if a given PMD bed index occurs in the given presentation
 */
static inline
pmd_bool                          /** @return 1 if bed in presentation, 0 otherwise */
pmd_bed_in_presentation
    (pmd_apd *p                   /**< [in] PMD presentation */
    ,unsigned int idx             /**< [in] PMD bed index */
    )
{
    return pmd_elements_test(&p->elements, idx);
}



static inline
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


static inline
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


static inline
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


#endif /* PMD_MODEL_H_ */
