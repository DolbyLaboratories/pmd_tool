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


#ifndef __PMDSTUDIO_DEVICE_PVT_H__
#define __PMDSTUDIO_DEVICE_PVT_H__
#include "pmd_smpte_337m.h" // for MAX_DATA_BYTES
#include "pmd_studio_device.h"
#include "pmd_studio.h"
#include "dlb_pmd_api.h"
#include "am824_framer.h"
#include <mutex>
#include "dlb_st2110_api.h"
#include "pmd_studio_device_settings.h"
#include "ring_buffer.h"

#define MAX_NUM_INTERFACES (16)
#define MAX_NUM_INPUT_STREAMS (128) // Max number to choose from. There is one received stream
#define SAP_SEARCH_TIME_SECS (40.0) // Worst case seconds to find SAP stream
#define MAX_RING_BUFFERS 10


struct pmd_studio_device_input_stream
{
    AoipServiceType service;
    char name[MAX_STREAM_NAME_LENGTH];
    unsigned int num_channels;
};

typedef char pmd_device_interface_list[MAX_NUM_INTERFACES][IFNAMSIZ];
typedef pmd_studio_device_input_stream pmd_device_input_stream_list[MAX_NUM_INPUT_STREAMS];


struct pmd_studio_tx_call_back_data
{
    pmd_studio_device *device;
    unsigned int index;
};

// Forward declaration to allow bidirectional reference
struct pmd_studio_device_settings_window;

struct pmd_studio_device
{
    struct pmd_studio_comp_mix_matrix comp_mix_matrix1;
    struct pmd_studio_comp_mix_matrix comp_mix_matrix2;
    struct pmd_studio_comp_mix_matrix *active_comp_mix_matrix;    
    unsigned int                    num_ring_buffers;
    PMDStudioRingBufferList         *ring_buffer_list;
    StreamInfo                      input_stream_info;
    dlb_pmd_bool                    input_stream_active;
    unsigned int                    num_output_streams;
    unsigned int                    num_output_audio_streams; // Excludes -41 streams
    StreamInfo                      output_stream_info[MAX_NUM_OUTPUT_STREAMS];
    unsigned int                    num_output_channels;
    ST2110TransceiverCallBackInfo   rxtx_call_back_info;
    ST2110TransmitterCallBackInfo   tx_call_back_info;
    ST2110TransmitterCallBackInfo   tx_41call_back_info;
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
    std::vector<StreamInfo>         tx41StreamInfos;
    uint8_t                         metadata1[MAX_DATA_BYTES];
    uint8_t                         metadata2[MAX_DATA_BYTES];
    uint8_t                         *active_metadata;
    unsigned int                    active_metadata_size;
    struct pmd_studio_device_settings_window *settings_window;
};


struct pmd_studio_device_settings_window
{
    uiWindow *window;
    pmd_studio *studio;
    pmd_studio_device_settings *device_settings;
    uiGrid *input_stream_grid;
    uiGrid *output_stream_grid;
    uiButton *applybutton;
    uiButton *add_output_stream_button;
    uiCombobox *media_interface_name;
    uiCombobox *manage_interface_name;
    uiEntry *ptp_domain;
    unsigned int num_interface_names;
    pmd_device_interface_list interface_names;
    uiCheckbox *sap_enable, *rav_enable, *nmos_enable;
    unsigned int enabled_services;
    uiEntry *nmos_registry;
    int input_stream_combo_box_params[8];
    uiCombobox *input_stream_name;
    uiLabel *input_stream_channel_count;
    pmd_device_input_stream_list    input_streams;
    unsigned int                    num_input_streams;
    std::mutex                      *input_stream_list_mutex;

    unsigned int num_output_streams;
    uiEntry *output_stream_name[MAX_NUM_OUTPUT_STREAMS];
    uiEntry *output_stream_address[MAX_ADDRESS_STRING_LEN]; 
    uiCombobox *output_stream_codec[MAX_NUM_OUTPUT_STREAMS];
    uiEntry *num_output_channel[MAX_NUM_OUTPUT_STREAMS];
    uiLabel *output_start_channel[MAX_NUM_OUTPUT_STREAMS];
    uiLabel *output_end_channel[MAX_NUM_OUTPUT_STREAMS];
    pmd_studio_settings *settings;
    uiButton *output_stream_delete[MAX_NUM_OUTPUT_STREAMS];
    uiCombobox *output_stream_frame_rate[MAX_NUM_OUTPUT_STREAMS];
};


unsigned int
pmd_studio_device_get_input_stream_names(
    pmd_studio *studio,
    pmd_device_input_stream_list *stream_name_list,
    unsigned int services);

unsigned int pmd_studio_device_get_interface_names(
    pmd_studio *studio,
    pmd_device_interface_list *output_interface_list);

void pmd_studio_device_recreate_input_stream_combo_box(
    pmd_studio_device_settings_window *window_settings
    );

void pmd_studio_device_enable_settings_window_updates(pmd_studio *studio, pmd_studio_device_settings_window *settings_window);
void pmd_studio_device_disable_settings_window_updates(pmd_studio *studio);

#endif
