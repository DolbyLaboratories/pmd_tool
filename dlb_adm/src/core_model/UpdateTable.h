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

#ifndef DLB_ADM_UPDATE_TABLE_H
#define DLB_ADM_UPDATE_TABLE_H

#include "UpdateRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    typedef std::tuple<dlb_adm_entity_id, dlb_adm_entity_id> UpdateTuple;

    struct UpdateTable_PK {};

    typedef composite_key<
        UpdateRecord,
        const_mem_fun<UpdateRecord, dlb_adm_entity_id, &UpdateRecord::GetTargetID>,
        const_mem_fun<UpdateRecord, dlb_adm_entity_id, &UpdateRecord::GetUpdateID>
    > UpdateKey;

    typedef multi_index_container <
        UpdateRecord,
        indexed_by<
            // Primary key (PK)
            ordered_unique<
                tag<UpdateTable_PK>,
                UpdateKey,
                composite_key_compare<
                    std::less<dlb_adm_entity_id>,
                    std::less<dlb_adm_entity_id>
                >
            >
        >
    > UpdateTable;

    typedef UpdateTable::index<UpdateTable_PK>::type UpdateTable_PKIndex;

    // Functional object(s) for comparison in searching

    typedef composite_key_result<UpdateKey> UpdateKeyResult;

    struct TargetIdCompare
    {
        bool operator()(const UpdateKeyResult &lhs, dlb_adm_entity_id  rhs) const;
        bool operator()(dlb_adm_entity_id  lhs, const UpdateKeyResult &rhs) const;
    };

}

#endif  // DLB_ADM_UPDATE_TABLE_H
