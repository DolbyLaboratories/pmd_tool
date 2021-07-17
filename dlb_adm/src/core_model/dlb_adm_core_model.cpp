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

#include "dlb_adm_core_model.h"

using namespace DlbAdm;

dlb_adm_core_model::dlb_adm_core_model(const dlb_adm_core_model_counts *counts)
    : mCoreModel()
{
    (void)counts;
}

dlb_adm_core_model::~dlb_adm_core_model()
{
    // Empty
}

DlbAdm::CoreModel &dlb_adm_core_model::GetCoreModel()
{
    return mCoreModel;
}

const DlbAdm::CoreModel &dlb_adm_core_model::GetCoreModel() const
{
    return mCoreModel;
}
