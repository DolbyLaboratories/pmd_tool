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

#include "CustomFormatDolbyeUtils.h"

#include "IngestAttributeHelpers.h"
#include "dlb_adm/src/adm_xml/RelationshipRecord.h"

#define CHECK_DOLBYE_MD_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)

namespace DlbAdm
{

    int GetDolbyEUintTagValue(XMLContainer &mContainer,
                              dlb_adm_entity_id id,
                              DLB_ADM_TAG value_tag,
                              dlb_adm_uint max_value,
                              dlb_adm_uint& value)
    {
        AttributeValue attrValue;
        int status = mContainer.GetValue(attrValue, id, value_tag);
        CHECK_DOLBYE_MD_STATUS(status);
        status = GetAttributeValue(value, attrValue);
        CHECK_DOLBYE_MD_STATUS(status);
        if (CheckRange(value, max_value) != DLB_ADM_STATUS_OK)
        {
            return static_cast<int>(DLB_ADM_STATUS_INVALID_DOLBYE_MD);
        }

        return status;
    }

    int GetDolbyEUintXMLFromContainer(XMLContainer &mContainer,
                                      dlb_adm_entity_id id,
                                      DLB_ADM_TAG value_tag,
                                      DLB_ADM_ENTITY_TYPE entity_type,
                                      dlb_adm_uint max_value,
                                      dlb_adm_uint& value,
                                      dlb_adm_bool is_mandatory)
    {
        int status;
        RelationshipDB::RelationshipCallbackFn handleUintValue = [&](const RelationshipRecord &rel)
        {
            dlb_adm_entity_id uintId = rel.toId;
            return GetDolbyEUintTagValue(mContainer, uintId, value_tag, max_value, value);
        };

        if (is_mandatory && (mContainer.RelationshipCount(id, entity_type) != 1))
        {
            return static_cast<int>(DLB_ADM_STATUS_INVALID_DOLBYE_MD);
        }
        value = 0;
        status = mContainer.ForEachRelationship(id, entity_type, handleUintValue);

        return status;
    }

    int GetDolbyEBoolXMLFromContainer(XMLContainer &mContainer,
                                      dlb_adm_entity_id id,
                                      DLB_ADM_TAG value_tag,
                                      DLB_ADM_ENTITY_TYPE entity_type,
                                      dlb_adm_bool& value,
                                      dlb_adm_bool is_mandatory)
    {
        dlb_adm_uint result = 0;
        int status = GetDolbyEUintXMLFromContainer(mContainer, id, value_tag, entity_type, static_cast<dlb_adm_uint>(DLB_ADM_TRUE), result, is_mandatory);
        value = static_cast<dlb_adm_bool>(result);
        return status;
    }

    unsigned int GetProgramCountFromDolbyeProgramConfig(DLB_ADM_DOLBYE_PROGRAM_CONFIG programConfig)
    {
        switch(programConfig)
        {
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_51:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_71:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_71S:
                return 1;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4_4:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4_2:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2:
                return 2;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4_2_2:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2_2:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_1_1:
                return 3;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4_2_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2_2_2:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_1_1_1_1:
                return 4;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_4_1_1_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2_2_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_1_1_1_1:
                return 5;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2_1_1_1_1:
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_1_1_1_1_1_1:
                return 6;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_1_1_1_1_1_1:
                return 7;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_1_1_1_1_1_1_1_1:
                return 8;
            case DLB_ADM_DOLBYE_PROGRAM_CONFIG_RESERVED:
            default:
                assert(0);
                return 1;
        }
    }


    unsigned int AddDolbyEUintTagWithValue(XMLContainer &mContainer,
                                           dlb_adm_entity_id parentId, 
                                           DLB_ADM_ENTITY_TYPE entity_type, 
                                           DLB_ADM_TAG value_tag,
                                           const dlb_adm_uint value)
    {
        const dlb_adm_entity_id id = mContainer.GetGenericID(entity_type);
        int status = mContainer.AddEntityWithRelationship(parentId, id);
        CHECK_DOLBYE_MD_STATUS(status);
        status = mContainer.SetValue(id, value_tag, value);
        return status;
    }

    unsigned int AddDolbyEStringTagWithValue(XMLContainer &mContainer,
                                             dlb_adm_entity_id parentId, 
                                             DLB_ADM_ENTITY_TYPE entity_type, 
                                             DLB_ADM_TAG value_tag,
                                             const std::string& value)
    {
        const dlb_adm_entity_id id = mContainer.GetGenericID(entity_type);
        int status = mContainer.AddEntityWithRelationship(parentId, id);
        CHECK_DOLBYE_MD_STATUS(status);
        status = mContainer.SetValue(id, value_tag, convertToAttributeString(value));
        return status;
    }
}