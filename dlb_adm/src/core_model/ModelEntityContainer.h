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
