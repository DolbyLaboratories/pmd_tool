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
