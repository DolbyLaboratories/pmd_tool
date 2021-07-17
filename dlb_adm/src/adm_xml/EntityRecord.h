/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_ENTITY_RECORD_H
#define DLB_ADM_ENTITY_RECORD_H

#include "TableIndex.h"
#include "dlb_adm/include/dlb_adm_api_types.h"

namespace DlbAdm
{

    struct EntityRecord
    {
        enum class STATUS
        {
            UNINITALIZED,
            FORWARD_REFERENCE,
            MUTABLE,
            IMMUTABLE,
            COMMON_DEFINITION,
            DESTROYED
        };

        dlb_adm_entity_id    id;
        TableIndex           attributesIndex;
        STATUS               status;

        EntityRecord();
        EntityRecord(const EntityRecord &x);
        ~EntityRecord();

        EntityRecord &operator=(const EntityRecord &x);
        bool operator<(const EntityRecord &x) const;
    };

}

#endif
