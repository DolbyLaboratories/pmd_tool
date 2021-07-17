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


#ifndef __PMD_STUDIO_AUDIO_PRSESENTATIONS_H__
#define __PMD_STUDIO_AUDIO_PRSESENTATIONS_H__

#include "pmd_studio.h"

#define MAX_PRES_ELEMENTS (MAX_AUDIO_BEDS + MAX_AUDIO_OBJECTS)

typedef struct
{
    pmd_studio_audio_presentations *presentations;
    dlb_pmd_bool enabled;
    dlb_pmd_presentation presentation;
    dlb_pmd_element_id elements[MAX_PRES_ELEMENTS];

    uiLabel *label;
    uiCheckbox *enable;
    uiCombobox *nlang;
    uiCombobox *plang;
    uiCombobox *bed;
    /* allocating space for the max amount of audio objects: */
    uiCheckbox *checkBoxes[MAX_AUDIO_OBJECTS];
    uiEntry *name;
} pmd_studio_audio_presentation;

struct pmd_studio_audio_presentations
{
	pmd_studio *studio;
    uiWindow *window;
    uiButton *add_pres_button;
    pmd_studio_audio_presentation presentations[MAX_AUDIO_PRESENTATIONS];
    unsigned int object_enable_mask;
    unsigned int presentation_count;
    unsigned int num_checkboxes;
    unsigned int num_beds;

    /* Temporary storage for query call */
    dlb_pmd_element_id presentation_ids[MAX_AUDIO_PRESENTATIONS];
    char *presentation_names[MAX_AUDIO_PRESENTATIONS];

    uiGrid *grid;
};


#endif