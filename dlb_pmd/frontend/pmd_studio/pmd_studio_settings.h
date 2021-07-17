/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
