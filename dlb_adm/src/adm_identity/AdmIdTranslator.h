/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2021, Dolby Laboratories Inc.
 * Copyright (c) 2020-2021, Dolby International AB.
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

#ifndef DLB_ADM_ADM_ID_TRANSLATOR_H
#define DLB_ADM_ADM_ID_TRANSLATOR_H

#include <string>

#include "dlb_adm/include/dlb_adm_api_types.h"

namespace DlbAdm
{

    static const size_t ADM_ID_MIN_LEN = 7;     // AO_1001, TP_0001
    static const size_t ADM_ID_MAX_LEN = 20;    // AB_00011001_00000001

    class AdmIdTranslator
    {
    public:
        AdmIdTranslator() {}
        AdmIdTranslator(const AdmIdTranslator &) {}
        ~AdmIdTranslator() {}

        AdmIdTranslator &operator=(const AdmIdTranslator &) { return *this; }

        DLB_ADM_ENTITY_TYPE GetEntityType(dlb_adm_entity_id id) const;

        dlb_adm_entity_id Translate(const char *id) const;
        dlb_adm_entity_id Translate(const std::string &id) const;

        std::string Translate(dlb_adm_entity_id id) const;

        bool IsGenericEntityType(DLB_ADM_ENTITY_TYPE entityType) const;

        bool SubcomponentIdReferencesComponent(const dlb_adm_entity_id parentId, const dlb_adm_entity_id subcomponentId) const;

        dlb_adm_entity_id ConstructGenericId(DLB_ADM_ENTITY_TYPE entityType, uint32_t sequenceNumber) const;

        dlb_adm_entity_id ConstructUntypedId(DLB_ADM_ENTITY_TYPE entityType, uint32_t sequenceNumber, uint32_t subSequenceNumber = 0) const;

        dlb_adm_entity_id ConstructTypedId(DLB_ADM_ENTITY_TYPE entityType, DLB_ADM_AUDIO_TYPE audioType, uint32_t sequenceNumber, uint32_t subSequenceNumber = 0) const;

        dlb_adm_entity_id ConstructSubcomponentId(dlb_adm_entity_id parentId, uint32_t sequenceNumber) const;

        void DeconstructUntypedId(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE *entityType, uint32_t *sequenceNumber, uint32_t *subSequenceNumber) const;

    private:
        dlb_adm_entity_id ConstructId(DLB_ADM_ENTITY_TYPE    entityType,
                                      DLB_ADM_AUDIO_TYPE     audioType,
                                      uint16_t               xw,
                                      uint32_t               z,
                                      uint64_t               ff,
                                      uint8_t                pp) const;
    };

}

#endif /* DLB_ADM_ADM_ID_TRANSLATOR_H */
