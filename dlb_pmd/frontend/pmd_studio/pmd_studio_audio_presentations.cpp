/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2023, Dolby Laboratories Inc.
 * Copyright (c) 2019-2023, Dolby International AB.
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

#include <string.h>
#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_common_defs.h"
#include "pmd_studio.h"
#include "pmd_studio_audio_presentations.h"
#include "pmd_studio_audio_presentations_pvt.h"
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_pvt.h"
#include "pmd_studio_settings_pvt.h"
#include "mix_matrix.h"
/* Definitions */


/**
 * @brief list of supported languages in the UI
 *
 * for a complete list of languages supported by the library, see
 * pmd_language.c source code file
 */
static const char *SUPPORTED_LANGUAGES[] =
{
    "alb", "bul", "cze", "dan", "dut", "eng", "est", "fin", "fre", "ger", "gre", "hun", "ice", "ita",
    "lav", "lit", "nor", "pol", "por", "qaa", "rum", "rus", "slo", "slv", "spa", "srp", "swe", "ukr", "und"
};

#define NUM_SUPPORTED_LANGUAGES (sizeof(SUPPORTED_LANGUAGES)/sizeof(SUPPORTED_LANGUAGES[0]))

#define DEFAULT_LANGUAGE_INDEX (NUM_SUPPORTED_LANGUAGES - 1)
#define DEFAULT_BED_INDEX 0


// Prototype (needed by onPresentationLangUpdated callback function)
static
void
onPresentationNameLangUpdated
    (uiCombobox *c
    ,void *data
    );


static
void
toggle_object
    (dlb_pmd_presentation *p
    ,dlb_pmd_bool enabled
    ,dlb_pmd_element_id eid
    );

static
dlb_pmd_success
add_audio_presentation
    (uiGrid *grid
    ,pmd_studio_audio_presentation *apres
    ,unsigned int gridYIndex
    ,pmd_studio_audio_presentations *presentations
    );

static
void
addMissingCheckBoxes
	(pmd_studio_audio_presentations *apres
	 );

static
dlb_pmd_element_id
get_selected_bed_eid
    (pmd_studio_audio_presentation *apre
    );


/* Callbacks */

static
void
onEnablePresentation
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    apres->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    pmd_studio_audio_output_update_presentation_names(apres->presentations->studio);
    pmd_studio_update_model(apres->presentations->studio);
}

static
void
onPresentationLangUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    pmd_studio *studio = apres->presentations->studio;

    int index = uiComboboxSelected(c);
    if (index < 0)
    {
    	memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    }
    else
    {
        memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[index], 4);
    }

    if(studio->settings->nlang_settings.behaviour == PMD_STUDIO_NLANG_BEHAVIOUR::FOLLOW_LANG)
    {
        uiComboboxSetSelected(apres->nlang, uiComboboxSelected(apres->plang));
        onPresentationNameLangUpdated(apres->nlang, apres);
    }
    else
    {
        pmd_studio_update_model(apres->presentations->studio);
    }
}

static
void
onPresentationBedUpdated
    (uiCombobox *c
    ,void *data
    )
{
    unsigned int num_beds;
    dlb_pmd_element_id *bed_eids;
    unsigned int *bed_labels;
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    unsigned int selected_bed = uiComboboxSelected(c);

    num_beds = pmd_studio_audio_beds_get_eids(&bed_eids, &bed_labels, apres->presentations->studio);

    // Make a copy of eids and labels as this will change
    dlb_pmd_element_id bed_eids2[num_beds];
    for(unsigned int i=0; i<num_beds; i++)
    {
        bed_eids2[i] = bed_eids[i];
    }

    dlb_pmd_element_id current_bed_id = get_selected_bed_eid(apres);

    if (current_bed_id > 0)
    {
        // Remove current bed
        toggle_object(&apres->presentation, 0, current_bed_id);
    }

    if (selected_bed < num_beds)
    {
        // Enable selected bed
        toggle_object(&apres->presentation, 1, bed_eids2[selected_bed]);
        pmd_studio_update_model(apres->presentations->studio);
    }
    else
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "selected bed that doesn't exist");
    }
}

static
void
onPresentationObjectToggled
    (uiCheckbox *c
    ,void *data
    )
{
    unsigned int i;
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    dlb_pmd_bool enabled = (dlb_pmd_bool)uiCheckboxChecked(c);
    dlb_pmd_presentation *p = &apres->presentation;
    dlb_pmd_element_id eid = 0;
    dlb_pmd_bool found = PMD_FALSE;
    dlb_pmd_element_id *object_eids;
    unsigned int num_objects;

    num_objects = pmd_studio_audio_objects_get_eids(&object_eids, apres->presentations->studio);
    for (i = 0; i < num_objects; i++)
    {
        if (apres->checkBoxes[i] == c)
        {
            eid = object_eids[i];
            found = PMD_TRUE;
        }
    }
    if (!found || (eid == 0))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to get object eid");  
        return;      
    }
    toggle_object(p, enabled, eid);
    //pmd_studio_audio_presentations_print_debug(apres->presentations->studio);
    pmd_studio_update_model(apres->presentations->studio);
    //pmd_studio_audio_presentations_print_debug(apres->presentations->studio);
}

static
void
onPresentationNameUpdated
    (uiEntry *e
    ,void *data
    )
{
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    apres->presentation.num_names = 1;
    snprintf(apres->presentation.names[0].text,
    sizeof(apres->presentation.names[0].text), "%s", uiEntryText(e));
    pmd_studio_update_model(apres->presentations->studio);
    pmd_studio_audio_output_update_presentation_names(apres->presentations->studio);
}


static
void
onPresentationNameLangUpdated
    (uiCombobox *c
    ,void *data
    )
{
    int index = uiComboboxSelected(c);
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    if (index < 0)
    {
    	memmove(apres->presentation.names[0].language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    }
    else
    {
        memmove(apres->presentation.names[0].language, SUPPORTED_LANGUAGES[index], 4);
    }
    
    pmd_studio_update_model(apres->presentations->studio);
}

static 
void 
onAddAudioPresentationButtonClicked
    (uiButton *button
    ,void *data)
{
    pmd_studio_audio_presentations *presentations = (pmd_studio_audio_presentations *)data;
    (void)button;

    if (presentations->presentation_count == MAX_AUDIO_PRESENTATIONS)
    {
        uiMsgBoxError(presentations->window, "error setting presentation", "max presentations reached");
    }
    else
    {
        memset(&presentations->presentations[presentations->presentation_count], '\0', sizeof(presentations->presentations[presentations->presentation_count]));
        add_audio_presentation(presentations->grid, &presentations->presentations[presentations->presentation_count], presentations->presentation_count+1, presentations);
        pmd_studio_audio_output_update_presentation_names(presentations->studio);        
        pmd_studio_update_model(presentations->studio);
    }
}


/* Private Functions */

static
void
pmd_studio_audio_presentation_init
    (pmd_studio_audio_presentation *apres,
     pmd_studio_audio_presentations *presentations
    )
{
    apres->presentations = presentations;
    apres->enabled = PMD_TRUE;
    apres->presentation.id = presentations->presentation_count + 1;
    apres->presentation.config = DLB_PMD_SPEAKER_CONFIG_2_0;
    memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    if(presentations->studio->settings->nlang_settings.behaviour == PMD_STUDIO_NLANG_BEHAVIOUR::USE_PRESET)
    {
        memmove(apres->presentation.names[0].language, presentations->studio->settings->nlang_settings.preset_nlang, 4);
    }
    else{
        memmove(apres->presentation.names[0].language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    }
    apres->presentation.num_elements = 0;
    apres->presentation.elements = apres->elements;
    apres->presentation.num_names = 1;
    snprintf(apres->presentation.names[0].text,
    sizeof(apres->presentation.names[0].text), "Presentation %d", presentations->presentation_count + 1);
}

static
void
toggle_object
    (dlb_pmd_presentation *p
    ,dlb_pmd_bool enabled
    ,dlb_pmd_element_id eid
    )
{
    unsigned int i;

    if (enabled)
    {
        for(i=0; i < p->num_elements; i++)
        {
            if(p->elements[i] == eid)
            {
                // Already enabled. Do nothing.
                return;
            }
        }

        /* add object */
        p->elements[p->num_elements] = eid;
        p->num_elements += 1;
    }
    else
    {
        /* remove object */
        for (i = 0; i < p->num_elements; ++i)
        {
            if (p->elements[i] == eid)
            {
                p->num_elements -= 1;
                if (i < p->num_elements)
                {
                    int num_to_move = p->num_elements - i;
                    memmove(&p->elements[i], &p->elements[i+1],
                            sizeof(p->elements[0]) * num_to_move);
                }
                break;
            }
        }
    }
}

static
dlb_pmd_success
add_audio_presentation
    (uiGrid *grid
    ,pmd_studio_audio_presentation *apres
    ,unsigned int gridYIndex
    ,pmd_studio_audio_presentations *presentations
    )
{
    char tmp[MAX_LABEL_LENGTH];
    unsigned int i;
    dlb_pmd_element_id *eid_list;
    unsigned int *bed_labels;

    pmd_studio *studio = presentations->studio;
    unsigned int gridXIndex = 0;

    apres->presentations = presentations;
    if (apres->presentation.id > 255 || apres->presentation.id == 0)
    {
        memset(apres->elements, '\0', sizeof(apres->elements));
        pmd_studio_audio_presentation_init(apres, presentations);    
    }

    snprintf(tmp, MAX_LABEL_LENGTH, "%d", gridYIndex);
    apres->label = uiNewLabel(tmp);

    apres->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(apres->enable, onEnablePresentation, apres);
    uiCheckboxSetChecked(apres->enable, apres->enabled);

    apres->plang = uiNewCombobox();
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        uiComboboxAppend(apres->plang, SUPPORTED_LANGUAGES[i]);
    }
    uiComboboxSetSelected(apres->plang, DEFAULT_LANGUAGE_INDEX);
    uiComboboxOnSelected(apres->plang, onPresentationLangUpdated, apres);

    /* add bed dropdown */
    apres->bed = uiNewCombobox();
    uiComboboxOnSelected(apres->bed, onPresentationBedUpdated, apres);

    // set presentation bed and config to first available bed values (if possible).
    presentations->num_beds = pmd_studio_audio_beds_get_eids(&eid_list, &bed_labels, presentations->studio);
    if(presentations->num_beds == 0)
    {
        uiControlDisable(uiControl(apres->bed));
    }
    else
    {
        for (i = 0; i < presentations->num_beds; ++i)
        {
            snprintf(tmp, MAX_LABEL_LENGTH, "%d", bed_labels[i]);
            uiComboboxAppend(apres->bed, tmp);
        }
        // By default, enable first available bed
        toggle_object(&apres->presentation, PMD_TRUE, eid_list[DEFAULT_BED_INDEX]);
        uiComboboxSetSelected(apres->bed, DEFAULT_BED_INDEX);
    }


    /* add name */
    apres->name = uiNewEntry();
    uiEntrySetText(apres->name, (const char*)apres->presentation.names[0].text);
    uiEntryOnChanged(apres->name, onPresentationNameUpdated, apres);

    apres->nlang = uiNewCombobox();
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        uiComboboxAppend(apres->nlang, SUPPORTED_LANGUAGES[i]);
    }
    switch(studio->settings->nlang_settings.behaviour)
    {
        case PMD_STUDIO_NLANG_BEHAVIOUR::FOLLOW_LANG:
            uiComboboxSetSelected(apres->nlang, uiComboboxSelected(apres->plang));
            uiControlDisable(uiControl(apres->nlang));
            break;
        case PMD_STUDIO_NLANG_BEHAVIOUR::USE_PRESET:
            uiComboboxSetSelected(apres->nlang, pmd_studio_language_index_from_lang(studio->settings->nlang_settings.preset_nlang));
            uiControlDisable(uiControl(apres->nlang));
            break;
        default:
            uiComboboxSetSelected(apres->nlang, DEFAULT_LANGUAGE_INDEX);
            break;
    }
    uiComboboxOnSelected(apres->nlang, onPresentationNameLangUpdated, apres);

    /* add object checkboxes */
    for (i = 0; i != presentations->num_checkboxes; ++i)
    {
        apres->checkBoxes[i] = uiNewCheckbox("");
        uiCheckboxOnToggled(apres->checkBoxes[i], onPresentationObjectToggled, apres);
        if (!(presentations->object_enable_mask & (1 << i)))
        {
            uiControlDisable((uiControl *)apres->checkBoxes[i]);
        }
    }

    uiGridAppend(grid, uiControl(apres->label),         gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(grid, uiControl(apres->enable),        gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(grid, uiControl(apres->plang),         gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(grid, uiControl(apres->bed),           gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(grid, uiControl(apres->name),          gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(grid, uiControl(apres->nlang),         gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
	for (i = 0; i != presentations->num_checkboxes; ++i)
    {
        uiGridAppend(grid, uiControl(apres->checkBoxes[i]), gridXIndex++, gridYIndex, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    }

    presentations->presentation_count++;
    return PMD_SUCCESS;
}



static
void
addMissingCheckBoxes
    (pmd_studio_audio_presentations *apres
     )
{
    unsigned int i, pIndex;
    uiCheckbox * cb;
    char object_label[16];

    for (pIndex = 0; pIndex < apres->presentation_count; pIndex++)
    {
        for (i = apres->num_checkboxes; i < apres->num_checkboxes; i++)
        {     
            if (pIndex == 0) /* add the object labels at the start */
            {
                sprintf(object_label, "%s%u", "o", i+1);
                add_grid_title(apres->grid, object_label, i+6, 0);
            }
            /*cb = apres->presentations[pIndex].checkBoxes[i];*/  /* TODO: Fix needed here, same as above, Why is cd overwritten? */
            cb = uiNewCheckbox("");
            apres->presentations[pIndex].checkBoxes[i] = cb;
            uiGridAppend(apres->grid, uiControl(apres->presentations[pIndex].checkBoxes[i] = cb), i+6, pIndex+1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
            uiCheckboxOnToggled(apres->presentations[pIndex].checkBoxes[i], onPresentationObjectToggled, &apres->presentations[pIndex]);
        }
    } 
}

static
dlb_pmd_element_id
get_selected_bed_eid
    (pmd_studio_audio_presentation *apre
    )
{
    dlb_pmd_element_id *bed_eids;
    unsigned int *bed_labels, i , j;
    dlb_pmd_bool found = PMD_FALSE;
    unsigned int num_beds;

    // Get all bed eids (including disabled)
    num_beds = pmd_studio_audio_beds_get_eids(&bed_eids, &bed_labels, apre->presentations->studio, false);

    for (i = 0 ; (i < apre->presentation.num_elements) && !found ; i++)
    {
        for (j = 0 ; j < num_beds ; j++)
        {
            if ( apre->presentation.elements[i] == bed_eids[j])
            {
                found = PMD_TRUE;
                break;
            }
        }
    }

    if (found)
    {
        return(bed_eids[j]);
    }
    else
    {
        return(0);
    }
}


/* Public Functions */

dlb_pmd_success
pmd_studio_audio_presentations_init
    (pmd_studio_audio_presentations **ap1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    )
{
    uiBox *vbox;
    unsigned int gridXIndex = 1; // column 0 is labels so we start at 1

    *ap1 = (pmd_studio_audio_presentations *) malloc(sizeof(pmd_studio_audio_presentations));
    if (!*ap1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Failed to allocate memory for presentations");
    	return PMD_FAIL;
    }

    memset(*ap1, 0, sizeof(pmd_studio_audio_presentations));

    (*ap1)->window = win;
    
    (*ap1)->studio = studio1;

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Presentations")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);


    /* 'add audio presentation' button functionality */
    (*ap1)->add_pres_button = uiNewButton("Add Audio Presentation");
    uiBoxAppend(vbox, uiControl((*ap1)->add_pres_button), 0);
    uiButtonOnClicked((*ap1)->add_pres_button, onAddAudioPresentationButtonClicked, *ap1);

    (*ap1)->grid = uiNewGrid();
    uiGridSetPadded((*ap1)->grid, 1);
    uiBoxAppend(vbox, uiControl((*ap1)->grid), 0);

    add_grid_title((*ap1)->grid, "En",       gridXIndex++, 0);
    add_grid_title((*ap1)->grid, "Lang",     gridXIndex++, 0);
    add_grid_title((*ap1)->grid, "bed",      gridXIndex++, 0);  
    add_grid_title((*ap1)->grid, "Name",     gridXIndex++, 0);    
    add_grid_title((*ap1)->grid, "NLang",    gridXIndex++, 0);

    (*ap1)->presentation_count = 0;
    (*ap1)->num_checkboxes = 0;
    (*ap1)->num_beds = 0;
    (*ap1)->object_enable_mask = 0;


    /* this appends the entire presentations box to the studio window */
    uiBoxAppend(box, uiControl(vbox), 0);
    /* add another (empty) box essentially to give the window some space at the bottom */
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}

void
pmd_studio_audio_presentations_refresh_ui
    (pmd_studio_audio_presentations *apres
    )
{
    pmd_studio_audio_presentation *p;
    unsigned int i, j, k, num_beds, num_objects;
    dlb_pmd_element_id *bed_eids, *object_eids;
    unsigned int *bed_labels;

    // Make sure we have the right number of checkboxes
    addMissingCheckBoxes(apres);

    p = apres->presentations;
    
    num_beds = pmd_studio_audio_beds_get_eids(&bed_eids, &bed_labels, apres->studio);
    num_objects = pmd_studio_audio_objects_get_eids(&object_eids, apres->studio);

    for (i = 0; i < apres->presentation_count; ++i, ++p)
    {

        uiCheckboxSetChecked(p->enable, p->enabled);
        uiComboboxSetSelected(p->plang, pmd_studio_language_index_from_lang(p->presentation.audio_language));
        uiComboboxSetSelected(p->nlang, pmd_studio_language_index_from_lang(p->presentation.names[0].language));
        
        uiEntrySetText(p->name, p->presentation.names[0].text);

        /* walk through the list of elements */
        for (j = 0; j != p->presentation.num_elements; ++j)
        {
            dlb_pmd_element_id eid = p->presentation.elements[j];
            // Determine if it is a bed
            for (k = 0 ; k < num_beds ; k++)
            {
                if (bed_eids[k] == eid)
                {
                    break;
                }
            }
            if (k < num_beds)
            {
                // found bed so set to be selected as there is only 1
                uiComboboxSetSelected(p->bed, k);                
            }
            else
            {
                // Should be an object
                for (k = 0 ; k < num_objects ; k++)
                {
                    if (object_eids[k] == eid)
                    {
                        break;
                    }
                }
                if (k < num_objects)
                {
                    uiCheckboxSetChecked(p->checkBoxes[k], 1);
                }
                else
                {
                    pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Element not found when refreshing presentation ui");
                }
            }
        }
    }
}


void
pmd_studio_audio_presentations_update_nlang
    (pmd_studio_audio_presentations *apres
    )
{
    pmd_studio *studio = apres->studio;
    // Incase of nlang settings change, update presentation nlang.
    for(unsigned int i=0; i < apres->presentation_count; i++){
        pmd_studio_audio_presentation *p = &apres->presentations[i];
        switch (studio->settings->nlang_settings.behaviour)
        {
        case PMD_STUDIO_NLANG_BEHAVIOUR::FOLLOW_LANG:
            memcpy(p->presentation.names[0].language, p->presentation.audio_language, 4);
            uiControlDisable(uiControl(p->nlang));
            break;
        case PMD_STUDIO_NLANG_BEHAVIOUR::USE_PRESET:
            memcpy(p->presentation.names[0].language, studio->settings->nlang_settings.preset_nlang, 4);
            uiControlDisable(uiControl(p->nlang));
            break;
        default:
            uiControlEnable(uiControl(p->nlang));
            break;
        }
    }

    pmd_studio_update_model(studio);
}

void
pmd_studio_audio_presentations_enable
    (pmd_studio_audio_presentations *apres
    )
{
    unsigned int i, j;

    for (i = 0; i < apres->presentation_count; i++)
    {
        uiControlEnable(uiControl(apres->presentations[i].enable));
        uiControlEnable(uiControl(apres->presentations[i].plang));
        uiControlEnable(uiControl(apres->presentations[i].name));
        uiControlEnable(uiControl(apres->presentations[i].bed));

        if(apres->studio->settings->nlang_settings.behaviour == PMD_STUDIO_NLANG_BEHAVIOUR::UNLOCKED){
            uiControlEnable(uiControl(apres->presentations[i].nlang));
        }

        for (j = 0 ; j < apres->num_checkboxes ; j++)
        {
            if (apres->object_enable_mask & (1 << j))
            {
                uiControlEnable(uiControl(apres->presentations[i].checkBoxes[j]));
            }
        }
     }
     uiControlEnable(uiControl(apres->add_pres_button));
}

void
pmd_studio_audio_presentations_disable
    (pmd_studio_audio_presentations *apres,
    bool live_mode                              // Currently unused.
    )
{
    unsigned int i, j;
    
    for (i = 0; i < apres->presentation_count; i++)
    {
        uiControlDisable(uiControl(apres->presentations[i].enable));
        uiControlDisable(uiControl(apres->presentations[i].nlang));
        uiControlDisable(uiControl(apres->presentations[i].plang));
        uiControlDisable(uiControl(apres->presentations[i].bed));
        uiControlDisable(uiControl(apres->presentations[i].name));
        for (j = 0 ; j < apres->num_checkboxes ; j++)
        {
            uiControlDisable(uiControl(apres->presentations[i].checkBoxes[j]));
        }
     }
     uiControlDisable(uiControl(apres->add_pres_button));
}



dlb_pmd_success
pmd_studio_audio_presentations_import
    (pmd_studio_audio_presentations *apres
    ,dlb_pmd_model *m
    )
{
    unsigned int i;
    pmd_studio_audio_presentation *p;
    dlb_pmd_presentation_iterator pi;

    if (dlb_pmd_presentation_iterator_init(&pi, m))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to add init presentation iterator");
        return PMD_FAIL;
    }

    /* Add missing presentations */
    if ( apres->presentation_count < dlb_pmd_num_presentations(m))
    {
    	for (i = apres->presentation_count ; i < dlb_pmd_num_presentations(m) ; i++)
    	{
    		add_audio_presentation(apres->grid, &apres->presentations[i], i+1, apres);
    	}
    }

    p = apres->presentations;
    for (i = 0 ; i < dlb_pmd_num_presentations(m) ; i++)
    {
        if (dlb_pmd_presentation_iterator_next(&pi, &p->presentation, MAX_PRES_ELEMENTS,
                                               p->elements))
        {
            break;
        }
        p->enabled = 1;
        ++p;
    }
    apres->presentation_count = i;

    return PMD_SUCCESS;
}

unsigned int
pmd_studio_audio_presentation_get_enabled
    (pmd_studio *studio,
    dlb_pmd_element_id (**ids)[MAX_AUDIO_PRESENTATIONS],
    char* (**names)[MAX_AUDIO_PRESENTATIONS]
    )
{
    pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
    unsigned int i,j;

    j = 0;
    // First refresh table
    for (i = 0 ; i < apres->presentation_count ; i++)
    {
        if (apres->presentations[i].enabled)
        {
            apres->presentation_ids[j] = apres->presentations[i].presentation.id;
            apres->presentation_names[j++] = &(apres->presentations[i].presentation.names[0].text[0]);
        }
    }
    // return pointers to tables
    *ids = &apres->presentation_ids;
    *names = &apres->presentation_names;
    return(j); // and enabled presentation count
}

void
pmd_studio_audio_presentations_reset
    (pmd_studio_audio_presentations *apres
    )
{
    unsigned int i,j;

    for (i = 0 ; i < apres->presentation_count ; i++)
    {
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].label));
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].enable));
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].nlang));
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].plang));
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].bed));
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].name));

        uiControlDestroy(uiControl(apres->presentations[i].label));
        uiControlDestroy(uiControl(apres->presentations[i].enable));
        uiControlDestroy(uiControl(apres->presentations[i].nlang));
        uiControlDestroy(uiControl(apres->presentations[i].plang));
        uiControlDestroy(uiControl(apres->presentations[i].bed));
        uiControlDestroy(uiControl(apres->presentations[i].name));

        for (j = 0 ; j < apres->num_checkboxes ; j++)
        {
            uiGridDelete(apres->grid, uiControl(apres->presentations[i].checkBoxes[j]));
            uiControlDestroy(uiControl(apres->presentations[i].checkBoxes[j]));
        }
    }
     apres->presentation_count = 0;
     apres->num_checkboxes = 0;
     apres->num_beds = 0;
}

void
pmd_studio_presentations_bed_enable
    (dlb_pmd_element_id eid,
     dlb_pmd_bool enable,
     pmd_studio *studio
    )
{
    pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
    dlb_pmd_element_id *eid_list;
    unsigned int *bed_labels;
    unsigned int num_eids;
    unsigned int i, j;
    unsigned int to_be_selected;
    dlb_pmd_bool still_selected = PMD_FALSE;
    char tmp[MAX_LABEL_LENGTH];
    dlb_pmd_element_id selected_bed_eid;
    // (void)enable;
    // (void) eid;

    num_eids = pmd_studio_audio_beds_get_eids(&eid_list, &bed_labels, studio);
    
    // Make a local copy of the eids and labels as this will change
    dlb_pmd_element_id eid_list2[num_eids];
    unsigned int bed_labels2[num_eids];
    for(i=0; i<num_eids; i++)
    {
       eid_list2[i] = eid_list[i];
       bed_labels2[i] = bed_labels[i];
    }

    for (i = 0 ; i < apres->presentation_count ; i++)
    {   
        selected_bed_eid = get_selected_bed_eid(&apres->presentations[i]);
        uiGridDelete(apres->grid, uiControl(apres->presentations[i].bed));
        uiControlDestroy(uiControl(apres->presentations[i].bed));
        
        apres->presentations[i].bed = uiNewCombobox();
        uiComboboxOnSelected(apres->presentations[i].bed, onPresentationBedUpdated, &apres->presentations[i]);
        uiGridAppend(apres->grid, uiControl(apres->presentations[i].bed), 3, i+1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
        still_selected = PMD_FALSE;

        if(num_eids == 0)
        {
            uiControlDisable(uiControl(apres->presentations[i].bed));
        }
        else
        {
            for (j = 0 ; j < num_eids ; j++)
            {
                snprintf(tmp, MAX_LABEL_LENGTH, "%d", bed_labels2[j]);
                uiComboboxAppend(apres->presentations[i].bed, tmp);
                if (selected_bed_eid == eid_list2[j])
                {
                    still_selected = PMD_TRUE;
                    to_be_selected = j;
                }
            }
        }
        
        /* if we've removed the bed that was selected in the combobox then signal that this is now unselected */
        if (!still_selected)
        {
            // Check that the reason we couldn't reselect wasn't because nothing was selected
            if (selected_bed_eid != 0)
            {
                /* remove deslected bed from presentation */
                toggle_object(&apres->presentations[i].presentation, 0, selected_bed_eid);
            }
        }
        else
        {
            uiComboboxSetSelected(apres->presentations[i].bed, to_be_selected);
        }
    }
}


void
pmd_studio_presentations_add_audio_object_to_presentations
	(pmd_studio *studio,
     dlb_pmd_element_id eid
	 )
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
	unsigned int cbIndex = 0;
	unsigned int pIndex;
    unsigned int gridXIndex, gridYIndex;
    char object_label[16];
    uiCheckbox *cb;

    (void)eid;

    apres->num_checkboxes++;
    sprintf(object_label, "%s%u", "o", apres->num_checkboxes);
    add_grid_title(apres->grid, object_label, apres->num_checkboxes+5, 0);
    /* initialize the object's checkbox for each presentation */
    for (pIndex = 0; pIndex < apres->presentation_count; pIndex++)
    {
        cb = uiNewCheckbox("");                                         
        apres->presentations[pIndex].checkBoxes[apres->num_checkboxes-1] = cb;
        gridXIndex = apres->num_checkboxes+5;
        gridYIndex = cbIndex+1;
        uiGridAppend(apres->grid, uiControl(apres->presentations[pIndex].checkBoxes[apres->num_checkboxes-1] = cb), gridXIndex, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        uiCheckboxOnToggled(apres->presentations[pIndex].checkBoxes[apres->num_checkboxes-1], onPresentationObjectToggled, &apres->presentations[pIndex]);
        // Nex checkboxes for objects are always disabled by default until object enable received
        uiControlDisable(uiControl(apres->presentations[pIndex].checkBoxes[apres->num_checkboxes-1]));
        cbIndex +=1;
    }
}

void
pmd_studio_presentations_object_enable
    (dlb_pmd_element_id eid,
     dlb_pmd_bool enable,
     pmd_studio *studio
    )
{
    dlb_pmd_element_id *eid_list;
    pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
    unsigned int num_objects, object_index, i;


    num_objects = pmd_studio_audio_objects_get_eids(&eid_list, studio);

    for (object_index = 0 ; object_index < num_objects ; object_index++)
    {
        if (eid_list[object_index] == eid)
        {
            break;
        }
    }

    /* check to see if we successfully located object */
    if ((object_index < num_objects) && (eid_list[object_index] == eid))
    {
        /* enable/disable checkboxes */
        for (i = 0 ; i < apres->presentation_count ; i++)
        {
            if (enable)
            {
                uiControlEnable(uiControl(apres->presentations[i].checkBoxes[object_index]));
            }
            else
            {
                uiControlDisable(uiControl(apres->presentations[i].checkBoxes[object_index]));
            }
            /* For this presentation if object has checkbox checked then remove/add from presentation */
            if (uiCheckboxChecked(apres->presentations[i].checkBoxes[object_index]))
            {
                toggle_object(&apres->presentations[i].presentation, enable, eid);
            }
        }
        // Store a marker for adding presentations in future
        if (enable)
        {
            apres->object_enable_mask |= 1 << object_index;
        }
        else
        {
            apres->object_enable_mask &= ~(1 << object_index);            
        }
    }
    else
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Couldn't find object in object list");
    }
}

void
pmd_studio_audio_presentations_update_model
	(pmd_studio_audio_presentations *apres,
	 dlb_pmd_model *m
	 )
{
	pmd_studio_audio_presentation *apre = apres->presentations;
	unsigned int i, j;
    dlb_pmd_element_id eid;

    for (i = 0; i < apres->presentation_count ; i++, apre++)
    {
        // Set presentation configuration so it matches bed configuration
        for (j = 0 ; j < apre->presentation.num_elements ; j++)
        {
            eid = apre->presentation.elements[j];
            // Determine if bed or object
            if (eid == get_selected_bed_eid(apre))
            {
                // Set presentation config to equal bed config
                // This needs to be be here until a way to convey the bed config is found in S-ADM
                // and a separate control for bed config can be established
                apre->presentation.config = pmd_studio_audio_beds_get_bed_config(apres->studio, eid);
            }
        }

        if (apre->enabled)
        {
            if (dlb_pmd_set_presentation(m, &apre->presentation))
            {
                uiMsgBoxError(apres->window, "error setting presentation", dlb_pmd_error(m));
            }
        }
    }
}

void 
pmd_studio_audio_presentations_handle_element_eid_update 
    (pmd_studio *studio
    ,dlb_pmd_element_id old_eid
    ,dlb_pmd_element_id new_eid
    )
{
    unsigned int i,j;

    for( i = 0; i < studio->audio_presentations->presentation_count; i++)
    {
        dlb_pmd_presentation *pres = &studio->audio_presentations->presentations[i].presentation;
        for( j = 0; j < pres->num_elements; j++)
        {
            if(pres->elements[j] == old_eid)
            {
                pres->elements[j] = new_eid;
            }
        }
    }
}

dlb_pmd_success pmd_studio_audio_presentations_get_mix_matrix(
	unsigned int id,
	dlb_pmd_speaker_config config,
	pmd_studio_mix_matrix mix_matrix,
	pmd_studio *studio)
{
	// First get mix matrices for audio elements in presentation configuration
	dlb_pmd_element_id eid;
	unsigned int i;
	pmd_studio_audio_presentation *apre;
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);

	// Search for presentation
	for (i = 0 ; (i < apres->presentation_count) ; i++)
	{
		if (apres->presentations[i].presentation.id == id)
		{
			break;
		}
	}

	// Check that we found presentations and it is enabled
	if ((i >=  apres->presentation_count) || !apres->presentations[i].enabled)
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Referenced presentation doesn't exist or is not enabled");
		return(PMD_FAIL);
	}
	apre = &apres->presentations[i];

	// Start with a zeroed out matrix
	pmd_studio_mix_matrix_reset(mix_matrix);

    // Mix in beds and objects
    // Use 5.1.4 for internal plumbing instead of a presentation configuration

	for (i = 0 ; i < apre->presentation.num_elements ; i++)
	{
		eid = apre->presentation.elements[i];
		// Determine if bed or object
		if (eid == get_selected_bed_eid(apre))
		{
			if (pmd_studio_audio_beds_get_mix_matrix(eid, DLB_PMD_SPEAKER_CONFIG_5_1_4, mix_matrix, apres->studio) != PMD_SUCCESS)
			{
				return(PMD_FAIL);
			}
		}
		else
		{
			// Others are always objects
			if (pmd_studio_audio_objects_get_mix_matrix(eid, DLB_PMD_SPEAKER_CONFIG_5_1_4, mix_matrix, apres->studio) != PMD_SUCCESS)
			{
				return(PMD_FAIL);
			}
		}
	}

    switch(config)
    {
    case DLB_PMD_SPEAKER_CONFIG_2_0:
        // Do fold down of mix matrix from 5.1.4 to 2/0 here
        for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
        {
            mix_matrix[i][0] += (mix_matrix[i][2] + mix_matrix[i][4] + mix_matrix[i][6] + (mix_matrix[i][8] * PMD_STUDIO_M3DB)) * PMD_STUDIO_M3DB;
            mix_matrix[i][1] += (mix_matrix[i][2] + mix_matrix[i][5] + mix_matrix[i][7] + (mix_matrix[i][9] * PMD_STUDIO_M3DB)) * PMD_STUDIO_M3DB;
            // Zero out surround channels for safety and ease of debugging
            mix_matrix[i][2] = mix_matrix[i][3] = mix_matrix[i][4] = mix_matrix[i][5] = 0;              
            mix_matrix[i][6] = mix_matrix[i][7] = mix_matrix[i][8] = mix_matrix[i][9] = 0;              
        }
        break;
    case DLB_PMD_SPEAKER_CONFIG_5_1:
        // Do fold down of mix matrix from 5.1.4 to 5.1 here
        /* L=(L+(Ltf*0.707)), R=(R+(Rtf*0.707) - Reference PMD Application guide
           Ls=(Ls+(Ltr*0.707)), Rs=(Rs+(Rtr*0.707)) */

        for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
        {
            mix_matrix[i][0] += mix_matrix[i][6] * PMD_STUDIO_M3DB;
            mix_matrix[i][1] += mix_matrix[i][7] * PMD_STUDIO_M3DB;
            mix_matrix[i][4] += mix_matrix[i][8] * PMD_STUDIO_M3DB;
            mix_matrix[i][5] += mix_matrix[i][9] * PMD_STUDIO_M3DB;               
            // Zero out height channels for safety and ease of debugging
            mix_matrix[i][6] = mix_matrix[i][7] = mix_matrix[i][8] = mix_matrix[i][9] = 0;              
        }
        break;
    case DLB_PMD_SPEAKER_CONFIG_5_1_4:
        // Do nothing as we are natively using a 5.1.4 pipe
        break;
    default:
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Unsupported output configuration");
		return(PMD_FAIL);
	}
	return(PMD_SUCCESS);
}




void
pmd_studio_audio_presentations_print_debug(
    pmd_studio *studio)
{
    unsigned int i, j;
    pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
    pmd_studio_config *config_info; 

    printf("---****Presentations****----\n");
    printf("Number of Presentations: %u\n", apres->presentation_count);
    printf("Number of checkboxes %u\n", apres->num_checkboxes);
    printf("Number of Beds %u\n", apres->num_beds);

    for (i = 0 ; i < apres->presentation_count ; i++)
    {
        printf("Presentation #%u:\n", i);
        if (apres->presentations[i].enabled)
        {
            printf("\tPresentation is enabled\n" );
        }
        else
        {
            printf("\tPresentation is disabled\n" );
        }
        printf("\tID: %u\n", apres->presentations[i].presentation.id);
        printf("\tName: %s\n", apres->presentations[i].presentation.names[0].text);

        config_info = get_pmd_studio_config_info(studio, apres->presentations[i].presentation.config);
        printf("\tConfiguration: ");
        printf("\t%s\n", config_info->config_string);

        printf("\tElement Ids: ");
        for (j = 0 ; j < apres->presentations[i].presentation.num_elements ; j++)
        {
            printf("%u ", apres->presentations[i].presentation.elements[j]);
        }
        printf("\n");
    }
    printf("\n");
}


void
pmd_studio_audio_presentations_finish
    (pmd_studio_audio_presentations *apres
    )
{
    free(apres);
}


unsigned int
pmd_studio_get_supported_languages
    (const char ***lang_ptr
    )
{
    *lang_ptr = SUPPORTED_LANGUAGES;
    return NUM_SUPPORTED_LANGUAGES;
}

unsigned int
pmd_studio_get_default_language_index
    ()
{
    return DEFAULT_LANGUAGE_INDEX;
}

int
pmd_studio_language_index_from_lang
    (const char *lang
    )
{
    int i = 0;
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        if (!strcmp(lang, SUPPORTED_LANGUAGES[i]))
        {
            return i;
        }
    }
    return 0;
}
