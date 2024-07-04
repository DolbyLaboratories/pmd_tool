/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * Copyright (c) 2021, Dolby International AB.
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
