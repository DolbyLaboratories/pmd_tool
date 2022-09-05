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
 * @file cyclic_sample_buffer.h
 * @brief cyclicl buffer definition for feeding and reading from portaudio
 *
 * The trickiest part of this buffer is shuffling between 3- and 4- byte
 * representations of audio
 */


#ifndef __CYCLIC_SAMPLE_BUFFER_H_
#define __CYCLIC_SAMPLE_BUFFER_H_

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "dlb_buffer/include/dlb_buffer.h"

#define MAX_CHANNELS 255

/**
 * @brief cyclic sample buffer.
 *
 * The following numbers are all in counts of 'sample sets'
 *
 * - readpos always 'trails' writepos
 * - they loop round.
 */
typedef struct cyclic_sample_buffer
{
    unsigned int num_sets;   /**< size of data array (in sample sets) */
    unsigned int numchans;   /**< number of channels */
    unsigned int samplesize; /**< size of individual audio sample (in bytes) */
    unsigned int framesize;  /**< num samples before next channel, 1 means interleaved */

    uint8_t *data;           /**< data array holding the sample sets */
    uint8_t *end;            /**< first byte after end of data */
    uint8_t *readpos;        /**< read position within array */
    uint8_t *writepos;       /**< write position within array */

} cyclic_sample_buffer;


/**
 * @brief sign-extend 3-byte 'words' to longs
 *
 * Some platforms have 8-byte longs and some 4-bytes.
 * this utility function ensures little-endian platforms
 * are correct
 */
static inline
void
sign_extend_int
    (uint8_t *outp  /**< little-endian byte sequence with lsb 4-bytes set */
    )
{
    if (sizeof(int) > 4)
    {
        int word = outp[3] & 0x80 ? 0xff : 0x00;
        memset(outp, word, sizeof(int)-4);
    }
}


/**
 * @brief destroy a cyclic sample buffer
 *
 */
static inline
void
csb_finish
   (cyclic_sample_buffer *csb
   )
{
    if (NULL != csb->data)
    {
        free(csb->data);
        csb->data = NULL;
    }
}


/**
 * @brief initialise cyclic buffer
 */
static inline
void
csb_init
    (cyclic_sample_buffer *csb    /**< cyclic sample buffer to init */
    ,unsigned int sample_size     /**< bytes per individual audio sample */
    ,unsigned int num_channels    /**< number of channels in sample set */
    ,unsigned int num_samplesets  /**< number sample sets in buffer */
    ,unsigned int stride          /**< 1 = non-interleaved, >= num_channels = interleaved */
    ,unsigned int framesize       /**< framesize, 1 = interleaved */
    )
{
    unsigned int numbytes = num_channels * sample_size * num_samplesets;
    uint8_t *data = NULL;

    if (NULL != csb->data)
    {
        csb_finish(csb);
    }

    csb->num_sets = 0;
    csb->numchans = 0;
    csb->samplesize = 0;

    data = (uint8_t *)malloc(numbytes);

    if (NULL != data)
    {
        unsigned int chanstride =
            stride == 1
            ? sample_size * num_samplesets               /* non-interleaved */
            : sample_size * num_channels * num_samplesets;

        /* because we want this to be a lock-free communication
         * structure, we can only assume that individual word accesses
         * will be atomic.  This means that we must recompute the
         * different channel offsets every time we access the buffer.
         */

        csb->data = data;
        csb->end = data + chanstride;
        csb->readpos = data;
        csb->writepos = data;

        csb->num_sets = num_samplesets;
        csb->numchans = num_channels;
        csb->samplesize = sample_size;
        csb->framesize = framesize;
    }
}


/**
 * @brief read portaudio samples and deliver to dpf channels
 *
 * Note that we assume that the framecount is never larger
 * than the framesize of the buffer.  Otherwise we could not
 * copy non-interleaved buffers properly.
 *
 * Note that we assume that the number of output channels
 * equals the number of sink slots, which is the case with the
 * current portaudio arena.
 *
 */
static inline
void
csb_read_samples_priv
    (cyclic_sample_buffer *csb
    ,unsigned int sink_samplesize
    ,dlb_buffer *sink
    ,uint8_t *rp
    ,ptrdiff_t available_samples
    )
{
    const unsigned int numchannels = csb->numchans;

    ptrdiff_t i;
    unsigned int j;

    uint8_t *outp = (uint8_t *) sink->ppdata[0];

    if (csb->framesize == 1) /* buffer stores samples interleaved */
    {
        if (sink->nstride == (int)sink->nchannel) /* sink interleaved */
        {
            if (sink_samplesize == sizeof(int) && csb->samplesize == 3)
            {
                /* 32-bit audio gets rounded to 24-bit, and 24-bit audio is delivered
                 * unpacked in DPF, (PA uses 24-bit data).  In both cases, DPF sample
                 * size is 4 bytes and PA sample size is 3 bytes.
                 */
                for (i = 0; i < available_samples; ++i)
                {
                    for (j = 0; j < numchannels; ++j)
                    {
                        outp[0] = 0;
                        outp[1] = *rp++;
                        outp[2] = *rp++;
                        outp[3] = *rp++;
                        sign_extend_int(outp);                        
                        outp += sizeof(int);
                    }
                }
            }
            else
            {
                memmove(outp, rp, available_samples * sink_samplesize * numchannels);
            }
        }
        else
        {
            /* buffer interleaved, sink non-interleaved */
            /* @todo */
            abort();
        }
    }
    else
    {
        /* buffer stores data in non-interleaved format: device
         * presents input buffer as a set of pointers, one for each
         * slot
         */
        uint8_t *slotptrs[MAX_CHANNELS];

        for (j = 0; j != numchannels; ++j)
        {
            slotptrs[j] = rp;
            rp += csb->num_sets * csb->samplesize;
        }

        if (0 == slotptrs[0])
        {
            return;
        }

        if (sink->nstride == (int)sink->nchannel)
        {
            /* buffer stores data non-interleaved, but sink is interleaved */

            if (sink_samplesize == sizeof(int) && csb->samplesize == 3)
            {
                /* 32-bit audio gets rounded to 24-bit, and 24-bit audio is delivered
                 * unpacked in DPF, (PA uses 24-bit data).  In both cases, DPF sample
                 * size is 4 bytes and PA sample size is 3 bytes.
                 */
                for (j = 0; j < numchannels; ++j)
                {
                    uint8_t *srcp= (uint8_t *)slotptrs[j];
                    uint8_t *sinkp= (uint8_t *)sink->ppdata[j];
                    for (i = 0; i < available_samples; ++i)
                    {
                        sinkp[0] = 0;
                        sinkp[1] = srcp[0];
                        sinkp[2] = srcp[1];
                        sinkp[3] = srcp[2];
                        sign_extend_int(sinkp);
                        sinkp += sizeof(int);
                        srcp += 3;
                    }
                }
            }
            else
            {
                assert(sink_samplesize == csb->samplesize);

                for (j = 0; j < numchannels; ++j)
                {
                    uint8_t *srcp= (uint8_t *)slotptrs[j];
                    uint8_t *sinkp= (uint8_t *)sink->ppdata[j];
                    for (i = 0; i < available_samples; ++i)
                    {
                        memmove(sinkp, srcp, sink_samplesize);
                        sinkp += sink_samplesize;
                        srcp += sink_samplesize;
                    }
                }
            }
        }
        else
        {
            /* both buffer and sink are non-interleaved */
            /* @todo: problem of chunking! */
            if (sink_samplesize == sizeof(int) && csb->samplesize == 3)
            {
                for (j = 0; j < numchannels; ++j)
                {
                    uint8_t *srcp= (uint8_t *)slotptrs[j];
                    uint8_t *sinkp= (uint8_t *)sink->ppdata[j];
                    for (i = 0; i < available_samples; ++i)
                    {
                        sinkp[0] = 0;
                        sinkp[1] = srcp[0];
                        sinkp[2] = srcp[1];
                        sinkp[3] = srcp[2];
                        sinkp += sizeof(int);
                        srcp += 3;
                    }
                }
            }
            else
            {
                for (j = 0; j != numchannels; ++j)
                {
                    memmove(sink->ppdata[j], slotptrs[j],
                            sink_samplesize * available_samples);
                }
            }
        }
    }
}


static inline
void
csb_zero_samples
    (dlb_buffer   *sink
    ,unsigned int  sink_samplesize
    ,unsigned int  sample_count
    )
{
    unsigned int i;

    uint8_t *outp = (uint8_t *)sink->ppdata[0];

    if (sink->nstride == (int)sink->nchannel) /* sink interleaved */
    {
        memset(outp, 0, sample_count * sink_samplesize * sink->nchannel);
    }
    else
    {
        /* sink is non-interleaved */
        for (i = 0; i != sink->nchannel; ++i)
        {
            memset(sink->ppdata[i], 0, sample_count * sink_samplesize);
        }
    }
}


/**
 * @brief copy a given number of sample sets from buffer to output
 */
static inline
unsigned int                       /** @return number samples read */
csb_read_samples
    (cyclic_sample_buffer *csb     /**< buffer to read */
    ,size_t count                  /**< number sample sets to copy */
    ,dlb_buffer  *sink             /**< destination */
    ,unsigned int sink_samplesize  /**< desired output sample size */
    )
{
    unsigned int delivered = 0;
    unsigned int bytestride;
    ptrdiff_t sink_bytestride;
    uint8_t *outp;
    uint8_t *rp;
    uint8_t *wp;
    uint8_t *end;

    bytestride = (csb->framesize == 1)
        ? csb->samplesize * csb->numchans
        : csb->samplesize;
    sink_bytestride = sink_samplesize * sink->nstride;  /* interleaved only, todo: non-interleaved */
    if (0 != bytestride)
    {
        /* read the write position of the first channel and compute the
         * others: we may be unlucky and have the pointers update while we
         * are mid-copy.
         */

        rp = csb->readpos;
        wp = csb->writepos;
        end = csb->end;

        outp = (uint8_t *) sink->ppdata[0];

        /* The copy-from algorithm follows the first channel and 'does
         * the right thing' for the other channels assuming that they
         * behave synchronously with the first.
         */
        count *= bytestride;
        while (count > 0 && rp != wp)
        {
            ptrdiff_t available; /* available sample sets */
            if (rp > wp)
            {
                /* The buffer's data is split over the end and has
                 * wrapped around. Check to see if the data at the
                 * end is sufficient.
                 */
                available = end - rp;
            }
            else
            {
                /* the buffer's unread data is contiguous from
                 * write position to read position
                 */
                available = wp - rp;
            }

            if (available >= (ptrdiff_t)count)
            {
                available = count;
            }

            csb_read_samples_priv(csb, sink_samplesize, sink, rp, available/bytestride);
            count -= (unsigned int)available;
            rp += available;
            delivered += (unsigned int)available;
            if (rp == end)
            {
                rp = csb->data;
            }
            sink->ppdata[0] = (uint8_t*)sink->ppdata[0] + (available/bytestride) * sink_bytestride;
        }

        csb->readpos = rp;
        sink->ppdata[0] = outp;
        return delivered / bytestride;
    }
    return 0;
}


/**
 * @brief copy a given number of samples from source dlb_buffer to cyclic buffer
 */
static inline
void
csb_write_samples_priv2
    (cyclic_sample_buffer *csb
    ,unsigned int source_samplesize
    ,dlb_buffer *source
    ,uint8_t *wp
    ,unsigned int chanoffset
    ,ptrdiff_t available_samples
    )
{
    const unsigned int numchannels = csb->numchans;
    const unsigned int copychans =
        numchannels > source->nchannel ? source->nchannel : numchannels;
    const unsigned int skipwpchans = numchannels - copychans;
    const unsigned int skipinchans = source->nchannel - copychans;

    ptrdiff_t i;
    unsigned int j;

    uint8_t *inp = (uint8_t *) source->ppdata[0];

    if (csb->framesize == 1) /* interleaved */
    {
        if (source->nstride > 1) /* source interleaved */
        {
            wp += chanoffset * csb->samplesize;

            if (source_samplesize == sizeof(int) && csb->samplesize == 3)
            {
                /* 32-bit audio gets rounded to 24-bit, and 24-bit audio is delivered
                 * unpacked in DPF, (PA uses 24-bit data).  In both cases, PMD sample
                 * size is 4 bytes and PA sample size is 3 bytes.
                 */

                for (i = 0; i < available_samples; ++i)
                {
                    for (j = 0; j < copychans; ++j)
                    {
                        *wp++= inp[1];
                        *wp++= inp[2];
                        *wp++= inp[3];
                        inp += sizeof(int);
                    }
                    wp += skipwpchans * 3;
                    inp += sizeof(int) * skipinchans;
                }
            }
            else if (source->nchannel == csb->numchans)
            {
                memmove(wp, inp, source_samplesize * available_samples * numchannels);
            }
            else
            {
                unsigned int instride = source_samplesize * source->nchannel;
                unsigned int csbstride= source_samplesize * numchannels;
                unsigned int copysize = csbstride >= instride ? instride : csbstride;

                for (i = 0; i < available_samples; ++i)
                {
                    memmove(wp, inp, copysize);
                    wp += csbstride;
                    inp += instride;
                }
            }
        }
        else
        {
            /* buffer interleaved, sink non-interleaved */
            abort();
        }
    }
    else
    {
        /* buffer is non-interleaved */
        /* this case is complicated by the fact that we must chunk
         * up data into units of frame_size
         */

        /* non-interleaved format: device presents input buffer
         * as a set of pointers, one for each slot
         */
        uint8_t *chanptrs[MAX_CHANNELS];

        wp += chanoffset * csb->num_sets * csb->samplesize;

        for (j = 0; j != numchannels; ++j)
        {
            chanptrs[j] = wp;
            wp += csb->num_sets * csb->samplesize;
        }

        if (0 == chanptrs[0])
        {
            return;
        }

        if (source->nstride == (int)source->nchannel)
        {
            /* buffer non-interleaved, source interleaved */
            if (source_samplesize == sizeof(int) && csb->samplesize == 3)
            {
                /* 32-bit audio gets rounded to 24-bit, and 24-bit audio is delivered
                 * unpacked in DPF, (PA uses 24-bit data).  In both cases, DPF sample
                 * size is 4 bytes and PA sample size is 3 bytes.
                 */
                for (j = 0; j < source->nchannel; ++j)
                {
                    uint8_t *srcp = (uint8_t *)source->ppdata[j];
                    uint8_t *sinkp = (uint8_t *)chanptrs[j];
                    for (i = 0; i < available_samples; ++i)
                    {
                        sinkp[0] = srcp[1];
                        sinkp[1] = srcp[2];
                        sinkp[2] = srcp[3];
                        sinkp += 3;
                        srcp += sizeof(int) * source->nstride;
                    }
                }
            }
            else
            {
                ptrdiff_t copysize = source_samplesize * available_samples;

                assert(source_samplesize == csb->samplesize);

                for (j = 0; j < numchannels; ++j)
                {
                    uint8_t *srcp = (uint8_t *)source->ppdata[j];
                    uint8_t *sinkp = (uint8_t *)chanptrs[j];
                    memmove(sinkp, srcp, copysize);
                }
            }
        }
        else
        {
            /*buffer non-interleaved, source non-interleaved */
        }
    }
}


/**
 * @brief copy a given number of sample sets from input to buffer
 *
 * This method allows an external write position to be maintained,
 * such as might be used by a multiplexing input, where we copy
 * different inputs separately, and only want to update the buffer's
 * own write position once we have copied from all inputs.
 */
static inline
unsigned int                       /** @return number samples written */
csb_write_samples_priv
    (cyclic_sample_buffer *csb     /**< buffer to received data */
    ,size_t count                  /**< number sample sets to copy */
    ,uint8_t **writepos            /**< write pointer */
    ,unsigned int chanoffset       /**< channel offset */
    ,dlb_buffer *source            /**< source data to write */
    ,unsigned int source_samplesize /**< size of source samples */
    )
{
    ptrdiff_t sample_set_size;
    unsigned int source_sample_set_size;
    ptrdiff_t delivered = 0;
    ptrdiff_t unused_space = 0;
    uint8_t *wp = NULL;
    uint8_t *rp = NULL;
    uint8_t *end = NULL;

    assert(writepos != NULL);

    sample_set_size = (ptrdiff_t)csb->samplesize * csb->numchans;
    /* interleaved only: todo non-interleaved */
    source_sample_set_size = source_samplesize * source->nchannel; 

    wp = *writepos;
    rp = csb->readpos;
    end = csb->end;
    count *= sample_set_size;

    if (rp > wp)
    {
        unused_space = rp - wp;
    }
    else
    {
        unused_space = (end - wp) + (rp - csb->data);
    }

    unused_space = (unused_space / sample_set_size) * sample_set_size;

    /* Prevent writing all the way to read pointer */
    unused_space = unused_space > sample_set_size
        ? unused_space - sample_set_size
        : 0;

    if ((ptrdiff_t)count > unused_space)
    {
        count = (size_t)unused_space;
    }

    while (count > 0)
    {
        ptrdiff_t space;
        if (rp > wp)
        {
            /* The buffer's data is split over the end and has
             * wrapped around. Check to see if the data at the
             * end is sufficient.
             */
            space = rp - wp;
        }
        else
        {
            /* the buffer's unread data is contiguous from
             * write position to read position
             */
            space = end - wp;
        }

        if (space >= (ptrdiff_t)count)
        {
            space = count;
        }

        csb_write_samples_priv2(csb, source_samplesize,
                                source, wp, chanoffset,
                                space/sample_set_size);
        count -= (unsigned int)space;
        wp += space;   /* @todo: this is incorrect for non-interleaved,
                        * we want to increase wp by sample_set_size * num_samples
                        */
        delivered += space;
        if (wp == end)
        {
            wp = csb->data;
        }
        source->ppdata[0] = (uint8_t *)source->ppdata[0] + (space/sample_set_size) * source_sample_set_size;
    }

    *writepos = wp;

    assert(delivered == ((delivered/sample_set_size)*sample_set_size));

    return  (unsigned int)delivered / sample_set_size;
}


/**
 * @brief copy a given number of sample sets from input to buffer
 */
static inline
unsigned int                       /** @return number samples written */
csb_write_samples
    (cyclic_sample_buffer *csb     /**< buffer to received data */
    ,size_t count                  /**< number sample sets to copy */
    ,dlb_buffer *source            /**< source data to write */
    ,unsigned int source_samplesize /**< size of source samples */
    )
{
    return csb_write_samples_priv(csb, count, &csb->writepos, 0,
                                  source, source_samplesize);
}


static inline
int
csb_is_empty
    (cyclic_sample_buffer *csb
    )
{
    return csb->readpos == csb->writepos;
}


static inline
void
csb_purge
    (cyclic_sample_buffer *csb
    )
{
    if (csb->data != NULL)
    {
        csb->readpos = csb->data;
        csb->writepos = csb->data;
    }
}


#endif /* __CYCLIC_SAMPLE_BUFFER_H_ */
