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
 * @file dlb_pmd_metadata_set.c
 * @brief implementation of API to read/write metadata sets
 */

#include "dlb_pmd_api.h"
#include "pmd_model.h"
#include <assert.h>


#define ROUND_UP(x,multiple) ((((x) + ((multiple)-1)) / (multiple)) * (multiple))
#define ALLOCSIZE(x) ROUND_UP(x,sizeof(void*))


/**
 * @brief copy title information out of the model into the metadata set
 */
static
dlb_pmd_success                        /** @return 0 on success, 1 on failure */
copy_title
    (const dlb_pmd_model *model        /**< [in] model to read */
    ,char (*title)[DLB_PMD_TITLE_SIZE] /**< [in/out] space to copy title */
    )
{
    memcpy(*title, model->title, sizeof(*title));
    return PMD_SUCCESS;
}


/**
 * @brief copy bed information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                 /** @return 0 on success, 1 on failure */
copy_beds
    (const dlb_pmd_model *model /**< [in] model to read */
    ,dlb_pmd_bed **beds         /**< [in] bed array to populate */
    ,uintptr_t *mem             /**< [in] pointer to free memory (as integer) */
    )
{
#define NUM_SOURCES (256)
    dlb_pmd_source sources[NUM_SOURCES];
    dlb_pmd_bed_iterator bi;
    dlb_pmd_bed *bed;
    unsigned int num_beds;

    num_beds = dlb_pmd_num_beds(model);
    *beds = (dlb_pmd_bed*)*mem;
    *mem  += num_beds * ALLOCSIZE(sizeof(dlb_pmd_bed));

    bed = *beds;
    if (dlb_pmd_bed_iterator_init(&bi, model)) return PMD_FAIL;
    while (!dlb_pmd_bed_iterator_next(&bi, bed, NUM_SOURCES, sources))
    {
        size_t memsz = bed->num_sources * ALLOCSIZE(sizeof(dlb_pmd_source));
        bed->sources = (dlb_pmd_source*)*mem;
        *mem += memsz;
        memcpy(bed->sources, sources, memsz);
        ++bed;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy object information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                 /** @return 0 on success, 1 on failure */
copy_objects
    (const dlb_pmd_model *model /**< [in] model to read */
    ,dlb_pmd_object **objects   /**< [in] object array to populate */
    ,uintptr_t *mem             /**< [in] pointer to free memory (as integer) */
    )
{
    dlb_pmd_object_iterator oi;
    dlb_pmd_object *object;
    unsigned int num_objects;

    num_objects = dlb_pmd_num_objects(model);
    *objects = (dlb_pmd_object*)*mem;
    *mem  += num_objects * ALLOCSIZE(sizeof(dlb_pmd_object));

    object = *objects;
    if (dlb_pmd_object_iterator_init(&oi, model)) return PMD_FAIL;
    while (!dlb_pmd_object_iterator_next(&oi, object))
    {
        ++object;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy update information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                 /** @return 0 on success, 1 on failure */
copy_updates
    (const dlb_pmd_model *model /**< [in] model to read */
    ,dlb_pmd_update **updates   /**< [in] update array to populate */
    ,uintptr_t *mem             /**< [in] pointer to free memory (as integer) */
    )
{
    dlb_pmd_update_iterator ui;
    dlb_pmd_update *update;
    unsigned int num_updates;

    num_updates = dlb_pmd_num_updates(model);
    *updates = (dlb_pmd_update*)*mem;
    *mem  += num_updates * ALLOCSIZE(sizeof(dlb_pmd_update));

    update = *updates;
    if (dlb_pmd_update_iterator_init(&ui, model)) return PMD_FAIL;
    while (!dlb_pmd_update_iterator_next(&ui, update))
    {
        ++update;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy presentation information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                  /** @return 0 on success, 1 on failure */
copy_presentations
    (const dlb_pmd_model *model  /**< [in] model to read */
    ,dlb_pmd_presentation **ps   /**< [in] presentation array to populate */
    ,uintptr_t *mem              /**< [in] pointer to free memory (as integer) */
    )
{
#define NUM_ELEMENTS (4096)
    dlb_pmd_element_id elements[NUM_ELEMENTS];
    dlb_pmd_presentation_iterator pi;
    dlb_pmd_presentation *p;
    unsigned int num_pres;

    num_pres = dlb_pmd_num_presentations(model);
    *ps = (dlb_pmd_presentation*)*mem;
    *mem  += num_pres * ALLOCSIZE(sizeof(dlb_pmd_presentation));

    p = *ps;
    if (dlb_pmd_presentation_iterator_init(&pi, model)) return PMD_FAIL;
    while (!dlb_pmd_presentation_iterator_next(&pi, p, NUM_ELEMENTS, elements))
    {
        size_t memsz = ALLOCSIZE(p->num_elements * sizeof(dlb_pmd_element_id));
        p->elements = (dlb_pmd_element_id*)*mem;
        *mem += memsz;
        memcpy(p->elements, elements, memsz);
        ++p;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy loudness information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                   /** @return 0 on success, 1 on failure */
copy_loudness
    (const dlb_pmd_model *model   /**< [in] model to read */
    ,dlb_pmd_loudness **loudness  /**< [in] loudness array to populate */
    ,uintptr_t *mem               /**< [in] pointer to free memory (as integer) */
    )
{
    dlb_pmd_loudness_iterator li;
    dlb_pmd_loudness *loud;
    unsigned int num_loudness;

    num_loudness = dlb_pmd_num_loudness(model);
    *loudness = (dlb_pmd_loudness*)*mem;
    *mem  += num_loudness * ALLOCSIZE(sizeof(dlb_pmd_loudness));

    loud = *loudness;
    if (dlb_pmd_loudness_iterator_init(&li, model)) return PMD_FAIL;
    while (!dlb_pmd_loudness_iterator_next(&li, loud))
    {
        ++loud;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy Identity and Timing information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                         /** @return 0 on success, 1 on failure */
copy_iat
    (const dlb_pmd_model *model         /**< [in] model to read */
    ,dlb_pmd_identity_and_timing **iat  /**< [in] IAT to populate */
    ,uintptr_t *mem                     /**< [in] pointer to free memory (as integer) */
    )
{
    unsigned int num_iat;

    *iat = NULL;
    
    num_iat = dlb_pmd_num_iat(model);
    if (num_iat)
    {
        assert(num_iat == 1);
        *iat = (dlb_pmd_identity_and_timing*)*mem;
        *mem  += ALLOCSIZE(sizeof(dlb_pmd_identity_and_timing));
        return dlb_pmd_iat_lookup(model, *iat);
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy EAC3 encoding parameters information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                   /** @return 0 on success, 1 on failure */
copy_eac3
    (const dlb_pmd_model *model   /**< [in] model to read */
    ,dlb_pmd_eac3 **eac3          /**< [in] eac3 array to populate */
    ,uintptr_t *mem               /**< [in] pointer to free memory (as integer) */
    )
{
    dlb_pmd_eac3_iterator ei;
    dlb_pmd_eac3 *e;
    unsigned int num_eac3;

    num_eac3 = dlb_pmd_num_eac3(model);
    *eac3 = (dlb_pmd_eac3*)*mem;
    *mem  += num_eac3 * ALLOCSIZE(sizeof(dlb_pmd_eac3));

    e = *eac3;
    if (dlb_pmd_eac3_iterator_init(&ei, model)) return PMD_FAIL;
    while (!dlb_pmd_eac3_iterator_next(&ei, e))
    {
        ++e;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy ED2 turnaround information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                    /** @return 0 on success, 1 on failure */
copy_ed2_turnarounds
    (const dlb_pmd_model *model    /**< [in] model to read */
    ,dlb_pmd_ed2_turnaround **edt  /**< [in] eac3 array to populate */
    ,uintptr_t *mem                /**< [in] pointer to free memory (as integer) */
    )
{
    dlb_pmd_ed2_turnaround_iterator ei;
    dlb_pmd_ed2_turnaround *e;
    unsigned int num_turnarounds;

    num_turnarounds = dlb_pmd_num_ed2_turnarounds(model);
    *edt = (dlb_pmd_ed2_turnaround*)*mem;
    *mem  += num_turnarounds * ALLOCSIZE(sizeof(dlb_pmd_ed2_turnaround));

    e = *edt;
    if (dlb_pmd_ed2_turnaround_iterator_init(&ei, model)) return PMD_FAIL;
    while (!dlb_pmd_ed2_turnaround_iterator_next(&ei, e))
    {
        ++e;
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy ED2 system information out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                    /** @return 0 on success, 1 on failure */
copy_ed2_system
    (const dlb_pmd_model *model    /**< [in] model to read */
    ,dlb_pmd_ed2_system **eds      /**< [in] IAT to populate */
    ,uintptr_t *mem                /**< [in] pointer to free memory (as integer) */
    )
{
    unsigned int num_eds;

    *eds = NULL;
    
    num_eds = dlb_pmd_num_ed2_system(model);
    if (num_eds)
    {
        assert(num_eds == 1);
        *eds = (dlb_pmd_ed2_system*)*mem;
        *mem  += ALLOCSIZE(sizeof(dlb_pmd_ed2_system));
        return dlb_pmd_ed2_system_lookup(model, *eds);
    }
    return PMD_SUCCESS;
}


/**
 * @brief copy headphone element descriptions out of the model into the metadata set
 *
 * Note that this function assumes that the given memory is sufficient. 
 */
static
dlb_pmd_success                  /** @return 0 on success, 1 on failure */
copy_headphones
    (const dlb_pmd_model *model  /**< [in] model to read */
    ,dlb_pmd_headphone **hs      /**< [in] headphone array to populate */
    ,uintptr_t *mem              /**< [in] pointer to free memory (as integer) */
    )
{
    dlb_pmd_hed_iterator hi;
    dlb_pmd_headphone *hed;
    unsigned int num_hed;

    num_hed = dlb_pmd_num_headphone_element_desc(model);
    *hs = (dlb_pmd_headphone*)*mem;
    *mem  += num_hed * ALLOCSIZE(sizeof(dlb_pmd_headphone));

    hed = *hs;
    if (dlb_pmd_hed_iterator_init(&hi, model)) return PMD_FAIL;
    while (!dlb_pmd_hed_iterator_next(&hi, hed))
    {
        ++hed;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_title
    (dlb_pmd_model *model
    ,const char (*title)[DLB_PMD_TITLE_SIZE]
    )
{
    memcpy(model->title, *title, sizeof(model->title));
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_beds
    (dlb_pmd_model *model
    ,dlb_pmd_bed *beds
    ,unsigned int num_beds
    )
{
    unsigned int i;

    for (i = 0; i != num_beds; ++i)
    {
        if (dlb_pmd_set_bed(model, beds)) return PMD_FAIL;
        ++beds;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_objects
    (dlb_pmd_model *model
    ,dlb_pmd_object *objects
    ,unsigned int num_objects
    )
{
    unsigned int i;

    for (i = 0; i != num_objects; ++i)
    {
        if (dlb_pmd_set_object(model, objects)) return PMD_FAIL;
        ++objects;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_updates
    (dlb_pmd_model *model
    ,dlb_pmd_update *update
    ,unsigned int num_updates
    )
{
    unsigned int i;

    for (i = 0; i != num_updates; ++i)
    {
        if (dlb_pmd_set_update(model, update)) return PMD_FAIL;
        ++update;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_presentations
    (dlb_pmd_model *model
    ,dlb_pmd_presentation *ps
    ,unsigned int num_presentations
    )
{
    unsigned int i;

    for (i = 0; i != num_presentations; ++i)
    {
        if (dlb_pmd_set_presentation(model, ps)) return PMD_FAIL;
        ++ps;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_loudness
    (dlb_pmd_model *model
    ,dlb_pmd_loudness *loud
    ,unsigned int num_loudness
    )
{
    unsigned int i;

    for (i = 0; i != num_loudness; ++i)
    {
        if (dlb_pmd_set_loudness(model, loud)) return PMD_FAIL;
        ++loud;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_iat
    (dlb_pmd_model *model
    ,dlb_pmd_identity_and_timing *iat
    ,unsigned int num_iat
    )
{
    if (num_iat)
    {
        assert(num_iat == 1);
        return dlb_pmd_set_iat(model, iat);
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_eac3
    (dlb_pmd_model *model
    ,dlb_pmd_eac3 *eac3
    ,unsigned int num_eac3
    )
{
    unsigned int i;

    for (i = 0; i != num_eac3; ++i)
    {
        if (dlb_pmd_set_eac3(model, eac3)) return PMD_FAIL;
        ++eac3;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_ed2_turnarounds
    (dlb_pmd_model *model
    ,dlb_pmd_ed2_turnaround *edt
    ,unsigned int num_edt
    )
{
    unsigned int i;

    for (i = 0; i != num_edt; ++i)
    {
        if (dlb_pmd_set_ed2_turnaround(model, edt)) return PMD_FAIL;
        ++edt;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_ed2_system
    (dlb_pmd_model *model
    ,dlb_pmd_ed2_system *sys
    ,unsigned int num_eds
    )
{
    if (num_eds)
    {
        assert(num_eds == 1);
        return dlb_pmd_set_ed2_system(model, sys);
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success
add_headphones
    (dlb_pmd_model *model
    ,dlb_pmd_headphone *heds
    ,unsigned int num_heds
    )
{
    unsigned int i;

    for (i = 0; i != num_heds; ++i)
    {
        if (dlb_pmd_set_headphone_element(model, heds)) return PMD_FAIL;
        ++heds;
    }
    return PMD_SUCCESS;
}


/* --------------------------- public API -------------------------------- */


size_t
dlb_pmd_metadata_set_query_memory
    (const dlb_pmd_model *model
    )
{
#define NUM_SOURCES (256)
#define NUM_ELEMENTS (4096)
    dlb_pmd_source sources[NUM_SOURCES];
    dlb_pmd_element_id elements[NUM_ELEMENTS];
    dlb_pmd_presentation_iterator pi;
    dlb_pmd_metadata_count count;
    dlb_pmd_presentation pres;
    dlb_pmd_bed_iterator bi;
    dlb_pmd_bed bed;
    size_t size = 0;

    if (dlb_pmd_count_entities(model, &count)) return 0;
    
    size = ALLOCSIZE(sizeof(dlb_pmd_metadata_set))
        + count.num_objects *            ALLOCSIZE(sizeof(dlb_pmd_object))
        + count.num_updates *            ALLOCSIZE(sizeof(dlb_pmd_update))
        + count.num_loudness *           ALLOCSIZE(sizeof(dlb_pmd_loudness))
        + count.num_iat *                ALLOCSIZE(sizeof(dlb_pmd_identity_and_timing))
        + count.num_eac3 *               ALLOCSIZE(sizeof(dlb_pmd_eac3))
        + count.num_ed2_system *         ALLOCSIZE(sizeof(dlb_pmd_ed2_system))
        + count.num_ed2_turnarounds *    ALLOCSIZE(sizeof(dlb_pmd_ed2_turnaround))
        + count.num_headphone_desc *     ALLOCSIZE(sizeof(dlb_pmd_headphone))
        ;
    /* add beds */
    if (dlb_pmd_bed_iterator_init(&bi, model)) return 0;
    while (!dlb_pmd_bed_iterator_next(&bi, &bed, NUM_SOURCES, sources))
    {
        size += ALLOCSIZE(sizeof(dlb_pmd_bed))
            + bed.num_sources * ALLOCSIZE(sizeof(dlb_pmd_source));
    }
    
    /* add presentations */
    if (dlb_pmd_presentation_iterator_init(&pi, model)) return 0;
    while (!dlb_pmd_presentation_iterator_next(&pi, &pres, NUM_ELEMENTS, elements))
    {
        size += ALLOCSIZE(sizeof(dlb_pmd_presentation))
            + ALLOCSIZE(pres.num_elements * sizeof(dlb_pmd_element_id));
    }
    return size;
}


size_t
dlb_pmd_metadata_set_max_memory
    (void
    )
{
    size_t max_element_size = sizeof(dlb_pmd_bed)
        + DLB_PMD_MAX_BED_SOURCES * sizeof(dlb_pmd_source);
    size_t max_presentation_size = sizeof(dlb_pmd_presentation)
        + DLB_PMD_MAX_AUDIO_ELEMENTS * sizeof(dlb_pmd_element_id);

    size_t size = ALLOCSIZE(sizeof(dlb_pmd_metadata_set))
        + DLB_PMD_MAX_AUDIO_ELEMENTS           * ALLOCSIZE(max_element_size)
        + DLB_PMD_MAX_UPDATES                  * ALLOCSIZE(sizeof(dlb_pmd_update))
        + DLB_PMD_MAX_PRESENTATIONS            * ALLOCSIZE(sizeof(dlb_pmd_loudness))
        + DLB_PMD_MAX_PRESENTATIONS            * ALLOCSIZE(max_presentation_size)
        +                                        ALLOCSIZE(sizeof(dlb_pmd_identity_and_timing))
        + DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS * ALLOCSIZE(sizeof(dlb_pmd_eac3))
        +                                        ALLOCSIZE(sizeof(dlb_pmd_ed2_system))
        + DLB_PMD_MAX_ED2_TURNAROUNDS          * ALLOCSIZE(sizeof(dlb_pmd_ed2_turnaround))
        + DLB_PMD_MAX_HEADPHONE                * ALLOCSIZE(sizeof(dlb_pmd_headphone))
        ;
    return size;
}


dlb_pmd_success
dlb_pmd_create_metadata_set
    (const dlb_pmd_model *model
    ,void *memory
    ,dlb_pmd_metadata_set **out
    )
{
    dlb_pmd_metadata_set *mdset = (dlb_pmd_metadata_set*)memory;
    uintptr_t mem = (uintptr_t)(mdset+1);

    if (   dlb_pmd_count_entities(model, &mdset->count)
        || copy_title(model, &mdset->title)
        || copy_beds(model, &mdset->beds, &mem)
        || copy_objects(model, &mdset->objects, &mem)
        || copy_updates(model, &mdset->updates, &mem)
        || copy_presentations(model, &mdset->presentations, &mem)
        || copy_loudness(model, &mdset->loudness, &mem)
        || copy_iat(model, &mdset->iat, &mem)
        || copy_eac3(model, &mdset->eac3, &mem)
        || copy_ed2_turnarounds(model, &mdset->ed2_turnarounds, &mem)
        || copy_ed2_system(model, &mdset->ed2_system, &mem)
        || copy_headphones(model, &mdset->headphones, &mem)
       )
    {
        *out = NULL;
        return PMD_FAIL;
    }
    *out = mdset;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_ingest_metadata_set
    (      dlb_pmd_model        *model
    ,const dlb_pmd_metadata_set *mdset
    )
{
    const dlb_pmd_metadata_count *count = &mdset->count;
    unsigned int i;
    unsigned int j;

    dlb_pmd_reset(model);

    /* add signals based on objects and bed ids */
    for (i = 0; i < count->num_beds; ++i)
    {
        if (mdset->beds[i].bed_type == PMD_BED_ORIGINAL)
        {
            for (j = 0; j < mdset->beds[i].num_sources; ++j)
            {
                if (!pmd_signals_test(&model->signals, mdset->beds[i].sources[j].source - 1))
                {
                    pmd_signals_add(&model->signals, mdset->beds[i].sources[j].source - 1);
                }
            }
        }
    }

    for (i = 0; i < count->num_objects; ++i)
    {
        if (!pmd_signals_test(&model->signals, mdset->objects[i].source - 1))
        {
            pmd_signals_add(&model->signals, mdset->objects[i].source - 1);
        }
    }
    
    model->num_signals = (uint16_t)pmd_signals_count(&model->signals);

    return add_title          (model, &mdset->title)
        || add_beds           (model, mdset->beds,            count->num_beds)
        || add_objects        (model, mdset->objects,         count->num_objects)
        || add_updates        (model, mdset->updates,         count->num_updates)
        || add_presentations  (model, mdset->presentations,   count->num_presentations)
        || add_loudness       (model, mdset->loudness,        count->num_loudness)
        || add_iat            (model, mdset->iat,             count->num_iat)
        || add_eac3           (model, mdset->eac3,            count->num_eac3)
        || add_ed2_turnarounds(model, mdset->ed2_turnarounds, count->num_ed2_turnarounds)
        || add_ed2_system     (model, mdset->ed2_system,      count->num_ed2_system)
        || add_headphones     (model, mdset->headphones,      count->num_headphone_desc);
}

