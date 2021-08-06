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
