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

#ifndef DLB_ADM_ENTITY_RELATIONSHIP_H
#define DLB_ADM_ENTITY_RELATIONSHIP_H

namespace DlbAdm
{

    enum class ENTITY_RELATIONSHIP
    {
        NONE,
        CONTAINS,
        CONTAINED_BY,
        REFERENCES,
        REFERENCED_BY,
    };

    ENTITY_RELATIONSHIP Inverse(ENTITY_RELATIONSHIP r);

}

#endif
