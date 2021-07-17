/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef S337M_PMD_BITSTREAM_
#define S337M_PMD_BITSTREAM_

/**
 * @file pmd_bitstream.h
 * @brief definitions for generating an pmd bitstream suitable for embedding
 * within SMPTE-337m encoded PCM.
 */

#include "dlb_pmd_klv.h"
#include "pmd_smpte_337m.h"

/**
 * @brief helper function to encapsulate the process of generating a PMD bitstream
 */
int                                /** @return bytes used */
generate_pmd_bitstream
    (pmd_s337m *s337m
    ,const dlb_pmd_model *model
    ,unsigned int block
    ,unsigned int block_count
    ,dlb_klvpmd_universal_label ul
    ,uint8_t *klvbuf
    );


#endif /* S337M_PMD_BITSTREAM_ */
