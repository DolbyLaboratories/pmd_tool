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

#ifndef KLV_H_
#define KLV_H_

#include <assert.h>
#include "klv_bitfield_helpers.h"

#ifdef __GNUC__
#  define MAY_BE_UNUSED __attribute__((unused))
#else
#  define MAY_BE_UNUSED
#endif


/**
 * @def UNIVERSAL_KEY_SIZE (16)
 * @brief length of an ST2109 universal key in bytes
 */
#define UNIVERSAL_KEY_SIZE (16)


/**
 * @brief top 8 bytes of SMPTE 2109 'Audio Medatadata Pack' Universal Label (UL)
 */
static uint8_t DLB_PMD_KLV_UNIVERSAL_KEY_HEADER[8] =
{
    /* UL Header, bytes 1-2 ------------------------------------------------------ */
    0x06,    /*  1: Object Identifier (OID) fixed byte, always 0x06                */
    0x0E,    /*  2: UL Size,               always 0x0E                             */

    /* UL Designator, bytes 3-8 -------------------------------------------------- */
    0x2B,    /*  3: UL Code,              always 0x2B                              */
    0x34,    /*  4: SMPTE Designator,     always 0x34                              */
    0x02,    /*  5: Category Designator,  '2' means 'Groups(Sets and Packs)'       */
    0x05,    /*  defined-length pack                                               */
    0x01,    /*  7: Structure Designator, 'major version number'                   */
    0x01,    /*  8: Version Number                                                 */
};


/**
 * @brief bottom 8 bytes of SMPTE 2109 Universal Label
 */
static uint8_t DLB_PMD_KLV_LOCAL_SET_ST2109[8] =
{
    /* Local Set Designator, bytes 9-6 ------------------------------------------- */
    0x0C,    /* these 8 bytes are registered with SMPTE and mean 'Audio Metadata Set'*/
    0x04,    /* according to SMPTE ST2109 */
    0x01,    
    0x00,    
    0x00,
    0x00,
    0x00,
    0x00
};


/**
 * @brief bottom 8 bytes of Dolby Private Universal Label
 */
static uint8_t DLB_PMD_KLV_LOCAL_SET_DOLBY[8] =
{
    /* Local Set Designator, bytes 9-6 ------------------------------------------- */
    0x0E,
    0x09,
    0x07,
    0x01,
    0x00,    
    0x00,
    0x00,
    0x00,
};


/**
 * @brief 11-byte Payload UL prefix
 *
 * These eleven bytes form the first 11 bytes of a 16-byte Universal Label
 * specifying a tag.  For tag byte X, we create the 16-byte label by
 * appending the 5 bytes X,0,0,0,0.  These fields are imported for dynamic
 * tagging.
 */
MAY_BE_UNUSED
static uint8_t DLB_PMD_KLV_PAYLOAD_UL[11] =
{
    0x06,
    0x0e,
    0x2b,
    0x34,
    0x04,
    0x01,
    0x01,
    0x0d,
    0x04,
    0x04,
    0x02
};


/**
 * @def KLV_LENGTH_FIELD_RESERVED
 * @brief space to reserve for the KLV length field.
 *
 * We don't know how many octets we'll need for the payload until we
 * actually write it.  The KLV length field is variable length, but we
 * *do* know that in our application it will only ever be at most 3
 * octets long.
 *
 * The maximum length of a PCM block KLV buffer is 390 octets:
 *
 *    (160 sample-burst
 *     - 4) sync words
 *          * 20 bits
 *              / 8 bits-per-byte
 *
 * For actual lengths < 128 octets long, we use 1 octet, which is just
 * the actual length (bit 7 is 0 in this case).
 *
 * For lengths >= 128 octets, we use 1 'length-of-length' byte to
 * encode the number of bytes encoding the length (and here this byte
 * has bit 7 set to 1).  The actual length is coded in up-to two
 * octets in big-endian order.
 *
 * The intent is that we reserve these bytes until we write the entire
 * payload, then write the length field, and shift the payload down
 * if the length field uses fewer than 3 octets.
 */
#define KLV_LENGTH_FIELD_RESERVED (4)


/**
 * @brief enumerate KLV 'local' tags for PMD metadata
 *
 * PMD is transmitted as a 'localset'.  The format of the entire payload is:
 *   - 16-byte universal key (whose structure indicates it is a local set key),
 *      @todo: make sure that the UL key above codes for
 *             - 2 byte local tags
 *             - 2-byte length fields (big-endian) for tags
 *   - ASN.1 BER coded length
 *   - payloads P0 to Pn, where each Pi has form
 *      - BER coded local tag  (in range 0-127, ASN.1 'short form')
 *      - BER coded bytes length field (, short form) per tag
 *      - payload!
 */
typedef enum 
{
   KLV_PMD_LOCAL_TAG_UNUSED                   = 0,
   /* administrative tags ----------------------------------------- */
   KLV_PMD_LOCAL_TAG_CONFIG                   = 0x01,
   KLV_PMD_LOCAL_TAG_SYNC                     = 0x02,     /* not used in PMD */
   KLV_PMD_LOCAL_TAG_CRC                      = 0x03,

   /* PMD payloads  -------------------------------------- (abbrev) */
   KLV_PMD_LOCAL_TAG_VERSION                  = 0x04,
   KLV_PMD_LOCAL_TAG_AUDIO_BED_DESC           = 0x05,  /* (ABD) */
   KLV_PMD_LOCAL_TAG_AUDIO_OBJECT_DESC        = 0x06,  /* (AOD) */
   KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_DESC  = 0x07,  /* (APD) */
   KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_NAMES = 0x08,  /* (APN) */
   KLV_PMD_LOCAL_TAG_AUDIO_ELEMENT_NAMES      = 0x09,  /* (AEN) */
   KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_DESC       = 0x0A,  /* (ESD) */
   KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_NAMES      = 0x0B,  /* (ESN) */
   KLV_PMD_LOCAL_TAG_EAC3_ENCODING_PARAMETERS = 0x0C,  /* (EEP) */
   KLV_PMD_LOCAL_TAG_DYNAMIC_UPDATES          = 0x0D,  /* (XYZ) */
   KLV_PMD_LOCAL_TAG_IAT                      = 0x0E,  /* (IAT) */
   KLV_PMD_LOCAL_TAG_PRES_LOUDNESS_DESC       = 0x0F,  /* (PLD) */
   KLV_PMD_LOCAL_TAG_ED2_TURNAROUND_DESC      = 0x10,  /* (ETD) */
   KLV_PMD_LOCAL_TAG_HEADPHONE_ELEMENT_DESC   = 0x11,  /* (HED) */

} klv_local_tag;


/**
 * @def KLV_CONTAINER_CONFIG_VERSION
 * @brief one-byte version field defined in ST2109
 *
 * This indicates the version of the ST2109 document that defines the
 * Audio Metadata application of the SMPTE 336m 'KLV' metadata format.
 */
#define KLV_CONTAINER_CONFIG_VERSION (0)


/**
 * @brief helper to match incoming 16-bytes against universal label
 */
static inline
pmd_bool                             /** @return 1 if 16-bytes match, 0 otherwise */
klv_match_universal_label
    (uint8_t *data                   /**< [in] data buffer to parse */
    ,size_t length                   /**< [in] length of data buffer */
    )
{
    if (length <= 16)
    {
        return 0;
    }

    /* first eight bytes must match exactly */
    if (memcmp(data, DLB_PMD_KLV_UNIVERSAL_KEY_HEADER, 8))
    {
        return 0;
    }
    /* compare against bottom 8 bytes exactly */
    if (!memcmp(&data[8], DLB_PMD_KLV_LOCAL_SET_ST2109, 8))
    {
        return 1;
    }
    if (!memcmp(&data[8], DLB_PMD_KLV_LOCAL_SET_DOLBY, 8))
    {
        return 1;
    }
    return 0;
}


#endif /* KLV_H */
