/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_STUDIO_AUDIO_OUTPUTS_H_
#define PMD_STUDIO_AUDIO_OUTPUTS_H_

extern "C"{
#include "ui.h"
#include "dlb_pmd_api.h"
}
#include "pmd_studio_limits.h"
#include "pmd_studio.h"
#include "pmd_studio_device.h"

struct pmd_studio_outputs;
struct pmd_studio_metadata_output;

dlb_pmd_success
pmd_studio_outputs_init
    (pmd_studio_outputs **ao1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    );


dlb_pmd_success
pmd_studio_audio_output_update_presentation_names
    (pmd_studio *studio);

void
pmd_studio_outputs_refresh_ui(
    pmd_studio_outputs *aouts
    );

void
pmd_studio_outputs_enable
    (pmd_studio_outputs *outs
    );

void
pmd_studio_outputs_disable
    (pmd_studio_outputs *outs,
    bool live_mode = false
    );

void
pmd_studio_outputs_stop_all_metadata_outputs
    (pmd_studio *studio);

dlb_pmd_success 
pmd_studio_audio_outputs_get_mix_matrix
    (pmd_studio_mix_matrix mix_matrix
    ,pmd_studio *studio
    );

void
pmd_studio_outputs_reset
    (pmd_studio_outputs *ao1
    );


dlb_pmd_success
pmd_studio_outputs_finish
    (pmd_studio_outputs *ao1
    );

void
pmd_studio_outputs_print_debug
    (pmd_studio *studio
    );

/**
 * Update single metadata output with new pmd model.
 */
dlb_pmd_success 
pmd_studio_audio_outputs_update_metadata_output
    (pmd_studio_metadata_output *mout
    );

/**
 * Update all metadata outputs with new pmd model.
 */
void 
pmd_studio_audio_outputs_update_metadata_outputs
    (pmd_studio* s
    );

/**
 * Add links between metadata output and ring buffer struct for
 * mutex access and more.
 */
void
pmd_studio_metadata_output_assign_ring_buffer_struct
    (pmd_studio_metadata_output *mdout
    ,pmd_studio_ring_buffer_struct *assigned_struct
    );

/**
 * Returns true if there is at least one metadata output active
 */
dlb_pmd_bool
pmd_studio_metadata_output_active
    (pmd_studio *studio
    );

/**
 * Callback for when augmentor fails to generate metadata from model
 */
void
pmd_studio_on_augmentor_fail_cb
    (void* data
    ,dlb_pmd_model *model
    );


#endif /* PMD_STUDIO_AUDIO_OUTPUTS_H_ */
