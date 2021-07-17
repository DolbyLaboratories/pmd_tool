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

#include "DlbPmdModelWrapper.h"

#include <stdexcept>

namespace DlbAdm
{

    void DlbPmdModelWrapper::Init(dlb_pmd_model *pmdModel, dlb_adm_core_model *coreModel, dlb_pmd_bool use_adm_common_defs)
    {
        dlb_pmd_success success;
        size_t sz;

        if (mModelCombo == nullptr)
        {
            throw std::runtime_error("DlbPmdModelWrapper::DlbPmdModelWrapper(): modelCombo argument is null");
        }
        sz = dlb_pmd_model_combo_query_mem(pmdModel, coreModel);
        mModelMemory = new char[sz];
        if (mModelMemory == nullptr)
        {
            throw std::runtime_error("DlbPmdModelWrapper::Init(): out of memory");
        }
        success = dlb_pmd_model_combo_init(mModelCombo, pmdModel, coreModel, use_adm_common_defs, mModelMemory);
        if (success != PMD_SUCCESS)
        {
            throw std::runtime_error("DlbPmdModelWrapper::Init(): call to dlb_pmd_model_combo_init() failed");
        }
    }

    DlbPmdModelWrapper::DlbPmdModelWrapper(dlb_pmd_model_combo **modelCombo, dlb_pmd_model *pmdModel, dlb_pmd_bool use_adm_common_defs)
        : mModelCombo(modelCombo)
    {
        Init(pmdModel, nullptr, use_adm_common_defs);
    }

    DlbPmdModelWrapper::DlbPmdModelWrapper(dlb_pmd_model_combo **modelCombo, dlb_adm_core_model *coreModel, dlb_pmd_bool use_adm_common_defs)
        : mModelCombo(modelCombo)
    {
        Init(nullptr, coreModel, use_adm_common_defs);
    }

    DlbPmdModelWrapper::~DlbPmdModelWrapper()
    {
        if (mModelCombo != nullptr)
        {
            (void)dlb_pmd_model_combo_destroy(mModelCombo);
        }
        if (mModelMemory != nullptr)
        {
            delete[] mModelMemory;
        }
    }

}
