/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2019 by Dolby Laboratories,
 *                Copyright (C) 2017-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_ED2_STREAM_DESCRIPTION_H
#define PMD_ED2_STREAM_DESCRIPTION_H

/**
 * @file pmd_esd.h
 * @brief internal representation of ED2 stream description payload information
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

