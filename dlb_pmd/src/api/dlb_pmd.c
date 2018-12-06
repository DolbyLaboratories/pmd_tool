/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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
#include "pmd_model.h"

/**
 * @def DLB_PMD_VERSION_MAJOR
 * @brief current implementation major version number
 *
 * Changes to major version reflects breakage in compatibility with
 * previous versions.  Alternatively, it may signify a significant
 * rewrite or internal change which the maintainers will wish to
 * headline with a new number.
 */
#define DLB_PMD_VERSION_MAJOR (3)


/**
 * @def DLB_PMD_VERSION_MINOR
 * @brief current implementation minor version number
 *
 * Changes to minor version typically reflect feature additions that
 * are backwards compatible with previous versions.
 */
#define DLB_PMD_VERSION_MINOR (0)


/**
 * @def DLB_PMD_VERSION_RELEASE
 * @brief current implementation release number
 * 
 * Changes to release numbers indicate things like fixes, tidy-ups,
 * comments or documentation etc.
 */
#define DLB_PMD_VERSION_RELEASE (0)


void
dlb_pmd_library_version
    (unsigned int *maj
    ,unsigned int *min
    ,unsigned int *release
    ,unsigned int *bs_maj
    ,unsigned int *bs_min
    )
{
    *maj     = DLB_PMD_VERSION_MAJOR;
    *min     = DLB_PMD_VERSION_MINOR;
    *release = DLB_PMD_VERSION_RELEASE;
    *bs_maj  = PMD_BITSTREAM_VERSION_MAJOR;
    *bs_min  = PMD_BITSTREAM_VERSION_MINOR;
}


size_t
dlb_pmd_query_mem
    (void
    )
{
    return sizeof(struct dlb_pmd_model);
}


void
dlb_pmd_init
    (dlb_pmd_model **modelptr
    ,void *mem
    )
{
    dlb_pmd_model *model = (dlb_pmd_model*)mem;
    *modelptr = model;
    pmd_model_init(model);
    pmd_mutex_init(&model->lock);
    model->version_avail = 1;
    model->version_maj = PMD_BITSTREAM_VERSION_MAJOR;
    model->version_min = PMD_BITSTREAM_VERSION_MINOR;
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


void
dlb_pmd_finish
    (dlb_pmd_model *model
    )
{
    pmd_mutex_finish(&model->lock);
    return;
}


void
dlb_pmd_copy
    (dlb_pmd_model *dest
    ,const dlb_pmd_model *src
    )
{
    unsigned int offset = offsetof(dlb_pmd_model, title);
    unsigned int size = sizeof(dlb_pmd_model) - offset;
    uint8_t *d = (uint8_t*)dest + offset;
    uint8_t *s = (uint8_t*)src + offset;

    memmove(d, s, size);
}


dlb_pmd_success
dlb_pmd_default_presentation
    (const dlb_pmd_model *model
    ,dlb_pmd_presentation *pres
    ,unsigned int num_elements
    ,dlb_pmd_element_id *elements
    )
{
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
    )
{
    unsigned int i;
    pmd_xyz *update = model->xyz_list;
    for (i = 0; i != model->num_xyz; ++i, ++update)
    {
        pmd_model_apply_update(model, update);
    }
    memset(model->xyz_list, '\xff', sizeof(model->xyz_list));
    model->num_xyz = 0;
    return PMD_SUCCESS;
}

