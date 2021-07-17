/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_PMD_BITSET_H
#define DLB_PMD_BITSET_H

/**
 * @def BYTE_BITS
 * @brief number of bits in a byte
 */
#define BYTE_BITS (8)

static
void
set_element_bit
    (uint8_t            *element_set
    ,dlb_pmd_element_id  element_id
    )
{
    while (element_id >= BYTE_BITS)
    {
        element_set++;
        element_id -= BYTE_BITS;
    }
    *element_set |= (uint8_t)(1 << element_id);
}

static
dlb_pmd_bool
get_element_bit
    (uint8_t            *element_set
    ,dlb_pmd_element_id  element_id
    )
{
    while (element_id >= BYTE_BITS)
    {
        element_set++;
        element_id -= BYTE_BITS;
    }
    return (dlb_pmd_bool)((*element_set & (uint8_t)(1 << element_id)) ? PMD_TRUE : PMD_FALSE);
}

#endif  // DLB_PMD_BITSET_H
