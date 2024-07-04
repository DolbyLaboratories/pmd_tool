/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#ifndef PMD_STUDIO_PVT_H
#define PMD_STUDIO_PVT_H

#include "ui.h"
#include "pmd_studio.h"
#include "pmd_studio_file_menu.h"
#include "pmd_studio_audio_beds.h"

#define MAX_BED_SOURCES (16)

struct pmd_studio
{
    int                             argc;
    char                            **argv;
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

#endif //PMD_STUDIO_PVT_H
