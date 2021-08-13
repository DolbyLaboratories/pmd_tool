/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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