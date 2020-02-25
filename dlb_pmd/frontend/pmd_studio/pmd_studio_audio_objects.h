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


typedef struct
{
    pmd_studio *studio;
    dlb_pmd_bool enabled;

    dlb_pmd_object object;

    uiCombobox *ty;
    uiEntry *name;
    uiCombobox *ch;
    uiCheckbox *enable;
    uiCheckbox *diverge;
    uiCombobox *gain;
    uiCombobox *x;
    uiCombobox *y;
    uiCombobox *z;
} pmd_studio_audio_object;


typedef struct
{
    uiWindow *window;
    pmd_studio_audio_object objects[MAX_AUDIO_OBJECTS];
} pmd_studio_audio_objects;
    
/* Initialize these outside of any function so they can keep their values in between calls,
i.e. so you can add to the grid with multiple add audio object calls */
uiGrid *grid_object;
static pmd_studio_audio_objects *ao;


static
void
onEnableObject
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectClassUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.object_class = (dlb_pmd_object_class)uiComboboxSelected(c);
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectDivergeToggled
    (uiCheckbox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.diverge = (dlb_pmd_bool)uiCheckboxChecked(c);
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectChannelUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.source = (dlb_pmd_signal)(1+(int)uiComboboxSelected(c));
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectGainUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    int value = uiComboboxSelected(c);
    aobj->object.source_gain = (value == 63)
        ? -INFINITY
        : ((float)(62-value) * 0.5f) - 25.0f;
    pmd_studio_update_model(aobj->studio);

}


static
void
onObjectXUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.x = ((float)uiComboboxSelected(c) -10.0f) / 10.0f;
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectYUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.y = ((float)uiComboboxSelected(c) -10.0f) / 10.0f;
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectZUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.z = ((float)uiComboboxSelected(c) -10.0f) / 10.0f;
    pmd_studio_update_model(aobj->studio);
}


static
void
onObjectNameUpdated
    (uiEntry *e
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    snprintf(aobj->object.name, sizeof(aobj->object.name), "%s", uiEntryText(e));
    pmd_studio_update_model(aobj->studio);
}


static
void
pmd_studio_audio_object_init
    (pmd_studio_audio_object *aobj
    )
{
    aobj->enabled    = 0;
    aobj->object.object_class    = PMD_CLASS_GENERIC;
    aobj->object.dynamic_updates = 0;
    aobj->object.x               = 0.0f;
    aobj->object.y               = 0.0f;
    aobj->object.z               = 0.0f;
    aobj->object.size            = 0;
    aobj->object.size_3d         = 0;
    aobj->object.diverge         = 0;
    aobj->object.source          = 1;
    aobj->object.source_gain     = 0.0f;
    snprintf(aobj->object.name, sizeof(aobj->object.name), "Object %u", aobj->object.id);
}



static inline
dlb_pmd_success
add_audio_object
    (uiGrid *gobj
    ,pmd_studio_audio_object *aobj
    ,unsigned int top
    ,pmd_studio *s
    )
{
    int obj_import_count;
    char tmp[32];
    int i;

    aobj->studio     = s;
    obj_import_count = getImportObjectCount(s);
    if (obj_import_count == -1)
    {
        pmd_studio_audio_object_init(aobj);        
    }
    else if (aobj->object.id > 255 || aobj->object.id == 0)
    {
        pmd_studio_audio_object_init(aobj);        
    }
    aobj->object.id = (dlb_pmd_element_id)(MAX_AUDIO_BEDS + top);

    snprintf(tmp, sizeof(tmp), "%d", top);
    uiGridAppend(gobj, uiControl(uiNewLabel(tmp)), 0, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(aobj->enable, onEnableObject, aobj);
    uiGridAppend(gobj, uiControl(aobj->enable), 1, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->ty = uiNewCombobox();
    uiComboboxAppend(aobj->ty, "Dialog");
    uiComboboxAppend(aobj->ty, "VDS");
    uiComboboxAppend(aobj->ty, "Voice Over");
    uiComboboxAppend(aobj->ty, "Generic");
    uiComboboxAppend(aobj->ty, "Spkn Subt");
    uiComboboxAppend(aobj->ty, "Emrg Alert");
    uiComboboxAppend(aobj->ty, "Emrg Info");
    uiComboboxSetSelected(aobj->ty, (int)aobj->object.object_class);
    uiComboboxOnSelected(aobj->ty, onObjectClassUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->ty), 2, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->diverge = uiNewCheckbox("");
    uiCheckboxOnToggled(aobj->diverge, onObjectDivergeToggled, aobj);
    uiGridAppend(gobj, uiControl(aobj->diverge), 3, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);    

    aobj->ch = uiNewCombobox();    
    for (i = 1; i != MAX_AUDIO_SIGNALS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i);
        uiComboboxAppend(aobj->ch, tmp);
    }
    uiComboboxOnSelected(aobj->ch, onObjectChannelUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->ch), 4, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->gain = uiNewCombobox();
    for (i = 62; i >= 0; --i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i * 0.5) - 25.0);
        uiComboboxAppend(aobj->gain, tmp);
    }
    uiComboboxAppend(aobj->gain, "-inf");

    uiComboboxSetSelected(aobj->gain, 12); /* default to 0.0 dB */
    uiComboboxOnSelected(aobj->gain, onObjectGainUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->gain), 5, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->x = uiNewCombobox();
    for (i = -10; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->x, tmp);
    }
    uiComboboxSetSelected(aobj->x, 10);
    uiComboboxOnSelected(aobj->x, onObjectXUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->x), 6, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->y = uiNewCombobox();
    for (i = -10; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->y, tmp);
    }
    uiComboboxSetSelected(aobj->y, 10);
    uiComboboxOnSelected(aobj->y, onObjectYUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->y), 7, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->z = uiNewCombobox();
    for (i = -10; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->z, tmp);
    }
    uiComboboxSetSelected(aobj->z, 10);
    uiComboboxOnSelected(aobj->z, onObjectZUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->z), 8, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->name = uiNewEntry();
    uiEntryOnChanged(aobj->name, onObjectNameUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->name), 9, top, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    return PMD_SUCCESS;
}

static 
void 
onAddAudioObjectButtonClicked
    (uiButton *button 
    ,void *data
    )
{
    int pIndex, object_count, presentation_count, cbIndex;
    int gridXIndex, gridYIndex;
    char object_label[16];

    (void)button;
    (void)data;

    object_count = getCurrentObjectCount(studio);
    if (object_count== MAX_AUDIO_OBJECTS)
    {
        uiMsgBoxError(ao->window, "error setting object", "max objects reached");
    }
    else 
    {
        uiGrid *pres_grid;
        object_count+=1;
        increaseCurrentObjectCount(studio);
        presentation_count = getCurrentPresentationCount(studio);
        pres_grid = getAudioPresentationsGrid(ap);
        cbIndex = 0;
        sprintf(object_label, "%s%u", "o", object_count);
        add_grid_title(pres_grid, object_label, object_count+6, 0);
        /* initialize the object's checkbox for each presentation */
        for (pIndex = 0; pIndex < presentation_count; pIndex++)
        {     
            uiCheckbox * cb = getCheckBox(studio, pIndex, object_count-1);  /* TODO: get here... */
            cb = uiNewCheckbox("");                                         /* TODO: ...and new here?  WTF? */
            ap->presentations[pIndex].checkBoxes[object_count-1] = cb;
            gridXIndex = object_count+6;
            gridYIndex = cbIndex+1;
            uiGridAppend(pres_grid, uiControl(ap->presentations[pIndex].checkBoxes[object_count-1] = cb), gridXIndex, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
            uiCheckboxOnToggled(ap->presentations[pIndex].checkBoxes[object_count-1], onPresentationObjectToggled, &ap->presentations[pIndex]);
            cbIndex +=1;
        } 
        add_audio_object(grid_object, &ao->objects[object_count-1], object_count, studio);
        pmd_studio_update_model(ao->objects[object_count-1].studio);
    }
}

static 
void 
audio_object_dynamic_import
    (int obj_import_count
    )
{
    /* 
     * this function adds the objects automatically upon import from xml. Furthermore,
     *it adds all the necessary checkboxes to the presentations already present. 
     */
    int i, obj_count, pres_count, pIndex;
    char object_label[16];
    uiGrid *pres_grid;
    uiCheckbox * cb;

    pres_grid = getAudioPresentationsGrid(ap);
    obj_count = getCurrentObjectCount(studio);
    for (i = obj_count; i < obj_import_count; i++)
    {
        add_audio_object(grid_object, &ao->objects[i], i+1, ao->objects[0].studio);
        setCurrentObjectCount(studio, obj_import_count);  
        pmd_studio_update_model(ao->objects[0].studio);
    }

    pres_count = getCurrentPresentationCount(studio);
    for (pIndex = 0; pIndex < pres_count; pIndex++)
    {
        for (i = obj_count; i < obj_import_count; i++)
        {     
            if (pIndex == 0) /* add the object labels at the start */
            {
                sprintf(object_label, "%s%u", "o", i+1);
                add_grid_title(pres_grid, object_label, i+7, 0);
            }
            cb = getCheckBox(studio, pIndex ,i);
            cb = uiNewCheckbox("");
            ap->presentations[pIndex].checkBoxes[i] = cb;
            uiGridAppend(pres_grid, uiControl(ap->presentations[pIndex].checkBoxes[i] = cb), i+7, pIndex+1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
            uiCheckboxOnToggled(ap->presentations[pIndex].checkBoxes[i], onPresentationObjectToggled, &ap->presentations[0]);
        }
    } 
}




static inline
dlb_pmd_success
pmd_studio_audio_objects_init
    (pmd_studio_audio_objects *ao1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    )
{
    unsigned int i;
    uiBox *vbox;
    uiButton *button;
    ao1->window = win;

    /* 
     * Initialize the global audio_objects & studio. 
     * There is probably a more effective way to do this but I'm not sure.
     */
    ao = ao1;
    studio = studio1;

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Objects")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    button = uiNewButton("Add Audio Object");
    uiBoxAppend(vbox, uiControl(button), 1);
    uiButtonOnClicked(button, onAddAudioObjectButtonClicked, studio);

    grid_object = uiNewGrid();
    uiGridSetPadded(grid_object, 1);
    uiBoxAppend(vbox, uiControl(grid_object), 0);

    add_grid_title(grid_object, "En",       1, 0);
    add_grid_title(grid_object, "Type",     2, 0);
    add_grid_title(grid_object, "Div",      3, 0);
    add_grid_title(grid_object, "Ch",       4, 0);    
    add_grid_title(grid_object, "Gain(dB)", 5, 0);
    add_grid_title(grid_object, "X",        6, 0);
    add_grid_title(grid_object, "Y",        7, 0);
    add_grid_title(grid_object, "Z",        8, 0);
    add_grid_title(grid_object, "Name",     9, 0);
    
    for (i = 0; i != INIT_AUDIO_OBJECTS; ++i)
    {
        if (add_audio_object(grid_object, &ao->objects[i], i+1, studio))
        {
            return PMD_FAIL;
        }
    }

    uiBoxAppend(box, uiControl(vbox), 0);
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}


static inline
int
ui_gain
    (float gain
    )
{
    if (isinf(gain) && gain < 0)
    {
        return 63;
    }
    else
    {
        /* Different calculations depending on if gain is negative or positive */
        if (gain > 0)
        {
            return (int)((gain*2)-12)*-1;
        }
        else if (gain < 0)
        {
            return (int)(gain * -2) + 12;
        }
        else {
            return (int)12;
        }        
    }
}


static
void
pmd_studio_audio_objects_refresh_ui
    (pmd_studio_audio_objects *aobj
    )
{
    pmd_studio_audio_object *a;
    int i;
    int obj_count, obj_import_count, count;
    uiButton *button;

    a = aobj->objects;
    obj_count = getCurrentObjectCount(studio);
    obj_import_count = getImportObjectCount(studio);
    button = uiNewButton("");

    /* 
        obj_count is the number of objects that are visually present in the UI
        obj_import_count is however many are present in the xml
    */

    if (obj_import_count == obj_count || obj_import_count < obj_count)
    {
        /* If there are enough objects present in the studio */
        count = obj_count;
    }
    else
    {
        /* Add objects dynamically upon import */
        audio_object_dynamic_import(obj_import_count);
        count = obj_import_count;
    }
    
    for (i = 0; i < count; ++i, ++a)
    {
        uiCheckboxSetChecked (a->enable,  a->enabled);
        uiComboboxSetSelected(a->ty,      (int)a->object.object_class);
        uiCheckboxSetChecked (a->diverge, a->object.diverge);
        uiComboboxSetSelected(a->ch,      (int)a->object.source - 1);
        uiComboboxSetSelected(a->gain,    ui_gain(a->object.source_gain));
        /*
         * Check if any of the x,y,z positions is zero
         * Separate process if position equals 1, -1, or 0
         */
        if ((a->object.x ==  -1) || (a->object.x ==  1) || (a->object.x ==  0)){
            uiComboboxSetSelected(a->x,       (int)((1 + a->object.x) * 10.0f));
        }
        else {
            uiComboboxSetSelected(a->x,       (int)((1.1 + a->object.x) * 10.0f));

        }
        if ((a->object.y ==  -1) || (a->object.y ==  1) || (a->object.y ==  0)){
            uiComboboxSetSelected(a->y,       (int)((1 + a->object.y) * 10.0f));
        }
        else {
            uiComboboxSetSelected(a->y,       (int)((1.1 + a->object.y) * 10.0f));

         }
        if ((a->object.z ==  -1) || (a->object.z ==  1) || (a->object.z ==  0)){
            uiComboboxSetSelected(a->z,       (int)((1 + a->object.z) * 10.0f));
        }
        else {
            uiComboboxSetSelected(a->z,       (int)((1.1 + a->object.z) * 10.0f));

        }
        uiEntrySetText(a->name, a->object.name);
    }
}


static
dlb_pmd_success
pmd_studio_audio_objects_import
    (pmd_studio_audio_objects *aobj
    ,dlb_pmd_model *m
    )
{
    int object_count;
    pmd_studio_audio_object *a;
    dlb_pmd_object_iterator oi;

    if (dlb_pmd_object_iterator_init(&oi, m))
    {
        return PMD_FAIL;
    }

    a = aobj->objects;

    /*
     * Function taken from dlb_pmd_api_read.c
     * Used so so the correct number of objects are imported since it 
     * can vary depending on the xml 
     */
    object_count = dlb_pmd_num_objects(m); 

    while (a < &aobj->objects[object_count])
    {
        if (dlb_pmd_object_iterator_next(&oi, &a->object))
        {
            break;
        }
        a->enabled = 1;
        ++a;
    }
    return PMD_SUCCESS;
}


static
void
pmd_studio_audio_objects_reset
    (pmd_studio_audio_objects *aobj
    )
{
    pmd_studio_audio_object *a;
    int i, obj_count;
    obj_count = getCurrentObjectCount(studio);
    a = aobj->objects;
    for (i = 0; i != obj_count; ++i, ++a)
    {
        pmd_studio_audio_object_init(a);
    }
}




static inline
void
pmd_studio_audio_objects_finish
    (pmd_studio_audio_objects *aobj
    )
{
    (void)aobj;
}


#endif /* PMD_STUDIO_AUDIO_OBJECTS_H_ */
