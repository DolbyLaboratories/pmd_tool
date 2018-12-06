/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#ifndef PMD_ED2_STREAM_DESCRIPTION_H
#define PMD_ED2_STREAM_DESCRIPTION_H

/**
 * @file pmd_edt.h
 * @brief internal represenation of ED2 stream description payload information
 *
 * ED2 is built on top of Dolby E which can carry a maximum of 8 PCM
 * channels.  Modern object-audio streams are larger, often 16
 * channels, but potentially unlimited.  This means we must deliver
 * audio in multiple parallel streams which have to be synchronized
 * together.
 *
 * The ED2 stream description information outlines the topology of the
 * ED2 streams used to deliver one content experience.
 *
 * Note that the stream description structure will be broken down into
 * different payloads transmitted with each individual stream.  This
 * structure contains the overall top-level view.
 */

#include "pmd_signals.h"

/**
 * @def PMD_MAX_STREAMS
 * @brief maximum number of possible ED2 streams we need to maintain
 */
#define PMD_MAX_STREAMS (16)


/**
 * @brief information about a single ED2 stream
 *
 * This is the structure that is encoded as a PMD payload in
 * the bitstream tagged onto an individual ED2 stream.
 */
typedef struct
{
    dlb_pmd_de_program_config   config;      /**< DE stream config */
    uint8_t                     compression; /**< additional compression of DE audio */
} pmd_ed2_stream_description;


/**
 * @brief information about a single ED2 stream
 *
 * This is the structure that is encoded as a PMD payload in
 * the bitstream tagged onto an individual ED2 stream.
 */
typedef struct
{
    uint8_t                    count;     /**< total number of streams (1-16) representing 0-15) */
    dlb_pmd_frame_rate         rate;      /**< frame rate */
    pmd_ed2_stream_description streams[PMD_MAX_STREAMS]; /**< individual stream information */
    uint32_t                   streams_read; /**< bitmap of streams read this frame */
} pmd_esd;


#endif /* PMD_ED2_STREAM_DESCRIPTION_H */

