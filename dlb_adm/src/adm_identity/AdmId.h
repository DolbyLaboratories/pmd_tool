/************************************************************************
 * dlb_adm
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

#ifndef DLB_ADM_ADM_ID_H
#define DLB_ADM_ADM_ID_H

#include "dlb_adm/include/dlb_adm_entity_id.h"

namespace DlbAdm
{

    static const size_t ENTITY_TYPE_SHIFT = 7 * 8;
    static const size_t AUDIO_TYPE_SHIFT = 6 * 8;
    static const size_t X_W_SHIFT = 4 * 8;
    static const size_t FRAME_PART_SHIFT = AUDIO_TYPE_SHIFT;

    static const unsigned int FF_HEX_WIDTH = 11;

    static const dlb_adm_entity_id MASK_08 = 0x00000000000000ff;
    static const dlb_adm_entity_id MASK_16 = 0x000000000000ffff;
    static const dlb_adm_entity_id MASK_32 = 0x00000000ffffffff;
    static const dlb_adm_entity_id MASK_48 = 0x0000ffffffffffff;

}

#define DLB_ADM_ID_GET_ENTITY_TYPE(ID) ((ID) >> DlbAdm::ENTITY_TYPE_SHIFT)

#endif  // DLB_ADM_ADM_ID_H
