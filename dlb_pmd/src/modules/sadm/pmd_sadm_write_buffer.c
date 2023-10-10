/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
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

#include "dlb_pmd_sadm.h"
#include "dlb_pmd_sadm_buffer.h"
#include "dlb_pmd_model_combo.h"
#include "sadm_bitstream_encoder.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include <stdlib.h>

#define CHECK_STATUS(s, m)  if ((s) != PMD_SUCCESS) { goto finish; }

struct dlb_pmd_sadm_buffer_writer
{
    sadm_bitstream_encoder *enc;
};

/** ------------------------------ public API ------------------------- */
size_t
dlb_pmd_sadm_buffer_writer_query_mem()
{
    size_t size = sadm_bitstream_encoder_query_mem()
              + sizeof(dlb_pmd_sadm_buffer_writer);
    return size;
}

dlb_pmd_success
dlb_pmd_sadm_buffer_writer_init
    (dlb_pmd_sadm_buffer_writer **writer
    ,void                        *mem
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    uintptr_t mc = (uintptr_t)mem;
    dlb_pmd_sadm_buffer_writer *w = (dlb_pmd_sadm_buffer_writer*)mc;
    mc += sizeof(dlb_pmd_sadm_buffer_writer);

    success = sadm_bitstream_encoder_init((void*)mc, &(w->enc));
    *writer = w;
    return success;
}


dlb_pmd_success
dlb_pmd_sadm_buffer_write
   (dlb_pmd_sadm_buffer_writer  *writer
   ,uint8_t                     *output_buffer
   ,size_t                       outbuf_size
   ,dlb_pmd_model_combo         *model
   ,dlb_pmd_bool                 compression
   )
{
    const dlb_adm_core_model *core_model = NULL;
    dlb_pmd_success success = PMD_SUCCESS;
    int byte_written;
    
    if (writer == NULL)
    {
        CHECK_STATUS(PMD_FAIL, "Uninitialized writer pointer!\n");
    }
    if (output_buffer == NULL || outbuf_size == 0)
    {
        CHECK_STATUS(PMD_FAIL, "Invalid output buffer!\n");
    }

    success = dlb_pmd_model_combo_ensure_readable_core_model(model, &core_model);
    CHECK_STATUS(success, "Could not get a readable core model!\n");

    byte_written = sadm_bitstream_encoder_payload_ext(writer->enc, core_model, compression, output_buffer, outbuf_size);
    CHECK_STATUS((byte_written != 0), "Could not get a readable core model!\n");

finish:

    return success;
}
