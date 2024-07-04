/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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

/**
 * @file pa.c
 * @brief portaudio interface for PCM+PMD realtime app
 */

#include "pa.h"
#include "cyclic_sample_buffer.h"
#include "portaudio.h"
#if defined (_MSC_VER)
#  include <Windows.h>
#  include "pa_win_wmme.h"
#  include "pa_win_wasapi.h"
#  include "pa_win_ds.h"
#  include "pa_asio.h"
#elif defined (__linux__)
#  include "pa_linux_alsa.h"
#  include "pa_jack.h"
#  include <pthread.h>
#  include <alsa/asoundlib.h>
#elif defined (__APPLE__)
#  include "pa_mac_core.h"
#endif
#include <stdio.h>
#include "pmd_os.h"

#ifdef _MSC_VER
# define FRAME_QUEUE_SIZE (8)
# define snprintf _snprintf
#else
# define FRAME_QUEUE_SIZE (3)
#endif


/**
 * @brief selection of different host type parameters
 */
typedef union
{
#if defined(_MSC_VER)
    PaWinMmeStreamInfo wmme;
    PaWasapiStreamInfo wasapi;
    PaWinDirectSoundStreamInfo ds;
    PaAsioStreamInfo asio;
#elif defined (__linux__)
    PaAlsaStreamInfo alsa;
#elif defined (__APPLE__)
    PaMacCoreStreamInfo mac;
#else
    int dummy;
#endif
} host_sink_params;


/**
 * @brief portaudio state
 */
struct pa_state
{
    PaStreamParameters   pa_sink_params;
    PaStreamParameters   pa_source_params;
    host_sink_params     pa_host_sink_params;

    PaStream            *stream;

    dlb_pmd_bool         pa_start_data_ready;
    dlb_pmd_bool         pa_started;
    dlb_pmd_bool         interleaved;
    
    cyclic_sample_buffer input;
    cyclic_sample_buffer output;

    unsigned int         n_sink_slots;
    unsigned int         n_source_slots;
    unsigned int         n_output_pins;

    unsigned int         sourced_samples;  /**< count of samples read from device */
    unsigned int         dropped_samples;  /**< source samples we had to drop */
    unsigned int         sinked_samples;   /**< count of samples sent to device */

    pmd_semaphore        wait;
};


/**
 * @brief called regularly by the portaudio library to read input (if enabled)
 * or write output or both.
 */
static
int
pa_stream_callback
    (const void *devinput
    ,void *devoutput
    ,unsigned long frameCount
    ,const PaStreamCallbackTimeInfo *timeInfo
    ,PaStreamCallbackFlags statusFlags
    ,void *userData
    )
{
    /* find number of bytes per sample */
    pa_state *state = (pa_state *)userData;
    unsigned int i = 0;

    (void)statusFlags;
    (void)timeInfo;

    if (state->n_sink_slots > 0)
    {
        dlb_buffer sink;
        void *sinkptrs[MAX_CHANNELS];
        unsigned int pasamplesize = state->input.samplesize;
        unsigned int read;
        uint8_t *addr = (uint8_t *)devoutput;

        sink.nchannel = state->n_sink_slots;
        if (state->interleaved)
        {
            for (i = 0; i != state->n_sink_slots; ++i)
            {
                sinkptrs[i] = addr;
                addr += pasamplesize;
            }
            sink.ppdata = sinkptrs;
            sink.nstride = sink.nchannel;
            memset(sink.ppdata[0], '\0', pasamplesize * frameCount * state->n_sink_slots);
        }
        else
        {
            sink.ppdata = (void **)devoutput;
            sink.nstride = 1;
            for (i = 0; i != state->n_sink_slots; ++i)
            {
                memset(sink.ppdata[i], '\0', pasamplesize * frameCount);
            }
        }

        read = csb_read_samples(&state->input, frameCount, &sink, pasamplesize);
        if (read < frameCount)
        {
            printf("PA output underflow: %lu samples\n", frameCount - read);
        }
        state->sinked_samples += read;
    }

    if (state->n_source_slots > 0)
    {
        dlb_buffer source;
        void *srcptrs[MAX_CHANNELS];
        unsigned int pasamplesize = state->output.samplesize;
        unsigned int written;
        uint8_t *addr = (uint8_t *)devinput;

        source.nchannel = state->n_source_slots;
        if (state->interleaved)
        {
            for (i = 0; i != state->n_source_slots; ++i)
            {
                srcptrs[i] = addr;
                addr += pasamplesize;
            }
            source.ppdata = srcptrs;
            source.nstride = source.nchannel;
        }
        else
        {
            source.ppdata = (void **)devinput;
            source.nstride = 1;
        }
        written = csb_write_samples(&state->output, frameCount, &source, pasamplesize);
        state->sourced_samples += written;
        state->dropped_samples += (frameCount - written);
    }

    pmd_semaphore_signal(&state->wait);
    return paContinue;
}


static
void
discard_alsa_errors
    (const char *file
    ,int line
    ,const char *function
    ,int err
    ,const char *fmt
    ,...
    )
{
    (void)file;
    (void)line;
    (void)function;
    (void)err;
    (void)fmt;
}


/**
 * @brief configure source slots
 */
static
PaStreamParameters *
configure_source_slots
    (pa_state *state
    ,Args *args
    ,unsigned int num_channels
    )
{
    PaStreamParameters *source_params = NULL;
    const PaDeviceInfo *devinfo;

    state->n_source_slots = num_channels;
    if (state->n_source_slots > 0)
    {
        /* now configure the output cyclic sample buffer */
        csb_init(&state->output,
                 3,
                 state->n_source_slots,
                 FRAME_QUEUE_SIZE * args->frame_size,
                 args->interleaved ? state->n_source_slots : 1,
                 args->interleaved ? 1 : args->frame_size);

        /* Finally, set up the portaudio parameters
         */
        state->pa_source_params.device = args->in_device;
        devinfo = Pa_GetDeviceInfo(state->pa_source_params.device);
        state->pa_source_params.channelCount = state->n_source_slots;
        state->pa_source_params.sampleFormat = paInt24;
        if (!args->interleaved)
        {
            state->pa_source_params.sampleFormat |= paNonInterleaved;
        }

        state->pa_source_params.hostApiSpecificStreamInfo = NULL;
        state->pa_source_params.suggestedLatency = (PaTime)args->desired_input_latency / 1000.0;
        if (args->desired_input_latency == 0)
        {
            state->pa_source_params.suggestedLatency = devinfo->defaultHighInputLatency;
        }
        else if (args->desired_input_latency < 0)
        {
            state->pa_source_params.suggestedLatency = devinfo->defaultLowInputLatency;
        }
        source_params = &state->pa_source_params;
    }
    return source_params;
}


/**
 * @brief configure portaudio sink parameters
 *
 * This function is used exclusively by portaudio_reconfig
 */
static
PaStreamParameters *
configure_sink_slots
    (pa_state     *state
    ,Args         *args
    ,unsigned int num_channels
    ,unsigned int stride
    )
{
    PaStreamParameters  *sink_params = NULL;
    const PaDeviceInfo  *devinfo;
    const PaHostApiInfo *hostinfo;

    state->n_sink_slots = num_channels > stride ? num_channels : stride;
    if (state->n_sink_slots > 0)
    {
        csb_init(&state->input,
                 3, /* size of paInt24 */
                 state->n_sink_slots,
                 FRAME_QUEUE_SIZE * args->frame_size,
                 args->interleaved ? state->n_sink_slots : 1,
                 args->interleaved ? 1 : args->frame_size);

        state->pa_sink_params.device = args->out_device;
        devinfo = Pa_GetDeviceInfo(args->out_device);
        if (NULL == devinfo)
        {
            printf("Device %d not available", args->out_device);
            return NULL;
        }

        hostinfo = Pa_GetHostApiInfo(devinfo->hostApi);
        assert(NULL != hostinfo);
        switch (hostinfo->type)
        {
#if defined (_MSC_VER)
        case paMME:
            {
                PaWinMmeStreamInfo *wmme = &state->pa_host_sink_params.wmme;
                wmme->size = sizeof(PaWinMmeStreamInfo);
                wmme->hostApiType = paMME;
                wmme->version = 1;
                wmme->flags = paWinMmeDontThrottleOverloadedProcessingThread | paWinMmeUseLowLevelLatencyParameters;
                wmme->framesPerBuffer = args->frame_size;
                wmme->bufferCount = 2;
                state->pa_sink_params.hostApiSpecificStreamInfo = wmme;
            }
            break;
        case paWASAPI:
            {
                PaWasapiStreamInfo *wasapi = &state->pa_host_sink_params.wasapi;
                wasapi->size = sizeof(PaWasapiStreamInfo);
                wasapi->hostApiType = paWASAPI;
                wasapi->version = 1;
                wasapi->flags = paWinWasapiThreadPriority|paWinWasapiExclusive;
                wasapi->threadPriority = eThreadPriorityProAudio;
                state->pa_sink_params.hostApiSpecificStreamInfo = wasapi;
            }
            break;
        case paASIO:        break;
        case paDirectSound: break;
            /* WDM-KS also todo, requires upgrading portaudio DLL */
#elif defined (__linux__)
        case paALSA: break;
        case paOSS:  break;
        case paJACK: break;
#elif defined (__APPLE__)
        case paCoreAudio: break;
#endif
        default:
            state->pa_sink_params.hostApiSpecificStreamInfo = NULL;
            break;
        }

        state->pa_sink_params.channelCount = state->n_sink_slots;
        state->pa_sink_params.sampleFormat = paInt24;
        if (!args->interleaved)
        {
            state->pa_sink_params.sampleFormat |= paNonInterleaved;
        }

        state->pa_sink_params.suggestedLatency = (PaTime)args->desired_output_latency / 1000.0;
        if (args->desired_output_latency == 0)
        {
            state->pa_sink_params.suggestedLatency = devinfo->defaultHighOutputLatency;
        }
        else if (args->desired_output_latency < 0)
        {
            state->pa_sink_params.suggestedLatency = devinfo->defaultLowOutputLatency;
        }
        sink_params = &state->pa_sink_params;
    }
    return sink_params;
}


dlb_pmd_success
pa_start
    (pa_state *state
    )
{
    PaError err = Pa_StartStream(state->stream);
    if (err != paNoError)
    {
        printf("Pa_StartStream() returned error \"%s\"", Pa_GetErrorText(err));
        Pa_CloseStream(state->stream);
        state->stream = NULL;
        return PMD_FAIL;
    }
    state->pa_started = 1;
    state->pa_start_data_ready = 0;
    return PMD_SUCCESS;
}


/**
 * @brief stop portaudio thread if it is running
 */
dlb_pmd_success
pa_stop
    (pa_state *state
    )
{
    if (state->pa_started)
    {
        /* stop PA if it is running */
        PaError err = Pa_CloseStream(state->stream);
        if (err != paNoError)
        {
            printf("Pa_CloseStream() returned error \"%s\"", Pa_GetErrorText(err));
            return PMD_FAIL;
        }
        state->stream = NULL;
        state->pa_started = 0;
    }
    return PMD_SUCCESS;
}


/* ---------------------------- public api ---------------------------- */


dlb_pmd_success
pa_init
    (void
    )
{
    PaError err;

    #ifdef __linux__
    /* throw away ALSA debug messages when parsing ALSA conf */
    snd_lib_error_set_handler(discard_alsa_errors);
    #endif

    err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("Portaudio_Initialize() returned error \"%s\"", Pa_GetErrorText(err));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


dlb_pmd_success
pa_finish
    (void
    )
{
    Pa_Terminate();
    return PMD_SUCCESS;
}


dlb_pmd_success
pa_channel_count
    (unsigned int device
    ,unsigned int *input_chans
    ,unsigned int *output_chans
    )
{
    const PaDeviceInfo *devinfo;

    devinfo = Pa_GetDeviceInfo(device);
    if (!devinfo)
    {
        printf("ERROR: non-existent device %d\n", device);
        return PMD_FAIL;
    }
    if (input_chans)
    {
        printf("INPUTCHANS: %u\n", devinfo->maxInputChannels);
        *input_chans = devinfo->maxInputChannels;
    }
    if (output_chans)
    {
        *output_chans = devinfo->maxOutputChannels;
    }
    
    return PMD_SUCCESS;
}


dlb_pmd_success
pa_list
    (void
    )
{
    PaDeviceIndex count = Pa_GetDeviceCount();
    int i;

    printf("Portaudio information\n");
    printf("=====================\n");
    printf("Number Devices: %d\n", count);

    for (i = 0; i < count; ++i)
    {
        const PaDeviceInfo  *devinfo;
        const PaHostApiInfo *apiinfo;
        char name[128];

        /* dynamically generate a string describing the requested device */
        devinfo = Pa_GetDeviceInfo(i);
        apiinfo = Pa_GetHostApiInfo(devinfo->hostApi);
        snprintf(name, sizeof(name), "%s: %s", apiinfo->name, devinfo->name);

        printf("Device %02d: \"%s\" (max input chans:%u, max output chans:%u)\n",
               i, name, devinfo->maxInputChannels, devinfo->maxOutputChannels);
    }
    return PMD_SUCCESS;
}


/**
 * @brief Initialize PortAudio library.
 */
dlb_pmd_success
pa_state_init
    (pa_state   **stateptr
    ,Args        *args
    ,unsigned int input_chans
    ,unsigned int output_chans
    )
{
    const double sample_rate = 48000.0;
    const PaDeviceInfo *devinfo;
    PaStreamParameters *source_params = NULL;
    PaStreamParameters *sink_params = NULL;
    unsigned int pa_buffer_size = 0;
    PaStreamCallbackFlags paFlags = 0;
    unsigned int num_channels;
    PaError err = 0;

    pa_state *state = (pa_state*)malloc(sizeof(pa_state));
    *stateptr = state;
    if (!state)
    {
        printf("ERROR: could not allocate\n");
        return PMD_FAIL;
    }
    
    memset(state, '\0', sizeof(pa_state));

    if (pmd_semaphore_init(&state->wait, "pawait", 1)) return PMD_FAIL;

    /* compute the input configuration from the routing config */
    state->interleaved = !!args->interleaved;
    if (args->in_device != NO_DEVICE)
    {
        devinfo = Pa_GetDeviceInfo(args->in_device);
        if (!devinfo)
        {
            printf("ERROR: non-existent device %d\n", args->in_device);
            return PMD_FAIL;
        }
        num_channels = devinfo->maxInputChannels;
        if (num_channels > input_chans) num_channels = input_chans;
        source_params = configure_source_slots(state, args, num_channels);
    }
    if (args->out_device != NO_DEVICE)
    {
        devinfo = Pa_GetDeviceInfo(args->out_device);
        if (!devinfo)
        {
            printf("ERROR: non-existent device %d\n", args->out_device);
            return PMD_FAIL;
        }
        num_channels = devinfo->maxOutputChannels;
        if (num_channels > output_chans) num_channels = output_chans;
        sink_params = configure_sink_slots(state, args, num_channels, num_channels);
    }
    if (source_params || sink_params)
    {
        err = Pa_IsFormatSupported(source_params, sink_params, sample_rate);
        if (err != paFormatIsSupported)
        {
            printf("Pa_IsFormatSupported() returned error \"%s\"", Pa_GetErrorText(err));
            state->stream = NULL;
            return PMD_FAIL;
        }
        
        paFlags
            = paClipOff | paDitherOff                  // don't mess with the samples
            | paPrimeOutputBuffersUsingStreamCallback; // don't prime with zero-fill
        
    }

    pa_buffer_size = args->frame_size
        ? args->frame_size
        : paFramesPerBufferUnspecified;
    
    err = Pa_OpenStream(&state->stream,
                        source_params,
                        sink_params,
                        sample_rate,
                        pa_buffer_size,
                        paFlags,
                        pa_stream_callback,
                        state);
    
    if (err != paNoError)
    {
        printf("Pa_OpenStream() returned error \"%s\"", Pa_GetErrorText(err));
        state->stream = NULL;
        return PMD_FAIL;
    }
    
    return PMD_SUCCESS;
}


void
pa_state_finish
    (pa_state *state
    )
{
    if (state->stream)
    {
        Pa_CloseStream(state->stream);
        state->stream = NULL;
    }
    if (state->n_sink_slots)
    {
        csb_finish(&state->input);
        state->n_sink_slots = 0;
    }
    if (state->n_source_slots)
    {
        csb_finish(&state->output);
        state->n_source_slots = 0;
    }
    pmd_semaphore_finish(&state->wait);
    free(state);
}


void
pa_state_feed
    (pa_state   *state
    ,dlb_buffer *input
    ,size_t      num_samples
    )
{
    num_samples -= csb_write_samples(&state->input, num_samples, input, sizeof(uint32_t));
    while (num_samples)
    {
        pmd_semaphore_wait(&state->wait);
        num_samples -= csb_write_samples(&state->input, num_samples, input, sizeof(uint32_t));
    }
}


size_t
pa_state_read
    (pa_state   *state
    ,dlb_buffer *output
    ,size_t      num_samples
    )
{
    pmd_semaphore_wait(&state->wait);
    return (size_t)csb_read_samples(&state->output, num_samples, output, sizeof(uint32_t));
}



