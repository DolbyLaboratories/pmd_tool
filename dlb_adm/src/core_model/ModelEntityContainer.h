/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef DLB_ADM_MODEL_ENTITY_CONTAINER_H
#define DLB_ADM_MODEL_ENTITY_CONTAINER_H

#include "ModelEntity.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    // Model entity record

    class ModelEntityRecord
    {
    public:
        explicit ModelEntityRecord(ConstModelEntityPtr p);

        const ModelEntity *GetPointer() const;
        const ModelEntity &GetReference() const;

    private:
        ConstModelEntityPtr modelEntityPtr;
    };

    // Model entity container

    struct ModelEntityContainer_PK {};

    typedef multi_index_container<
        ModelEntityRecord,
        indexed_by<
            // Primary key (PK)
            ordered_unique<tag<ModelEntityContainer_PK>, const_mem_fun<ModelEntityRecord, const ModelEntity &, &ModelEntityRecord::GetReference> >
        >
    > ModelEntityContainer;

    typedef ModelEntityContainer::index<ModelEntityContainer_PK>::type ModelEntityContainer_PKIndex;

    // Functional object(s) for comparison in searching

    struct ModelEntityIdCompare
    {
        bool operator()(const ModelEntity &lhs, dlb_adm_entity_id  rhs) const;
        bool operator()(dlb_adm_entity_id  lhs, const ModelEntity &rhs) const;
    };

    struct ModelEntityTypeCompare
    {
        bool operator()(const ModelEntity   &lhs, DLB_ADM_ENTITY_TYPE  rhs) const;
        bool operator()(DLB_ADM_ENTITY_TYPE  lhs, const ModelEntity   &rhs) const;
    };

}

#endif  // DLB_ADM_MODEL_ENTITY_CONTAINER_H
