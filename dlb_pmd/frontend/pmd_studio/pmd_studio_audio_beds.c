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

#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"

#include <stdio.h>

/* Definitions */

#define MAX_BED_SOURCES (16)

const float config_mix_coefs[NUM_PMD_SPEAKER_CONFIGS] = 
{
	1.0, 1.0
};


typedef struct
{
    pmd_studio_audio_beds *audio_beds;

    dlb_pmd_bool   enabled;
    dlb_pmd_bed    bed;
    dlb_pmd_source sources[MAX_BED_SOURCES];

    uiCombobox *cfg;
    uiEntry *name;
    uiCombobox *gain;
    uiCombobox *start;
    uiCheckbox *enable;

} pmd_studio_audio_bed;


struct pmd_studio_audio_beds
{
	pmd_studio *studio;

    uiWindow *window;
    pmd_studio_audio_bed beds[MAX_AUDIO_BEDS];
    unsigned int bed_count;
    uiGrid *grid;
};
    

/* Call backs */

static
void
onEnableBed
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    abed->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    pmd_studio_update_model(abed->audio_beds->studio);
}


static
void
onBedConfigUpdated
    (uiCombobox *c
    ,void *data
    )
{
    static uint8_t SPEAKER_COUNTS[NUM_PMD_SPEAKER_CONFIGS] =  {2, 3, 6, 8, 10, 12, 16, 2, 2};
    static unsigned int SPEAKER_CONFIG_CHANNELS[NUM_PMD_SPEAKER_CONFIGS][16] =
    {
        /* 2.0 */     { 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 3.0 */     { 1,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 5.1 */     { 1,  2,  3,  4,  5,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 5.1.2 */   { 1,  2,  3,  4,  5,  6, 11, 12,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 5.1.4 */   { 1,  2,  3,  4,  5,  6,  9, 10, 13, 14,  0,  0,  0,  0,  0,  0 },
        /* 7.1.4 */   { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 13, 14,  0,  0,  0,  0 },
        /* 9.1.6 */   { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16 },
        /* portable */{ 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* headphn */ { 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    };

    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    unsigned int i;
    int idx = uiComboboxSelected(c);
    
    abed->bed.config = DLB_PMD_SPEAKER_CONFIG_2_0;
    if (idx > 0)
    {
        abed->bed.config = (dlb_pmd_speaker_config)idx;
        abed->bed.num_sources = SPEAKER_COUNTS[abed->bed.config];
        /* todo: check that 1st channel + num sources fits into max signal range */
        for (i = 0; i != abed->bed.num_sources; ++i)
        {
            abed->bed.sources[i].target = SPEAKER_CONFIG_CHANNELS[abed->bed.config][i];
            abed->bed.sources[i].source = (dlb_pmd_signal)(abed->bed.sources[0].source + i);
            abed->bed.sources[i].gain = 0.0f;
        }

        pmd_studio_update_model(abed->audio_beds->studio);
    }
}


static
void
onBedStartUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    unsigned int i;

    abed->bed.sources[0].source = (dlb_pmd_signal)(1 + uiComboboxSelected(c));
    for (i = 1; i != abed->bed.num_sources; ++i)
    {
        abed->bed.sources[i].source = (dlb_pmd_signal)(abed->bed.sources[0].source + i);
    }

    pmd_studio_update_model(abed->audio_beds->studio);
}


static
void
onBedNameUpdated
    (uiEntry *e
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    snprintf(abed->bed.name, sizeof(abed->bed.name), "%s", uiEntryText(e));
    pmd_studio_update_model(abed->audio_beds->studio);
}

static
void
onBedGainUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    int i, value = uiComboboxSelected(c);
    dlb_pmd_gain newgain = (value == 63)
        ? -INFINITY
        : ((float)(62-value) * 0.5f) - 25.0f;
    for (i = 0 ; i < abed->bed.num_sources ; i++)
    {
    	abed->bed.sources[i].gain = newgain;
    }
    pmd_studio_update_model(abed->audio_beds->studio);
}


/* Private Functions */

static
void
pmd_studio_audio_bed_init
    (pmd_studio_audio_bed *abed
    )
{
    abed->enabled         = 0;
    abed->bed.config      = DLB_PMD_SPEAKER_CONFIG_2_0;
    abed->bed.bed_type    = PMD_BED_ORIGINAL;
    abed->bed.source_id   = 0;
    abed->bed.num_sources = 2;
    abed->bed.sources     = abed->sources;
    abed->bed.sources[0].target = PMD_SPEAKER_L;
    abed->bed.sources[0].source = 1;
    abed->bed.sources[0].gain   = 0.0f;
    abed->bed.sources[1].target = PMD_SPEAKER_R;
    abed->bed.sources[1].source = 2;
    abed->bed.sources[1].gain   = 0.0f;
    snprintf((char*)abed->bed.name, sizeof(abed->bed.name), "Bed %u", abed->bed.id);
}


static inline
dlb_pmd_success
add_audio_bed
    (uiGrid *gbed
    ,pmd_studio_audio_bed *abed
    ,unsigned int top
    ,pmd_studio_audio_beds *audio_beds
    )
{
    char tmp[32];
    int i;

    abed->audio_beds = audio_beds;
    abed->bed.id = (dlb_pmd_element_id)top;
    pmd_studio_audio_bed_init(abed);
    
    snprintf(tmp, sizeof(tmp), "%d", top);
    uiGridAppend(gbed, uiControl(uiNewLabel(tmp)), 0, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(abed->enable, onEnableBed, abed);
    uiGridAppend(gbed, uiControl(abed->enable), 1, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->cfg = uiNewCombobox();
    uiComboboxAppend(abed->cfg, "2.0");
    uiComboboxAppend(abed->cfg, "3.0");
    uiComboboxAppend(abed->cfg, "5.1");
    uiComboboxAppend(abed->cfg, "5.1.2");
    uiComboboxAppend(abed->cfg, "5.1.4");
    uiComboboxAppend(abed->cfg, "7.1.4");    
    uiComboboxAppend(abed->cfg, "9.1.6");
    uiComboboxSetSelected(abed->cfg, (int)abed->bed.config); 
    uiComboboxOnSelected(abed->cfg, onBedConfigUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->cfg), 2, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->start = uiNewCombobox();    
    for (i = 1; i != MAX_AUDIO_SIGNALS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i);
        uiComboboxAppend(abed->start, tmp);
    }
    uiComboboxSetSelected(abed->start, 0); 
    uiComboboxOnSelected(abed->start, onBedStartUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->start), 3, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->gain = uiNewCombobox();
    for (i = 62; i >= 0; --i)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)i * 0.5) - 25.0);
        uiComboboxAppend(abed->gain, tmp);
    }
    uiComboboxAppend(abed->gain, "-inf");

    uiComboboxSetSelected(abed->gain, 12); /* default to 0.0 dB */
    uiComboboxOnSelected(abed->gain, onBedGainUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->gain), 4, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    //uiGridAppend(gbed, uiControl(uiNewLabel("  ")), 4, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    abed->name = uiNewEntry();
    uiEntrySetText(abed->name, (const char*)abed->bed.name);
    uiEntryOnChanged(abed->name, onBedNameUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->name), 5, top, 1, 1, 1, uiAlignFill, 0, uiAlignCenter);
    audio_beds->bed_count++;
    return PMD_SUCCESS;
}

/* Public Functions */

dlb_pmd_success
pmd_studio_audio_beds_init
    (pmd_studio_audio_beds **ab1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *s
    )
{
    unsigned int i;
    uiBox *vbox;
    
    *ab1 = (pmd_studio_audio_beds *)malloc(sizeof(pmd_studio_audio_beds));

    if (!*ab1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Failed to allocate memory for beds");
    	return(PMD_FAIL);
    }

    (*ab1)->studio = s;

    (*ab1)->window = win;
    
    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Beds")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);

    (*ab1)->grid = uiNewGrid();
    uiGridSetPadded((*ab1)->grid, 1);
    uiBoxAppend(vbox, uiControl((*ab1)->grid), 1);

    add_grid_title((*ab1)->grid, "En",     1, 0);
    add_grid_title((*ab1)->grid, "Config", 2, 0);
    add_grid_title((*ab1)->grid, "Start",  3, 0);
    add_grid_title((*ab1)->grid, "Gain",   4, 0);
    add_grid_title((*ab1)->grid, "Name",   5, 0);

    (*ab1)->bed_count = 0;
    
    for (i = 0; i < INIT_AUDIO_BEDS; ++i)
    {
        if (add_audio_bed((*ab1)->grid, &((*ab1)->beds[i]), i+1, *ab1))
        {
            pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to add audio bed");
            return PMD_FAIL;
        }
    }

    uiBoxAppend(box, uiControl(vbox), 1);
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}

void
pmd_studio_audio_beds_refresh_ui
    (pmd_studio_audio_beds *abeds
    )
{
    pmd_studio_audio_bed *abed;
    unsigned int i, gain_index;
    abed = abeds->beds;
    for (i = 0; i != MAX_AUDIO_BEDS; ++i, ++abed)
    {
        uiCheckboxSetChecked (abed->enable,  abed->enabled);
        uiComboboxSetSelected(abed->cfg,     (int)abed->bed.config);
        uiComboboxSetSelected(abed->start,   abed->bed.sources[0].source - 1);
        /* Calculate bed gain index from dB float */
        if (abed->bed.sources[0].gain == -INFINITY)
        {
            uiComboboxSetSelected(abed->gain, 63);
        }
        else
        {
            gain_index = 12 - (2 * (unsigned int)(abed->bed.sources[0].gain));
            if (gain_index > 63)
            {
                gain_index = 63;
            }
            uiComboboxSetSelected(abed->gain, gain_index);
        }
        uiEntrySetText(abed->name, abed->bed.name);
    }
}

dlb_pmd_success
pmd_studio_audio_beds_import
    (pmd_studio_audio_beds *abeds
    ,dlb_pmd_model *m
    )
{
    pmd_studio_audio_bed *abed;
    dlb_pmd_bed_iterator bi;

    if (dlb_pmd_bed_iterator_init(&bi, m))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to initialize bed iterator");
        return PMD_FAIL;        
    }

    abed = abeds->beds;
    while (abed < &abeds->beds[MAX_AUDIO_BEDS])
    {
        if (dlb_pmd_bed_iterator_next(&bi, &abed->bed, MAX_BED_SOURCES, abed->sources))
        {
            break;
        }
        
        abed->enabled = 1;
        ++abed;
    }
    return PMD_SUCCESS;
}

void
pmd_studio_audio_beds_reset
    (pmd_studio_audio_beds *abeds
    )
{
    pmd_studio_audio_bed *abed;
    unsigned int i;
    
    abed = abeds->beds;
    for (i = 0; i != INIT_AUDIO_BEDS; ++i, ++abed)
    {
        pmd_studio_audio_bed_init(abed);
    }
}

void
pmd_studio_audio_beds_update_model
	(pmd_studio_audio_beds *abeds,
	 dlb_pmd_model *m
	)
{
    pmd_studio_audio_bed *abed;
    unsigned int i;

    abed = abeds->beds;
    for (i = 0; i != MAX_AUDIO_BEDS; ++i, ++abed)
    {
        if (abed->enabled)
        {
            if (dlb_pmd_set_bed(m, &abed->bed))
            {
                uiMsgBoxError(abeds->window, "error setting bed", dlb_pmd_error(m));
            }
        }
    }
}

dlb_pmd_success pmd_studio_audio_beds_get_mix_matrix(

	unsigned int id,
	dlb_pmd_speaker_config config,
	pmd_studio_mix_matrix mix_matrix,
	pmd_studio *studio)
{
	pmd_studio_audio_beds *abeds = pmd_studio_get_beds(studio);
	pmd_studio_audio_bed *abed;
	unsigned int index, i;
	unsigned int start_chan, end_chan;
    float bed_gain;

    // first search for bed as id is provided
    for (index = 0 ; index < abeds->bed_count ; index++)
    {
        if (abeds->beds[index].bed.id == id)
        {
            break;
        }
    }

	if (index >= abeds->bed_count)
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Referenced bed not found");
		return(PMD_FAIL);
	}

	abed = &abeds->beds[index];

	// Can't produced matrix unless bed is enabled
	// This shouldn't happen
	if (!abed->enabled)
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Referenced bed is not enabled");
		return(PMD_FAIL);
	}

	start_chan = uiComboboxSelected(abed->start);
	
	end_chan = start_chan + pmd_studio_speaker_config_num_channels[config]; // last channel + 1
	if (end_chan > MAX_INPUT_CHANNELS)
	{
		// The bed extends beyond the input channel array
		// Probably because of incorrect combination of start and config
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "End channel of bed extends beyond last input channel");        
		return(PMD_FAIL);
	}

	// For now only supporting 2/0, 5.1 and 5.1.4 for real-time outputs
	if ((config != DLB_PMD_SPEAKER_CONFIG_5_1_4) && (config != DLB_PMD_SPEAKER_CONFIG_5_1) && (config != DLB_PMD_SPEAKER_CONFIG_2_0))
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Unsupported bed configuration");                
		return(PMD_FAIL);
	}

    bed_gain = gain_from_db(abed->bed.sources[0].gain);

	if (config == abed->bed.config)
	{
		/* This is the simple case where the bed configuration is equal to the bed config */
		for (i = 0 ; i < pmd_studio_speaker_config_num_channels[config] ; i++)
		{
			// All the sources bed gains are the same so just use the first one
			mix_matrix[i + start_chan][i] = bed_gain;
		}
	}
	else
	{
		/* This is the downmix or upmix case. This normally gets triggered when the presentation config */
		/* does not equal the bed config, forcing an upmix or a downmix of the bed */
		/* As we are only supporting 2.0 and 5.1 for now just support stereo downmix and upmix */
		switch (config)
		{
            case DLB_PMD_SPEAKER_CONFIG_5_1:
            if (abed->bed.config != DLB_PMD_SPEAKER_CONFIG_5_1_4)
            {
                pmd_studio_error(PMD_STUDIO_ERR_UI, "Unsupported bed configuration");        
                return (PMD_FAIL);
            }
            /* Downmix from 5.1.4 to 5.1 */
            /* L=(L+(Ltf*0.707)), R=(R+(Rtf*0.707) - Reference PMD Application guide
               Ls=(Ls+(Ltr*0.707)), Rs=(Rs+(Rtr*0.707)) */

            /* Left */
            mix_matrix[start_chan][0] = 1.0f * bed_gain;
            mix_matrix[start_chan + 6][0] = PMD_STUDIO_M3DB * bed_gain;
            /* Right */
            mix_matrix[start_chan + 1][1] = 1.0f * bed_gain;
            mix_matrix[start_chan + 7][1] = PMD_STUDIO_M3DB * bed_gain;
            /* Centre */
            mix_matrix[start_chan + 2][2] = 1.0f * bed_gain;
            /* Left Surround*/
            mix_matrix[start_chan + 4][4] = 1.0f * bed_gain;
            mix_matrix[start_chan + 8][4] = PMD_STUDIO_M3DB * bed_gain;
            /* Right Surround */
            mix_matrix[start_chan + 5][5] = 1.0f * bed_gain;
            mix_matrix[start_chan + 9][5] = PMD_STUDIO_M3DB * bed_gain;
            break;

			case DLB_PMD_SPEAKER_CONFIG_2_0:
			switch (abed->bed.config)
			{
                case DLB_PMD_SPEAKER_CONFIG_5_1_4:
                /* Downmix from 5.1.4 to 2/0 - Cascade of 5.1.4 -> 5.1 -> 2/0 */
                /* Left */
                mix_matrix[start_chan][0] = 1.0f * bed_gain;
                mix_matrix[start_chan + 2][0] = PMD_STUDIO_M3DB * bed_gain;
                mix_matrix[start_chan + 4][0] = PMD_STUDIO_M3DB * bed_gain;
                mix_matrix[start_chan + 6][0] = PMD_STUDIO_M3DB * bed_gain;
                mix_matrix[start_chan + 8][0] = PMD_STUDIO_M3DB * PMD_STUDIO_M3DB * bed_gain;
                /* Right */
                mix_matrix[start_chan + 1][1] = 1.0f * bed_gain;
                mix_matrix[start_chan + 2][1] = PMD_STUDIO_M3DB * bed_gain;
                mix_matrix[start_chan + 5][1] = PMD_STUDIO_M3DB * bed_gain;
                mix_matrix[start_chan + 7][1] = PMD_STUDIO_M3DB * bed_gain;
                mix_matrix[start_chan + 9][1] = PMD_STUDIO_M3DB * PMD_STUDIO_M3DB * bed_gain;
                break;
				case DLB_PMD_SPEAKER_CONFIG_5_1:
				/* Downmix from 5.1 to 2/0, classic ITU downmix*/
                /* Left */
				mix_matrix[start_chan][0] = 1.0f * bed_gain;
				mix_matrix[start_chan + 2][0] = PMD_STUDIO_M3DB * bed_gain;
				mix_matrix[start_chan + 4][0] = PMD_STUDIO_M3DB * bed_gain;
				/* Right */
                mix_matrix[start_chan + 1][1] = 1.0f * bed_gain;
				mix_matrix[start_chan + 2][1] = PMD_STUDIO_M3DB * bed_gain;
				mix_matrix[start_chan + 5][1] = PMD_STUDIO_M3DB * bed_gain;
				break;
				default:
				// Must be downmixing from an unsupported configuration
                pmd_studio_warning("Unsupported bed configuration");        
				return (PMD_FAIL);
			}
			break;
			default:
			// Upmixing not supported
                pmd_studio_warning("Upmixing not supported");        
				return(PMD_FAIL);
		}
	}
	return(PMD_SUCCESS);
}


void
pmd_studio_audio_beds_finish
    (pmd_studio_audio_beds *abeds
    )
{
    free(abeds);
}
