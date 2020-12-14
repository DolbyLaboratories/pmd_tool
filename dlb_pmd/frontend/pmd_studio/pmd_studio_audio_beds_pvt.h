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

#ifndef EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_BEDS_PVT_H
#define EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_BEDS_PVT_H

extern "C"{
    #include "dlb_pmd_api.h"
    #include "ui.h"
}
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_limits.h"

#define MAX_BED_SOURCES (16)
/* Definitions */

const float config_mix_coefs[NUM_PMD_SPEAKER_CONFIGS] = 
{
	1.0, 1.0
};


typedef struct
{
    pmd_studio_audio_beds *audio_beds;

    dlb_pmd_bool   enabled;
    dlb_pmd_bed    bed;
    dlb_pmd_source sources[MAX_BED_SOURCES];
    uiLabel *label;
    uiCombobox *cfg;
    uiEntry *name;
    uiCombobox *gain;
    uiCombobox *start;
    uiCheckbox *enable;

} pmd_studio_audio_bed;


struct pmd_studio_audio_beds
{
	pmd_studio *studio;

    uiWindow *window;
    pmd_studio_audio_bed beds[MAX_AUDIO_BEDS];
    uiButton *add_bed_button;
    unsigned int bed_count;
    uiGrid *grid;
    dlb_pmd_element_id bed_eids[MAX_AUDIO_BEDS];
    unsigned int bed_labels[MAX_AUDIO_BEDS];
};
    

#endif //EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_BEDS_PVT_H
