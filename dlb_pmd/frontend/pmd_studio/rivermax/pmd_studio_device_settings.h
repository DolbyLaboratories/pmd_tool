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

#ifndef __PMD_STUDIO_DEVICE_SETTINGS_H__
#define __PMD_STUDIO_DEVICE_SETTINGS_H__

#define MAX_STREAM_NAME_LENGTH (128)
#define MAX_NUM_OUTPUT_STREAMS (3) // actual maximum number of simultaneous output streams

enum class PMD_STUDIO_STREAM_CODEC
{
    AES67_L16 = 0,
    AES67_L24,
    AM824,
    NUM_CODECS
};

struct pmd_studio_device_settings
{
    char input_stream_name[MAX_STREAM_NAME_LENGTH];
    unsigned int num_output_streams;
    char output_stream_name[MAX_NUM_OUTPUT_STREAMS][MAX_STREAM_NAME_LENGTH];
    enum PMD_STUDIO_STREAM_CODEC output_stream_codec[MAX_NUM_OUTPUT_STREAMS];
    unsigned int num_output_channel[MAX_NUM_OUTPUT_STREAMS];
    unsigned int num_output_channels;
    char interface_name[IFNAMSIZ];
};

#endif // __PMD_STUDIO_DEVICE_SETTINGS_H__
