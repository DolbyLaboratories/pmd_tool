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
 * @file klv_version.h
 * @brief defines reading and writing internal bitstream version local tag
 */


#ifndef KLV_VERSION_H_
#define KLV_VERSION_H_

#include "klv_reader.h"


/**
 * @brief write local bitstream version tag
 */
static inline
int                          /** @return 0 on success, 1 on failure */
klv_version_write
    (klv_writer *w
    )
{
    if (w->model->version_avail != 0xff)
    {
        if (klv_writer_space(w) >= 5)
        {
            /* now put the version marker */
            *w->wp++ = KLV_PMD_LOCAL_TAG_VERSION;    /* this is a single byte */
            *w->wp++ = 2;  /* payload length is 2 */
            *w->wp++ = w->model->version_maj;
            *w->wp++ = w->model->version_min;
            return 0;
        }
        return 1;
    }
    return 0;
}


/**
 * @brief read bitstream version tag
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_version_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,dlb_pmd_model *model                       /**< [in] PMD model */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    if (payload_length != 2)
    {
        /* corrupt version length */
        return 1;
    }

    if (!global_testing_version_numbers)
    {
        if (model->version_avail)
        {
            if (model->version_maj != r->rp[0])
            {
                if (read_status)
                {
                    read_status->payload_status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
                }
                klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status, "Incompatible bitstream versions: "
                                    "found %d.%d, expected %d.x\n",
                                    r->rp[0], r->rp[1], model->version_maj);
                return 1;
            }
        }
        
        if (r->rp[0] != PMD_BITSTREAM_VERSION_MAJOR)
        {
            if (read_status)
            {
                read_status->payload_status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
            }
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status, "Incoming bitstream has version %u.%u,"
                                " which is incompatible with current supported version %u.xxx\n",
                                r->rp[0], r->rp[1], PMD_BITSTREAM_VERSION_MAJOR);
            return 1;
        }
    }
    model->version_avail = 1;
    model->version_maj = r->rp[0];
    model->version_min = r->rp[1];
    r->rp += 2;
    return 0;
}


#endif /* KLV_VERSION_H_ */
