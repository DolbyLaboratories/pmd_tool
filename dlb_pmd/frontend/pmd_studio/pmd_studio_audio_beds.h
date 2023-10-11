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

#ifndef PMD_STUDIO_AUDIO_BEDS_H_
#define PMD_STUDIO_AUDIO_BEDS_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_common_defs.h"
#include "pmd_studio.h"
#include "mix_matrix.h"

typedef struct pmd_studio_audio_beds pmd_studio_audio_beds;
    
void
onBedGainUpdated
    (uiCombobox *c
    ,void *data
    );

dlb_pmd_success
pmd_studio_audio_beds_init
    (pmd_studio_audio_beds **ab1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *s
    );

void
pmd_studio_audio_beds_refresh_ui
    (pmd_studio_audio_beds *abeds
    );

dlb_pmd_success
pmd_studio_audio_beds_import
    (pmd_studio_audio_beds *abeds
    ,dlb_pmd_model *m
    );

void
pmd_studio_audio_beds_reset
    (pmd_studio_audio_beds *abeds
    );

void
pmd_studio_audio_beds_enable
    (pmd_studio_audio_beds *abeds
    );

void
pmd_studio_audio_beds_disable
    (pmd_studio_audio_beds *abeds,
    bool live_mode=false);
    
void
pmd_studio_audio_beds_update_model
    (pmd_studio_audio_beds *abeds,
     dlb_pmd_model *m
    );

dlb_pmd_success pmd_studio_audio_beds_get_mix_matrix(
    unsigned int index,
    dlb_pmd_speaker_config config,
    pmd_studio_mix_matrix mix_matrix,
    pmd_studio *studio);

/**
 * Get list of available bed element ids.
 * @param enabled_only only populate with enabled bed elements.
 */
unsigned int 
pmd_studio_audio_beds_get_eids(
    dlb_pmd_element_id **eid_list,
    unsigned int **bed_labels,
    pmd_studio *s,
    bool enabled_only = true        // Get enabled beds only
    );

void
pmd_studio_audio_beds_print_debug
    (pmd_studio *s
    );

void
pmd_studio_audio_beds_finish
    (pmd_studio_audio_beds *abeds
    );

/**
 * Get bed channel configuration by element id
 */
dlb_pmd_speaker_config
pmd_studio_audio_beds_get_bed_config
    (pmd_studio *studio
    ,dlb_pmd_element_id bed_id
    );

dlb_pmd_success 
pmd_studio_set_bed_gain
    (pmd_studio *studio
    ,dlb_pmd_element_id bed_eid
    ,float gain_db
    );

#endif /* PMD_STUDIO_AUDIO_BEDS_H_ */
