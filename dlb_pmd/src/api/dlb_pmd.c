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
 * @file dlb_pmd.c
 * @brief top-level API for PMD, core definitions
 */

#include "dlb_pmd_api.h"
#include "dlb_pmd_api_version.h"
#include "pmd_model.h"
#include "pmd_error_helper.h"

#include <math.h>


/**
 * @def ALIGN_TO(size,A)
 * @brief align size to next multiple of A
 */
#define ALIGN_TO(size,A) ((((size) + (A)-1)/(A))*(A))


/**
 * @def ALIGN_TO_MPTR(size)
 * @brief align a size to be a multiple of void*
 */
#define ALIGN_TO_MPTR(size) ALIGN_TO(size, sizeof(void*))


void
dlb_pmd_library_version
    (unsigned int *epoch
    ,unsigned int *maj
    ,unsigned int *min
    ,unsigned int *build
    ,unsigned int *bs_maj
    ,unsigned int *bs_min
    )
{
    *epoch   = DLB_PMD_VERSION_EPOCH;
    *maj     = DLB_PMD_VERSION_MAJOR;
    *min     = DLB_PMD_VERSION_MINOR;
    *build   = DLB_PMD_VERSION_BUILD;
    *bs_maj  = PMD_BITSTREAM_VERSION_MAJOR;
    *bs_min  = PMD_BITSTREAM_VERSION_MINOR;
}


void
dlb_pmd_max_constraints
    (dlb_pmd_model_constraints  *c
    ,dlb_pmd_bool                use_adm_common_defs
    )
{
    if (NULL != c)
    {
        pmd_profile p;

        pmd_profile_max(&p);
        p.constraints.use_adm_common_defs = use_adm_common_defs;
        memcpy(c, &p.constraints, sizeof(*c));
    }
}


/**
 * @brief check that input constraints make sense
 */
static
dlb_pmd_bool
check_constraints
    (const dlb_pmd_model_constraints *constraints
    )
{
    if (constraints)
    {
        return constraints->max_presentation_names >= constraints->max.num_presentations
            && constraints->max_elements > 0
            && constraints->max.num_signals > 0
            && (constraints->max.num_beds > 0 || constraints->max.num_objects > 0)
            && constraints->max.num_presentations > 0
            && constraints->max.num_iat < 2
            && constraints->max.num_ed2_system < 2;
    }
    return PMD_FALSE;
}


size_t
dlb_pmd_query_mem_constrained
    (const dlb_pmd_model_constraints *constraints
    )
{
    if (check_constraints(constraints))
    {
        const dlb_pmd_metadata_count *max = &constraints->max;
        unsigned int max_elements = constraints->max_elements;
        if (!max_elements)
        {
            max_elements = max->num_beds + max->num_objects;
        }
        return ALIGN_TO_MPTR(sizeof(dlb_pmd_model))
            +  ALIGN_TO_MPTR(max_elements * sizeof(pmd_element))
            +  ALIGN_TO_MPTR(max_elements * sizeof(pmd_aen))
            +  ALIGN_TO_MPTR(max->num_presentations * sizeof(pmd_apd))
            +  ALIGN_TO_MPTR(max->num_updates * sizeof(pmd_xyz))
            +  ALIGN_TO_MPTR(max->num_loudness * sizeof(pmd_pld))
            +  ALIGN_TO_MPTR((!!max->num_iat) * sizeof(pmd_iat))
            +  ALIGN_TO_MPTR(max->num_eac3 * sizeof(pmd_eep))
            +  ALIGN_TO_MPTR((!!max->num_ed2_system) * sizeof(pmd_esd))
            +  ALIGN_TO_MPTR(max->num_ed2_turnarounds * sizeof(pmd_etd))
            +  ALIGN_TO_MPTR(max->num_headphone_desc * sizeof(pmd_hed))
            +  ALIGN_TO_MPTR(constraints->max_presentation_names * sizeof(pmd_apn))
            ;
    }
    return 0;
}

    
#define ASSIGN_AND_INC(element, type, count)                            \
    {                                                                   \
        element = NULL;                                                 \
        if (count)                                                      \
        {                                                               \
            element = (type *)mem;                                      \
            mem += ALIGN_TO_MPTR((count) * sizeof(type));               \
        }                                                               \
    }
    

void
dlb_pmd_init_constrained
    (      dlb_pmd_model **modelptr
    ,const dlb_pmd_model_constraints *constraints
    ,      void *memvoid
    )
{
    pmd_bool mallocated = PMD_FALSE;

    if (modelptr == NULL || constraints == NULL)
    {
        return;
    }

    if (memvoid == NULL)
    {
        size_t sz = dlb_pmd_query_mem_constrained(constraints);

        memvoid = malloc(sz);
        if (memvoid != NULL)
        {
            mallocated = PMD_TRUE;
        }
    }

    *modelptr = NULL;
    if (memvoid)
    {
        const dlb_pmd_metadata_count *max = &constraints->max;
        uint8_t *mem = (uint8_t*)memvoid;
        dlb_pmd_model *model;
        unsigned int max_elements = constraints->max_elements;

        if (!max_elements)
        {
            max_elements = max->num_beds + max->num_objects;
        }

        ASSIGN_AND_INC(model,                dlb_pmd_model, 1);
        ASSIGN_AND_INC(model->element_list,  pmd_element, max_elements);
        ASSIGN_AND_INC(model->aen_list,      pmd_aen,     max_elements);
        ASSIGN_AND_INC(model->apd_list,      pmd_apd,     max->num_presentations);
        ASSIGN_AND_INC(model->pld_list,      pmd_pld,     max->num_loudness);
        ASSIGN_AND_INC(model->xyz_list,      pmd_xyz,     max->num_updates);
        ASSIGN_AND_INC(model->iat,           pmd_iat,     !!max->num_iat);
        ASSIGN_AND_INC(model->eep_list,      pmd_eep,     max->num_eac3);
        ASSIGN_AND_INC(model->esd,           pmd_esd,     !!max->num_ed2_system);
        ASSIGN_AND_INC(model->etd_list,      pmd_etd,     max->num_ed2_turnarounds);
        ASSIGN_AND_INC(model->hed_list,      pmd_hed,     max->num_headphone_desc);
        ASSIGN_AND_INC(model->apn_list.pool, pmd_apn,     constraints->max_presentation_names);
        
        model->limits = *constraints;

        pmd_model_init(model);
        pmd_mutex_init(&model->lock);
        model->version_avail = 1;
        model->version_maj = PMD_BITSTREAM_VERSION_MAJOR;
        model->version_min = PMD_BITSTREAM_VERSION_MINOR;
        model->mallocated = mallocated;
        *modelptr = model;
    }
}


size_t
dlb_pmd_query_mem_profile
    (unsigned int profile
    ,unsigned int level
    )
{
    pmd_profile p;

    if (!pmd_profile_set(&p, profile, level, NULL))
    {
        return dlb_pmd_query_mem_constrained(&p.constraints);
    }
    return 0;
}


void
dlb_pmd_init_profile
    (dlb_pmd_model **modelptr
    ,unsigned int profile
    ,unsigned int level
    ,void *mem
    )
{
    pmd_profile p;
    *modelptr = NULL;
    if (!pmd_profile_set(&p, profile, level, NULL))
    {
        dlb_pmd_init_constrained(modelptr, &p.constraints, mem);
    }
}


void
dlb_pmd_get_constraints
    (const dlb_pmd_model *model
    ,dlb_pmd_model_constraints *c
    )
{
    memmove(c, &model->limits, sizeof(model->limits));
}


size_t
dlb_pmd_query_mem
    (void
    )
{
    return dlb_pmd_query_mem_profile(0, 0);
}


void
dlb_pmd_init
    (dlb_pmd_model **modelptr
    ,void *mem
    )
{
    dlb_pmd_init_profile(modelptr, 0, 0, mem);
}

 
dlb_pmd_success
dlb_pmd_reset
    (dlb_pmd_model *model
    )
{
    pmd_model_init(model);
    model->version_avail = 1;
    model->version_maj = PMD_BITSTREAM_VERSION_MAJOR;
    model->version_min = PMD_BITSTREAM_VERSION_MINOR;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_set_error_callback
    (dlb_pmd_model                      *model
    ,dlb_pmd_model_error_callback        fn
    ,dlb_pmd_model_error_callback_arg    cbarg
    )
{
    if (model == NULL)
    {
        return PMD_FAIL;
    }

    model->error_callback = fn;
    model->error_cbarg = cbarg;

    return PMD_SUCCESS;
}


void
dlb_pmd_finish
    (dlb_pmd_model *model
    )
{
    pmd_mutex_finish(&model->lock);
    if (model->mallocated)
    {
        free(model);
    }
    return;
}


#define COMPARE(model, larger, smaller, field)                          \
    if (larger->field < smaller->field)                                 \
    {                                                                   \
        error(model, "destination model " #field " %u too small (need %u)\n", \
              larger->field, smaller->field);                           \
        return 0;                                                       \
    }                                                                   \
                                                                        \

/**
 * @brief determine if one set of constraints is large enough to include another
 */
static
dlb_pmd_bool
constraints_larger
    (dlb_pmd_model *model
    ,dlb_pmd_model_constraints *larger
    ,dlb_pmd_model_constraints *smaller
    )
{
    COMPARE(model, larger, smaller, max_elements);
    COMPARE(model, larger, smaller, max_presentation_names);
    COMPARE(model, larger, smaller, max.num_signals);
    COMPARE(model, larger, smaller, max.num_updates);
    COMPARE(model, larger, smaller, max.num_presentations);
    COMPARE(model, larger, smaller, max.num_loudness);
    COMPARE(model, larger, smaller, max.num_iat);
    COMPARE(model, larger, smaller, max.num_eac3);
    COMPARE(model, larger, smaller, max.num_ed2_system);
    COMPARE(model, larger, smaller, max.num_ed2_turnarounds);
    COMPARE(model, larger, smaller, max.num_headphone_desc);
    /* TODO: compare use_adm_common_defs? */
    return 1;
}


/**
 * @def COPY_ONEOF(tmp, dest, src, field, type)
 * @brief helper macro to define copying optional single entity, IAT or ESD
 */
#define COPY_ONEOF(tmp, dest, src, field, type)                  \
    tmp.field = dest->field;                                     \
    if (dest->field)                                             \
    {                                                            \
        if (src->field)                                          \
        {                                                        \
            memmove(dest->field, src->field, sizeof(type));      \
        }                                                        \
        else                                                     \
        {                                                        \
            memset(dest->field, '\0', sizeof(type));             \
        }                                                        \
    }                                                            \


/**
 * @def COPY_ENTITY
 * @brief helper macro to define copying a list of elements
 */
#define COPY_ENTITY(tmp, dest, src, field, count)                       \
    tmp.field = dest->field;                                            \
    memmove(dest->field, src->field, sizeof(src->field[0]) * count);    \



dlb_pmd_success
dlb_pmd_copy
    (dlb_pmd_model *dest
    ,const dlb_pmd_model *src
    )
{
    unsigned int offset = offsetof(dlb_pmd_model, title);
    dlb_pmd_model_constraints scon;
    dlb_pmd_model tmp;
    uint8_t *d; 
    uint8_t *s; 
    
    FUNCTION_PROLOGUE(dest);
    FUNCTION_PROLOGUE(src);

    if (dlb_pmd_count_entities(src, &scon.max))
    {
        return PMD_FAIL;
    }
    
    scon.max_elements = scon.max.num_objects + scon.max.num_beds;
    scon.max_presentation_names = src->apn_list.num;
    if (!constraints_larger(dest, &dest->limits, &scon))
    {
        return PMD_FAIL;
    }
    scon.use_adm_common_defs = src->limits.use_adm_common_defs;

    /* tmp will be a clone of dest until we fix it up */
    memmove(&tmp, dest, sizeof(tmp));
    d = (uint8_t*)&tmp + offset;
    s = (uint8_t*)src + offset;
    memmove(d, s, sizeof(tmp) - offset);
    
    /* tmp will now point to entity arrays in src; one-by-one
     * fixup these pointers to point to the destination model's
     * arrays, and copy their contents
     */
    COPY_ENTITY(tmp, dest, src, element_list, tmp.limits.max_elements);
    COPY_ENTITY(tmp, dest, src, apd_list,     tmp.limits.max.num_presentations);
    COPY_ENTITY(tmp, dest, src, pld_list,     tmp.limits.max.num_loudness);
    COPY_ENTITY(tmp, dest, src, xyz_list,     tmp.limits.max.num_updates);
    COPY_ENTITY(tmp, dest, src, aen_list,     tmp.limits.max_elements);
    COPY_ENTITY(tmp, dest, src, eep_list,     tmp.limits.max.num_eac3);
    COPY_ENTITY(tmp, dest, src, etd_list,     tmp.limits.max.num_ed2_turnarounds);
    COPY_ENTITY(tmp, dest, src, hed_list,     tmp.limits.max.num_headphone_desc);

    COPY_ONEOF(tmp, dest, src, iat, pmd_iat);
    COPY_ONEOF(tmp, dest, src, esd, pmd_esd);

    /* APN needs to be copied separately */
    tmp.apn_list.pool = dest->apn_list.pool;
    memmove(tmp.apn_list.pool, src->apn_list.pool, sizeof(pmd_apn) * src->apn_list.max);

    /* finally, copy everything back to the destination */
    memmove(dest, &tmp, sizeof(tmp));
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_default_presentation
    (const dlb_pmd_model *model
    ,dlb_pmd_presentation *pres
    ,unsigned int num_elements
    ,dlb_pmd_element_id *elements
    )
{
    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, pres);
    CHECK_PTRARG(model, elements);

    if (model->num_apd > 0)
    {
        dlb_pmd_presentation_id id = model->apd_list[0].id;
        return dlb_pmd_presentation_lookup(model, id, pres, num_elements, elements);
    }
    return PMD_FAIL;
}


dlb_pmd_success
dlb_pmd_apply_updates
    (dlb_pmd_model *model
    ,dlb_pmd_frame_rate rate
    )
{
    unsigned int i;
    pmd_xyz *update;

    FUNCTION_PROLOGUE(model);

    update = model->xyz_list;
    for (i = 0; i != model->num_xyz; ++i, ++update)
    {
        pmd_model_apply_update(model, update);
    }
    memset(model->xyz_list, '\xff', sizeof(*model->xyz_list) * model->limits.max.num_updates);
    model->num_xyz = 0;

    if (model->iat && rate < NUM_PMD_FRAMERATES)
    {
        static const uint64_t frame_ticks[NUM_PMD_FRAMERATES] =
        {
            /*  23.98 */ 10010,
            /*  24    */ 10000,
            /*  25    */  9600,
            /*  29.97 */  8008,
            /*  30    */  8000,
            /*  50    */  4800,
            /*  59.94 */  4004,
            /*  60    */  4000,
            /* 100    */  2400,
            /* 119.88 */  2002,
            /* 120    */  2000
        };
        model->iat->timestamp += frame_ticks[rate];
    }

    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_prune_unused_signals
    (dlb_pmd_model *m
    )
{
    dlb_pmd_source sources[DLB_PMD_MAX_BED_SOURCES];
    dlb_pmd_object_iterator oi;
    dlb_pmd_bed_iterator bi;
    dlb_pmd_object object;
    pmd_signals unseen;
    dlb_pmd_bed bed;
    uint16_t count;
    unsigned int i;

    FUNCTION_PROLOGUE(m);

    count = 0;
    pmd_signals_copy(&unseen, &m->signals);

    if (dlb_pmd_bed_iterator_init(&bi, m)) return PMD_FAIL;
    while (PMD_SUCCESS == dlb_pmd_bed_iterator_next(&bi, &bed, DLB_PMD_MAX_BED_SOURCES, sources))
    {
        if (PMD_BED_ORIGINAL == bed.bed_type)
        {
            for (i = 0; i != bed.num_sources; ++i)
            {
                if (pmd_signals_test(&unseen, bed.sources[i].source-1))
                {
                    pmd_signals_remove(&unseen, bed.sources[i].source-1);
                    ++count;
                }
            }
        }
    }

    if (dlb_pmd_object_iterator_init(&oi, m)) return PMD_FAIL;
    while (PMD_SUCCESS == dlb_pmd_object_iterator_next(&oi, &object))
    {
        if (pmd_signals_test(&unseen, object.source-1))
        {
            pmd_signals_remove(&unseen, object.source-1);
            ++count;
        }
    }

    pmd_signals_subtract(&m->signals, &unseen);
    m->num_signals = count;
    return PMD_SUCCESS;
}


dlb_pmd_success
dlb_pmd_initialize_payload_set_status
    (dlb_pmd_payload_set_status     *payload_set_status
    ,dlb_pmd_payload_status_record  *xyz_status
    ,unsigned int                    xyz_count
    )
{
    dlb_pmd_success success = PMD_FAIL;

    if (payload_set_status)
    {
        memset(payload_set_status, 0, sizeof(*payload_set_status));
        if (xyz_status && xyz_count)
        {
            memset(xyz_status, 0, xyz_count * sizeof(*xyz_status));
            payload_set_status->xyz_payload_status = xyz_status;
            payload_set_status->xyz_payload_count_max = xyz_count;
            success = PMD_SUCCESS;
        }
        else if (!xyz_status && !xyz_count)
        {
            success = PMD_SUCCESS;
        }
    }

    return success;
}


dlb_pmd_success
dlb_pmd_initialize_payload_set_status_with_callback
    (dlb_pmd_payload_set_status             *payload_set_status
    ,dlb_pmd_payload_status_record          *xyz_status
    ,unsigned int                            xyz_count
    ,void                                   *callback_arg
    ,dlb_pmd_payload_set_status_callback     callback
    )
{
    dlb_pmd_success success = dlb_pmd_initialize_payload_set_status(payload_set_status, xyz_status, xyz_count);

    if (success == PMD_SUCCESS)
    {
        payload_set_status->callback_arg = callback_arg;
        payload_set_status->callback = callback;
    }

    return success;
}

void
dlb_pmd_clear_payload_set_status
    (dlb_pmd_payload_set_status *payload_set_status
    )
{
    if (payload_set_status != NULL)
    {
        dlb_pmd_bool count_frames = payload_set_status->count_frames;
        uint64_t frame_count = payload_set_status->frame_count;
        uint64_t burst_count = payload_set_status->burst_count;

        (void)dlb_pmd_initialize_payload_set_status_with_callback(payload_set_status,
                                                                  payload_set_status->xyz_payload_status,
                                                                  payload_set_status->xyz_payload_count_max,
                                                                  payload_set_status->callback_arg,
                                                                  payload_set_status->callback);
        payload_set_status->count_frames = count_frames;
        payload_set_status->frame_count = frame_count;
        payload_set_status->burst_count = burst_count;
    }
}

/* This is just a convenient place to put this */
dlb_pmd_bool
is_infinity
    (float f)
{
    return (dlb_pmd_bool)isinf(f);
}
