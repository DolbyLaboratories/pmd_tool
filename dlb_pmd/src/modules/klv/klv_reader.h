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
 * @file klv_reader.h
 * @brief KLV reader abstraction
 */

#ifndef KLV_READER_H_
#define KLV_READER_H_

#include "pmd_model.h"
#include "pmd_bitstream_version.h"
#include "klv.h"
#include "pmd_idmap.h"
#include <stdio.h>


/**
 * @brief hidden reference to a global variable used to determine
 * whether or not we're checking version numbers.
 *
 * This is currently defined in the pmd_xml_reader.c file.
 */
extern pmd_bool global_testing_version_numbers;


/**
 * @brief reader abstraction structure
 */
typedef struct
{
    dlb_pmd_model *model;         /**< model being created */
    uint8_t *buffer;              /**< buffer to read */
    uint8_t *end;                 /**< 1st byte after end of buffer */
    uint8_t *rp;                  /**< current read pointer */

    char *errmsg;                 /**< current write pointer for error messages */
    int errmsg_capacity;          /**< remaining capacity of error message buffer */

    uint8_t stream_index;         /**< ED2 stream index if present */

    pmd_signals signals;          /**< discovered channels */
    unsigned int num_signals;     /**< number of discovered audio signals */

    pmd_idmap *element_ids;            /**< maps element ids to array indices in model */
    pmd_idmap *apd_ids;       /**< maps presentation ids to array indices in model */
    pmd_idmap *eep_ids; /**< maps eac3 encoding parameters ids to
                                         * array indices in model */
    pmd_idmap *etd_ids;     /**< maps ED2 turnaround ids to array indices in model */
    pmd_idmap *aen_ids;       /**< maps element ids to array of element names */
} klv_reader;
    

/**
 * @brief helper function to compute unread space
 */
static inline
int                      /** @return unread space */
klv_reader_space
    (klv_reader *r       /**< [in] reader abstraction */
    )
{
    return (int)(r->end - r->rp);
}


/**
 * @brief helper function to return current byte position
 */
static inline
int                      /** @return unread space */
klv_reader_pos
    (klv_reader *r       /**< [in] reader abstraction */
    )
{
    return (int)(r->rp - r->buffer);
}


/**
 * @brief helper function to add error information
 */
static inline
void
klv_reader_error
    (klv_reader *r       /**< [in] reader abstraction */
    ,const char *fmt     /**< [in] format string */
    ,...                 /**< [in] varags */
    )
{
    va_list args;
    int len;
    
    va_start(args, fmt);
    len = vsnprintf(r->errmsg, r->errmsg_capacity, fmt, args);
    va_end(args);

    if (len > r->errmsg_capacity)
    {
        len = r->errmsg_capacity;
    }
    r->errmsg += len;
    r->errmsg_capacity -= len;
}


/**
 * @brief write an error message with location information
 */
static inline
void
klv_reader_error_at
    (klv_reader *r                              /**< [in] reader abstraction */
    ,dlb_pmd_payload_status code                /**< [in] error code */
    ,dlb_pmd_payload_status_record *read_status /**< [out] current read status record (may be NULL) */
    ,const char *fmt                            /**< [in] format string */
    ,...                                        /**< [in] varags */
    )
{
    va_list args;
    int len;

    len = snprintf(r->errmsg, r->errmsg_capacity,
                   "Error at byte %u: ", klv_reader_pos(r));
    if (len > r->errmsg_capacity)
    {
        len = r->errmsg_capacity;
    }
    r->errmsg += len;
    r->errmsg_capacity -= len;
    
    va_start(args, fmt);
    len = vsnprintf(r->errmsg, r->errmsg_capacity, fmt, args);
    va_end(args);

    if (read_status)
    {
        read_status->payload_status = code;
        read_status->error_description[DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_LAST] = '\0';
        strncpy(read_status->error_description, r->errmsg, DLB_PMD_PAYLOAD_ERROR_DESCRIPTION_LAST);
    }

    if (len > r->errmsg_capacity)
    {
        len = r->errmsg_capacity;
    }
    r->errmsg += len;
    r->errmsg_capacity -= len;
}


/**
 * @brief helper function to read BER-encoded length
 */
static inline
int                                                 /** @return 0 on success, 1 on failure */
klv_read_ber_value
    (klv_reader *r                                  /**< [in] reader abstraction */
    ,unsigned int *value                            /**< [out] decoded value */
    ,unsigned int *encoding_length                  /**< [out] optional size of payload length encoding,
                                                      *        or NULL if unwanted */
    , dlb_pmd_payload_status_record *read_status    /**< [out] optional read status record */
    )
{
    ptrdiff_t length = r->end - r->rp;
    int count;
    uint64_t v = 0;
    uint8_t byte;

    if (length < 1)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status, "BER length too small\n");
        return 1;
    }

    byte = *r->rp;
    r->rp += 1;
    if (byte < 128)
    {
        *value = byte;
        if (encoding_length)
        {
            *encoding_length = 1;
        }
        return 0;
    }

    count = (byte & 0x7f); /* number of length bytes */
    if (length < count+1)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status, "BER length extends beyond end of buffer\n");
        return 1;
    }
    if (encoding_length)
    {
        *encoding_length = count+1;
    }
    while (count)
    {
        v = (v << 8) | *r->rp;
        r->rp += 1;
        count -= 1;
        if (v > 0xffffff)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status, "BER length too large\n");
            return 1;
        }
    }
    *value = (unsigned int)(v & 0xffffff);
    return 0;
}


/**
 * @brief helper function to reinitialize key parts of decoder on new frame
 */
static inline
void
klv_reader_new_frame
    (klv_reader *r                 /**< [in] reader abstraction */
    ,dlb_pmd_model *model          /**< [in] destination model struct */
    )
{
    TRACE(("KLV READ NEW FRAME\n"));

    pmd_model_new_frame(model);
    r->num_signals = 0;
    pmd_signals_init(&r->signals);    
    pmd_idmap_init(r->element_ids);
    pmd_idmap_init(r->apd_ids);
    if (model->esd)
    {
        model->esd->streams_read = 0;
    }
}


/**
 * @brief if the payload is CRC32-protected, check 
 *
 * The function returns PMD_TRUE either if there is no CRC, or if it
 * does exist, then the buffer passes the CRC32 check.
 */
static inline
pmd_bool                           /** @return PMD_TRUE on success, PMD_FALSE otherwise */
klv_reader_check_crc32
    (uint8_t *buf                  /**< [in] data buffer to parse */
    ,int length                    /**< [in] length of data buffer */
    )
{
    /* The crc exists if the CRC32 payload is the last word of the buffer
     * That has 4 bytes for the crc, 2 bytes for length and 2 bytes for
     * tag.  Hence the last 8 bytes should look like
     *
     *   0x01  -> tag msb
     *   0x03  -> tag lsb (KLV_PMD_LOCAL_TAG_CRC32)
     *   0x00  -> length msb
     *   0x04  -> length lsb
     *   crc32[0]
     *   crc32[1]
     *   crc32[2]
     *   crc32[3]
     */
    uint8_t *crc_payload = buf + length - 6;

    if (!memcmp(crc_payload, "\x03\x04", 2))
    {
        uint32_t crc32 = pmd_compute_crc32(buf, length);
        return (crc32 == 0);
    }
    return PMD_TRUE;
}


/**
 * @brief helper function to initialize read struct
 */
static inline
int                                /** @return 0 on success, 1 on failure */
klv_reader_init
    (klv_reader *r                 /**< [in] reader abstraction */
    ,pmd_bool new_frame            /**< [in] start of a new video frame? */
    ,dlb_pmd_model *model          /**< [in] destination model struct */
    ,uint8_t *buf                  /**< [in] data buffer to parse */
    ,size_t length                 /**< [in] length of data buffer */
    )
{
    unsigned int payload_length;

    new_frame = new_frame
        && (r->stream_index == 0 || r->stream_index == DLB_PMD_NO_ED2_STREAM_INDEX);

    r->element_ids = &model->element_ids;
    r->apd_ids = &model->apd_ids;
    r->eep_ids = &model->eep_ids;
    r->etd_ids = &model->etd_ids;
    r->aen_ids = &model->aen_ids;

    /* we don't know if we have a CRC32 payload, so we can't check
     * whether we have correct bitstream
     */
    if (new_frame)
    {
        klv_reader_new_frame(r, model);
    }
    else
    {
        r->num_signals = model->num_signals;
        pmd_signals_copy(&r->signals, &model->signals);
    }

    r->model      = model;
    r->buffer     = buf;
    r->end        = r->buffer + length;
    r->rp         = r->buffer;
    r->stream_index = DLB_PMD_NO_ED2_STREAM_INDEX;
    r->errmsg     = model->error;
    r->errmsg_capacity = sizeof(model->error);

    if (!klv_match_universal_label(r->rp, length))
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, NULL, "Universal Label not found\n");
        return 1;
    }

    length -= UNIVERSAL_KEY_SIZE;
    r->rp += UNIVERSAL_KEY_SIZE;

    if (klv_read_ber_value(r, &payload_length, NULL, NULL))
    {
        return 1;
    }

    if (r->rp + payload_length > r->end)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, NULL, "payload length longer than byte buffer\n");
        return 1;
    }

    if (!klv_reader_check_crc32(r->rp, payload_length))
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_ERROR, NULL, "CRC failure\n");
        return 1;
    }
    
    /* reset end pointer to be end of payload, to skip any
     * trailing bytes that may be added by transport
     */
    r->end = r->rp + payload_length;
    return 0;
}


/* Language code character encoding*/
#define LANG_CODE_CHAR_ENC_FIRST_RESERVED (0x1c)
#define LANG_CODE_CHAR_ENC_LAST_RESERVED (0x1f)


/**
* @brief validate an encoded language code character
*/
static inline
dlb_pmd_payload_status
klv_reader_validate_langcod_char
    (uint8_t char_val
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (char_val > LANG_CODE_CHAR_ENC_LAST_RESERVED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (char_val >= LANG_CODE_CHAR_ENC_FIRST_RESERVED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


#endif /* KLV_READER_H_ */
