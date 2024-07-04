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
