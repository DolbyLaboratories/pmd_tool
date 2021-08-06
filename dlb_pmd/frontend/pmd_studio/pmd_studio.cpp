/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
extern "C"{
#include "ui.h"
#include "dlb_pmd_klv.h"
#include "pmd_tool_klv.h"
#include "xml.h"
#include "pcm.h"
#include "pmd_model.h"
#include "dlb_pmd_model_combo.h"
}
#include "pmd_studio.h"
#include "pmd_studio_pvt.h"
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_audio_presentations.h"
#include "pmd_studio_audio_outputs.h"
#include "pmd_studio_device.h"
#include "pmd_studio_settings.h"
#include "pmd_studio_file_menu.h"
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_settings_pvt.h"
#include "pmd_studio_audio_outputs.h"

pmd_studio_error_code c;

#define PMD_STUDIO_VERSION "1.8.0"


const char* pmd_studio_error_messages[PMD_STUDIO_NUM_ERROR_MESSAGES] =
{
    "OK",
    "Port Audio Error",
    "Assertion",
    "Memory",
    "User Interface Error",
    "File Error",
    "Augmentor Error"
};

/* Program version string (use the function, not the global variable!) */

#define VERSION_STRING_SIZE (128)
static char version_string[VERSION_STRING_SIZE];

static const char *get_version_string(const char *prefix)
{
    unsigned int epoch;
    unsigned int major;
    unsigned int minor;
    unsigned int build;
    unsigned int bs_maj;
    unsigned int bs_min;

    dlb_pmd_library_version(&epoch, &major, &minor, &build, &bs_maj, &bs_min);

    if (prefix == NULL)
    {
        prefix = "";
    }

    snprintf(version_string, VERSION_STRING_SIZE, "%sv%s PMD: %u.%u.%u.%u-%u.%u",
        prefix, PMD_STUDIO_VERSION, epoch, major, minor, build, bs_maj, bs_min);

    return version_string;
}


/* Prototypes */

static
void
print_usage
    (void
    );




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
} file_mode;

/**
 * @brief helper function to determine filename type
 *
 * We use the filename suffixes to determine what kind of operation
 * we want to achieve.  For instance, if the input filename is a.xml
 * and the output b.klv, we want to translate metadata in XML format
 * into the serialized KLV format.
 */

/* Private functions */

static
file_mode                          /** @return file's mode */
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


#ifdef todo
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
#endif


static
int
onClosing
    (uiWindow *w
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio *)data;
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
    pmd_studio *s = (pmd_studio *)data;
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

static
void
print_usage
    (void
    )
{
    fprintf(stderr, "%s\n", get_version_string("pmd_studio [OPTION]... "));
    fprintf(stderr, "Copyright Dolby Laboratories Inc., 2021. All rights reserved.\n\n");
    fprintf(stderr, "-fb                  File-based mode (overrides streaming options)\n");
    fprintf(stderr, "-f <filename>        Filename of file to load on launch\n");
    fprintf(stderr, "-l <latency>         Input and output latency in seconds\n");
    fprintf(stderr, "-device <option>     Device specific option\n");
    fprintf(stderr, "-buf <samples>       Buffer size in samples\n");
    fprintf(stderr, "-h                   This message\n");
    fprintf(stderr, "--help               This message\n");
}


static
void
eid_init(pmd_studio *studio
    )
{
    unsigned int i;
    for (i = 0 ; i < MAX_EIDS ; i++)
    {
        studio->eids[i] = 0;
    }
    studio->num_eids = 0;
}

static
void
config_init(
    pmd_studio *s,
    dlb_pmd_bool file_based_mode
    )
{
    const unsigned int num_configs = 7;

    static const dlb_pmd_speaker_config configs[num_configs] =
    {
        DLB_PMD_SPEAKER_CONFIG_2_0,       /**< L, R                                               */
        DLB_PMD_SPEAKER_CONFIG_5_1,       /**< L, R, C, Lfe, Ls, Rs                               */
        DLB_PMD_SPEAKER_CONFIG_5_1_4,     /**< L, R, C, Lfe, Ls, Rs, Ltf, Rtf, Ltr, Rtr           */
        DLB_PMD_SPEAKER_CONFIG_3_0,       /**< L, R, C                                            */
        DLB_PMD_SPEAKER_CONFIG_5_1_2,     /**< L, R, C, Lfe, Ls, Rs, Ltm, Rtm                     */
        DLB_PMD_SPEAKER_CONFIG_7_1_4,     /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Ltf, Rtf, Ltr, Rtr */
        DLB_PMD_SPEAKER_CONFIG_9_1_6,     /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Lfw, Rfw,
                                           *                         Ltf, Rtf, Ltm, Rtm, Ltr, Rtr */
    };

    static const char *config_strings[num_configs] = 
    {
        "2.0",
        "5.1",
        "5.1.4",
        "3.0",
        "5.1.2",
        "7.1.4",
        "9.1.6",
    };

    static const unsigned int config_num_channels[num_configs] = 
    {
        2,       /**< L, R                                               */
        6,       /**< L, R, C, Lfe, Ls, Rs                               */
        10,      /**< L, R, C, Lfe, Ls, Rs, Ltf, Rtf, Ltr, Rtr           */
        3,       /**< L, R, C                                            */
        8,       /**< L, R, C, Lfe, Ls, Rs, Ltm, Rtm                     */
        12,      /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Ltf, Rtf, Ltr, Rtr */
        16,      /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Lfw, Rfw,
                 *   Ltf, Rtf, Ltm, Rtm, Ltr, Rtr */
    };

    static const dlb_pmd_speaker speaker_config_channels[num_configs][16] =
    {
        /* 2.0 */    { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL },
        /* 5.1 */    { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_C, PMD_SPEAKER_LFE, PMD_SPEAKER_LS, PMD_SPEAKER_RS, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL },
        /* 5.1.4 */  { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_C, PMD_SPEAKER_LFE, PMD_SPEAKER_LS, PMD_SPEAKER_RS, PMD_SPEAKER_LTF, PMD_SPEAKER_RTF, PMD_SPEAKER_LTR, PMD_SPEAKER_RTR, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL },
        /* 3.0 */    { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_C, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL },
        /* 5.1.2 */  { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_C, PMD_SPEAKER_LFE, PMD_SPEAKER_LS, PMD_SPEAKER_RS, PMD_SPEAKER_LTM, PMD_SPEAKER_RTM, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL },
        /* 7.1.4 */  { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_C, PMD_SPEAKER_LFE, PMD_SPEAKER_LS, PMD_SPEAKER_RS, PMD_SPEAKER_LRS, PMD_SPEAKER_RRS, PMD_SPEAKER_LTF, PMD_SPEAKER_RTF, PMD_SPEAKER_LTR, PMD_SPEAKER_RTR, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL, PMD_SPEAKER_NULL },
        /* 9.1.6 */  { PMD_SPEAKER_L, PMD_SPEAKER_R, PMD_SPEAKER_C, PMD_SPEAKER_LFE, PMD_SPEAKER_LS, PMD_SPEAKER_RS, PMD_SPEAKER_LRS, PMD_SPEAKER_RRS, PMD_SPEAKER_LTF, PMD_SPEAKER_RTF, PMD_SPEAKER_LTM, PMD_SPEAKER_RTM, PMD_SPEAKER_LTR, PMD_SPEAKER_RTR, PMD_SPEAKER_LFW, PMD_SPEAKER_RFW },
    };
  
    unsigned int i;

    if (file_based_mode)
    {
        s->num_configs = num_configs;
    }
    else
    {
        s->num_configs = 3;
    }

    for (i = 0 ; i < s->num_configs ; i++)
    {
        s->configs[i].config = configs[i];
        s->configs[i].config_string = config_strings[i];
        s->configs[i].num_channels = config_num_channels[i];
        s->configs[i].config_channels = &speaker_config_channels[i];
    }
}

/* Private functions */


static
dlb_pmd_success
pmd_studio_init
    (pmd_studio *s,
     int argc,
     char **argv)
{
    uiInitOptions options;
    const char *err;
    uiBox *titlebox, *hbox1, *hbox2;
    int i;
    char *file_to_load = NULL;
    dlb_pmd_success status;

    // First init UI so we can have dialog box errors
    memset(&options, 0, sizeof (uiInitOptions));
    err = uiInit(&options);
    if (err)
    {
        fprintf(stderr, "error initializing libui: %s", err);
        uiFreeInitError(err);
        return PMD_FAIL;
    }

    // Lock down menu edits immediately as menus are finalized on mac once a window is created

    if (pmd_studio_file_menu_init(&s->file_menu, s))
    {
        return PMD_FAIL;
    }

    if (pmd_studio_console_menu_init(&s->console_menu, s))
    {
        return PMD_FAIL;
    }


    /* initialize the system random number generator */
    srand((unsigned int)time(NULL));
    pmd_studio_settings_init(&s->settings);
    s->console = nullptr;

    /* Process command line options */
    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-fb"))
        {
            s->settings->file_based_mode = PMD_TRUE;
        }
        else if (!strcmp(argv[i], "-l"))
        {
            s->settings->common_device_settings.latency = (float)atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-device"))
        {
        	if (i < (argc - 1))
        	{
        		pmd_studio_device_option(s, argv[i+1]);
        	}
            i++;
        }
        else if (!strcmp(argv[i], "-buf"))
        {
            s->settings->common_device_settings.frames_per_buffer = atoi(argv[i + 1]);
            if (s->settings->common_device_settings.frames_per_buffer > MAX_FRAMES_PER_BUFFER)
            {
                fprintf(stderr, "Error: Buffer size %u exceeds maximum allowed size of Frames: %u\n", s->settings->common_device_settings.frames_per_buffer, MAX_FRAMES_PER_BUFFER);
                print_usage();
                exit(-1);                
            }            
            if (s->settings->common_device_settings.frames_per_buffer < MIN_FRAMES_PER_BUFFER)
            {
                fprintf(stderr, "Error: Buffer size %u less than minimum allowed size of Frames: %u\n", s->settings->common_device_settings.frames_per_buffer, MIN_FRAMES_PER_BUFFER);
                print_usage();
                exit(-1);                
            }
            i++;        
        }
        else if (!strcmp(argv[i], "-f"))
        {
            file_to_load = argv[i + 1];
            if ((strlen(file_to_load) == 0) || strlen(file_to_load) > PMD_STUDIO_MAX_FILENAME_LENGTH) 
            {
                fprintf(stderr, "Error: Invalid file name: \"%s\"", file_to_load);
                if(strlen(file_to_load) == 0){
                    fprintf(stderr, " - Empty string\n");
                }
                else{
                    fprintf(stderr, " - Too long\n");
                }
                print_usage();
                exit(-1);
            }
            i++;
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i],"--help"))
        {
            print_usage();
            exit(0);
        }
        else
        {
            fprintf(stderr, "Error: Unknown option\n");
            print_usage();
            exit(-1);
        }
    }

    if (model_init(&s->pmd))
    {
        return PMD_FAIL;
    }
#if LATER
    s->pmd.model->error_callback = pmd_studio_on_augmentor_fail_cb;
#endif

    if (s->settings->file_based_mode)
    {
        s->mode = PMD_STUDIO_MODE_FILE_EDIT;
    }
    else
    {
        s->mode = PMD_STUDIO_MODE_EDIT;        
    }

    s->window = uiNewWindow(get_version_string("Professional Metadata Studio "), 640, 380, 1);
    uiWindowOnClosing(s->window, onClosing, s);
    uiOnShouldQuit(onShouldQuit, s);


    s->toplevelbox = uiNewVerticalBox();
    hbox1 = uiNewHorizontalBox();
    hbox2 = uiNewHorizontalBox();
    uiBoxSetPadded(s->toplevelbox, 1);
    uiBoxSetPadded(hbox1, 1);
    uiBoxSetPadded(hbox2, 1);
    uiWindowSetChild(s->window, uiControl(s->toplevelbox));
    uiWindowSetMargined(s->window, 1);

    titlebox = uiNewHorizontalBox();
    uiBoxAppend(titlebox, uiControl(uiNewLabel("Title ")), 0);
    s->title_entry = uiNewEntry();

    snprintf(s->title, sizeof(s->title), "<untitled>");
    uiEntrySetText(s->title_entry, s->title);
    uiEntryOnChanged(s->title_entry, onTitleChanged, s);
    uiBoxAppend(titlebox, uiControl(s->title_entry), 1);
    uiBoxAppend(s->toplevelbox, uiControl(titlebox), 0);
    uiBoxAppend(s->toplevelbox, uiControl(hbox1), 0);
    uiBoxAppend(s->toplevelbox, uiControl(uiNewHorizontalSeparator()), 0);
    uiBoxAppend(s->toplevelbox, uiControl(uiNewHorizontalSeparator()), 0);
    uiBoxAppend(s->toplevelbox, uiControl(hbox2), 0);
    if (pmd_studio_get_mode(s) != PMD_STUDIO_MODE_FILE_EDIT)
    {
        s->connection_section = new PMDStudioUIFooterHandler(s);
    }
    eid_init(s);
    config_init(s, s->settings->file_based_mode);

    status = pmd_studio_audio_beds_init(&s->audio_beds, s->window, hbox1, s);
    uiBoxAppend(hbox1, uiControl(uiNewVerticalSeparator()), 0);
    status = status || pmd_studio_audio_objects_init(&s->audio_objects, s->window, hbox1, s);
    status = status || pmd_studio_audio_presentations_init(&s->audio_presentations, s->window, hbox2, s);
 

    if (status)
    {
        return(PMD_FAIL);
    }
    else
    {
        /* This is to signal that we are initializing the device for the first time */
        s->device = NULL;
        /* Try to bring up the audio subsystem, if this fails then revert to file only */
        pmd_studio_device_init(&s->device, &s->settings->common_device_settings, &s->settings->device_settings, s->window, s);
        if (!s->settings->file_based_mode)
        {
            uiBoxAppend(hbox2, uiControl(uiNewVerticalSeparator()), 0);
            pmd_studio_outputs_init(&s->outputs, s->window, hbox2, s);
        }
        else
        {
            // Explicitly signal that outputs are disabled
            s->outputs = NULL;
        }

        if (file_to_load != NULL)
        {
            pmd_studio_reset(s);
            pmd_studio_open_file(s, file_to_load);
        }

        return(PMD_SUCCESS);
    }
}

static
void
pmd_studio_run
    (pmd_studio *s
    )
{
    uiControlShow(uiControl(s->window));
    uiMain();
}

static
void
pmd_studio_finish
    (pmd_studio *s
    )
{
    delete s->connection_section;
    pmd_studio_audio_beds_finish(s->audio_beds);
    pmd_studio_audio_objects_finish(s->audio_objects);
    pmd_studio_audio_presentations_finish(s->audio_presentations);
    pmd_studio_outputs_finish(s->outputs);
    pmd_studio_device_close(s->device);
    pmd_studio_settings_close(s->settings);
    console_disconnect(s);
}


/*
 * refresh_ui is called upon making a new studio (via the UI menu) or importing an xml.
*/
static
void
refresh_ui
    (pmd_studio *s
    )
{
#if LATER
    dlb_pmd_model *m = s->pmd.model;
    const char *title;

    if (!dlb_pmd_title(m, &title))
    {
        snprintf(s->title, sizeof(s->title), "%s", title);
    }
#endif

    pmd_studio_audio_beds_refresh_ui(s->audio_beds);
    pmd_studio_audio_objects_refresh_ui(s->audio_objects);
    pmd_studio_audio_presentations_refresh_ui(s->audio_presentations);
    pmd_studio_outputs_refresh_ui(s->outputs);
}

static
void
generate_random_bytes
    (unsigned char  *bytes
    ,size_t          count
    )
{
    size_t i;

    for (i = 0; i < count; i++)
    {
        int r_int = rand() % 256;
        bytes[i] = (unsigned char)(r_int & 0xff);
    }
}


typedef unsigned char numeric_uuid[16];
typedef char string_uuid[37];

static
void
generate_random_uuid
    (string_uuid    random_uuid
    )
{
    numeric_uuid uuid;
    char *p = random_uuid;
    size_t i, j, u;

    generate_random_bytes(uuid, sizeof(uuid));
    uuid[6] = (uuid[6] & 0x0f) | 0x40;
    uuid[8] = (uuid[8] & 0x3f) | 0x80;

    for (u = 0, i = 0; i < 4; i++)
    {
        sprintf(p, "%02hhx", uuid[u++]);
        p += 2;
    }
    sprintf(p++, "-");

    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < 2; i++)
        {
            sprintf(p, "%02hhx", uuid[u++]);
            p += 2;
        }
        sprintf(p++, "-");
    }

    for (i = 0; i < 6; i++)
    {
        sprintf(p, "%02hhx", uuid[u++]);
        p += 2;
    }
}

/*** Public Functions ***/

pmd_studio_audio_presentations *pmd_studio_get_presentations(pmd_studio *studio)
{
    return(studio->audio_presentations);
}

pmd_studio_audio_beds *pmd_studio_get_beds(pmd_studio *studio)
{
    return(studio->audio_beds);
}

pmd_studio_audio_objects *pmd_studio_get_objects(pmd_studio *studio)
{
    return(studio->audio_objects);
}

pmd_studio_outputs *pmd_studio_get_outputs(pmd_studio *studio)
{
    return(studio->outputs);
}

pmd_studio_device *pmd_studio_get_device(pmd_studio *studio)
{
    return(studio->device);
}

pmd_studio_settings *pmd_studio_get_settings(pmd_studio *studio)
{
    return(studio->settings);
}

uiWindow
*pmd_studio_get_window
    (pmd_studio *studio
    )
{
    return(studio->window);
}

void
pmd_studio_reset
    (pmd_studio *s
    )
{
    console_disconnect(s);

    (void)dlb_pmd_model_combo_clear(s->pmd.model);
    snprintf(s->title, sizeof(s->title), "<untitled>");
    uiEntrySetText(s->title_entry, s->title);

   eid_init(s);
    
    if (s->outputs != nullptr)
    {
        pmd_studio_outputs_reset(s->outputs);
    }
    pmd_studio_audio_beds_reset(s->audio_beds);
    pmd_studio_audio_objects_reset(s->audio_objects);
    pmd_studio_audio_presentations_reset(s->audio_presentations);
    pmd_studio_device_reset(s->device);
    refresh_ui(s);
}

void
pmd_studio_enable
    (pmd_studio *s
    )
{
    pmd_studio_switch_mode(s, PMD_STUDIO_MODE_EDIT);
}


void
pmd_studio_disable (pmd_studio *s)
{
    pmd_studio_switch_mode(s, PMD_STUDIO_MODE_LIVE);
}

void
pmd_studio_import
    (pmd_studio *s
    )
{   
    dlb_pmd_model *m;
    const char *title;

    (void)dlb_pmd_model_combo_get_writable_pmd_model(s->pmd.model, &m, PMD_FALSE);
    
    pmd_studio_audio_beds_reset(s->audio_beds);
    pmd_studio_audio_objects_reset(s->audio_objects);
    pmd_studio_audio_presentations_reset(s->audio_presentations);
    eid_init(s);

    if (   pmd_studio_audio_beds_import(s->audio_beds, m)
        || pmd_studio_audio_objects_import(s->audio_objects, m)
        || pmd_studio_audio_presentations_import(s->audio_presentations, m))
    {
        uiMsgBoxError(s->window, "error importing model", dlb_pmd_error(m));
        dlb_pmd_reset(m);
    }
    refresh_ui(s);
    /* The code below was moved from the top of the function because its position there was ineffective on linux */
    /* It is unkown why exactly but it is thought to be to do with timing and the closing of the */
    /* file opening dialogue box */
    if (dlb_pmd_title(m, &title))
    {
        uiEntrySetText(s->title_entry, "<unknown>");
    }
    else
    {
        snprintf(s->title, sizeof(s->title), "%s", title);
        uiEntrySetText(s->title_entry, s->title);
    }
}

void pmd_studio_information(const char message[])
{
    uiWindow *win;

    win = uiNewWindow("PMD Studio Error", 400, 410, 0);
    if (win)
    {
        uiMsgBoxError(win, "PMD Studio Information", message);
        uiControlDestroy(uiControl(win));
    }
    else
    {
        fprintf(stderr, "PMD Studio Information\n");            
        fprintf(stderr, "%s\n", message);
        fprintf(stderr, "Press Enter to continue\n");            
        getchar();
    }
}


void pmd_studio_error(pmd_studio_error_code err, const char error_message[])
{
    uiWindow *win;

    if (err == PMD_STUDIO_ERR_UI)
    {
        win = uiNewWindow("PMD Studio Error", 400, 410, 0);
        if (win)
        {
            uiMsgBoxError(win, "PMD Studio Error", error_message);
            uiControlDestroy(uiControl(win));
        }
        else
        {
            fprintf(stderr, "PMD Studio Error\n");            
            fprintf(stderr, "%s\n", error_message);
            fprintf(stderr, "Press Enter to continue\n");            
            getchar();
        }
        return;
    }

#ifndef NDEBUG
    if (err < PMD_STUDIO_NUM_ERROR_MESSAGES)
    {
        fprintf(stderr, "%d,%s,%s\n", err, error_message, pmd_studio_error_messages[err]);
    }
    else
    {
        fprintf(stderr, "%d,%s\n", err, error_message);
    }
    if (err == PMD_STUDIO_ERR_ASSERT)
    {
        exit(err);
    }
#endif
}


void
pmd_studio_update_model(pmd_studio *s)
{
    string_uuid random_uuid;
    dlb_pmd_model *m;

    memset(&random_uuid, 0, sizeof(random_uuid));
    generate_random_uuid(random_uuid);
   
    if (   dlb_pmd_model_combo_clear(s->pmd.model)
        || dlb_pmd_model_combo_get_writable_pmd_model(s->pmd.model, &m, PMD_FALSE)
        || dlb_pmd_add_signals(m, MAX_STUDIO_AUDIO_SIGNALS)
        || dlb_pmd_set_title(m, s->title)
        || dlb_pmd_iat_add(m, (uint64_t)0u)
        || dlb_pmd_iat_content_id_uuid(m, random_uuid))
    {
        uiMsgBoxError(s->window, "error updating model", dlb_pmd_error(m));
        return;
    }

    pmd_studio_audio_beds_update_model(s->audio_beds, m);
    pmd_studio_audio_objects_update_model(s->audio_objects, m);
    pmd_studio_audio_presentations_update_model(s->audio_presentations, m);

    // If we have an audio subsystem up and running then update the mix matrix
    if (s->outputs)
    {
        pmd_studio_device_update_mix_matrix(s);
        pmd_studio_audio_outputs_update_metadata_outputs(s);
    }
}

dlb_pmd_model_combo
*pmd_studio_get_model
    (pmd_studio *studio
    )
{
    return(studio->pmd.model);
}

unsigned int
get_pmd_studio_configs
    (pmd_studio *studio,
     pmd_studio_config **configs
    )
{
    *configs = studio->configs;
    return(studio->num_configs);
}

pmd_studio_config *get_pmd_studio_config_info
    (pmd_studio *studio,
    dlb_pmd_speaker_config config
    )
{
    unsigned int i;

    for (i = 0 ; i < studio->num_configs ; i++)
    {
        if (studio->configs[i].config == config)
        {
            return(&studio->configs[i]);
        }
    }
    pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Can't find specified speaker config in list");
    return(nullptr);
}

dlb_pmd_success
pmd_studio_eid_get_next
    (pmd_studio *studio,
    dlb_pmd_element_id *eid_out
    )
{
    unsigned int j;
    dlb_pmd_bool found;
   
    // EIDS start at 1, 0 is invalid or reserved
    for(*eid_out = 1; *eid_out < (MAX_EIDS + 1); *eid_out = *eid_out + 1)
    {
        found = PMD_FALSE;
        for (j = 0 ; j < studio->num_eids ; j++)
        {
            if (studio->eids[j] == *eid_out)
            {
                found = PMD_TRUE;
                break;
            }
        }
        /* Success is if the eid is not found i.e. it is free */
        if (found == PMD_FALSE)
        {
            break;
        }
    }

    // If found is True, then we're out of eids.
    if (found == PMD_FALSE)
    {
        studio->eids[studio->num_eids++] = *eid_out;
        return(PMD_SUCCESS);
    }
    else
    {
        *eid_out = 0;
        return(PMD_FAIL);
    }
}

dlb_pmd_success
pmd_studio_eid_replace
    (pmd_studio *studio
    ,dlb_pmd_element_id old_eid
    ,dlb_pmd_element_id new_eid
    )
{

    int existing_old_index = -1;
    for(unsigned int i = 0; i<studio->num_eids; i++)
    {
        if(studio->eids[i] == old_eid)
        {
            existing_old_index = i;
        }
        else if(studio->eids[i] == new_eid)
        {
            // New EID already exists, cannot update.
            return PMD_FAIL;
        }
    }
    if(existing_old_index == -1)
    {
        // No record of old eid exists. Cannot update.
        return PMD_FAIL;
    }

    studio->eids[existing_old_index] = new_eid;
    pmd_studio_audio_presentations_handle_element_eid_update(studio, old_eid, new_eid);
    return PMD_SUCCESS;
}

void
pmd_studio_open_file
    (pmd_studio *s
    ,const char *filename
    )
{
    dlb_pmd_model_combo *combo_model = s->pmd.model;
    file_mode m;
    
    m = read_file_mode(filename);
    (void)dlb_pmd_model_combo_clear(combo_model);

    switch (m)
    {
        case MODE_XML:
            if (xml_read(filename, combo_model, PMD_FALSE, PMD_FALSE))
            {
                uiMsgBoxError(s->window, "open model", "xml_read() failed");
                return;
            }
            break;
        case MODE_KLV:
            if (klv_read(filename, combo_model))
            {
                uiMsgBoxError(s->window, "open model", "klv_read() failed");
                return;
            }
            break;
#ifdef todo
        case MODE_WAV:
            if (pcm_read(filename, rate, chan, ispair, vsync, 0, combo_model))
            {
                uiMsgBoxError(s->window, "open model", dlb_pmd_error(model));                
                return;
            }
            break;
#endif
        default:
            uiMsgBoxError(s->window, "open model", "Unsupported file extension");
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
    dlb_pmd_model_combo *combo_model = s->pmd.model;
    file_mode m;
    
    if (filename)
    {
        m = read_file_mode(filename);
        switch (m)
        {
        case MODE_XML:
            if (xml_write(filename, combo_model, sadm))
            {
                uiMsgBoxError(s->window, "save model", "xml_write() failed");
            }
            break;
        case MODE_KLV:
            if (klv_write(filename, combo_model, ul))
            {
                uiMsgBoxError(s->window, "save model", "klv_write() failed");
            }
            break;
#ifdef todo
        case MODE_WAV:
        {
            char pcm_out[256];
            generate_output_filename(filename, 0, pcm_out, sizeof(pcm_out));
            if (pcm_write(filename, pcm_out, rate, chan, ispair, ul, mark, sadm, combo_model))
            {
                uiMsgBoxError(s->window, "save model", dlb_pmd_error(model));
            }
            break;
        }        
#endif
        default:
            uiMsgBoxError(s->window, "save model", "Unsupported file extension");
            break;
        }
    }
}

dlb_pmd_bool
pmd_studio_settings_update
    (pmd_studio *s,
     uiWindow *win
    )
{
    pmd_studio_audio_presentations_update_nlang(s->audio_presentations);
    pmd_studio_audio_presentations_refresh_ui(s->audio_presentations);

    if (!s->settings->file_based_mode)
    {
        return(pmd_studio_device_init(&s->device, &s->settings->common_device_settings, &s->settings->device_settings, win, s));
    }
    else
    {
        return(PMD_SUCCESS);
    }
}

void
pmd_studio_debug
    (pmd_studio *studio
    )
{
    unsigned int i;

    printf("----------********** PMD Studio Debug **********----------\n");

    printf("EID List: ");
    for (i = 0 ; i < studio->num_eids ; i++)
    {
        printf("%u ", studio->eids[i]);
    }
    printf("\n");

    pmd_studio_audio_beds_print_debug(studio);
    pmd_studio_audio_objects_print_debug(studio);
    pmd_studio_audio_presentations_print_debug(studio);
    pmd_studio_outputs_print_debug(studio);
}

static
void 
_on_connection_section_btn_clicked_wrapper
    (uiButton* b
    ,void* data)
{
    pmd_studio *studio = (pmd_studio *) data;
    studio->connection_section->onButtonPressed(b, data);
}

PMDStudioUIFooterHandler::PMDStudioUIFooterHandler
    (pmd_studio *studio
    )
{
    mLabel = uiNewLabel("");
    mButton = uiNewButton("");
    mCallback = nullptr;
    uiControlHide(uiControl(mButton));
    uiButtonOnClicked(mButton, _on_connection_section_btn_clicked_wrapper, studio);
    uiBoxAppend(studio->toplevelbox, uiControl(mLabel), 0);
    uiBoxAppend(studio->toplevelbox, uiControl(mButton), 0);
}

void 
PMDStudioUIFooterHandler::clear()
{
    uiLabelSetText(mLabel, "");
    if(uiControlVisible(uiControl(mButton)) == 1) uiControlHide(uiControl(mButton));
    mCallback = nullptr;
}

void
PMDStudioUIFooterHandler::set
    (const char* title
    )
{

    if(uiControlVisible(uiControl(mButton)) == 1) uiControlHide(uiControl(mButton));
    uiLabelSetText(mLabel, title);
    mCallback = nullptr;
}

void
PMDStudioUIFooterHandler::set
    (const char* btn_label
    ,void(*callback)(uiButton*, void*)
    )
{
    uiLabelSetText(mLabel, "");
    uiButtonSetText(mButton, btn_label);

    if(uiControlVisible(uiControl(mButton)) == 1) uiControlShow(uiControl(mButton));
    mCallback = callback;
}

void
PMDStudioUIFooterHandler::set
    (const char *title
    ,const char *btn_label
    ,void(*callback)(uiButton*, void*)
    )
{
    uiLabelSetText(mLabel, title);
    uiButtonSetText(mButton, btn_label);
    if(uiControlVisible(uiControl(mButton)) == 0) uiControlShow(uiControl(mButton));
    mCallback = callback;
}

void
PMDStudioUIFooterHandler::onButtonPressed
    (uiButton *button
    ,void *data)
{
    if(mCallback != nullptr)
    {
        mCallback(button, data);
    }
}


int 
pmd_studio_gaindb_to_combobox_index
    (float gain
    )
{
    int index = int((6-gain)*2);
    if(index < 0)
    {
        index = 0;
    }
    else if (index > 63)
    {
        index = 63;
    }
    return index;
}

float pmd_studio_combobox_index_to_gaindb
    ( int index
    )
{
    return (index == 63) ? -INFINITY:(6-float(index)/2);
}



static
void
onEditModeButtonClicked
    (uiButton *button
    , void* data
    )
{
    pmd_studio *studio = (pmd_studio*) data;
    // Disabling metadata output should trigger edit mode.
    pmd_studio_outputs_stop_all_metadata_outputs(studio);

    // PMD Studio should have gone into edit mode. If not, force
    if(studio->mode != PMD_STUDIO_MODE_EDIT)
    {
        pmd_studio_switch_mode(studio, PMD_STUDIO_MODE_EDIT); 
    }
}

enum pmd_studio_mode
pmd_studio_get_mode
    (pmd_studio *studio
    )
{
    return(studio->mode);
}

void 
pmd_studio_switch_mode
    (pmd_studio *studio
    ,pmd_studio_mode newmode)
{
    switch (newmode)
    {
    default:
        // Defaults to EDIT, but first print error.
        fprintf(stderr, "Unrecognized pmd_studio mode\n");
    case PMD_STUDIO_MODE_EDIT:
        // Enable all UI components
        pmd_studio_audio_beds_enable(studio->audio_beds);
        pmd_studio_audio_objects_enable(studio->audio_objects);
        pmd_studio_audio_presentations_enable(studio->audio_presentations);
        pmd_studio_outputs_enable(studio->outputs);
        studio->mode = PMD_STUDIO_MODE_EDIT;
        studio->connection_section->clear();
        break;
    
    case PMD_STUDIO_MODE_LIVE:
        // Selectively disable UI components (leave things like )
        pmd_studio_audio_beds_disable(studio->audio_beds, true);
        pmd_studio_audio_objects_disable(studio->audio_objects, true);
        pmd_studio_audio_presentations_disable(studio->audio_presentations, true);
        pmd_studio_outputs_disable(studio->outputs, true);
        studio->mode = PMD_STUDIO_MODE_LIVE;
        studio->connection_section->set("PMDStudio is in live mode.", "Edit", onEditModeButtonClicked);
        break;
    
    case PMD_STUDIO_MODE_CONSOLE_LIVE:
        // Disable all UI components.
        pmd_studio_audio_beds_disable(studio->audio_beds);
        pmd_studio_audio_objects_disable(studio->audio_objects);
        pmd_studio_audio_presentations_disable(studio->audio_presentations);
        pmd_studio_outputs_disable(studio->outputs);
        studio->mode = PMD_STUDIO_MODE_CONSOLE_LIVE;
        break;
    }
}

/* Application Main */

int
main
    (int argc,
     char **argv)
{
    pmd_studio s;

    if (!pmd_studio_init(&s, argc, argv))
    {
        pmd_studio_run(&s);
    }
    pmd_studio_finish(&s);

    return 0;
}

