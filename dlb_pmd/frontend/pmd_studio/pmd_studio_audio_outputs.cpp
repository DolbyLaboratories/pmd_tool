/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

extern "C"{
    #include "dlb_pmd_pcm.h"
    #include "ui.h"
    #include "dlb_pmd_api.h"
    #include "pmd_model.h"
    #include "dlb_pmd_model_combo.h"
    #include "sadm_bitstream_encoder.h"
}

#include "pmd_studio_common_defs.h"
#include "pmd_studio.h"
#include "pmd_studio_device.h"
#include "pmd_studio_pvt.h"
#include "mix_matrix.h"

#include "pmd_studio_audio_outputs.h"
#include "pmd_studio_audio_outputs_pvt.h"
#include "ring_buffer.h"
/* Definitons */

/* Definitions */

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
dlb_pmd_success
pmd_studio_outputs_start_metadata_output
    (pmd_studio_metadata_output *mdout
    );

static
dlb_pmd_success
pmd_studio_outputs_stop_metadata_output
    (pmd_studio_metadata_output *mdout
    );

static
void
disable_audio_output(
    pmd_studio_audio_output *aout
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
    pmd_studio_config *configs;

    get_pmd_studio_configs(aout->outputs->studio, &configs);

    channel_count = configs[aout->config_index].num_channels;
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
                uiMsgBoxError(aout->outputs->window, "error enabling audio output", "invalid output configuration");
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
            /* No error message here as that done by allocateAudioOutput */
            uiCheckboxSetChecked(en, 0);
            aout->enabled = 0;
            return;
        }
    }
    else
    {
        disable_audio_output(aout);
    }

    /* Should never hit this but just in case */
    if (alloc_idx == MAX_OUTPUT_CHANNELS)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Channel allocation index out of bounds");
    }

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
        if (allocateMetadataOutput(mdout))
        {
            // Error handled by alloc
            uiCheckboxSetChecked(en, 0);
            mdout->enabled = 0;
        }
        else
        {
            if (pmd_studio_outputs_start_metadata_output(mdout))
            {
                uiMsgBoxError(mdout->outputs->window, "Error Enabling Metadata Output", "Can't restart metadata output");
                uiCheckboxSetChecked(en, 0);
                mdout->enabled = 0;                
            }
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
    unsigned int old_config_index = aout->config_index;

    aout->config_index = uiComboboxSelected(c);

    if (aout->enabled)
    {
        // This is an enabled output so this could cause the entire table to be recalculated
        deallocateAudioOutput(aout);
        // Now attempt to reallocate at the new postion
        if (allocateAudioOutput(aout) == PMD_SUCCESS)
        {
            recalculateAudioOutputChannelLabels(aout->outputs->studio);
            if (pmd_studio_device_update_mix_matrix(aout->outputs->studio))
            {
                uiMsgBoxError(aout->outputs->window, "error reassigning audio output", "invalid output configuration");
                aout->config_index = old_config_index;
                uiComboboxSetSelected(c, aout->config_index);
                deallocateAudioOutput(aout);
                recalculateAudioOutputChannelLabels(aout->outputs->studio);
                return;
            }
        }
        else
        {
            /* Failed so disable enablement */
            /* No error message here as that done by allocateAudioOutput */
            aout->config_index = old_config_index;
            uiComboboxSetSelected(c,aout->config_index);
            /* reallocate */
            recalculateAudioOutputChannelLabels(aout->outputs->studio);
            return;
        }

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

    if (index < aout->outputs->num_presentations)
    {
        aout->presentation_id = (*aout->outputs->presentation_ids)[index];
    }
    // Enabled checkbox is enabled as soon as a valid presentation is selected
    if (pmd_studio_get_mode(aout->outputs->studio) != PMD_STUDIO_MODE_CONSOLE_LIVE)
    {
        uiControlEnable(uiControl(aout->enable));
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
    unsigned int oldstartchannel = aout->startchannel;

    aout->startchannel = index;
    if (aout->enabled)
    {
        // This is an enabled output so this could cause the entire table to be recalculated
        deallocateAudioOutput(aout);
        // Now attempt to reallocate at the new postion
        if (allocateAudioOutput(aout) == PMD_SUCCESS)
        {
            recalculateAudioOutputChannelLabels(aout->outputs->studio);
            if (pmd_studio_device_update_mix_matrix(aout->outputs->studio))
            {
                uiMsgBoxError(aout->outputs->window, "Error Reassigning Audio Output", "Invalid output configuration");
                aout->startchannel = oldstartchannel;
                uiComboboxSetSelected(c,aout->startchannel);
                deallocateAudioOutput(aout);
                recalculateAudioOutputChannelLabels(aout->outputs->studio);
                return;
            }
        }
        else
        {
            /* Failed so disable enablement */
            /* No error message here as that done by allocateAudioOutput */
            aout->startchannel = oldstartchannel;
            uiComboboxSetSelected(c,aout->startchannel);
            /* The following should work as we just deallocated */
            allocateAudioOutput(aout);
            return;
        }

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
            // No ui error here as 
            uiCheckboxSetChecked(mdout->enable, 0);
            mdout->enabled = 0;
        }
    }   
}

static
void
onMetadataFrameRateUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_metadata_output *mdout = (pmd_studio_metadata_output *)data;
    mdout->frame_rate = (pmd_studio_video_frame_rate)uiComboboxSelected(c);
    // If output is running, update ring buffer
    if (mdout->enabled)
    {
        pmd_studio_outputs_stop_metadata_output(mdout);
        // re-enable with new format/ Disable output on failure
        if (pmd_studio_outputs_start_metadata_output(mdout))
        {
            // No ui error here as 
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
    dlb_pmd_bool oldsubframemode = mdout->subframemode;
    mdout->subframemode = (dlb_pmd_bool)uiComboboxSelected(c);
    if (mdout->enabled)
    {
        // Must test allocation as could be increasing from 1 to 2 channels
        if (allocateMetadataOutput(mdout))
        {
            uiMsgBoxError(mdout->outputs->window, "Error Changing Metadata Format", "Channel(s) already in use");
            // allocation failed so revert to previous setting
            mdout->subframemode = oldsubframemode;
            uiComboboxSetSelected(c, mdout->subframemode);

        }
        pmd_studio_outputs_stop_metadata_output(mdout);
        // re-enable with new format/ Disable output on failure
        if (pmd_studio_outputs_start_metadata_output(mdout) == PMD_FAIL)
        {
            uiMsgBoxError(mdout->outputs->window, "Error Changing Metadata Format", "Can't restart Metadata output");
            uiCheckboxSetChecked(mdout->enable, 0);
            mdout->enabled = 0;
        }
        pmd_studio_audio_outputs_update_metadata_output(mdout);
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
    unsigned int oldchannel = mdout->channel;

    mdout->channel = (int)uiComboboxSelected(c);
    if (mdout->enabled)
    {
        pmd_studio_outputs_stop_metadata_output(mdout);
        if (allocateMetadataOutput(mdout))
        {
            // no error here are alloc provides error
            // allocation failed so revert to previous setting
            mdout->channel = oldchannel;
            uiComboboxSetSelected(c,mdout->channel);
        }
        if (pmd_studio_outputs_start_metadata_output(mdout))
        {
            uiMsgBoxError(mdout->outputs->window, "Error changing Metadata channel", "Can't restart metadata output");
            // Restart failed so disable     
            uiCheckboxSetChecked(mdout->enable, 0);
            mdout->enabled = 0;
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
        uiMsgBoxError(outs->window, "Error Setting Output", "Max outputs reached");
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
        uiMsgBoxError(outs->window, "Error Setting Output", "Max outputs reached");
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
    aout->config_index = 0; // First output by default, probably 2/0
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
    mdout->frame_rate = FPS_25;
}


static
dlb_pmd_success allocateAudioOutput(
    pmd_studio_audio_output *aout
    )
{
    unsigned int channel_count;
    unsigned int alloc_idx = 0;
    char tmp[10];
    pmd_studio_config *configs;


    get_pmd_studio_configs(aout->outputs->studio, &configs);
    channel_count = configs[aout->config_index].num_channels;


    // First check to see if output is enabled
    if (!aout->enabled)
    {
        // Not enabled to nothing to allocate but channel label should be removed
        uiLabelSetText(aout->echan, "");
        return(PMD_SUCCESS);
    }

    /* Ensure that we have enough channels to allocate */
    if ((channel_count + aout->outputs->num_chan_allocated) > MAX_OUTPUT_CHANNELS)
    {
        uiMsgBoxError(aout->outputs->window, "Error Allocating Channels", "Insufficient channels available");
        return(PMD_FAIL);
    }

    /* Allocate channels */
    /* start at user specified start channel */
    alloc_idx = aout->startchannel;
    /* loop over channels to allocate */
    while((channel_count > 0) && (alloc_idx < MAX_OUTPUT_CHANNELS))
    {
        if (aout->outputs->chan_allocated[alloc_idx] && (aout->outputs->chan_allocated[alloc_idx] != aout->id))
        {
            // Detected a channel clash
            char tmp[100];
            if (aout->outputs->chan_allocated[alloc_idx] > METADATA_OUTPUT_ID_OFFSET)
            {
                snprintf(tmp, 100, "Channel %d is already in use by metadata output %d.", alloc_idx+1, aout->outputs->chan_allocated[alloc_idx] - METADATA_OUTPUT_ID_OFFSET);
            }
            else
            {
                snprintf(tmp, 100, "Channel %d is already in use by audio output %d.", alloc_idx+1, aout->outputs->chan_allocated[alloc_idx]);                
            }
            uiMsgBoxError(aout->outputs->studio->window, "Error Allocating Audio Output", tmp);            
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
      
            aout->outputs->chan_allocated[alloc_idx++] = aout->id;
            channel_count--;
            aout->outputs->num_chan_allocated++;
        }
    }
    // Check to see if there are any remaining channels to be allocated
    if (channel_count > 0)
    {
        // if some channels were allocated then deallocate them
        if (channel_count < configs[aout->config_index].num_channels)
        {
            deallocateAudioOutput(aout);
        }
        return PMD_FAIL;
    }
    return(PMD_SUCCESS);
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
    pmd_studio *studio = mdout->outputs->studio;

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
            char tmp[100];
            if (mdout->outputs->chan_allocated[i] > METADATA_OUTPUT_ID_OFFSET)
            {
                snprintf(tmp, 100, "Channel %d is already in use by metadata output %d.", i+1, mdout->outputs->chan_allocated[i] - METADATA_OUTPUT_ID_OFFSET);
            }
            else
            {
                snprintf(tmp, 100, "Channel %d is already in use by audio output %d.", i+1, mdout->outputs->chan_allocated[i]);                
            }
            uiMsgBoxError(studio->window, "Error Allocating Metadata Output", tmp);            
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
    int channel_count;
    pmd_studio_config *configs;


    get_pmd_studio_configs(aout->outputs->studio, &configs);
    channel_count = configs[aout->config_index].num_channels;

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
            channels_left--;
        }
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
    pmd_studio_config *configs;
    unsigned int num_configs = get_pmd_studio_configs(outs->studio, &configs);

    pmd_studio_audio_output_init(aout);
    aout->outputs = outs;
    aout->id = id;

    snprintf(tmp, sizeof(tmp), "%d", aout->id);
    aout->label = uiNewLabel(tmp);

    aout->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(aout->enable, onEnableOutput, aout);
    // Disable by default
    uiControlDisable(uiControl(aout->enable));
    
    aout->cfg = uiNewCombobox();
    
    for (i = 0 ; i < num_configs ; i++)
    {
        uiComboboxAppend(aout->cfg, configs[i].config_string);
    }

    uiComboboxSetSelected(aout->cfg, DLB_PMD_SPEAKER_CONFIG_2_0);
    uiComboboxOnSelected(aout->cfg, onOutputConfigUpdated, aout);

    aout->pres = uiNewCombobox();

    for (i = 0; i < outs->num_presentations ; i++)
    {
        uiComboboxAppend(aout->pres, (*outs->presentation_names)[i]);
    }
    uiComboboxOnSelected(aout->pres, onOutputPresentationUpdated, aout);

    /* add start channel label */
    aout->schan = uiNewCombobox();
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        sprintf(tmp, "%d",i + 1); // uiLabels start at 1
        uiComboboxAppend(aout->schan, tmp);
    }
    uiComboboxSetSelected(aout->schan, 0); // Channel 1 
    uiComboboxOnSelected(aout->schan, onOutputStartChannelUpdated, aout);
 
    /* add end channel label */
    aout->echan = uiNewLabel("");


    uiGridAppend(grid, uiControl(aout->label),  0, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(aout->enable), 1, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(aout->cfg),    2, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(aout->pres),   3, gridYIndex, 1, 1, 0, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(grid, uiControl(aout->schan),  4, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(aout->echan),  5, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
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
    mdout->label = uiNewLabel(tmp);

    mdout->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(mdout->enable, onEnableMetadataOutput, mdout);

    mdout->fmt = uiNewCombobox();
    for (i = 0 ; i < NUM_METADATA_FORMATS ; i++)
    {
        uiComboboxAppend(mdout->fmt, pmd_studio_metadata_format_names[i]);
    }
    mdout->format = SADM_OUTPUT_MODE;
    uiComboboxSetSelected(mdout->fmt, mdout->format);
    uiComboboxOnSelected(mdout->fmt, onMetadataFormatUpdated, mdout);

    mdout->mode = uiNewCombobox();
    uiComboboxAppend(mdout->mode, "Frame (Stereo)");
    uiComboboxAppend(mdout->mode, "Subframe (Mono)");
    mdout->subframemode = PMD_TRUE;
    uiComboboxSetSelected(mdout->mode, 1); // Subframe mode 
    uiComboboxOnSelected(mdout->mode, onMetadataModeUpdated, mdout);

    mdout->fps = uiNewCombobox();
    for(unsigned int j = 0; j < NUM_VIDEO_FRAME_RATES ;  j++)
    {
        if ((((int)ceil(pmd_studio_video_frame_rate_floats[j]*100)) % 100) == 0)
        {
            snprintf(tmp, sizeof(tmp), "%.0f FPS", pmd_studio_video_frame_rate_floats[j]);
        }
        else
        {
            snprintf(tmp, sizeof(tmp), "%.2f FPS", pmd_studio_video_frame_rate_floats[j]);            
        }
        uiComboboxAppend(mdout->fps, tmp);
    }
    uiComboboxSetSelected(mdout->fps, (int)mdout->frame_rate);
    uiComboboxOnSelected(mdout->fps, onMetadataFrameRateUpdated, mdout);


    mdout->chan = uiNewCombobox();
    for (i = 0 ; i < MAX_OUTPUT_CHANNELS ; i++)
    {
        sprintf(tmp, "%d",i + 1); // uiLabels start at 1
        uiComboboxAppend(mdout->chan, tmp);
    }
    uiComboboxSetSelected(mdout->chan, 0); // Channel 1 
    uiComboboxOnSelected(mdout->chan, onMetadataChannelUpdated, mdout);


    uiGridAppend(grid, uiControl(mdout->label),      1, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(mdout->enable),     2, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(mdout->fmt),        3, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(mdout->fps),        4, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);    
    uiGridAppend(grid, uiControl(mdout->mode),       5, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(mdout->chan),       6, gridYIndex, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
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
        allocateAudioOutput(&outs->audio_outputs[i]);
    }
}


static
dlb_pmd_success
pmd_studio_outputs_start_metadata_output
    (pmd_studio_metadata_output *mdout
    )
{
    pmd_studio *studio = mdout->outputs->studio;


    mdout->smpte337_wrapped = pmd_studio_device_channel_requires_smpte337(mdout->channel, studio);

    unsigned int num_channels = (mdout->subframemode)? 1:2;

    // Using invalid frame rate signals to the ring buffer to just use a large buffer as we are streaming
    // to a data channel not audio channel
    pmd_studio_device_add_ring_buffer(mdout->channel, num_channels, mdout->frame_rate, studio);

    // Update empty buffer with model
    if(pmd_studio_audio_outputs_update_metadata_output(mdout) == PMD_FAIL)
    {
        return PMD_FAIL;
    }

    if(studio->mode == PMD_STUDIO_MODE_EDIT)
    {
        pmd_studio_switch_mode(studio, PMD_STUDIO_MODE_LIVE);
    }
    return(PMD_SUCCESS);
}

static
dlb_pmd_success
pmd_studio_outputs_stop_metadata_output
    (pmd_studio_metadata_output *mdout
    )
{
    pmd_studio *studio = mdout->outputs->studio;

    // Check that channel isn't already being used in existing ring buffer?
    PMDStudioRingBufferList *ring_buffer_list = pmd_studio_device_get_ring_buffer_list(studio);
    ring_buffer_list->DeleteRingBuffer(mdout->channel);
    
    if(pmd_studio_metadata_output_active(studio) == PMD_FALSE)
    {
        pmd_studio_switch_mode(studio, PMD_STUDIO_MODE_EDIT);
    }

    return(PMD_SUCCESS);
}

static
void
disable_audio_output(
    pmd_studio_audio_output *aout
    )
{
    uiCheckboxSetChecked(aout->enable, 0);
    aout->enabled = 0;
    deallocateAudioOutput(aout);
    recalculateAudioOutputChannelLabels(aout->outputs->studio);
    pmd_studio_device_update_mix_matrix(aout->outputs->studio);
}

static
dlb_pmd_success
get_sadm_payload(dlb_pmd_model_combo *model, void *payload, unsigned int &max_payload_size)
{
    sadm_bitstream_encoder *enc;
    int sadm_payload_bytes;
    size_t sadm_end_mem_size;

    const dlb_adm_core_model *core_model = NULL;

    if (max_payload_size > MAX_DATA_BYTES)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Memory for sADM payload not large enough");
        return (PMD_FAIL);
    }

    sadm_end_mem_size = sadm_bitstream_encoder_query_mem();
    void *mem = malloc(sadm_end_mem_size);

    if(mem == nullptr)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Could not allocate memory for SADM/PMD encoder");
        return(PMD_FAIL);
    }

    sadm_bitstream_encoder_init(mem, &enc);

    if (!dlb_pmd_model_combo_ensure_readable_core_model(model, &core_model))
    {
        sadm_payload_bytes = sadm_bitstream_encoder_payload(enc, core_model, (uint8_t *)payload);
    }
    else
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to get readable core model from combo model");
        return (PMD_FAIL);
    }

    if (sadm_payload_bytes > MAX_DATA_BYTES)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "sADM payload was larger than expected, corruption may have occurred");
        return (PMD_FAIL);
    }

    max_payload_size = sadm_payload_bytes;

    return (PMD_SUCCESS);
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
    
    *outs = (pmd_studio_outputs *) malloc(sizeof(pmd_studio_outputs));
    if(!*outs)
    {
        return(PMD_FAIL);
    }

    (*outs)->window = win;

    (*outs)->studio = studio1;

    (*outs)->audio_output_count = 0;
    (*outs)->metadata_output_count = 0;
    (*outs)->num_presentations = 0;

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
    (*outs)->add_aout_button = uiNewButton("Add Audio Output");
    //uiBoxAppend(hbox1, uiControl(button), 1);
    uiGridAppend((*outs)->audio_output_grid, uiControl((*outs)->add_aout_button), 1, 0, 5, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiButtonOnClicked((*outs)->add_aout_button, onAddAudioOutputButtonClicked, *outs);

 	uiGridAppend((*outs)->audio_output_grid, uiControl(uiNewLabel("En")),           1, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->audio_output_grid, uiControl(uiNewLabel("Cfg")),          2, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->audio_output_grid, uiControl(uiNewLabel("Presentation")), 3, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->audio_output_grid, uiControl(uiNewLabel("Start")),        4, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->audio_output_grid, uiControl(uiNewLabel("End")),          5, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    uiBoxAppend(hbox, uiControl(uiNewVerticalSeparator()), 0);

    (*outs)->metadata_output_grid = uiNewGrid();
    uiGridSetPadded((*outs)->metadata_output_grid, 1);
    uiBoxAppend(hbox, uiControl((*outs)->metadata_output_grid), 0);

    (*outs)->add_mdout_button = uiNewButton("Add Metadata Output");
    //uiBoxAppend(hbox1, uiControl(button), 1);
    uiGridAppend((*outs)->metadata_output_grid, uiControl((*outs)->add_mdout_button), 1, 0, 5, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiButtonOnClicked((*outs)->add_mdout_button, onAddMetadataOutputButtonClicked, *outs);

 	uiGridAppend((*outs)->metadata_output_grid, uiControl(uiNewLabel("En")),      2, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->metadata_output_grid, uiControl(uiNewLabel("Format")),  3, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*outs)->metadata_output_grid, uiControl(uiNewLabel("Frame Rate")),  4, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->metadata_output_grid, uiControl(uiNewLabel("Mode")),    5, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
 	uiGridAppend((*outs)->metadata_output_grid, uiControl(uiNewLabel("Channel")), 6, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);


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

    // Quit if in file only mode
    if (!outs)
    {
    	return(PMD_SUCCESS);
    }

    // Refesh local copy of ids
    outs->num_presentations = pmd_studio_audio_presentation_get_enabled(outs->studio, &outs->presentation_ids, &outs->presentation_names);

    // Reconstruct Comboboxes
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        uiGridDelete(outs->audio_output_grid, uiControl(outs->audio_outputs[i].pres));
        uiControlDestroy(uiControl(outs->audio_outputs[i].pres));
        outs->audio_outputs[i].pres = uiNewCombobox();
        for (j = 0; j < outs->num_presentations ; j++)
        {
            uiComboboxAppend(outs->audio_outputs[i].pres, (*outs->presentation_names)[j]);
        }
        uiComboboxOnSelected(outs->audio_outputs[i].pres, onOutputPresentationUpdated, &outs->audio_outputs[i]);
        uiGridAppend(outs->audio_output_grid, uiControl(outs->audio_outputs[i].pres), 3, outs->audio_outputs[i].id + 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    }

    // Restore Combobox selection based on preexisting ids
    // If id doesn't exist then disable output and set to first
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        // First establish if selected id is currently in list
        // If so then set combobox using the index in the id table
        // If not then disable and set to 0
        for (j = 0; j < outs->num_presentations ; j++)
        {
            if (outs->audio_outputs[i].presentation_id == (*outs->presentation_ids)[j])
            {
                break;
            }
        }
        if (j < outs->num_presentations)
        {
            uiComboboxSetSelected(outs->audio_outputs[i].pres, j);
        }
        else
        {
            // If output is enabled and now presentation has been removed then disable it
            if (outs->audio_outputs[i].enabled)
            {
                disable_audio_output(&outs->audio_outputs[i]);
            }
            // Presentation Ids start at 1 so make it invalid by selecting 0
            outs->audio_outputs[i].presentation_id = 0;
            // Disable enabled control until presentation selection occurs
            uiControlDisable(uiControl(outs->audio_outputs[i].enable));
        }
    }
    return PMD_SUCCESS;

}

void
pmd_studio_outputs_print_debug
    (pmd_studio *studio
    )
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);
    unsigned int i;
    pmd_studio_config *configs;
    unsigned int num_configs = get_pmd_studio_configs(outs->studio, &configs);
    const dlb_pmd_bool mix_matrix_debug = PMD_TRUE; // Set to true to get matrix debug

    if (!outs)
    {
    	return;
    }

    printf("Audio Outputs\n===== =======\n");
    printf("Audio output count: %u\n", outs->audio_output_count);
    printf("Number of Presentations: %u\n", outs->num_presentations);
    printf("Presentation Id Table:\n");
    for (i = 0 ; i < outs->num_presentations ; i++)
    {
        printf("\tId: %hu\t%s\n", (*outs->presentation_ids)[i], (*outs->presentation_names)[i]);
    }
    printf("\n");
    for (i = 0 ; i < outs->audio_output_count ; i++)
    {
        printf("Audio Output #%u\n----- ------ --\n",i);
        printf("\tEnabled: %u\n", outs->audio_outputs[i].enabled);
        printf("\tId: %u\n", outs->audio_outputs[i].id);
        if (outs->audio_outputs[i].config_index < num_configs)
        {
            printf("\tSpeaker Config: %s\n", configs[outs->audio_outputs[i].config_index].config_string);
        }
        else
        {
            printf("\tSpeaker Config Out of Range: %u\n", outs->audio_outputs[i].config_index);
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

    dlb_pmd_success status;
    pmd_studio_mix_matrix_array mix_matrix;
    if (mix_matrix_debug)
    {
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

void
pmd_studio_outputs_enable
    (pmd_studio_outputs *outs
    )
{
    unsigned int i;

    for (i = 0; i < outs->audio_output_count; i++)
    {
        uiControlEnable(uiControl(outs->audio_outputs[i].enable));
        uiControlEnable(uiControl(outs->audio_outputs[i].cfg));
        uiControlEnable(uiControl(outs->audio_outputs[i].schan));
     }

    for (i = 0; i < outs->metadata_output_count; i++)
    {
        uiControlEnable(uiControl(outs->metadata_outputs[i].enable));
        uiControlEnable(uiControl(outs->metadata_outputs[i].fmt));
        uiControlEnable(uiControl(outs->metadata_outputs[i].mode));
        uiControlEnable(uiControl(outs->metadata_outputs[i].chan));
    }
    uiControlEnable(uiControl(outs->add_aout_button));
    uiControlEnable(uiControl(outs->add_mdout_button));
}

void
pmd_studio_outputs_disable
    (pmd_studio_outputs *outs,
    bool live_mode
    )
{
    if(!live_mode)
    {
        unsigned int i;
        for (i = 0; i < outs->audio_output_count; i++)
        {
            uiControlDisable(uiControl(outs->audio_outputs[i].enable));
            uiControlDisable(uiControl(outs->audio_outputs[i].cfg));
            uiControlDisable(uiControl(outs->audio_outputs[i].schan));
        }

        for (i = 0; i < outs->metadata_output_count; i++)
        {
            uiControlDisable(uiControl(outs->metadata_outputs[i].enable));
            uiControlDisable(uiControl(outs->metadata_outputs[i].fmt));
            uiControlDisable(uiControl(outs->metadata_outputs[i].mode));
            uiControlDisable(uiControl(outs->metadata_outputs[i].chan));
        }
        uiControlDisable(uiControl(outs->add_aout_button));
        uiControlDisable(uiControl(outs->add_mdout_button));
    }
    else
    {
        pmd_studio_outputs_enable(outs);
    }
}

void
pmd_studio_outputs_stop_all_metadata_outputs(
    pmd_studio *studio)
{
    unsigned int i;

    for(i = 0; i < studio->outputs->metadata_output_count; i++)
    {
        pmd_studio_metadata_output *mdout = &studio->outputs->metadata_outputs[i];
        if(uiCheckboxChecked(mdout->enable))
        {
            uiCheckboxSetChecked(mdout->enable, 0);
            onEnableMetadataOutput(mdout->enable, mdout);
        }
    }

}


dlb_pmd_success pmd_studio_audio_outputs_get_mix_matrix(
    pmd_studio_mix_matrix mix_matrix,
    pmd_studio *studio)
{
    pmd_studio_outputs *outs = pmd_studio_get_outputs(studio);
    pmd_studio_config *configs;
    (void)get_pmd_studio_configs(outs->studio, &configs);

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
            if (pmd_studio_audio_presentations_get_mix_matrix(out->presentation_id, configs[out->config_index].config, pres_mix_matrix, studio) != PMD_SUCCESS)
            {
                return(PMD_FAIL);
            }
            // The returned mix matrix is not at the correct channel because the presentation has no knowledge of the
            // output channel location.
            // Copy returned mix matrix to correct location in the output matrix
            for (j = 0 ; j < MAX_INPUT_CHANNELS ; j++)
            {
                for (k = 0 ; k < configs[out->config_index].num_channels ; k++)
                {
                    mix_matrix[j][k + out->startchannel] = pres_mix_matrix[j][k];
                }
            }
        }
    }
    return(PMD_SUCCESS);
}

void
pmd_studio_outputs_reset
    (pmd_studio_outputs *ao1
    )
{
    unsigned int i;
    pmd_studio_outputs_stop_all_metadata_outputs(ao1->studio);

    for (i = 0 ; i < ao1->audio_output_count ; i++)
    {
        uiGridDelete(ao1->audio_output_grid, uiControl(ao1->audio_outputs[i].label));
        uiGridDelete(ao1->audio_output_grid, uiControl(ao1->audio_outputs[i].enable));
        uiGridDelete(ao1->audio_output_grid, uiControl(ao1->audio_outputs[i].cfg));
        uiGridDelete(ao1->audio_output_grid, uiControl(ao1->audio_outputs[i].pres));
        uiGridDelete(ao1->audio_output_grid, uiControl(ao1->audio_outputs[i].schan));
        uiGridDelete(ao1->audio_output_grid, uiControl(ao1->audio_outputs[i].echan));

        uiControlDestroy(uiControl(ao1->audio_outputs[i].label));
        uiControlDestroy(uiControl(ao1->audio_outputs[i].enable));
        uiControlDestroy(uiControl(ao1->audio_outputs[i].cfg));
        uiControlDestroy(uiControl(ao1->audio_outputs[i].pres));
        uiControlDestroy(uiControl(ao1->audio_outputs[i].schan));
        uiControlDestroy(uiControl(ao1->audio_outputs[i].echan));

    }
    for (i = 0 ; i < ao1->metadata_output_count ; i++)
    {
        uiGridDelete(ao1->metadata_output_grid, uiControl(ao1->metadata_outputs[i].label));
        uiGridDelete(ao1->metadata_output_grid, uiControl(ao1->metadata_outputs[i].enable));
        uiGridDelete(ao1->metadata_output_grid, uiControl(ao1->metadata_outputs[i].fmt));
        uiGridDelete(ao1->metadata_output_grid, uiControl(ao1->metadata_outputs[i].fps));

        uiGridDelete(ao1->metadata_output_grid, uiControl(ao1->metadata_outputs[i].mode));
        uiGridDelete(ao1->metadata_output_grid, uiControl(ao1->metadata_outputs[i].chan));

        uiControlDestroy(uiControl(ao1->metadata_outputs[i].label));
        uiControlDestroy(uiControl(ao1->metadata_outputs[i].enable));
        uiControlDestroy(uiControl(ao1->metadata_outputs[i].fmt));
        uiControlDestroy(uiControl(ao1->metadata_outputs[i].fps));
        uiControlDestroy(uiControl(ao1->metadata_outputs[i].mode));
        uiControlDestroy(uiControl(ao1->metadata_outputs[i].chan));
    }

    ao1->audio_output_count = 0;
    ao1->metadata_output_count = 0;
    ao1->num_presentations = 0;
    ao1->num_chan_allocated = 0;

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

dlb_pmd_success
pmd_studio_audio_outputs_update_metadata_output
    (pmd_studio_metadata_output *mout
    )
{
    void *newbuf;
    unsigned int newbuf_size_bytes = 0;
    PMDStudioRingBufferList *ring_buffer_list = pmd_studio_device_get_ring_buffer_list(mout->outputs->studio);

    if(mout != nullptr)
    {
        dlb_pmd_model_combo *combo_model = pmd_studio_get_model(mout->outputs->studio);

        newbuf = ring_buffer_list->GetBufferForUpdate(mout->channel, newbuf_size_bytes);

        if (!mout->smpte337_wrapped)
        {
            if (mout->format == SADM_OUTPUT_MODE)
            {
                // No SMPTE wrapping so just get payload
                get_sadm_payload(combo_model, newbuf, newbuf_size_bytes);
                // Commit with a fit to the data i.e. no padding
                ring_buffer_list->CommitUpdate(mout->channel, newbuf_size_bytes);
            }
            else
            {
                pmd_studio_error(PMD_STUDIO_ERR_UI, "PMD over SMPTE ST 2110-41 is not supported");
                return(PMD_FAIL);
            }
        }
        else
        {
            // Reset augmentor error flag and callback arg
            mout->augmentor_error = PMD_FALSE;
    #if LATER
            model->error_cbarg = (void *) mout;
    #endif

            dlb_pmd_bool sadm = (mout->format == SADM_OUTPUT_MODE)? PMD_TRUE : PMD_FALSE;
            void *mem = malloc(dlb_pcmpmd_augmentor_query_mem(sadm));
            if(mem == nullptr){
                pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Could not allocate memory for SADM/PMD encoder");
                return(PMD_FAIL);
            }
            dlb_pmd_bool pair;
            unsigned int num_channels;
            if(mout->subframemode)
            {
                pair = PMD_FALSE;
                num_channels = 1;
            }
            else
            {
                pair = PMD_TRUE;
                num_channels = 2;
            }
            dlb_pcmpmd_augmentor *aug;
            unsigned int wrap_depth = 0;    // TODO: add a SMPTE 337m wrapping bit depth UI element and use the value here
            dlb_pcmpmd_augmentor_init3(&aug, combo_model, mem, wrap_depth, METADATA_DLB_PMD_FRAME_RATE, DLB_PMD_KLV_UL_ST2109, PMD_FALSE, num_channels, num_channels, pair, 0, sadm);

            if(mout->augmentor_error == PMD_FALSE)
            {
                unsigned int num_frames = newbuf_size_bytes / (sizeof(uint32_t) * num_channels);
                dlb_pcmpmd_augment(aug, (uint32_t *)newbuf, num_frames , 0);
                dlb_pcmpmd_augmentor_finish(aug);

                free(mem);
                // Queue new buffer for update
                ring_buffer_list->CommitUpdate(mout->channel);
            }
            else
            {
                free(mem);
                // Force disable metadata output
                uiCheckboxSetChecked(mout->enable, 0);
                onEnableMetadataOutput(mout->enable, mout);
                return PMD_FAIL;
            }
        }
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}

void
pmd_studio_audio_outputs_update_metadata_outputs(
    pmd_studio* s
    )
{
    for(unsigned int i = 0; i<s->outputs->metadata_output_count; i++)
    {
        pmd_studio_metadata_output *mout = &s->outputs->metadata_outputs[i];
        if(mout->enabled)
        {
            pmd_studio_audio_outputs_update_metadata_output(mout);
        }
    }
};

/**
 * Returns PMD_TRUE if there is at least one metadata output active
 */
dlb_pmd_bool
pmd_studio_metadata_output_active
    (pmd_studio *studio
    )
{
    for( unsigned int i=0; i<studio->outputs->metadata_output_count; i++)
    {
        if(studio->outputs->metadata_outputs[i].enabled)
        {
            return PMD_TRUE;
        }
    }
    return PMD_FALSE;
}

pmd_studio_video_frame_rate
pmd_studio_metadata_output_frame_rate
    (unsigned int channel
    ,pmd_studio *studio)
{
    for(unsigned int i = 0; i < studio->outputs->metadata_output_count; i++)
    {
        pmd_studio_metadata_output *mout = &studio->outputs->metadata_outputs[i];
        if(mout->channel == channel ||
          (!mout->subframemode && (mout->channel == channel + 1)))
        {
            return(mout->frame_rate);
        }
    }
    return(INVALID_FRAME_RATE);
}


void
pmd_studio_on_augmentor_fail_cb
    (void* data
    ,dlb_pmd_model *model
    )
{
    pmd_studio_metadata_output *mdout = (pmd_studio_metadata_output *) data;
    
    // Incase there's an augmentor error before mdout is set.
    if(mdout != nullptr)
    {
        pmd_studio_warning(model->error);
        uiMsgBoxError(mdout->outputs->studio->window, "Unable to generate sADM frame", model->error);
        mdout->augmentor_error = PMD_TRUE;  
    }
}

