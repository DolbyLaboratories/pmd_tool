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

#ifndef DLB_PMD_MODEL_WRAPPER_H
#define DLB_PMD_MODEL_WRAPPER_H

#include "dlb_pmd_model_combo.h"

namespace DlbAdm
{

    /**
     * @brief A class to manage wrapping a PMD or core model in a combo model in C++ client code
     * (obviously it is not useful for C client code).  Automatically cleans up resources in the
     * destructor.
     */
    class DLB_PMD_DLL_ENTRY DlbPmdModelWrapper
    {
    public:
        DlbPmdModelWrapper(dlb_pmd_model_combo **modelCombo, dlb_pmd_model      *pmdModel,  dlb_pmd_bool use_adm_common_defs);
        DlbPmdModelWrapper(dlb_pmd_model_combo **modelCombo, dlb_adm_core_model *coreModel, dlb_pmd_bool use_adm_common_defs);
        ~DlbPmdModelWrapper();

    private:
        void Init(dlb_pmd_model *pmdModel, dlb_adm_core_model *coreModel, dlb_pmd_bool use_adm_common_defs);

        dlb_pmd_model_combo    **mModelCombo;
        char                    *mModelMemory;
    };

}

#endif  // DLB_PMD_MODEL_WRAPPER_H
