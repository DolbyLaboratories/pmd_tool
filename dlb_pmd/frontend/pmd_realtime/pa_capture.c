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

/**
 * @file pa_capture.c
 * @brief implementation of the CAPTURE mode of the realtime PMD tool
 */

#include "model.h"
#include "buffer.h"
#include "pa.h"
#include "pa_capture.h"
#include "md_reader.h"
#include "pmd_os.h"

#include "dlb_pmd_pcm.h"
#include "dlb_pmd_xml_file.h"
#include "dlb_wave/include/dlb_wave.h"
#include "dlb_wave/include/dlb_wave_int.h"

#include <string.h>

#define SAMPLE_RATE (48000)
#define OUTPUT_PCM_BIT_DEPTH (24)


/**
 * @brief Portaudio recorder state structure
 */
typedef struct
{
    md_reader mdsink;
    dlb_wave_file sink;
    pa_state *state;
    buffer b;
    int listen_only;
    int good;
    int running;
} pa_recorder;
    

/**
 * @brief helper function to open output .wav file if required
 */
static inline
dlb_pmd_success           /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
open_output_file
    (pa_recorder *p       /**< [in] recorder state */
    ,Args *args           /**< [in] command-line arguments */
    ,unsigned int nc      /**< [in] number of channels to write */
    )
{
    p->listen_only = 1;
    if (args->file_out)
    {
        int res = dlb_wave_open_write(&p->sink, args->file_out, 0, SAMPLE_RATE,
                                      nc, 0, OUTPUT_PCM_BIT_DEPTH);
        if (DLB_RIFF_OK != res)
        {
            printf("ERROR: could not open file \"%s\"\n", args->file_out);
            return PMD_FAIL;
        }
        
        res = dlb_wave_begin_data(&p->sink);
        if (res != DLB_RIFF_OK)
        {
            printf("ERROR: failed to begin output file's data chunk \"%s\"", args->file_out);
            dlb_wave_close(&p->sink);
            return PMD_FAIL;
        }
        p->listen_only = 0;
    }
    return PMD_SUCCESS;
}


/**
 * @brief helper function to close output .wav file, if opened
 */
static inline
void
close_output_file
    (pa_recorder *p       /**< [in] recorder state */
    )
{
    if (!p->listen_only)
    {
        dlb_wave_end_data(&p->sink);
        dlb_wave_close(&p->sink);
    }
}


/**
 * @brief helper function to initialize the portaudio capture state
 */
static
dlb_pmd_success            /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
recorder_init
    (pa_recorder *p        /**< [in] capture state to initialize */
    ,Args *args            /**< [in] command-line arguments */
    )
{
    unsigned int nc;

    p->good = 0;

    memset(p, '\0', sizeof(*p));
    p->running = 1;
    pmd_ctrlc_handle(&p->running);

    if (pa_channel_count(args->in_device, &nc, NULL)) goto error1;
    if (open_output_file(p, args, nc)) goto error1;
    if (buffer_init(&p->b, nc, args->frame_size)) goto error2;
    if (md_reader_init(&p->mdsink, args, nc)) goto error3;
    if (pa_state_init(&p->state, args, nc, 0)) goto error4;
    if (pa_start(p->state)) goto error5;

    p->good = 1;
    return PMD_SUCCESS;

  error5: pa_state_finish(p->state);
  error4: md_reader_finish(&p->mdsink);
  error3: buffer_finish(&p->b);
  error2: close_output_file(p);
  error1: return PMD_FAIL;
}


/**
 * @brief helper function to tidy up and close down the recorder
 */
static inline
void
recorder_finish
   (pa_recorder *p                /** [in] recorder state to finish */
    )
{
    if (p->good)
    {
        pa_stop(p->state);
        pa_state_finish(p->state);    
        md_reader_finish(&p->mdsink);
        buffer_finish(&p->b);
        close_output_file(p);
        p->good = 0;
    }
}


dlb_pmd_success
pa_capture
    (Args *args
    )
{
    dlb_pmd_success result = PMD_FAIL;
    pa_recorder recorder;
    size_t video_sync;
    size_t read;
    int res;
    
    if (!recorder_init(&recorder, args))
    {
        video_sync = 0;
        res = 0;
        while (0 == res && recorder.running)
        {
            buffer_reset(&recorder.b);
            read = pa_state_read(recorder.state, &recorder.b.buf, args->frame_size);
            if (!recorder.listen_only)
            {
                res = dlb_wave_int_write(&recorder.sink, &recorder.b.buf, read);
            }
            if (args->md_file_out)
            {
                md_reader_feed(&recorder.mdsink, recorder.b.channeldata, read, &video_sync);
            }
        }
    }
    
    recorder_finish(&recorder);
    return result;
}



