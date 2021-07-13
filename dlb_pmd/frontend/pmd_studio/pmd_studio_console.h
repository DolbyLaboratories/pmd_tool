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

/**
 * @file pmd_studio_console.h
 * @brief Console integration module
 */

#ifndef __PMD_STUDIO_CONSOLE_H__
#define __PMD_STUDIO_CONSOLE_H__


#include <netinet/in.h>
extern "C"{
    #include "ui.h"
}
#include "dlb_pmd_api.h"
#include "pmd_studio.h"
#include <string>


enum pmd_studio_console_type{
    CONSOLE_TYPE_EMBERPLUS
    // , CONSOLE_TYPE_OSC
};

typedef struct
{
    struct sockaddr_in address;
    pmd_studio_console_type type;
} pmd_studio_console_settings;

typedef enum{
    CONSOLE_ERR_NONE,
    CONSOLE_ERR_HOST_NOT_FOUND, 
    CONSOLE_ERR_SOCK_CREATION_FAIL,
    CONSOLE_ERR_SOCK_TIMEOUT,
    CONSOLE_ERR_SOCK_CONFIG_FAIL,
    CONSOLE_ERR_CONNECTION_FAIL,
    CONSOLE_ERR_SERVICE_ERR,
    CONSOLE_ERR_CONNECTION_LOST,
    CONSOLE_ERR_N_ERRS
} ConsoleConnectErrorInfo;

typedef enum{
    CONSOLE_DISCONNECTED, 
    CONSOLE_CONNECTING, 
    CONSOLE_CONNECTED,
    CONSOLE_NSTATES
} ConsoleConnectionState;


typedef struct{
    ConsoleConnectErrorInfo errInfo;
    ConsoleConnectionState state;
} ConsoleConnectionStatus;

extern const char* PMDStudioConsoleErrInfoToMsg[CONSOLE_ERR_N_ERRS];
extern const char* PMDStudioConsoleConnectStateToMsg[CONSOLE_NSTATES];

class 
PMDStudioConsoleInterface
{
    public:
    pmd_studio *mPmdStudio;
    pmd_studio_console_settings *settings;
    ConsoleConnectionStatus status;
    PMDStudioConsoleInterface(pmd_studio *studio);
    virtual ~PMDStudioConsoleInterface();
    virtual dlb_pmd_success connect() = 0;
    virtual dlb_pmd_success disconnect() = 0;
    virtual dlb_pmd_success poll(int timeout) = 0;
    void setObjGain(int index, float gain_dB);
    void setBedGain(int index, float gain_dB);
    void onConnected();
    void onDisconnected();
    void onConnecting();
    void updateStatus(ConsoleConnectionState state, ConsoleConnectErrorInfo errInfo=CONSOLE_ERR_NONE, const char *errMsg="");
};

typedef struct
{
    uiMenu *menu;
    uiMenuItem *connect;
} pmd_studio_console_menu;

// dlb_pmd_success
// pmd_studio_console_init(pmd_studio_console **console, pmd_studio *s);

/**
 * Create a new PMDStudioConsoleInterface instance and begin connecton. 
 * Will disconnect and delete existing instance, if present.
 */
void
console_connect
    (uiMenuItem *item
    ,uiWindow *window
    ,void *data
    );

/**
 * Disconnect and delete PMDStudioConsoleInterface instance. 
 * Safe to call multiple times.
 */
void
console_disconnect
    (uiMenuItem *item,
     uiWindow *window,
     void *studio);

/**
 * Disconnect and delete PMDStudioConsoleInterface instance. 
 * Safe to call multiple times.
 */
void
console_disconnect
    (void *studio
    );

dlb_pmd_success
pmd_studio_console_init_settings
    (pmd_studio_console_settings *settings
    );

dlb_pmd_success
pmd_studio_console_menu_init
    (pmd_studio_console_menu *cm
    ,pmd_studio *studio
    );

#endif /* __PMD_STUDIO_CONSOLE_H__ */
