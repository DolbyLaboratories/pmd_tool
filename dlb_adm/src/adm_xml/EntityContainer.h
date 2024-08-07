/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2021, Dolby Laboratories Inc.
 * Copyright (c) 2020-2021, Dolby International AB.
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

#ifndef DLB_ADM_ENTITIES_H
#define DLB_ADM_ENTITIES_H

#include "EntityRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    // Entity container

    struct EntityContainer_PK {};

    typedef multi_index_container<
        EntityRecord,
        indexed_by<
            // Primary key (PK)
            ordered_unique<tag<EntityContainer_PK>, identity<EntityRecord> >
        >
        ,boost::interprocess::managed_heap_memory::allocator<EntityRecord>::type
    > EntityContainer;

    typedef EntityContainer::index<EntityContainer_PK>::type EntityContainer_PKIndex;

    // Functional object(s) for comparison in searching

    struct EntityIdCompare
    {
        bool operator()(const EntityRecord &lhs, dlb_adm_entity_id   rhs) const;
        bool operator()(dlb_adm_entity_id   lhs, const EntityRecord &rhs) const;
    };

    struct EntityTypeCompare
    {
        bool operator()(const EntityRecord  &lhs, DLB_ADM_ENTITY_TYPE  rhs) const;
        bool operator()(DLB_ADM_ENTITY_TYPE  lhs, const EntityRecord  &rhs) const;
    };

}

#endif
