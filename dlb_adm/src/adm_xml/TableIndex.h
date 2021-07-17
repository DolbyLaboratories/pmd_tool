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

#ifndef DLB_ADM_TABLE_INDEX_H
#define DLB_ADM_TABLE_INDEX_H

#include <cstdint>

namespace DlbAdm
{

    typedef uint32_t TableIndex;
    static const TableIndex TableIndex_NIL = 0xffffffff;

}

#endif
