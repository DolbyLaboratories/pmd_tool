/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#include <arpa/inet.h>

// Required fro sleep function
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#ifndef _WIN32
  #include <sys/socket.h>
#else
  #include <unistd.h>
#endif


#include "pmd_studio.h"
#include "pmd_studio_pvt.h"
#include "pmd_studio_limits.h"
#include "pmd_studio_console.h"
#include "pmd_studio_settings_pvt.h"
#include "pmd_studio_console_emberplus.h"

#include <iostream>

using namespace dolby::dlb_pmd::ember;

dlb_pmd_success
pmd_studio_console_init_settings(
    pmd_studio_console_settings *settings
    )
{
    inet_pton(AF_INET, "127.0.0.1", &settings->address.sin_addr);        
    settings->address.sin_port = 9000;
    settings->type = CONSOLE_TYPE_EMBERPLUS;
    return PMD_SUCCESS;
}

void
pmd_studio_console_destroy
    (void *data
    )
{
    pmd_studio *s = (pmd_studio*) data;
    if(s->console != nullptr)
    {
        delete s->console;
        s->console = nullptr;
    }
}

/**
 * Internal (private) function.
 * Called every 5ms. When connecting, waits for connection/timeout. When connected, checks for
 * and handles new data. When disconnected, stops retrigger.
 * Return val of 1 triggers loop repeat, 0 does not.
 */
int 
console_poll
    (void *data
    )
{
    PMDStudioConsoleInterface *console = (PMDStudioConsoleInterface *) data;
    if(console == nullptr)
    {
        // Console has been deleted/disconnected. stop polling.
        return 0;
    }
    if(console->status.state == CONSOLE_DISCONNECTED)
    {
        fprintf(stderr, "PMDStudioConsole - Disconnected during poll\n");
        return 0;
    }
    if(console->poll(0) != PMD_SUCCESS)
    {
        fprintf(stderr, "PMDStudioConsole - Poll failed. Disconnecting\n");
        console->disconnect();
        return 0;
    }
    return 1;
}

static 
void 
onCancelConnectButtonClicked
    (uiButton *b
    ,void* data
    )
{
    pmd_studio *studio = (pmd_studio *) data;
    studio->console->disconnect();
}

void
console_connect
    (uiMenuItem *item
    ,uiWindow *window
    ,void *data
    )
{
    pmd_studio *studio = (pmd_studio *)data;

    if(studio->console != nullptr)
    {
        console_disconnect(studio);
    }
    switch(studio->settings->console_settings.type)
    {
        case CONSOLE_TYPE_EMBERPLUS:
            studio->console = new PMDStudioConsoleEmberPlus(studio);
            break;
        default:
            studio->settings->console_settings.type = CONSOLE_TYPE_EMBERPLUS;
            // TODO: Update error description when console type added to settings
            uiMsgBoxError(studio->window, "ERROR: Unrecognized console type setting", "This should never happen, defaulting to Ember+. Please try again.");
            return;
    }
    if(!studio->console->connect())
    {
        uiTimer(5, console_poll, (void *) studio->console);
    }
    else
    {
        std::cerr << "PMDStudioConsoleInterface - Failed to connect" << std::endl;
    }
}

void
console_disconnect
    (uiMenuItem *item,
     uiWindow *window,
     void *data)
{
    pmd_studio *studio = (pmd_studio *) data;
    if(studio->console == nullptr)
    {
        return;
    }
    if(studio->console->status.state != CONSOLE_DISCONNECTED)
    {
        studio->console->disconnect();
    }
    delete studio->console;
    studio->console = nullptr;
}

void
console_disconnect
    (void *data
    )
{
    console_disconnect(nullptr, nullptr, data);
}

dlb_pmd_success
pmd_studio_console_menu_init
    (pmd_studio_console_menu *cm
    ,pmd_studio *studio
    )
{
    cm->menu     = uiNewMenu("Console");
    cm->connect  = uiMenuAppendItem(cm->menu, "Connect");

    uiMenuItemOnClicked(cm->connect, console_connect, studio);
    return(PMD_SUCCESS);
}

PMDStudioConsoleInterface::PMDStudioConsoleInterface
    (pmd_studio *studio
    )
    :mPmdStudio{studio}
    ,settings{&studio->settings->console_settings}
{
    this->updateStatus(CONSOLE_DISCONNECTED);
}


PMDStudioConsoleInterface::~PMDStudioConsoleInterface()
{
    // Empty
}


void 
PMDStudioConsoleInterface::updateStatus
    (ConsoleConnectionState state
    ,ConsoleConnectErrorInfo errInfo
    ,const char *errMsg
    )
{
    this->status.state = state;
    this->status.errInfo = errInfo;
    if (errInfo){
        std::cerr << "PMDStudioConsole - Error: " << PMDStudioConsoleErrInfoToMsg[errInfo] << std::endl;
    }
}

void 
PMDStudioConsoleInterface::onConnected()
{
    char address[INET_ADDRSTRLEN];
    char windowtitle[MAX_LABEL_LENGTH];
    inet_ntop(AF_INET, &(settings->address.sin_addr), address, INET_ADDRSTRLEN);
    sprintf(windowtitle, "Connected to Console at %s:%d", address, settings->address.sin_port);
    
    // std::cout << "PMDStudioConsoleInterface - " << windowtitle << std::endl;
    pmd_studio_switch_mode(mPmdStudio, PMD_STUDIO_MODE_CONSOLE_LIVE);
    mPmdStudio->connection_section->set(windowtitle, "Disconnect", onCancelConnectButtonClicked);
}

void 
PMDStudioConsoleInterface::onConnecting()
{
    char addr[INET_ADDRSTRLEN];
    char windowtitle[MAX_LABEL_LENGTH];
    inet_ntop(AF_INET, &(settings->address.sin_addr), addr, INET_ADDRSTRLEN);
    sprintf(windowtitle, "Connecting to Console at %s...", addr);
    mPmdStudio->connection_section->set(windowtitle, "Cancel", onCancelConnectButtonClicked); 
    // fprintf(stdout, "PMDStudioConsoleInterface - Connecting to console at %s:%d...\n", addr, this->settings->address.sin_port);
}

void 
PMDStudioConsoleInterface::onDisconnected()
{
    // fprintf(stdout, "PMDStudioConsoleInterface - Disconnected\n");
    mPmdStudio->connection_section->set("Console disconnected.");
    if(this->status.errInfo)
    {
        // Connection Failed.
        // fprintf(stderr, "PMDStudioConsoleInterface - Error caused disconnect: %s\n", PMDStudioConsoleErrInfoToMsg[this->status.errInfo]);
        uiMsgBoxError(this->mPmdStudio->window, "Console connection error", PMDStudioConsoleErrInfoToMsg[this->status.errInfo]);
    }
    if(pmd_studio_metadata_output_active(mPmdStudio))
        pmd_studio_switch_mode(mPmdStudio, PMD_STUDIO_MODE_LIVE);
    else pmd_studio_switch_mode(mPmdStudio, PMD_STUDIO_MODE_EDIT);

    // Queue the destruction of the interface instance.
    uiQueueMain(console_disconnect, mPmdStudio);
}

void 
PMDStudioConsoleInterface::setObjGain(int index, float gain_dB)
{
    dlb_pmd_element_id *eids;
    unsigned int nobjs = pmd_studio_audio_objects_get_eids(&eids, mPmdStudio, false);

    (void)nobjs;
    pmd_studio_set_obj_gain(mPmdStudio, eids[index], gain_dB);
}

void 
PMDStudioConsoleInterface::setBedGain(int index, float gain_dB)
{
    dlb_pmd_element_id *eids;
    unsigned int *labels;
    unsigned int nbeds = pmd_studio_audio_beds_get_eids(&eids, &labels, mPmdStudio, false);

    (void)nbeds;
    pmd_studio_set_bed_gain(mPmdStudio, eids[index], gain_dB);
}


const char* PMDStudioConsoleErrInfoToMsg[CONSOLE_ERR_N_ERRS] = 
{
    "No Error",
    "Host not found",
    "Socket creation failed",
    "Socket timeout",
    "Socket configuration failed",
    "Socket connection failed",
    "Service error",
    "Connection lost"
};


const char* PMDStudioConsoleConnectStateToMsg[CONSOLE_NSTATES] = 
{
    "Disconnected",
    "Connecting",
    "Connected"
};


