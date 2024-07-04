/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020-2023, Dolby Laboratories Inc.
 * Copyright (c) 2020-2023, Dolby International AB.
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


#include <iostream>
#include <vector>
#include <ifaddrs.h>

extern "C"{
//    #include "dlb_pmd_pcm.h"
//    #include "ui.h"
//    #include "pmd_smpte_337m.h"
//    #include "dlb_pmd_model_combo.h"
//    #include "sadm_bitstream_encoder.h"
}


#include "pmd_studio.h"
#include "pmd_studio_common_defs.h"
#include "pmd_studio_device.h"
#include "pmd_studio_device_pvt.h"

#include "dlb_st2110_logging.h"
#include "dlb_st2110_api.h"
#include "mclock.h"
#include "mix_matrix.h"


#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <stdint.h>
#endif

using namespace std;

#define SAMPLE_RATE 48000
#define SADM_DATA_ITEM_TYPE 0x000100 // Value from preliminary spec of SMPTE 2110-41

const char
*pmd_studio_device_get_settings_menu_name(void)
{
    return("Streams");
}


/* Private Functions */
/* Mix Matrix type and methods */
        

static
void
pmd_studio_mix_matrices_reset(
    pmd_studio_device *device)
{
    device->comp_mix_matrix1.size = 0;
    device->comp_mix_matrix2.size = 0;
    device->active_comp_mix_matrix = &device->comp_mix_matrix1;
}


static
bool RxTxCallback(void *data,
                  void *inputAudio,
                  void *outputAudio,
                  unsigned int inputBytes,
                  unsigned int outputBytes
                  )
{
    int32_t *readPtr = (int32_t *)inputAudio;
    int32_t *writePtr = (int32_t *)outputAudio;
    unsigned int i,j;
    pmd_studio_device *device = (pmd_studio_device *)data;
    struct pmd_studio_comp_mix_matrix *cm = device->active_comp_mix_matrix;
    // frames per buffer is same for input and output buffer channels may not be
    // Doesn't matter whether you use input or output here but you can't mix the two. Audio format is common (32 bit)
    unsigned int framesPerBuffer = inputBytes / (GetAoipBytesPerSample(device->rxtx_call_back_info.audioFormat) * device->input_stream_info.audio.numChannels);

    if( framesPerBuffer > 0 )
    {
        // First run mixing matrix in compressed form
        for( i = 0; i < framesPerBuffer ; i++ )
        {
            // Replace this loop with memset outside all loops for speed
            for (j = 0 ; j < device->input_stream_info.audio.numChannels ; j++)
            {
                writePtr[j] = 0;
            }
            for (j = 0 ; j < cm->size ; j++)
            {
                writePtr[cm->output[j]] += readPtr[cm->input[j]] * cm->coef[j]; /* TODO: clarify mixed-types arithmetic -- where do we truncate? */
            }
            writePtr += device->input_stream_info.audio.numChannels;
            readPtr += device->input_stream_info.audio.numChannels;
        }
	    // Now add ring buffers
        device->ring_buffer_list->WriteRingBuffers((int32_t *) outputAudio, device->input_stream_info.audio.numChannels, framesPerBuffer);
    }
    return(true); // always continue
}

static
bool TxCallback(void *data,
                void *audioPtr,
                unsigned int &inputBytes,
                bool& haveTimeStamp,
                uint32_t& timeStamp)
{
    pmd_studio_tx_call_back_data *tx_call_back_data = (pmd_studio_tx_call_back_data *)data;
    pmd_studio_device *device = tx_call_back_data->device;
    unsigned int tx_index = tx_call_back_data->index;
    unsigned int tx_num_channels = device->output_stream_info[tx_index].audio.numChannels;
    unsigned int framesPerBuffer = inputBytes / (tx_num_channels * GetAoipBytesPerSample(device->tx_call_back_info.audioFormat));
    // frames per buffer is same for input and output buffer channels may not be
    // Doesn't matter whether you use input or output here but you can't mix the two. Audio format is common (32 bit)

    if( framesPerBuffer > 0 )
    {
        // Clear the entire output buffer
        // This is slightly inefficient as we will probably overwrite later but is simple
        
        memset(audioPtr, 0, framesPerBuffer * device->output_stream_info[tx_index].audio.numChannels * sizeof(int32_t));
 
        device->ring_buffer_list->WriteRingBuffers((int32_t *) audioPtr, tx_num_channels, framesPerBuffer);
    }
    haveTimeStamp = false;
    return(true); // always continue
}

static
bool Tx41Callback(void *data,
                void *metadataPtr,
                unsigned int &inputBytes,
                bool& haveTimeStamp,
                uint32_t& timeStamp)
{
    pmd_studio_tx_call_back_data *tx_call_back_data = (pmd_studio_tx_call_back_data *)data;
    pmd_studio_device *device = tx_call_back_data->device;
    unsigned int tx_index = tx_call_back_data->index;

    if (device->output_stream_info[tx_index].metadata.dataItemTypes.size() < 1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Missing DIT");
    }

    if (device->output_stream_info[tx_index].metadata.dataItemTypes.size() > 1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Multiple DITs - not supported");
    }

    if (device->output_stream_info[tx_index].metadata.dataItemTypes.front() != SADM_DATA_ITEM_TYPE)
    { 
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Wrong DIT, Only S-ADM supported");
    }

    // Copy ring buffer associated with this output

    inputBytes = device->ring_buffer_list->CopyEntireActiveBufferBytes(device->output_stream_info[tx_index].sourceIndex, (uint32_t *)metadataPtr);

    haveTimeStamp = false;
    return(true); // always continue
}


static
void pmd_studio_device_init_transceiver(
    void *data
    )
{
    pmd_studio *studio = (pmd_studio *)data;
    uiWindow *win = pmd_studio_get_window(studio);
    pmd_studio_device *device = pmd_studio_get_device(studio);
    unsigned int i;
    dlb_pmd_bool status = PMD_FAIL; // suppress warning    
    std::vector<StreamInfo>         txStreamInfos;
    std::vector<StreamInfo>         tx41StreamInfos;


    // Prepare lists for transceiver and transmitters based on streamInfo lists

    if ((device->input_stream_active) && (device->num_output_streams > 0))
    {
        // Input stream is active so create list of audio streams for transceiver and additional list for -41 streams
        unsigned int j = 0; // Counting -41 streams
        for (i = 0 ; i < device->num_output_streams ; i++)
        {
            if ((device->output_stream_info[i].streamType == AES67) || (device->output_stream_info[i].streamType == AM824))
            {
                txStreamInfos.push_back(device->output_stream_info[i]);
            }
            else
            {
                tx41StreamInfos.push_back(device->output_stream_info[i]);
                device->tx_call_back_data[j].device = device;
                device->tx_call_back_data[j++].index = i; // i = index into streamInfo list
            }
        }
    }
    else
    {
        for (i = 0 ; i < device->num_output_streams ; i++)
        {
            txStreamInfos.push_back(device->output_stream_info[i]);
            device->tx_call_back_data[i].device = device;
            device->tx_call_back_data[i].index = i;
        }
    }

    try
    {
        // Only do rxtx if there are both input and output streams
        // It is possible to have an input stream without an output stream as the only outputstreams could be -41 metadata
        if ((device->input_stream_active) && (device->num_output_streams > 0))
        {
            if (txStreamInfos.size() > 0)
            {
                device->rxtx_hdl = device->aoip_services->AddRxTxStream(device->input_stream_info, txStreamInfos, device->latency, device->rxtx_call_back_info);
                try {
                    if (device->aoip_services->StartRxTxStream(device->rxtx_hdl))
                    {
                        status = PMD_SUCCESS;
                    }
                }
                catch(runtime_error& e)
                {
                    char errStr[PMD_STUDIO_ERROR_MESSAGE_SIZE];
                    strcpy(errStr, "Failed to Start Streams");
                    strncat(errStr, e.what(), PMD_STUDIO_ERROR_MESSAGE_SIZE);
                    uiMsgBoxError(win, "PMD Studio Error", errStr);
                }
            }
            else
            {
                    status = PMD_SUCCESS;
            }
            i = 0;
            for (vector<StreamInfo>::iterator tx41StreamInfo = tx41StreamInfos.begin() ; tx41StreamInfo != tx41StreamInfos.end() ; tx41StreamInfo++)
            {
                ST2110TransmitterCallBackInfo *call_back_info = &device->tx_41call_back_info;
                call_back_info->data = &device->tx_call_back_data[i];
                try
                {
                    device->aoip_services->AddTxStream(*tx41StreamInfo, call_back_info);
                    device->aoip_services->StartTxStream(tx41StreamInfo->streamName);
                }
                catch(runtime_error& e)
                {
                    char errStr[PMD_STUDIO_ERROR_MESSAGE_SIZE];
                    strcpy(errStr, "Failed to Start Streams");
                    strncat(errStr, e.what(), PMD_STUDIO_ERROR_MESSAGE_SIZE);
                    uiMsgBoxError(win, "PMD Studio Error", errStr);
                }                
                i++;
            }
        }
        else
        {
            i = 0;
            for (vector<StreamInfo>::iterator txStreamInfo = txStreamInfos.begin() ; txStreamInfo != txStreamInfos.end() ; txStreamInfo++)
            {
                ST2110TransmitterCallBackInfo *call_back_info;

                if (txStreamInfo->streamType == SMPTE2110_41)
                {
                    call_back_info = &device->tx_41call_back_info;
                }
                else
                {
                    call_back_info = &device->tx_call_back_info;
                }
                call_back_info->data = &device->tx_call_back_data[i];
                device->aoip_services->AddTxStream(*txStreamInfo, call_back_info);
                device->aoip_services->StartTxStream(txStreamInfo->streamName);
                i++;
            }
            status = PMD_SUCCESS;
        }
    }
    catch(runtime_error& e)
    {
        char msg[PMD_STUDIO_ERROR_MESSAGE_SIZE];
        strcpy(msg, "Failed to Start IP Streams - ");
        strncat(msg, e.what(), PMD_STUDIO_ERROR_MESSAGE_SIZE);
        uiMsgBoxError(win, "PMD Studio Error", msg);
    }

    if (status != PMD_SUCCESS)
    {
        pmd_studio_error(PMD_STUDIO_ERR_STREAMING, "Starting Streams Failed");
    }
    
}

static
void
onCancelButtonClicked
    (uiButton *c
    ,void *data
    )
{
    pmd_studio_device *device = (pmd_studio_device *)data;
    // Advance timer
    device->rx_search_time_taken = SAP_SEARCH_TIME_SECS;
}

static
int pmd_studio_device_receive_poll(
    void *data
    )
{
    pmd_studio_device *device = (pmd_studio_device *)data;
    const AoipService *input_service  = nullptr;
    const unsigned int progress_label_size = 80;
    char label[progress_label_size];

    if (device->progress_window == NULL)
    {
        device->progress_window = uiNewWindow("Searching for Input Stream", 350, 50, 0);
        uiWindowSetMargined(device->progress_window, 1);
        uiBox *vbox = uiNewVerticalBox();
        uiBoxPadded(vbox);
        uiWindowSetChild(device->progress_window, uiControl(vbox));
        snprintf(label, progress_label_size, "Searching for Input Stream: %s", device->input_stream_info.streamName.c_str());
        
        device->search_progress = uiNewProgressBar();
        uiBoxAppend(vbox, uiControl(uiNewLabel(label)), 1);
        uiButton *cancel_button = uiNewButton("Cancel");
        uiButtonOnClicked(cancel_button, onCancelButtonClicked, device);
        uiBoxAppend(vbox, uiControl(cancel_button), 0);
        uiBoxAppend(vbox, uiControl(device->search_progress), 1);
        uiControlShow(uiControl(device->progress_window));
    }


    device->rx_search_time_taken += 1.0; // 1 second poll interval
    if (device->rx_search_time_taken >= SAP_SEARCH_TIME_SECS)
    {
        // Make sure we signal that we are in transmit only mode
        device->input_stream_active = PMD_FALSE;
        // Disable timer
        uiMsgBoxError(device->progress_window, "PMD Studio Error", "Selected Input Stream Not Found");
        uiControlDestroy(uiControl(device->progress_window));
        device->progress_window = NULL;
        // Initiate transmission without reception
        // Reception failure is signaled via input_stream_active
        uiQueueMain(pmd_studio_device_init_transceiver, device->studio);
        return(0);
    }
    else
    {
        int progress = (int)(device->rx_search_time_taken * (100.0/SAP_SEARCH_TIME_SECS)); // Progress Bar takes integer 1-100
        uiProgressBarSetValue(device->search_progress, progress);
        vector<AoipService>& rxStreams = device->aoip_services->GetAvailableServicesForRx();
        for (vector<AoipService>::iterator stream = rxStreams.begin() ; stream != rxStreams.end() ; stream++)
        {
            if (stream->GetName() == device->input_stream_info.streamName)
            {
                input_service = &(*stream);
                device->input_stream_info = input_service->GetStreamInfo();
                if (device->input_stream_info.samplingFrequency != SAMPLE_RATE)
                {
                    uiMsgBoxError(device->progress_window, "PMD Studio Error", "Input stream sampling Frequency is not supported (48kHz only)");
                    device->input_stream_active = PMD_FALSE;
                }
                else
                {
                    device->input_stream_active = PMD_TRUE;
                }
                break;
            }
        }
        if (device->input_stream_active == PMD_TRUE)
        {
            // Initiate transmission with reception
            // Reception success is signaled via input_stream_active
            uiControlDestroy(uiControl(device->progress_window));
            device->progress_window = NULL;
            uiQueueMain(pmd_studio_device_init_transceiver, device->studio);
            return(0);
        }
        else
        {
            // Keep going
            return(1);
        }
    }
}

/*

static
void
pmd_studio_device_on_augmentor_fail_cb
    (void* data
    ,dlb_pmd_model *model
    )
{
    pmd_studio_device *device = (pmd_studio_device *) data;
    
    // Incase there's an augmentor error before mdout is set.
    if(device != nullptr)
    {
        pmd_studio_warning(model->error);
        device->augmentor_error = PMD_TRUE;  
    }
}

*/


/* Public Functions */

unsigned int
pmd_studio_device_get_input_stream_names(
    pmd_studio *studio,
    pmd_device_input_stream_list *stream_list,
    unsigned int services)
{
    pmd_studio_device *device;
    unsigned int num_input_streams;
    StreamInfo streamInfo;
    AoipServiceType service_type;

    if (!studio)
    {
        return(0);
    }

    device = pmd_studio_get_device(studio);
    if (!device)
    {
        return(0);
    }
    if (!device->aoip_services)
    {
        return(0);
    }
    vector<AoipService>& rxServices = device->aoip_services->GetAvailableServicesForRx();
    string prefix, display_name;

    num_input_streams = 0;
    for (vector<AoipService>::iterator service = rxServices.begin() ; (service != rxServices.end()) && (num_input_streams < MAX_NUM_INPUT_STREAMS) ; service++)
    {
        service_type = service->GetType();
        // If this service is included in the provided mask then add it to the list
        if (service_type & services)
        {
            (*stream_list)[num_input_streams].service = service_type;
            streamInfo = service->GetStreamInfo();
            (*stream_list)[num_input_streams].num_channels = streamInfo.audio.numChannels;
            display_name = service->GetName();
            strncpy((*stream_list)[num_input_streams++].name, display_name.c_str(), MAX_STREAM_NAME_LENGTH);
        }
    }
    return(num_input_streams);
}

unsigned int pmd_studio_device_get_interface_names(
    pmd_studio *studio,
    pmd_device_interface_list *output_interface_list)
{
    struct ifaddrs *addresses;
    struct ifaddrs *address;
    int family;
    unsigned int num_interfaces;

    if (!studio)
    {
        return(0);
    }

    if (getifaddrs(&addresses) == -1)
    {
        pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "getifaddrs call failed");
        return(0);
    }
    address = addresses;

    num_interfaces = 0;
    while(address && (num_interfaces < MAX_NUM_INTERFACES))
    {
        family = address->ifa_addr->sa_family;
        // Only support IPv4
        if (family == AF_INET)
        {
            strncpy((*output_interface_list)[num_interfaces++], address->ifa_name, IFNAMSIZ);
        }
        address = address->ifa_next;
    }
    freeifaddrs(addresses);
    return(num_interfaces);
}

static
void pmd_studio_device_async_settings_update(
    void *data
    )
{
    pmd_studio *studio = (pmd_studio *)data;
    pmd_studio_settings_update(studio, NULL); // NULL is window, this signals to use default window    
}


void pmd_studio_device_enable_settings_window_updates(pmd_studio *studio, pmd_studio_device_settings_window *settings_window)
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    device->settings_window = settings_window;
}

void pmd_studio_device_disable_settings_window_updates(pmd_studio *studio)
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    device->settings_window = NULL;
}


static
void pmd_studio_device_async_new_service(
    void *data
    )
{
    pmd_studio_device  *device = (pmd_studio_device *)data;
    // Now in UI context so can recreate Combobox with new service
    pmd_studio_device_recreate_input_stream_combo_box(device->settings_window);
}


void pmd_studio_device_new_service(pmd_studio_device *device, pmd_studio_device_settings *settings, const AoipService &newService)
{
    string display_name = newService.GetName();

    // Only update new service list immediately if setting window is currently active
    if (device->settings_window)
    {
        uiQueueMain(pmd_studio_device_async_new_service, device);
    }
}


void pmd_studio_device_receive_stream_changed(pmd_studio_device *device, pmd_studio_device_settings *settings, string streamName)
{
    // Check to see if name has actually changed from Nmos or this is just a confirmation of change request
    if (device->input_stream_info.streamName != streamName)
    {
        device->input_stream_info.streamName = streamName;
        strncpy(settings->input_stream_name, streamName.c_str(), MAX_STREAM_NAME_LENGTH);
        // This is a deep callback
        // To perform reset we need to be in uiContext
        uiQueueMain(pmd_studio_device_async_settings_update, device->studio);
    }
}



/* This can be called with *s as a NULL pointer to indicate first time initialization */
/* if *s is non null then we are applying new settings to existing device */
dlb_pmd_success
pmd_studio_device_init(
	pmd_studio_device **retdevice,
    pmd_studio_common_device_settings *common_settings,
    pmd_studio_device_settings *settings,
    uiWindow *win,
    pmd_studio *studio
	)
{
	pmd_studio_device *device;
    dlb_pmd_bool status;
    struct sched_param sp;
    pmd_device_interface_list output_interface_list;
    dlb_pmd_bool transmit_only_mode = (strlen(settings->input_stream_name) == 0);
    unsigned int sourceIndex, i;
    AoipServices::CallBacks serviceCallBacks;

    #ifdef LOGGING
    InitLogging(0, nullptr, "");
    #else
    SuppressLogging();
    #endif

    // Check to see if we are initializing for the first time
    // or just applying new settings
    if (*retdevice == NULL)
    {

        *retdevice = new pmd_studio_device;
        if(!*retdevice)
        {
            return(PMD_FAIL);
        }
        device = *retdevice;
        device->settings_window = NULL;
        device->studio = studio;
        device->ring_buffer_list = nullptr;
        device->aoip_services = nullptr;

        pmd_studio_mix_matrices_reset(device);
    }
    else
    {
        device = *retdevice;
        // This is a device update. We need to recreate the stream using the new settings
        // Try and maintain UI settings such as matrix and ring buffers
        // Stop any existing outputs
        pmd_studio_outputs_stop_all_metadata_outputs(studio);

        // Implement sutting down services and reopening them

        // destroy ring buffer list if it exists so new one can be created
    }

    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    status = sched_setscheduler(0, SCHED_FIFO, &sp);
    if (status == -1)
    {
        uiMsgBoxError(win, "PMD Studio Error", "Failed to set priority to MAX/SCHED_FIFO - Need to be root");
        return(PMD_FAIL);
    }

    /* Interface Name */
    device->aoip_system.mediaInterface.interfaceName = settings->media_interface_name;
    device->aoip_system.manageInterface.interfaceName = settings->manage_interface_name;
    // An empty interface name means we need to get the default
    if (device->aoip_system.mediaInterface.interfaceName.empty() ||
        device->aoip_system.manageInterface.interfaceName.empty())
    {

        if (pmd_studio_device_get_interface_names(studio, &output_interface_list) == 0)
        {
            uiMsgBoxError(win, "PMD Studio Error", "No valid interfaces found");
            return(PMD_FAIL);
        }
        else
        {
            // Take first interface by default
            if (device->aoip_system.mediaInterface.interfaceName.empty())
            {
                device->aoip_system.mediaInterface.interfaceName = output_interface_list[0];
            }
            if (device->aoip_system.manageInterface.interfaceName.empty())
            {
                device->aoip_system.manageInterface.interfaceName = output_interface_list[0];
            }
        }
    }

    device->aoip_system.samplingFrequency = SAMPLE_RATE;
    device->aoip_system.name = "pmd-studio";
    device->aoip_system.nmosRegistry = settings->nmos_registry;

    if (device->aoip_services != nullptr)
    {
        // In this case we are updating the device with new settings so the
        // services need to be torn down and recreated
        delete device->aoip_services;

        // As rx/tx services have now stopped, it is now safe to tear down the buffer lists as well
        // ready for the update
        if (device->ring_buffer_list != nullptr)
        {
            delete device->ring_buffer_list;
            device->ring_buffer_list = nullptr;
        }
    }

    // Start Aoip Services to kick off discovery
    try
    {
        // Create null callbacks for now
        AoipServices::NewRxServiceCallBack newRxServiceCallBack;
        serviceCallBacks.newRxServiceCallBack = [device, settings](const AoipService &newService)
        {
            pmd_studio_device_new_service(device, settings, newService);
        };

        
        serviceCallBacks.connectionReqCallBack = [device, settings](string streamName)
        {
            pmd_studio_device_receive_stream_changed(device, settings, streamName);
        };
        device->aoip_system.domain = settings->ptp_domain;
        device->aoip_services = new AoipServices(device->aoip_system, settings->enabled_services, serviceCallBacks);
    }
    catch(runtime_error& e)
    {
        char errStr[PMD_STUDIO_ERROR_MESSAGE_SIZE];
        strcpy(errStr, "Failed to Initialize IP - ");
        strncat(errStr, e.what(), PMD_STUDIO_ERROR_MESSAGE_SIZE);
        uiMsgBoxError(win, "PMD Studio Error", errStr);
        // As the Aoip service is on the last line of the try we know that either it
        // wasn't reached or the exception occurred inside the new
        // either way allocation did not occur or memory was deallocated so safe to reset pointer for next time
        device->aoip_services = nullptr;
        return(PMD_FAIL);
    }

    // Nothing more can happen without output streams
    device->num_output_streams = settings->num_output_streams;
    if (device->num_output_streams == 0)
    {
        device->input_stream_active = PMD_FALSE;
        uiMsgBoxError(win, "PMD Studio Error", "No output streams defined");
        return(PMD_FAIL);        
    }

    // Record common setting so can be retrieved with only device context
    device->latency = common_settings->latency;

    // If an input stream is defined then try to find it
    device->input_stream_active = PMD_FALSE;

    // Setup callbacks for both options

    device->rxtx_call_back_info.audioFormat = DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM;
    device->rxtx_call_back_info.callBack = RxTxCallback;
    device->rxtx_call_back_info.data = (void *)device;
    device->rxtx_call_back_info.blockSize = common_settings->frames_per_buffer;

    device->tx_call_back_info.audioFormat = DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM;
    device->tx_call_back_info.callBack = TxCallback;
    device->tx_call_back_info.blockSize = common_settings->frames_per_buffer;

    device->tx_41call_back_info.callBack = Tx41Callback;
    // Blocksize and Audio Format are not used by -41 transmitter

    // Setup txStreamInfos
    device->num_output_channels = 0;
    sourceIndex = 0;
    // Empty txStreamInfos
    device->txStreamInfos.clear();
    for (i = 0 ; i < settings->num_output_streams ; i++)
    {
        if (device->input_stream_active)
        {
            device->output_stream_info[i].samplingFrequency = device->input_stream_info.samplingFrequency;
        }
        else
        {
            device->output_stream_info[i].latency = common_settings->latency;
            device->output_stream_info[i].samplingFrequency = SAMPLE_RATE;
        }
        device->output_stream_info[i].streamName = settings->output_stream_name[i];
        switch(settings->output_stream_codec[i])
        {
            case PMD_STUDIO_STREAM_CODEC::AES67_L16:
                device->output_stream_info[i].streamType = AES67;
                device->output_stream_info[i].audio.payloadBytesPerSample = 2;
            break;
            case PMD_STUDIO_STREAM_CODEC::AES67_L24:
                device->output_stream_info[i].streamType = AES67;
                device->output_stream_info[i].audio.payloadBytesPerSample = 3;
            break;
            case PMD_STUDIO_STREAM_CODEC::AM824:
                device->output_stream_info[i].streamType = AM824;
                device->output_stream_info[i].audio.payloadBytesPerSample = 3;
            break;
            case PMD_STUDIO_STREAM_CODEC::SMPTE2110_41:
                device->output_stream_info[i].streamType = SMPTE2110_41;
                device->output_stream_info[i].metadata.maxPayloadSizeBytes = MAX_DATA_BYTES;
                device->output_stream_info[i].metadata.packetTimeMs = 40.0; // set to 25fps by default, will be changed by output stream
                device->output_stream_info[i].metadata.dataItemTypes.clear();
                device->output_stream_info[i].metadata.dataItemTypes.push_back(SADM_DATA_ITEM_TYPE); // Hardcode to S-ADM for now
             break;
            default:
                pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Unknown Codec");
                return(PMD_FAIL);
        }
        device->output_stream_info[i].payloadType = 98; // Dynamic payloads only 96 - 127
        device->output_stream_info[i].port = 0; // Select for aoipSystem to select Port
        device->output_stream_info[i].dstIpStr = settings->output_stream_address[i];
        device->output_stream_info[i].sourceIndex = sourceIndex;

        if ((device->output_stream_info[i].streamType == AES67) ||
            (device->output_stream_info[i].streamType == AM824))
        {
            device->output_stream_info[i].audio.numChannels = settings->num_output_channel[i];
            sourceIndex += device->output_stream_info[i].audio.numChannels;
            if (device->output_stream_info[i].audio.numChannels == 0)
            {
                pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Can't have zero channels in output stream");
                return(PMD_FAIL);
            }
            else if (device->output_stream_info[i].audio.numChannels < 9)
            {
                device->output_stream_info[i].audio.samplesPerPacket = 48; // 2110-30/31 class A     
            }
            else
            {
                device->output_stream_info[i].audio.samplesPerPacket = 6; // 2110-30/31 class C  
            }
            device->num_output_channels += device->output_stream_info[i].audio.numChannels;
        }
    }

    device->ring_buffer_list = new PMDStudioRingBufferList(MAX_OUTPUT_CHANNELS);


    // initialize metadata elements, ready for an update
    device->active_metadata = device->metadata1;
    device->active_metadata_size = 0;

    if (!transmit_only_mode)
    { 
        device->input_stream_info.streamName = settings->input_stream_name;
        device->rx_search_time_taken = 0.0;
        device->progress_window = NULL;
        uiTimer(1000, pmd_studio_device_receive_poll, device);
    }
    else
    {
        uiQueueMain(pmd_studio_device_init_transceiver, device->studio);
    }
    return(PMD_SUCCESS);
}

PMDStudioRingBufferList *pmd_studio_device_get_ring_buffer_list(pmd_studio *studio)
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    return device->ring_buffer_list;
}


dlb_pmd_success
pmd_studio_device_add_ring_buffer(
    unsigned int ring_buffer_channel,         // output channel index starting at 0
    unsigned int ring_buffer_num_channels,         // number of channels (1/2)
    pmd_studio_video_frame_rate frame_rate,
    pmd_studio *studio
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);


    if (frame_rate == INVALID_FRAME_RATE)
    {
        std::runtime_error("INVALID_FRAME_RATE");
    }

    StreamInfo *output_stream_info;
    bool found = false;
    for (unsigned int i = 0 ; i < device->num_output_streams ; i++)
    {
        unsigned int start_channel = device->output_stream_info[i].sourceIndex;
        unsigned int num_channels = (device->output_stream_info[i].streamType == SMPTE2110_41) ? 1 : device->output_stream_info[i].audio.numChannels;
        unsigned int last_channel = start_channel + num_channels - 1;
        if ((ring_buffer_channel >= start_channel) && (ring_buffer_channel <= last_channel))
        {
            found = true;
            output_stream_info = &device->output_stream_info[i];
        }
    }
    if (!found)
    {
        return PMD_FAIL;
    }
    if (output_stream_info->streamType == SMPTE2110_41)
    {
        device->aoip_services->StopTxStream(output_stream_info->streamName);
        output_stream_info->metadata.packetTimeMs = 1000.0 / pmd_studio_video_frame_rate_floats[frame_rate];
        device->ring_buffer_list->AddRingBuffer(ring_buffer_channel, ring_buffer_num_channels, INVALID_FRAME_RATE);
        device->aoip_services->UpdateTxStream(*output_stream_info);
        device->aoip_services->StartTxStream(output_stream_info->streamName);
    }
    else
    {
        device->ring_buffer_list->AddRingBuffer(ring_buffer_channel, ring_buffer_num_channels, frame_rate);
    }
    return PMD_SUCCESS;
 }



dlb_pmd_success
pmd_studio_device_reset(
    pmd_studio_device *device
    )
{
    pmd_studio_mix_matrices_reset(device);

    device->ring_buffer_list->Reset();

    device->num_ring_buffers = 0;
    return(PMD_SUCCESS);
}

void pmd_studio_device_option(pmd_studio *studio, const char *option)
{
    if (!strcmp(option, "list"))
    {
        printf("RiverMax Version String: %s\n", rmax_get_version_string());
    }
}


dlb_pmd_success
pmd_studio_device_update_mix_matrix(
	pmd_studio *studio
	)
{
    pmd_studio_mix_matrix_array mix_matrix;
	pmd_studio_device *dev = pmd_studio_get_device(studio);

    // If no input stream then nothing to do
    // If using only -41 then num_output_channels could be 0
    if (!dev->input_stream_active || (dev->num_output_channels == 0))
    {
        return(PMD_SUCCESS);
    }

	if (dev->active_comp_mix_matrix == &dev->comp_mix_matrix1)
	{
		if (pmd_studio_audio_outputs_get_mix_matrix(mix_matrix, studio) == PMD_SUCCESS)
		{
            compress_mix_matrix(mix_matrix, &dev->comp_mix_matrix2, dev->input_stream_info.audio.numChannels, dev->num_output_channels);
            dev->active_comp_mix_matrix = &dev->comp_mix_matrix2;
		}
		else
		{
			pmd_studio_warning("pmd_studio_audio_outputs_get_mix_matrix failed");
			return(PMD_FAIL);
		}
	}
	else if (dev->active_comp_mix_matrix == &dev->comp_mix_matrix2)
	{
		if (pmd_studio_audio_outputs_get_mix_matrix(mix_matrix, studio) == PMD_SUCCESS)
		{
            compress_mix_matrix(mix_matrix, &dev->comp_mix_matrix1, dev->input_stream_info.audio.numChannels, dev->num_output_channels);
            dev->active_comp_mix_matrix = &dev->comp_mix_matrix1;
		}
		else
		{
			pmd_studio_warning("pmd_studio_audio_outputs_get_mix_matrix failed");
			return(PMD_FAIL);
		}
	}
	else
	{
		pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "mix matrix update failed");
	}
	return(PMD_SUCCESS);
}

bool pmd_studio_device_channel_requires_smpte337(unsigned int channel, pmd_studio *studio)
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    for (unsigned int i = 0 ; i < device->num_output_streams ; i++)
    {
        unsigned int start_channel = device->output_stream_info[i].sourceIndex;
        unsigned int num_channels = device->output_stream_info[i].audio.numChannels;
        unsigned int last_channel = start_channel + num_channels - 1;
        if ((channel >= start_channel) && (channel <= last_channel))
        {
            // Found txInfo that relates to requested channel
            if (device->output_stream_info[i].streamType == AM824)
            {
                return(true);
            }
            else
            {
                return(false);
            }
        }
    }
    return(false);
}


void
pmd_studio_device_print_debug(
    pmd_studio *studio
    )
{
    pmd_studio_device *device;

    if (!studio)
    {
        return;
    }

    device = pmd_studio_get_device(studio);
    if (!device)
    {
        return;
    }

    printf("Devices\n=======\n");

    if (device->ring_buffer_list != nullptr)
    {
        device->ring_buffer_list->PrintDebug();
    }
}


void
pmd_studio_device_close(
	pmd_studio_device *s
	)
{

	if (!s)
	{
		return;
	}

    if (s->aoip_services)
    {
        delete s->aoip_services;
    }

 }
