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
