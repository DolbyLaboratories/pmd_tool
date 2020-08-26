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
pmd_studio_audio_objects_finish(
    pmd_studio_audio_objects *aobj
    );

unsigned int
pmd_studio_object_get_count
    (pmd_studio *studio
    );


#endif /* PMD_STUDIO_AUDIO_OBJECTS_H_ */
