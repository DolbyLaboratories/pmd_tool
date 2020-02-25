/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @file dlb_pmd_klv.h
 * @brief KLV reading/writing
 */

#ifndef DLB_PMD_KLV_H
#define DLB_PMD_KLV_H

#include <stdint.h>
#include "dlb_pmd_types.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief max buffer size for single KLV payload
 *
 * The realtime requirements for PMD-KLV is that it must be delivered
 * SMPTE-337m-wrapped on a stereo pair in 160-sample chunks.
 *
 * This amounts to (160 - (4 sync words)) * 20 bits, using a 20-bit
 * SMPTE encoding.
 */
#define DLB_PMD_MAX_PCMKLV_SIZE (156 * 20 / 8)


/**
 * @def DLB_PMD_NO_ED2_STREAM_INDEX (0xff)
 * @brief magic value returned by #dlb_klvpmd_read_payload to indicate
 * this is not an ED2 stream
 */
#define DLB_PMD_NO_ED2_STREAM_INDEX (0xff)


/**
 * @brief enumerate possible SMPTE 2109 Universal Labels
 */
typedef enum
{
    DLB_PMD_KLV_UL_ST2109,  /**< standard SMPTE 2109 Universal Label */
    DLB_PMD_KLV_UL_DOLBY    /**< Dolby-private Universal Label */
}
dlb_klvpmd_universal_label;


/**
 * @brief return minimum size of a block
 *
 * This is used to determine if the block carries no actual metadata
 */
unsigned int
dlb_klvpmd_min_block_size
    (void
    );


/**
 * @brief write MTx(0), 1st 160-sample payload 
 *
 * REQ-0009 MTx(0) shall only contain AudioObjects V1.0 payloads
 *          of type "Object"
 */
int                               /** @return number bytes written, 0 on failure */
dlb_klvpmd_write_block
   (dlb_pmd_model *model          /**< [in] model to write */
   ,uint8_t        sindex         /**< [in] ED2 stream index, or 0xff if not relevant */
   ,unsigned int   block_number   /**< [in] number of block since frame start */
   ,uint8_t       *buffer         /**< [in] pointer to buffer to write */
   ,size_t         capacity       /**< [in] capacity of buffer to write, must be
                                   *        at least #DLB_PMD_MAX_PCMKLV_SIZE */
   ,dlb_klvpmd_universal_label ul /**< [in] universal label */
   );


/**
 * @brief write a complete ED2-frame of metadata
 */
int                               /** @return number bytes written, 0 on failure */
dlb_klvpmd_write_all
   (dlb_pmd_model *model          /**< [in] model to write */
   ,uint8_t        sindex         /**< [in] ED2 stream index, or 0xff if not relevant */
   ,unsigned char *buffer         /**< [in] pointer to buffer to write */
   ,size_t         capacity       /**< [in] capacity of buffer */
   ,dlb_klvpmd_universal_label ul /**< [in] universal label */
   );


/**
 * @brief read a PMD SMPTE336 KLV payloads from given input buffer
 *
 * Note that the input buffer corresponds to one chunk of data
 * that would be stored in a 160-sample block in one pair of
 * SMPTE-337m data.
 */
int                                /** @return 0 on success, non-zero otherwise */
dlb_klvpmd_read_payload
    (uint8_t                    *buffer         /**< [in] input buffer */
    ,size_t                      length         /**< [in] actual number of bytes in buffer */
    ,dlb_pmd_model              *model          /**< [in] PMD model structure to populate */
    ,int                         new_frame      /**< [in] is this the first payload of a new video frame? */ 
    ,uint8_t                    *sindex         /**< [in/out] ED2 stream index if known or
                                                 *        #DLB_PMD_NO_ED2_STREAM_INDEX, may be null */
    ,dlb_pmd_payload_set_status *read_status    /**< [out] Payload read status, may be null */
    );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_kLV_H */
