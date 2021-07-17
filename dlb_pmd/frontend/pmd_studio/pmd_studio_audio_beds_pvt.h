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
