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

#ifndef PMD_STUDIO_SETTINGS_H_
#define PMD_STUDIO_SETTINGS_H_

#include <stdio.h>
#include <arpa/inet.h>

#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio.h"
#include "pmd_studio_console.h"
#include "pmd_studio_device.h"

#define MAX_LABEL_SIZE 40

struct pmd_studio_settings;


/* This module has to own the common device settings as devices could be swapped in and out */
/* This could be moved to a pmd_studio_common_device.h in the future */

struct pmd_studio_common_device_settings
{
    float latency;
    unsigned int frames_per_buffer;
};

void
pmd_studio_settings_edit_device_settings
    (uiMenuItem *item
    ,uiWindow *w
    ,void *data
    );

void
edit_settings
    (uiMenuItem *item
    ,uiWindow *w
    ,void *data
    );


dlb_pmd_success
pmd_studio_settings_init(pmd_studio_settings **settings);

dlb_pmd_success
pmd_studio_settings_close(pmd_studio_settings *settings);






#endif /* PMD_STUDIO_SETTINGS_H_ */
