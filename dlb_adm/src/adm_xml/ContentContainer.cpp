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

#include "ContentContainer.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <tuple>
#include <fstream>

namespace DlbAdm
{
    using namespace boost::multi_index;

    ContentRecord::ContentRecord()
    {
        Clear();
    }

    ContentRecord::ContentRecord(dlb_adm_entity_id targetGroupID, dlb_adm_entity_id contentGroupID, DLB_ADM_CONTENT_KIND contentKind)
        : mTargetGroupID(targetGroupID)
        , mContentGroupID(contentGroupID)
        , mContentKind(contentKind)
    {
        // Empty
    }

    ContentRecord::ContentRecord(const ContentRecord &x)
        : mTargetGroupID(x.mTargetGroupID)
        , mContentGroupID(x.mContentGroupID)
        , mContentKind(x.mContentKind)
    {
        // Empty
    }

    ContentRecord::~ContentRecord()
    {
        Clear();
    }

    ContentRecord &ContentRecord::operator=(const ContentRecord &x)
    {
        mTargetGroupID = x.mTargetGroupID;
        mContentGroupID = x.mContentGroupID;
        mContentKind = x.mContentKind;
        return *this;
    }

    bool ContentRecord::operator<(const ContentRecord &x) const
    {
        return std::tie(mTargetGroupID, mContentGroupID) < std::tie(x.mTargetGroupID, x.mContentGroupID);
    }

    bool ContentRecord::Validate() const
    {
        AdmIdTranslator xlator;

        return
            (xlator.GetEntityType(mTargetGroupID)  == DLB_ADM_ENTITY_TYPE_PACK_FORMAT) &&
            (xlator.GetEntityType(mContentGroupID) == DLB_ADM_ENTITY_TYPE_CONTENT);
    }

    void ContentRecord::Clear()
    {
        mTargetGroupID = DLB_ADM_NULL_ENTITY_ID;
        mContentGroupID = DLB_ADM_NULL_ENTITY_ID;
        mContentKind = DLB_ADM_CONTENT_KIND_UNKNOWN;
    }

    bool TargetGroupIdCompare::operator()(const ContentRecord &lhs, dlb_adm_entity_id rhs) const
    {
        return lhs.mTargetGroupID < rhs;
    }

    bool TargetGroupIdCompare::operator()(dlb_adm_entity_id lhs, const ContentRecord &rhs) const
    {
        return lhs < rhs.mTargetGroupID;
    }

    void Dump(const ContentContainer &container, const char *fileName)
    {
        std::ofstream f(fileName);

        if (f.good())
        {
            const ContentContainer_PKIndex &index = container.get<ContentContainer_PK>();
            auto it = index.begin();
            std::string targetGroupIDString;
            std::string contentGroupIDString;
            AdmIdTranslator xlator;

            while (it != index.end())
            {
                targetGroupIDString = xlator.Translate(it->mTargetGroupID);
                contentGroupIDString = xlator.Translate(it->mContentGroupID);
                f << targetGroupIDString << ',' << contentGroupIDString << std::endl;
                ++it;
            }
        }
    }

}
