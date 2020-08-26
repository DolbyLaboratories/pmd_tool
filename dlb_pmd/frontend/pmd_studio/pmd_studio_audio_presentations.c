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

#include <string.h>
#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"
#include "pmd_studio_audio_presentations.h"


/* Definitions */

#define MAX_PRES_ELEMENTS (MAX_AUDIO_BEDS + MAX_AUDIO_OBJECTS)

typedef struct
{
    pmd_studio_audio_presentations *presentations;
    dlb_pmd_bool enabled;
    dlb_pmd_presentation presentation;
    dlb_pmd_element_id elements[MAX_PRES_ELEMENTS];

    uiCheckbox *enable;
    uiCombobox *cfg;
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
    unsigned int ids[MAX_AUDIO_PRESENTATIONS]; // Convenience structure for outputs to use
    pmd_studio_audio_presentation presentations[MAX_AUDIO_PRESENTATIONS];
    unsigned int num_checkBoxes;
    unsigned int presentation_count;

    uiGrid *grid;
};

/**
 * @brief list of supported languages in the UI
 *
 * for a complete list of languages supported by the library, see
 * pmd_language.c source code file
 */
static const char *SUPPORTED_LANGUAGES[] =
{
    "bg",  "ces", "dan", "dut", "eng", "est", "fin", "fra", "ger", "gre", "hun", "ice", "ita",
    "lav", "lit", "nor", "pol", "rum", "rus", "slk", "slv", "spa", "sr",  "swe", "ukr", "und",
};

#define NUM_SUPPORTED_LANGUAGES (sizeof(SUPPORTED_LANGUAGES)/sizeof(SUPPORTED_LANGUAGES[0]))

#define DEFAULT_LANGUAGE_INDEX (NUM_SUPPORTED_LANGUAGES - 1)
#define DEFAULT_BED_INDEX 0

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

void
addMissingCheckBoxes
	(pmd_studio_audio_presentations *apres
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
    pmd_studio_update_model(apres->presentations->studio);
    pmd_studio_audio_output_update_presentation_names(apres->presentations->studio);
    // Disabling a presentation may affect output
    if (!apres->enabled)
    {
    	pmd_studio_audio_output_presentation_disabled(apres->presentations->studio, apres->presentation.id);
    }
}

static
void
onPresentationConfigUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    apres->presentation.config = (dlb_pmd_speaker_config)uiComboboxSelected(c);    
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
    int index = uiComboboxSelected(c);
    if (index < 0)
    {
    	memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    }
    else
    {
        memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[index], 4);
    }
    pmd_studio_update_model(apres->presentations->studio);
}

static
void
onPresentationBedUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    /* in this UI, 1st presentation element is always the bed */
    apres->presentation.elements[0] = (dlb_pmd_element_id)(1 + uiComboboxSelected(c));
    pmd_studio_update_model(apres->presentations->studio);
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
    
    dlb_pmd_element_id eid = 1 + MAX_AUDIO_BEDS;    
    for (i = 0; i < MAX_AUDIO_OBJECTS; i++)
    {
        if (apres->checkBoxes[i] == c)
        {
            eid = (dlb_pmd_element_id)(i+1 + MAX_AUDIO_BEDS);    
        }
    }
    toggle_object(p, enabled, eid);
    pmd_studio_update_model(apres->presentations->studio);
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
        pmd_studio_update_model(presentations->studio);
    }
}

/* Private Functions */

static
void
pmd_studio_audio_presentation_init
    (pmd_studio_audio_presentation *apres
    )
{
    apres->enabled = 0;
    apres->presentation.config = DLB_PMD_SPEAKER_CONFIG_2_0;
    memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    memmove(apres->presentation.names[0].language, SUPPORTED_LANGUAGES[DEFAULT_LANGUAGE_INDEX], 4);
    apres->presentation.num_elements = 1;
    apres->presentation.elements = apres->elements;
    apres->presentation.elements[0] = 1;
    apres->presentation.num_names = 1;
    snprintf(apres->presentation.names[0].text,
    sizeof(apres->presentation.names[0].text), "Presentation %d", apres->presentations->presentation_count + 1);
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
    char tmp[32];
    unsigned int i;
    unsigned int objects_count;

    /* the previous objects in the grid are added up to index 6, so start appending the checkboxes at 7 */
    int gridXIndex = 7;

    objects_count = pmd_studio_object_get_count(presentations->studio);

    apres->presentations = presentations;
    if (apres->presentation.id > 255 || apres->presentation.id == 0)
    {
        memset(apres->elements, '\0', sizeof(apres->elements));
        pmd_studio_audio_presentation_init(apres);    
    }
    apres->presentation.id = (dlb_pmd_presentation_id)gridYIndex;


    snprintf(tmp, sizeof(tmp), "%d", gridYIndex);
    uiGridAppend(grid, uiControl(uiNewLabel(tmp)), 0, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    apres->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(apres->enable, onEnablePresentation, apres);
    uiGridAppend(grid, uiControl(apres->enable), 1, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    apres->cfg = uiNewCombobox();
    uiComboboxAppend(apres->cfg, "2.0");
    uiComboboxAppend(apres->cfg, "3.0");
    uiComboboxAppend(apres->cfg, "5.1");
    uiComboboxAppend(apres->cfg, "5.1.2");
    uiComboboxAppend(apres->cfg, "5.1.4");
    uiComboboxAppend(apres->cfg, "7.1.4");    
    uiComboboxSetSelected(apres->cfg, (int)apres->presentation.config); 
    uiComboboxOnSelected(apres->cfg, onPresentationConfigUpdated, apres);
    uiGridAppend(grid, uiControl(apres->cfg), 2, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    apres->plang = uiNewCombobox();
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        uiComboboxAppend(apres->plang, SUPPORTED_LANGUAGES[i]);
    }
    uiComboboxSetSelected(apres->plang, DEFAULT_LANGUAGE_INDEX);
    uiComboboxOnSelected(apres->plang, onPresentationLangUpdated, apres);
    uiGridAppend(grid, uiControl(apres->plang), 3, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    /* add bed */
    apres->bed = uiNewCombobox();
    for (i = 0; i != MAX_AUDIO_BEDS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i+1);
        uiComboboxAppend(apres->bed, tmp);
    }
    uiComboboxSetSelected(apres->bed, DEFAULT_BED_INDEX);
    uiComboboxOnSelected(apres->bed, onPresentationBedUpdated, apres);
    uiGridAppend(grid, uiControl(apres->bed), 4, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    /* add object checkboxes */
    for (i = 0; i != objects_count; ++i)
    {
        apres->checkBoxes[i] = uiNewCheckbox("");
        uiCheckboxOnToggled(apres->checkBoxes[i], onPresentationObjectToggled, apres);
        uiGridAppend(grid, uiControl(apres->checkBoxes[i]), gridXIndex, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
        gridXIndex+=1;

    }

    /* add name */
    apres->name = uiNewEntry();
    uiGridAppend(grid, uiControl(apres->name), 5, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiEntrySetText(apres->name, (const char*)apres->presentation.names[0].text);
    uiEntryOnChanged(apres->name, onPresentationNameUpdated, apres);

    apres->nlang = uiNewCombobox();
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        uiComboboxAppend(apres->nlang, SUPPORTED_LANGUAGES[i]);
    }
    uiComboboxSetSelected(apres->nlang, DEFAULT_LANGUAGE_INDEX);
    uiComboboxOnSelected(apres->nlang, onPresentationNameLangUpdated, apres);
    uiGridAppend(grid, uiControl(apres->nlang), 6, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    presentations->presentation_count++;
    return PMD_SUCCESS;
}

static
int
ui_language
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



/* Public Functions */

dlb_pmd_success
pmd_studio_audio_presentations_init
    (pmd_studio_audio_presentations **ap1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    )
{
    unsigned int i;
    uiBox *vbox;
    uiButton *button;
    char object_label[16];
    unsigned int gridTitleIndex;

    *ap1 = malloc(sizeof(pmd_studio_audio_presentations));
    if (!*ap1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Failed to allocate memory for presentations");
    	return PMD_FAIL;
    }

    (*ap1)->window = win;
    
    (*ap1)->studio = studio1;

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Presentations")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);


    /* 'add audio presentation' button functionality */
    button = uiNewButton("Add Audio Presentation");
    uiBoxAppend(vbox, uiControl(button), 1);
    uiButtonOnClicked(button, onAddAudioPresentationButtonClicked, *ap1);

    (*ap1)->grid = uiNewGrid();
    uiGridSetPadded((*ap1)->grid, 1);
    uiBoxAppend(vbox, uiControl((*ap1)->grid), 0);

    add_grid_title((*ap1)->grid, "En",       1, 0);
    add_grid_title((*ap1)->grid, "Cfg",      2, 0);
    add_grid_title((*ap1)->grid, "Lang",     3, 0);
    add_grid_title((*ap1)->grid, "bed",      4, 0);  
    add_grid_title((*ap1)->grid, "Name",     5, 0);    
    add_grid_title((*ap1)->grid, "NLang",    6, 0);

    gridTitleIndex = 7;
    for (i = 0; i < INIT_AUDIO_OBJECTS; i++)
    {
        sprintf(object_label, "%s%u", "o", i+1);
        add_grid_title((*ap1)->grid, object_label, gridTitleIndex, 0);
        gridTitleIndex+=1;
    } 
    
    (*ap1)->presentation_count = 0;
    (*ap1)->num_checkBoxes = 0;

    for (i = 0; i != INIT_AUDIO_PRESENTATIONS; ++i)
    {
        if (add_audio_presentation((*ap1)->grid, &((*ap1)->presentations[i]), i+1, *ap1))
        {
            pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to add audio presentation");
            return PMD_FAIL;
        }
    }

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
    unsigned int i;
    unsigned int j; /* unsigned bc the num it's compared to is unsigned */

    // Make sure we have the right number of checkboxes
    addMissingCheckBoxes(apres);

    p = apres->presentations;
    
    for (i = 0; i < apres->presentation_count; ++i, ++p)
    {
        uiCheckboxSetChecked(p->enable, p->enabled);
        uiComboboxSetSelected(p->cfg, (int)p->presentation.config);
        uiComboboxSetSelected(p->plang, ui_language(p->presentation.audio_language));
        uiComboboxSetSelected(p->nlang, ui_language(p->presentation.names[0].language));
        uiEntrySetText(p->name, p->presentation.names[0].text);

        /* walk through the list of elements */
        for (j = 0; j != p->presentation.num_elements; ++j)
        {
            dlb_pmd_element_id eid = p->presentation.elements[j];
            if (eid > MAX_AUDIO_BEDS)
            {
                uiCheckboxSetChecked(p->checkBoxes[eid - MAX_AUDIO_BEDS - 1], 1);
            }

            else if (eid <= MAX_AUDIO_BEDS)
            {
                uiComboboxSetSelected(p->bed, (int)eid - 1);
            }
        }
    }
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
pmd_studio_audio_presentations_get_count
    (pmd_studio *studio
    )
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
	return(apres->presentation_count);
}


const char
*pmd_studio_audio_presentation_get_name
    (pmd_studio *studio,
     unsigned int index
    )
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
	return(apres->presentations[index].presentation.names[0].text);
}


const char
*pmd_studio_audio_presentation_get_name_by_id
    (pmd_studio *studio,
     unsigned int id
    )
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
	unsigned int i;

	for (i = 0 ; (i < apres->presentation_count) ; i++)
	{
		if (id == apres->presentations[i].presentation.id)
		{
			break;
		}
	}
	if (id == apres->presentations[i].presentation.id)
	{
		return(apres->presentations[i].presentation.names[0].text);
	}
	else
	{
		return("");
	}
}


dlb_pmd_success
pmd_studio_audio_presentation_enabled_by_id
    (pmd_studio *studio,    // input
     unsigned int id,       // input
     unsigned int *enabled  // output
    )
{
	unsigned int i;
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);

	for (i = 0 ; (i < apres->presentation_count) ; i++)
	{
		if (id == apres->presentations[i].presentation.id)
		{
			break;
		}
	}

	if (id == apres->presentations[i].presentation.id)
	{
		*enabled = apres->presentations[i].enabled;
		return(PMD_SUCCESS);
	}
	else
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to find presentation referenced by ID");
		return(PMD_FAIL);
	}

}


unsigned int
pmd_studio_audio_presentation_get_id
    (pmd_studio *studio,
     unsigned int index
    )
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
    if (index >= apres->presentation_count)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Presentation index out of range");
    }
	return(apres->presentations[index].presentation.id);
}

unsigned int get_studio_presentation_ids
	(pmd_studio *studio,
	unsigned int **ids
	)
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
	unsigned int i;

	// First refresh table
	for (i = 0 ; (i < apres->presentation_count) ; i++)
	{
		apres->ids[i] = apres->presentations[i].presentation.id;
	}
	*ids = apres->ids; // return pointer to table
	return(apres->presentation_count); // and presentation count
}


void
pmd_studio_audio_presentations_reset
    (pmd_studio_audio_presentations *apres
    )
{
    int i, j, pres_count, obj_count;    
    pmd_studio_audio_presentation *p;

    pres_count = apres->presentation_count;
    obj_count = pmd_studio_object_get_count(apres->studio);
    p = apres->presentations;
    for (i = 0; i != pres_count; ++i, ++p)
    {
        pmd_studio_audio_presentation_init(p);
        for (j = 0; j < obj_count; j++)
        {
            uiCheckboxSetChecked(p->checkBoxes[j], 0);
        }
     }
}


void
addAudioObjectToPresentations
	(pmd_studio *studio,
	 unsigned int object_count
	 )
{
	pmd_studio_audio_presentations *apres = pmd_studio_get_presentations(studio);
	unsigned int cbIndex = 0;
	unsigned int pIndex;
    unsigned int gridXIndex, gridYIndex;
    char object_label[16];

    sprintf(object_label, "%s%u", "o", object_count);
    add_grid_title(apres->grid, object_label, object_count+6, 0);
    /* initialize the object's checkbox for each presentation */
    for (pIndex = 0; pIndex < apres->presentation_count; pIndex++)
    {
        uiCheckbox * cb = apres->presentations[pIndex].checkBoxes[object_count-1];  /* TODO: get here... */
        cb = uiNewCheckbox("");                                         /* TODO: ...and new here?  WTF? */
        apres->presentations[pIndex].checkBoxes[object_count-1] = cb;
        gridXIndex = object_count+6;
        gridYIndex = cbIndex+1;
        uiGridAppend(apres->grid, uiControl(apres->presentations[pIndex].checkBoxes[object_count-1] = cb), gridXIndex, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        uiCheckboxOnToggled(apres->presentations[pIndex].checkBoxes[object_count-1], onPresentationObjectToggled, &apres->presentations[pIndex]);
        cbIndex +=1;
    }
    apres->num_checkBoxes++;
}

void
addMissingCheckBoxes
	(pmd_studio_audio_presentations *apres
	 )
{
	unsigned int i, pIndex;
	uiCheckbox * cb;
	char object_label[16];
	unsigned int object_count = pmd_studio_object_get_count(apres->studio);

    for (pIndex = 0; pIndex < apres->presentation_count; pIndex++)
    {
        for (i = apres->num_checkBoxes; i < object_count; i++)
        {     
            if (pIndex == 0) /* add the object labels at the start */
            {
                sprintf(object_label, "%s%u", "o", i+1);
                add_grid_title(apres->grid, object_label, i+7, 0);
            }
            cb = apres->presentations[pIndex].checkBoxes[i];  /* TODO: Fix needed here, same as above, Why is cd overwritten? */
            cb = uiNewCheckbox("");
            apres->presentations[pIndex].checkBoxes[i] = cb;
            uiGridAppend(apres->grid, uiControl(apres->presentations[pIndex].checkBoxes[i] = cb), i+7, pIndex+1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
            uiCheckboxOnToggled(apres->presentations[pIndex].checkBoxes[i], onPresentationObjectToggled, &apres->presentations[pIndex]);
        }
    } 
}

void
pmd_studio_audio_presentations_update_model
	(pmd_studio_audio_presentations *apres,
	 dlb_pmd_model *m
	 )
{
	pmd_studio_audio_presentation *p = apres->presentations;
	unsigned int i;

    for (i = 0; i < apres->presentation_count ; i++, p++)
    {
        if (p->enabled)
        {
            if (dlb_pmd_set_presentation(m, &p->presentation))
            {
                uiMsgBoxError(apres->window, "error setting presentation", dlb_pmd_error(m));
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

	for (i = 0 ; i < apre->presentation.num_elements ; i++)
	{
		eid = apre->presentation.elements[i];
		// First element is always the bed
		if (i == 0)
		{
			if (pmd_studio_audio_beds_get_mix_matrix(eid, apre->presentation.config, mix_matrix, apres->studio) != PMD_SUCCESS)
			{
				return(PMD_FAIL);
			}
		}
		else
		{
			// Others are always objects
			if (pmd_studio_audio_objects_get_mix_matrix(eid, apre->presentation.config, mix_matrix, apres->studio) != PMD_SUCCESS)
			{
				return(PMD_FAIL);
			}
		}
	}

	// Then convert presentation configuration to requested configuration
	if (config != apre->presentation.config)
	{
		// Only support 5.1 downmix at the moment
		if ((config == DLB_PMD_SPEAKER_CONFIG_2_0) && (apre->presentation.config == DLB_PMD_SPEAKER_CONFIG_5_1))
		{
			// Do fold down of mix matrix from 5.1 to 2/0 here
			for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
			{
				mix_matrix[i][0] += (mix_matrix[i][2] + mix_matrix[i][4]) * PMD_STUDIO_M3DB;
				mix_matrix[i][1] +=  (mix_matrix[i][2] + mix_matrix[i][5]) * PMD_STUDIO_M3DB;
				// Zero out surround channels for safety and ease of debugging
				mix_matrix[i][2] = mix_matrix[i][3] = mix_matrix[i][4] = mix_matrix[i][5] = 0;				
			}
		}
        else if ((config == DLB_PMD_SPEAKER_CONFIG_2_0) && (apre->presentation.config == DLB_PMD_SPEAKER_CONFIG_5_1_4))
        {
            // Do fold down of mix matrix from 5.1.4 to 2/0 here
            for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
            {
                mix_matrix[i][0] += (mix_matrix[i][2] + mix_matrix[i][4] + mix_matrix[i][6] + (mix_matrix[i][8] * PMD_STUDIO_M3DB)) * PMD_STUDIO_M3DB;
                mix_matrix[i][1] += (mix_matrix[i][2] + mix_matrix[i][5] + mix_matrix[i][7] + (mix_matrix[i][9] * PMD_STUDIO_M3DB)) * PMD_STUDIO_M3DB;
                // Zero out surround channels for safety and ease of debugging
                mix_matrix[i][2] = mix_matrix[i][3] = mix_matrix[i][4] = mix_matrix[i][5] = 0;              
                mix_matrix[i][6] = mix_matrix[i][7] = mix_matrix[i][8] = mix_matrix[i][9] = 0;              
            }
        }
        else if ((config == DLB_PMD_SPEAKER_CONFIG_5_1) && (apre->presentation.config == DLB_PMD_SPEAKER_CONFIG_5_1_4))
        {
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
        }
		else
		{
            pmd_studio_warning("Unsupported output configuration")
			return(PMD_FAIL);
		}
	}
	return(PMD_SUCCESS);
}

void
pmd_studio_audio_presentations_finish
    (pmd_studio_audio_presentations *apres
    )
{
    free(apres);
}


