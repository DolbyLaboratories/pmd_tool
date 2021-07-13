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

/**
 * @file pmd_realtime.c
 * @brief simple PCM+PMD real-time PMD reader/writer using portaudio
 *
 * This tool will perform PCM+PMD processing on a live audio feed
 * using portaudio, and will operate in one of three modes
 *
 *   - read audio device and save to .wav file.  (pmd_tool can then extract PMD)
 *   - play audio .wav file
 *   - read audio, add PMD and play it out again
 */

#include "args.h"
#include "pa.h"
#include "pa_play.h"
#include "pa_capture.h"
#include "pa_pipe.h"


/**
 * @brief execute the select mode
 */
static
dlb_pmd_success    /** @return success of mode execution */
do_mode
    (Args *args
    )
{
    switch (args->mode)
    {
        case MODE_LIST:     return pa_list();
        case MODE_PLAY:     return pa_play(args);
        case MODE_CAPTURE:  return pa_capture(args);
        case MODE_PIPE:     return pa_pipe(args);
        default:            return PMD_FAIL;
    }
}


/**
 * @brief main entry point
 */
int                         /** @return exit code */
main
    (int argc               /**< [in] number of command-line arguments */
    ,const char *argv[]     /**< [in] array of command-line arguments */
    )
{
    Args args;
    return chkargs(&args, argc, argv)
        || pa_init()
        || do_mode(&args)
        || pa_finish();
}
