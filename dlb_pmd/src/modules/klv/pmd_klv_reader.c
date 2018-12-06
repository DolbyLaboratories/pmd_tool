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

/**
 * @file klv_reader.c
 * @brief populate model from a sequence of SMPTE 336 (KLV)-encoded bytes
 */

#include "dlb_pmd_klv.h"
#include "pmd_model.h"

//#define KLV_READ_TRACE
#ifndef TRACE
#  ifdef KLV_READ_TRACE
#    define TRACE(x) printf x
#  else
#    define TRACE(x)
#  endif
#endif

#include "pmd_crc32.h"
#include "klv_reader.h"
#include "klv_abd.h"
#include "klv_aod.h"
#include "klv_aen.h"
#include "klv_apd.h"
#include "klv_pld.h"
#include "klv_hed.h"
#include "klv_xyz.h"
#include "klv_eep.h"
#include "klv_etd.h"
#include "klv_iat.h"
#include "klv_apn.h"
#include "klv_esd.h"
#include "klv_esn.h"
#include "klv_version.h"
#include "klv_container_config.h"


/**
 * @brief handle the extraction of an IAT from KLV and storing it in model
 */
static inline
int                                 /** @return 0 on success, 1 on error */
klv_read_iat_key
    (klv_reader *r
    ,dlb_pmd_model *model
    ,unsigned int len
    )
{
    if (model->iat.options & PMD_IAT_PRESENT)
    {
        if (model->iat_read_this_frame)
        {
            klv_reader_error_at(r, "Only one IAT allowed per frame\n");
            return 1;
        }
        /* otherwise, reset IAT ready to be overwritten */
        memset(&model->iat, '\0', sizeof(model->iat));
    }

    if (klv_iat_read(r, len, &model->iat))
    {
        klv_reader_error(r, "decoding IAT\n");
        return 1; /* decode error */
    }

    model->iat.options |= PMD_IAT_PRESENT;
    model->iat_read_this_frame = 1;
    return 0;
}


/**
 * @brief remap local tag if it has been dynamically updated
 */
static inline
pmd_bool                         /** @return 1 if 'remapped' tag is understood, 0 if not */
klv_remap_tag
    (pmd_smpte2109 *smpte2109    /**< [in] current SMPTE 2109 payload and container config info */
    ,unsigned int *tagptr        /**< [in] tag to be remapped, [out] tag after remapping, if any */
    )
{
    unsigned int tag = *tagptr;

    /* we cannot remap the container config payload */
    /* NOTE: this relies on the fact that payload ULs follow a pattern:
     * 11 constant bytes, then a PMD local tag, then four zero bytes.
     * In the future, this could change.  In which case, we'd need to
     * store a table of payload ULs
     */
    if (tag != KLV_PMD_LOCAL_TAG_CONFIG)
    {
        pmd_dynamic_tag *dtag = smpte2109->dynamic_tags;
        unsigned int i = 0;
        
        while (i < smpte2109->num_dynamic_tags)
        {
            if (dtag->local_tag == tag)
            {
                /* check the universal label to see if it belongs to PMD */
                if (!memcmp(dtag->universal_label, DLB_PMD_KLV_PAYLOAD_UL,
                            sizeof(DLB_PMD_KLV_PAYLOAD_UL)))
                {
                    tag = dtag->universal_label[11];
                    *tagptr = tag;
                    return 1;
                }
                return 0;
            }
            ++dtag;
            ++i;
        }
    }
    return 1;
}


/**
 * @brief read all the local keys until error
 */
static inline
int                           /** @return 0 on success, 1 on failure */
klv_read_local_keys
    (klv_reader *r
    ,dlb_pmd_model *model
    )
{
    unsigned int remaining = (unsigned int)(r->end - r->rp);
    unsigned int lenlen;
    unsigned int taglen;
    unsigned int len;
    unsigned int tag;
    
    TRACE(("read payload...\n"));

    while (remaining > 4)
    {
        if (klv_read_ber_value(r, &tag, &taglen))
        {
            klv_reader_error_at(r, "Could not read BER-encoded tag\n");
            return 1;
        }
        
        if (   klv_read_ber_value(r, &len, &lenlen)
            || remaining < (taglen + lenlen + len))
        {
            /* This may happen when trying to parse padding bytes at the end
             * of the buffer: we must pad up to a multiple of 10 */
            if (remaining > 9)
            {
                klv_reader_error_at(r, "Unexpected padding at end of buffer\n");
                return 1;
            }
            remaining = 0;
            break;
        }
        
        if (!klv_remap_tag(&model->smpte2109, &tag))
        {
            r->rp += len;
        }
        else
        {
            switch (tag)
            {
            case KLV_PMD_LOCAL_TAG_CONFIG:
                TRACE(("    read container config..."));
                if (klv_container_config_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_SYNC:
                /* sync payload wraps a sequence of tags. We want to
                 * ignore Sync info, but parse the nested tags */
                len = 0;
                break;
                
            case KLV_PMD_LOCAL_TAG_CRC:
                /* ignore: we will already have processed this (if it
                 * was the final payload) */
                break;
                
            case KLV_PMD_LOCAL_TAG_VERSION:
                TRACE(("    read bitstream version..."));
                if (klv_version_read(r, len, model))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_BED_DESC:
                TRACE(("    read beds...\n"));
                if (klv_abd_read(r, len))
                {
                    TRACE(("    failed\n"));
                    return 1;
                }
                TRACE(("    OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_OBJECT_DESC:
                TRACE(("    read objects...\n"));
                if (klv_aod_read(r, len))
                {
                    TRACE(("    failed\n"));
                    return 1;
                }
                TRACE(("    OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_DESC:
                TRACE(("    read presentations...\n"));
                if (klv_apd_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;                

            case KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_NAMES:
                TRACE(("    read presentation names...\n"));
                if (klv_apn_read(r, len))
                {
                    TRACE(("    failed\n"));
                    return 1;
                }
                TRACE(("    OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_ELEMENT_NAMES:
                TRACE(("    read element names...\n"));
                if (klv_aen_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;


            case KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_DESC:
                TRACE(("    read ED2 substream description...\n"));
                if (klv_esd_read(r, len, &model->esd))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                model->esd_present = 1;
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_NAME:
                TRACE(("    read ED2 substream names...\n"));
                if (klv_esn_read(r, len))
                {
                    TRACE(("   failed\n"));
                    return 1;
                }
                TRACE(("   OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_EAC3_ENCODING_PARAMETERS:
                TRACE(("    read EAC3 encoding parameters...\n"));
                if (klv_eep_read(r, len))
                {
                    TRACE(("    failed\n"));
                    return 1;
                }
                TRACE(("    OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_DYNAMIC_UPDATES:
                TRACE(("    read dynamic update...\n"));
                if (klv_xyz_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_IAT:
                TRACE(("    read IAT...\n"));
                if (klv_read_iat_key(r, model, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_PRES_LOUDNESS_DESC:
                TRACE(("    read PLD...\n"));
                if (klv_pld_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_ED2_TURNAROUND_DESC:
                TRACE(("    read ED2 turnaround descriptor...\n"));
                if (klv_etd_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            case KLV_PMD_LOCAL_TAG_HEADPHONE_ELEMENT_DESC:
                TRACE(("    read headphone element descriptor...\n"));
                if (klv_hed_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                TRACE((" OK\n"));
                break;

            default:
                TRACE(("    skip unsupported payload %02x\n", tag));
                break;
            }
        }
        
        remaining -= (taglen + lenlen + len);
    }

    model->num_signals = r->num_signals > MAX_AUDIO_SIGNALS
        ? MAX_AUDIO_SIGNALS
        : r->num_signals;

    pmd_signals_init(&model->signals);
    if (r->num_signals > 0)
    {
        pmd_signals_copy(&model->signals, &r->signals);
    }
    /* only succeed (return 0) if we've read everything */
    return remaining != 0;
}


int
dlb_klvpmd_read_payload
    (uint8_t            *buffer
    ,size_t              length
    ,dlb_pmd_model      *model
    ,int                 new_frame
    ,uint8_t            *sindex
    )
{
    klv_reader r;
    int res;

    pmd_mutex_lock(&model->lock);

    r.stream_index = sindex ? *sindex : 0;

    res =  klv_reader_init(&r, new_frame, model, buffer, length)
        || klv_read_local_keys(&r, model);

    if (!res && !model->version_avail)
    {
        klv_reader_error_at(&r, "No version payload found\n");
        res = 1;
    }

    pmd_mutex_unlock(&model->lock);

    if (NULL != sindex)
    {
        *sindex = r.stream_index;
    }
    
    return res;
}

