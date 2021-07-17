/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_STUDIO_SETTINGS_PVT_H_
#define PMD_STUDIO_SETTINGS_PVT_H_

#include "dlb_pmd_api.h"
#include "pmd_studio_device.h"
#include "ui.h"


/* Constants */

/* Enumerations */

enum class PMD_STUDIO_NLANG_BEHAVIOUR
{
    FOLLOW_LANG = 0,
    USE_PRESET = 1,
    UNLOCKED = 2,
    NUM_BEHAVIOURS = 3
};

/* Structures */

struct pmd_studio_nlang_settings
{
    PMD_STUDIO_NLANG_BEHAVIOUR behaviour;
    char preset_nlang[4];  
};

struct pmd_studio_settings
{
    dlb_pmd_bool file_based_mode;
    pmd_studio_device_settings      device_settings;
    pmd_studio_common_device_settings common_device_settings;
    pmd_studio_console_settings     console_settings;
    pmd_studio_nlang_settings       nlang_settings;
};

struct pmd_studio_settings_window
{
    uiWindow *window;
    pmd_studio *studio;
    uiButton *applybutton;
    uiEntry *latency;
    uiEntry *frames_per_buffer;
    uiEntry *console_address;
    uiEntry *console_port;
    uiCombobox *nlang_behaviour;
    uiCombobox *nlang_preset;
    pmd_studio_settings *settings;
};

#endif