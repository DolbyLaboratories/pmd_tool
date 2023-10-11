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

#include "portaudio.h"
#include "pmd_studio.h"
#include "pmd_studio_common_defs.h"
#include "pmd_studio_device.h"
#include "am824_framer.h"
#include <iostream>
#include <string.h>
#include "pmd_studio_device_pvt.h"
#include "mix_matrix.h"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include "pa_win_wasapi.h"
#include <stdint.h>
#if PA_USE_ASIO
#include "pa_asio.h"
#endif
#endif

#define PA_SAMPLE_TYPE  paInt32
#define BIT_DEPTH 24
#define BYTES_PER_SAMPLE 3
#define SAMPLE_RATE 48000
#define DEFAULT_FRAMES_PER_BUFFER (128)
#define DEFAULT_LATENCY (0.0)

const char
*pmd_studio_device_get_settings_menu_name(void)
{
    return("Audio Device Configuration");
}

/* Private Functions */

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
void
pmd_studio_set_device_names(
        pmd_studio_device *device
)
{
    int     i, numDevices;
    const   PaDeviceInfo *deviceInfo;

    device->num_input_devices = 0;
    device->num_output_devices = 0;
    numDevices = Pa_GetDeviceCount();
    //printf ("numDevices: %d\n", numDevices);
    if (numDevices < 0)
    {
        pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_GetDeviceCount returned negative value");
    }

    for (i = 0; i < numDevices; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0)
        {
            strncpy(device->input_device_names[device->num_input_devices], deviceInfo->name, MAX_DEVICE_NAME_LENGTH - 1);
/*            if (i == Pa_GetDefaultInputDevice())   This causes trouble because name is no longer the same as the actual device name
            {
                strncat(device->input_device_names[device->num_input_devices], " (Default)", MAX_DEVICE_NAME_LENGTH - 1);
            }*/
            device->input_device_indices[device->num_input_devices++] = i;

//            printf("input #%d: %s = %d\n", device->num_input_devices - 1, device->input_device_names[device->num_input_devices - 1], i);
        }

        if (deviceInfo->maxOutputChannels > 0)
        {
            strncpy(device->output_device_names[device->num_output_devices], deviceInfo->name, MAX_DEVICE_NAME_LENGTH - 1);
/*            if (i == Pa_GetDefaultOutputDevice())
            {
                strncat(device->output_device_names[device->num_output_devices], " (Default)", MAX_DEVICE_NAME_LENGTH - 1);
            }*/
            device->output_device_indices[device->num_output_devices++] = i;

//            printf("output #%d: %s = %d\n", device->num_output_devices - 1, device->output_device_names[device->num_output_devices - 1], i);
        }
    }
}

static
dlb_pmd_success
pmd_studio_set_input_device_name(
    const char *name,
    pmd_studio *studio
    )
{
    unsigned int i;
    pmd_studio_device *device = pmd_studio_get_device(studio);

    for (i = 0 ; (i < device->num_input_devices) ; i++)
    {
        if (!strcmp(name,device->input_device_names[i]))
        {
            break;
        }
    }
    if (i < device->num_input_devices)
    {
        device->inputParameters.device = device->input_device_indices[i];
        device->input_device_name = device->input_device_names[i];
        return(PMD_SUCCESS);
    }
    else
    {
        return(PMD_FAIL);
    }

}

static
dlb_pmd_success
pmd_studio_set_output_device_name(
    const char *name,
    pmd_studio *studio
    )
{
    unsigned int i;
    pmd_studio_device *device = pmd_studio_get_device(studio);

    for (i = 0 ; (i < device->num_output_devices) ; i++)
    {
        if (!strcmp(name,device->output_device_names[i]))
        {
            break;
        }
    }
    if (i < device->num_output_devices)
    {
        device->outputParameters.device = device->output_device_indices[i];
        device->output_device_name = device->output_device_names[i];
        return(PMD_SUCCESS);
    }
    else    
    {
        return(PMD_FAIL);
    }

}


static
int
recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData
               )
{
    int *readPtr = (int *)inputBuffer;
    int *writePtr = (int *)outputBuffer;
    unsigned int i,j;
    pmd_studio_device *device = (pmd_studio_device *)userData;
    struct pmd_studio_comp_mix_matrix *cm = device->active_comp_mix_matrix;

    (void) timeInfo;
    (void) statusFlags;

    if( inputBuffer != NULL )
    {
        // First run mixing matrix in compressed form
        for( i = 0; i < framesPerBuffer ; i++ )
        {
            for (j = 0 ; j < (unsigned int)device->outputParameters.channelCount ; j++)
            {
                writePtr[j] = 0;
            }
            for (j = 0 ; j < cm->size ; j++)
            {
                writePtr[cm->output[j]] += readPtr[cm->input[j]] * cm->coef[j]; /* TODO: clarify mixed-types arithmetic -- where do we truncate? */
            }
            writePtr += device->outputParameters.channelCount;
            readPtr += device->inputParameters.channelCount;
        }
        // Now add ring buffers
        device->ring_buffer_list->WriteRingBuffers((int32_t *)outputBuffer, device->outputParameters.channelCount, framesPerBuffer);
    }
    return paContinue; // returning paComplete would terminate stream
}

static
void
pmd_studio_device_list(
    void
	)
{
	unsigned int i;
	const   PaDeviceInfo *deviceInfo;
    pmd_studio_device device;
    PaError err;


    err = Pa_Initialize();

    if( err != paNoError )
    {
        pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_Initialize failed");
    }

    pmd_studio_set_device_names(&device);


	printf("PortAudio version: 0x%08X\n", Pa_GetVersion());
	printf("Version text: '%s'\n", Pa_GetVersionText());


	printf("Number of input devices = %d\n", device.num_input_devices);
	for (i = 0; i < device.num_input_devices; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(device.input_device_indices[i]);

		printf("------------------- Input Device #%d --------------------\n", i);

#ifdef WIN32
        {   /* Use wide char on windows, so we can show UTF-8 encoded device names */
            wchar_t wideName[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, deviceInfo->name, -1, wideName, MAX_PATH - 1);
            wprintf(L"Name                        = %s\n", wideName);
        }
#else
        printf("Name                        = %s\n", deviceInfo->name);
#endif
        printf("Max input channels          = %d\n", deviceInfo->maxInputChannels);
        printf("Default low input latency   = %4.4f\n", deviceInfo->defaultLowInputLatency);
        printf("Default high input latency  = %4.4f\n", deviceInfo->defaultHighInputLatency);
        printf("Host API                    = %s\n", Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
#ifdef WIN32
#if PA_USE_ASIO
        /* ASIO specific latency information */
        if (Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paASIO) {
            long minLatency, maxLatency, preferredLatency, granularity;

            err = PaAsio_GetAvailableLatencyValues(i,
                &minLatency, &maxLatency, &preferredLatency, &granularity);
            if (err != paNoError)
            {
                pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Error getting latency values");
            }

            printf("ASIO minimum buffer size    = %ld\n", minLatency);
            printf("ASIO maximum buffer size    = %ld\n", maxLatency);
            printf("ASIO preferred buffer size  = %ld\n", preferredLatency);

            if (granularity == -1)
                printf("ASIO buffer granularity     = power of 2\n");
            else
                printf("ASIO buffer granularity     = %ld\n", granularity);
        }
#endif /* PA_USE_ASIO */
#endif /* WIN32 */
        printf("--------------------------------------------------------\n");
    }

    for (i = 0; i < device.num_output_devices; i++)
    {    
        deviceInfo = Pa_GetDeviceInfo(device.output_device_indices[i]);

        printf("------------------- Output Device #%d --------------------\n", i);

#ifdef WIN32
        {   /* Use wide char on windows, so we can show UTF-8 encoded device names */
            wchar_t wideName[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, deviceInfo->name, -1, wideName, MAX_PATH - 1);
            wprintf(L"Name                        = %s\n", wideName);
        }
#else
        printf("Name                        = %s\n", deviceInfo->name);
#endif

        printf("Max output channels         = %d\n", deviceInfo->maxOutputChannels);
        printf("Default low output latency   = %4.4f\n", deviceInfo->defaultLowOutputLatency);
        printf("Default high output latency  = %4.4f\n", deviceInfo->defaultHighOutputLatency);
        printf("Host API                    = %s\n", Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
#ifdef WIN32
#if PA_USE_ASIO
        /* ASIO specific latency information */
        if (Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paASIO) {
            long minLatency, maxLatency, preferredLatency, granularity;

            err = PaAsio_GetAvailableLatencyValues(i,
                &minLatency, &maxLatency, &preferredLatency, &granularity);
            if (err != paNoError)
            {
                pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Error getting latency values");
            }

            printf("ASIO minimum buffer size    = %ld\n", minLatency);
            printf("ASIO maximum buffer size    = %ld\n", maxLatency);
            printf("ASIO preferred buffer size  = %ld\n", preferredLatency);

            if (granularity == -1)
                printf("ASIO buffer granularity     = power of 2\n");
            else
                printf("ASIO buffer granularity     = %ld\n", granularity);
        }
#endif /* PA_USE_ASIO */
#endif /* WIN32 */
        printf("---------------------------------------------------------\n");
    }

    Pa_Terminate();
}


/* Public Functions */

void
pmd_studio_device_update_metadata_outputs(
    pmd_studio *studio
)
{
    /* Do nothing as only channel based metadata outputs supported in this version */
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
 	PaError err;
	pmd_studio_device *device;
    pmd_studio_mix_matrix_array mix_matrix;
    const   PaDeviceInfo *inputdevinfo;
    const   PaDeviceInfo *outputdevinfo;
    dlb_pmd_bool status;

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
        device->ring_buffer_list = nullptr;
        #if defined(_WIN32) || defined(_WIN64)
        struct PaWasapiStreamInfo wasapiInfo;
        WSADATA data;
        WSAStartup( MAKEWORD( 2, 2 ), &data );
        #endif

        err = Pa_Initialize();

        if( err != paNoError )
        {
            pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_Initialize failed");
        }

        /* Create the list of names base information from Portaudio */
        pmd_studio_set_device_names(device);
    }
    else
    {
        device = *retdevice;
        // This is a device update. We need to recreate the stream using the new settings

        // Stop any existing outputs
        pmd_studio_outputs_stop_all_metadata_outputs(studio);

        // Close the current stream before opening the new one with the new settings
        if (device->stream != nullptr){
            err = Pa_CloseStream( device->stream );
            if( err != paNoError )
            {
                pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_CloseStream Failed");
            }
            device->stream = nullptr;
        }
        if (device->ring_buffer_list != nullptr)
        {
            delete device->ring_buffer_list;
            device->ring_buffer_list = nullptr;
        }
        
    }

    status = pmd_studio_set_input_device_name(settings->input_device, studio);
    if (status != PMD_SUCCESS)
    {
        pmd_studio_information("Input Device Name not recognized - using default input device");
        //fprintf(stderr, "Input Device Error, Name not recognized - using default input device\n");
        device->inputParameters.device = paNoDevice;
    }

    status = pmd_studio_set_output_device_name(settings->output_device, studio);
    if (status != PMD_SUCCESS)
    {
        pmd_studio_information("Output Device Name not recognized - using default output device");
        //fprintf(stderr, "output Device Error, Name not recognized - using default output device\n");
        device->outputParameters.device = paNoDevice;
    }

    if (device->inputParameters.device == paNoDevice) {
        status = pmd_studio_set_input_device_name(Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->name, studio);
    }
    if (device->inputParameters.device == paNoDevice) {
        //Pa_Terminate();
        return(PMD_FAIL);
    }
    if (device->outputParameters.device == paNoDevice) {
        status = pmd_studio_set_output_device_name(Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->name, studio);
    }
    if (device->outputParameters.device == paNoDevice) {
        //Pa_Terminate();
        return(PMD_FAIL);
    }

    inputdevinfo = Pa_GetDeviceInfo(device->inputParameters.device);
    outputdevinfo = Pa_GetDeviceInfo(device->outputParameters.device);

    if (inputdevinfo->maxInputChannels == 0)
    {
        //printf("Input: Device #%u = %s, Channels = %u\n", device->inputParameters.device, inputdevinfo->name, inputdevinfo->maxInputChannels);
    	pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Input device has zero channels");
        return(PMD_FAIL);
    }

    if (outputdevinfo->maxOutputChannels == 0)
    {
        //printf("Input: Device #%u = %s, Channels = %u\n", device->outputParameters.device, outputdevinfo->name, outputdevinfo->maxOutputChannels);
    	pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Output device has zero channels");
        return(PMD_FAIL);
    }

    // Make sure that the number of channels selected does not exceed the capabilities of the device or global maximum
    settings->num_channels = std::min(settings->num_channels, std::min(inputdevinfo->maxInputChannels, std::min(outputdevinfo->maxOutputChannels, MAX_INPUT_CHANNELS)));

    pmd_studio_mix_matrix_reset(mix_matrix);
    compress_mix_matrix(mix_matrix, &device->comp_mix_matrix1, settings->num_channels, settings->num_channels);
    pmd_studio_mix_matrix_reset(mix_matrix);
    compress_mix_matrix(mix_matrix, &device->comp_mix_matrix2, settings->num_channels, settings->num_channels);
    device->active_comp_mix_matrix = &device->comp_mix_matrix1;

    device->inputParameters.channelCount = settings->num_channels;
    device->inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    
    // Make 0.0 latency a special value whereby we choose default low latency
    if (common_settings->latency > 0.0)
    {
    	device->inputParameters.suggestedLatency = common_settings->latency;
    	device->outputParameters.suggestedLatency = common_settings->latency;
    }
    else
    {
    	device->inputParameters.suggestedLatency = inputdevinfo->defaultLowInputLatency;
    	device->outputParameters.suggestedLatency = outputdevinfo->defaultLowOutputLatency;
	}
    device->outputParameters.channelCount = settings->num_channels;  
    device->outputParameters.sampleFormat = PA_SAMPLE_TYPE;
	device->outputParameters.hostApiSpecificStreamInfo = NULL;
    device->am824_mode = settings->am824_mode;

#if defined(_WIN32) || defined(_WIN64)

	if (Pa_GetHostApiInfo(inputdevinfo->hostApi)->type == paWASAPI) {
		wasapiInfo.size = sizeof(PaWasapiStreamInfo);
		wasapiInfo.hostApiType = paWASAPI;
		wasapiInfo.version = 1;
		wasapiInfo.flags = (paWinWasapiExclusive | paWinWasapiThreadPriority);
		wasapiInfo.channelMask = 0;
		wasapiInfo.hostProcessorOutput = NULL;
		wasapiInfo.hostProcessorInput = NULL;
		wasapiInfo.threadPriority = eThreadPriorityProAudio;
		device->inputParameters.hostApiSpecificStreamInfo = (&wasapiInfo);
		printf("Detected WASAPI device and setting exclusive mode\n");
	}
	else {
#endif
		device->inputParameters.hostApiSpecificStreamInfo = NULL;
#if defined(_WIN32) || defined(_WIN64)
	}
#endif

	err = Pa_IsFormatSupported(&device->inputParameters, &device->outputParameters, SAMPLE_RATE);
	if (err != paNoError)
	{
    	//Pa_Terminate();
		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_IsFormatSupported Failed");
	}

	//printf("Pa_IsFormatSupported succeeded\n");

    // Check for automatic frames per buffer selection
    if (common_settings->frames_per_buffer == AUTO_FRAMES_PER_BUFFER)
    {
        common_settings->frames_per_buffer = DEFAULT_FRAMES_PER_BUFFER;
    }
/*
    printf("Frames_per_buffer = %u\n", common_settings->frames_per_buffer);
    printf("Input: Device = %u, Channels = %u, Latency = %f\n", device->inputParameters.device, device->inputParameters.channelCount, device->inputParameters.suggestedLatency);
    printf("Output: Device = %u, Channels = %u, Latency = %f\n", device->outputParameters.device, device->outputParameters.channelCount, device->outputParameters.suggestedLatency);
*/
    device->ring_buffer_list = new PMDStudioRingBufferList(device->outputParameters.channelCount);

    err = Pa_OpenStream(
              &device->stream,
              &device->inputParameters,
              &device->outputParameters,               
              SAMPLE_RATE,
              common_settings->frames_per_buffer,
              paClipOff,    
              recordCallback,
              device);

    if( err != paNoError )
    {
    	//Pa_Terminate();
    	pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_OpenStream Failed");
    }

    err = Pa_StartStream( device->stream );
    if( err != paNoError )
    {
    	//Pa_Terminate();
   		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_StartStream Failed");
    }
    
    return(PMD_SUCCESS);
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

    device->ring_buffer_list->AddRingBuffer(ring_buffer_channel, ring_buffer_num_channels, frame_rate);
    return PMD_SUCCESS;
 }


dlb_pmd_success
pmd_studio_device_update_mix_matrix(
	pmd_studio *studio
	)
{
    pmd_studio_mix_matrix_array mix_matrix;
	pmd_studio_device *dev = pmd_studio_get_device(studio);

	if (dev->active_comp_mix_matrix == &dev->comp_mix_matrix1)
	{
		if (pmd_studio_audio_outputs_get_mix_matrix(mix_matrix, studio) == PMD_SUCCESS)
		{
            compress_mix_matrix(mix_matrix, &dev->comp_mix_matrix2, dev->inputParameters.channelCount, dev->outputParameters.channelCount);
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
            compress_mix_matrix(mix_matrix, &dev->comp_mix_matrix1, dev->inputParameters.channelCount, dev->outputParameters.channelCount);
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
    (void)channel;
    (void)studio;
    return(true);
}

dlb_pmd_success
pmd_studio_get_input_device_name(
    char **name,
    pmd_studio *studio,
    int index
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    // Check value of index
    // if(index < 0 || index > device->num_output_devices){
    //     throw std::length_error("Requested output device index is outside the device name array");
    // }
    *name = device->input_device_names[index];
    // current device or existing devices
    if (index == CURRENT_DEVICE)
    {
        if (!device->input_device_name)
        {
            return(PMD_FAIL);
        }
        else
        {
            *name = device->input_device_name;
        }
    }
    else
    {
        if (index < (int)device->num_input_devices)
        {
            *name = device->input_device_names[index];
        }
        else
        {
            // This is normal behaviour and signals that there are no more devices
            *name = NULL;
        }
    }
    return(PMD_SUCCESS);
}

dlb_pmd_success
pmd_studio_get_output_device_name(
    char **name,
    pmd_studio *studio,
    int index
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    // // Check value of index
    // if(index < 0 || index > device->num_output_devices){
    //     throw std::length_error("Requested output device index is outside the device name array");
    // }

    // *name = device->output_device_names[index];
    // current device or existing devices
    if (index == CURRENT_DEVICE)
    {
        if (!device->output_device_name)
        {
            return(PMD_FAIL);
        }
        else
        {
            *name = device->output_device_name;
        }
    }
    else
    {
        if (index < (int)device->num_output_devices)
        {
            *name = device->output_device_names[index];
        }
        else
        {
            // This is normal behaviour and signals that there are no more devices
            *name = NULL;
        }
    }
    return(PMD_SUCCESS);
}

void pmd_studio_device_option(pmd_studio *studio, const char *option
)
{
	if (!strcmp(option, "list"))
	{
		pmd_studio_device_list();
	}
}


dlb_pmd_success
pmd_studio_device_reset(
    pmd_studio_device *device
    )
{
    pmd_studio_mix_matrices_reset(device);

    // reset ring buffers
    device->ring_buffer_list->Reset();

    return(PMD_SUCCESS);
}

PMDStudioRingBufferList *pmd_studio_device_get_ring_buffer_list(pmd_studio *studio)
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    return device->ring_buffer_list;
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

    device->ring_buffer_list->PrintDebug();

}



void
pmd_studio_device_close(
	pmd_studio_device *s
	)
{
	PaError err;

	if (!s)
	{
		return;
	}

    err = Pa_CloseStream( s->stream );
    if( err != paNoError )
    {
    	pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Error closing stream");
    }

    err = Pa_Terminate();
    if( err != paNoError )
    {
   		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Error terminating stream");
    }
}
