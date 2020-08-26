/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
#include "pmd_studio_limits.h"
#include "pmd_studio_device.h"
#include "am824_framer.h"

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
#define MAX_RING_BUFFERS 10
#define MAX_COMP_MIX_MATRIX_SIZE (MAX_INPUT_CHANNELS * MAX_OUTPUT_CHANNELS)

struct pmd_studio_device_ring_buffer
{
    unsigned int startchannel;
    unsigned int num_channels;
    unsigned int index;
    uint32_t *pcmbuf;
    unsigned int pcmbufsize; // number of 32 bit words in pcmbuf
    AM824Framer am824framer;
};

struct pmd_studio_comp_mix_matrix
{
    unsigned int size;
    unsigned int input[MAX_COMP_MIX_MATRIX_SIZE];
    unsigned int output[MAX_COMP_MIX_MATRIX_SIZE];
    float coef[MAX_COMP_MIX_MATRIX_SIZE];
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
    struct pmd_studio_device_ring_buffer ring_buffers[MAX_RING_BUFFERS];
    unsigned int 					num_channels;
    dlb_pmd_bool                    am824_mode;
};

/* Private Functions */

static
void
compress_mix_matrix(pmd_studio_mix_matrix_array mix_matrix, struct pmd_studio_comp_mix_matrix *comp_mix_matrix)
{
    unsigned int i,j;

    comp_mix_matrix->size = 0;
    for (i = 0 ; i < MAX_INPUT_CHANNELS ; i++)
    {
        for (j = 0 ; j < MAX_OUTPUT_CHANNELS ; j++)
        {
            if (mix_matrix[i][j] != 0.0)
            {
                comp_mix_matrix->input[comp_mix_matrix->size] = i;
                comp_mix_matrix->output[comp_mix_matrix->size] = j;
                comp_mix_matrix->coef[comp_mix_matrix->size++] = mix_matrix[i][j];
                if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
                {
                    break;
                }
            }
            if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
            {
                break;
            }
        }
        if (comp_mix_matrix->size == MAX_COMP_MIX_MATRIX_SIZE)
        {
            break;
        }
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
    unsigned int i,j,k;
    pmd_studio_device *s = (pmd_studio_device *)userData;
    struct pmd_studio_comp_mix_matrix *cm = s->active_comp_mix_matrix;

    (void) timeInfo;
    (void) statusFlags;

    if( inputBuffer != NULL )
    {
        // First run mixing matrix in compressed form
        for( i = 0; i < framesPerBuffer ; i++ )
        {
            for (j = 0 ; j < MAX_OUTPUT_CHANNELS ; j++)
            {
                writePtr[j] = 0;
            }
            for (j = 0 ; j < cm->size ; j++)
            {
                writePtr[cm->output[j]] += readPtr[cm->input[j]] * cm->coef[j]; /* TODO: clarify mixed-types arithmetic -- where do we truncate? */
            }
            writePtr += MAX_OUTPUT_CHANNELS;
            readPtr += MAX_INPUT_CHANNELS;
        }
	    // Now add ring buffers

        if (s->am824_mode)
        {
	        for (i = 0 ; i < s->num_ring_buffers ; i++)
	        {
	            if (s->ring_buffers[i].pcmbufsize > 0)
	            {
	                writePtr = outputBuffer;
	                writePtr += s->ring_buffers[i].startchannel;
	                for( j = 0; j < framesPerBuffer ; j++ )
	                {
	                    for (k = 0 ; k < s->ring_buffers[i].num_channels ; k++)
	                    {
	                    	getAM824Sample(&s->ring_buffers[i].am824framer, s->ring_buffers[i].pcmbuf[s->ring_buffers[i].index] >> 8, (uint8_t *)writePtr);
/*	                    	if (s->ring_buffers[i].pcmbuf[s->ring_buffers[i].index])
	                    	{
	                    		printf("%08x -> %08x\n", s->ring_buffers[i].pcmbuf[s->ring_buffers[i].index], *writePtr);
	                    	}
*/
	                    	s->ring_buffers[i].index++;
	                        writePtr++;
	                        if (s->ring_buffers[i].index >= s->ring_buffers[i].pcmbufsize)
	                        {
	                            s->ring_buffers[i].index = 0;
	                        }
	                    }
	                    writePtr += MAX_OUTPUT_CHANNELS - s->ring_buffers[i].num_channels;
	                }
	            }
	        }        	
        }
        else
        {
	        for (i = 0 ; i < s->num_ring_buffers ; i++)
	        {
	            if (s->ring_buffers[i].pcmbufsize > 0)
	            {
	                writePtr = outputBuffer;
	                writePtr += s->ring_buffers[i].startchannel;
	                for( j = 0; j < framesPerBuffer ; j++ )
	                {
	                    for (k = 0 ; k < s->ring_buffers[i].num_channels ; k++)
	                    {
	                        *writePtr++ = s->ring_buffers[i].pcmbuf[s->ring_buffers[i].index++];    
	                        if (s->ring_buffers[i].index >= s->ring_buffers[i].pcmbufsize)
	                        {
	                            s->ring_buffers[i].index = 0;
	                        }
	                    }
	                    writePtr += MAX_OUTPUT_CHANNELS - s->ring_buffers[i].num_channels;
	                }
	            }
	        }
	    }
    }
    return paContinue; // returning paComplete would terminate stream
}

/* Public Functions */

dlb_pmd_success
pmd_studio_device_init(
	pmd_studio_device **s,
	int input_device,
	int output_device,
	unsigned int num_channels,
	float latency,
	unsigned int frames_per_buffer,
	dlb_pmd_bool am824_mode
	)
{
 	PaError err;
	pmd_studio_device *device;
    pmd_studio_mix_matrix_array mix_matrix;

    *s = malloc(sizeof(pmd_studio_device));
    if(!*s)
    {
        return(PMD_FAIL);
    }
    device = *s;

	#if defined(_WIN32) || defined(_WIN64)
	struct PaWasapiStreamInfo wasapiInfo;
	WSADATA data;
	WSAStartup( MAKEWORD( 2, 2 ), &data );
	#endif

    device->inputParameters.device = input_device;
    device->outputParameters.device = output_device;
    device->num_channels = num_channels;
    device->am824_mode = am824_mode;

    pmd_studio_mix_matrix_unity(mix_matrix);
    compress_mix_matrix(mix_matrix, &device->comp_mix_matrix1);
    pmd_studio_mix_matrix_reset(mix_matrix);
    compress_mix_matrix(mix_matrix, &device->comp_mix_matrix2);
    device->active_comp_mix_matrix = &device->comp_mix_matrix1;

	err = Pa_Initialize();

	if( err != paNoError )
	{
		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_Initialize failed");
	}

	if (device->inputParameters.device == paNoDevice) {
		device->inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	}
    if (device->inputParameters.device == paNoDevice) {
    	Pa_Terminate();
        return(PMD_FAIL);
    }
	if (device->outputParameters.device == paNoDevice) {
		device->outputParameters.device = Pa_GetDefaultOutputDevice(); /* default input device */
	}
    if (device->outputParameters.device == paNoDevice) {
    	Pa_Terminate();
        return(PMD_FAIL);
    }

    device->inputParameters.channelCount = num_channels;
    device->inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    if (latency > 0.0)
    {
    	device->inputParameters.suggestedLatency = latency;
    	device->outputParameters.suggestedLatency = latency;
    }
    else
    {
    	device->inputParameters.suggestedLatency = Pa_GetDeviceInfo( device->inputParameters.device )->defaultLowInputLatency;
    	device->outputParameters.suggestedLatency = Pa_GetDeviceInfo( device->outputParameters.device )->defaultLowOutputLatency;
	}
    device->outputParameters.channelCount = num_channels;  
    device->outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    device->outputParameters.suggestedLatency = Pa_GetDeviceInfo( device->outputParameters.device )->defaultLowOutputLatency;
	device->outputParameters.hostApiSpecificStreamInfo = NULL;

#if defined(_WIN32) || defined(_WIN64)

	if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(device->inputParameters.device)->hostApi)->type == paWASAPI) {
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
    	Pa_Terminate();
		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_IsFormatSupported Failed");
	}

	printf("Pa_IsFormatSupported succeeded\n");

    err = Pa_OpenStream(
              &device->stream,
              &device->inputParameters,
              &device->outputParameters,               
              SAMPLE_RATE,
              frames_per_buffer,
              paClipOff,    
              recordCallback,
              device);

    if( err != paNoError )
    {
    	Pa_Terminate();
    	pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_OpenStream Failed");
    }

    err = Pa_StartStream( device->stream );
    if( err != paNoError )
    {
    	Pa_Terminate();
   		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_StartStream Failed");
    }
    
    return(PMD_SUCCESS);
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
            compress_mix_matrix(mix_matrix, &dev->comp_mix_matrix2);
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
            compress_mix_matrix(mix_matrix, &dev->comp_mix_matrix1);
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

void
pmd_studio_device_list(
	void
	)
{
	int     i, numDevices, defaultDisplayed;
	const   PaDeviceInfo *deviceInfo;

	printf("PortAudio version: 0x%08X\n", Pa_GetVersion());
	printf("Version text: '%s'\n", Pa_GetVersionText());

	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0)
	{
		pmd_studio_error(PMD_STUDIO_ERR_PA_ERROR, "Pa_GetDeviceCount returned negative value");
	}

	printf("Number of devices = %d\n", numDevices);
	for (i = 0; i < numDevices; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(i);

		printf("------------------- device #%d --------------------\n", i);

		/* Mark global and API specific default devices */
		defaultDisplayed = 0;
		if (i == Pa_GetDefaultInputDevice())
		{
			printf("[ Default Input");
			defaultDisplayed = 1;
		}
		else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice)
		{
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
			printf("[ Default %s Input", hostInfo->name);
			defaultDisplayed = 1;
		}

		if (i == Pa_GetDefaultOutputDevice())
		{
			printf("[ Default Output");
			defaultDisplayed = 1;
		}
		else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice)
		{
			const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
			printf("[ Default %s Output", hostInfo->name);
			defaultDisplayed = 1;
		}

		if (defaultDisplayed)
			printf(" ]\n");

		/* print device info fields */
#ifdef WIN32
		{   /* Use wide char on windows, so we can show UTF-8 encoded device names */
			wchar_t wideName[MAX_PATH];
			MultiByteToWideChar(CP_UTF8, 0, deviceInfo->name, -1, wideName, MAX_PATH - 1);
			wprintf(L"Name                        = %s\n", wideName);
		}
#else
		printf("Name                        = %s\n", deviceInfo->name);
#endif
		printf("Host API                    = %s\n", Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
		printf("Max input channels          = %d\n", deviceInfo->maxInputChannels);
		printf("Max output channels         = %d\n", deviceInfo->maxOutputChannels);

		printf("Default low input latency   = %4.4f\n", deviceInfo->defaultLowInputLatency);
		printf("Default high input latency  = %4.4f\n", deviceInfo->defaultHighInputLatency);

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

		printf("Default sample rate         = %8.2f\n", deviceInfo->defaultSampleRate);
		printf("---------------------------------------------------\n");

	}
}


dlb_pmd_success
pmd_studio_device_add_ring_buffer(
        unsigned int startchannel,         // output channel index starting at 0
        unsigned int num_channels,         // number of channels (1/2)
        uint32_t pcmbuf[],                 // base pointer for ring buffer 
        unsigned int pcmbufsize,           // size is in 32 bit words/samples
        unsigned int *ring_buffer_handle,  // returned handle that can be used to disable/delete later
        pmd_studio *studio
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    unsigned int i;
    unsigned int first_free_ring_buffer;


    if (device->num_ring_buffers == MAX_RING_BUFFERS)
    {
        return(PMD_FAIL);
    }

    // Check that channel isn't already being used in existing ring buffer
    // Also find empty ring buffer slot
    first_free_ring_buffer = device->num_ring_buffers;
    for (i = 0 ; i < device->num_ring_buffers ; i++)
    {
        if (device->ring_buffers[i].pcmbufsize == 0)
        {
            first_free_ring_buffer = i;
        }
        // Check for overlap in request ring buffer and existing ring buffer
        // The last check is to avoid the 0,0 case which causes the last channel index to underflow i.e. 0 + 0 -1 = max unsigned int
        if (((startchannel + num_channels - 1) >= device->ring_buffers[i].startchannel) &&
            startchannel <= ((device->ring_buffers[i].startchannel + device->ring_buffers[i].num_channels - 1)) &&
            (device->ring_buffers[i].num_channels > 0))
        {
            return(PMD_FAIL);            
        }
    }

    device->ring_buffers[first_free_ring_buffer].pcmbuf = pcmbuf;
    device->ring_buffers[first_free_ring_buffer].pcmbufsize = pcmbufsize;
    device->ring_buffers[first_free_ring_buffer].startchannel = startchannel;
    device->ring_buffers[first_free_ring_buffer].num_channels = num_channels;
    device->ring_buffers[first_free_ring_buffer].index = 0;
    if (device->am824_mode)
    {
    	 if (AM824Framer_init(&device->ring_buffers[first_free_ring_buffer].am824framer, (uint8_t)num_channels, 24, AM824_LITTLE_ENDIAN)
    	 	!= AM824_ERR_OK)
    	{
    		return(PMD_FAIL);
    	}
    }

    *ring_buffer_handle = first_free_ring_buffer;

    // if we've needed to use a new ring buffer then increment total
    if (first_free_ring_buffer >= device->num_ring_buffers)
    {
        device->num_ring_buffers = first_free_ring_buffer + 1;
    }

    return(PMD_SUCCESS);
}


dlb_pmd_success
pmd_studio_device_delete_ring_buffer(
        unsigned int ring_buffer_handle,
        pmd_studio *studio
    )
{
    pmd_studio_device *device = pmd_studio_get_device(studio);
    int i;

    if (ring_buffer_handle >= device->num_ring_buffers)
    {
        return(PMD_FAIL);
    }

    // Check that channel isn't already being used in existing ring buffer?

    device->ring_buffers[ring_buffer_handle].num_channels = 0;
    device->ring_buffers[ring_buffer_handle].pcmbufsize = 0;
    device->ring_buffers[ring_buffer_handle].pcmbuf = 0;
    device->ring_buffers[ring_buffer_handle].startchannel = 0;
    device->ring_buffers[ring_buffer_handle].index = 0;

    // try and reduce device->num_ring_buffers
    for (i = device->num_ring_buffers - 1 ; i >= 0  ; i--)
    {
        if (device->ring_buffers[i].pcmbufsize > 0)
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
        printf("Ring Buffer#%u\n---- --------\n", i+1);
        printf("\tStart Channel: %u\n", device->ring_buffers[i].startchannel);
        printf("\tNumber of Channel: %u\n", device->ring_buffers[i].num_channels);
        printf("\tindex: %u\n", device->ring_buffers[i].index);
        printf("\tBuffer Size: %u\n", device->ring_buffers[i].pcmbufsize);
    }
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

