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

#include "AdmIdSequenceMap.h"

namespace DlbAdm
{

    AdmIdSequenceMap::AdmIdSequenceMap()
        : mSequenceMap()
        , mSubcomponentMap()
    {
        // Empty
    }

    AdmIdSequenceMap::AdmIdSequenceMap(const AdmIdSequenceMap &x)
        : mSequenceMap(x.mSequenceMap)
        , mSubcomponentMap(x.mSubcomponentMap)
    {
        // Empty
    }

    AdmIdSequenceMap::~AdmIdSequenceMap()
    {
        // Empty
    }

    AdmIdSequenceMap & AdmIdSequenceMap::operator=(const AdmIdSequenceMap &x)
    {
        mSequenceMap = x.mSequenceMap;
        mSubcomponentMap = x.mSubcomponentMap;
        return *this;
    }

    template <typename M, typename K>
    uint32_t Next(M &theMap, K theKey)
    {
        auto it = theMap.find(theKey);
        uint32_t n;

        if (it == theMap.end())
        {
            n = 1;
            theMap[theKey] = 2;
        }
        else
        {
            n = it->second++;
        }

        return n;
    }

    AdmIdSequenceNumber AdmIdSequenceMap::GetSequenceNumber(DLB_ADM_ENTITY_TYPE entityType)
    {
        return Next(mSequenceMap, entityType);
    }

    AdmIdSubcomponentNumber AdmIdSequenceMap::GetSubcomponentNumber(dlb_adm_entity_id parentID)
    {
        return Next(mSubcomponentMap, parentID);
    }

    void AdmIdSequenceMap::Clear()
    {
        mSubcomponentMap.clear();
        mSequenceMap.clear();
    }

    bool AdmIdSequenceMap::IsEmpty() const
    {
        return mSequenceMap.empty() && mSubcomponentMap.empty();
    }

}
