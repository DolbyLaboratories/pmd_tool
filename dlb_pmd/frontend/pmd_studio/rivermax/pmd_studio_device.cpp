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


#include <iostream>
#include <vector>
#include <ifaddrs.h>
#include "pmd_studio.h"
#include "pmd_studio_limits.h"
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

PMDStudioDeviceRingBuffer::PMDStudioDeviceRingBuffer
    (PMDStudioDeviceRingBufferHandler *parent
    )
{
    this->parent = parent;
    this->pcmbufsize = parent->pcmbufsize;
    this->pcmbuf = new uint32_t[pcmbufsize * parent->num_channels];
    memset(this->pcmbuf, 0, sizeof(pcmbufsize * parent->num_channels * sizeof(uint32_t)));
}

PMDStudioDeviceRingBuffer::~PMDStudioDeviceRingBuffer()
{
    delete this->pcmbuf;
}

PMDStudioDeviceRingBuffer *
PMDStudioDeviceRingBuffer::newBufferFromRingBufferStruct
        (pmd_studio_ring_buffer_struct *output_buf_struct
        )
{
    PMDStudioDeviceRingBufferHandler *handler = output_buf_struct->handler;
    return new PMDStudioDeviceRingBuffer(handler);
}

void 
PMDStudioDeviceRingBuffer::push()
{   
    this->parent->queueNewBuffer(this);
}

PMDStudioDeviceRingBufferHandler::PMDStudioDeviceRingBufferHandler(unsigned int startchannel, unsigned int num_channels, unsigned int pcmbufsize):
    startchannel{startchannel},
    num_channels{num_channels},
    index{0},
    pcmbufsize{pcmbufsize},
    active{nullptr},
    queued{nullptr}
    {}

PMDStudioDeviceRingBufferHandler::~PMDStudioDeviceRingBufferHandler(){
    if(active != nullptr) delete active;
    if(queued != nullptr) delete queued;
}

void PMDStudioDeviceRingBufferHandler::queueNewBuffer(PMDStudioDeviceRingBuffer *buf){
    // Make sure no other process is using the queued buffer pointer.
    queued_mutex.lock();
    if(active == nullptr){
        active = buf;
    }
    else{
        if(queued != nullptr) delete queued;
        queued = buf;
    }
    queued_mutex.unlock();
}

void PMDStudioDeviceRingBufferHandler::updateBuffer(){
    // Make sure no other process tries to queue a new buffer
    queued_mutex.lock();
    if(queued != nullptr){
        if(active != nullptr) delete active;
        active = queued;
        queued = nullptr;
    }
    queued_mutex.unlock();
}

uint32_t PMDStudioDeviceRingBufferHandler::peek(){
    if(active == nullptr) return uint32_t{0};
    return active->pcmbuf[this->index];
}

uint32_t PMDStudioDeviceRingBufferHandler::next(){
    if(active == nullptr) return uint32_t{0};
    uint32_t to_ret = active->pcmbuf[index];
    index++;
    if(index >= pcmbufsize){
        index = 0;
        updateBuffer();
    }
    return to_ret;
}

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
    unsigned int i,j,k;
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
        for (i = 0 ; i < device->num_ring_buffers ; i++)
        {   
            if(device->ring_buffers[i].mutex.try_lock()){
                PMDStudioDeviceRingBufferHandler *handle = device->ring_buffers[i].handler;
                if (handle != nullptr)
                {
                    writePtr = (int32_t *) outputAudio;
                    writePtr += handle->startchannel;
                    for( j = 0; j < framesPerBuffer ; j++ )
                    {
                        for (k = 0 ; k < handle->num_channels ; k++)
                        {
                            *writePtr = handle->next();
                            writePtr++;
                        }
                        writePtr += device->input_stream_info.audio.numChannels - handle->num_channels;
                    }
                }
                device->ring_buffers[i].mutex.unlock();
            }
        } 
    }
    return(true); // always continue
}

static
bool TxCallback(void *data,
                void *audioPtr,
                unsigned int inputBytes,
                bool& haveTimeStamp,
                uint32_t& timeStamp)
{
    int32_t *writePtr = (int32_t *)audioPtr;
    unsigned int i,j,k;
    pmd_studio_tx_call_back_data *tx_call_back_data = (pmd_studio_tx_call_back_data *)data;
    pmd_studio_device *device = tx_call_back_data->device;
    unsigned int tx_index = tx_call_back_data->index;
    unsigned int tx_start_channel = device->output_stream_info[tx_index].sourceIndex;
    unsigned int tx_num_channels = device->output_stream_info[tx_index].audio.numChannels;
    unsigned int tx_last_channel = tx_start_channel + tx_num_channels - 1;
    unsigned int overlap_start, overlap_end;
    unsigned int framesPerBuffer = inputBytes / (tx_num_channels * GetAoipBytesPerSample(device->tx_call_back_info.audioFormat));
    // frames per buffer is same for input and output buffer channels may not be
    // Doesn't matter whether you use input or output here but you can't mix the two. Audio format is common (32 bit)

    if( framesPerBuffer > 0 )
    {
        // Clear the entire output buffer
        // This is slightly inefficient as we will probably overwrite later but is simple
        
        for( i = 0; i < framesPerBuffer * device->output_stream_info[tx_index].audio.numChannels ; i++ )
        {
            *writePtr++ = 0;
        }
        
       // Only ring buffers as no input audio
        for (i = 0 ; i < device->num_ring_buffers ; i++)
        {   
            if(device->ring_buffers[i].mutex.try_lock())
            {
                PMDStudioDeviceRingBufferHandler *handle = device->ring_buffers[i].handler;
                overlap_start = max(handle->startchannel, tx_start_channel);
                overlap_end = min(handle->startchannel + handle->num_channels - 1, tx_last_channel);
                if ((handle != nullptr) && (overlap_end >= overlap_start))
                {
                    writePtr = (int32_t *) audioPtr;
                    writePtr += overlap_start - tx_start_channel;
                    for( j = 0; j < framesPerBuffer ; j++ )
                    {
                        for (k = 0 ; k < (overlap_end - overlap_start + 1) ; k++)
                        {
                            *writePtr = handle->next();
                            writePtr++;
                        }
                        writePtr += tx_num_channels - k;
                    }
                }
                device->ring_buffers[i].mutex.unlock();
            }
        } 
    }
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

    try
    {
        if (device->input_stream_active)
        {
            device->rxtx_hdl = device->aoip_services->AddRxTxStream(device->input_stream_info, device->txStreamInfos, device->latency, device->rxtx_call_back_info);
            if (device->aoip_services->StartRxTxStream(device->rxtx_hdl))
            {
                status = PMD_SUCCESS;
            }
            else
            {
                status = PMD_FAIL;
            }
        }
        else
        {
            i = 0;
            for (vector<StreamInfo>::iterator txStreamInfo = device->txStreamInfos.begin() ; txStreamInfo != device->txStreamInfos.end() ; txStreamInfo++)
            {
                device->tx_call_back_data[i].device = device;
                device->tx_call_back_data[i].index = i;
                device->tx_call_back_info.data = &device->tx_call_back_data[i];
                device->aoip_services->AddTxStream(*txStreamInfo, &device->tx_call_back_info);
                device->aoip_services->StartTxStream(txStreamInfo->streamName);
                i++;
            }
            status = PMD_SUCCESS;
        }
    }
    catch(runtime_error& e)
    {
        cout << e.what() << "\n";
        uiMsgBoxError(win, "PMD Studio Error", "Failed to Start IP Streams - Check Interface");
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

/* Public Functions */

unsigned int
pmd_studio_device_get_input_stream_names(
    pmd_studio *studio,
    pmd_device_input_stream_list *stream_list)
{
    pmd_studio_device *device;
    unsigned int num_input_streams;

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
        (*stream_list)[num_input_streams].service = service->GetType();
        display_name = service->GetName();
        strncpy((*stream_list)[num_input_streams++].name, display_name.c_str(), MAX_STREAM_NAME_LENGTH);
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
        device->studio = studio;
        device->aoip_services = nullptr;

        device->num_ring_buffers = 0;

    }
    else
    {
        device = *retdevice;
        // This is a device update. We need to recreate the stream using the new settings
        // Try and maintain UI settings such as matrix and ring buffers
        // Stop any existing outputs
        //pmd_studio_outputs_stop_all_metadata_outputs(studio);

        // Implement sutting down services and reopening them
        
    }

    MClock::CheckTaiOffset();


    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    status = sched_setscheduler(0, SCHED_FIFO, &sp);
    if (status == -1)
    {
        uiMsgBoxError(win, "PMD Studio Error", "Failed to set priority to MAX/SCHED_FIFO - Need to be root");
        return(PMD_FAIL);
    }

    /* Interface Name */
    device->aoip_system.interface = settings->interface_name;
    // An empty interface name means we need to get the default
    if (device->aoip_system.interface.empty())
    {

        if (pmd_studio_device_get_interface_names(studio, &output_interface_list) == 0)
        {
            uiMsgBoxError(win, "PMD Studio Error", "No valid interfaces found");
            return(PMD_FAIL);
        }
        else
        {
            // Take first interface by default
            device->aoip_system.interface = output_interface_list[0];
        }
    }

    device->aoip_system.samplingFrequency = SAMPLE_RATE;
    device->aoip_system.name = "pmd-studio";

    if (device->aoip_services != nullptr)
    {
        // In this case we are updating the device with new settings so the
        // services need to be torn down and recreated
        delete device->aoip_services;
    }

    // Start Aoip Services to kick off discovery
    try
    {
        device->aoip_services = new AoipServices(device->aoip_system, nullptr);
    }
    catch(runtime_error& e)
    {
        cout << e.what() << "\n";
        uiMsgBoxError(win, "PMD Studio Error", "Failed to Initialize IP");
        return(PMD_FAIL);
    }

    // Nothing more can happen without output streams
    if (settings->num_output_streams == 0)
    {
        device->num_output_streams = 0;
        device->input_stream_active = PMD_FALSE;
        uiMsgBoxError(win, "PMD Studio Error", "No output streams defined");
        return(PMD_FAIL);        
    }
    else
    {
        device->num_output_streams = settings->num_output_streams;
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


    // Setup txStreamInfos
    device->num_output_channels = 0;
    sourceIndex = 0;
    // Empty txStreamInfos
    while(!device->txStreamInfos.empty())
    {
        device->txStreamInfos.pop_back();
    }

    for (i = 0 ; i < device->num_output_streams ; i++)
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
            default:
                pmd_studio_error(PMD_STUDIO_ERR_ASSERT, "Unknown Codec");
                return(PMD_FAIL);
        }
        device->output_stream_info[i].payloadType = 98; // Dynamic payloads only 96 - 127
        device->output_stream_info[i].port = 0; // Select for aoipSystem to select Port
        device->output_stream_info[i].dstIpStr = string("239.150.150.") + to_string(i + 1); // Multicast address range used is 239.150.150.N where N is 1..M
        device->output_stream_info[i].audio.numChannels = settings->num_output_channel[i];
        device->output_stream_info[i].sourceIndex = sourceIndex;
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
        device->txStreamInfos.push_back(device->output_stream_info[i]);
    }

    pmd_studio_mix_matrices_reset(device);

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

dlb_pmd_success
pmd_studio_device_reset(
    pmd_studio_device *device
    )
{
    pmd_studio_mix_matrices_reset(device);

    // reset ring buffers
    for (unsigned int i = 0 ; i < device->num_ring_buffers ; i++ )
    {
        pmd_studio_device_delete_ring_buffer(&device->ring_buffers[i], device->studio);
    }

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
    if (!dev->input_stream_active)
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

dlb_pmd_success
pmd_studio_device_add_ring_buffer(
        unsigned int startchannel,         // output channel index starting at 0
        unsigned int num_channels,         // number of channels (1/2)
        unsigned int pcm_bufsize,
        pmd_studio *studio,
        pmd_studio_ring_buffer_struct **assigned_struct
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    unsigned int i;
    unsigned int first_free_ring_buffer;
    
    if (device->num_ring_buffers >= MAX_RING_BUFFERS)
    {
        return(PMD_FAIL);
    }

    // Check that channel isn't already being used in existing ring buffer
    // Also find empty ring buffer slot
    first_free_ring_buffer = device->num_ring_buffers;
    for (i = 0 ; i < device->num_ring_buffers ; i++)
    {
        if (device->ring_buffers[i].handler == nullptr)
        {
            first_free_ring_buffer = i;
            break;
        }
        // Check for overlap in request ring buffer and existing ring buffer
        // The last check is to avoid the 0,0 case which causes the last channel index to underflow i.e. 0 + 0 -1 = max unsigned int
        if (((startchannel + num_channels - 1) >= device->ring_buffers[i].handler->startchannel) &&
            startchannel <= ((device->ring_buffers[i].handler->startchannel + device->ring_buffers[i].handler->num_channels - 1)) &&
            (device->ring_buffers[i].handler->num_channels > 0))
        {
            return(PMD_FAIL);            
        }
    }
    device->ring_buffers[first_free_ring_buffer].mutex.lock();
    device->ring_buffers[first_free_ring_buffer].handler = new PMDStudioDeviceRingBufferHandler(startchannel, num_channels, pcm_bufsize);
    
    (*assigned_struct) = &device->ring_buffers[first_free_ring_buffer];

    // Register mutex to mdout for future use (when stopping metadata output)
    // mdout->ring_buffer_handle_parent_mutex = &device->ring_buffers[first_free_ring_buffer].mutex;
    
    device->ring_buffers[first_free_ring_buffer].mutex.unlock();

    // if we've needed to use a new ring buffer then increment total
    if (first_free_ring_buffer >= device->num_ring_buffers)
    {
        device->num_ring_buffers = first_free_ring_buffer + 1;
    }

    return(PMD_SUCCESS);
}


dlb_pmd_success
pmd_studio_device_delete_ring_buffer(
        pmd_studio_ring_buffer_struct *rbuf,
        pmd_studio *studio
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    int i;

    if (rbuf->handler == nullptr)
    {
        return(PMD_FAIL);
    }

    // Check that channel isn't already being used in existing ring buffer?
    rbuf->mutex.lock();
    delete rbuf->handler;
    rbuf->handler = nullptr;
    rbuf->mutex.unlock();

    // try and reduce device->num_ring_buffers
    for (i = device->num_ring_buffers - 1 ; i >= 0  ; i--)
    {
        if (device->ring_buffers[i].handler != nullptr)
        {
            break;
        }
    }
    device->num_ring_buffers = i + 1;

    return(PMD_SUCCESS);
}

void
pmd_studio_device_print_debug(
    pmd_studio *studio
    )
{
    pmd_studio_device *device;
    unsigned int i;

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
    printf("Number of ring buffers: %u\n", device->num_ring_buffers);
    for (i = 0 ; i < device->num_ring_buffers ; i++)
    {
        PMDStudioDeviceRingBufferHandler *handler = device->ring_buffers[i].handler;
        if(handler != nullptr){
            printf("Ring Buffer#%u\n---- --------\n", i+1);
            printf("\tStart Channel: %u\n", handler->startchannel);
            printf("\tNumber of Channel: %u\n", handler->num_channels);
            printf("\tindex: %u\n", handler->index);
            printf("\tBuffer Size: %u\n", handler->pcmbufsize);
        }
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
