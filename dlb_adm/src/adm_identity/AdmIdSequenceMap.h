/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_ADM_ID_SEQUENCE_MAP_H
#define DLB_ADM_ADM_ID_SEQUENCE_MAP_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <map>

namespace DlbAdm
{

    typedef uint32_t AdmIdSequenceNumber;       // TODO: possibly this should be uint64_t
    typedef uint32_t AdmIdSubcomponentNumber;   // TODO: possibly this could be uint16_t

    class AdmIdSequenceMap
    {
    public:
        AdmIdSequenceMap();
        AdmIdSequenceMap(const AdmIdSequenceMap &x);
        ~AdmIdSequenceMap();

        AdmIdSequenceMap &operator=(const AdmIdSequenceMap &x);

        AdmIdSequenceNumber GetSequenceNumber(DLB_ADM_ENTITY_TYPE entityType);

        AdmIdSubcomponentNumber GetSubcomponentNumber(dlb_adm_entity_id parentID);

        void Clear();

        bool IsEmpty() const;

    private:
        std::map<DLB_ADM_ENTITY_TYPE, AdmIdSequenceNumber> mSequenceMap;
        std::map<dlb_adm_entity_id,   AdmIdSubcomponentNumber> mSubcomponentMap;
    };

}

#endif  // DLB_ADM_ADM_ID_SEQUENCE_MAP_H
