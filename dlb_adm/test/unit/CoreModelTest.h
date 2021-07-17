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

#ifndef DLB_ADM_TEST_CORE_MODEL_TEST_H
#define DLB_ADM_TEST_CORE_MODEL_TEST_H

#include "dlb_adm/include/dlb_adm_data_types.h"
#include "core_model/dlb_adm_core_model.h"

namespace DlbAdmTest
{

    bool CheckNames(dlb_adm_core_model &model, dlb_adm_entity_id id, dlb_adm_data_names &names);

}

#endif
