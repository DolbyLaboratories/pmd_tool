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

#ifndef PMD_STUDIO_AUDIO_PRESENTATIONS_H_
#define PMD_STUDIO_AUDIO_PRESENTATIONS_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"

typedef struct pmd_studio_audio_presentations pmd_studio_audio_presentations;

void
audio_presentation_dynamic_import
    (int pres_import_count
    );

dlb_pmd_success
pmd_studio_audio_presentations_init
    (pmd_studio_audio_presentations **ap1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    );

void
pmd_studio_audio_presentations_refresh_ui
    (pmd_studio_audio_presentations *apres
    );

/**
 * Handle nlang settings changes (and apply changes to model)
 */
void
pmd_studio_audio_presentations_update_nlang
    (pmd_studio_audio_presentations *apres
    );

void
pmd_studio_audio_presentations_enable
    (pmd_studio_audio_presentations *apres
    );

void
pmd_studio_audio_presentations_disable
    (pmd_studio_audio_presentations *apres,
    bool live_mode = false
    );

dlb_pmd_success
pmd_studio_audio_presentations_import
    (pmd_studio_audio_presentations *apres
    ,dlb_pmd_model *m
    );

void
pmd_studio_audio_presentations_reset
    (pmd_studio_audio_presentations *apres
    );

unsigned int
pmd_studio_audio_presentation_get_enabled
    (pmd_studio *studio,
    dlb_pmd_element_id (**ids)[MAX_AUDIO_PRESENTATIONS],
    char* (**names)[MAX_AUDIO_PRESENTATIONS]
    );

void
pmd_studio_presentations_bed_enable
    (dlb_pmd_element_id eid,
     dlb_pmd_bool enable,
     pmd_studio *studio
    );

void
pmd_studio_presentations_add_audio_object_to_presentations
    (pmd_studio *studio,
     dlb_pmd_element_id eid
     );

void
pmd_studio_presentations_object_enable
    (dlb_pmd_element_id eid,
     dlb_pmd_bool enable,
     pmd_studio *studio
    );

void
pmd_studio_audio_presentations_update_model
    (pmd_studio_audio_presentations *apres,
     dlb_pmd_model *m
     );

dlb_pmd_success pmd_studio_audio_presentations_get_mix_matrix(
    unsigned int id,
    dlb_pmd_speaker_config config,
    pmd_studio_mix_matrix mix_matrix,
    pmd_studio *studio);

void pmd_studio_audio_presentations_handle_element_eid_update 
    (pmd_studio *studio
    ,dlb_pmd_element_id old_eid
    ,dlb_pmd_element_id new_eid
    );

void
pmd_studio_audio_presentations_print_debug(
    pmd_studio *studio);

void
pmd_studio_audio_presentations_finish
    (pmd_studio_audio_presentations *apres
    );


/**
 * Get array of supported languages
 * @param lang_ptr pointer to be set
 * @returns number of entries in the array
 */
unsigned int
pmd_studio_get_supported_languages
    (const char ***lang_ptr
    );

unsigned int
pmd_studio_get_default_language_index
    ();

int
pmd_studio_language_index_from_lang
    (const char *lang
    );

#endif /* PMD_STUDIO_AUDIO_PRESENTATIONS_H_ */
