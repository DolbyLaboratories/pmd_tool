/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#ifndef __PMD_STUDIO_AUDIO_OUTPUTS_PVT_H__
#define __PMD_STUDIO_AUDIO_OUTPUTS_PVT_H__

#include "pmd_studio_audio_outputs.h"
#include "pmd_studio_device.h"

/* Definitons */

static const dlb_pmd_speaker_config pmd_studio_output_config[PMD_STUDIO_OUTPUT_NUM_CONFIG] =
{
    DLB_PMD_SPEAKER_CONFIG_2_0,
    DLB_PMD_SPEAKER_CONFIG_5_1,
    DLB_PMD_SPEAKER_CONFIG_5_1_4
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
    unsigned int config_index;
    unsigned int presentation_id; //  Currently selected ids
    unsigned int startchannel; // Starts at 1, 0 is unassigned
    unsigned int endchannel; // Starts at 1, 0 is unassigned
    uiLabel    *label;
    uiCheckbox *enable;
    uiCombobox *cfg;
    uiCombobox *pres;
    uiCombobox *schan;
    uiLabel *echan;
} pmd_studio_audio_output;


struct pmd_studio_metadata_output
{
    pmd_studio_outputs *outputs;
    dlb_pmd_bool enabled;
    dlb_pmd_bool smpte337_wrapped;
    unsigned int id;
    pmd_studio_metadata_format format;
    dlb_pmd_bool subframemode;
    unsigned int channel; // Starts at 1, 0 is unassigned
    pmd_studio_video_frame_rate frame_rate;
    uiLabel    *label;
    uiCheckbox *enable;
    uiCombobox *fmt;
    uiCombobox *fps;
    uiCombobox *mode;
    uiCombobox *chan;
    dlb_pmd_bool augmentor_error;
};

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
    uiButton *add_aout_button;
    uiButton *add_mdout_button;

    /* shortcuts for presentation informtation between updates */
    unsigned int num_presentations;
    dlb_pmd_element_id (*presentation_ids)[MAX_AUDIO_PRESENTATIONS];
    char* (*presentation_names)[MAX_AUDIO_PRESENTATIONS];

    /* channel based elements */
    unsigned int num_chan_allocated;
    unsigned int chan_allocated[MAX_OUTPUT_CHANNELS]; /* 0 for unallocated, use output/metadata id to indicate allocation */
};

/**
 * Callback for when dlb_pmd augmentor fails.
 */
void
pmd_studio_on_augmentor_fail_cb
    (void* data
    ,dlb_pmd_model *model
    );

#endif