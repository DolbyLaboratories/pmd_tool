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

#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_common_defs.h"
#include "pmd_studio.h"
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_audio_beds_pvt.h"
#include "pmd_studio_settings_pvt.h"
#include <stdio.h>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

using namespace std;

static
dlb_pmd_success
add_audio_bed
    (pmd_studio_audio_beds *audio_beds
    );

/* Call backs */

inline
void
onEnableBed
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    abed->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);

    pmd_studio_presentations_bed_enable(abed->bed.id, abed->enabled, abed->audio_beds->studio);
    pmd_studio_update_model(abed->audio_beds->studio);
}


inline
void
onBedConfigUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    unsigned int i;
    int idx = uiComboboxSelected(c);
    pmd_studio_config *configs;

    get_pmd_studio_configs(abed->audio_beds->studio, &configs);
    abed->bed.config = DLB_PMD_SPEAKER_CONFIG_2_0;
    if (idx >= 0)
    {
        abed->bed.config = (dlb_pmd_speaker_config)configs[idx].config;
        abed->bed.num_sources = configs[idx].num_channels;   
        float gain_val = pmd_studio_combobox_index_to_gaindb(uiComboboxSelected(abed->gain));
        /* todo: check that 1st channel + num sources fits into max signal range */
        for (i = 0; i != abed->bed.num_sources; ++i)
        {
            abed->bed.sources[i].target = (dlb_pmd_speaker) (*(configs[idx].config_channels))[i];
            abed->bed.sources[i].source = (dlb_pmd_signal)(abed->bed.sources[0].source + i);
            abed->bed.sources[i].gain = gain_val;
        }

        pmd_studio_update_model(abed->audio_beds->studio);
    }
}


inline
void
onBedStartUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    unsigned int i;
    float gain_val = pmd_studio_combobox_index_to_gaindb(uiComboboxSelected(abed->gain));

    abed->bed.sources[0].source = (dlb_pmd_signal)(1 + uiComboboxSelected(c));
    abed->bed.sources[0].gain = gain_val;
    for (i = 1; i != abed->bed.num_sources; ++i)
    {
        abed->bed.sources[i].source = (dlb_pmd_signal)(abed->bed.sources[0].source + i);
        abed->bed.sources[i].gain = gain_val;
    }

    pmd_studio_update_model(abed->audio_beds->studio);
}

inline
void
updateStudioAbedNameFromUi
    (pmd_studio_audio_bed *abed
    )
{
    BedClassifier classifier = static_cast<BedClassifier>(uiComboboxSelected(abed->classifier));
    std::string pmdBedLabel = generatePMDBedLabel(uiEntryText(abed->name), classifier);
    snprintf(abed->bed.name, sizeof(abed->bed.name), "%s", pmdBedLabel.c_str());
    pmd_studio_update_model(abed->audio_beds->studio);
}

inline
void
updateUiFromStudioAbedName
    (pmd_studio_audio_bed *abed
    )
{
    auto [classifier, label] = parsePMDBedLabel(abed->bed.name);
    uiComboboxSetSelected(abed->classifier, static_cast<int>(classifier));
    uiEntrySetText(abed->name, label.c_str());
}

inline
void
onBedNameUpdated
    (uiEntry *e
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    updateStudioAbedNameFromUi(abed);
}

inline
void
onBedClassUpdated
    (uiCombobox *cb
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *) data;
    updateStudioAbedNameFromUi(abed);
}

inline
void
onBedGainUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    int i, value = uiComboboxSelected(c);
    dlb_pmd_gain newgain = pmd_studio_combobox_index_to_gaindb(value);
    for (i = 0 ; i < abed->bed.num_sources ; i++)
    {
    	abed->bed.sources[i].gain = newgain;
    }
    pmd_studio_update_model(abed->audio_beds->studio);
}

static 
void 
onAddAudioBedButtonClicked
    (uiButton *button 
    ,void *data
    )
{
    pmd_studio_audio_beds *abeds = (pmd_studio_audio_beds *)data;

    (void)button;

#ifdef LIMITED_MODE 
    pmd_studio_settings *settings = pmd_studio_get_settings(abeds->studio);
    if(abeds->bed_count >= (settings->limited_mode ? 1 : MAX_AUDIO_BEDS))
    {
        uiMsgBoxError(abeds->window, "error setting object", settings->limited_mode ? "Max beds reached. To add more, disable limited mode in settings" : "max beds reached");
#else
    if(abeds->bed_count == MAX_AUDIO_BEDS)
    {
        uiMsgBoxError(abeds->window, "error setting object", "max beds reached");
#endif
    }
    else 
    {
        add_audio_bed(abeds);
        pmd_studio_update_model(abeds->studio);
    }
}

/* Private Functions */

static
dlb_pmd_success
pmd_studio_audio_bed_init
    (pmd_studio_audio_bed *abed,
     pmd_studio *studio
    )
{
    pmd_studio_audio_beds *beds;

    abed->enabled         = 1;
    beds = pmd_studio_get_beds(studio);
    if(pmd_studio_eid_get_next(studio, &abed->bed.id))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to get eid for new bed");
        return(PMD_FAIL);        
    }
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
    snprintf((char*)abed->bed.name, sizeof(abed->bed.name), "Bed %u", beds->bed_count + 1);
    return(PMD_SUCCESS);
}


static
dlb_pmd_success
add_audio_bed
    (pmd_studio_audio_beds *abeds
    )
{
    char tmp[32];
    unsigned int i, row;
    pmd_studio_audio_bed *abed;
    pmd_studio_config *configs;
    unsigned int num_configs = get_pmd_studio_configs(abeds->studio, &configs);

#ifdef LIMITED_MODE
    pmd_studio_settings *settings = pmd_studio_get_settings(abeds->studio);
    if (abeds->bed_count == (settings->limited_mode ? 1 : MAX_AUDIO_BEDS))
#else
    if (abeds->bed_count == MAX_AUDIO_BEDS)
#endif
    {
        return(PMD_FAIL);
    }

    abed = &(abeds->beds[abeds->bed_count]);

    abed->audio_beds = abeds;
    pmd_studio_audio_bed_init(abed, abeds->studio);
    abeds->bed_count++;
    row = abeds->bed_count;

    snprintf(tmp, sizeof(tmp), "%d", row);
    abed->label = uiNewLabel(tmp);

    abed->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(abed->enable, onEnableBed, abed);
    uiCheckboxSetChecked(abed->enable, abed->enabled);

    abed->classifier = uiNewCombobox();

    for(int i=0; i<static_cast<int>(BedClassifier::LAST); i++)
    {
        if(i == static_cast<int>(BedClassifier::DEFAULT))
        {
            uiComboboxAppend(abed->classifier, "Default");
        } 
        else 
        {
            // Search for description definition
            auto c = static_cast<BedClassifier>(i);
            for(auto [classifier, tag, description] : BED_CLASSIFIER_TAG_MAP)
            {
                if(classifier == c)
                {
                    // Found description.
                    uiComboboxAppend(abed->classifier, description.c_str());
                    break;
                }
            }
        }
    }

    uiComboboxOnSelected(abed->classifier, onBedClassUpdated, abed);

    abed->name = uiNewEntry();

    // With classifier and name initialized, now parse bed label and update.
    updateUiFromStudioAbedName(abed);

    abed->cfg = uiNewCombobox();
    for (i = 0 ; i < num_configs ; i++)
    {
        uiComboboxAppend(abed->cfg, configs[i].config_string);
    }

    uiComboboxOnSelected(abed->cfg, onBedConfigUpdated, abed);

    abed->start = uiNewCombobox();    
    for (i = 1; i != MAX_STUDIO_AUDIO_SIGNALS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i);
        uiComboboxAppend(abed->start, tmp);
    }
    uiComboboxSetSelected(abed->start, 0); 
    uiComboboxOnSelected(abed->start, onBedStartUpdated, abed);

    abed->gain = uiNewCombobox();
    for (i = 0; i < 63; i++)
    {
        snprintf(tmp, sizeof(tmp), "%0.1f", ((double)(62 - i) * 0.5) - 25.0);
        uiComboboxAppend(abed->gain, tmp);
    }

#ifdef LIMITED_MODE
    if(settings->limited_mode == PMD_FALSE)
    {
        uiComboboxAppend(abed->gain, "-inf");
    }
#else  
    uiComboboxAppend(abed->gain, "-inf");       
#endif

    uiComboboxSetSelected(abed->gain, 12); /* default to 0.0 dB */
    uiComboboxOnSelected(abed->gain, onBedGainUpdated, abed);

    //uiGridAppend(gbed, uiControl(uiNewLabel("  ")), 4, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    uiEntryOnChanged(abed->name, onBedNameUpdated, abed);

    i=0;
    uiGridAppend(abeds->grid, uiControl(abed->label),       i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(abeds->grid, uiControl(abed->enable),      i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(abeds->grid, uiControl(abed->classifier),  i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(abeds->grid, uiControl(abed->cfg),         i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(abeds->grid, uiControl(abed->start),       i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
    uiGridAppend(abeds->grid, uiControl(abed->gain),        i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignFill);    
    uiGridAppend(abeds->grid, uiControl(abed->name),        i++, row, 1, 1, 1, uiAlignFill, 1, uiAlignCenter);

    // Add bed to presentations bed comboboxes if intialized enabled
    if(abed->enabled) pmd_studio_presentations_bed_enable(abed->bed.id, PMD_TRUE, abeds->studio);

    for (i = 0 ; i < num_configs ; i++)
    {
        if(abed->bed.config == configs[i].config)
        {
            uiComboboxSetSelected(abed->cfg, i); 
        }
    }

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
    (*ab1)->add_bed_button = uiNewButton("Add Audio Bed");
    uiBoxAppend(vbox, uiControl((*ab1)->add_bed_button), 0);
    uiButtonOnClicked((*ab1)->add_bed_button, onAddAudioBedButtonClicked, *ab1);

    (*ab1)->grid = uiNewGrid();
    uiGridSetPadded((*ab1)->grid, 1);
    uiBoxAppend(vbox, uiControl((*ab1)->grid), 0);

    int i=1;
    uiGridAppend((*ab1)->grid, uiControl(uiNewLabel("En")),     i++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ab1)->grid, uiControl(uiNewLabel("Class")),  i++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ab1)->grid, uiControl(uiNewLabel("Config")), i++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ab1)->grid, uiControl(uiNewLabel("Start")),  i++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ab1)->grid, uiControl(uiNewLabel("Gain")),   i++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend((*ab1)->grid, uiControl(uiNewLabel("Name")),   i++, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

    (*ab1)->bed_count = 0;
    
    uiBoxAppend(box, uiControl(vbox), 0);
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}

void
pmd_studio_audio_beds_refresh_ui
    (pmd_studio_audio_beds *abeds
    )
{
    pmd_studio_audio_bed *abed;
    unsigned int i, j, gain_index;
    abed = abeds->beds;

    pmd_studio_config *configs;
    unsigned int nconfigs = get_pmd_studio_configs(abeds->studio, &configs);

    for (i = 0; i != abeds->bed_count ; ++i, ++abed)
    {
        for(j=0; j < nconfigs; j++)
        {
            if(abed->bed.config == configs[j].config)
            {
                uiComboboxSetSelected(abed->cfg, j);
                break;
            }
        }

        uiCheckboxSetChecked (abed->enable,  abed->enabled);
        uiComboboxSetSelected(abed->start,   abed->bed.sources[0].source - 1);
        /* Calculate bed gain index from dB float */
#ifdef LIMITED_MODE
        pmd_studio_settings *settings = pmd_studio_get_settings(abeds->studio);
        unsigned int index_limit = settings->limited_mode ? 62 : 63;
#else
        unsigned int index_limit = 63;
#endif
        if (abed->bed.sources[0].gain == -INFINITY)
        {
            uiComboboxSetSelected(abed->gain, index_limit);
        }
        else
        {
            gain_index = 12 - (2 * (unsigned int)(abed->bed.sources[0].gain));
            if (gain_index > index_limit)
            {
                gain_index = index_limit;
            }
            uiComboboxSetSelected(abed->gain, gain_index);
        }
        updateUiFromStudioAbedName(abed);
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
    unsigned int new_bed_count = dlb_pmd_num_beds(m);
    unsigned int i;


    if (dlb_pmd_bed_iterator_init(&bi, m))
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Failed to initialize bed iterator");
        return PMD_FAIL;        
    }

    /* Create missing beds required for import */
    if (new_bed_count > abeds->bed_count)
    {
        for (i = abeds->bed_count ; i < new_bed_count ; i++)
        {
            add_audio_bed(abeds);
        }
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
    unsigned int i;

 // Pull down user interface
    for (i = 0 ; i < abeds->bed_count ; i++)
    {
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].label));
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].cfg));
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].name));
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].gain));
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].start));
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].enable));
        uiGridDelete(abeds->grid, uiControl(abeds->beds[i].classifier));
        uiControlDestroy(uiControl(abeds->beds[i].label));
        uiControlDestroy(uiControl(abeds->beds[i].cfg));
        uiControlDestroy(uiControl(abeds->beds[i].name));
        uiControlDestroy(uiControl(abeds->beds[i].gain));
        uiControlDestroy(uiControl(abeds->beds[i].start));
        uiControlDestroy(uiControl(abeds->beds[i].enable));
        uiControlDestroy(uiControl(abeds->beds[i].classifier));
    }
    abeds->bed_count = 0;
}

void
pmd_studio_audio_beds_enable
    (pmd_studio_audio_beds *abeds
    )
{
    uiControlEnable(uiControl(abeds->add_bed_button));
    for (unsigned int i = 0; i < abeds->bed_count; i++)
    {
        uiControlEnable(uiControl(abeds->beds[i].cfg));
        uiControlEnable(uiControl(abeds->beds[i].gain));
        uiControlEnable(uiControl(abeds->beds[i].start));
        uiControlEnable(uiControl(abeds->beds[i].enable));
        uiControlEnable(uiControl(abeds->beds[i].name));
        uiControlEnable(uiControl(abeds->beds[i].classifier));
    }

}


void
pmd_studio_audio_beds_disable
    (pmd_studio_audio_beds *abeds,
    bool live_mode
    )
{
    uiControlDisable(uiControl(abeds->add_bed_button));
    for (unsigned int i = 0; i < abeds->bed_count; i++)
    {
        uiControlDisable(uiControl(abeds->beds[i].cfg));
        uiControlDisable(uiControl(abeds->beds[i].start));
        uiControlDisable(uiControl(abeds->beds[i].enable));
        uiControlDisable(uiControl(abeds->beds[i].name));
        uiControlDisable(uiControl(abeds->beds[i].classifier));
        if(!live_mode)
        {
            uiControlDisable(uiControl(abeds->beds[i].gain));
        }
        else
        {
            uiControlEnable(uiControl(abeds->beds[i].gain));
        }
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
    for (i = 0; i < abeds->bed_count; ++i, ++abed)
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
    pmd_studio_config *output_config_info = get_pmd_studio_config_info(studio, config);
    pmd_studio_config *bed_config_info;

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

    bed_config_info = get_pmd_studio_config_info(studio, abed->bed.config);

	// Can't produced matrix unless bed is enabled
	// This shouldn't happen
	if (!abed->enabled)
	{
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Referenced bed is not enabled");
		return(PMD_FAIL);
	}

	start_chan = uiComboboxSelected(abed->start);
	
	end_chan = start_chan + output_config_info->num_channels; // last channel + 1
	if (end_chan > MAX_INPUT_CHANNELS)
	{
		// The bed extends beyond the input channel array
		// Probably because of incorrect combination of start and config
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "End channel of bed extends beyond last input channel");        
		return(PMD_FAIL);
	}

    bed_gain = gain_from_db(abed->bed.sources[0].gain);

    // The test below relies on the fact that the DLB_PMD_SEAKER Enums are ordered in such a way that 
    // the channels for the larger number configs are always and extension of the lower ones so that
    // A simple bed gain across all the bed channels is possible
	if (config >= abed->bed.config)
	{
		/* For all channels in the bed */
		for (i = 0 ; i < bed_config_info->num_channels ; i++)
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
            /* LFE */
            mix_matrix[start_chan + 3][3] = 1.0f * bed_gain;
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


unsigned int 
pmd_studio_audio_beds_get_eids(
    dlb_pmd_element_id **eid_list,
    unsigned int **bed_labels,
    pmd_studio *s,
    bool enabled_only
    )
{
    // Refresh list
    unsigned int i,num_enabled;
    pmd_studio_audio_beds *abeds = pmd_studio_get_beds(s);

    num_enabled = 0;
    for (i = 0 ; i < abeds->bed_count ; i++)
    {
        // Indicate enabled beds with non-zero eid
        if (abeds->beds[i].enabled || !enabled_only)
        {
            abeds->bed_eids[num_enabled] = abeds->beds[i].bed.id;
            abeds->bed_labels[num_enabled++] = i + 1;
        }
    }
    *eid_list = abeds->bed_eids;
    *bed_labels = abeds->bed_labels;

    return(num_enabled);
}

void
pmd_studio_audio_beds_print_debug
    (pmd_studio *s
    )
{
    pmd_studio_audio_beds *abeds = pmd_studio_get_beds(s);
    unsigned int i;
    pmd_studio_config *bed_config_info;

    printf("---****Beds****----\n");
    printf("Number of Beds: %u\n", abeds->bed_count);
    for (i = 0 ; i < abeds->bed_count ; i++)
    {
        printf("Bed #%u:\n", i);
        if (abeds->beds[i].enabled)
        {
            printf("\tBed is enabled\n" );
        }
        else
        {
            printf("\tBed is disabled\n" );
        }
        printf("\tID: %u", abeds->beds[i].bed.id);
        printf("\tNames: %s\n", abeds->beds[i].bed.name);
        bed_config_info = get_pmd_studio_config_info(s, abeds->beds[i].bed.config);
        printf("\tConfiguration: ");
        printf("\t%s\n", bed_config_info->config_string);
    }
    printf("\n");
}


void
pmd_studio_audio_beds_finish
    (pmd_studio_audio_beds *abeds
    )
{
    free(abeds);
}

dlb_pmd_speaker_config
pmd_studio_audio_beds_get_bed_config
    (pmd_studio *studio
    ,dlb_pmd_element_id eid
    )
{
    unsigned int i;

    pmd_studio_audio_beds *abeds = pmd_studio_get_beds(studio);
    for( i = 0; i < abeds->bed_count; i++)
    {
        if(abeds->beds[i].bed.id == eid)
        {
            return abeds->beds[i].bed.config;
        }
    }
    // Default - return default config value
    return DLB_PMD_SPEAKER_CONFIG_2_0;
}

dlb_pmd_success 
pmd_studio_set_bed_gain
    (pmd_studio *studio
    ,dlb_pmd_element_id eid
    ,float gain_db
    )
{
    pmd_studio_audio_beds *beds = pmd_studio_get_beds(studio);
    pmd_studio_audio_bed *bed;
    unsigned int i, combo_index;

    for( i = 0; i<beds->bed_count; i++)
    {   
        bed = &beds->beds[i];
        if(bed->bed.id == eid)
        {
            combo_index = pmd_studio_gaindb_to_combobox_index(gain_db);
#ifdef LIMITED_MODE
            pmd_studio_settings *settings = pmd_studio_get_settings(studio);
            if(settings->limited_mode)
            {
                combo_index = combo_index > 62 ? 62 : combo_index;
            }
#endif
            uiComboboxSetSelected(bed->gain, combo_index);
            onBedGainUpdated(bed->gain, bed);
            return PMD_SUCCESS;
        }
    }
    return PMD_FAIL;
}


// Scans for and parses bed classifier strings in label
std::tuple<BedClassifier, std::string> 
parsePMDBedLabel
    (std::string label
    )
{
    for(auto [classifier, tag, description] : BED_CLASSIFIER_TAG_MAP)
    {
        if(tag.size() > label.size()) continue;
        if(boost::algorithm::ends_with(label, tag))
        {
            std::string text = label.substr(0, label.size() - tag.size());
            return{classifier, text};
        }
    }
    // Default bed classifier is first defined classifier
    return {BedClassifier::DEFAULT, label};
}

// Generates PMD label with bed classifier string
std::string 
generatePMDBedLabel
    (std::string label
    ,BedClassifier bedClass
    )
{
    if(bedClass == BedClassifier::DEFAULT) return label;

    std::stringstream ss;
    ss << label;
    for(auto [classifier, tag, description] : BED_CLASSIFIER_TAG_MAP)
    {
        if(classifier == bedClass)
        {
            ss << tag;
            break;
        }
    }
    return ss.str();
}
