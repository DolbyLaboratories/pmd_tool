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

#ifndef DLB_ADM_CONTENT_CONTAINER_H
#define DLB_ADM_CONTENT_CONTAINER_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    // Content record

    struct ContentRecord
    {
        dlb_adm_entity_id mTargetGroupID;
        dlb_adm_entity_id mContentGroupID;
        DLB_ADM_CONTENT_KIND mContentKind;

        ContentRecord();
        ContentRecord(dlb_adm_entity_id targetGroupID, dlb_adm_entity_id contentGroupID, DLB_ADM_CONTENT_KIND contentKind);
        ContentRecord(const ContentRecord &x);
        ~ContentRecord();

        ContentRecord &operator=(const ContentRecord &x);

        bool operator<(const ContentRecord &x) const;

        bool Validate() const;

    private:
        void Clear();
    };

    // Content container

    struct ContentContainer_PK {};

    typedef multi_index_container<
        ContentRecord,
        indexed_by<
            // Primary key (PK)
            ordered_unique<tag<ContentContainer_PK>, identity<ContentRecord> >
        >
    > ContentContainer;

    typedef ContentContainer::index<ContentContainer_PK>::type ContentContainer_PKIndex;

    // Functional object(s) for comparison in searching

    struct TargetGroupIdCompare
    {
        bool operator()(const ContentRecord &lhs, dlb_adm_entity_id   rhs) const;
        bool operator()(dlb_adm_entity_id   lhs, const ContentRecord &rhs) const;
    };

    // Debugging

    void Dump(const ContentContainer &container, const char *fileName);

}

#endif  // DLB_ADM_CONTENT_CONTAINER_H
