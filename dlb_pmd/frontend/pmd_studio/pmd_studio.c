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
#include <locale.h>
#include "ui.h"
#include "pmd_studio.h"
#include "dlb_pmd_klv.h"
#include "xml.h"
#include "pmd_tool_klv.h"
#include "pcm.h"

#include "pmd_studio_file_menu.h"
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_objects.h"
#include "pmd_studio_audio_presentations.h"
#include "pmd_studio_audio_outputs.h"
#include "pmd_studio_device.h"

#define PMD_STUDIO_VERSION "0.3(madi-dev)"

/*
 * @brief type of pmd_studio abstraction
 */
struct pmd_studio
{
    char title[DLB_PMD_MAX_NAME_LENGTH];
    model pmd;
    uiWindow 						*window;
    uiEntry 						*title_entry;
    pmd_studio_file_menu 			file_menu;
    pmd_studio_audio_beds 			*audio_beds;
    pmd_studio_audio_objects 		*audio_objects;
    pmd_studio_audio_presentations 	*audio_presentations;
    pmd_studio_outputs  			*outputs;
    pmd_studio_device 				*device;
};

const unsigned int pmd_studio_speaker_config_num_channels[NUM_PMD_SPEAKER_CONFIGS] = 
{
    2,       /**< L, R                                               */
    3,       /**< L, R, C                                            */
    6,       /**< L, R, C, Lfe, Ls, Rs                               */
    8,       /**< L, R, C, Lfe, Ls, Rs, Ltm, Rtm                     */
    10,      /**< L, R, C, Lfe, Ls, Rs, Ltf, Rtf, Ltr, Rtr           */
    12,      /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Ltf, Rtf, Ltr, Rtr */
    16,      /**< L, R, C, Lfe, Ls, Rs, Lrs, Rrs, Lfw, Rfw,
             *   Ltf, Rtf, Ltm, Rtm, Ltr, Rtr */
    2,       /**< L, R, portable speakers   */
    2,       /**< L, R, portable headphones */
};


const char *pmd_studio_speaker_config_strings[NUM_PMD_SPEAKER_CONFIGS] = 
{
    "2.0",
    "3.0",
    "5.1",
    "5.1.2",
    "5.1.4",
    "7.1.4",
    "9.1.6",
    "Portable",
    "Headphone"
};

const char* pmd_studio_error_messages[PMD_STUDIO_NUM_ERROR_MESSAGES] =
{
    "OK",
    "Port Audio Error",
    "Assertion",
    "Memory",
    "User Interface Error"
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
} mode;

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

static
void
print_usage
    (void
    )
{
    fprintf(stderr, "%s\n", get_version_string("pmd_studio [OPTION]... "));
    fprintf(stderr, "Copyright Dolby Laboratories Inc., 2020. All rights reserved.\n\n");
    fprintf(stderr, "-fb                  File-based mode (overrides streaming options)\n");
    fprintf(stderr, "-di <index>          Device index to use for input\n");
    fprintf(stderr, "-do <index>          Device index to use for output\n");
    fprintf(stderr, "-dl                  List input and output devices\n");
    fprintf(stderr, "-c <channels>        Number of input and output channels\n");
    fprintf(stderr, "-f <filename>        Filename of file to load on launch\n");
    fprintf(stderr, "-l <latency>         Input and output latency in seconds\n");
    fprintf(stderr, "-buf <samples>       Buffer size in samples\n");
    fprintf(stderr, "-am824               Select am824 framing for metadata output. For use with ALSA AES67 driver\n");
    fprintf(stderr, "-h                   This message\n");
    fprintf(stderr, "--help               This message\n");
}

static
dlb_pmd_success
pmd_studio_init
    (pmd_studio *s,
     int argc,
     char **argv)
{
    uiInitOptions options;
    const char *err;
    uiBox *toplevelbox, *titlebox, *rowbox, *column1box, *column2box;
    int i;
    int input_device = paNoDevice;
    int output_device = paNoDevice;
    unsigned int num_channels = MAX_CHANNELS;
    float latency = 0.0;
    unsigned int buffer_size = DEFAULT_FRAMES_PER_BUFFER;
    dlb_pmd_bool file_based_mode = PMD_FALSE;
    dlb_pmd_bool am824_mode = PMD_FALSE;
    char *file_to_load = NULL;

    /* initialize the system random number generator */
    srand((unsigned int)time(NULL));

    /*
     * assign values to obj and pres count in their respective studio fields based on the initial values
     * found in limits
     */

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-fb"))
        {
            file_based_mode = PMD_TRUE;
        }
        else if (!strcmp(argv[i], "-di"))
        {
            if (input_device != paNoDevice)
            {
                fprintf(stderr, "Error: Selected input device multiple times\n");
                print_usage();
                exit(-8);
            }
            if (i == (argc - 1))
            {
                fprintf(stderr, "Error: Can't find soundcard index\n");
                print_usage();
                exit(-1);
            }
            input_device = atoi(argv[i + 1]);
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }
        else if (!strcmp(argv[i], "-do"))
        {
            if (output_device != paNoDevice)
            {
                fprintf(stderr, "Error: Selected output device multiple times\n");
                print_usage();
                exit(-9);
            }
            if (i == (argc - 1))
            {
                fprintf(stderr, "Error: Can't find soundcard index\n");
                print_usage();
                exit(-1);
            }
            output_device = atoi(argv[i + 1]);
            // We increment i here to step over the next parameter
            // which has been parsed as the value
            i++;
        }
        else if (!strcmp(argv[i], "-dl"))
        {
            Pa_Initialize();
            pmd_studio_device_list();
            Pa_Terminate();
            exit(0);
        }
        else if (!strcmp(argv[i], "-l"))
        {
            latency = (float)atof(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-am824"))
        {
            am824_mode = PMD_TRUE;
        }
        else if (!strcmp(argv[i], "-buf"))
        {
            buffer_size = atoi(argv[i + 1]);
            if (buffer_size > MAX_FRAMES_PER_BUFFER)
            {
                fprintf(stderr, "Error: Buffer size %u exceeds maximum allowed size of channels: %u\n", buffer_size, MAX_FRAMES_PER_BUFFER);
                print_usage();
                exit(-1);                
            }            
            if (buffer_size < MIN_FRAMES_PER_BUFFER)
            {
                fprintf(stderr, "Error: Buffer size %u exceeds maximum allowed size of channels: %u\n", buffer_size, MIN_FRAMES_PER_BUFFER);
                print_usage();
                exit(-1);                
            }
            i++;        
        }
        else if (!strcmp(argv[i], "-f"))
        {
            file_to_load = argv[i + 1];
            if ((strlen(file_to_load) == 0) || strlen(file_to_load) > DLB_PMD_MAX_NAME_LENGTH)
            {
                fprintf(stderr, "Error: Invalid file name");
                print_usage();
                exit(-1);
            }
            i++;
        }
        else if (!strcmp(argv[i], "-c"))
        {
            num_channels = atoi(argv[i + 1]);
            if (num_channels > MAX_CHANNELS)
            {
                fprintf(stderr, "Error: Number of specified channels %u exceeds maximum number of channels: %u\n", num_channels, MAX_CHANNELS);
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

    s->window = uiNewWindow(get_version_string("Professional Metadata Studio "), 640, 480, 1);

    uiWindowOnClosing(s->window, onClosing, s);
    uiOnShouldQuit(onShouldQuit, s);

    toplevelbox = uiNewVerticalBox();
    rowbox = uiNewHorizontalBox();
    column1box = uiNewVerticalBox();
    column2box = uiNewVerticalBox();
    uiBoxSetPadded(toplevelbox, 1);
    uiBoxSetPadded(rowbox, 1);
    //uiBoxSetPadded(column1box, 1);
    //uiBoxSetPadded(column2box, 1);
    uiWindowSetChild(s->window, uiControl(toplevelbox));
    uiWindowSetMargined(s->window, 1);

    titlebox = uiNewHorizontalBox();
    uiBoxAppend(titlebox, uiControl(uiNewLabel("Title ")), 0);
    s->title_entry = uiNewEntry();

    snprintf(s->title, sizeof(s->title), "<untitled>");
    uiEntrySetText(s->title_entry, s->title);
    uiEntryOnChanged(s->title_entry, onTitleChanged, s);
    uiBoxAppend(toplevelbox, uiControl(titlebox), 0);
    uiBoxAppend(toplevelbox, uiControl(rowbox), 0);
    uiBoxAppend(rowbox, uiControl(column1box), 0);
    uiBoxAppend(rowbox, uiControl(uiNewVerticalSeparator()), 0);
    uiBoxAppend(rowbox, uiControl(column2box), 0);   
    uiBoxAppend(titlebox, uiControl(s->title_entry), 1);

    if (pmd_studio_audio_beds_init(&s->audio_beds, s->window, column1box, s)
        || pmd_studio_audio_objects_init(&s->audio_objects, s->window, column2box, s)
        || pmd_studio_audio_presentations_init(&s->audio_presentations, s->window, column1box, s))
    {
        return(PMD_FAIL);
    }
    else
    {
        /* Try to bring up the audio subsystem, if this fails then revert to file only */
        if (!file_based_mode && !pmd_studio_device_init(&s->device, input_device, output_device, num_channels, latency, buffer_size, am824_mode))
        {
            pmd_studio_outputs_init(&s->outputs, s->window, column2box, s);
        }
        else
        {
            // Explicitly signal that outputs are disabled
            s->outputs = NULL;
            s->device = NULL;
        }

        if (file_to_load != NULL)
        {
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
    pmd_studio_audio_beds_finish(s->audio_beds);
    pmd_studio_audio_objects_finish(s->audio_objects);
    pmd_studio_audio_presentations_finish(s->audio_presentations);
    pmd_studio_outputs_finish(s->outputs);
    pmd_studio_device_close(s->device);
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
    dlb_pmd_model *m = s->pmd.model;
    const char *title;

    if (!dlb_pmd_title(m, &title))
    {
        snprintf(s->title, sizeof(s->title), "%s", title);
    }

    pmd_studio_audio_beds_refresh_ui(s->audio_beds);
    pmd_studio_audio_objects_refresh_ui(s->audio_objects);
    pmd_studio_audio_presentations_refresh_ui(s->audio_presentations);
    pmd_studio_outputs_refresh_ui(s->outputs);
}

/* Public functions */

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

void
pmd_studio_reset
    (pmd_studio *s
    )
{
    dlb_pmd_reset(s->pmd.model);
    snprintf(s->title, sizeof(s->title), "<untitled>");
    uiEntrySetText(s->title_entry, s->title);
    
    pmd_studio_audio_beds_reset(s->audio_beds);
    pmd_studio_audio_objects_reset(s->audio_objects);
    pmd_studio_audio_presentations_reset(s->audio_presentations);
    refresh_ui(s);
}


void
pmd_studio_import
    (pmd_studio *s
    )
{   
    dlb_pmd_model *m = s->pmd.model;
    const char *title;
    
    pmd_studio_audio_beds_reset(s->audio_beds);
    pmd_studio_audio_objects_reset(s->audio_objects);
    pmd_studio_audio_presentations_reset(s->audio_presentations);

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

void
pmd_studio_update_model
    (pmd_studio *s
    )
{
    string_uuid random_uuid;
    dlb_pmd_model *m = s->pmd.model;

    memset(&random_uuid, 0, sizeof(random_uuid));
    generate_random_uuid(random_uuid);
   
    if (   dlb_pmd_reset(m)
        || dlb_pmd_add_signals(m, MAX_AUDIO_SIGNALS)
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
    }
}

dlb_pmd_model
*pmd_studio_get_model
    (pmd_studio *studio
    )
{
    return(studio->pmd.model);
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

/* Application Main */

static const char *LOCALE_SPECIFIER = "en-US";

int
main
    (int argc,
     char **argv)
{
    char *locale = setlocale(LC_ALL, LOCALE_SPECIFIER); /* Prevent use of ',' for numeric decimal points */;
    const struct lconv *lc = localeconv();
    pmd_studio s;

    if (locale && strcmp(locale, LOCALE_SPECIFIER))
    {
        printf("WARNING: attempt to set locale to %s failed!\n", locale);
    }

    if (lc && lc->decimal_point[0] != '.')
    {
        printf("ERROR: decimal point character is set to '%c' instead of '.'!\n", lc->decimal_point[0]);
        return 1;
    }

    if (!pmd_studio_init(&s, argc, argv))
    {
        pmd_studio_run(&s);
    }

    pmd_studio_finish(&s);
    return 0;
}

