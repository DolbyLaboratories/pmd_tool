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

#include <math.h>
#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"
#include "pmd_studio_audio_presentations.h"
#include "pmd_studio_audio_objects.h"


/* Definitions */

typedef struct
{
	pmd_studio_audio_objects *objects;

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


struct pmd_studio_audio_objects
{
	pmd_studio *studio;
	uiGrid *grid;
    uiWindow *window;
    pmd_studio_audio_object objects[MAX_AUDIO_OBJECTS];
    unsigned int object_count;

};

static
dlb_pmd_success
add_audio_object
    (uiGrid *gobj
    ,pmd_studio_audio_object *aobj
    ,unsigned int top
    , pmd_studio_audio_objects *aobjs
    );

/* Callbacks */

static
void
onEnableObject
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    pmd_studio_update_model(aobj->objects->studio);
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
    pmd_studio_update_model(aobj->objects->studio);
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
    pmd_studio_update_model(aobj->objects->studio);
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
    pmd_studio_update_model(aobj->objects->studio);
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
    pmd_studio_update_model(aobj->objects->studio);

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
    pmd_studio_update_model(aobj->objects->studio);
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
    pmd_studio_update_model(aobj->objects->studio);
}


static
void
onObjectZUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    aobj->object.z = (float)uiComboboxSelected(c) / 10.0f;
    pmd_studio_update_model(aobj->objects->studio);
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
    pmd_studio_update_model(aobj->objects->studio);
}

static 
void 
onAddAudioObjectButtonClicked
    (uiButton *button 
    ,void *data
    )
{
    pmd_studio_audio_objects *aobjs = (pmd_studio_audio_objects *)data;

    (void)button;

    if (aobjs->object_count == MAX_AUDIO_OBJECTS)
    {
        uiMsgBoxError(aobjs->window, "error setting object", "max objects reached");
    }
    else 
    {
        addAudioObjectToPresentations(aobjs->studio, aobjs->object_count + 1);
        add_audio_object(aobjs->grid, &aobjs->objects[aobjs->object_count], aobjs->object_count + 1, aobjs);
        pmd_studio_update_model(aobjs->studio);
    }
}

/* Private Functions */

static
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

#define PAN_LAW_3DB 1

#define PI_DIV_4 (uiPi / 4.0f)
#define PI_DIV_2 (uiPi / 2.0f)

static
float
pan_up_xy(float x
	     )
{
#ifdef PAN_LAW_3DB
	return(sin((x + 1.0f) * PI_DIV_4));
#else
	return((1.0f + x) / 2.0f);
#endif
}

static
float
pan_down_xy(float x
	     )
{
#ifdef PAN_LAW_3DB
	return(cos((x + 1.0f) * PI_DIV_4));
#else
	return((1.0f - x) / 2.0f);	
#endif
}

static
float
cpan_x_l(float x
	     )
{
	if (x < 0)
	{
#ifdef PAN_LAW_3DB
		return(sin(-x * PI_DIV_2));
#else
		return(-x);
#endif
	}
	else
	{
		return(0);
	}
}

static
float
cpan_x_c(float x
	     )
{
#ifdef PAN_LAW_3DB
	if (x < 0)
	{
		return(sin((x + 1.0f) * PI_DIV_2));
	}
	else
	{
		return(cos(x * PI_DIV_2));
	}
#else
	if (x < 0)
	{
		return(1.0f+x);
	}
	else
	{
		return(1.0f-x);
	}
#endif
}

static
float
cpan_x_r(float x
	     )
{
	if (x < 0)
	{
		return(0);
	}
	else
	{
#ifdef PAN_LAW_3DB
		return(sin(x * PI_DIV_2));
#else
		return(x);
#endif
	}
}

static
float
pan_up_z(float z
	     )
{
#ifdef PAN_LAW_3DB
	return(sin(z * PI_DIV_2));
#else
	return(z);
#endif
}

static
float
pan_down_z(float z
	     )
{
#ifdef PAN_LAW_3DB
	return(cos(z * PI_DIV_2));
#else
	return(1.0f - z);
#endif
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
    aobj->object.y               = 1.0f;
    aobj->object.z               = 0.0f;
    aobj->object.size            = 0;
    aobj->object.size_3d         = 0;
    aobj->object.diverge         = 0;
    aobj->object.source          = 1;
    aobj->object.source_gain     = 0.0f;
    snprintf(aobj->object.name, sizeof(aobj->object.name), "Object %u", aobj->objects->object_count + 1);
}


static
dlb_pmd_success
add_audio_object
    (uiGrid *gobj
    ,pmd_studio_audio_object *aobj
    ,unsigned int top
    , pmd_studio_audio_objects *aobjs
    )
{
    char tmp[32];
    int i;

    aobj->objects     = aobjs;
    pmd_studio_audio_object_init(aobj);        
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
    uiComboboxSetSelected(aobj->ch, (int)aobj->objects->object_count);
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
    uiComboboxSetSelected(aobj->x, (unsigned int)((aobj->object.x * 10.0) + 10.0));
    uiComboboxOnSelected(aobj->x, onObjectXUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->x), 6, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->y = uiNewCombobox();
    for (i = -10; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->y, tmp);
    }
    uiComboboxSetSelected(aobj->y, (unsigned int)((aobj->object.y * 10.0) + 10.0));
    uiComboboxOnSelected(aobj->y, onObjectYUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->y), 7, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->z = uiNewCombobox();
    for (i = 0; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->z, tmp);
    }
    uiComboboxSetSelected(aobj->z, (unsigned int)(aobj->object.z * 10.0));
    uiComboboxOnSelected(aobj->z, onObjectZUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->z), 8, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aobj->name = uiNewEntry();
    uiEntrySetText(aobj->name, (const char*)aobj->object.name);
    uiEntryOnChanged(aobj->name, onObjectNameUpdated, aobj);
    uiGridAppend(gobj, uiControl(aobj->name), 9, top, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    aobjs->object_count++;
    return PMD_SUCCESS;
}

/* Public Functions */

dlb_pmd_success
pmd_studio_audio_objects_init
    (pmd_studio_audio_objects **ao1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    )
{
    unsigned int i;
    uiBox *vbox;
    uiButton *button;


    *ao1 = malloc(sizeof(pmd_studio_audio_objects));
    if (!*ao1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Failed to allocate memory for audio objects");
    	return(PMD_FAIL);
    }

    (*ao1)->window = win;


    /* 
     * Initialize the global audio_objects & studio. 
     * There is probably a more effective way to do this but I'm not sure.
     */
    (*ao1)->studio = studio1;

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Objects")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    button = uiNewButton("Add Audio Object");
    uiBoxAppend(vbox, uiControl(button), 1);
    uiButtonOnClicked(button, onAddAudioObjectButtonClicked, *ao1);

    (*ao1)->grid = uiNewGrid();
    uiGridSetPadded((*ao1)->grid, 1);
    uiBoxAppend(vbox, uiControl((*ao1)->grid), 0);

    add_grid_title((*ao1)->grid, "En",       1, 0);
    add_grid_title((*ao1)->grid, "Type",     2, 0);
    add_grid_title((*ao1)->grid, "Div",      3, 0);
    add_grid_title((*ao1)->grid, "Ch",       4, 0);    
    add_grid_title((*ao1)->grid, "Gain(dB)", 5, 0);
    add_grid_title((*ao1)->grid, "X",        6, 0);
    add_grid_title((*ao1)->grid, "Y",        7, 0);
    add_grid_title((*ao1)->grid, "Z",        8, 0);
    add_grid_title((*ao1)->grid, "Name",     9, 0);
    
    (*ao1)->object_count = 0;

    for (i = 0; i != INIT_AUDIO_OBJECTS; ++i)
    {
        if (add_audio_object((*ao1)->grid, &((*ao1)->objects[i]), i+1, *ao1))
        {
            pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to add audio object");
            return PMD_FAIL;
        }
    }

    uiBoxAppend(box, uiControl(vbox), 0);
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}

void
pmd_studio_audio_objects_refresh_ui
    (pmd_studio_audio_objects *aobjs
    )
{
    pmd_studio_audio_object *a;
    unsigned int i;

    a = aobjs->objects;
   
    for (i = 0; i < aobjs->object_count; ++i, ++a)
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

dlb_pmd_success
pmd_studio_audio_objects_import
    (pmd_studio_audio_objects *aobjs
    ,dlb_pmd_model *m
    )
{
	unsigned int i;
    pmd_studio_audio_object *a;
    dlb_pmd_object_iterator oi;
    unsigned int new_object_count = dlb_pmd_num_objects(m);

    if (dlb_pmd_object_iterator_init(&oi, m))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to initialize object iterator");
        return PMD_FAIL;
    }

    /* Create missing objects required for import */
    if (new_object_count > aobjs->object_count)
    {
    	for (i = aobjs->object_count ; i < new_object_count ; i++)
    	{
    		add_audio_object(aobjs->grid, &aobjs->objects[i], i+1, aobjs);
        }
    }

    /* Presentations need to know about objects to have correct # of checkboxes */

    /*
     * Function taken from dlb_pmd_api_read.c
     * Used so so the correct number of objects are imported since it 
     * can vary depending on the xml 
     */
    a = aobjs->objects;

    for (i = 0 ; i < aobjs->object_count ; i++)
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

void
pmd_studio_audio_objects_reset
    (pmd_studio_audio_objects *aobjs
    )
{
    pmd_studio_audio_object *a;
    unsigned int i;

    a = aobjs->objects;
    for (i = 0; i < aobjs->object_count ; ++i, ++a)
    {
        pmd_studio_audio_object_init(a);
    }
}

unsigned int
pmd_studio_object_get_count
    (pmd_studio *studio
    )
{
	return(pmd_studio_get_objects(studio)->object_count);
}

void
pmd_studio_audio_objects_update_model(
	    pmd_studio_audio_objects *aobjs,
	     dlb_pmd_model *m	
)
{
	pmd_studio_audio_object *aobj = aobjs->objects;
	unsigned int i;
    
    for (i = 0; i < aobjs->object_count; ++i, ++aobj)
    {
        if (aobj->enabled)
        {
            if (dlb_pmd_set_object(m, &aobj->object))
            {
                uiMsgBoxError(aobjs->window, "error setting object", dlb_pmd_error(m));
            }
        }
    }
}

dlb_pmd_success pmd_studio_audio_objects_get_mix_matrix(

	unsigned int id,
	dlb_pmd_speaker_config config,
	pmd_studio_mix_matrix mix_matrix,
	pmd_studio *studio)
{
	pmd_studio_audio_objects *aobjs = pmd_studio_get_objects(studio);
	pmd_studio_audio_object *aobj;
	unsigned int object_index, channel;

	// Find object
	for (object_index = 0 ; object_index < aobjs->object_count ; object_index++)
	{
		if (aobjs->objects[object_index].object.id == id)
		{
			break;
		}
	}

	// Check we found the object and that it is enabled
	if ((object_index >= aobjs->object_count) || !aobjs->objects[object_index].enabled)
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Referenced object either doesn't exist or is not enabled");
		return(PMD_FAIL);
	}
	aobj = &aobjs->objects[object_index];

	channel = uiComboboxSelected(aobj->ch);

	if ((config != DLB_PMD_SPEAKER_CONFIG_5_1_4) &&
		(config != DLB_PMD_SPEAKER_CONFIG_5_1) &&
		(config != DLB_PMD_SPEAKER_CONFIG_2_0))
	{
		pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Unsupported configuration, only 5.1.4, 5.1 and 2/0 supported in streaming mode");
        return(PMD_FAIL);
	}
	// Left
	mix_matrix[channel][0] = cpan_x_l(aobj->object.x)    * pan_up_xy(aobj->object.y)   * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
	// Right
	mix_matrix[channel][1] = cpan_x_r(aobj->object.x)    * pan_up_xy(aobj->object.y)   * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
	// Centre
	mix_matrix[channel][2] = cpan_x_c(aobj->object.x)    * pan_up_xy(aobj->object.y)   * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);

	// Left Surround
	mix_matrix[channel][4] = pan_down_xy(aobj->object.x) * pan_down_xy(aobj->object.y) * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
	// Right Surround
	mix_matrix[channel][5] = pan_up_xy(aobj->object.x)   * pan_down_xy(aobj->object.y) * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
	// Top Front Left
	mix_matrix[channel][6] = pan_down_xy(aobj->object.x) * pan_up_xy(aobj->object.y)   * pan_up_z(aobj->object.z)   * gain_from_db(aobj->object.source_gain);
    // Top Front Right
	mix_matrix[channel][7] = pan_up_xy(aobj->object.x)   * pan_up_xy(aobj->object.y)   * pan_up_z(aobj->object.z)   * gain_from_db(aobj->object.source_gain);
    // Top Back Left
	mix_matrix[channel][8] = pan_down_xy(aobj->object.x) * pan_down_xy(aobj->object.y) * pan_up_z(aobj->object.z)   * gain_from_db(aobj->object.source_gain);
    // Top Back Right
	mix_matrix[channel][9] = pan_up_xy(aobj->object.x)   * pan_down_xy(aobj->object.y) * pan_up_z(aobj->object.z)   * gain_from_db(aobj->object.source_gain);

    if (config == DLB_PMD_SPEAKER_CONFIG_5_1_4)
    {
    	return(PMD_SUCCESS);
    }
    // Downmix the Heights into the fronts and surrounds
	mix_matrix[channel][0] += PMD_STUDIO_M3DB * mix_matrix[channel][6];
	mix_matrix[channel][1] += PMD_STUDIO_M3DB * mix_matrix[channel][7];
	mix_matrix[channel][4] += PMD_STUDIO_M3DB * mix_matrix[channel][8];
	mix_matrix[channel][5] += PMD_STUDIO_M3DB * mix_matrix[channel][9];
	mix_matrix[channel][6] = mix_matrix[channel][7] = mix_matrix[channel][8] = mix_matrix[channel][9] = 0.0f;

    if (config == DLB_PMD_SPEAKER_CONFIG_5_1)
    {
    	return(PMD_SUCCESS);
    }

	// Go to L/R without C
	mix_matrix[channel][0] = pan_down_xy(aobj->object.x) * pan_up_xy(aobj->object.y) * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
	mix_matrix[channel][1] = pan_up_xy(aobj->object.x)   * pan_up_xy(aobj->object.y) * pan_down_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
    // Mix the front heights back in
    mix_matrix[channel][0] += PMD_STUDIO_M3DB * pan_down_xy(aobj->object.x) * pan_up_xy(aobj->object.y) * pan_up_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
    mix_matrix[channel][1] += PMD_STUDIO_M3DB * pan_up_xy(aobj->object.x)   * pan_up_xy(aobj->object.y) * pan_up_z(aobj->object.z) * gain_from_db(aobj->object.source_gain);
    // Mix in surrounds
    mix_matrix[channel][0] += PMD_STUDIO_M3DB * mix_matrix[channel][4];
    mix_matrix[channel][1] += PMD_STUDIO_M3DB * mix_matrix[channel][5];        
	mix_matrix[channel][2] = mix_matrix[channel][4] = mix_matrix[channel][5] = 0.0f;
	return(PMD_SUCCESS);
}


void
pmd_studio_audio_objects_finish
    (pmd_studio_audio_objects *aobj
    )
{
    free(aobj);
}
