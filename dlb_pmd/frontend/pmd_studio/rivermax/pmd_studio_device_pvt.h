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


#ifndef __PMDSTUDIO_DEVICE_PVT_H__
#define __PMDSTUDIO_DEVICE_PVT_H__
#include "pmd_studio_device.h"
#include "pmd_studio.h"
#include "dlb_pmd_api.h"
#include "am824_framer.h"
#include <mutex>
#include "dlb_st2110_api.h"
#include "pmd_studio_device_settings.h"

#define MAX_RING_BUFFERS 10
#define MAX_NUM_INTERFACES (16)
#define MAX_NUM_INPUT_STREAMS (128) // Max number to choose from. There is one received stream
#define SAP_SEARCH_TIME_SECS (40.0) // Worst case seconds to find SAP stream

struct pmd_studio_device_ring_buffer
{
    unsigned int startchannel;
    unsigned int num_channels;
    unsigned int index;
    uint32_t *pcmbuf;
    unsigned int pcmbufsize; // number of 32 bit words in pcmbuf
    AM824Framer am824framer;
};

struct pmd_studio_device_input_stream
{
    AoipServiceType service;
    char name[MAX_STREAM_NAME_LENGTH];
};

typedef char pmd_device_interface_list[MAX_NUM_INTERFACES][IFNAMSIZ];
typedef pmd_studio_device_input_stream pmd_device_input_stream_list[MAX_NUM_INPUT_STREAMS];

unsigned int
pmd_studio_device_get_input_stream_names(
    pmd_studio *studio,
    pmd_device_input_stream_list *stream_name_list);

unsigned int pmd_studio_device_get_interface_names(
    pmd_studio *studio,
    pmd_device_interface_list *output_interface_list);

struct pmd_studio_ring_buffer_struct{
    PMDStudioDeviceRingBufferHandler *handler;          // Handler for ring buffer. Enables changes to output buffer mid-operation.
    std::mutex                      mutex;              // Should only be used when deleting (or in portaudio callback).
};

struct pmd_studio_tx_call_back_data
{
    pmd_studio_device *device;
    unsigned int index;
};

struct pmd_studio_device
{
    struct pmd_studio_comp_mix_matrix comp_mix_matrix1;
    struct pmd_studio_comp_mix_matrix comp_mix_matrix2;
    struct pmd_studio_comp_mix_matrix *active_comp_mix_matrix;    
    unsigned int                    num_ring_buffers;
    pmd_studio_ring_buffer_struct   ring_buffers[MAX_RING_BUFFERS];
    StreamInfo                      input_stream_info;
    dlb_pmd_bool                    input_stream_active;
    unsigned int                    num_output_streams;
    StreamInfo                      output_stream_info[MAX_NUM_OUTPUT_STREAMS];
    unsigned int                    num_output_channels;
    ST2110TransceiverCallBackInfo   rxtx_call_back_info;
    ST2110TransmitterCallBackInfo   tx_call_back_info;
    pmd_studio_tx_call_back_data    tx_call_back_data[MAX_NUM_OUTPUT_STREAMS];       
    AoipSystem                      aoip_system;
    AoipServices                    *aoip_services;
    void                            *rxtx_hdl;
    pmd_studio                      *studio;
    float                           rx_search_time_taken;
    uiProgressBar                   *search_progress;
    uiWindow                        *progress_window;
    float                           latency;
    std::vector<StreamInfo>         txStreamInfos;
};


struct pmd_studio_device_settings_window
{
    uiWindow *window;
    pmd_studio *studio;
    pmd_studio_device_settings *device_settings;
    uiGrid *output_stream_grid;
    uiButton *applybutton;
    uiButton *add_output_stream_button;
    uiCombobox *interface_name;
    unsigned int num_interface_names;
    pmd_device_interface_list interface_names;
    uiCombobox *input_stream_name;
    pmd_device_input_stream_list input_streams;
    unsigned int num_input_streams;
    unsigned int num_output_streams;
    uiEntry *output_stream_name[MAX_NUM_OUTPUT_STREAMS];
    uiCombobox *output_stream_codec[MAX_NUM_OUTPUT_STREAMS];
    uiEntry *num_output_channel[MAX_NUM_OUTPUT_STREAMS];
    uiLabel *output_start_channel[MAX_NUM_OUTPUT_STREAMS];
    uiLabel *output_end_channel[MAX_NUM_OUTPUT_STREAMS];
    pmd_studio_settings *settings;
    uiButton *output_stream_delete[MAX_NUM_OUTPUT_STREAMS];
};

#endif
