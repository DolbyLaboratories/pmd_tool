/************************************************************************
 * dlb_adm
 * Copyright (c) 2025, Dolby Laboratories Inc.
 * Copyright (c) 2025, Dolby International AB.
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

#include "gtest/gtest.h"

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm/include/dlb_adm_api_types.h"

class DlbXMLToAPICommon : public testing::Test
{
    protected:

        dlb_adm_container_counts     containerCounts;
        dlb_adm_core_model_counts    coreModelCounts;
        dlb_adm_xml_container       *originalContainer; 
        dlb_adm_core_model          *coreModel;

        virtual void SetUp()
        {
            int status;
            ::memset(&containerCounts, 0, sizeof(containerCounts));
            ::memset(&coreModelCounts, 0, sizeof(coreModelCounts));
            originalContainer = nullptr;  
            coreModel = nullptr;

            status = ::dlb_adm_container_open(&originalContainer, &containerCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);      
            status = ::dlb_adm_core_model_open(&coreModel, &coreModelCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        }

        virtual void TearDown()
        {
            if (originalContainer != nullptr)
            {
                if (::dlb_adm_container_close(&originalContainer))
                {
                    originalContainer = nullptr;
                }
            }       

            if (coreModel != nullptr)
            {
                if (::dlb_adm_core_model_close(&coreModel))
                {
                    coreModel = nullptr;
                }
            }
        }
};

class DlbXMLToXMLCommon : public testing::Test
{
    protected:

        dlb_adm_container_counts     containerCounts;
        dlb_adm_core_model_counts    coreModelCounts;
        dlb_adm_xml_container       *originalContainer;
        dlb_adm_core_model          *coreModel;

        virtual void SetUp()
        {
            int status;
            ::memset(&containerCounts, 0, sizeof(containerCounts));
            ::memset(&coreModelCounts, 0, sizeof(coreModelCounts));
            originalContainer = nullptr;
            coreModel = nullptr;

            status = ::dlb_adm_container_open(&originalContainer, &containerCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);          
            status = ::dlb_adm_core_model_open(&coreModel, &coreModelCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        }

        virtual void TearDown()
        {
            if (originalContainer != nullptr)
            {
                if (::dlb_adm_container_close(&originalContainer))
                {
                    originalContainer = nullptr;
                }
            }

            if (coreModel != nullptr)
            {
                if (::dlb_adm_core_model_close(&coreModel))
                {
                    coreModel = nullptr;
                }
            }
        }
};