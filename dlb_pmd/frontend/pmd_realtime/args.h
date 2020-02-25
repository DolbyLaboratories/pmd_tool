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

/**
 * @file args.h
 * @brief command-line argument struct and parser for simple PCM+PMD real-time PMD reader/writer 
 */

#ifndef PMD_REALTIME_ARGS_H_
#define PMD_REALTIME_ARGS_H_

#include "dlb_pmd_api.h"
#include "dlb_pmd_klv.h"

/**
 * @brief mode the realtime app will run in
 */
typedef enum
{
    MODE_LIST,        /**< simply list audio devices */
    MODE_PLAY,        /**< stream from file to output device */
    MODE_CAPTURE,     /**< stream from input device to file */
    MODE_PIPE,        /**< stream from input device to output device */
} mode;


/**
 * @def NO_CHAN
 * @brief symbolic constant meaning channel hasn't been specified on command line
 */
#define NO_CHAN (~0u)


/**
 * @def symbolic constant indicating no device specified
 */
#define NO_DEVICE (~0u)


/**
 * @brief command-line parameter structure
 */
typedef struct 
{
    mode mode;                  /**< execution mode */
    const char *file_in;        /**< input .wav file, or NULL */
    const char *file_out;       /**< output .wav file, or NULL */
    const char *md_file_in;     /**< input. xml metadata file, or NULL */
    const char *md_file_out;    /**< output .xml metadata file, or NULL */
    const char *server_service; /**< name of request filename HTTP server responds to */
    int server_port;            /**< port number of HTTP server listener, 0 if unset */
    unsigned int in_device;     /**< input portaudio device number, or #NO_DEVICE */
    unsigned int out_device;    /**< output portaudio device number, or #NO_DEVICE */
    unsigned int channel_count; /**< channel count */
    unsigned int time;          /**< length of time */
    unsigned int interleaved;   /**< 1: interleaved; 0: non-interleaved */
    unsigned int frame_size;    /**< read/write block size */

    dlb_pmd_frame_rate rate;    /**< PCM frame rate */
    dlb_klvpmd_universal_label ul; /**< which PMD KLV 16-byte key to use */
    unsigned int pmd_chan;      /**< start channel of PMD */
    dlb_pmd_bool subframe_mode; /**< single-channel, or pair mode SMPTE 337m wrapping for PMD? */
    dlb_pmd_bool mark_pcm_blocks; /**< mark empty PCM blocks with NULL SMPTE 337m frames? */
    unsigned int skip_pcm_samples; /**< skip this many samples before beginning to write */
    unsigned int vsync;            /**< video sync offset */
    dlb_pmd_bool loop_playback;    /**< loop playback file indefinitely */
    dlb_pmd_bool filter_md;        /**< only dump metadata files if change */
    dlb_pmd_bool sadm;           /**< prefer sADM metadata format? */

    int desired_input_latency;  /**< desired portaudio input latency */
    int desired_output_latency; /**< desired portaudio output latency */
} Args;


/**
 * @brief parse command-line arguments
 */
dlb_pmd_success               /** @return PMD_SUCCESS if args OK, PMD_FAIL otherwise */
chkargs 
    (Args *args               /**< [in/out] args struct to populate */
    ,int argc                 /**< [in] number of arguments */
    ,const char **argv        /**< [in] array of arguments */
    );


#endif /* PMD_REALTIME_ARGS_H_ */

