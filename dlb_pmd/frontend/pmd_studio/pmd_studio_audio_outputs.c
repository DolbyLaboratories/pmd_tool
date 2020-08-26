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
#include "pmd_studio_device.h"
#include "dlb_pmd_pcm.h"

/* Definitons */

typedef enum
{
    PMD_OUTPUT_MODE,
    SADM_OUTPUT_MODE,
    NUM_METADATA_FORMATS
} pmd_studio_metadata_format;

#define PMD_STUDIO_OUTPUT_NUM_CONFIG 3

static const dlb_pmd_speaker_config pmd_studio_output_config[PMD_STUDIO_OUTPUT_NUM_CONFIG] =
{
    DLB_PMD_SPEAKER_CONFIG_2_0,
    DLB_PMD_SPEAKER_CONFIG_5_1,
    DLB_PMD_SPEAKER_CONFIG_5_1_4
};

static const char pmd_studio_metadata_format_names[][MAX_LABEL_LENGTH] = 
{
    "PMD",
    "sADM"
};

// Required so can distinguish between Metadata and audio outputs in common
// allocation table
#define METADATA_OUTPUT_ID_OFFSET (100) 
#define METADATA_FRAME_RATE (25)
#define METADATA_DLB_PMD_FRAME_RATE (DLB_PMD_FRAMERATE_2500)
#define METADATA_SAMPLES_PER_FRAME (48000 / METADATA_FRAME_RATE)
#define METADATA_MAX_CHANNELS (2)
#define METADATA_MAX_BYTES_PER_SAMPLE (4)
#define MAX_PCM_BUF_SIZE (METADATA_SAMPLES_PER_FRAME * METADATA_MAX_CHANNELS * METADATA_MAX_BYTES_PER_SAMPLE)

typedef struct
{
    pmd_studio_outputs *outputs;
    dlb_pmd_bool enabled;
    unsigned int id;
    float mixcoefs[MAX_OUTPUT_CHANNELS];
    dlb_pmd_speaker_config config;
    unsigned int presentation_id; //  Currently selected ids
    unsigned int startchannel; // Starts at 1, 0 is unassigned
    unsigned int endchannel; // Starts at 1, 0 is unassigned
    uiCheckbox *enable;
    uiCombobox *cfg;
    uiCombobox *pres;
    uiCombobox *schan;
    uiLabel *echan;
} pmd_studio_audio_output;


typedef struct
{
    pmd_studio_outputs *outputs;
    dlb_pmd_bool enabled;
    unsigned int id;
    pmd_studio_metadata_format format;
    dlb_pmd_bool subframemode;
    unsigned int channel; // Starts at 1, 0 is unassigned
    uiCheckbox *enable;
    uiCombobox *fmt;
    uiCombobox *mode;
    uiCombobox *chan;
    uint32_t pcmbuf[MAX_PCM_BUF_SIZE];
    unsigned int pcmbufsize; // number of valid bytes in pcmbuf
    unsigned int ring_buffer_handle;
} pmd_studio_metadata_output;

struct pmd_studio_outputs
{
    pmd_studio *studio;
    uiWindow *window;
    pmd_studio_audio_output audio_outputs[MAX_AUDIO_OUTPUTS];
    pmd_studio_metadata_output metadata_outputs[MAX_METADATA_OUTPUTS];

    /* add in a count field to the struct for how many of the allocated outputs are intialized */
    unsigned int audio_output_count;
    unsigned int metadata_output_count;
    uiGrid *audio_output_grid;
    uiGrid *metadata_output_grid;

    unsigned int presentation_ids[MAX_AUDIO_PRESENTATIONS];
    unsigned int num_presentations;

    /* channel based elements */
    unsigned int num_chan_allocated;
    unsigned int chan_allocated[MAX_OUTPUT_CHANNELS]; /* 0 for unallocated, use output/metadata id to indicate allocation */
};

static
dlb_pmd_success allocateAudioOutput(
    pmd_studio_audio_output *aout
    );
static
dlb_pmd_success deallocateAudioOutput(
    pmd_studio_audio_output *aout
    );

static
dlb_pmd_success deallocateMetadataOutput(
    pmd_studio_metadata_output *mdout
    );


static
dlb_pmd_success
add_metadata_output
    (uiGrid *grid
    ,pmd_studio_metadata_output *mdout
    ,unsigned int id
    ,pmd_studio_outputs *outs
    );

static
dlb_pmd_success
add_audio_output
    (uiGrid *grid
    ,pmd_studio_audio_output *aout
    ,unsigned int id
    ,pmd_studio_outputs *outs
    );

static
void
recalculateAudioOutputChannelLabels(
    pmd_studio *studio
    );

static
dlb_pmd_success
allocateMetadataOutput(
    pmd_studio_metadata_output *mdout
    );

static
void
recreateChannelAllocationTable(
    pmd_studio *studio
    );

static
dlb_pmd_success
pmd_studio_outputs_start_metadata_output
    (pmd_studio_metadata_output *mdout
    );

static
dlb_pmd_success
pmd_studio_outputs_stop_metadata_output
    (pmd_studio_metadata_output *mdout
    );


/* Callbacks */

static
void
onEnableOutput
    (uiCheckbox *en
    ,void *data
    )
{
    int channel_count;
    unsigned int alloc_idx = 0;
    pmd_studio_audio_output *aout = (pmd_studio_audio_output *)data;
    unsigned int pres_enabled;

    /* First check that presentation is enabled */
    if (pmd_studio_audio_presentation_enabled_by_id(aout->outputs->studio, aout->presentation_id, &pres_enabled) == PMD_SUCCESS)
    {
        if (!pres_enabled)
        {
            uiMsgBoxError(aout->outputs->window, "error enabling output", "presentation not enabled");
            uiCheckboxSetChecked(en, 0);
            return;
        }
    }
    else
    {
        uiMsgBoxError(aout->outputs->window, "error enabling output", "error trying to determine if presentation is enabled");
        uiCheckboxSetChecked(en, 0);
        return;
    }

    channel_count = pmd_studio_speaker_config_num_channels[aout->config];
    /* This has to be done here as matrix calculations required enabled output */
    aout->enabled = (dlb_pmd_bool)uiCheckboxChecked(en); 

    /* Allocate/Deallocate channels */
    if (aout->enabled)
    {
        /* Ensure that we have enough channels to allocate */
        if ((channel_count + aout->outputs->num_chan_allocated) > MAX_OUTPUT_CHANNELS)
        {
            uiMsgBoxError(aout->outputs->window, "error allocating channels", "insufficient channels available");
            uiCheckboxSetChecked(en, 0);
            aout->enabled = 0;
            return;
        }
        /* reset checkbox on failure */
        if (allocateAudioOutput(aout) == PMD_SUCCESS)
        {
            recalculateAudioOutputChannelLabels(aout->outputs->studio);
            if (pmd_studio_device_update_mix_matrix(aout->outputs->studio))
            {
                uiMsgBoxError(aout->outputs->window, "error disabling audio output", "invalid output configuration");
                uiCheckboxSetChecked(en, 0);
                aout->enabled = 0;
                deallocateAudioOutput(aout);
                recalculateAudioOutputChannelLabels(aout->outputs->studio);
                return;
            }
        }
        else
        {
            /* Failed so disable enablement */
            uiMsgBoxError(aout->outputs->window, "error allocating channels", "insufficient channels available");
            uiCheckboxSetChecked(en, 0);
            aout->enabled = 0;
            return;
        }
    }
    else
    {
        deallocateAudioOutput(aout);
        recalculateAudioOutputChannelLabels(aout->outputs->studio);
        pmd_studio_device_update_mix_matrix(aout->outputs->studio);
    }

    /* Should never hit this but just in case */
    if (alloc_idx == MAX_OUTPUT_CHANNELS)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Channel allocation index out of bounds");
    }

    /* Success to change state to follow UI */

#ifndef NDEBUG
    pmd_studio_outputs_print_debug(aout->outputs->studio);
#endif
}

static
void
onEnableMetadataOutput
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_metadata_output *mdout = (pmd_studio_metadata_output *)data;
    mdout->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    if (mdout->enabled)
    {
        if (allocateMetadataOutput(mdout) || pmd_studio_outputs_start_metadata_output(mdout))
        {
            uiCheckboxSetChecked(en, 0);
            mdout->enabled = 0;
        }
    }
    else
    {
        deallocateMetadataOutput(mdout);
        pmd_studio_outputs_stop_metadata_output(mdout);
    }
}


static
void
onOutputConfigUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_output *aout = (pmd_studio_audio_output *)data;
    aout->config = pmd_studio_output_config[uiComboboxSelected(c)];
    recalculateAudioOutputChannelLabels(aout->outputs->studio);
    if (aout->enabled)
    {
        pmd_studio_device_update_mix_matrix(aout->outputs->studio);
    }

}


static
void
onOutputPresentationUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_output *aout = (pmd_studio_audio_output *)data;
    unsigned int index = uiComboboxSelected(c);

    if (index < pmd_studio_audio_presentations_get_count(aout->outputs->studio))
    {
        aout->presentation_id = pmd_studio_audio_presentation_get_id(aout->outputs->studio, index);
    }
    if (aout->enabled)
    {
        pmd_studio_device_update_mix_matrix(aout->outputs->studio);
    }
}

static
void
onOutputStartChannelUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_output *aout = (pmd_studio_audio_output *)data;
    unsigned int index = uiComboboxSelected(c);

    aout->startchannel = index;
    if (aout->enabled)
    {
        // This is an enabled output so this could cause the entire table to be recalculated
        recreateChannelAllocationTable(aout->outputs->studio);
        recalculateAudioOutputChannelLabels(aout->outputs->studio);
        pmd_studio_device_update_mix_matrix(aout->outputs->studio);
    }
}


static
void
onMetadataFormatUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_metadata_output *mdout = (pmd_studio_metadata_output *)data;
    mdout->format = (pmd_studio_metadata_format)uiComboboxSelected(c);
    // If output is running, update ring buffer
    if (mdout->enabled)
    {
        pmd_studio_outputs_stop_metadata_output(mdout);
        // re-enable with new format/ Disable output on failure
        if (pmd_studio_outputs_start_metadata_output(mdout))
        {
            uiCheckboxSetChecked(mdout->enable, 0);
            mdout->enabled = 0;
        }
    }   
}

static
void
onMetadataModeUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_metadata_output *mdout = (pmd_studio_metadata_output *)data;
    mdout->subframemode = (dlb_pmd_bool)uiComboboxSelected(c);
    if (mdout->enabled)
    {
        allocateMetadataOutput(mdout);
        pmd_studio_outputs_stop_metadata_output(mdout);
        // re-enable with new format/ Disable output on failure
        if (pmd_studio_outputs_start_metadata_output(mdout))
        {
            uiCheckboxSetChecked(mdout->enable, 0);
            mdout->enabled = 0;
        }
    }  
}

static
void
onMetadataChannelUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_metadata_output *mdout = (pmd_studio_metadata_output *)data;
    mdout->channel = (int)uiComboboxSelected(c);
    if (mdout->enabled)
    {
        pmd_studio_outputs_stop_metadata_output(mdout);
        if (allocateMetadataOutput(mdout) || pmd_studio_outputs_start_metadata_output(mdout))
        {
            // allocation failed so unenable the output
            mdout->enabled = 0;
            uiCheckboxSetChecked(mdout->enable, 0);
        }
    }
}

static 
void 
onAddAudioOutputButtonClicked
    (uiButton *button
    ,void *data)
{
    pmd_studio_outputs *outs = (pmd_studio_outputs *)data;
    (void)button;

    if (outs->audio_output_count >= MAX_AUDIO_OUTPUTS)
    {
        uiMsgBoxError(outs->window, "error setting output", "max outputs reached");
    }
    else
    {
        memset(&outs->audio_outputs[outs->audio_output_count], '\0', sizeof(outs->audio_outputs[outs->audio_output_count]));
        add_audio_output(outs->audio_output_grid, &outs->audio_outputs[outs->audio_output_count], outs->audio_output_count+1, outs);
    }

}

static 
void 
onAddMetadataOutputButtonClicked
    (uiButton *button
    ,void *data)
{
    pmd_studio_outputs *outs = (pmd_studio_outputs *)data;
    (void)button;

    if (outs->metadata_output_count == MAX_METADATA_OUTPUTS)
    {
        uiMsgBoxError(outs->window, "error setting output", "max outputs reached");
    }
    else
    {
        memset(&outs->metadata_outputs[outs->metadata_output_count], '\0', sizeof(outs->metadata_outputs[outs->metadata_output_count]));
        add_metadata_output(outs->metadata_output_grid, &outs->metadata_outputs[outs->metadata_output_count], outs->metadata_output_count+1, outs);
    }
}


/* Private Functions */

static
void
pmd_studio_audio_output_init
    (pmd_studio_audio_output *aout
    )
{
    unsigned int i;

    aout->enabled = 0;
    aout->config = DLB_PMD_SPEAKER_CONFIG_2_0;
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        aout->mixcoefs[i] = 0.0;
    }
    aout->startchannel = 0;
    aout->endchannel = 0;
}

static
void
pmd_studio_metadata_output_init
    (pmd_studio_metadata_output *mdout
    )
{
    mdout->enabled = 0;
    mdout->format = PMD_OUTPUT_MODE;
    mdout->subframemode = PMD_TRUE;
    mdout->channel = 0;
}


static
dlb_pmd_success allocateAudioOutput(
    pmd_studio_audio_output *aout
    )
{
    unsigned int channel_count = pmd_studio_speaker_config_num_channels[aout->config];
    unsigned int alloc_idx = 0;
    char tmp[10];

    /* Ensure that we have enough channels to allocate */
    if ((channel_count + aout->outputs->num_chan_allocated) > MAX_OUTPUT_CHANNELS)
    {
        uiMsgBoxError(aout->outputs->window, "error allocating channels", "insufficient channels available");
        return(PMD_FAIL);
    }

    /* Allocate channels */
    /* start at user specified start channel */
    alloc_idx = aout->startchannel;
    /* loop over channels to allocate */
    while((channel_count > 0) && (alloc_idx < MAX_OUTPUT_CHANNELS))
    {
        /* chec to see if we've hit a metadata output which takes precedence */
        if (aout->outputs->chan_allocated[alloc_idx] < METADATA_OUTPUT_ID_OFFSET)
        {
            if (aout->outputs->chan_allocated[alloc_idx])
            {
                // Detected a channel clash
                break;
            }
            else
            {
                if (channel_count == 1)
                {
                    aout->endchannel = alloc_idx;
                    sprintf(tmp, "%d", alloc_idx + 1);
                    uiLabelSetText(aout->echan, tmp);
                }
          
                aout->outputs->chan_allocated[alloc_idx] = aout->id;
                channel_count--;
                aout->outputs->num_chan_allocated++;
            }
        }
        alloc_idx++;
    }
    if (channel_count > 0)
    {
        deallocateAudioOutput(aout);
        uiMsgBoxError(aout->outputs->window, "Error allocating channels", "Selection clashes with existing output");
        return PMD_FAIL;
    }
    return(PMD_SUCCESS);
}

static
void
recreateChannelAllocationTable(
    pmd_studio *studio
    )
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);

    unsigned int alloc_idx = 0;
    unsigned int i; 

    // First Wipe out allocation table
    for (alloc_idx = 0 ; (alloc_idx < MAX_OUTPUT_CHANNELS) ; alloc_idx++)
    {
        outs->chan_allocated[alloc_idx] = 0;
    }
    outs->num_chan_allocated--;

    // Next allocate metadata outpus
    for (i = 0 ; i < outs->metadata_output_count ; i++)
    {
        outs->chan_allocated[outs->metadata_outputs[i].channel] = outs->metadata_outputs[i].id + METADATA_OUTPUT_ID_OFFSET;
    }
    outs->num_chan_allocated = outs->metadata_output_count;

    // Now allocate each output
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        allocateAudioOutput(&outs->audio_outputs[i]);
    }
}

static
dlb_pmd_success
allocateMetadataOutput(
    pmd_studio_metadata_output *mdout
    )
{
    unsigned md_alloc_id = mdout->id + METADATA_OUTPUT_ID_OFFSET;
    unsigned int alloc_idx = 0;
    unsigned int right_channel;
    unsigned int i;

    // First run through and deallocate and channels using this output
    for (alloc_idx = 0 ; alloc_idx < MAX_OUTPUT_CHANNELS ; alloc_idx++)
    {
        if (mdout->outputs->chan_allocated[alloc_idx] == md_alloc_id)
        {
            mdout->outputs->chan_allocated[alloc_idx] = 0;
            mdout->outputs->num_chan_allocated--;
        }
    }
    
    if (mdout->subframemode)
    {
        right_channel = mdout->channel;
    }
    else
    {
        // Can't have subframe mode on the last channel
        if (mdout->channel == (MAX_OUTPUT_CHANNELS - 1))
        {
            uiMsgBoxError(mdout->outputs->window, "Error allocating metadata", "Can't select frame mode on last channel");            
            return(PMD_FAIL);
        }
        right_channel = mdout->channel + 1;
    }

    for (i = mdout->channel ; i <= right_channel ; i++)
    {
        // Now attempt to allocate directly
        if (!mdout->outputs->chan_allocated[i])
        {
            mdout->outputs->chan_allocated[i] = md_alloc_id;
            mdout->outputs->num_chan_allocated++;
        }
        else
        {
            uiMsgBoxError(mdout->outputs->window, "Assert - error allocating metadata", "Can't allocate metadata output");            
            // Blocked, if on right channel then deallocate left and exit
            if (i > mdout->channel)
            {
                mdout->outputs->chan_allocated[mdout->channel] = 0;
                mdout->outputs->num_chan_allocated--;
            }
            return(PMD_FAIL);
        }
    }
    return(PMD_SUCCESS);
}

static
dlb_pmd_success deallocateAudioOutput(
    pmd_studio_audio_output *aout
    )
{
    unsigned int alloc_idx = 0;
    int channel_count = pmd_studio_speaker_config_num_channels[aout->config];

    /* Deallocate channels */
    /* search through all channels and deallocate any that match id */
    for (alloc_idx = 0 ; (alloc_idx < MAX_OUTPUT_CHANNELS) && (aout->outputs->num_chan_allocated > 0) ; alloc_idx++)
    {
        if (aout->outputs->chan_allocated[alloc_idx] == aout->id)
        {
            aout->outputs->chan_allocated[alloc_idx] = 0;
            channel_count--;
            aout->outputs->num_chan_allocated--;        
        }
    }
    if (channel_count != 0)
    {
        uiMsgBoxError(aout->outputs->window, "Assert - error deallocating channels", "didn't reach 0 after deallocating channels");
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


static
dlb_pmd_success deallocateMetadataOutput(
    pmd_studio_metadata_output *mdout
    )
{
    unsigned int alloc_idx;
    unsigned md_alloc_id = mdout->id + METADATA_OUTPUT_ID_OFFSET;
    int channels_left;

    if (mdout->subframemode)
    {
        channels_left = 1;
    }
    else
    {
        channels_left = 2;
    }

    for (alloc_idx = 0 ; alloc_idx < MAX_OUTPUT_CHANNELS ; alloc_idx++)
    {
        if (mdout->outputs->chan_allocated[alloc_idx] == md_alloc_id)
        {
            mdout->outputs->chan_allocated[alloc_idx] = 0;
            mdout->outputs->num_chan_allocated--;
        }
        channels_left--;
    }

    if (channels_left == 0)
    {
        return(PMD_SUCCESS);
    }
    else
    {
        return(PMD_FAIL);
    }
}


static
dlb_pmd_success
add_audio_output
    (uiGrid *grid
    ,pmd_studio_audio_output *aout
    ,unsigned int id
    ,pmd_studio_outputs *outs
    )
{
    char tmp[32];
    unsigned int i;
    unsigned int gridYIndex = id + 1;

    pmd_studio_audio_output_init(aout);
    aout->outputs = outs;
    aout->id = id;

    snprintf(tmp, sizeof(tmp), "%d", aout->id);
    uiGridAppend(grid, uiControl(uiNewLabel(tmp)), 0, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aout->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(aout->enable, onEnableOutput, aout);
    uiGridAppend(grid, uiControl(aout->enable), 1, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aout->cfg = uiNewCombobox();
    for (i = 0 ; i < PMD_STUDIO_OUTPUT_NUM_CONFIG ; i++)
    {
        uiComboboxAppend(aout->cfg, pmd_studio_speaker_config_strings[pmd_studio_output_config[i]]);
    }
    uiComboboxSetSelected(aout->cfg, (int)aout->config); 
    uiComboboxOnSelected(aout->cfg, onOutputConfigUpdated, aout);
    uiGridAppend(grid, uiControl(aout->cfg), 2, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    aout->pres = uiNewCombobox();

    for (i = 0; i < outs->num_presentations ; i++)
    {
        uiComboboxAppend(aout->pres, pmd_studio_audio_presentation_get_name_by_id(outs->studio, outs->presentation_ids[i]));
    }
    uiComboboxOnSelected(aout->pres, onOutputPresentationUpdated, aout);
    uiGridAppend(grid, uiControl(aout->pres), 3, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    /* add start channel label */
    aout->schan = uiNewCombobox();
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        sprintf(tmp, "%d",i + 1); // uiLabels start at 1
        uiComboboxAppend(aout->schan, tmp);
    }
    uiComboboxSetSelected(aout->schan, (int)aout->startchannel); 
    uiComboboxOnSelected(aout->schan, onOutputStartChannelUpdated, aout);
    uiGridAppend(grid, uiControl(aout->schan), 4, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
 
    /* add end channel label */
    aout->echan = uiNewLabel("");
    uiGridAppend(grid, uiControl(aout->echan), 5, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    outs->audio_output_count++;
    return PMD_SUCCESS;
}

static
dlb_pmd_success
add_metadata_output
    (uiGrid *grid
    ,pmd_studio_metadata_output *mdout
    ,unsigned int id
    ,pmd_studio_outputs *outs
    )
{
    char tmp[32];
    unsigned int i;
    unsigned int gridYIndex = id + 1;

    mdout->outputs = outs;
    pmd_studio_metadata_output_init(mdout);
    mdout->id = id;

    snprintf(tmp, sizeof(tmp), "%d", mdout->id);
    uiGridAppend(grid, uiControl(uiNewLabel(tmp)), 1, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    mdout->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(mdout->enable, onEnableMetadataOutput, mdout);
    uiGridAppend(grid, uiControl(mdout->enable), 2, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    mdout->fmt = uiNewCombobox();
    for (i = 0 ; i < NUM_METADATA_FORMATS ; i++)
    {
        uiComboboxAppend(mdout->fmt, pmd_studio_metadata_format_names[i]);
    }
    uiComboboxSetSelected(mdout->fmt, (pmd_studio_metadata_format)mdout->format); 
    uiComboboxOnSelected(mdout->fmt, onMetadataFormatUpdated, mdout);
    uiGridAppend(grid, uiControl(mdout->fmt), 3, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    mdout->mode = uiNewCombobox();
    uiComboboxAppend(mdout->mode, "Frame (Stereo)");
    uiComboboxAppend(mdout->mode, "Subframe (Mono)");
    uiComboboxSetSelected(mdout->mode, (dlb_pmd_bool)mdout->subframemode); 
    uiComboboxOnSelected(mdout->mode, onMetadataModeUpdated, mdout);
    uiGridAppend(grid, uiControl(mdout->mode), 4, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    mdout->chan = uiNewCombobox();
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        sprintf(tmp, "%d",i + 1); // uiLabels start at 1
        uiComboboxAppend(mdout->chan, tmp);
    }
    uiComboboxSetSelected(mdout->chan, (int)mdout->channel); 
    uiComboboxOnSelected(mdout->chan, onMetadataChannelUpdated, mdout);
    uiGridAppend(grid, uiControl(mdout->chan), 5, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    outs->metadata_output_count++;
    return PMD_SUCCESS;
}

static
void
recalculateAudioOutputChannelLabels(
    pmd_studio *studio
    )
{
    unsigned int i;

    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);

    /* First reset allocation array */
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        outs->chan_allocated[i] = 0;
    }
    outs->num_chan_allocated = 0;

    /* Allocate metadata output*/
    for (i = 0 ; i < outs->metadata_output_count ; i++)
    {
        if (outs->metadata_outputs[i].enabled)
        {
            allocateMetadataOutput(&outs->metadata_outputs[i]);
        }
    }


    /* Loop over audio outputs */
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        if (outs->audio_outputs[i].enabled)
        {
            allocateAudioOutput(&outs->audio_outputs[i]);
        }
    }
}


static
void
outputs_add_grid_title
    (uiGrid *gbed
    ,const char *txt
    ,int left
    ,int xspan // No of cells
    ,int hexpand // 1 or 0
    ,int halign //  uiAlignFill, uiAlignStart, uiAlignCenter, uiAlignEnd,
    )
{
    // _UI_EXTERN void uiGridAppend(uiGrid *g, uiControl *c, int left, int top, int xspan, int yspan, int hexpand, uiAlign halign, int vexpand, uiAlign valign);
    uiGridAppend(gbed, uiControl(uiNewLabel(txt)), left, 1, xspan, 1, hexpand, halign, 0, uiAlignFill);
}

static
dlb_pmd_success
pmd_studio_outputs_start_metadata_output
    (pmd_studio_metadata_output *mdout
    )
{
    dlb_pmd_model *model;
    dlb_pmd_bool pair;
    dlb_pmd_bool sadm;
    unsigned int num_channels;
    dlb_pcmpmd_augmentor *aug;
    void *mem;
    dlb_pmd_model_constraints limits;

    // Don't start an unenabled metadata ouput
    if (!mdout->enabled)
    {
        return(PMD_FAIL);
    }

    if (mdout->format == SADM_OUTPUT_MODE)
    {
        sadm = PMD_TRUE;
    }
    else
    {
        sadm = PMD_FALSE;
    }

    model = pmd_studio_get_model(mdout->outputs->studio);

    dlb_pmd_get_constraints(model, &limits);
    mem = malloc(dlb_pcmpmd_augmentor_query_mem2(sadm, &limits));

    if (!mem)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Could not allocate memory for SADM/PMD encoder");
        return(PMD_FAIL);
    }

    if (mdout->subframemode)
    {
        pair = PMD_FALSE;
        num_channels = 1;
    }
    else
    {
        pair = PMD_TRUE;
        num_channels = 2;
    }

    dlb_pcmpmd_augmentor_init2
        (&aug,
         model,
         mem,
         METADATA_DLB_PMD_FRAME_RATE,
         DLB_PMD_KLV_UL_ST2109,
         PMD_FALSE,
         num_channels,
         num_channels,
         pair,
         0,
         sadm);

    // Define buffer of size of 1 video frame
    mdout->pcmbufsize = METADATA_SAMPLES_PER_FRAME * num_channels;

    memset(mdout->pcmbuf, '\0', (size_t)mdout->pcmbufsize * METADATA_MAX_BYTES_PER_SAMPLE);

    dlb_pcmpmd_augment(aug, mdout->pcmbuf, mdout->pcmbufsize, 0); //DLB_PMD_VSYNC_NONE);

    dlb_pcmpmd_augmentor_finish(aug); 

    free(mem);

    
    if (pmd_studio_device_add_ring_buffer(mdout->channel, num_channels, mdout->pcmbuf, mdout->pcmbufsize,
     &mdout->ring_buffer_handle, mdout->outputs->studio) == PMD_FAIL)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Adding ring buffer failed");        
    }

    // Inform device layer of sadm buffer so that it can setup point

    return(PMD_SUCCESS);
}

static
dlb_pmd_success
pmd_studio_outputs_stop_metadata_output
    (pmd_studio_metadata_output *mdout
    )
{
    return(pmd_studio_device_delete_ring_buffer(mdout->ring_buffer_handle, mdout->outputs->studio));
}



/* Public Functions */

dlb_pmd_success
pmd_studio_outputs_init
    (pmd_studio_outputs **outs
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *studio1
    )
{
    unsigned int i;
    //uiBox *vbox;
    uiBox *vbox;
    uiBox *hbox;

    uiButton *button;
    
    *outs = malloc(sizeof(pmd_studio_outputs));
    if(!*outs)
    {
        return(PMD_FAIL);
    }

    (*outs)->window = win;

    (*outs)->studio = studio1;

    (*outs)->audio_output_count = 0;
    (*outs)->metadata_output_count = 0;

   /* Do channels init here to avoid having yet another function */
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        (*outs)->chan_allocated[i] = 0;
    }
    (*outs)->num_chan_allocated = 0;


    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);

    uiBoxAppend(vbox, uiControl(uiNewLabel("Outputs")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);    
    //uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);

    hbox = uiNewHorizontalBox();
    uiBoxSetPadded(hbox, 1);

    (*outs)->audio_output_grid = uiNewGrid();
    uiGridSetPadded((*outs)->audio_output_grid, 1);
    uiBoxAppend(hbox, uiControl((*outs)->audio_output_grid), 0);

    /* 'add audio output' button functionality */
    button = uiNewButton("Add Audio Output");
    //uiBoxAppend(hbox1, uiControl(button), 1);
    uiGridAppend((*outs)->audio_output_grid, uiControl(button), 1, 0, 5, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiButtonOnClicked(button, onAddAudioOutputButtonClicked, *outs);

    outputs_add_grid_title((*outs)->audio_output_grid, "En",           1, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->audio_output_grid, "Cfg",          2, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->audio_output_grid, "Presentation", 3, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->audio_output_grid, "Start",        4, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->audio_output_grid, "End",          5, 2, 0, uiAlignFill);

    for (i = 0; i < INIT_AUDIO_OUTPUTS; i++)
    {
        if (add_audio_output((*outs)->audio_output_grid, &((*outs)->audio_outputs[i]), i+1, *outs)) // Ids start at 1
        {
            return PMD_FAIL;
        }
    }

    uiBoxAppend(hbox, uiControl(uiNewVerticalSeparator()), 0);

    (*outs)->metadata_output_grid = uiNewGrid();
    uiGridSetPadded((*outs)->metadata_output_grid, 1);
    uiBoxAppend(hbox, uiControl((*outs)->metadata_output_grid), 0);

    button = uiNewButton("Add Metadata Output");
    //uiBoxAppend(hbox1, uiControl(button), 1);
    uiGridAppend((*outs)->metadata_output_grid, uiControl(button), 1, 0, 5, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiButtonOnClicked(button, onAddMetadataOutputButtonClicked, *outs);


    outputs_add_grid_title((*outs)->metadata_output_grid, "En",       2, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->metadata_output_grid, "Format",   3, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->metadata_output_grid, "Mode",     4, 1, 0, uiAlignFill);
    outputs_add_grid_title((*outs)->metadata_output_grid, "Channel",  5, 1, 0, uiAlignFill);

    for (i = 0; i < INIT_METADATA_OUTPUTS; i++)
    {
        if (add_metadata_output((*outs)->metadata_output_grid, &((*outs)->metadata_outputs[i]), i+1, *outs)) // Ids start at 1
        {
            return PMD_FAIL;
        }
    }

    /* this appends the entire outputs box to the studio window */
    uiBoxAppend(vbox, uiControl(hbox), 0);
    uiBoxAppend(box, uiControl(vbox), 0);
    /* add another (empty) box essentially to give the window some space at the bottom */
    uiBoxAppend(hbox, uiControl(uiNewLabel("")), 0);



    return PMD_SUCCESS;
}


dlb_pmd_success
pmd_studio_audio_output_update_presentation_names
    (pmd_studio *studio)
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);
    unsigned int i,j;
    unsigned int *new_presentation_ids;

    // Quit if in file only mode
    if (!outs)
    {
    	return(PMD_SUCCESS);
    }

    // Refesh local copy of ids
    outs->num_presentations = get_studio_presentation_ids(outs->studio, &new_presentation_ids);
    for (i = 0 ; i < outs->num_presentations ; i++)
    {
        outs->presentation_ids[i] = new_presentation_ids[i];
    }

    // Reconstruct Comboboxes
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        uiGridDelete(outs->audio_output_grid, uiControl(outs->audio_outputs[i].pres));
        uiControlDestroy(uiControl(outs->audio_outputs[i].pres));
        outs->audio_outputs[i].pres = uiNewCombobox();
        for (j = 0; j < outs->num_presentations ; j++)
        {
            uiComboboxAppend(outs->audio_outputs[i].pres, pmd_studio_audio_presentation_get_name_by_id(studio, outs->presentation_ids[j]));
        }
        uiComboboxOnSelected(outs->audio_outputs[i].pres, onOutputPresentationUpdated, &outs->audio_outputs[i]);
        uiGridAppend(outs->audio_output_grid, uiControl(outs->audio_outputs[i].pres), 3, outs->audio_outputs[i].id + 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    }

    // Restore Combobox selection based on preexisting ids
    // If id doesn't exist then disable output and set to first
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        // First etablish if selected id is currently in list
        // If so then set combobox using the index in the id table
        // If not then disable and set to 0
        for (j = 0; j < outs->num_presentations ; j++)
        {
            if (outs->audio_outputs[i].presentation_id == outs->presentation_ids[j])
            {
                break;
            }
        }
        if (outs->audio_outputs[i].presentation_id == outs->presentation_ids[j])
        {
            uiComboboxSetSelected(outs->audio_outputs[i].pres, j);
        }
        else
        {
            outs->audio_outputs[i].enabled = 0;
            uiComboboxSetSelected(outs->audio_outputs[i].pres, 0);
            outs->audio_outputs[i].presentation_id = pmd_studio_audio_presentation_get_id(outs->studio, 0);
        }
    }
    return PMD_SUCCESS;

}

void
pmd_studio_audio_output_presentation_disabled
    (pmd_studio *studio, unsigned int id)
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);
    unsigned int i;

    if (!outs)
    {
    	return;
    }

    // Go through outputs and disable any that have the disabled presentation selected
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        if (outs->audio_outputs[i].presentation_id == id)
        {
            outs->audio_outputs[i].enabled = 0;
            uiCheckboxSetChecked(outs->audio_outputs[i].enable, 0);
            deallocateAudioOutput(&outs->audio_outputs[i]);
        }
    }
}


void
pmd_studio_outputs_print_debug
    (pmd_studio *studio
    )
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);
    unsigned int i;
    dlb_pmd_success status;
    pmd_studio_mix_matrix_array mix_matrix;

    if (!outs)
    {
    	return;
    }

    printf("Audio Outputs\n===== =======\n");
    printf("Audio output count: %u\n", outs->audio_output_count);
    printf("Number of Presentations: %u\n", outs->num_presentations);
    printf("Presentation Id Table: ");
    for (i = 0 ; i < outs->num_presentations ; i++)
    {
        printf("%u ", outs->presentation_ids[i]);
    }
    printf("\n");
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        printf("Audio Output #%u\n----- ------ --\n",i);
        printf("\tEnabled: %u\n", outs->audio_outputs[i].enabled);
        printf("\tId: %u\n", outs->audio_outputs[i].id);
        if (outs->audio_outputs[i].config < PMD_STUDIO_OUTPUT_NUM_CONFIG)
        {
            printf("\tSpeaker Config: %s\n", pmd_studio_speaker_config_strings[outs->audio_outputs[i].config]);
        }
        else
        {
            printf("\tSpeaker Config Out of Range: %u\n", outs->audio_outputs[i].config);
        }
        printf("\tPresentation Id: %u\n", outs->audio_outputs[i].presentation_id);
        printf("\tStart Channel: %d\n", outs->audio_outputs[i].startchannel);
        printf("\tEnd Channel: %d\n", outs->audio_outputs[i].endchannel);
    }

    printf("Metadata Outputs\n===== =======\n");
    printf("Metadata output count: %u\n", outs->metadata_output_count);
    for (i = 0 ; i < outs->metadata_output_count ; i++)
    {
        printf("Metadata Output #%u\n----- ------ --\n",i);
        printf("\tEnabled: %u\n", outs->metadata_outputs[i].enabled);
        printf("\tId: %u\n", outs->metadata_outputs[i].id);
        printf("Format: %s\n", pmd_studio_metadata_format_names[outs->metadata_outputs[i].format]);
        if (outs->metadata_outputs[i].subframemode)
        {
            printf("\tMode: Subframe Mode\n");
        }
        else
        {
            printf("\tMode: Frame Mode\n");
        }
        printf("\tChannel: %d\n", outs->metadata_outputs[i].channel);
    }

    printf("Channel Outputs\n======= =======\n");
    printf("Number of channels allocated: %u\n", outs->num_chan_allocated);
    printf("Allocation Table\n---------- -----\n");
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        if (outs->chan_allocated[i])
        {
            printf("\tChannel %2u: ",i);
            if (outs->chan_allocated[i] < METADATA_OUTPUT_ID_OFFSET)
            {
                printf("---> Audio Output #%u\n", outs->chan_allocated[i]);
            }
            else
            {
                printf("---> Metadata Output #%u\n", outs->chan_allocated[i] - METADATA_OUTPUT_ID_OFFSET);
            }
        }
    }

    status = pmd_studio_audio_outputs_get_mix_matrix(mix_matrix, outs->studio);
    if (status != PMD_SUCCESS)
    {
        printf("Error: Failed to get mix matrix\n");
    }
    else
    {
        pmd_studio_mix_matrix_print(mix_matrix);
    }
}

void
pmd_studio_outputs_refresh_ui(
    pmd_studio_outputs *outs
    )
{
	if (outs)
	{
    	pmd_studio_audio_output_update_presentation_names(outs->studio);
    }
}



dlb_pmd_success pmd_studio_audio_outputs_get_mix_matrix(
    pmd_studio_mix_matrix mix_matrix,
    pmd_studio *studio)
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);

    // Loop over audio outputs
    unsigned int i,j,k;
    pmd_studio_audio_output *out;
    pmd_studio_mix_matrix_array pres_mix_matrix;

    if (!outs)
    {
    	return(PMD_FAIL);
    }

    pmd_studio_mix_matrix_reset(mix_matrix);
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        out = &(outs->audio_outputs[i]);
        if (out->enabled)
        {
            pmd_studio_mix_matrix_reset(pres_mix_matrix);
            // get presentation mix mixtrix
            if (pmd_studio_audio_presentations_get_mix_matrix(out->presentation_id, out->config, pres_mix_matrix, studio) != PMD_SUCCESS)
            {
                return(PMD_FAIL);
            }
            // The returned mix matrix is not at the correct channel because the presentation has no knowledge of the
            // output channel location.
            // Copy returned mix matrix to correct location in the output matrix
            for (j = 0 ; j < MAX_INPUT_CHANNELS ; j++)
            {
                for (k = 0 ; k < pmd_studio_speaker_config_num_channels[out->config] ; k++)
                {
                    mix_matrix[j][k + out->startchannel] = pres_mix_matrix[j][k];
                }
            }
        }
    }
    return(PMD_SUCCESS);
}


dlb_pmd_success
pmd_studio_outputs_finish
    (pmd_studio_outputs *ao1
    )
{
	if (ao1)
	{
    	free(ao1);
    }
    return(PMD_SUCCESS);
}

