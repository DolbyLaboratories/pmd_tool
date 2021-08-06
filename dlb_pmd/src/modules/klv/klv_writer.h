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
 * @file klv_writer.h
 * @brief KLV writer abstraction
 */


#ifndef KLV_WRITER_H_
#define KLV_WRITER_H_

#include "pmd_crc32.h"
#include "klv.h"


/**
 * @def KLV_BYTES_FOR_CRC_PAYLOAD
 * @brief symbolic constant listing number of bytes that must be reserved
 * for the CRC
 */
#define KLV_BYTES_FOR_CRC_PAYLOAD (6)


/**
 * @brief state of writer
 */
typedef struct
{
    dlb_pmd_model *model;          /**< model being written */
    uint8_t sindex;                /**< ED2 stream index, or 0xff */
    uint8_t *buffer;               /**< raw data buffer */
    uint8_t *end;                  /**< 1st byte after end of buffer */
    uint8_t *wp;                   /**< current write position */
    uint8_t *localkey;             /**< latest local KLV payload start position,
                                     * or NULL */
    dlb_klvpmd_universal_label ul; /**< KLV universal label */
} klv_writer;


/**
 * @brief return space remaining in writer buffer
 */
static inline
unsigned int            /** @return length of unused buffer in bytes */
klv_writer_space
    (klv_writer *w      /**< [in] writer abstraction */
    )
{
    return (unsigned int)(w->end - w->wp);
}


/**
 * @brief helper function to encode BER fields
 */
static inline
int
klv_write_ber
    (uint8_t *wp
    ,size_t v
    )
{
    if (v < 128)
    {
        /* one-byte length field, shift everything down 1 byte */
        *wp++ = (uint8_t)v;
        return 1;
    }
    if (v < 0x100)
    {
        *wp++ = 0x81;
        *wp++ = (uint8_t)v;
        return 2;
    }
    if (v < 0x10000)
    {
        *wp++ = 0x82;
        *wp++ = (v >> 8) & 0xff;
        *wp++ = (v & 0xff);
        return 3;
    }
    if (v < 0x1000000)
    {
        *wp++ = 0x83;
        *wp++ = (v >> 16) & 0xff;
        *wp++ = (v >> 8) & 0xff;
        *wp++ = (v & 0xff);
        return 4;
    }
#if defined(__x86_64__) || defined(_IA64) || defined(__IA64__) || defined(_M_X64)
    if (v < UINT64_C(0x100000000))
    {
        *wp++ = 0x84;
        *wp++ = (v >> 24) & 0xff;
        *wp++ = (v >> 16) & 0xff;
        *wp++ = (v >> 8) & 0xff;
        *wp++ = (v & 0xff);
        return 5;
    }
#endif
    abort();
}


/**
 * @brief write local key tag and reserve space for 2-byte local length
 *
 * We assume that we will never nest local keys, so it is safe to
 * remember only the 'latest tag'.   We set it to NULL to allow for
 * the case where we run out of space for optional tags.
 */
static inline
int                          /** @return 0 on success, 1 on failure */
klv_write_local_open
   (klv_writer *w            /**< [in] KLV writer */
   ,klv_local_tag tag        /**< [in] local tag for this payload */
   )
{
    w->localkey = NULL;
    if (klv_writer_space(w) >= 6)
    {
        w->wp += klv_write_ber(w->wp, tag);
        w->wp += KLV_LENGTH_FIELD_RESERVED;   /* max space for length field (4-byte BER) */
        w->localkey = w->wp;
    }
    return 0;
}


/**
 * @brief fill-in length field of most recent local tag
 *
 * We assume that we will never nest local keys, so it is safe to
 * remember only the 'latest tag'
 */
static inline
int                          /** @return 0 on success, 1 on failure */
klv_write_local_close
   (klv_writer *w            /**< [in] KLV writer */
   )
{
    if (NULL != w->localkey)
    {
        ptrdiff_t length = w->wp - w->localkey;
        uint8_t *wp;
        int lensz;
        
        if (length == 0)
        {
            /* if not enough space for the payload itself,
             * don't write the tag or length */
            w->wp -= 5;
            if (w->wp == w->buffer)
            {
                return 0;
            }
        }
        else
        {
            wp = w->localkey - KLV_LENGTH_FIELD_RESERVED;
            lensz = klv_write_ber(wp, length);
            lensz = KLV_LENGTH_FIELD_RESERVED - lensz;
            if (lensz)
            {
                memmove(w->localkey - lensz, w->localkey, length);
                w->wp -= lensz;
            }
            w->localkey = NULL;
        }
    }
    return 0;
}


/**
 * @brief helper to check whether or not the local key was
 * opened (ie., enough space)
 */
static inline
pmd_bool
klv_write_local_key_opened
   (klv_writer *w            /**< [in] KLV writer */
   )
{
    return w->localkey != NULL;
}


#include "klv_container_config.h"
#include "klv_version.h"


/**
 * @brief initialize writer state
 */
static inline
int                                /** @return 0 on success, 1 on failure */
klv_writer_init
   (klv_writer *w                  /**< [in] writer state */
   ,dlb_pmd_model *model           /**< [in] model being written */
   ,uint8_t sindex                 /**< [in] ED2 stream index or 0xff */
   ,unsigned char *buf             /**< [in] output buffer to populate */
   ,size_t capacity                /**< [in] capacity of output buffer */
   ,dlb_klvpmd_universal_label ul  /**< [in] KLV Universal label */
   )
{
    w->model  = model;
    w->sindex = sindex;
    w->buffer = buf;
    w->wp     = w->buffer;
    w->end    = w->wp + capacity - KLV_BYTES_FOR_CRC_PAYLOAD;
    w->ul     = ul;

    memmove(w->wp, DLB_PMD_KLV_UNIVERSAL_KEY_HEADER, sizeof(DLB_PMD_KLV_UNIVERSAL_KEY_HEADER));

    switch (ul)
    {
    case DLB_PMD_KLV_UL_ST2109: memmove(&w->wp[8], DLB_PMD_KLV_LOCAL_SET_ST2109, 8); break;
    case DLB_PMD_KLV_UL_DOLBY:  memmove(&w->wp[8], DLB_PMD_KLV_LOCAL_SET_DOLBY,  8); break;
    default:  return 1;
    }
    
    w->wp += UNIVERSAL_KEY_SIZE;

    memset(w->wp, '\0', KLV_LENGTH_FIELD_RESERVED);
    w->wp += KLV_LENGTH_FIELD_RESERVED;

    /* we must always add a mandatory container config */
    return klv_write_local_open(w, KLV_PMD_LOCAL_TAG_CONFIG)
        || klv_container_config_write(w)
        || klv_write_local_close(w)
        || klv_version_write(w);
}

    
/**
 * @brief finish writer, send final data to be written
 */
static inline
unsigned int         /** @return number of bytes written */
klv_writer_finish
   (klv_writer *w    /**< [in] writer abstraction */
   )
{
    unsigned int crc_size = 0;
    uint32_t crc32;
    size_t vlen;  /* length of value */
    size_t vlen2;
    uint8_t *wp;

    /* do we have enough space for the crc? */
    if (klv_writer_space(w) >= 6)
    {
        crc_size = 6;
    }

    wp = w->buffer + UNIVERSAL_KEY_SIZE;
    /* wp points to length field */

    /* compute size of write buffer *after* reserved length field,
     * adding 8 bytes for CRC payload, if available.
     */
    vlen = w->wp - wp - KLV_LENGTH_FIELD_RESERVED + crc_size;
    assert(vlen < 0xffffff);
    /* we always write long-form here */
    *wp++ = 0x83;
    *wp++ = (vlen >> 16) & 0xff;
    *wp++ = (vlen >> 8) & 0xff;
    *wp++ = (vlen & 0xff);

    /* wp points to start of Audio Metadata payload, and w->wp the end */
    if (crc_size)
    {
        *w->wp++ = (uint8_t)KLV_PMD_LOCAL_TAG_CRC;
        *w->wp++ = 4;  /* length of crc32 is 4 bytes */
        *w->wp++ = 0;
        *w->wp++ = 0;
        *w->wp++ = 0;
        *w->wp++ = 0;

        crc32 = pmd_compute_crc32(wp, vlen-4);
        w->wp[-4] = (crc32 >> 24) & 0xff;
        w->wp[-3] = (crc32 >> 16) & 0xff;
        w->wp[-2] = (crc32 >> 8) & 0xff;
        w->wp[-1] = crc32 & 0xff;
    }

    vlen = (w->wp - w->buffer);
    /* round up to next highest even number of 20-bit words, which is
     * what will be stored in smpte headers
     */
    vlen2 = ((vlen + 9)/10) * 10;
    memset(w->wp, '\0', vlen2 - vlen);
    return (unsigned int)vlen2;
}


#endif /* KLV_WRITER_H_ */
