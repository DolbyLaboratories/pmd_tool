/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * Copyright (c) 2021, Dolby International AB.
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

#ifndef DLB_ADM_SOURCE_TABLE_H
#define DLB_ADM_SOURCE_TABLE_H

#include "SourceRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    struct SourceTable_PK {};
    struct SourceTable_AudioTrack {};

    typedef multi_index_container <
        SourceRecord,
        indexed_by <
            // Primary key (PK)
            ordered_unique<tag<SourceTable_PK>, identity<SourceRecord> >,
            // AudioTrack
            ordered_unique<tag<SourceTable_AudioTrack>, member<SourceRecord, dlb_adm_entity_id, &SourceRecord::audioTrackID> >
        >,
        boost::interprocess::managed_heap_memory::allocator<SourceRecord>::type
    > SourceTable;

    typedef SourceTable::index<SourceTable_PK>::type SourceTable_PKIndex;
    typedef SourceTable::index<SourceTable_AudioTrack>::type SourceTable_AudioTrackIndex;

}

#endif  // DLB_ADM_SOURCE_TABLE_H
