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

#ifndef EMBERPLUS_MXBOLT_PMD_STUDIO_PVT_H
#define EMBERPLUS_MXBOLT_PMD_STUDIO_PVT_H

#include "ui.h"
#include "pmd_studio.h"
#include "pmd_studio_file_menu.h"
#include "pmd_studio_audio_beds.h"

#define MAX_BED_SOURCES (16)

struct pmd_studio
{
    char title[PMD_STUDIO_MAX_FILENAME_LENGTH];
    model pmd;
    uiWindow 						*window;
    uiEntry 						*title_entry;
    uiBox                           *toplevelbox;
    PMDStudioUIFooterHandler        *connection_section;
    pmd_studio_file_menu 			file_menu;
    pmd_studio_console_menu         console_menu;
    pmd_studio_audio_beds 			*audio_beds;
    pmd_studio_audio_objects 		*audio_objects;
    pmd_studio_audio_presentations 	*audio_presentations;
    pmd_studio_outputs  			*outputs;
    pmd_studio_device 				*device;
    PMDStudioConsoleInterface       *console;
    pmd_studio_settings             *settings;
    pmd_studio_mode                 mode;
    pmd_studio_config               configs[NUM_PMD_SPEAKER_CONFIGS];
    unsigned int                    num_configs;
    dlb_pmd_element_id              eids[MAX_EIDS];
    unsigned int                    num_eids;
};

#endif //EMBERPLUS_MXBOLT_PMD_STUDIO_PVT_H
