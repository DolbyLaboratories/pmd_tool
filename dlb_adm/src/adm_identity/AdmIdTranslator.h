/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
