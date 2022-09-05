/************************************************************************
 * dlb_adm
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

#ifndef DLB_ADM_ELEMENT_RECORD_H
#define DLB_ADM_ELEMENT_RECORD_H

#include "dlb_adm/include/dlb_adm_entity_id.h"

namespace DlbAdm
{

    struct ElementRecord
    {
        dlb_adm_entity_id   audioElementID;
        dlb_adm_entity_id   targetGroupID;
        dlb_adm_entity_id   targetID;
        dlb_adm_entity_id   audioTrackID;

        ElementRecord();
        ElementRecord
        (
            dlb_adm_entity_id elementID,
            dlb_adm_entity_id groupID,
            dlb_adm_entity_id tgtID,
            dlb_adm_entity_id trackID
        );
        ElementRecord(const ElementRecord &x);
        ~ElementRecord();

        ElementRecord &operator=(const ElementRecord &x);

        bool operator<(const ElementRecord &x) const;

        ElementRecord &Clear();

        bool IsNull() const;
        bool Validate(bool nullOK = false) const;
    };

}

#endif  // DLB_ADM_ELEMENT_RECORD_H
