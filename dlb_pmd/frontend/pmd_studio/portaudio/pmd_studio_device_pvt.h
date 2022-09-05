/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef __PMDSTUDIO_DEVICE_PVT_H__
#define __PMDSTUDIO_DEVICE_PVT_H__
#include "pmd_studio_device.h"
#include "pmd_studio.h"
#include "dlb_pmd_api.h"
#include "am824_framer.h"
#include "portaudio.h"
#include <mutex>
extern "C"{
#include "ui.h"
}


#define MAX_RING_BUFFERS 10
#define MAX_DEVICES (128)
#define MAX_DEVICE_NAME_LENGTH (48)
#define CURRENT_DEVICE (-1)
#define MAX_CHANNELS (32)


struct pmd_studio_device_ring_buffer
{
    unsigned int startchannel;
    unsigned int num_channels;
    unsigned int index;
    uint32_t *pcmbuf;
    unsigned int pcmbufsize; // number of 32 bit words in pcmbuf
    AM824Framer am824framer;
};

struct pmd_studio_ring_buffer_struct{
    PMDStudioDeviceRingBufferHandler *handler;          // Handler for ring buffer. Enables changes to output buffer mid-operation.
    std::mutex                      mutex;              // Should only be used when deleting (or in portaudio callback).
};

struct pmd_studio_device
{
    PaStreamParameters  			inputParameters;
    PaStreamParameters  			outputParameters;
    PaStream*           			stream;
    struct pmd_studio_comp_mix_matrix comp_mix_matrix1;
    struct pmd_studio_comp_mix_matrix comp_mix_matrix2;
    struct pmd_studio_comp_mix_matrix *active_comp_mix_matrix;    
    unsigned int                    num_ring_buffers;
    pmd_studio_ring_buffer_struct   ring_buffers[MAX_RING_BUFFERS];
    dlb_pmd_bool                    am824_mode;
    char                            input_device_names[MAX_DEVICES][MAX_DEVICE_NAME_LENGTH];
    char                            *input_device_name;
    unsigned int                    input_device_indices[MAX_DEVICES];
    unsigned int                    num_input_devices;
    char                            output_device_names[MAX_DEVICES][MAX_DEVICE_NAME_LENGTH];
    char                            *output_device_name;
    unsigned int                    output_device_indices[MAX_DEVICES];
    unsigned int                    num_output_devices;
    pmd_studio                      *studio;
};

struct pmd_studio_device_settings_window
{
    uiWindow *window;
    pmd_studio *studio;
    pmd_studio_device_settings *device_settings;
    uiButton *applybutton;
    uiCombobox *indevice;
    uiCombobox *outdevice;
    uiEntry *channels;
};


#endif
