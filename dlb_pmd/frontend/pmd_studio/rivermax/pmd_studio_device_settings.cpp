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
#include <arpa/inet.h>
extern "C"{
#include "ui.h"
}
#include "dlb_pmd_api.h"
#include "pmd_studio.h"
#include "pmd_studio_device.h"
#include "pmd_studio_settings.h"
#include "pmd_studio_device_pvt.h"


const char codec_labels[(int)PMD_STUDIO_STREAM_CODEC::NUM_CODECS][MAX_LABEL_SIZE] = 
{
    "AES67/ST2110-30 L16",
    "AES67/ST2110-30 L24",
    "AM824/ST2110-31"
};


static
void
onNumOutputChannelsChanged
    (uiEntry *e
    ,void *data
    )
{
	pmd_studio_device_settings_window *settings = (pmd_studio_device_settings_window *)data;
	unsigned int start_channel = 1;
	unsigned int num_channels, end_channel;
	char label_text[MAX_LABEL_SIZE];
	(void)e;
	char *entry_text;

	/* update channel labels to reflect changed output channel number */
	/* It is not known which changed but simpler to just update all of them */
	for (unsigned int i = 0 ; i < settings->num_output_streams ; i++)
	{
		entry_text = uiEntryText(settings->num_output_channel[i]);
		// If a number of channels entry is empty then stop as we can't calculate
		// channel labels anymore
		if (strlen(entry_text) == 0)
		{
			break;
		}
		num_channels = atoi(entry_text);
		end_channel = start_channel + num_channels - 1;
		snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel);
		uiLabelSetText(settings->output_start_channel[i], label_text);
		snprintf(label_text, MAX_LABEL_SIZE, "%u", end_channel);
		uiLabelSetText(settings->output_end_channel[i], label_text);
		start_channel = end_channel + 1;
	}
}

static
int
onDeviceSettingsWindowClosing
    (uiWindow *w
    ,void *data
    )
{
    free(data);
    uiControlDestroy(uiControl(w));
    return(0);   
}


static
void
onDeleteOutputStreamButton
    (uiButton *c
    ,void *data
    )
{
	pmd_studio_device_settings_window *settings = (pmd_studio_device_settings_window *)data;
	char label_text[MAX_LABEL_SIZE];
	unsigned int start_channel;

	// First find out the index of the button pressed
	// set to invalid value
	unsigned int button_index = settings->num_output_streams;
	for (unsigned int i = 0 ; i < settings->num_output_streams ; i++)
	{
		if (settings->output_stream_delete[i] == c)
		{
			button_index = i;
			break;
		}
	}
	if (button_index < settings->num_output_streams)
	{
		// Set deleted line to the one below except for last line
		for (unsigned int i = button_index ; i < settings->num_output_streams - 1 ; i++)
		{
			uiEntrySetText(settings->output_stream_name[i], uiEntryText(settings->output_stream_name[i + 1]));
    		uiComboboxSetSelected(settings->output_stream_codec[i], uiComboboxSelected(settings->output_stream_codec[i + 1]));
    		uiEntrySetText(settings->num_output_channel[i], uiEntryText(settings->num_output_channel[i + 1]));
    		if (i == 0)
            {
                start_channel = 1;
            }
            else
    		{
    			start_channel = atoi(uiLabelText(settings->output_end_channel[i - 1])) + 1;
    			snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel);
    			uiLabelSetText(settings->output_start_channel[i], label_text);
    		}
    		snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel + atoi(uiEntryText(settings->num_output_channel[i + 1])) - 1);
    		uiLabelSetText(settings->output_end_channel[i], label_text);
		}
		// delete the last line
		settings->num_output_streams--;
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_stream_name[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_stream_codec[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->num_output_channel[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_start_channel[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_end_channel[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_stream_delete[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_stream_name[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_stream_codec[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->num_output_channel[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_start_channel[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_end_channel[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_stream_delete[settings->num_output_streams]));
	}
}

static
void
add_output_stream(
	pmd_studio_device_settings_window *window_settings,
	unsigned int i
	)
{
    char label_text[MAX_LABEL_SIZE];
    unsigned int start_channel;
    pmd_studio_device_settings *device_settings = window_settings->device_settings;

    window_settings->output_stream_name[i] = uiNewEntry();
    uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_stream_name[i]), 0, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
    uiEntrySetText(window_settings->output_stream_name[i], device_settings->output_stream_name[i]);

    window_settings->output_stream_codec[i] = uiNewCombobox();
    uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_stream_codec[i]), 1, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
    for(unsigned int j = 0; j < (unsigned int)PMD_STUDIO_STREAM_CODEC::NUM_CODECS ;  j++)
    {
        uiComboboxAppend(window_settings->output_stream_codec[i], codec_labels[j]);
    }
    uiComboboxSetSelected(window_settings->output_stream_codec[i], (int)device_settings->output_stream_codec[i]);
    window_settings->num_output_channel[i] = uiNewEntry();
    uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->num_output_channel[i]), 2, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
    snprintf(label_text, MAX_LABEL_SIZE, "%u", device_settings->num_output_channel[i]);
    uiEntrySetText(window_settings->num_output_channel[i], label_text);
    uiEntryOnChanged(window_settings->num_output_channel[i], onNumOutputChannelsChanged, window_settings);
    start_channel = 1;
 	for (unsigned int j = 0 ; j < i ; j++)
 	{
 		start_channel += device_settings->num_output_channel[j];
 	}
    snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel);
    window_settings->output_start_channel[i] = uiNewLabel(label_text);
    snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel + device_settings->num_output_channel[i] - 1);
	window_settings->output_end_channel[i]   = uiNewLabel(label_text);
   	uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_start_channel[i]), 3, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
   	uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_end_channel[i]), 5, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
   	window_settings->output_stream_delete[i] = uiNewButton("-");
   	uiButtonOnClicked(window_settings->output_stream_delete[i], onDeleteOutputStreamButton, window_settings);
   	uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_stream_delete[i]), 6, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
}



static
void
onAddOutputStreamButton
    (uiButton *c
    ,void *data
    )
{
	pmd_studio_device_settings_window *window_settings = (pmd_studio_device_settings_window *)data;
	if (window_settings->num_output_streams == MAX_NUM_OUTPUT_STREAMS)
	{
		uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Maximum number of output streams reached");
	}
	else
	{
		strcpy(window_settings->device_settings->output_stream_name[window_settings->num_output_streams],"");
		window_settings->device_settings->output_stream_codec[window_settings->num_output_streams] = PMD_STUDIO_STREAM_CODEC::AES67_L16;
		window_settings->device_settings->num_output_channel[window_settings->num_output_streams] = 0;
		add_output_stream(window_settings, window_settings->num_output_streams);
        window_settings->num_output_streams = window_settings->num_output_streams + 1;
	}
}

static
void
onStreamsApplyButtonClicked
    (uiButton *c
    ,void *data
    )
{
	pmd_studio_device_settings_window *window_settings = (pmd_studio_device_settings_window *)data;
    pmd_studio_device_settings *device_settings = window_settings->device_settings;

    dlb_pmd_success result;
    char *text;
    int combo_index;
	unsigned int total_channel_count = 0, num_channels;

    combo_index = uiComboboxSelected(window_settings->interface_name);
    if ((int)window_settings->num_interface_names > combo_index)
    {
        text = &((window_settings->interface_names)[combo_index][0]);
        if (strlen(text) > 0)
        {
            strcpy(device_settings->interface_name, text);
        }
    }

	// Do all the checks first before application then apply without interuption
	// This avoid partial settings application
    for (unsigned int i = 0 ; i < window_settings->num_output_streams ; i++)
    {
	    text = uiEntryText(window_settings->output_stream_name[i]);
	    if (strlen(text) == 0)
	    {
	        uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Invalid output Stream Name");
	        return;
	    }
	    num_channels = atoi(uiEntryText(window_settings->num_output_channel[i]));
	    if ((num_channels == 0) || (num_channels > MAX_OUTPUT_CHANNELS))
	    {
	        uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Invalid output stream channel count");
	        return;	    	
	    }
	    total_channel_count += num_channels;
	    if (total_channel_count > MAX_OUTPUT_CHANNELS)
	    {
	        uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Total channel count exceeds maximum allowed");
	        return; 	
	    }
	}

    combo_index = uiComboboxSelected(window_settings->input_stream_name);
    // Check for none case
    if (combo_index == 0)
    {
    	strcpy(device_settings->input_stream_name, "");
    }
    else if ((combo_index - 1) < (int)window_settings->num_input_streams)
    {
        text = &((window_settings->input_streams)[combo_index - 1].name[0]);
        if (strlen(text) > 0)
        {
            strcpy(device_settings->input_stream_name, text);
        }
    }

    for (unsigned int i = 0 ; i < window_settings->num_output_streams ; i++)
    {
	    text = uiEntryText(window_settings->output_stream_name[i]);
	    strcpy(device_settings->output_stream_name[i], text);
	    device_settings->output_stream_codec[i] = (PMD_STUDIO_STREAM_CODEC) uiComboboxSelected(window_settings->output_stream_codec[i]);
	   	text = uiEntryText(window_settings->num_output_channel[i]);
	   	device_settings->num_output_channel[i] = atoi(text);
	}
	device_settings->num_output_streams = window_settings->num_output_streams;
	device_settings->num_output_channels = total_channel_count; // This is a helper


    result = pmd_studio_settings_update(window_settings->studio, window_settings->window);
    if (result != PMD_SUCCESS)
    {
        uiMsgBoxError(window_settings->window, "PMD Studio Error", "Error applying stream settings");
    }
    else
    {
        uiMsgBox(window_settings->window, "PMD Studio Settings", "Stream settings applied");
    }
}

void
pmd_studio_device_init_settings(
    pmd_studio_device_settings *settings
    )
{
    strcpy(settings->input_stream_name, "");
    settings->num_output_streams = 0;
    strcpy(settings->interface_name, "");
}

void
pmd_studio_device_edit_settings
(
    pmd_studio_device_settings *device_settings,
    uiWindow *win,
    pmd_studio *studio
    )
{
    uiGrid *grid;
    uiBox *vbox;
    unsigned int i;
    pmd_studio_device_settings_window *window_settings;
    char label_text[MAX_LABEL_SIZE];
    unsigned int current_input_stream;
    unsigned int current_interface;

    window_settings = (pmd_studio_device_settings_window *)malloc(sizeof(pmd_studio_device_settings_window));
    if (!window_settings)
    {
        return;
    }
    /* set up context */
    window_settings->window = uiNewWindow("Streams", 600, 200, 0);
    window_settings->studio = studio;
    window_settings->device_settings = device_settings;

    uiWindowOnClosing(window_settings->window, onDeviceSettingsWindowClosing, window_settings);
    uiWindowSetMargined(window_settings->window, 1);

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    grid = uiNewGrid();
    uiWindowSetChild(window_settings->window, uiControl(vbox));
    uiGridSetPadded(grid, 1);
    uiBoxAppend(vbox, uiControl(grid), 0);

    window_settings->num_interface_names = pmd_studio_device_get_interface_names(studio, &window_settings->interface_names);
    uiGridAppend(grid, uiControl(uiNewLabel("Interface")), 0, 0, 1, 1, 0, uiAlignStart, 0, uiAlignFill);
    window_settings->interface_name = uiNewCombobox();
    current_interface = 0;
    for (i = 0; i < window_settings->num_interface_names; i++)
    {
        uiComboboxAppend(window_settings->interface_name, window_settings->interface_names[i]);
        if (!strncmp(window_settings->interface_names[i], device_settings->interface_name, IFNAMSIZ))
        {
            current_interface = i;
        }
    }
    uiComboboxSetSelected(window_settings->interface_name, current_interface);
    uiGridAppend(grid, uiControl(window_settings->interface_name), 1, 0, 3, 1, 1, uiAlignEnd, 0, uiAlignFill);


    window_settings->num_input_streams = pmd_studio_device_get_input_stream_names(studio, &window_settings->input_streams);
    uiGridAppend(grid, uiControl(uiNewLabel("Input Stream Name")), 0, 1, 1, 1, 0, uiAlignStart, 0, uiAlignFill);
    window_settings->input_stream_name = uiNewCombobox();
    current_input_stream = 0;
    uiComboboxAppend(window_settings->input_stream_name, "None - Transmit Only");

    for (i = 0; i < window_settings->num_input_streams; i++)
    {
    	strcpy(label_text, "");
    	if (window_settings->input_streams[i].service == AOIP_SERVICE_SAP)
    	{
    		strcat(label_text, "sap:");
    	}
    	if (window_settings->input_streams[i].service == AOIP_SERVICE_RAVENNA)
    	{
    		strcat(label_text, "rav:");    		
    	}
    	strcat(label_text, window_settings->input_streams[i].name);
        uiComboboxAppend(window_settings->input_stream_name, label_text);
        if (!strncmp(window_settings->input_streams[i].name, device_settings->input_stream_name, MAX_STREAM_NAME_LENGTH))
        {
            current_input_stream = i + 1; // +1 because we are using 0 for None
        }
    }
    uiComboboxSetSelected(window_settings->input_stream_name, current_input_stream);
    uiGridAppend(grid, uiControl(window_settings->input_stream_name), 1, 1, 3, 1, 1, uiAlignEnd, 0, uiAlignFill);

    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
	uiBoxAppend(vbox, uiControl(uiNewLabel("Output Streams")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);

    window_settings->add_output_stream_button = uiNewButton("Add Output Stream");
    uiBoxAppend(vbox, uiControl(window_settings->add_output_stream_button), 0);
    uiButtonOnClicked(window_settings->add_output_stream_button, onAddOutputStreamButton, window_settings);

    // new grid for output streams
    window_settings->output_stream_grid = uiNewGrid();

    // Headings row
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Name")), 0, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Codec")), 1, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Channels")), 2, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Start")), 3, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("   ")), 4, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("End")), 5, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);


    // Get number of output streams from settings
	window_settings->num_output_streams = device_settings->num_output_streams;
    for (i = 0 ; i < window_settings->num_output_streams; i++)
    {
    	add_output_stream(window_settings, i);
    }
    uiBoxAppend(vbox, uiControl(window_settings->output_stream_grid), 0);

    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    window_settings->applybutton = uiNewButton("Apply");
    uiBoxAppend(vbox, uiControl(window_settings->applybutton), 0);
    uiButtonOnClicked(window_settings->applybutton, onStreamsApplyButtonClicked, window_settings);

    uiControlShow(uiControl(window_settings->window));
}
