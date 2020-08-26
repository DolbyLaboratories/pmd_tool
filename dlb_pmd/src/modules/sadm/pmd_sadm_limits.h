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

#ifndef PMD_SADM_LIMITS_H_
#define PMD_SADM_LIMITS_H_

#include "dlb_pmd_api.h"
#include "sadm/dlb_sadm_model.h"
#include <string.h>

#define SADM_LIMITS_MAX_CHANNELS_PER_BED (16)

static inline
void
compute_sadm_limits
    (dlb_pmd_model_constraints *c
    ,dlb_sadm_counts *sc
    )
{
    memset(sc, '\0', sizeof(*sc));

    sc->max_programme_labels   = 16;
    sc->max_programme_contents = c->max_elements;
    sc->max_content_objects    = SADM_LIMITS_MAX_CHANNELS_PER_BED;
    sc->max_object_objects     = MAX_AO_AO;
    sc->max_object_track_uids  = SADM_LIMITS_MAX_CHANNELS_PER_BED;
    sc->max_packfmt_chanfmts   = SADM_LIMITS_MAX_CHANNELS_PER_BED;
    sc->max_chanfmt_blkfmts    = 1 + 1; /* +1 for Fhg and NHK SADM content */
    
    sc->num_programmes = c->max.num_presentations;
    sc->num_track_uids = c->max_elements * SADM_LIMITS_MAX_CHANNELS_PER_BED;
    sc->num_objects    = c->max_elements;
    
    sc->num_contents   = c->max_elements;
    sc->num_packfmts   = sc->num_objects;
    sc->num_chanfmts   = sc->num_objects * SADM_LIMITS_MAX_CHANNELS_PER_BED;
    sc->num_blkfmts    = sc->num_chanfmts * sc->max_chanfmt_blkfmts;

    sc->use_common_defs = c->use_adm_common_defs;
}


#endif /* PMD_SADM_LIMITS_H_ */
