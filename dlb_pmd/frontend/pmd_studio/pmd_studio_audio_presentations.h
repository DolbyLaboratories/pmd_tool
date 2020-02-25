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

#ifndef PMD_STUDIO_AUDIO_PRESENTATIONS_H_
#define PMD_STUDIO_AUDIO_PRESENTATIONS_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"

#define MAX_PRES_ELEMENTS (MAX_AUDIO_BEDS + MAX_AUDIO_OBJECTS)

typedef struct
{
    pmd_studio *studio;
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
    int initialized;
} pmd_studio_audio_presentation;


typedef struct
{
    uiWindow *window;
    pmd_studio_audio_presentation presentations[MAX_AUDIO_PRESENTATIONS];

    /* add in a count field to the struct for how many of the allocated presentations are intialized */
    int presentation_count;

    uiGrid *grid;
} pmd_studio_audio_presentations;


/** 
 * added this function so you can access the uiGrid that the presentations
 * are all on from pmd_studio_audio_objects.h 
**/
static 
uiGrid * 
getAudioPresentationsGrid
    (pmd_studio_audio_presentations *ap
    )
{
    return ap->grid;
}


pmd_studio_audio_presentations *ap;

static
void
pmd_studio_audio_presentation_init
    (pmd_studio_audio_presentation *apres
    )
{
    apres->enabled = 0;
    apres->presentation.config = DLB_PMD_SPEAKER_CONFIG_2_0;
    apres->presentation.audio_language[0] = 'u';
    apres->presentation.audio_language[1] = 'n';
    apres->presentation.audio_language[2] = 'd';
    apres->presentation.audio_language[3] = '\0';
    apres->presentation.num_elements = 1;
    apres->presentation.elements = apres->elements;
    apres->presentation.num_names = 0;
    memset(apres->presentation.names, '\0', sizeof(apres->presentation.names));
}


static
void
onEnablePresentation
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_presentation *apres = (pmd_studio_audio_presentation *)data;
    apres->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    pmd_studio_update_model(apres->studio);
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
    pmd_studio_update_model(apres->studio);
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
        apres->presentation.audio_language[0] = 'u';
        apres->presentation.audio_language[1] = 'n';
        apres->presentation.audio_language[2] = 'd';
        apres->presentation.audio_language[3] = '\0';
    }
    else
    {
        memmove(apres->presentation.audio_language, SUPPORTED_LANGUAGES[index], 4);
    }
    pmd_studio_update_model(apres->studio);
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
    pmd_studio_update_model(apres->studio);
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
    pmd_studio_update_model(apres->studio);
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
    pmd_studio_update_model(apres->studio);
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
        apres->presentation.names[0].language[0] = 'u';
        apres->presentation.names[0].language[1] = 'n';
        apres->presentation.names[0].language[2] = 'd';
        apres->presentation.names[0].language[3] = '\0';
    }
    else
    {
        memmove(apres->presentation.names[0].language, SUPPORTED_LANGUAGES[index], 4);
    }
    
    pmd_studio_update_model(apres->studio);
}


static inline
dlb_pmd_success
add_audio_presentation
    (uiGrid *grid
    ,pmd_studio_audio_presentation *apres
    ,unsigned int gridYIndex
    ,pmd_studio *s
    )
{
    char tmp[32];
    unsigned int i;
    unsigned int pIndex;
    unsigned int objects_count;

    /* the previous objects in the grid are added up to index 6, so start appending the checkboxes at 7 */
    int gridXIndex = 7;

    objects_count = getCurrentObjectCount(s);
    pIndex = getCurrentPresentationCount(s)-1;

    apres->studio = s;
    if (apres->presentation.id > 255 || apres->presentation.id == 0)
    {
        memset(apres->elements, '\0', sizeof(apres->elements));
        pmd_studio_audio_presentation_init(apres);    
    }
    apres->presentation.id = (dlb_pmd_presentation_id)gridYIndex;


    snprintf(tmp, sizeof(tmp), "%d", gridYIndex);
    uiGridAppend(grid, uiControl(uiNewLabel(tmp)), 0, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    apres->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(apres->enable, onEnablePresentation, apres);
    uiGridAppend(grid, uiControl(apres->enable), 1, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    apres->cfg = uiNewCombobox();
    uiComboboxAppend(apres->cfg, "2.0");
    uiComboboxAppend(apres->cfg, "3.0");
    uiComboboxAppend(apres->cfg, "5.1");
    uiComboboxAppend(apres->cfg, "5.1.2");
    uiComboboxAppend(apres->cfg, "5.1.4");
    uiComboboxAppend(apres->cfg, "7.1.4");    
    uiComboboxSetSelected(apres->cfg, (int)apres->presentation.config); 
    uiComboboxOnSelected(apres->cfg, onPresentationConfigUpdated, apres);
    uiGridAppend(grid, uiControl(apres->cfg), 2, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    apres->plang = uiNewCombobox();
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        uiComboboxAppend(apres->plang, SUPPORTED_LANGUAGES[i]);
    }
    uiComboboxOnSelected(apres->plang, onPresentationLangUpdated, apres);
    uiGridAppend(grid, uiControl(apres->plang), 3, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    /* add bed */
    apres->bed = uiNewCombobox();
    for (i = 0; i != MAX_AUDIO_BEDS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i+1);
        uiComboboxAppend(apres->bed, tmp);
    }
    uiComboboxOnSelected(apres->bed, onPresentationBedUpdated, apres);
    uiGridAppend(grid, uiControl(apres->bed), 4, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    /* add object checkboxes */
    for (i = 0; i != objects_count; ++i)
    {
        apres->checkBoxes[i] = uiNewCheckbox("");
        uiCheckboxOnToggled(apres->checkBoxes[i], onPresentationObjectToggled, apres);
        uiGridAppend(grid, uiControl(apres->checkBoxes[i]), gridXIndex, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        gridXIndex+=1;

    }

    /* add name */
    apres->name = uiNewEntry();
    uiGridAppend(grid, uiControl(apres->name), 5, gridYIndex, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiEntryOnChanged(apres->name, onPresentationNameUpdated, apres);

    apres->nlang = uiNewCombobox();
    for (i = 0; i != NUM_SUPPORTED_LANGUAGES; ++i)
    {
        uiComboboxAppend(apres->nlang, SUPPORTED_LANGUAGES[i]);
    }
    uiComboboxOnSelected(apres->nlang, onPresentationNameLangUpdated, apres);
    uiGridAppend(grid, uiControl(apres->nlang), 6, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    return PMD_SUCCESS;
}

static 
void 
onAddAudioPresentationButtonClicked
    (uiButton *button
    ,void *data)
{
    unsigned int presentation_count;
    /* button and data have to be passed because that's the way libui works */
    (void)button;
    (void)data;
    presentation_count = getCurrentPresentationCount(studio);

    if (presentation_count == MAX_AUDIO_PRESENTATIONS)
    {
        uiMsgBoxError(ap->window, "error setting presentation", "max presentations reached");
    }
    else
    {
        presentation_count += 1;
        increaseCurrentPresentationCount(studio);
        memset(&ap->presentations[presentation_count-1], '\0', sizeof(ap->presentations[presentation_count-1]));
        add_audio_presentation(ap->grid, &ap->presentations[presentation_count-1], presentation_count, ap->presentations[presentation_count-2].studio);
        pmd_studio_update_model(ap->presentations[presentation_count-1].studio);
    }
}

static 
void
audio_presentation_dynamic_import
    (int pres_import_count
    )
{
    int i, pres_count, count;
    pres_count = getCurrentPresentationCount(studio);
    if (pres_import_count > pres_count)
    {
        count = pres_import_count;
    }
    else
    {
        count = pres_count;
    }
    for (i = pres_count; i < count; i++)
    {
        add_audio_presentation(ap->grid, &ap->presentations[i], i+1, ap->presentations[0].studio);
        setCurrentPresentationCount(studio, pres_import_count);  
    }

    pmd_studio_update_model(ap->presentations[0].studio);

}
static inline
dlb_pmd_success
pmd_studio_audio_presentations_init
    (pmd_studio_audio_presentations *ap1
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

    ap1->window = win;
    
    ap = ap1;
    studio = studio1;

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Presentations")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);


    /* 'add audio presentation' button functionality */
    button = uiNewButton("Add Audio Presentation");
    uiBoxAppend(vbox, uiControl(button), 1);
    uiButtonOnClicked(button, onAddAudioPresentationButtonClicked, button);

    ap->grid = uiNewGrid();
    uiGridSetPadded(ap->grid, 1);
    uiBoxAppend(vbox, uiControl(ap->grid), 0);

    add_grid_title(ap->grid, "En",       1, 0);
    add_grid_title(ap->grid, "Cfg",      2, 0);
    add_grid_title(ap->grid, "Lang",     3, 0);
    add_grid_title(ap->grid, "bed",      4, 0);  
    add_grid_title(ap->grid, "Name",     5, 0);    
    add_grid_title(ap->grid, "NLang",    6, 0);

    gridTitleIndex = 7;
    for (i = 0; i < INIT_AUDIO_OBJECTS; i++)
    {
        sprintf(object_label, "%s%u", "o", i+1);
        add_grid_title(ap->grid, object_label, gridTitleIndex, 0);
        gridTitleIndex+=1;
    } 
    
    for (i = 0; i != INIT_AUDIO_PRESENTATIONS; ++i)
    {
        if (add_audio_presentation(ap->grid, &ap->presentations[i], i+1, studio))
        {
            return PMD_FAIL;
        }
    }

    /* this appends the entire presentations box to the studio window */
    uiBoxAppend(box, uiControl(vbox), 0);
    /* add another (empty) box essentially to give the window some space at the bottom */
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}


static inline
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


static
void
pmd_studio_audio_presentations_refresh_ui
    (pmd_studio_audio_presentations *apres
    )
{
    pmd_studio_audio_presentation *p;
    int i;
    unsigned int j; /* unsigned bc the num it's compared to is unsigned */
    int pres_count, pres_import_count, count;
    uiButton *button;

    p = apres->presentations;
    button = uiNewButton("");
    pres_count = getCurrentPresentationCount(studio);
    pres_import_count = getImportPresentationCount(p->studio);

    if (pres_import_count == pres_count || pres_import_count < pres_count)
    {
        count = pres_count;
    }
    else
    {
        audio_presentation_dynamic_import(pres_import_count);
        count = pres_import_count;
    }

    for (i = 0; i < count; ++i, ++p)
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


static
dlb_pmd_success
pmd_studio_audio_presentations_import
    (pmd_studio_audio_presentations *apres
    ,dlb_pmd_model *m
    )
{
    int pres_count;
    pmd_studio_audio_presentation *p;
    dlb_pmd_presentation_iterator pi;

    if (dlb_pmd_presentation_iterator_init(&pi, m))
    {
        return PMD_FAIL;
    }

    p = apres->presentations;
    pres_count = dlb_pmd_num_presentations(m);
    while (p < &apres->presentations[pres_count])
    {
        if (dlb_pmd_presentation_iterator_next(&pi, &p->presentation, MAX_PRES_ELEMENTS,
                                               p->elements))
        {
            break;
        }
        p->enabled = 1;
        ++p;
    }
    return PMD_SUCCESS;
}


static
void
pmd_studio_audio_presentations_reset
    (pmd_studio_audio_presentations *apres
    )
{
    int i, j, pres_count, obj_count;    
    pmd_studio_audio_presentation *p;
    pres_count = getCurrentPresentationCount(studio);
    obj_count = getCurrentObjectCount(studio);
    p = apres->presentations;
    for (i = 0; i != pres_count; ++i, ++p)
    {
        pmd_studio_audio_presentation_init(p);
        for (j = 0; j < obj_count; j++)
        {
            uiCheckboxSetChecked(p->checkBoxes[j], 0);
        }
     }
    /* wipe existing checkboxes clean */

}


static inline
void
pmd_studio_audio_presentations_finish
    (pmd_studio_audio_presentations *apres
    )
{
    (void)apres;
}


#endif /* PMD_STUDIO_AUDIO_PRESENTATIONS_H_ */
