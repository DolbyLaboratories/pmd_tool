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
 * @file buffer.h
 * @brief simple abstraction to handle dlb_buffer and memory
 */


#include "dlb_buffer/include/dlb_buffer.h"
#include <string.h>

#ifndef MAX_CHANNELS
#  define MAX_CHANNELS (256)
#endif


/**
 * @brief encapsulate everything to do with allocating dlb_buffer
 */
typedef struct
{
    uint32_t *channeldata;
    uint32_t *ppdata[MAX_CHANNELS];
    dlb_buffer buf;
} buffer;


/**
 * @brief initialize a dlb_buffer buffer
 */
static inline
dlb_pmd_success
buffer_init
    (buffer *b                  /**< [in] buffer structure to initialise */
    ,unsigned int nchannel      /**< [in] number of channels in buffer */
    ,unsigned int block_size    /**< [in] number of samplesets in buffer */
    )
{
    unsigned int i;

    b->channeldata   = malloc(nchannel * block_size * sizeof(uint32_t));
    b->buf.nchannel  = nchannel;
    b->buf.nstride   = nchannel;
    b->buf.data_type = DLB_BUFFER_INT_LEFT;
    b->buf.ppdata    = (void**)b->ppdata;

    for (i = 0; i != nchannel; ++i)
    {
        b->ppdata[i] = &b->channeldata[i];
    }
    return PMD_SUCCESS;
}


/**
 * @brief reset the pointers of a dlb_buffer to point to beginning of
 * actual data area
 */
static inline
void
buffer_reset
    (buffer *buf              /**< [in] buffer to reset */
    )
{
    unsigned int i;
    
    for (i = 0; i != buf->buf.nchannel; ++i)
    {
        buf->ppdata[i] = &buf->channeldata[i];
    }
}


/**
 * @brief shift data in buffer to start of buffer
 */
static inline
void
buffer_shift
    (buffer *buf            /**< [in] buffer to shift */
    ,size_t skip            /**< [in] start sample to keep */
    ,size_t amount          /**< [in] number of samples to keep */
    )
{
    size_t sample_set_size = sizeof(uint32_t) * buf->buf.nstride;
    memmove(buf->channeldata, buf->channeldata + skip * sample_set_size, sample_set_size * amount);
}

/**
 * @brief tidy up buffer
 */
static inline
void
buffer_finish
    (buffer *buf                   /**< [in] buffer to finish */
    )
{
    if (buf->channeldata)
    {
        free(buf->channeldata);
        buf->channeldata = NULL;
    }
}
