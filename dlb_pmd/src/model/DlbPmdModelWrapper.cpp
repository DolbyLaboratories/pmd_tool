/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

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
