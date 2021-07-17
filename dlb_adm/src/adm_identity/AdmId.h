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

#ifndef DLB_ADM_ADM_ID_H
#define DLB_ADM_ADM_ID_H

#include "dlb_adm/include/dlb_adm_entity_id.h"

namespace DlbAdm
{

    static const size_t ENTITY_TYPE_SHIFT = 7 * 8;
    static const size_t AUDIO_TYPE_SHIFT = 6 * 8;
    static const size_t X_W_SHIFT = 4 * 8;
    static const size_t FRAME_PART_SHIFT = AUDIO_TYPE_SHIFT;

    static const unsigned int FF_HEX_WIDTH = 11;

    static const dlb_adm_entity_id MASK_08 = 0x00000000000000ff;
    static const dlb_adm_entity_id MASK_16 = 0x000000000000ffff;
    static const dlb_adm_entity_id MASK_32 = 0x00000000ffffffff;
    static const dlb_adm_entity_id MASK_48 = 0x0000ffffffffffff;

}

#define DLB_ADM_ID_GET_ENTITY_TYPE(ID) ((ID) >> DlbAdm::ENTITY_TYPE_SHIFT)

#endif  // DLB_ADM_ADM_ID_H
