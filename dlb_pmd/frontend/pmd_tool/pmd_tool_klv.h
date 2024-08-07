/************************************************************************
 * dlb_pmd
 * Copyright (c) 2016-2021, Dolby Laboratories Inc.
 * Copyright (c) 2016-2021, Dolby International AB.
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
 * @file pmd_tool_klv.h
 * @brief KLV reader/writer functionality for pmd tool
 */

#include "dlb_pmd_klv.h"

/**
 * @brief parse a KLV file to ingest a PMD model
 */
int                                 /** @return 0 on success, 1 on failure */
klv_read
    (const char                 *filename   /**< [in]  name of file to ingest */
    ,dlb_pmd_model_combo        *model      /**< [out] destination struct for model */
    );


/**
 * @brief write an KLV file to given filename
 *
 * @todo - complete: only dumps audio objects for now
 */
int                                 /** @return 0 on success, 1 on failure */
klv_write
    (const char                 *filename   /**< [in] name of file to write */
    ,dlb_pmd_model_combo        *model      /**< [in] PMD model to write */
    ,dlb_klvpmd_universal_label  ul         /**< [in] SMPTE2109 Universal Label */
    );
