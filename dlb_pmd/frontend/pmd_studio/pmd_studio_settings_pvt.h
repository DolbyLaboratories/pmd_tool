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
#ifndef PMD_STUDIO_SETTINGS_PVT_H_
#define PMD_STUDIO_SETTINGS_PVT_H_

#include "dlb_pmd_api.h"
#include "pmd_studio_device.h"
#include "ui.h"

enum class PMD_STUDIO_NLANG_BEHAVIOUR
{
    FOLLOW_LANG = 0,
    USE_PRESET = 1,
    UNLOCKED = 2,
    NUM_BEHAVIOURS = 3
};

struct pmd_studio_nlang_settings
{
    PMD_STUDIO_NLANG_BEHAVIOUR behaviour;
    char preset_nlang[4];  
};

struct pmd_studio_settings
{
    dlb_pmd_bool file_based_mode;
    pmd_studio_device_settings      device_settings;
    pmd_studio_console_settings     console_settings;
    pmd_studio_nlang_settings       nlang_settings;
};

struct pmd_studio_settings_window
{
    uiWindow *window;
    pmd_studio *studio;
    uiButton *applybutton;
    uiCombobox *indevice;
    uiCombobox *outdevice;
    uiEntry *channels;
    uiEntry *latency;
    uiEntry *frames_per_buffer;
    uiEntry *console_address;
    uiEntry *console_port;
    uiCombobox *nlang_behaviour;
    uiCombobox *nlang_preset;
    pmd_studio_settings *settings;
};

#endif
