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

#ifndef PMD_STUDIO_AUDIO_OBJECTS_H_
#define PMD_STUDIO_AUDIO_OBJECTS_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"
#include "pmd_studio_audio_presentations.h"

typedef struct pmd_studio_audio_objects pmd_studio_audio_objects;

void 
audio_object_dynamic_import
    (int obj_import_count
    );

void
onObjectGainUpdated
    (uiCombobox *c
    ,void *data
    );

dlb_pmd_success
pmd_studio_audio_objects_init
    (pmd_studio_audio_objects **ao1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    );

void
pmd_studio_audio_objects_refresh_ui
    (pmd_studio_audio_objects *aobj
    );

dlb_pmd_success
pmd_studio_audio_objects_import
    (pmd_studio_audio_objects *aobj
    ,dlb_pmd_model *m
    );


void
pmd_studio_audio_objects_reset
    (pmd_studio_audio_objects *aobj
    );

void
pmd_studio_audio_objects_update_model(
        pmd_studio_audio_objects *aobjs,
         dlb_pmd_model *m   
    );

dlb_pmd_success pmd_studio_audio_objects_get_mix_matrix(

    unsigned int id,
    dlb_pmd_speaker_config config,
    pmd_studio_mix_matrix mix_matrix,
    pmd_studio *studio);

void
pmd_studio_audio_objects_print_debug(
    pmd_studio *studio);

void
pmd_studio_audio_objects_finish(
    pmd_studio_audio_objects *aobj
    );

void
pmd_studio_audio_objects_disable
    (pmd_studio_audio_objects *aobjs,
    bool live_mode = false
    );

void
pmd_studio_audio_objects_enable
    (pmd_studio_audio_objects *aobjs
    );
    
/**
 * Get list of available object element ids
 * @param enabled_only list enabled objects only
 */
unsigned int 
pmd_studio_audio_objects_get_eids(
    dlb_pmd_element_id **eid_list,
    pmd_studio *s,
    bool enabled_only = true
    );


dlb_pmd_success 
pmd_studio_set_obj_gain
    (pmd_studio *studio
    ,dlb_pmd_element_id obj_eid
    ,float gain_db
    );



#endif /* PMD_STUDIO_AUDIO_OBJECTS_H_ */
