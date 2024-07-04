/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2021, Dolby Laboratories Inc.
 * Copyright (c) 2019-2021, Dolby International AB.
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
#include <string.h>
#include <arpa/inet.h>
extern "C"{
#include "ui.h"
}
#include "dlb_pmd_api.h"
#include "pmd_studio.h"
#include "pmd_studio_device.h"
#include "pmd_studio_settings.h"
#include "pmd_studio_device_pvt.h"


#define DEFAULT_MAX_CHANNELS 2


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
onAudioDeviceSettingsApplyButtonClicked
    (uiButton *c
    ,void *data
    )
{
	pmd_studio_device_settings_window *window_settings = (pmd_studio_device_settings_window *)data;
    pmd_studio_device_settings *device_settings = window_settings->device_settings;

    dlb_pmd_success result;
    char *text;
    int combo_index;
    char *indevname, *outdevname;
    int channels; // Because PortAudio uses int
    char label[MAX_LABEL_SIZE];

    (void)c;

    if (pmd_studio_get_mode(window_settings->studio) != PMD_STUDIO_MODE_FILE_EDIT)
    {

        combo_index = uiComboboxSelected(window_settings->indevice);
        result =  pmd_studio_get_input_device_name(&indevname, window_settings->studio, combo_index);
        if (result != PMD_SUCCESS)
        {
            uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Error setting input device");
            return;
        }
        combo_index = uiComboboxSelected(window_settings->outdevice);    
        result =  pmd_studio_get_output_device_name(&outdevname, window_settings->studio, combo_index);
        if (result != PMD_SUCCESS)
        {
            uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Error setting output device");
            return;
        }

        text = uiEntryText(window_settings->channels);
        channels = atoi(text);
        if ((channels == 0) || (channels > MAX_CHANNELS))
        {
            uiMsgBoxError(window_settings->window, "PMD Studio Settings Error", "Number of channels invalid");
            // Revert back to original setting
            snprintf(label, MAX_LABEL_SIZE, "%u", device_settings->num_channels);
            uiEntrySetText(window_settings->channels, label);
            return;
        }

        strcpy(device_settings->input_device, indevname);
        strcpy(device_settings->output_device, outdevname);
        if (device_settings->num_channels != channels)
        {
            uiMsgBox(window_settings->window, "PMD Studio Settings", "Changing Channels Requires Reset");
            pmd_studio_reset(window_settings->studio);
        }
        device_settings->num_channels = channels;        
    }

    result = pmd_studio_settings_update(window_settings->studio, window_settings->window);
    if (result != PMD_SUCCESS)
    {
        uiMsgBoxError(window_settings->window, "PMD Studio Error", "error applying settings");
    }
    else
    {
        uiMsgBox(window_settings->window, "PMD Studio Settings", "Setting Applied");
    }

}

void
pmd_studio_device_init_settings(
    pmd_studio_device_settings *settings
    )
{
    strcpy(settings->input_device, "");
    strcpy(settings->output_device, "");
    settings->num_channels = DEFAULT_MAX_CHANNELS;
    settings->am824_mode = PMD_FALSE;
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
    unsigned int i, currentdev;
    pmd_studio_device_settings_window *window_settings;
    char label_text[MAX_LABEL_SIZE];
    char *currentindevname;
    char *currentoutdevname;
    unsigned int row = 0;
    dlb_pmd_success status;
    char *name; 

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

    if (pmd_studio_get_mode(studio) != PMD_STUDIO_MODE_FILE_EDIT)
    {
        uiGridAppend(grid, uiControl(uiNewLabel("Input Device")), 0, row, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        window_settings->indevice = uiNewCombobox();
        // Get current device name so we initialize combobox
        status = pmd_studio_get_input_device_name(&currentindevname, studio, CURRENT_DEVICE);
        // Get first name
        status = pmd_studio_get_input_device_name(&name, studio, 0);
        currentdev = 0;
        for (i = 0; (name != NULL) && (status == PMD_SUCCESS); i++)
        {
            uiComboboxAppend(window_settings->indevice, name);
            if (!strncmp(name, currentindevname, MAX_DEVICE_NAME_LENGTH - 1))
            {
                currentdev = i;
            }
            status = pmd_studio_get_input_device_name(&name, studio, i+1);
        }

        uiComboboxSetSelected(window_settings->indevice, currentdev);
        uiGridAppend(grid, uiControl(window_settings->indevice), 2, row++, 2, 1, 1, uiAlignFill, 0, uiAlignFill);

        uiGridAppend(grid, uiControl(uiNewLabel("Output Device")), 0, row, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        window_settings->outdevice = uiNewCombobox();

        // Get current device name so we initialize combobox
        status = pmd_studio_get_output_device_name(&currentoutdevname, studio, CURRENT_DEVICE);
        // Get first name
        status = pmd_studio_get_output_device_name(&name, studio, 0);
        for (i = 0; (name != NULL) && (status == PMD_SUCCESS); i++)
        {
            uiComboboxAppend(window_settings->outdevice, name);
            if (!strncmp(name, currentoutdevname, MAX_DEVICE_NAME_LENGTH - 1))
            {
                currentdev = i;
            }
            status = pmd_studio_get_output_device_name(&name, studio, i+1);
        }

        uiComboboxSetSelected(window_settings->outdevice, currentdev);
        uiGridAppend(grid, uiControl(window_settings->outdevice), 2, row++, 2, 1, 1, uiAlignFill, 0, uiAlignFill);

        uiGridAppend(grid, uiControl(uiNewLabel("Number of Channels")), 0, row, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
        window_settings->channels = uiNewEntry();
        uiGridAppend(grid, uiControl(window_settings->channels), 2, row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
        snprintf(label_text, MAX_LABEL_SIZE, "%u", device_settings->num_channels);
        uiEntrySetText(window_settings->channels, label_text);

    }

    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    window_settings->applybutton = uiNewButton("Apply");
    uiBoxAppend(vbox, uiControl(window_settings->applybutton), 0);
    uiButtonOnClicked(window_settings->applybutton, onAudioDeviceSettingsApplyButtonClicked , window_settings);

    uiControlShow(uiControl(window_settings->window));

}