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

#ifndef EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_OBJECTS_PVT_H
#define EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_OBJECTS_PVT_H

#include "ui.h"
#include "pmd_studio.h"

typedef struct
{
	pmd_studio_audio_objects *objects;

    dlb_pmd_bool enabled;
    dlb_pmd_object object;
    uiCombobox *ty;
    uiEntry *name;
    uiLabel *label;
    uiCombobox *ch;
    uiCheckbox *enable;
    uiCheckbox *diverge;
    uiCombobox *gain;
    uiCombobox *x;
    uiCombobox *y;
    uiCombobox *z;
} pmd_studio_audio_object;


struct pmd_studio_audio_objects
{
	pmd_studio *studio;
	uiGrid *grid;
    uiWindow *window;
    uiButton *add_object_button;
    pmd_studio_audio_object objects[MAX_AUDIO_OBJECTS];
    dlb_pmd_element_id object_eids[MAX_AUDIO_OBJECTS];
    unsigned int object_count;

};


#endif //EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_OBJECTS_PVT_H
