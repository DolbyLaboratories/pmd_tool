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

#include "dlb_pmd_sadm.h"
#include "dlb_pmd_sadm_buffer.h"
#include "dlb_pmd_model_combo.h"
#include "sadm_bitstream_decoder.h"
#include <stdlib.h>

#define CHECK_STATUS(s, m)  if ((s) != PMD_SUCCESS) { error_msg = (m); goto finish; }

static
void
sadm_buf_dec_callback
    (void *user_data
    ,sadm_dec_state state
    )
{
    *(sadm_dec_state*)user_data = state;
}

struct dlb_pmd_sadm_buffer_reader
{
    sadm_bitstream_decoder *dec;
};

/** ------------------------------ public API ------------------------- */

size_t
dlb_pmd_sadm_buffer_reader_query_mem()
{
    size_t size = sadm_bitstream_decoder_query_mem()
              + sizeof(dlb_pmd_sadm_buffer_reader);
    return size;
}

dlb_pmd_success
dlb_pmd_sadm_buffer_reader_init
    (dlb_pmd_sadm_buffer_reader **reader
    ,void                        *mem
    )
{
    dlb_pmd_success success = PMD_SUCCESS;
    uintptr_t mc = (uintptr_t)mem;
    dlb_pmd_sadm_buffer_reader *r = (dlb_pmd_sadm_buffer_reader*)mc;
    mc += sizeof(dlb_pmd_sadm_buffer_reader);

    success = sadm_bitstream_decoder_init((void*)mc, &(r->dec));
    *reader = r;
    return success;
}

dlb_pmd_success
dlb_pmd_sadm_buffer_read
   (dlb_pmd_sadm_buffer_reader  *reader
   ,const uint8_t               *input_buffer
   ,size_t                       buffer_size
   ,dlb_pmd_model_combo         *model
   ,dlb_pmd_bool                 use_common_defs
   ,dlb_pmd_sadm_error_callback  error_callback
   ,void                        *arg
   )
{
    dlb_adm_core_model *core_model = NULL;
    dlb_pmd_success success = PMD_SUCCESS;
    const char *error_msg = NULL;
    sadm_dec_state state;

    if (reader == NULL)
    {
        CHECK_STATUS(PMD_FAIL, "Uninitialized reader pointer!\n");
    }
    if (input_buffer == NULL || buffer_size == 0)
    {
        CHECK_STATUS(PMD_FAIL, "Invalid input buffer!\n");
    }

    success = dlb_pmd_model_combo_get_writable_core_model(model, &core_model);
    CHECK_STATUS(success, "Could not get a writable core model!\n");
    success = sadm_bitstream_decoder_decode(reader->dec, input_buffer, buffer_size, core_model, use_common_defs, sadm_buf_dec_callback, &state);
    if (success != PMD_SUCCESS)
    {
        if (state == SADM_DECOMPRESS_ERR)
        {
            error_msg = "Could not decompress SADM buffer!\n";
        }
        if (state == SADM_PARSE_ERR)
        {
            error_msg = "Could not parse SADM buffer!\n";
        }
    }

finish:
    if (success != PMD_SUCCESS && error_callback)
    {
        (*error_callback)(error_msg, arg);
    }
    return success;
}
