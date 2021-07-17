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

#ifndef DLB_ADM_ELEMENT_TABLE_H
#define DLB_ADM_ELEMENT_TABLE_H

#include "ElementRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    // Element table

    struct ElementTable_PK {};

    typedef multi_index_container <
        ElementRecord,
        indexed_by<
            // Primary key (PK)
            ordered_unique<tag<ElementTable_PK>, identity<ElementRecord> >
        >
    > ElementTable;

    typedef ElementTable::index<ElementTable_PK>::type ElementTable_PKIndex;

    // Functional object(s) for comparison in searching

    struct AudioElementIdCompare
    {
        bool operator()(const ElementRecord &lhs, dlb_adm_entity_id  rhs) const;
        bool operator()(dlb_adm_entity_id  lhs, const ElementRecord &rhs) const;
    };

}

#endif  // DLB_ADM_ELEMENT_TABLE_H
