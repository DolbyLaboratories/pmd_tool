/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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