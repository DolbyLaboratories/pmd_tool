/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_STUDIO_AUDIO_BEDS_H_
#define PMD_STUDIO_AUDIO_BEDS_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
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
