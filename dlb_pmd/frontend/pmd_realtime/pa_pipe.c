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
 * @file pa_pipe.c
 * @brief implementation of mode that reads data from one device and sends it to another
 */


#include "model.h"
#include "buffer.h"
#include "pa.h"
#include "pa_pipe.h"
#include "md_reader.h"
#include "md_writer.h"
#include "pmd_os.h"

#include "pcm_vsync_timer.h"
#include "dlb_wave/include/dlb_wave.h"
#include "dlb_wave/include/dlb_wave_int.h"


/**
 * @brief type of the portaudio piper state
 */
typedef struct
{
    md_reader mdr;        /**< [in] metadata reader, if capturing input metadata */
    md_writer mdw;        /**< [in] metadata writer, if sending new metadata output */
    pa_state *state;      /**< [in] portaudio manager state */
    buffer b;             /**< [in] buffer */
    volatile int running; /**< [in] boolean used to detect CTRL-C events */
} pa_piper;
    

/**
 * @brief initialize the piper structure
 */
static inline
dlb_pmd_success           /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
piper_init
    (pa_piper *p          /**< [in] piper struct to initialize */
    ,Args *args           /**< [in] command-line arguments */
    )
{
    unsigned int ic;
    unsigned int oc;

    memset(p, '\0', sizeof(*p));

    p->running = 1;
    pmd_ctrlc_handle(&p->running);

    if (pa_channel_count(args->in_device,  &ic,  NULL)) return PMD_FAIL;
    if (pa_channel_count(args->out_device, NULL, &oc))  return PMD_FAIL;

    if (ic < oc) oc = ic;
    if (args->channel_count != NO_CHAN)
    {
        if (args->channel_count < ic) ic = args->channel_count;
    }

    if (md_reader_init(&p->mdr, args, ic)) goto error1;
    if (buffer_init(&p->b, ic, args->frame_size)) goto error2;
    if (md_writer_init(&p->mdw, args, ic)) goto error3;
    if (pa_state_init(&p->state, args, ic, ic)) goto error4;
    if (pa_start(p->state)) goto error5;

    return PMD_SUCCESS;

  error5: pa_state_finish(p->state);
  error4: md_writer_finish(&p->mdw);
  error3: buffer_finish(&p->b);
  error2: md_reader_finish(&p->mdr);
  error1:
    return PMD_FAIL;
}


/**
 * @brief tear down the portaudio piper state
 */
static inline
void
piper_finish
    (pa_piper *p               /**< [in] state to tear down */
    )
{
    pa_stop(p->state);
    pa_state_finish(p->state);    
    md_writer_finish(&p->mdw);
    md_reader_finish(&p->mdr);
    buffer_finish(&p->b);
}


dlb_pmd_success
pa_pipe
    (Args *args
    )
{
    dlb_pmd_success result = PMD_FAIL;
    pa_piper piper;
    vsync_timer vt;
    size_t video_sync;
    size_t read;
    
    vsync_timer_init(&vt, args->rate, args->skip_pcm_samples);
    if (!piper_init(&piper, args))
    {
        video_sync = 0;
        while (piper.running)
        {
            buffer_reset(&piper.b);
            read = pa_state_read(piper.state, &piper.b.buf, args->frame_size);
            video_sync = vsync_timer_add_samples(&vt, read);
            if (args->md_file_out || args->md_file_in)
            {
                md_reader_feed(&piper.mdr, piper.b.channeldata, read, &video_sync);
            }
            if (args->md_file_in)
            {
                md_writer_write(&piper.mdw, piper.b.channeldata, read, video_sync);
            }
            pa_state_feed(piper.state, &piper.b.buf, read);
        }
    }

    piper_finish(&piper);
    return result;
}



