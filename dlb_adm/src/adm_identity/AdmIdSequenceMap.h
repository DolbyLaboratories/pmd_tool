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

#ifndef DLB_ADM_ADM_ID_SEQUENCE_MAP_H
#define DLB_ADM_ADM_ID_SEQUENCE_MAP_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/allocators/node_allocator.hpp>
#include <boost/interprocess/containers/map.hpp>

namespace DlbAdm
{
    using namespace boost::interprocess;

    typedef uint32_t AdmIdSequenceNumber;       // TODO: possibly this should be uint64_t
    typedef uint32_t AdmIdSubcomponentNumber;   // TODO: possibly this could be uint16_t

    typedef std::pair<const DLB_ADM_ENTITY_TYPE, AdmIdSequenceNumber> SequenceValue;
    typedef std::pair<const dlb_adm_entity_id, AdmIdSubcomponentNumber> SubcomponentValue;
    typedef node_allocator<SubcomponentValue, managed_heap_memory::segment_manager> subcomponentAllocator;
    typedef node_allocator<SequenceValue, managed_heap_memory::segment_manager> sequenceAllocator;
    typedef map<dlb_adm_entity_id, AdmIdSubcomponentNumber, std::less<dlb_adm_entity_id>, subcomponentAllocator> SubcomponentMap;
    typedef map<DLB_ADM_ENTITY_TYPE, AdmIdSequenceNumber, std::less<DLB_ADM_ENTITY_TYPE>, sequenceAllocator> SequenceMap;

    class AdmIdSequenceMap
    {
    public:
        AdmIdSequenceMap(boost::interprocess::managed_heap_memory &memory);
        AdmIdSequenceMap(const AdmIdSequenceMap &x);
        ~AdmIdSequenceMap();

        AdmIdSequenceMap &operator=(const AdmIdSequenceMap &x);

        AdmIdSequenceNumber GetSequenceNumber(DLB_ADM_ENTITY_TYPE entityType);

        AdmIdSubcomponentNumber GetSubcomponentNumber(dlb_adm_entity_id parentID);

        void Clear();

        bool IsEmpty() const;

    private:

        void Init();

        SequenceMap* mSequenceMap;
        SubcomponentMap *mSubcomponentMap;
    };

}

#endif  // DLB_ADM_ADM_ID_SEQUENCE_MAP_H
