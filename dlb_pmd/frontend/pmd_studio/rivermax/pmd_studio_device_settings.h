/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#ifndef __PMD_STUDIO_DEVICE_SETTINGS_H__
#define __PMD_STUDIO_DEVICE_SETTINGS_H__

#include "pmd_studio_audio_outputs.h"
#include "pmd_studio.h"

#define MAX_STREAM_NAME_LENGTH (128)
#define MAX_HOSTNAME_LENGTH (254)
#define MAX_NUM_OUTPUT_STREAMS (6)
#define MAX_ADDRESS_STRING_LEN (40) // COPES WITH IPV6

enum class PMD_STUDIO_STREAM_CODEC
{
    AES67_L16 = 0,
    AES67_L24,
    AM824,
    SMPTE2110_41,
    NUM_CODECS
};

struct pmd_studio_device_settings
{
    char input_stream_name[MAX_STREAM_NAME_LENGTH];
    unsigned int num_output_streams;
    char output_stream_name[MAX_NUM_OUTPUT_STREAMS][MAX_STREAM_NAME_LENGTH];
    char output_stream_address[MAX_NUM_OUTPUT_STREAMS][MAX_ADDRESS_STRING_LEN];
    enum PMD_STUDIO_STREAM_CODEC output_stream_codec[MAX_NUM_OUTPUT_STREAMS];
    unsigned int num_output_channel[MAX_NUM_OUTPUT_STREAMS];
    unsigned int num_output_channels;
    char media_interface_name[IFNAMSIZ];
    char manage_interface_name[IFNAMSIZ];
    unsigned int ptp_domain;
    unsigned int enabled_services;
    char nmos_registry[MAX_HOSTNAME_LENGTH];
};

#endif // __PMD_STUDIO_DEVICE_SETTINGS_H__
