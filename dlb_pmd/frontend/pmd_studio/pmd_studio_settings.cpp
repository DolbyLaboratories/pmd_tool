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
#include <arpa/inet.h>
extern "C"{
#include "ui.h"
}
#include "dlb_pmd_api.h"
#include "pmd_studio.h"
#include "pmd_studio_device.h"
#include "pmd_studio_console.h"
#include "pmd_studio_settings.h"
#include "pmd_studio_settings_pvt.h"
#include "pmd_studio_pvt.h"
#include "pmd_studio_audio_presentations_pvt.h"

#define CONFIG_FILENAME "pmd_studio.cfg"

const char* NLANG_BEHAVIOUR_DESC_STR[(int) PMD_STUDIO_NLANG_BEHAVIOUR::NUM_BEHAVIOURS]
{
    "Follow presentation lang",
    "Use preset nlang",
    "Unlocked"
};

static
void
onApplyButtonClicked
    (uiButton *c
    ,void *data
    )
{
    pmd_studio_settings_window *s = (pmd_studio_settings_window*)data;
    int index;
    char *indevname, *outdevname;
    dlb_pmd_success result;
    char *text;
    int channels; // Because PortAudio uses int
    unsigned int latency_ms, frames_per_buffer;
    char label[MAX_LABEL_SIZE];
    struct sockaddr_in sa;
    char orgaddress[INET_ADDRSTRLEN];
    uint32_t address;


    (void)c;

    if (pmd_studio_get_mode(s->studio) != PMD_STUDIO_MODE_FILE_EDIT)
    {

        index = uiComboboxSelected(s->indevice);
        result =  pmd_studio_get_input_device_name(&indevname, s->studio, index);
        if (result != PMD_SUCCESS)
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Error setting input device");
            return;
        }
        index = uiComboboxSelected(s->outdevice);    
        result =  pmd_studio_get_output_device_name(&outdevname, s->studio, index);
        if (result != PMD_SUCCESS)
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Error setting output device");
            return;
        }

        text = uiEntryText(s->channels);
        channels = atoi(text);
        if ((channels == 0) || (channels > MAX_CHANNELS))
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Number of channels invalid");
            // Revert back to original setting
            snprintf(label, MAX_LABEL_SIZE, "%u", s->settings->device_settings.num_channels);
            uiEntrySetText(s->channels, label);
            return;
        }

        text = uiEntryText(s->latency);
        latency_ms = atoi(text);
        if (latency_ms > MAX_LATENCY_MS)
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Latency invalid");
            // Revert back to original setting
            snprintf(label, MAX_LABEL_SIZE, "%u", (unsigned int)(s->settings->device_settings.latency * 1000.0));
            uiEntrySetText(s->latency, label);
            return;
        }

        text = uiEntryText(s->frames_per_buffer);
        frames_per_buffer = atoi(text);
        if ((frames_per_buffer > MAX_FRAMES_PER_BUFFER) || (frames_per_buffer < MIN_FRAMES_PER_BUFFER))
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Buffer Size invalid");
            // Revert back to original setting
            snprintf(label, MAX_LABEL_SIZE, "%u", s->settings->device_settings.frames_per_buffer);
            uiEntrySetText(s->frames_per_buffer, label);
            return;
        }

        text = uiEntryText(s->console_address);
        
        inet_pton(AF_INET, text, &(sa.sin_addr));
        address = ntohl(sa.sin_addr.s_addr);
        // Rudimentary IP address check, check not multicast or network/broadcast address
        if (( address > 0xe0000000) || ((address & 0xff) == 0) || ((address & 0xff) == 255))
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Invalid IP address");
            // Revert back to original setting
            inet_ntop(AF_INET, &(s->settings->console_settings.address.sin_addr), orgaddress, INET_ADDRSTRLEN);
            uiEntrySetText(s->console_address, orgaddress);
            return;
        }

        text = uiEntryText(s->console_port);
        sa.sin_port = atoi(text);

        // Rudimentary port check
        if (sa.sin_port < 1024)
        {
            uiMsgBoxError(s->window, "PMD Studio Settings Error", "Invalid Port");
            // Revert back to original setting
            snprintf(label, MAX_LABEL_SIZE, "%u", s->settings->console_settings.address.sin_port);
            uiEntrySetText(s->console_port, label);
            return;
        }

        strcpy(s->settings->device_settings.input_device, indevname);
        strcpy(s->settings->device_settings.output_device, outdevname);
        if (s->settings->device_settings.num_channels != channels)
        {
            uiMsgBox(s->window, "PMD Studio Settings", "Changing Channels Requires Reset");
            pmd_studio_reset(s->studio);
        }
        s->settings->device_settings.num_channels = channels;        
        s->settings->device_settings.latency = latency_ms / 1000.0;        
        s->settings->device_settings.frames_per_buffer = frames_per_buffer;        
        s->settings->console_settings.address.sin_addr = sa.sin_addr;        
        s->settings->console_settings.address.sin_port = sa.sin_port;
    }

    index = uiComboboxSelected(s->nlang_behaviour);
    s->studio->settings->nlang_settings.behaviour = (PMD_STUDIO_NLANG_BEHAVIOUR) index;
    const char **langs;
    pmd_studio_get_supported_languages(&langs);
    memcpy(s->studio->settings->nlang_settings.preset_nlang, langs[uiComboboxSelected(s->nlang_preset)], 4);

    result = pmd_studio_settings_update(s->studio, s->window);
    if (result != PMD_SUCCESS)
    {
        uiMsgBoxError(s->window, "PMD Studio Error", "error applying settings");
    }
    else
    {
        uiMsgBox(s->window, "PMD Studio Settings", "Setting Applied");
    }
}

static
int
onSettingsWindowClosing
    (uiWindow *w
    ,void *data
    )
{
    pmd_studio_settings_window *s = (pmd_studio_settings_window*)data;

    free(s);
    uiControlDestroy(uiControl(w));
    return(0);   
}

static
void
onNlangBehaviourUpdate
    (uiCombobox *c
    ,void *data
    )
{
    // Just update settings window for now, update pmd_studio settings on apply.
    pmd_studio_settings_window *window = (pmd_studio_settings_window *) data;
    if((PMD_STUDIO_NLANG_BEHAVIOUR) uiComboboxSelected(window->nlang_behaviour) != PMD_STUDIO_NLANG_BEHAVIOUR::USE_PRESET)
    {
        uiControlDisable(uiControl(window->nlang_preset));
    }
    else
    {
        uiControlEnable(uiControl(window->nlang_preset));
    }
}

void
edit_settings
    (uiMenuItem *item
    ,uiWindow *w
    ,void *data
    )
{
    pmd_studio *s = (pmd_studio*)data;
    uiGrid *grid;
    uiBox *vbox;
    unsigned int row = 0;
    unsigned int i, currentdev;
    pmd_studio_settings_window *settings;
    char *name;
    char *currentindevname;
    char *currentoutdevname;
    dlb_pmd_success status;
    char ipaddress_text[INET_ADDRSTRLEN];
    char label_text[MAX_LABEL_SIZE];

    (void)item;
    (void)w;

    settings = (pmd_studio_settings_window *)malloc(sizeof(pmd_studio_settings_window));
    if (!settings)
    {
        return;
    }
    /* set up context */
    settings->window = uiNewWindow("Settings", 400, 410, 0);
    settings->settings = pmd_studio_get_settings(s);
    settings->studio = s;

    uiWindowOnClosing(settings->window, onSettingsWindowClosing, settings);
    uiWindowSetMargined(settings->window, 1);

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    grid = uiNewGrid();
    uiWindowSetChild(settings->window, uiControl(vbox));
    uiGridSetPadded(grid, 1);
    uiBoxAppend(vbox, uiControl(grid), 0);

    if (pmd_studio_get_mode(s) != PMD_STUDIO_MODE_FILE_EDIT)
    {
        uiGridAppend(grid, uiControl(uiNewLabel("Input Device")), 0, row, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        settings->indevice = uiNewCombobox();
        // Get current device name so we initialize combobox
        status = pmd_studio_get_input_device_name(&currentindevname, s, CURRENT_DEVICE);
        // Get first name
        status = pmd_studio_get_input_device_name(&name, s, 0);
        currentdev = 0;
        for (i = 0; (name != NULL) && (status == PMD_SUCCESS); i++)
        {
            uiComboboxAppend(settings->indevice, name);
            if (!strncmp(name, currentindevname, MAX_DEVICE_NAME_LENGTH - 1))
            {
                currentdev = i;
            }
            status = pmd_studio_get_input_device_name(&name, s, i+1);
        }

        uiComboboxSetSelected(settings->indevice, currentdev);
        uiGridAppend(grid, uiControl(settings->indevice), 2, row++, 2, 1, 1, uiAlignFill, 0, uiAlignFill);

        uiGridAppend(grid, uiControl(uiNewLabel("Output Device")), 0, row, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
        settings->outdevice = uiNewCombobox();

        // Get current device name so we initialize combobox
        status = pmd_studio_get_output_device_name(&currentoutdevname, s, CURRENT_DEVICE);
        // Get first name
        status = pmd_studio_get_output_device_name(&name, s, 0);
        for (i = 0; (name != NULL) && (status == PMD_SUCCESS); i++)
        {
            uiComboboxAppend(settings->outdevice, name);
            if (!strncmp(name, currentoutdevname, MAX_DEVICE_NAME_LENGTH - 1))
            {
                currentdev = i;
            }
            status = pmd_studio_get_output_device_name(&name, s, i+1);
        }

        uiComboboxSetSelected(settings->outdevice, currentdev);
        uiGridAppend(grid, uiControl(settings->outdevice), 2, row++, 2, 1, 1, uiAlignFill, 0, uiAlignFill);

        uiGridAppend(grid, uiControl(uiNewLabel("Number of Channels")), 0, row, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
        settings->channels = uiNewEntry();
        uiGridAppend(grid, uiControl(settings->channels), 2, row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
        snprintf(label_text, MAX_LABEL_SIZE, "%u", settings->settings->device_settings.num_channels);
        uiEntrySetText(settings->channels, label_text);

        uiGridAppend(grid, uiControl(uiNewLabel("Requested Latency (ms)")), 0, row, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
        settings->latency = uiNewEntry();
        uiGridAppend(grid, uiControl(settings->latency), 2, row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
        snprintf(label_text, MAX_LABEL_SIZE, "%u", (unsigned int)(settings->settings->device_settings.latency * 1000.0));    
        uiEntrySetText(settings->latency, label_text);

        uiGridAppend(grid, uiControl(uiNewLabel("Frames per Buffer")), 0, row, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
        settings->frames_per_buffer = uiNewEntry();
        uiGridAppend(grid, uiControl(settings->frames_per_buffer), 2, row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
        snprintf(label_text, MAX_LABEL_SIZE, "%u", settings->settings->device_settings.frames_per_buffer);
        uiEntrySetText(settings->frames_per_buffer, label_text);

        // Ember+ section   
        uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
        uiGrid *ember_grid = uiNewGrid();
        uiGridSetPadded(ember_grid, 1);
        int ember_grid_row = 0;
        uiBoxAppend(vbox, uiControl(ember_grid), 0);

        uiGridAppend(ember_grid, uiControl(uiNewLabel("Ember+ Console IP Address")), 0, ++ember_grid_row, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
        settings->console_address = uiNewEntry();
        uiGridAppend(ember_grid, uiControl(settings->console_address), 2, ember_grid_row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
        inet_ntop(AF_INET, &(settings->settings->console_settings.address.sin_addr), ipaddress_text, INET_ADDRSTRLEN);
        uiEntrySetText(settings->console_address, ipaddress_text);

        uiGridAppend(ember_grid, uiControl(uiNewLabel("Ember+ Console Port")), 0, ember_grid_row, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
        settings->console_port = uiNewEntry();
        uiGridAppend(ember_grid, uiControl(settings->console_port), 2, ember_grid_row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
        snprintf(label_text, MAX_LABEL_SIZE, "%u", settings->settings->console_settings.address.sin_port);
        uiEntrySetText(settings->console_port, label_text);

    }
    // NLang section
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    uiGrid *nlang_grid = uiNewGrid();
    uiGridSetPadded(nlang_grid, 1);
    int nlang_grid_row = 0;
    uiBoxAppend(vbox, uiControl(nlang_grid), 0);
    uiBoxAppend(vbox, uiControl(uiNewLabel("NOTE: \"NLang\" refers to the language of the presentation name.\r\nFor example, a presentation named \"French\" (targeting a french\r\naudience) should have lang set to \"fre\" and nlang set to \"eng\".")), 0);
    
    uiGridAppend(nlang_grid, uiControl(uiNewLabel("NLang Behaviour")), 0, nlang_grid_row, 2, 1, 1, uiAlignFill,0, uiAlignFill);
    settings->nlang_behaviour = uiNewCombobox();
    uiGridAppend(nlang_grid, uiControl(settings->nlang_behaviour), 2, nlang_grid_row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
    for(i=0; i<(int) PMD_STUDIO_NLANG_BEHAVIOUR::NUM_BEHAVIOURS; i++)
    {
        uiComboboxAppend(settings->nlang_behaviour, NLANG_BEHAVIOUR_DESC_STR[i]);
    }
    uiComboboxSetSelected(settings->nlang_behaviour, (int)settings->settings->nlang_settings.behaviour);
    uiComboboxOnSelected(settings->nlang_behaviour, onNlangBehaviourUpdate, settings);

    uiGridAppend(nlang_grid, uiControl(uiNewLabel("    Preset NLang")), 0, nlang_grid_row, 2, 1, 1, uiAlignFill,0, uiAlignFill);
    settings->nlang_preset = uiNewCombobox();
    uiGridAppend(nlang_grid, uiControl(settings->nlang_preset), 2, nlang_grid_row++, 1, 1, 0, uiAlignEnd, 0, uiAlignFill);
    //unsigned int selected_preset_nlang = pmd_studio_get_default_language_index();
    const char** languages;
    unsigned int nlanguages = pmd_studio_get_supported_languages(&languages);
    for(i=0; i < nlanguages; i++)
    {
        uiComboboxAppend(settings->nlang_preset, languages[i]);
    }
    uiComboboxSetSelected(settings->nlang_preset, pmd_studio_language_index_from_lang(settings->settings->nlang_settings.preset_nlang));

    // Disable preset combobox if nlang_behaviour setting isn't preset
    onNlangBehaviourUpdate(settings->nlang_behaviour, settings);

    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);
    settings->applybutton = uiNewButton("Apply");
    uiBoxAppend(vbox, uiControl(settings->applybutton), 0);
    uiButtonOnClicked(settings->applybutton, onApplyButtonClicked, settings);

    uiControlShow(uiControl(settings->window));
}

static
dlb_pmd_success
pmd_studio_settings_read_config_file
    (pmd_studio_settings *settings
    )
{
    FILE *cfgFile;
    size_t result;

    printf("Reading config file...\n");

    cfgFile = fopen(CONFIG_FILENAME, "rb");
    if (!cfgFile)
    {
        pmd_studio_warning("Can't open config file for reading");
        return(PMD_FAIL);
    }

    result = fread(settings, sizeof(pmd_studio_settings), 1, cfgFile);
    if (!result)
    {
        pmd_studio_error(PMD_STUDIO_ERR_FILE, "Reading of application settings unsuccessful");
        fclose(cfgFile);
        return(PMD_FAIL);       
    }
    
    fclose(cfgFile);
    return(PMD_SUCCESS);
}

static
dlb_pmd_success
pmd_studio_settings_write_config_file
    (pmd_studio_settings *settings
    )
{
    FILE *cfgFile;
    size_t result;

    printf("Writing config file...\n");

    cfgFile = fopen(CONFIG_FILENAME, "w");
    if (!cfgFile)
    {
        pmd_studio_error(PMD_STUDIO_ERR_FILE, "Can't open config file for writing");
        return(PMD_FAIL);
    }

    result = fwrite(settings, sizeof(pmd_studio_settings), 1, cfgFile);
    if (!result)
    {
        pmd_studio_error(PMD_STUDIO_ERR_FILE, "Writing of application settings unsuccessful");        
        fclose(cfgFile);
        return(PMD_FAIL);
    }

    fclose(cfgFile);
    return(PMD_SUCCESS);
}

dlb_pmd_success
pmd_studio_settings_init(pmd_studio_settings **settings)
{
    // *settings = (pmd_studio_settings *)malloc(sizeof(pmd_studio_settings));
    *settings = new pmd_studio_settings;

    if (!*settings)
    {
        pmd_studio_error(PMD_STUDIO_ERR_MEMORY, "Failed to allocate memory for beds");
        return(PMD_FAIL);
    }

    // Set to safe defaults in case config file read fails
    pmd_studio_device_init_settings(&(*settings)->device_settings);
    pmd_studio_console_init_settings(&(*settings)->console_settings);

    // Set default nlang fields
    (*settings)->nlang_settings.behaviour = PMD_STUDIO_NLANG_BEHAVIOUR::FOLLOW_LANG;
    const char **langs;
    pmd_studio_get_supported_languages(&langs);
    memcpy((*settings)->nlang_settings.preset_nlang, langs[pmd_studio_get_default_language_index()], 4);

    /* Load config file first so it can be overwritten by command line option */
    pmd_studio_settings_read_config_file(*settings);

    (*settings)->file_based_mode = PMD_FALSE;
    return(PMD_SUCCESS);
}

dlb_pmd_success
pmd_studio_settings_close(pmd_studio_settings *settings)
{
    dlb_pmd_success status;
    status = pmd_studio_settings_write_config_file(settings);
    // free(settings);
    delete settings;
    return(status);
}
