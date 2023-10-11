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


#include <regex>
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
    "AM824/ST2110-31",
    "SMPTE2110-41"
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
        if (num_channels > 0)
        {
            snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel);
            uiLabelSetText(settings->output_start_channel[i], label_text);
            snprintf(label_text, MAX_LABEL_SIZE, "%u", end_channel);
            uiLabelSetText(settings->output_end_channel[i], label_text);            
        }
        else
        {
            uiLabelSetText(settings->output_start_channel[i],"");
            uiLabelSetText(settings->output_end_channel[i], "");            
        }
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
    pmd_studio_device_settings_window *settings = (pmd_studio_device_settings_window *)data;
    pmd_studio_device_disable_settings_window_updates(settings->studio);
    delete settings->input_stream_list_mutex;
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
        uiGridDelete(settings->output_stream_grid, uiControl(settings->output_stream_address[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->num_output_channel[settings->num_output_streams]));
        uiGridDelete(settings->output_stream_grid, uiControl(settings->output_start_channel[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_end_channel[settings->num_output_streams]));
		uiGridDelete(settings->output_stream_grid, uiControl(settings->output_stream_delete[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_stream_name[settings->num_output_streams]));
		uiControlDestroy(uiControl(settings->output_stream_codec[settings->num_output_streams]));
        uiControlDestroy(uiControl(settings->output_stream_address[settings->num_output_streams]));
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

    window_settings->output_stream_address[i] = uiNewEntry();
    uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_stream_address[i]), 1, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
    uiEntrySetText(window_settings->output_stream_address[i], device_settings->output_stream_address[i]);

    window_settings->output_stream_codec[i] = uiNewCombobox();
    uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_stream_codec[i]), 2, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
    for(unsigned int j = 0; j < (unsigned int)PMD_STUDIO_STREAM_CODEC::NUM_CODECS ;  j++)
    {
        uiComboboxAppend(window_settings->output_stream_codec[i], codec_labels[j]);
    }
    uiComboboxSetSelected(window_settings->output_stream_codec[i], (int)device_settings->output_stream_codec[i]);
    


    window_settings->num_output_channel[i] = uiNewEntry();
    uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->num_output_channel[i]), 4, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
    snprintf(label_text, MAX_LABEL_SIZE, "%u", device_settings->num_output_channel[i]);
    uiEntrySetText(window_settings->num_output_channel[i], label_text);
    uiEntryOnChanged(window_settings->num_output_channel[i], onNumOutputChannelsChanged, window_settings);
    if (device_settings->num_output_channel[i] > 0)
    {
        start_channel = 1;
     	for (unsigned int j = 0 ; j < i ; j++)
     	{
     		start_channel += device_settings->num_output_channel[j];
     	}
        snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel);
        window_settings->output_start_channel[i] = uiNewLabel(label_text);
        snprintf(label_text, MAX_LABEL_SIZE, "%u", start_channel + device_settings->num_output_channel[i] - 1);
    	window_settings->output_end_channel[i]   = uiNewLabel(label_text);
    }
    else
    {
        window_settings->output_start_channel[i] = uiNewLabel("");
        window_settings->output_end_channel[i]   = uiNewLabel("");
    }
   	uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_start_channel[i]), 5, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
   	uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_end_channel[i]), 7, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);
   	window_settings->output_stream_delete[i] = uiNewButton("-");
   	uiButtonOnClicked(window_settings->output_stream_delete[i], onDeleteOutputStreamButton, window_settings);
   	uiGridAppend(window_settings->output_stream_grid, uiControl(window_settings->output_stream_delete[i]), 8, i+1, 1, 1, 0, uiAlignCenter, 0, uiAlignFill);


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
        sprintf(window_settings->device_settings->output_stream_address[window_settings->num_output_streams], "239.150.150.%d", window_settings->num_output_streams + 1);
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

    text = uiEntryText(window_settings->nmos_registry);
    // Check registry URL for validity
    if (strlen(text) > 0)
    {
        std::string url_pattern = R"((https?)://(-\.)?([^\s/?\.#-]+\.?)+(/[^\s]*)?$)";
        // Construct regex object
        std::regex url_regex(url_pattern);

        // Check for match
        if (!std::regex_match(text, url_regex))
        {
            uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Registry URL must be in the form http(s)://address:port");
            return;
        }
    }

    // Check Multicast addresses for validity
    for (unsigned int i = 0 ; i < window_settings->num_output_streams ; i++)
    {
        std::string address_pattern = R"(^2(?:2[4-9]|3\d)(?:\.(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d?|0)){3}$)";
        std::regex address_regex(address_pattern);

        if (!std::regex_match(uiEntryText(window_settings->output_stream_address[i]), address_regex))
        {
            uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Output Stream multicast address is malformed");
            return;
        }
    }

    // Check PTP Domain
    if (atoi(uiEntryText(window_settings->ptp_domain)) > 127)
    {
        uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "PTP Domain is invalid");
        return;
    }

    // Stop Async callbacks
    pmd_studio_device_disable_settings_window_updates(window_settings->studio);

    // Checks have passed now start applying window settings to device settings
    // Interface
    combo_index = uiComboboxSelected(window_settings->media_interface_name);
    if ((int)window_settings->num_interface_names > combo_index)
    {
        text = &((window_settings->interface_names)[combo_index][0]);
        if (strlen(text) > 0)
        {
            strcpy(device_settings->media_interface_name, text);
        }
    }
    combo_index = uiComboboxSelected(window_settings->manage_interface_name);
    if ((int)window_settings->num_interface_names > combo_index)
    {
        text = &((window_settings->interface_names)[combo_index][0]);
        if (strlen(text) > 0)
        {
            strcpy(device_settings->manage_interface_name, text);
        }
    }

    // PTP Domain
    device_settings->ptp_domain = atoi(uiEntryText(window_settings->ptp_domain));
    // Service Types
    device_settings->enabled_services = window_settings->enabled_services;
    // NMOS registry
    text = uiEntryText(window_settings->nmos_registry);
    strncpy(device_settings->nmos_registry, text, MAX_HOSTNAME_LENGTH);

    // Input Stream name
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

    // Output Streams
    for (unsigned int i = 0 ; i < window_settings->num_output_streams ; i++)
    {
	    text = uiEntryText(window_settings->output_stream_name[i]);
	    strcpy(device_settings->output_stream_name[i], text);
        text = uiEntryText(window_settings->output_stream_address[i]);
        strcpy(device_settings->output_stream_address[i], text);
        
        int codec_index = uiComboboxSelected(window_settings->output_stream_codec[i]);
	    device_settings->output_stream_codec[i] = (PMD_STUDIO_STREAM_CODEC)codec_index;
	   	text = uiEntryText(window_settings->num_output_channel[i]);
	   	device_settings->num_output_channel[i] = atoi(text);
	}
	device_settings->num_output_streams = window_settings->num_output_streams;
	device_settings->num_output_channels = total_channel_count; // This is a helper

    result = pmd_studio_settings_update(window_settings->studio, window_settings->window);
    if (result != PMD_SUCCESS)
    {
        // If enabled service flags have been cleared, there was an error enabling the service
        if((window_settings->enabled_services & 0x7) != (device_settings->enabled_services & 0x7))
        {
            uiMsgBox(window_settings->window, "PMD Studio Warning", "Error when applying SAP/RAVENNA/NMOS settings");
            uiCheckboxSetChecked(window_settings->sap_enable, device_settings->enabled_services & AOIP_SERVICE_SAP);
            uiCheckboxSetChecked(window_settings->nmos_enable, device_settings->enabled_services & AOIP_SERVICE_NMOS);
            uiCheckboxSetChecked(window_settings->rav_enable, device_settings->enabled_services & AOIP_SERVICE_RAVENNA);
        }
        uiMsgBoxError(window_settings->window, "PMD Studio Error", "Error applying stream settings");
    }
    else
    {
        uiMsgBox(window_settings->window, "PMD Studio Settings", "Stream settings applied");
    }
}

static
void
onInputStreamChanged
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_device_settings_window *window_settings = (pmd_studio_device_settings_window *)data;
    unsigned int index = uiComboboxSelected(c);
    char label_text[MAX_LABEL_SIZE];

    // Handle the transmit only case
    if (index == 0)
    {
        snprintf(label_text, MAX_LABEL_SIZE, "0");
        uiLabelSetText(window_settings->input_stream_channel_count, label_text);
        return;      
    }
    if (index < window_settings->num_input_streams + 1)
    {
        snprintf(label_text, MAX_LABEL_SIZE, "%u", window_settings->input_streams[index -1].num_channels);
        uiLabelSetText(window_settings->input_stream_channel_count, label_text);
    }
}


static
void
onServicesChanged
    (uiCheckbox *c
    ,void *data
    )
{
    pmd_studio_device_settings_window *window_settings = (pmd_studio_device_settings_window *)data;
    // Services have changed so the input stream list needs to be refiltered based on new service selection

    pmd_studio_device_recreate_input_stream_combo_box(window_settings);
    // Enable or disable nmos registry entry box based on whether nmos is enabled or not
    if (uiCheckboxChecked(window_settings->nmos_enable))
    {
        uiControlEnable(uiControl(window_settings->nmos_registry));
    }
    else
    {
        uiControlDisable(uiControl(window_settings->nmos_registry));
    }
}

/***** Public Functions ****/

void pmd_studio_device_recreate_input_stream_combo_box(
    pmd_studio_device_settings_window *window_settings, int left, int top, int xspan, int yspan, int hexpand, uiAlign halign, int vexpand, uiAlign valign
    )
{
    pmd_studio_device_settings *device_settings = window_settings->device_settings;
    char label_text[MAX_LABEL_SIZE];
    unsigned int current_input_stream = 0;
    unsigned int num_input_channels = 0;
    unsigned int new_input_stream_index = 1; // start at 1 because we are using 0 for None    

    // Get Mutex as this can be triggered by the addition of a new service in another thread
    window_settings->input_stream_list_mutex->lock();

    window_settings->input_stream_combo_box_params[0] = left;
    window_settings->input_stream_combo_box_params[1] = top;
    window_settings->input_stream_combo_box_params[2] = xspan;
    window_settings->input_stream_combo_box_params[3] = yspan;
    window_settings->input_stream_combo_box_params[4] = hexpand;
    window_settings->input_stream_combo_box_params[5] = halign;
    window_settings->input_stream_combo_box_params[6] = vexpand;
    window_settings->input_stream_combo_box_params[7] = valign;


    // If comboBox already exists then destroy it
    if (window_settings->input_stream_name)
    {
        uiGridDelete(window_settings->input_stream_grid, uiControl(window_settings->input_stream_name));
        uiControlDestroy(uiControl(window_settings->input_stream_name));
    }
    window_settings->input_stream_name = uiNewCombobox();

    if (uiCheckboxChecked(window_settings->sap_enable))
    {
        window_settings->enabled_services |= AOIP_SERVICE_SAP;
    }
    else
    {
        window_settings->enabled_services &= ~AOIP_SERVICE_SAP;      
    }

    if (uiCheckboxChecked(window_settings->rav_enable))
    {
        window_settings->enabled_services |= AOIP_SERVICE_RAVENNA;
    }
    else
    {
        window_settings->enabled_services &= ~AOIP_SERVICE_RAVENNA;      
    }

    if (uiCheckboxChecked(window_settings->nmos_enable))
    {
        window_settings->enabled_services |= AOIP_SERVICE_NMOS;
    }
    else
    {
        window_settings->enabled_services &= ~AOIP_SERVICE_NMOS;      
    }
    window_settings->num_input_streams = pmd_studio_device_get_input_stream_names(window_settings->studio, &window_settings->input_streams, window_settings->enabled_services);
    uiComboboxAppend(window_settings->input_stream_name, "None - Transmit Only");
    for (unsigned int i = 0; i < window_settings->num_input_streams; i++)
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
        if (window_settings->input_streams[i].service == AOIP_SERVICE_NMOS)
        {
            strcat(label_text, "nmos:");         
        }
        strcat(label_text, window_settings->input_streams[i].name);
        uiComboboxAppend(window_settings->input_stream_name, label_text);
        if (!strncmp(window_settings->input_streams[i].name, device_settings->input_stream_name, MAX_STREAM_NAME_LENGTH))
        {
            current_input_stream = new_input_stream_index; 
            num_input_channels = window_settings->input_streams[i].num_channels;
        }
        new_input_stream_index++;
    }
    uiComboboxSetSelected(window_settings->input_stream_name, current_input_stream);
    uiComboboxOnSelected(window_settings->input_stream_name, onInputStreamChanged, window_settings);
    snprintf(label_text, MAX_LABEL_SIZE, "%u", num_input_channels);
    uiLabelSetText(window_settings->input_stream_channel_count, label_text);

    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->input_stream_name), left, top, xspan, yspan, hexpand, halign, vexpand, valign);

    window_settings->input_stream_list_mutex->unlock();
}

void pmd_studio_device_recreate_input_stream_combo_box(
    pmd_studio_device_settings_window *window_settings
    )
{
    pmd_studio_device_recreate_input_stream_combo_box(window_settings, window_settings->input_stream_combo_box_params[0], window_settings->input_stream_combo_box_params[1],
        window_settings->input_stream_combo_box_params[2], window_settings->input_stream_combo_box_params[3], window_settings->input_stream_combo_box_params[4],
        window_settings->input_stream_combo_box_params[5], window_settings->input_stream_combo_box_params[6], window_settings->input_stream_combo_box_params[7]);
}

void
pmd_studio_device_init_settings(
    pmd_studio_device_settings *settings
    )
{
    strcpy(settings->input_stream_name, "");
    settings->num_output_streams = 0;
    strcpy(settings->media_interface_name, "");
    strcpy(settings->manage_interface_name, "");
    settings->ptp_domain = 0;
    settings->enabled_services = AOIP_SERVICE_RAVENNA | AOIP_SERVICE_SAP | AOIP_SERVICE_NMOS;
    strcpy(settings->nmos_registry, "");
}


void
pmd_studio_device_edit_settings
(
    pmd_studio_device_settings *device_settings,
    uiWindow *win,
    pmd_studio *studio
    )
{
    uiBox *vbox, *hbox;
    unsigned int i;
    pmd_studio_device_settings_window *window_settings;
    unsigned int current_media_interface, current_manage_interface;
    char label_text[MAX_LABEL_SIZE];


    window_settings = (pmd_studio_device_settings_window *)malloc(sizeof(pmd_studio_device_settings_window));
    if (!window_settings)
    {
        return;
    }
    /* set up context */
    window_settings->window = uiNewWindow("Streams", 600, 200, 0);
    window_settings->studio = studio;
    window_settings->device_settings = device_settings;
    window_settings->input_stream_list_mutex = new std::mutex();

    uiWindowOnClosing(window_settings->window, onDeviceSettingsWindowClosing, window_settings);
    uiWindowSetMargined(window_settings->window, 1);

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    window_settings->input_stream_grid = uiNewGrid();
    uiWindowSetChild(window_settings->window, uiControl(vbox));
    uiGridSetPadded(window_settings->input_stream_grid, 1);
    uiBoxAppend(vbox, uiControl(window_settings->input_stream_grid), 0);

    window_settings->num_interface_names = pmd_studio_device_get_interface_names(studio, &window_settings->interface_names);
    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("Media Interface")), 0, 0, 1, 1, 0, uiAlignStart, 0, uiAlignFill);
    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("Management Interface")), 0, 1, 1, 1, 0, uiAlignStart, 0, uiAlignFill);
    window_settings->media_interface_name = uiNewCombobox();
    window_settings->manage_interface_name = uiNewCombobox();
    current_media_interface = 0;
    current_manage_interface = 0;
    for (i = 0; i < window_settings->num_interface_names; i++)
    {
        uiComboboxAppend(window_settings->media_interface_name, window_settings->interface_names[i]);
        uiComboboxAppend(window_settings->manage_interface_name, window_settings->interface_names[i]);
        if (!strncmp(window_settings->interface_names[i], device_settings->media_interface_name, IFNAMSIZ))
        {
            current_media_interface = i;
        }
        if (!strncmp(window_settings->interface_names[i], device_settings->manage_interface_name, IFNAMSIZ))
        {
            current_manage_interface = i;
        }
    }
    uiComboboxSetSelected(window_settings->media_interface_name, current_media_interface);
    uiComboboxSetSelected(window_settings->manage_interface_name, current_manage_interface);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->media_interface_name), 4, 0, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->manage_interface_name), 4, 1, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);

    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("PTP Clock Domain")), 0, 2, 1, 1, 1, uiAlignStart, 0, uiAlignFill);
    window_settings->ptp_domain = uiNewEntry();
    snprintf(label_text, MAX_LABEL_SIZE, "%u", device_settings->ptp_domain);
    uiEntrySetText(window_settings->ptp_domain, label_text);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->ptp_domain), 4, 2, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);


    window_settings->sap_enable = uiNewCheckbox("SAP");
    window_settings->rav_enable = uiNewCheckbox("Ravenna");
    window_settings->nmos_enable = uiNewCheckbox("NMOS");
    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("Stream Discovery/Announcement")), 0, 3, 1, 1, 1, uiAlignStart, 0, uiAlignFill);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->sap_enable), 1, 3, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->rav_enable), 2, 3, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->nmos_enable), 3, 3, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    if (device_settings->enabled_services & AOIP_SERVICE_SAP)
    {
        uiCheckboxSetChecked(window_settings->sap_enable, 1);
    }
    if (device_settings->enabled_services & AOIP_SERVICE_RAVENNA)
    {
        uiCheckboxSetChecked(window_settings->rav_enable, 1);
    }
    if (device_settings->enabled_services & AOIP_SERVICE_NMOS)
    {
        uiCheckboxSetChecked(window_settings->nmos_enable, 1);
    }
    uiCheckboxOnToggled(window_settings->sap_enable, onServicesChanged, window_settings);
    uiCheckboxOnToggled(window_settings->rav_enable, onServicesChanged, window_settings);
    uiCheckboxOnToggled(window_settings->nmos_enable, onServicesChanged, window_settings);

    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("NMOS Registry")), 0, 4, 1, 1, 1, uiAlignStart, 0, uiAlignFill);
    window_settings->nmos_registry = uiNewEntry();
    uiEntrySetText(window_settings->nmos_registry, device_settings->nmos_registry);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->nmos_registry), 2, 4, 3, 1, 1, uiAlignFill, 0, uiAlignFill);

    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("Input Stream Name")), 0, 5, 1, 1, 1, uiAlignStart, 0, uiAlignFill);

    window_settings->input_stream_name = NULL;
    window_settings->input_stream_channel_count = uiNewLabel("");
    pmd_studio_device_recreate_input_stream_combo_box(window_settings, 4, 5, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);

    uiGridAppend(window_settings->input_stream_grid, uiControl(uiNewLabel("Number of Input Channels")), 0, 6, 1, 1, 1, uiAlignStart, 0, uiAlignFill);
    uiGridAppend(window_settings->input_stream_grid, uiControl(window_settings->input_stream_channel_count), 4, 6, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);

    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    hbox = uiNewHorizontalBox();
    uiBoxSetPadded(hbox, 1);
    window_settings->add_output_stream_button = uiNewButton("+");
    uiButtonOnClicked(window_settings->add_output_stream_button, onAddOutputStreamButton, window_settings);
    uiBoxAppend(hbox, uiControl(window_settings->add_output_stream_button), 0);
	uiBoxAppend(hbox, uiControl(uiNewLabel("Output Streams")), 0);
    uiBoxAppend(vbox, uiControl(hbox), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);

    // new grid for output streams
    window_settings->output_stream_grid = uiNewGrid();

    // Headings row
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Name")), 0, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Address")), 1, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Codec")), 2, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Channels")), 4, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("Start")), 5, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("   ")), 6, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
    uiGridAppend(window_settings->output_stream_grid, uiControl(uiNewLabel("End")), 7, 0, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);


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

    // Enable Callbacks from other threads
    pmd_studio_device_enable_settings_window_updates(window_settings->studio, window_settings);

    uiControlShow(uiControl(window_settings->window));
}
