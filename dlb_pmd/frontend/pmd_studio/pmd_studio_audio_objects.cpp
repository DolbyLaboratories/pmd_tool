/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include <math.h>
#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"
#include "pmd_studio_audio_presentations.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_audio_objects_pvt.h"

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

    pmd_studio_presentations_object_enable(aobj->object.id, aobj->enabled, aobj->objects->studio);

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
    aobj->object.source_gain = pmd_studio_combobox_index_to_gaindb(uiComboboxSelected(aobj->gain));
    pmd_studio_update_model(aobj->objects->studio);
}


void
onObjectGainUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_object *aobj = (pmd_studio_audio_object *)data;
    int value = uiComboboxSelected(c);
    aobj->object.source_gain = pmd_studio_combobox_index_to_gaindb(value);
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
dlb_pmd_success
pmd_studio_audio_object_init
    (pmd_studio_audio_object *aobj,
     pmd_studio *studio
    )
{
    aobj->objects = pmd_studio_get_objects(studio);
    aobj->enabled    = 0;
    if (pmd_studio_eid_get_next(studio, &aobj->object.id))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to get eid for new object");
        return(PMD_FAIL);
    }
    // The preference here is that when in live mode we select dialog based objects as default
    // as this class is more likely to work with encoders. When in file mode we revert to
    // the more correct practice of selecting generic by default
    if (pmd_studio_get_mode(studio) == PMD_STUDIO_MODE_FILE_EDIT)
    {
        aobj->object.object_class    = PMD_CLASS_GENERIC;
    }
    else
    {
        aobj->object.object_class    = PMD_CLASS_DIALOG;        
    }
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
    return(PMD_SUCCESS);
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
    unsigned int xPos = 0;

    aobj->objects     = aobjs;
    if (pmd_studio_audio_object_init(aobj, aobjs->studio) != PMD_SUCCESS)
    {
        return(PMD_FAIL);
    } 

    snprintf(tmp, sizeof(tmp), "%d", top);
    aobj->label = uiNewLabel(tmp);

    aobj->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(aobj->enable, onEnableObject, aobj);
    // Object enable checkbox enabled by default
    uiCheckboxSetChecked(aobj->enable, 1);
    aobjs->object_count++;
    pmd_studio_presentations_add_audio_object_to_presentations(aobjs->studio, aobj->object.id);
    aobj->enabled = 1;
    pmd_studio_presentations_object_enable(aobj->object.id, aobj->enabled, aobjs->studio);

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

    // Divergence available in file based mode only
    if (pmd_studio_get_mode(aobjs->studio) == PMD_STUDIO_MODE_FILE_EDIT)
    {
        aobj->diverge = uiNewCheckbox("");
        uiCheckboxOnToggled(aobj->diverge, onObjectDivergeToggled, aobj);
    }
    aobj->ch = uiNewCombobox();    
    for (i = 1; i != MAX_STUDIO_AUDIO_SIGNALS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i);
        uiComboboxAppend(aobj->ch, tmp);
    }
    uiComboboxSetSelected(aobj->ch, 0);
    uiComboboxOnSelected(aobj->ch, onObjectChannelUpdated, aobj);

    aobj->gain = uiNewCombobox();
    for (i = 62; i >= 0; --i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i * 0.5) - 25.0);
        uiComboboxAppend(aobj->gain, tmp);
    }
    uiComboboxAppend(aobj->gain, "-inf");

    uiComboboxSetSelected(aobj->gain, 12); /* default to 0.0 dB */
    uiComboboxOnSelected(aobj->gain, onObjectGainUpdated, aobj);

    aobj->x = uiNewCombobox();
    for (i = -10; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->x, tmp);
    }
    uiComboboxSetSelected(aobj->x, (unsigned int)((aobj->object.x * 10.0) + 10.0));
    uiComboboxOnSelected(aobj->x, onObjectXUpdated, aobj);

    aobj->y = uiNewCombobox();
    for (i = -10; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->y, tmp);
    }
    uiComboboxSetSelected(aobj->y, (unsigned int)((aobj->object.y * 10.0) + 10.0));
    uiComboboxOnSelected(aobj->y, onObjectYUpdated, aobj);

    aobj->z = uiNewCombobox();
    for (i = 0; i <= 10; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i / 10.0));
        uiComboboxAppend(aobj->z, tmp);
    }
    uiComboboxSetSelected(aobj->z, (unsigned int)(aobj->object.z * 10.0));
    uiComboboxOnSelected(aobj->z, onObjectZUpdated, aobj);

    aobj->name = uiNewEntry();
    uiEntrySetText(aobj->name, (const char*)aobj->object.name);
    uiEntryOnChanged(aobj->name, onObjectNameUpdated, aobj);

    uiGridAppend(gobj, uiControl(aobj->label),   xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(gobj, uiControl(aobj->enable),  xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(gobj, uiControl(aobj->ty),      xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    if (pmd_studio_get_mode(aobjs->studio) == PMD_STUDIO_MODE_FILE_EDIT)
    {
    	uiGridAppend(gobj, uiControl(aobj->diverge), xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    }   
    uiGridAppend(gobj, uiControl(aobj->ch),      xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(gobj, uiControl(aobj->gain),    xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(gobj, uiControl(aobj->x),       xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(gobj, uiControl(aobj->y),       xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(gobj, uiControl(aobj->z),       xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);    
    uiGridAppend(gobj, uiControl(aobj->name),    xPos++, top, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

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
    uiBox *vbox;
    unsigned int xPos;

    *ao1 = (pmd_studio_audio_objects *) malloc(sizeof(pmd_studio_audio_objects));
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
    (*ao1)->add_object_button = uiNewButton("Add Audio Object");
    uiBoxAppend(vbox, uiControl((*ao1)->add_object_button), 0);
    uiButtonOnClicked((*ao1)->add_object_button, onAddAudioObjectButtonClicked, *ao1);

    (*ao1)->grid = uiNewGrid();
    uiGridSetPadded((*ao1)->grid, 1);
    uiBoxAppend(vbox, uiControl((*ao1)->grid), 0);
    xPos = 1;
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("En")),       xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Type")),     xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
     // Divergence available in file based mode only
    if (pmd_studio_get_mode(studio1) == PMD_STUDIO_MODE_FILE_EDIT)
    {
    	uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Div")),  xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    }
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Ch")),       xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Gain(dB)")), xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("X")),        xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Y")),        xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Z")),        xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ao1)->grid, uiControl(uiNewLabel("Name")),     xPos++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    
    (*ao1)->object_count = 0;

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
        uiComboboxSetSelected(a->ch,      (int)a->object.source - 1);
        uiComboboxSetSelected(a->gain,    ui_gain(a->object.source_gain));
        /*
         * Check if any of the x,y,z positions is zero
         * Separate process if position equals 1, -1, or 0
         */
        if ((a->object.x ==  -1) || (a->object.x ==  1) || (a->object.x ==  0))
        {
            uiComboboxSetSelected(a->x,       (int)((1 + a->object.x) * 10.0f));
        }
        else 
        {
            uiComboboxSetSelected(a->x,       (int)((1.1 + a->object.x) * 10.0f));

        }
        
        if ((a->object.y ==  -1) || (a->object.y ==  1) || (a->object.y ==  0))
        {
            uiComboboxSetSelected(a->y,       (int)((1 + a->object.y) * 10.0f));
        }
        else 
        {
            uiComboboxSetSelected(a->y,       (int)((1.1 + a->object.y) * 10.0f));

        }

        if ((a->object.z ==  1) || (a->object.z ==  0))
        {
            uiComboboxSetSelected(a->z,       (int)((a->object.z) * 10.0f));
        }
        else 
        {
            uiComboboxSetSelected(a->z,       (int)((0.1 + a->object.z) * 10.0f));
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

    pmd_studio *studio = aobjs->studio;

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

    dlb_pmd_element_id *obj_eids;
    unsigned int num_obj_eids = pmd_studio_audio_objects_get_eids(&obj_eids, studio, false);

    (void)num_obj_eids;
    for (i = 0 ; i < aobjs->object_count ; i++)
    {
        // Since addition of eid management, need to preserve allocated object id
        dlb_pmd_element_id original = a->object.id;

        // The iterator automatically overwrites the object id.
        if (dlb_pmd_object_iterator_next(&oi, &a->object))
        {
            // No more objects
            break;
        }

        // Overwrite references to old eid with new eid
        pmd_studio_eid_replace(studio, original, a->object.id);
        
        a->enabled = 1;

        /* as we are manually enabling all imported objects we must enable them in the presentations too */
        pmd_studio_presentations_object_enable(a->object.id, 1, aobjs->studio);

        ++a;
    }
    return PMD_SUCCESS;
}

void
pmd_studio_audio_objects_reset
    (pmd_studio_audio_objects *aobjs
    )
{
    unsigned int i;

    for (i = 0 ; i < aobjs->object_count ; i++)
    {
        if(pmd_studio_get_mode(aobjs->studio) == PMD_STUDIO_MODE_FILE_EDIT){
            uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].diverge));
            uiControlDestroy(uiControl(aobjs->objects[i].diverge));
        }
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].label));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].ty));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].name));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].ch));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].enable));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].gain));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].x));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].y));
        uiGridDelete(aobjs->grid, uiControl(aobjs->objects[i].z));
        uiControlDestroy(uiControl(aobjs->objects[i].label));
        uiControlDestroy(uiControl(aobjs->objects[i].ty));
        uiControlDestroy(uiControl(aobjs->objects[i].name));
        uiControlDestroy(uiControl(aobjs->objects[i].ch));
        uiControlDestroy(uiControl(aobjs->objects[i].enable));
        uiControlDestroy(uiControl(aobjs->objects[i].gain));
        uiControlDestroy(uiControl(aobjs->objects[i].x));
        uiControlDestroy(uiControl(aobjs->objects[i].y));
        uiControlDestroy(uiControl(aobjs->objects[i].z));

    }

    aobjs->object_count = 0;
}

void
pmd_studio_audio_objects_enable
    (pmd_studio_audio_objects *aobjs
    )
{
    unsigned int i;

    for (i = 0; i < aobjs->object_count; i++)
    {
        if(pmd_studio_get_mode(aobjs->studio) == PMD_STUDIO_MODE_FILE_EDIT){
            uiControlEnable(uiControl(aobjs->objects[i].diverge));
        }
        uiControlEnable(uiControl(aobjs->objects[i].ty));
        uiControlEnable(uiControl(aobjs->objects[i].name));
        uiControlEnable(uiControl(aobjs->objects[i].ch));
        uiControlEnable(uiControl(aobjs->objects[i].enable));
        uiControlEnable(uiControl(aobjs->objects[i].gain));
        uiControlEnable(uiControl(aobjs->objects[i].x));
        uiControlEnable(uiControl(aobjs->objects[i].y));
        uiControlEnable(uiControl(aobjs->objects[i].z));
    }
    uiControlEnable(uiControl(aobjs->add_object_button));
}

void
pmd_studio_audio_objects_disable
    (pmd_studio_audio_objects *aobjs,
    bool live_mode
    )
{
    unsigned int i;

    uiControlDisable(uiControl(aobjs->add_object_button));
    for (i = 0; i < aobjs->object_count; i++)
    {
        uiControlDisable(uiControl(aobjs->objects[i].ty));
        uiControlDisable(uiControl(aobjs->objects[i].name));
        uiControlDisable(uiControl(aobjs->objects[i].ch));
        uiControlDisable(uiControl(aobjs->objects[i].enable));
        if(pmd_studio_get_mode(aobjs->studio) == PMD_STUDIO_MODE_FILE_EDIT){
            uiControlDisable(uiControl(aobjs->objects[i].diverge));
        }
        if(!live_mode)
        {
            uiControlDisable(uiControl(aobjs->objects[i].x));
            uiControlDisable(uiControl(aobjs->objects[i].y));
            uiControlDisable(uiControl(aobjs->objects[i].z));
            uiControlDisable(uiControl(aobjs->objects[i].gain));
        }
        else
        {
            uiControlEnable(uiControl(aobjs->objects[i].x));
            uiControlEnable(uiControl(aobjs->objects[i].y));
            uiControlEnable(uiControl(aobjs->objects[i].z));
            uiControlEnable(uiControl(aobjs->objects[i].gain));
        }
    }
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

unsigned int 
pmd_studio_audio_objects_get_eids(
    dlb_pmd_element_id **eid_list,
    pmd_studio *studio,
    bool enabled_only
    )
{
    // Refresh list
    unsigned int i;
    pmd_studio_audio_objects *aobjs = pmd_studio_get_objects(studio);

    for (i = 0 ; i < aobjs->object_count ; i++)
    {
        aobjs->object_eids[i] = aobjs->objects[i].object.id;
    }
    *eid_list = aobjs->object_eids;
    return(aobjs->object_count);
}


void
pmd_studio_audio_objects_print_debug(
    pmd_studio *studio)
{
    pmd_studio_audio_objects *aobjs = pmd_studio_get_objects(studio);
    unsigned int i;

    printf("---****Objects****----\n");
    printf("Number of Objects: %u\n", aobjs->object_count);

    for (i = 0 ; i < aobjs->object_count ; i++)
    {
        printf("Object #%u:\n", i);
        if (aobjs->objects[i].enabled)
        {
            printf("\tObject is enabled\n");
        }
        else
        {
            printf("\tObject is disabled\n" );
        }
        printf("\tID: %u", aobjs->objects[i].object.id);
        printf("\tType: ");
        switch (aobjs->objects[i].object.object_class)
        {
        case PMD_CLASS_DIALOG:
            printf("Dialog\n");
            break;    
        case PMD_CLASS_VDS:
            printf("VDS\n");
            break;    
        case PMD_CLASS_VOICEOVER:
            printf("Voiceover\n");
            break;    
        case PMD_CLASS_GENERIC:
            printf("Generic\n");
            break;    
        default:
            printf("Unknown\n");
        }
        printf("\tPosition- X: %f Y: %f Z: %f\n", aobjs->objects[i].object.x, aobjs->objects[i].object.y, aobjs->objects[i].object.z);
        printf("\tGain: %f\n", aobjs->objects[i].object.source_gain);
        printf("\tName: %s\n", aobjs->objects[i].object.name);
    }
    printf("\n");
}

void
pmd_studio_audio_objects_finish
    (pmd_studio_audio_objects *aobj
    )
{
    free(aobj);
}


dlb_pmd_success 
pmd_studio_set_obj_gain
    (pmd_studio *studio
    ,dlb_pmd_element_id eid
    , float gain_db
    )
{
    pmd_studio_audio_objects *objs = pmd_studio_get_objects(studio);
    pmd_studio_audio_object *obj;
    for(unsigned int i=0; i<objs->object_count; i++)
    {
        obj = &objs->objects[i];
        if(obj->object.id == eid)
        {
            int combo_index =  pmd_studio_gaindb_to_combobox_index(gain_db);
            uiComboboxSetSelected(obj->gain, combo_index);
            onObjectGainUpdated(obj->gain, obj);
            return PMD_SUCCESS;
        }
    }
    // If got this far, couldn't find matching eid in available objects.
    return PMD_FAIL;
}
