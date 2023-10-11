/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
