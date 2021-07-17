/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/
#ifndef __PMD_STUDIO_DEVICE_SETTINGS_H__
#define __PMD_STUDIO_DEVICE_SETTINGS_H__

#define MAX_DEVICE_NAME_LENGTH (48)


struct pmd_studio_device_settings
{
    char input_device[MAX_DEVICE_NAME_LENGTH];
    char output_device[MAX_DEVICE_NAME_LENGTH];
    int num_channels;
    dlb_pmd_bool am824_mode;
};

#endif // __PMD_STUDIO_DEVICE_SETTINGS_H__
