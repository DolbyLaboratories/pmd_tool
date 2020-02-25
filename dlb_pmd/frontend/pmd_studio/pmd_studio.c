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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "pmd_studio.h"
#include "dlb_pmd_klv.h"
#include "xml.h"
#include "klv.h"
#include "pcm.h"

#include "pmd_studio_file_menu.h"
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_audio_presentations.h"


#define PMD_STUDIO_VERSION "0.2(alpha)"


/*
 * @brief type of pmd_studio abstraction
 */
struct pmd_studio
{
    char title[DLB_PMD_MAX_NAME_LENGTH];
    model pmd;
    uiWindow *window;
    uiEntry *title_entry;
    pmd_studio_file_menu file_menu;
    pmd_studio_audio_beds audio_beds;
    pmd_studio_audio_objects audio_objects;
    pmd_studio_audio_presentations audio_presentations;

    int pmd_studio_audio_object_count;
    int pmd_studio_audio_presentation_count;
    int pmd_studio_audio_bed_count;

    /* 
     * separate counts which tell how many elements are present in
     * xml that is being imported
     */
    int pmd_studio_audio_object_count_import;
    int pmd_studio_audio_presentation_count_import;
    //int pmd_studio_audio_bed_count_import;


};


/* access the presentation checkboxes for dynamic appearance to the studio on front end */
static 
uiCheckbox * 
getCheckBox
    (pmd_studio *s
    ,int i
    ,int j
    )
{
    return s->audio_presentations.presentations[i].checkBoxes[j];
}

/*
counter for current objects initialized which can be accessed through the studio
this is better than just an arbitrary global variable
*/
static 
unsigned int 
getCurrentObjectCount
    (pmd_studio *s
    )
{
    return s->pmd_studio_audio_object_count;
}

static void 
setCurrentObjectCount
    (pmd_studio *s
    ,int current
    )
{
    s->pmd_studio_audio_object_count = current;
}

/* getter and setter only initialized and used if importing an xml file */
static 
unsigned int 
getImportObjectCount
    (pmd_studio *s
    )
{
    return s->pmd_studio_audio_object_count_import;
}

static void 
setImportObjectCount
    (pmd_studio *s
    ,int current
    )
{
    s->pmd_studio_audio_object_count_import = current;
}


/* ability to update the object count when a new object is added via the add object button */
static
void
increaseCurrentObjectCount
    (pmd_studio *s
    )
{
    int current;
    current = s->pmd_studio_audio_object_count;
    s->pmd_studio_audio_object_count = current+1;
}

/* counter for current presentations, same idea as with objects */
static
unsigned int 
getCurrentPresentationCount
    (pmd_studio *s
    )
{
    return s->pmd_studio_audio_presentation_count;
}

static 
void 
setCurrentPresentationCount
    (pmd_studio *s
    ,int current
    )
{
    s->pmd_studio_audio_presentation_count = current;
}

static
int 
getImportPresentationCount
    (pmd_studio *s
    )
{
    return s->pmd_studio_audio_presentation_count_import;
}

static 
void 
setImportPresentationCount
    (pmd_studio *s
    ,int current
    )
{
    s->pmd_studio_audio_presentation_count_import = current;
}


static 
void 
increaseCurrentPresentationCount
    (pmd_studio *s
    )
{
    int current;
    current = s->pmd_studio_audio_presentation_count;
    s->pmd_studio_audio_presentation_count = current+1;
}



/**
 * @brief list of input/output modes
 *
 * We use input/output modes, (mostly based on file suffix types) to
 * guide the type of processing we'd like done.
 */
typedef enum
{
    MODE_NONE = 0,
    MODE_UNKNOWN,
    MODE_XML,
    MODE_KLV,
    MODE_WAV,
} mode;

/**
 * @brief helper function to determine filename type
 *
 * We use the filename suffixes to determine what kind of operation
 * we want to achieve.  For instance, if the input filename is a.xml
 * and the output b.klv, we want to translate metadata in XML format
 * into the serialized KLV format.
 */
static
mode                          /** @return file's mode */
read_file_mode
    (const char *filename     /**< [in] filename */
    )
{
    size_t len = strlen(filename);
    const char *ptr = filename + len - 1;

    while (*ptr != '\\' && *ptr != '/' && *ptr != '.' && ptr != filename)
    {
        --ptr;
    }
    
    if (*ptr != '.')
    {
        return MODE_NONE;
    }

    ++ptr;
    if (0 == strncmp(ptr, "xml", 4))
    {
        return MODE_XML;
    }
    if (0 == strncmp(ptr, "klv", 4))
    {
        return MODE_KLV;
    }
    if (0 == strncmp(ptr, "wav", 4))
    {
        return MODE_WAV;
    }
    
    return MODE_UNKNOWN;
}


/**
 * @brief helper function to generate pcm+pmd output .wav file name
 */
static inline
void
generate_output_filename
    (const char  *filename        /**< [in] input .wav filename */
    ,dlb_pmd_bool sadm            /**< [in] is it a sadm .wav file? */
    ,      char  *outfile         /**< [out] array to store generated filename */
    ,      size_t outfile_size    /**< [in] size of #outfile array */
    )
{
    size_t len = strlen(filename);
    const char *ptr = filename + len - 1;

    if (len > outfile_size)
    {
        printf("input PCM filename too long\n");
        abort();
    }

    while (*ptr != '\\' && *ptr != '/' && *ptr != '.' && ptr != filename)
    {
        --ptr;
    }
    
    if (*ptr != '.')
    {
        sprintf(outfile, "%s_%s.wav", filename, sadm ? "sadm" : "klv");
    }
    else
    {
        strncpy(outfile, filename, ptr - filename);
        outfile += (ptr - filename);
        sprintf(outfile, "_%s.wav", sadm ? "sadm" : "klv");
    }
}


static
int
onClosing
    (uiWindow *w
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;
    (void)w;
    uiQuit();
    model_finish(&s->pmd);
    return 1;
}


static
int
onShouldQuit
    (void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;
    uiControlDestroy(uiControl(s->window));
    return 1;
}



static
void
onTitleChanged
    (uiEntry *e
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio *)data;
    snprintf(s->title, sizeof(s->title), "%s", uiEntryText(e));
    pmd_studio_update_model(s);
}

static inline
dlb_pmd_success
pmd_studio_init
    (pmd_studio *s
    )
{
    uiInitOptions options;
    const char *err;
    uiBox *vbox, *hbox;

    /*
     * assign values to obj and pres count in their respective studio fields based on the initial values
     * found in limits
     */
    s->pmd_studio_audio_bed_count = INIT_AUDIO_BEDS;
    s->pmd_studio_audio_object_count = INIT_AUDIO_OBJECTS;
    s->pmd_studio_audio_presentation_count = INIT_AUDIO_PRESENTATIONS;
    s->pmd_studio_audio_presentation_count_import = -1;
    s->pmd_studio_audio_object_count_import = -1;

    if (model_init(&s->pmd))
    {
        return PMD_FAIL;
    }

    memset(&options, 0, sizeof (uiInitOptions));
    err = uiInit(&options);
    if (err)
    {
        fprintf(stderr, "error initializing libui: %s", err);
        uiFreeInitError(err);
        return PMD_FAIL;
    }

    if (pmd_studio_file_menu_init(&s->file_menu, s))
    {
        return PMD_FAIL;
    }

    s->window = uiNewWindow("Professional Metadata Studio v" PMD_STUDIO_VERSION, 640, 480, 1);

    uiWindowOnClosing(s->window, onClosing, s);
    uiOnShouldQuit(onShouldQuit, s);

    vbox = uiNewVerticalBox();
    uiWindowSetChild(s->window, uiControl(vbox));
    uiWindowSetMargined(s->window, 1);

    hbox = uiNewHorizontalBox();
    uiBoxAppend(hbox, uiControl(uiNewLabel("Title ")), 0);
    s->title_entry = uiNewEntry();

    snprintf(s->title, sizeof(s->title), "<untitled>");
    uiEntrySetText(s->title_entry, s->title);
    uiEntryOnChanged(s->title_entry, onTitleChanged, s);
    uiBoxAppend(hbox, uiControl(s->title_entry), 1);
    uiBoxAppend(vbox, uiControl(hbox), 0);    

    return pmd_studio_audio_beds_init(&s->audio_beds, s->window, vbox, s)
        || pmd_studio_audio_objects_init(&s->audio_objects, s->window, vbox, s)
        || pmd_studio_audio_presentations_init(&s->audio_presentations, s->window, vbox, s)
        ;
}


static inline
void
pmd_studio_run
    (pmd_studio *s
    )
{
    uiControlShow(uiControl(s->window));
    uiMain();
}


static inline
void
pmd_studio_finish
    (pmd_studio *s
    )
{
    /* todo */
    (void)s;
}


/*
 * refresh_ui is called upon making a new studio (via the UI menu) or importing an xml.
*/
static inline
void
refresh_ui
    (pmd_studio *s
    )
{
    dlb_pmd_model *m = s->pmd.model;
    const char *title;

    if (!dlb_pmd_title(m, &title))
    {
        snprintf(s->title, sizeof(s->title), "%s", title);
    }

    pmd_studio_audio_beds_refresh_ui(&s->audio_beds);
    pmd_studio_audio_objects_refresh_ui(&s->audio_objects);
    pmd_studio_audio_presentations_refresh_ui(&s->audio_presentations);
}


void
pmd_studio_reset
    (pmd_studio *s
    )
{
    dlb_pmd_reset(s->pmd.model);
    snprintf(s->title, sizeof(s->title), "<untitled>");
    uiEntrySetText(s->title_entry, s->title);
    
    pmd_studio_audio_beds_reset(&s->audio_beds);
    pmd_studio_audio_objects_reset(&s->audio_objects);
    pmd_studio_audio_presentations_reset(&s->audio_presentations);
    refresh_ui(s);
}


void
pmd_studio_import
    (pmd_studio *s
    )
{   
    int presentation_count_import, object_count_import;
    dlb_pmd_model *m = s->pmd.model;
    const char *title;

    if (dlb_pmd_title(m, &title))
    {
        uiEntrySetText(s->title_entry, "<unknown>");
    }
    else
    {
        snprintf(s->title, sizeof(s->title), "%s", title);
        uiEntrySetText(s->title_entry, s->title);
    }
    
    pmd_studio_audio_beds_reset(&s->audio_beds);
    pmd_studio_audio_objects_reset(&s->audio_objects);
    pmd_studio_audio_presentations_reset(&s->audio_presentations);

    if (   pmd_studio_audio_beds_import(&s->audio_beds, m)
        || pmd_studio_audio_objects_import(&s->audio_objects, m)
        || pmd_studio_audio_presentations_import(&s->audio_presentations, m))
    {
        uiMsgBoxError(s->window, "error importing model", dlb_pmd_error(m));
        dlb_pmd_reset(m);
    }
    presentation_count_import = dlb_pmd_num_presentations(m);
    object_count_import =dlb_pmd_num_objects(m);
    setImportObjectCount(s, object_count_import);
    setImportPresentationCount(s, presentation_count_import);
    refresh_ui(s);
}


void
pmd_studio_update_model
    (pmd_studio *s
    )
{
    pmd_studio_audio_bed *abed;
    pmd_studio_audio_object *aobj;
    pmd_studio_audio_presentation *apres;
    dlb_pmd_model *m = s->pmd.model;
    int i;
   
    if (   dlb_pmd_reset(m)
        || dlb_pmd_add_signals(s->pmd.model, MAX_AUDIO_SIGNALS)
        || dlb_pmd_set_title(m, s->title)
        || dlb_pmd_iat_add(s->pmd.model, (uint64_t)0u))
    {
        uiMsgBoxError(s->window, "error updating model", dlb_pmd_error(m));
        return;
    }
        
    abed = s->audio_beds.beds;
    for (i = 0; i != MAX_AUDIO_BEDS; ++i, ++abed)
    {
        if (abed->enabled)
        {
            if (dlb_pmd_set_bed(m, &abed->bed))
            {
                uiMsgBoxError(s->window, "error setting bed", dlb_pmd_error(m));
            }
        }
    }

    aobj = s->audio_objects.objects;
    /*
    CHANGED from MAX_AUDIO_OBJECTS to obtain count from studio field
    */
    for (i = 0; i != s->pmd_studio_audio_object_count; ++i, ++aobj)
    {
        if (aobj->enabled)
        {
            if (dlb_pmd_set_object(m, &aobj->object))
            {
                uiMsgBoxError(s->window, "error setting object", dlb_pmd_error(m));
            }
        }
    }
    /* 
    Same as above but for presentations
    */
    apres = s->audio_presentations.presentations;
    for (i = 0; i != s->pmd_studio_audio_presentation_count ; ++i, ++apres)
    {
        if (apres->enabled)
        {
            if (dlb_pmd_set_presentation(m, &apres->presentation))
            {
                uiMsgBoxError(s->window, "error setting presentation", dlb_pmd_error(m));
            }
        }
    }
}


void
pmd_studio_open_file
    (pmd_studio *s
    ,const char *filename
    )
{
    dlb_pmd_model *model = s->pmd.model;
    mode m;
    m = read_file_mode(filename);
    dlb_pmd_reset(model);
    switch (m)
    {
        case MODE_XML:
            if (xml_read(filename, model, 0))
            {
                uiMsgBoxError(s->window, "open model", dlb_pmd_error(model));
                return;
            }
            break;
        case MODE_KLV:
            if (klv_read(filename, model))
            {
                uiMsgBoxError(s->window, "open model", dlb_pmd_error(model));
                return;
            }
            break;
#ifdef todo
        case MODE_WAV:
            if (pcm_read(filename, rate, chan, ispair, vsync, 0, model))
            {
                uiMsgBoxError(s->window, "open model", dlb_pmd_error(model));                
                return;
            }
            break;
#endif
        default:
            uiMsgBoxError(s->window, "open model", "Unknown file extension");
            return;
            break;
    }
    pmd_studio_import(s);
}


void
pmd_studio_save_file
    (pmd_studio *s
    ,const char *filename
    ,dlb_pmd_bool sadm
    )
{
    dlb_klvpmd_universal_label ul = DLB_PMD_KLV_UL_DOLBY;
    dlb_pmd_model *model = s->pmd.model;
    mode m;
    
    if (filename)
    {
        m = read_file_mode(filename);
        switch (m)
        {
        case MODE_XML:
            if (xml_write(filename, model, sadm))
            {
                uiMsgBoxError(s->window, "save model", dlb_pmd_error(model));
            }
            break;
        case MODE_KLV:
            if (klv_write(filename, model, ul))
            {
                uiMsgBoxError(s->window, "save model", dlb_pmd_error(model));
            }
            break;
#ifdef todo
        case MODE_WAV:
        {
            char pcm_out[256];
            generate_output_filename(filename, 0, pcm_out, sizeof(pcm_out));
            if (pcm_write(filename, pcm_out, rate, chan, ispair, ul, mark, sadm, model))
            {
                uiMsgBoxError(s->window, "save model", dlb_pmd_error(model));
            }
            break;
        }        
#endif
        default:
            uiMsgBoxError(s->window, "save model", "Unknown file extension");
            break;
        }
    }
}


int
main
    (void
    )
{
    pmd_studio s;
    if (!pmd_studio_init(&s))
    {
        pmd_studio_run(&s);
    }

    pmd_studio_finish(&s);
    return 0;
}

