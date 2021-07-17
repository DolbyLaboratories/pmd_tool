/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file pa_play.c
 * @brief implementation of mode that PLAYS audio to an output device
 */


#include "model.h"
#include "buffer.h"
#include "pa.h"
#include "pa_play.h"

#include "md_writer.h"

#include "pcm_vsync_timer.h"
#include "dlb_wave/include/dlb_wave.h"
#include "dlb_wave/include/dlb_wave_int.h"


/**
 * @brief state of the player
 */
typedef struct
{
    md_writer mdw;         /**< metadata writer, if we're augmenting audio with metadata */
    dlb_wave_file source;  /**< input .wav file */
    pa_state *state;       /**< portaudio manager state */
    buffer b;              /**< .wav file PCM buffer */
    dlb_pmd_bool good;     /**< player set up properly? */
} pa_player;
    

/**
 * @brief helper function to open input .wav file
 */
static inline
dlb_pmd_success            /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
open_input_file
    (pa_player *p          /**< [in] player struct */
    ,Args *args            /**< [in] command-line arguments */
    ,unsigned int *nc      /**< [out] number of channels in .wav file */
    )
{
    int r = dlb_wave_open_read(&p->source, args->file_in, NULL);
    if (DLB_RIFF_OK != r) return PMD_FAIL;
    *nc = p->source.format.channel_count;

    if (p->source.format.sample_rate != 48000)
    {
        printf("ERROR: only 48 kHz sample rate supported\n");
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


/**
 * @brief initialize player state
 */
static inline
dlb_pmd_success            /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
player_init
    (pa_player *p          /**< [in] player to initialize */
    ,Args *args            /**< [in] command-line arguments */
    )
{
    unsigned int nc;
    unsigned int ic;

    memset(p, '\0', sizeof(*p));

    if (open_input_file(p, args, &nc)) goto error1;
    if (pa_channel_count(args->out_device, &ic, NULL)) goto error2;
    if (buffer_init(&p->b, nc, args->frame_size)) goto error2;
    if (md_writer_init(&p->mdw, args, nc)) goto error3;    
    if (pa_state_init(&p->state, args, 0, nc)) goto error4;
    if (pa_start(p->state)) goto error5;
    p->good = 1;

    return PMD_SUCCESS;

  error5: pa_state_finish(p->state);
  error4: md_writer_finish(&p->mdw);
  error3: buffer_finish(&p->b);
  error2: dlb_wave_close(&p->source);
  error1: return PMD_FAIL;
}


/**
 * @brief tear down player
 */
static inline
void
player_finish
    (pa_player *p              /**< [in] player state struct to finish */
    )
{
    if (p->good)
    {
        pa_stop(p->state);
        pa_state_finish(p->state);    
        md_writer_finish(&p->mdw);   
        buffer_finish(&p->b);
        dlb_wave_close(&p->source);
        p->good = 0;
    }
}


dlb_pmd_success
pa_play
    (Args *args
    )
{
    dlb_pmd_success result  = PMD_FAIL;
    pa_player player;
    vsync_timer vt;
    size_t video_sync;
    size_t read;
    int res;
    
    vsync_timer_init(&vt, args->rate, args->skip_pcm_samples);
    if (!player_init(&player, args))
    {
        video_sync = 0;
        res = 0;
        for(;;)
        {
            while (0 == res)
            {
                buffer_reset(&player.b);
                res = dlb_wave_int_read(&player.source, &player.b.buf, args->frame_size, &read);
                video_sync = vsync_timer_add_samples(&vt, read);
                if (args->md_file_in || args->server_port)
                {
                    md_writer_write(&player.mdw, player.b.channeldata, read, video_sync);
                }
                pa_state_feed(player.state, &player.b.buf, read);
            }
            if (!args->loop_playback)
            {
                break;
            }
            /* loop back to start */
            dlb_wave_seek_to_frame(&player.source, DLB_WAVE_LOCATION_MIN);    
            res = 0;
        }
    }
    
    player_finish(&player);
    return result;
}



