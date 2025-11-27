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

#ifndef CUSTOM_FORMAT_DOLBY_E_UTILS_H
#define CUSTOM_FORMAT_DOLBY_E_UTILS_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "XMLContainer.h"

namespace DlbAdm
{
    typedef enum
    {
        DLB_ADM_METADATA_SEGMENT_IDS_DOLBY_E = 1,
        DLB_ADM_METADATA_SEGMENT_IDS_AC3_PROGRAMS = 3,
        DLB_ADM_METADATA_SEGMENT_IDS_ENCODE_PARAMETERS = 11,
    } DLB_ADM_METADATA_SEGMENT_IDS;

    int GetDolbyEUintTagValue(XMLContainer &mContainer,
                              dlb_adm_entity_id id,
                              DLB_ADM_TAG value_tag,
                              dlb_adm_uint max_value,
                              dlb_adm_uint& value);

    int GetDolbyEUintXMLFromContainer(XMLContainer &mContainer,
                                      dlb_adm_entity_id id,
                                      DLB_ADM_TAG value_tag,
                                      DLB_ADM_ENTITY_TYPE entity_type,                        
                                      dlb_adm_uint max_value,
                                      dlb_adm_uint& value,
                                      dlb_adm_bool is_mandatory = DLB_ADM_TRUE);

    int GetDolbyEBoolXMLFromContainer(XMLContainer &mContainer,
                                      dlb_adm_entity_id id,
                                      DLB_ADM_TAG value_tag,
                                      DLB_ADM_ENTITY_TYPE entity_type,
                                      dlb_adm_bool& value,
                                      dlb_adm_bool is_mandatory = DLB_ADM_TRUE);

    unsigned int GetProgramCountFromDolbyeProgramConfig(DLB_ADM_DOLBYE_PROGRAM_CONFIG programConfig);

    unsigned int AddDolbyEUintTagWithValue(XMLContainer &mContainer,
                                           dlb_adm_entity_id parentId, 
                                           DLB_ADM_ENTITY_TYPE entity_type, 
                                           DLB_ADM_TAG value_tag,
                                           const dlb_adm_uint value);

    unsigned int AddDolbyEStringTagWithValue(XMLContainer &mContainer,
                                             dlb_adm_entity_id parentId, 
                                             DLB_ADM_ENTITY_TYPE entity_type, 
                                             DLB_ADM_TAG value_tag,
                                             const std::string& value);

}
#endif /*CUSTOM_FORMAT_DOLBY_E_UTILS_H*/
