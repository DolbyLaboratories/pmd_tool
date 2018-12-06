/************************************************************************
 * dlb_wave
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

#include "dlb_wave/include/dlb_wave.h"
#include "dlb_wave/include/dlb_riff.h"
#include <string.h>
#include <assert.h>

/* private flags */
#define DLB_WAVEP_WRITING   1   /**< file is open for writing */

/* Helper for extracting a little-endian 64 bit size from an array of octets */
#if DLB_RIFF_64
/* We actually have a 64 bit data type */
#define DLB_WAVE_RF64_SIZE(a)\
    ( (((dlb_riff_size) ((unsigned char)(a)[0])) <<  0)\
    | (((dlb_riff_size) ((unsigned char)(a)[1])) <<  8)\
    | (((dlb_riff_size) ((unsigned char)(a)[2])) << 16)\
    | (((dlb_riff_size) ((unsigned char)(a)[3])) << 24)\
    | (((dlb_riff_size) ((unsigned char)(a)[4])) << 32)\
    | (((dlb_riff_size) ((unsigned char)(a)[5])) << 40)\
    | (((dlb_riff_size) ((unsigned char)(a)[6])) << 48)\
    | (((dlb_riff_size) ((unsigned char)(a)[7])) << 56)\
    )
#else
/* We don't have a 64 bit data type, so saturate to ULONG_MAX */
#define DLB_WAVE_RF64_SIZE(a)\
    ((a)[4] || (a)[5] || (a)[6] || (a)[7])\
    ?   DLB_RIFF_SIZE_MAX\
    :   ( (((dlb_riff_size) ((unsigned char)(a)[0])) <<  0)\
        | (((dlb_riff_size) ((unsigned char)(a)[1])) <<  8)\
        | (((dlb_riff_size) ((unsigned char)(a)[2])) << 16)\
        | (((dlb_riff_size) ((unsigned char)(a)[3])) << 24)\
        )
#endif

/* Extract a four byte value from a little-endian byte array into unsigned long. */
#define DLB_WAVE_E4(a)\
    ( (((unsigned long) ((unsigned char)(a)[0])) << 0)\
    | (((unsigned long) ((unsigned char)(a)[1])) << 8)\
    | (((unsigned long) ((unsigned char)(a)[2])) << 16)\
    | (((unsigned long) ((unsigned char)(a)[3])) << 24)\
    )

/* Extract a two byte value from a little-endian byte array into unsigned short. */
#define DLB_WAVE_E2(a)\
    ( (((unsigned short) ((unsigned char)(a)[0])) << 0)\
    | (((unsigned short) ((unsigned char)(a)[1])) << 8)\
    )

/* Make a little-endian eight byte array from a dlb_riff_size. */
#define DLB_WAVE_M8(a,i)\
    (a)[0] = (unsigned char) (((dlb_riff_size)(i) >> 0) & 0xff);\
    (a)[1] = (unsigned char) (((dlb_riff_size)(i) >> 8) & 0xff);\
    (a)[2] = (unsigned char) (((dlb_riff_size)(i) >> 16) & 0xff);\
    (a)[3] = (unsigned char) (((dlb_riff_size)(i) >> 24) & 0xff);\
    (a)[4] = (unsigned char) (((dlb_riff_size)(i) >> 32) & 0xff);\
    (a)[5] = (unsigned char) (((dlb_riff_size)(i) >> 40) & 0xff);\
    (a)[6] = (unsigned char) (((dlb_riff_size)(i) >> 48) & 0xff);\
    (a)[7] = (unsigned char) (((dlb_riff_size)(i) >> 56) & 0xff);

/* Make a little-endian four byte array from a long. */
#define DLB_WAVE_M4(a,i)\
    (a)[0] = (unsigned char) (((unsigned long)(i) >> 0) & 0xff);\
    (a)[1] = (unsigned char) (((unsigned long)(i) >> 8) & 0xff);\
    (a)[2] = (unsigned char) (((unsigned long)(i) >> 16) & 0xff);\
    (a)[3] = (unsigned char) (((unsigned long)(i) >> 24) & 0xff);

/* Make a little-endian two byte array from an int. */
#define DLB_WAVE_M2(a,i)\
    (a)[0] = (unsigned char) (((unsigned short)(i) >> 0) & 0xff);\
    (a)[1] = (unsigned char) (((unsigned short)(i) >> 8) & 0xff);

/* Wave format offsets */
#define DLB_FMT_TYPE                0   /**< PCM, IEEE_FLOAT, EXTENSIBLE */
#define DLB_FMT_CHANNEL_COUNT       2   /**< Number of channels */
#define DLB_FMT_SAMPLE_RATE         4   /**< Samples per second */
#define DLB_FMT_BYTES_PER_SECOND    8   /**< Byterate of compressed formats */
#define DLB_FMT_BLOCK_ALIGN         12  /**< Size of sample frame */
#define DLB_FMT_BIT_DEPTH           14  /**< How many bits are there? */
#define DLB_FMT_MIN_SIZE            16  /**< sizeof(WAVEFORMAT) */
#define DLB_FMT_EXTRA_SIZE          16  /**< Size of extra format info */
#define DLB_FMT_SIZE                18  /**< sizeof(WAVEFORMATEX) */
#define DLB_FMT_VALID_BIT_DEPTH     18  /**< How many bits actually contain audio? */
#define DLB_FMT_CHANNEL_MASK        20  /**< Bitfield of channels present */
#define DLB_FMT_SUBFORMAT           24  /**< Subformat GUID */
#define DLB_FMT_EX_SIZE             40  /**< sizeof(WAVEFORMAT_EXTENSIBLE) */

/* Here are some of the common format tags we might want to support. */
#define DLB_FMT_TAG_PCM         1       /* little-endian fixed point PCM */
#define DLB_FMT_TAG_IEEE_FLOAT  3       /* IEEE floating point PCM */
#define DLB_FMT_TAG_EXTENSIBLE  0xFFFE  /* Microsoft/EBU extensible format */

/* The raw bytes in a KSDATAFORMAT_SUBTYPE_PCM GUID
 * 00000001-0000-0010-8000-00aa00389b71
 */
#define DLB_FMT_GUID_SIZE 16
const unsigned char dlb_wave_ksdataformat_subtype_pcm[DLB_FMT_GUID_SIZE] =
{0x01, 0x00, 0x00, 0x00
,0x00, 0x00
,0x10, 0x00
,0x80, 0x00, 0x00, 0xaa
,0x00, 0x38, 0x9b, 0x71
};

/* The raw bytes in a KSDATAFORMAT_SUBTYPE_IEEE_FLOAT GUID
 * 00000003-0000-0010-8000-00aa00389b71
 */
#define DLB_FMT_GUID_SIZE 16
const unsigned char dlb_wave_ksdataformat_subtype_ieee_float[DLB_FMT_GUID_SIZE] =
{0x03, 0x00, 0x00, 0x00
,0x00, 0x00
,0x10, 0x00
,0x80, 0x00, 0x00, 0xaa
,0x00, 0x38, 0x9b, 0x71
};

/* Size of ds64 chunk to reserve */
#define DLB_WAVE_DS64_SIZE 28

/******************************************************************************
version functions
******************************************************************************/
static const DLB_WAVE_VERSION_INFO v =
{
  DLB_WAVE_V_API,     /**< assigning the defined macro values */
  DLB_WAVE_V_FCT,
  DLB_WAVE_V_MTNC,
  NULL
};

const DLB_WAVE_VERSION_INFO *
dlb_wave_get_version(void)
{
  return &v;
}

/******************************************************************************
general functions
******************************************************************************/
unsigned long
dlb_wave_get_sample_rate
    (const dlb_wave_file *pwf)
{
    return pwf->format.sample_rate;
}

unsigned
dlb_wave_get_format_flags
    (const dlb_wave_file *pwf)
{
    return pwf->format_flags;
}

int
dlb_wave_seek_to_frame
    (dlb_wave_file      *pwf
    ,dlb_wave_location  target_frame)
{
    /* Seek the desired number of frames from the start of the data chunk.
     * Care needs to be taken to handle WAVE files >= 4GB and seeks > LONG_MAX (2GB files).
     */
    int                 status;
    dlb_riff_location   offset_octets;
    dlb_riff_location   frame_bits;     /* A frame is a sample for each channel */

    if ((NULL == pwf) || NULL == (pwf->pdata))
    {
        return DLB_WAVE_E_NODATA;
    }
    if (dlb_wave_get_num_frames(pwf) <= target_frame)
    {
        return DLB_WAVE_E_NODATA;
    }
    /* Calculate the byte offset from the start of the data section
     * Note: We avoid seeking to any unusual locations.
     */
    frame_bits = pwf->format.bits_per_sample * pwf->format.channel_count;
    /* Avoid attempting to seek to unusual locations */
    if (frame_bits % 8)
    {
        return DLB_WAVE_E_NSEEK;
    }
    offset_octets = (frame_bits / 8) * target_frame;

    /* Now perform the seek */
    status = dlb_riff_seek_chunk(pwf->pdata, offset_octets);
    if (status)
    {
        return DLB_WAVE_E_NSEEK;
    }

    return status;
}

unsigned
dlb_wave_get_channel_count
    (const dlb_wave_file *pwf)
{
    return pwf->format.channel_count;
}

unsigned
dlb_wave_get_bit_depth
    (const dlb_wave_file *pwf)
{
    return pwf->format.bits_per_sample;
}

unsigned long
dlb_wave_get_channel_mask
    (const dlb_wave_file *pwf)
{
    return pwf->format.channel_mask;
}

dlb_wave_location
dlb_wave_get_num_frames
    (const dlb_wave_file *pwf)
{
    dlb_riff_size   num_frames;

    num_frames = (pwf->data_size * 8) 
               / (dlb_wave_get_bit_depth(pwf) * dlb_wave_get_channel_count(pwf));

    /* Return the number of frames */
    return num_frames;
}

dlb_wave_location
dlb_wave_get_current_frame
    (const dlb_wave_file *pwf)
{
    return
           (dlb_riff_chunk_location(pwf->pdata) * 8)
         / (dlb_wave_get_bit_depth(pwf) * dlb_wave_get_channel_count(pwf));

}

const dlb_wave_format*
dlb_wave_get_format
    (const dlb_wave_file *pwf)
{
    return &pwf->format;
}

void
dlb_wave_close
    (dlb_wave_file *pwf)
{
    if (pwf->priff)
        dlb_riff_close_chunk(pwf->priff);
    dlb_riff_close(&pwf->riff);
}

/******************************************************************************
functions for reading wave files
******************************************************************************/

static
int
dlb_wave_handle_ds64
    (dlb_wave_file  *pwf
    ,dlb_riff_chunk *pck
    )
{
    unsigned char data[16];

    /* Only bother reading the extended riff and data sizes for the moment.
     * The sample count doesn't seem particularly useful.
     * The table seems to be for future extension and it's not
     * clear what you'd use it for right now.
     */
    if (16 != dlb_riff_read_chunk_data(pck, data, 16))
        return DLB_WAVE_E_NODS64;

    pwf->riff_size = DLB_WAVE_RF64_SIZE(data+0);
    pwf->data_size = DLB_WAVE_RF64_SIZE(data+8);

    return DLB_RIFF_OK;
}

static
int
dlb_wave_handle_fmt
    (dlb_wave_file  *pwf
    ,dlb_riff_chunk *pck
    )
{
    unsigned char fmt[DLB_FMT_EX_SIZE];
    size_t size;
    unsigned short extra_size = 0;

    /* read format data */
    size = dlb_riff_read_chunk_data(pck, fmt, DLB_FMT_EX_SIZE);
    if (size < DLB_FMT_MIN_SIZE)
        /* Check that the fmt chunk is at least the minimum size */
        return DLB_WAVE_E_NOFMT;

    /* extract basic fields */
    pwf->format.format_type         = DLB_WAVE_E2(fmt+DLB_FMT_TYPE);
    pwf->format.channel_count       = DLB_WAVE_E2(fmt+DLB_FMT_CHANNEL_COUNT);
    pwf->format.sample_rate         = DLB_WAVE_E4(fmt+DLB_FMT_SAMPLE_RATE);
    pwf->format.bytes_per_second    = DLB_WAVE_E4(fmt+DLB_FMT_BYTES_PER_SECOND);
    pwf->format.block_alignment     = DLB_WAVE_E2(fmt+DLB_FMT_BLOCK_ALIGN);
    pwf->format.bits_per_sample     = DLB_WAVE_E2(fmt+DLB_FMT_BIT_DEPTH);
    pwf->format.octets_per_sample   = (pwf->format.bits_per_sample + 7)/8;

    /* read extra size count if it exists */
    if (size >= DLB_FMT_SIZE)
        extra_size = DLB_WAVE_E2(fmt+DLB_FMT_EXTRA_SIZE);

    pwf->format.valid_bits_per_sample = pwf->format.bits_per_sample;
    pwf->format.channel_mask = 0;

    switch (pwf->format.format_type)
    {
    case DLB_FMT_TAG_EXTENSIBLE:
        {
            unsigned i, ieee_float = 1;

            pwf->format_flags |= DLB_WAVE_EXTENSIBLE;

            /* Check for invalid size conditions */
            if  (   (size < DLB_FMT_EX_SIZE)
                ||  (extra_size < (DLB_FMT_EX_SIZE-DLB_FMT_SIZE))
                )
                return DLB_WAVE_E_NOFMT;

            /* Extract extra extensible information */
            pwf->format.valid_bits_per_sample = DLB_WAVE_E2(fmt+DLB_FMT_VALID_BIT_DEPTH);
            pwf->format.channel_mask          = DLB_WAVE_E4(fmt+DLB_FMT_CHANNEL_MASK);

            /* See if the extensible format is KSDATAFORMAT_SUBTYPE_IEEE_FLOAT */
            for (i = 0; i < DLB_FMT_GUID_SIZE; i++)
            {
                if (fmt[DLB_FMT_SUBFORMAT+i] != dlb_wave_ksdataformat_subtype_ieee_float[i])
                {
                    ieee_float = 0;
                    break;
                }
            }

            if (ieee_float)
                pwf->format_flags |= DLB_WAVE_FLOAT;
            else
            {
                /* Check that the extensible format is KSDATAFORMAT_SUBTYPE_PCM */
                for (i = 0; i < DLB_FMT_GUID_SIZE; i++)
                {
                    if (fmt[DLB_FMT_SUBFORMAT+i] != dlb_wave_ksdataformat_subtype_pcm[i])
                        return DLB_WAVE_W_NOPCM;
                }
            }
        }
        break;

    case DLB_FMT_TAG_PCM:
        break;

    case DLB_FMT_TAG_IEEE_FLOAT:
        pwf->format_flags |= DLB_WAVE_FLOAT;
        break;
    }

    return DLB_RIFF_OK;
}

static
int
wave_open_read
    (dlb_wave_file                *pwf
    ,const char                   *filename
    ,dlb_octfile                  *pfile
    ,const dlb_riff_chunk_handler *phandler
    )
{
    int status, success = DLB_RIFF_OK;
    unsigned char form[4];
    unsigned nfmt = 0;  /* number of fmt chunks seen */
    unsigned nds64 = 0; /* number of ds64 chunks seen */

    /* initialise stuff to known values */
    memset(pwf, 0, sizeof(*pwf));
    pwf->phandler = phandler;

    /* We use our pwf->chunks array to track active chunks.
     * We probably only need one chunk active at a time here, but
     * in case we encounter any chunks in here that we want to remember
     * the locations of, we can just increment pwf->nchunk and keep the
     * chunk info in out pwf->chunks array.
     */

    if (NULL != filename)
        status = dlb_riff_open_read(&pwf->riff, filename);
    else
        status = dlb_riff_octfile_read(&pwf->riff, pfile);
    if (status)
        return status;

    /* Read chunks until we encounter a RIFF or RF64 chunk. */
    do
    {
        /* Read next chunk from the file */
        status = dlb_riff_read_next_chunk(&pwf->riff, pwf->chunks+pwf->nchunk);
        if (status)
        {
            if (DLB_RIFF_W_EOF == status)
                status = DLB_WAVE_E_NORIFF;
            dlb_riff_close(&pwf->riff);
            return status;
        }
    } while (!dlb_riff_chunk_id_matches(pwf->chunks+pwf->nchunk, "RIFF") && !dlb_riff_chunk_id_matches(pwf->chunks+pwf->nchunk, "RF64"));

    if (dlb_riff_chunk_id_matches(pwf->chunks+pwf->nchunk, "RF64"))
    {
        pwf->format_flags |= DLB_WAVE_RF64;
    }

    /* Record RIFF size */
    pwf->riff_size = dlb_riff_chunk_size(pwf->chunks+pwf->nchunk);

    /* We've found our RIFF chunk. We now read the four character RIFF form
     * code.
     */
    if (4 != dlb_riff_read_chunk_data(pwf->chunks+pwf->nchunk, form, 4))
    {
        dlb_riff_close(&pwf->riff);
        return DLB_WAVE_E_NORIFF;
    }

    /* Check it's a RIFF file with WAVE form. AVI files and other forms are
     * not supported.
     */
    if  (   (form[0] != 'W')
        ||  (form[1] != 'A')
        ||  (form[2] != 'V')
        ||  (form[3] != 'E')
        )
    {
        dlb_riff_close(&pwf->riff);
        return DLB_WAVE_E_NOWAVE;
    }

    /* Look through all the chunks at the beginning of the file until we find
     * the data chunk. We support chunks at the beginning in any order.
     */
    do
    {
        /* Read next chunk */
        status = dlb_riff_read_next_chunk(&pwf->riff, pwf->chunks+pwf->nchunk);
        if (status)
        {
            if (DLB_RIFF_W_EOF == status)
                status = DLB_WAVE_E_NODATA;
            dlb_riff_close(&pwf->riff);
            return status;
        }

        if (dlb_riff_chunk_id_matches(pwf->chunks+pwf->nchunk, "data"))
        {
            /* We've found the (first) data chunk.
             * If this is an RF64 file we need to override the size in the data
             * chunk header with the size we read from the ds64 chunk.
             */
            if (pwf->format_flags & DLB_WAVE_RF64)
            {
                dlb_riff_override_chunk_size(pwf->chunks+pwf->nchunk, pwf->data_size);
            }
            else
            {
                pwf->data_size = pwf->chunks[pwf->nchunk].size;
            }
            break;
        }

        if (   (pwf->format_flags & DLB_WAVE_RF64)
                &&  (dlb_riff_chunk_id_matches(pwf->chunks+pwf->nchunk, "ds64"))
                )
        {
            /* handle ds64 chunk */
            status = dlb_wave_handle_ds64(pwf, pwf->chunks+pwf->nchunk);
            ++nds64;
        }
        else if (dlb_riff_chunk_id_matches(pwf->chunks+pwf->nchunk, "fmt "))
        {
            /* handle fmt chunk */
            status = dlb_wave_handle_fmt(pwf, pwf->chunks+pwf->nchunk);
            ++nfmt;
        }
        else
        {
            /* pass other chunks to custom chunk handler chain */
            pwf->format_flags |= DLB_WAVE_JUNK;
            status = dlb_riff_chunk_handler_call_chain
                (pwf->phandler
                ,pwf->chunks+pwf->nchunk
                );
        }

        if (status >= 0)
        {
            /* skip any unread data at end of chunk */
            success = status;
            status = dlb_riff_skip_chunk_data(pwf->chunks+pwf->nchunk);
        }
    } while (status == DLB_RIFF_OK);

    if (status)
    {
        dlb_riff_close(&pwf->riff);
        return status;
    }

    /* Check we've seen other chunks that we need */
    if (nfmt != 1)
    {
        dlb_riff_close(&pwf->riff);
        return DLB_WAVE_E_NOFMT;
    }
    if (pwf->format_flags & DLB_WAVE_RF64)
    {
        if (nds64 != 1)
        {
            /* This is an RF64 but we haven't seen a ds64 chunk */
            dlb_riff_close(&pwf->riff);
            return DLB_WAVE_E_NODS64;
        }
#if !DLB_RIFF_64
        else if (dlb_riff_chunk_size(pwf->chunks+pwf->nchunk) >= DLB_RIFF_SIZE_MAX)
        {
            /* This is an RF64 file, but we don't have a 64 bit size type.
             * In this case we'll only be able to access the first 4GB of the
             * file.
             */
            pwf->format_flags |= DLB_WAVE_RF64_4GB;
        }
#endif
    }

    /* When we get to here, we've seen the chunks we need, and we've found
     * a data chunk. Keep the data chunk in our list and start reading data.
     */
    pwf->pdata = pwf->chunks+pwf->nchunk++;
    return success;
}

int
dlb_wave_open_read
    (dlb_wave_file                *pwf
    ,const char                   *filename
    ,const dlb_riff_chunk_handler *phandler
    )
{
    return wave_open_read(pwf, filename, NULL, phandler);
}

int
dlb_wave_octfile_read
    (dlb_wave_file                *pwf
    ,dlb_octfile                  *pfile
    ,const dlb_riff_chunk_handler *phandler
    )
{
    return wave_open_read(pwf, NULL, pfile, phandler);
}

int
dlb_wave_read_data
    (dlb_wave_file *pwf
    ,void          *pv
    ,size_t         ndata
    ,size_t        *pnread
    )
{
    unsigned char *pdata = pv;
    int status = (pwf->pdata) ? DLB_RIFF_OK : DLB_WAVE_E_NODATA;
    size_t totalread = 0;
    while (ndata && !status)
    {
        size_t nread;

        nread = dlb_riff_read_chunk_data(pwf->pdata, pdata, ndata);
        pdata += nread;
        ndata -= nread;
        totalread += nread;

        /* Exit if we read all the data that was requested */
        if (!ndata)
        {
            break;
        }

        status = dlb_riff_skip_chunk_data(pwf->pdata);
        if (status)
        {
            break;
        }

        /* Look for another data chunk.
         * WAVE files are only supposed to have one, but some software
         * creates multiple data chunks to achieve files >4 GB.
         */
        do
        {
            dlb_riff_chunk next_chunk;
            status = dlb_riff_read_next_chunk(&pwf->riff, &next_chunk);
            if (status)
            {
                break;
            }

            if (dlb_riff_chunk_id_matches(&next_chunk, "data"))
            {
                /* We've found the next data chunk */
                *(pwf->pdata) = next_chunk;
                break;
            }

            /* pass other chunks to custom chunk handler */
            pwf->format_flags |= DLB_WAVE_JUNK;
            status = dlb_riff_chunk_handler_call_chain(pwf->phandler, &next_chunk);
            if (status)
            {
                break;
            }

            /* skip remainder of chunk */
            status = dlb_riff_skip_chunk_data(&next_chunk);
        } while (!status);
    }

    if (pnread)
    {
        *pnread = totalread;
    }

    return status;
}

/******************************************************************************
functions for writing wave files
******************************************************************************/
static
int
dlb_wave_create_fmt
    (dlb_wave_file  *pwf
    ,unsigned long   sample_rate
    ,unsigned        channel_count
    ,unsigned long   channel_mask
    ,unsigned        bit_depth
    )
{
    dlb_riff_chunk ck;
    int status;
    char fmt[DLB_FMT_EX_SIZE];
    size_t nfmt = 0;
    unsigned octet_depth;

    /* Assemble the format data */
    if  (   (pwf->format_flags & DLB_WAVE_EXTENSIBLE)
        ||  (channel_mask)
        )
    {
        /* Make it a WAVE_FORMAT_EXTENSIBLE */
        pwf->format_flags |= DLB_WAVE_EXTENSIBLE;
        pwf->format.format_type = DLB_FMT_TAG_EXTENSIBLE;
    }
    else if (pwf->format_flags & DLB_WAVE_FLOAT)
    {
        pwf->format.format_type = DLB_FMT_TAG_IEEE_FLOAT;
    }
    else
    {
        pwf->format.format_type = DLB_FMT_TAG_PCM;
    }

    if (pwf->format_flags & DLB_WAVE_FLOAT)
    {
        assert(bit_depth == 32);
    }

    if (bit_depth > 32)
        bit_depth = 32;

    octet_depth = (bit_depth + 7)/8;

    pwf->format.channel_count           = (unsigned short)channel_count;
    pwf->format.sample_rate             = sample_rate;
    pwf->format.bits_per_sample         = (unsigned short)octet_depth*8;
    pwf->format.octets_per_sample       = (unsigned short)octet_depth;
    pwf->format.valid_bits_per_sample   = (unsigned short)bit_depth;
    pwf->format.channel_mask            = channel_mask;
    pwf->format.block_alignment         = (unsigned short)(octet_depth*channel_count);
    pwf->format.bytes_per_second        = (unsigned long) pwf->format.block_alignment*sample_rate;

    /* Encode the format into it's on-disk image */
    DLB_WAVE_M2(fmt+DLB_FMT_TYPE, pwf->format.format_type)
    DLB_WAVE_M2(fmt+DLB_FMT_CHANNEL_COUNT, pwf->format.channel_count)
    DLB_WAVE_M4(fmt+DLB_FMT_SAMPLE_RATE, pwf->format.sample_rate)
    DLB_WAVE_M4(fmt+DLB_FMT_BYTES_PER_SECOND, pwf->format.bytes_per_second)
    DLB_WAVE_M2(fmt+DLB_FMT_BLOCK_ALIGN, pwf->format.block_alignment)
    DLB_WAVE_M2(fmt+DLB_FMT_BIT_DEPTH, pwf->format.bits_per_sample)

    if (pwf->format.format_type == DLB_FMT_TAG_EXTENSIBLE)
    {
        /* Set up extra byte count */
        DLB_WAVE_M2(fmt+DLB_FMT_EXTRA_SIZE, DLB_FMT_EX_SIZE - DLB_FMT_SIZE)
        nfmt = DLB_FMT_EX_SIZE;

        /* Pack extra EXTENSIBLE stuff */
        DLB_WAVE_M2(fmt+DLB_FMT_VALID_BIT_DEPTH, pwf->format.valid_bits_per_sample)
        DLB_WAVE_M4(fmt+DLB_FMT_CHANNEL_MASK, pwf->format.channel_mask)
        if (pwf->format_flags & DLB_WAVE_FLOAT)
            memcpy(fmt+DLB_FMT_SUBFORMAT, dlb_wave_ksdataformat_subtype_ieee_float, DLB_FMT_GUID_SIZE);
        else
            memcpy(fmt+DLB_FMT_SUBFORMAT, dlb_wave_ksdataformat_subtype_pcm, DLB_FMT_GUID_SIZE);
    }
    else
    {
        /* No extra bytes, just WAVEFORMATEX */
        DLB_WAVE_M2(fmt+DLB_FMT_EXTRA_SIZE, 0)
        nfmt = DLB_FMT_SIZE;
    }

    /* Add the chunk to the file */
    status = dlb_riff_open_chunk(&pwf->riff, &ck, pwf->priff, "fmt ");
    if (!status)
    {
        int close_status;
        status = dlb_riff_write_chunk_data(&ck, fmt, nfmt);
        close_status = dlb_riff_close_chunk(&ck);
        if (!status)
            status = close_status;
    }
    return status;
}

static
int
wave_open_write
    (dlb_wave_file  *pwf
    ,const char     *filename
    ,dlb_octfile    *pfile
    ,unsigned        format_flags
    ,unsigned long   sample_rate
    ,unsigned        channel_count
    ,unsigned long   channel_mask
    ,unsigned        bit_depth
    )
{
    int status;

    /* initialise stuff to known values */
    memset(pwf, 0, sizeof(*pwf));
    pwf->format_flags = format_flags;

    /* open the RIFF file */
    if (NULL != filename)
        status = dlb_riff_open_write(&pwf->riff, filename);
    else
        status = dlb_riff_octfile_write(&pwf->riff, pfile);
    if (status)
        return status;

    /* write the RIFF chunk and keep it open */
    pwf->priff = pwf->chunks+pwf->nchunk++;
    status = dlb_riff_open_chunk(&pwf->riff, pwf->priff, 0, "RIFF");
    if (!status)
        status = dlb_riff_write_chunk_data(pwf->priff, "WAVE", 4);
    if (status)
    {
        dlb_riff_close(&pwf->riff);
        return status;
    }

    if (format_flags & DLB_WAVE_RF64)
    {
        /* Reserve the ds64 chunk. Label it 'JUNK' per EBU TECH 3306-2009. */
        pwf->pds64 = pwf->chunks+pwf->nchunk++;
        status = dlb_riff_open_chunk(&pwf->riff, pwf->pds64, pwf->priff, "JUNK");
        if (!status)
        {
            int close_status;
            char junk[DLB_WAVE_DS64_SIZE];
            memset(junk, 0, DLB_WAVE_DS64_SIZE);
            status = dlb_riff_write_chunk_data(pwf->pds64, junk, DLB_WAVE_DS64_SIZE);
            close_status = dlb_riff_close_chunk(pwf->pds64);
            if (!status)
                status = close_status;
        }
        if (status)
        {
            dlb_riff_close(&pwf->riff);
            return status;
        }
    }

    /* Create the fmt chunk */
    return dlb_wave_create_fmt
                (pwf
                ,sample_rate
                ,channel_count
                ,channel_mask
                ,bit_depth
                );
}

int
dlb_wave_open_write
    (dlb_wave_file  *pwf
    ,const char     *filename
    ,unsigned        format_flags
    ,unsigned long   sample_rate
    ,unsigned        channel_count
    ,unsigned long   channel_mask
    ,unsigned        bit_depth
    )
{
    return wave_open_write
        (pwf
        ,filename
        ,NULL
        ,format_flags
        ,sample_rate
        ,channel_count
        ,channel_mask
        ,bit_depth
        );
}

int
dlb_wave_octfile_write
    (dlb_wave_file  *pwf
    ,dlb_octfile    *pfile
    ,unsigned        format_flags 
    ,unsigned long   sample_rate
    ,unsigned        channel_count
    ,unsigned long   channel_mask
    ,unsigned        bit_depth
    )
{
    return wave_open_write
        (pwf
        ,NULL
        ,pfile
        ,format_flags
        ,sample_rate
        ,channel_count
        ,channel_mask
        ,bit_depth
        );
}

int
dlb_wave_insert_chunk
    (dlb_wave_file  *pwf
    ,dlb_riff_chunk *priff
    ,const char      id[DLB_RIFF_NID]
    )
{
    return dlb_riff_open_chunk(&pwf->riff, priff, pwf->priff, id);
}

int
dlb_wave_begin_data
    (dlb_wave_file  *pwf)
{
    /* Create the data chunk */
    pwf->pdata = pwf->chunks+pwf->nchunk++;
    return dlb_riff_open_chunk(&pwf->riff, pwf->pdata, pwf->priff, "data");
}

int
dlb_wave_write_data
    (dlb_wave_file  *pwf
    ,const void     *pv
    ,size_t          ndata
    )
{
    const char     *pdata = pv;
    int status = DLB_RIFF_OK, sstatus;
    dlb_riff_location location;
    location = dlb_riff_chunk_location(pwf->pdata) + dlb_riff_chunk_location(pwf->priff);
    if  (   ((location + ndata) > DLB_RIFF_MAX_SIZE)
        &&  (!(pwf->format_flags & DLB_WAVE_RF64))
        )
    {
        /* We're going to blow our maximum RIFF chunk size.
         * We need to either make an RF64 file or only write some
         * of the data.
         */
        status = DLB_WAVE_E_TOOBIG;
        ndata = (size_t) (DLB_RIFF_MAX_SIZE - location);
    }
    sstatus = dlb_riff_write_chunk_data(pwf->pdata, pdata, ndata);
    if (sstatus)
        status = sstatus;
    return status;
}

int
dlb_wave_end_data
    (dlb_wave_file  *pwf)
{
    int status;

    /* close data chunk */
    status = dlb_riff_close_chunk(pwf->pdata);
    if (status < 0)
        return status;

    /* close RIFF chunk */
    status = dlb_riff_close_chunk(pwf->priff);
    if  (   (pwf->format_flags & DLB_WAVE_RF64)
        &&  (   (status == DLB_RIFF_W_TOOBIG)
            ||  (pwf->format_flags & DLB_WAVE_FORCE_RF64)
            )
        )
    {
        /* rename 'JUNK' chunk to 'ds64' */
        assert(pwf->pds64);
        status = dlb_riff_reopen_chunk(pwf->pds64);
        if (!status)
        {
            status = dlb_riff_rename_chunk(pwf->pds64, "ds64");
            if (!status)
            {
                char ds64[DLB_WAVE_DS64_SIZE];
                int close_status;
                dlb_riff_size riff_size, data_size, sample_count;

                riff_size = dlb_riff_chunk_size(pwf->priff);
                data_size = dlb_riff_chunk_size(pwf->pdata);

                /* We need something more sophisticated here if we want to
                 * support compressed data formats.
                 */
                sample_count = data_size / pwf->format.block_alignment;

                /* Populate ds64 data per EBU 3306 */
                DLB_WAVE_M8(ds64+0, riff_size);
                DLB_WAVE_M8(ds64+8, data_size);
                DLB_WAVE_M8(ds64+16, sample_count);

                /* Write a 0 in the table length field.
                 * This seems to be reserved for future expansion.
                 */
                DLB_WAVE_M4(ds64+24, 0);

                /* Write back ds64 data to file */
                status = dlb_riff_write_chunk_data(pwf->pds64, ds64, DLB_WAVE_DS64_SIZE);
                close_status = dlb_riff_close_chunk(pwf->pds64);
                if (close_status)
                    status = close_status;

                /* Rename the RIFF chunk to identify an RF64 file */
                close_status = dlb_riff_rename_chunk(pwf->priff, "RF64");
                if (close_status)
                    status = close_status;
            }
        }
    }
    return status;
}
