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
    ,dlb_pmd_payload_status_record *read_status
    )
{
    pmd_iat *iat = model->iat;

    if (!iat)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, read_status, "model constraints exclude IAT\n");
        return 1;
    }
    
    if (iat->options & PMD_IAT_PRESENT)
    {
        if (model->iat_read_this_frame)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, read_status, "Only one IAT allowed per frame\n");
            return 1;
        }
        /* otherwise, reset IAT ready to be overwritten */
        memset(iat, '\0', sizeof(*iat));
    }
    
    if (klv_iat_read(r, len, iat, read_status))
    {
        return 1; /* decode error */
    }
    
    iat->options |= PMD_IAT_PRESENT;
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
    ,dlb_pmd_payload_set_status *read_status
    )
{
    int final_status = 0;
    dlb_pmd_payload_status_record *payload_set_status = (read_status != NULL) ? &read_status->payload_set_status : NULL;
    unsigned int remaining = (unsigned int)(r->end - r->rp);
    unsigned int lenlen;
    unsigned int taglen;
    unsigned int len;
    unsigned int tag;
    
    TRACE(("read payload...\n"));

    while (remaining > 4)
    {
        int ber_status;

        ber_status = klv_read_ber_value(r, &tag, &taglen, payload_set_status);
        if (ber_status)
        {
            return 1;
        }

        ber_status = klv_read_ber_value(r, &len, &lenlen, payload_set_status);
        if (ber_status)
        {
            return 1;
        }
        
        if (remaining < (taglen + lenlen + len))
        {
            /* This may happen when trying to parse padding bytes at the end
             * of the buffer: we must pad up to a multiple of 10 */
            if (remaining > 9)
            {
                klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_ERROR, payload_set_status, "Unexpected padding at end of buffer\n");
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
            dlb_pmd_payload_status_record *current_status_record = NULL;
            dlb_pmd_bool show_ok = PMD_TRUE;
            int read_result = 0;

            switch (tag)
            {
            case KLV_PMD_LOCAL_TAG_CONFIG:
                TRACE(("    read container config..."));
                if (klv_container_config_read(r, len))
                {
                    TRACE((" failed\n"));
                    return 1;
                }
                break;

            case KLV_PMD_LOCAL_TAG_SYNC:
                /* sync payload wraps a sequence of tags. We want to
                 * ignore Sync info, but parse the nested tags */
                len = 0;
                show_ok = PMD_FALSE;
                break;
                
            case KLV_PMD_LOCAL_TAG_CRC:
                /* ignore: we will already have processed this (if it
                 * was the final payload) */
                /* DO set the read_status flag */
                if (read_status)
                {
                    read_status->has_crc_payload = PMD_TRUE;
                }
                show_ok = PMD_FALSE;
                break;
                
            case KLV_PMD_LOCAL_TAG_VERSION:
                TRACE(("    read bitstream version..."));
                if (read_status)
                {
                    read_status->has_ver_payload = PMD_TRUE;
                    current_status_record = &read_status->ver_payload_status;
                }
                read_result = klv_version_read(r, len, model, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_BED_DESC:
                TRACE(("    read beds...\n"));
                if (read_status)
                {
                    read_status->has_abd_payload = PMD_TRUE;
                    current_status_record = &read_status->abd_payload_status;
                }
                read_result = klv_abd_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_OBJECT_DESC:
                TRACE(("    read objects...\n"));
                if (read_status)
                {
                    read_status->has_aod_payload = PMD_TRUE;
                    current_status_record = &read_status->aod_payload_status;
                }
                read_result = klv_aod_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_DESC:
                TRACE(("    read presentations...\n"));
                if (read_status)
                {
                    read_status->has_apd_payload = PMD_TRUE;
                    current_status_record = &read_status->apd_payload_status;
                }
                read_result = klv_apd_read(r, len, current_status_record);
                break;                

            case KLV_PMD_LOCAL_TAG_AUDIO_PRESENTATION_NAMES:
                TRACE(("    read presentation names...\n"));
                if (read_status)
                {
                    read_status->has_apn_payload = PMD_TRUE;
                    current_status_record = &read_status->apn_payload_status;
                }
                read_result = klv_apn_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_AUDIO_ELEMENT_NAMES:
                TRACE(("    read element names...\n"));
                if (read_status)
                {
                    read_status->has_aen_payload = PMD_TRUE;
                    current_status_record = &read_status->aen_payload_status;
                }
                read_result = klv_aen_read(r, len, current_status_record);
                break;


            case KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_DESC:
                TRACE(("    read ED2 substream description...\n"));
                if (read_status)
                {
                    read_status->has_esd_payload = PMD_TRUE;
                    current_status_record = &read_status->esd_payload_status;
                }
                read_result = klv_esd_read(r, len, model->esd, current_status_record);
                model->esd_present = 1;
                break;

            case KLV_PMD_LOCAL_TAG_ED2_SUBSTREAM_NAMES:
                TRACE(("    read ED2 substream names...\n"));
                if (read_status)
                {
                    read_status->has_esn_payload = PMD_TRUE;
                    current_status_record = &read_status->esn_payload_status;
                }
                read_result = klv_esn_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_EAC3_ENCODING_PARAMETERS:
                TRACE(("    read EAC3 encoding parameters...\n"));
                if (read_status)
                {
                    read_status->has_eep_payload = PMD_TRUE;
                    current_status_record = &read_status->eep_payload_status;
                }
                read_result = klv_eep_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_DYNAMIC_UPDATES:
                TRACE(("    read dynamic update...\n"));
                if (read_status && read_status->xyz_payload_count <= read_status->xyz_payload_count_max)
                {
                    current_status_record = &read_status->xyz_payload_status[read_status->xyz_payload_count++];
                }
                read_result = klv_xyz_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_IAT:
                TRACE(("    read IAT...\n"));
                if (read_status)
                {
                    read_status->has_iat_payload = PMD_TRUE;
                    current_status_record = &read_status->iat_payload_status;
                }
                read_result = klv_read_iat_key(r, model, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_PRES_LOUDNESS_DESC:
                TRACE(("    read PLD...\n"));
                if (read_status)
                {
                    read_status->has_pld_payload = PMD_TRUE;
                    current_status_record = &read_status->pld_payload_status;
                }
                read_result = klv_pld_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_ED2_TURNAROUND_DESC:
                TRACE(("    read ED2 turnaround descriptor...\n"));
                if (read_status)
                {
                    read_status->has_etd_payload = PMD_TRUE;
                    current_status_record = &read_status->etd_payload_status;
                }
                read_result = klv_etd_read(r, len, current_status_record);
                break;

            case KLV_PMD_LOCAL_TAG_HEADPHONE_ELEMENT_DESC:
                TRACE(("    read headphone element descriptor...\n"));
                if (read_status)
                {
                    read_status->has_hed_payload = PMD_TRUE;
                    current_status_record = &read_status->hed_payload_status;
                }
                read_result = klv_hed_read(r, len, current_status_record);
                break;

            default:
                TRACE(("    skip unsupported payload %02x\n", tag));
                show_ok = PMD_FALSE;
                break;
            }

            if (read_result)
            {
                TRACE((" failed\n"));
                if (read_status)
                {
                    final_status = 1;
                }
                else
                {
                    return 1;
                }
            }
            else if (show_ok)
            {
                TRACE((" OK\n"));
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
    if (final_status && read_status && !read_status->payload_set_status.payload_status)
    {
        read_status->payload_set_status.payload_status = DLB_PMD_PAYLOAD_STATUS_ERROR;
        strncpy(read_status->payload_set_status.error_description, "Error reading KLV", DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_MAX);
    }
    /* only succeed (return 0) if we've read everything */
    return final_status || (remaining != 0);
}


int
dlb_klvpmd_read_payload
    (uint8_t                    *buffer
    ,size_t                      length
    ,dlb_pmd_model              *model
    ,int                         new_frame
    ,uint8_t                    *sindex
    ,dlb_pmd_payload_set_status *read_status
    )
{
    klv_reader r;
    int cb_res = 0;
    int res;

    if (read_status)
    {
        dlb_pmd_clear_payload_set_status(read_status);
        read_status->new_frame = new_frame;
    }

    pmd_mutex_lock(&model->lock);

    r.stream_index = sindex ? *sindex : 0;

    res =  klv_reader_init(&r, new_frame, model, buffer, length)
        || klv_read_local_keys(&r, model, read_status);

    if (!res && !model->version_avail)
    {
        dlb_pmd_payload_status_record *payload_set_status = (read_status != NULL) ? &read_status->payload_set_status : NULL;
        klv_reader_error_at(&r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, payload_set_status, "No version payload found\n");
        res = 1;
    }

    pmd_mutex_unlock(&model->lock);

    if (NULL != sindex)
    {
        *sindex = r.stream_index;
    }

    if (read_status && read_status->callback != NULL)
    {
        cb_res = (*read_status->callback)(read_status);
    }
    
    return (cb_res || res);
}

