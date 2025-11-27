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

#include "DolbyeInfo.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/range/algorithm/count.hpp>
#include <boost/algorithm/string.hpp>

namespace DlbAdm
{
    DolbyeInfo::DolbyeInfo()
        : ModelEntity()
    {

    }

    DolbyeInfo::DolbyeInfo(dlb_adm_entity_id id, DLB_ADM_DOLBYE_FRAME_RATE frameRate, DLB_ADM_DOLBYE_PROGRAM_CONFIG programConfig, dlb_adm_element_count programCount)
        : ModelEntity(id)
        , mFrameRate(frameRate)
        , mProgramConfig(programConfig)
        , mProgramCount(programCount)
    {

    }

    void DolbyeInfo::SetSmpteTimeCode(uint32_t tc1, uint32_t tc2, uint32_t tc3, uint32_t tc4)
    {
        mTimecode.tc1 = tc1;
        mTimecode.tc2 = tc2;
        mTimecode.tc3 = tc3;
        mTimecode.tc4 = tc4;
    }

    int DolbyeInfo::SetSmpteTimeCode(std::string timeCode)
    {
        int delimeterNum = boost::range::count(timeCode, ':');
        if (delimeterNum != 3)
        {
            return DLB_ADM_STATUS_ERROR;
        }
        
        size_t pos = 0;
        size_t read;
        unsigned int current = timeCode.find(":", pos);
        std::string substr = timeCode.substr(pos, current - pos);
        mTimecode.tc1 = std::stoi(substr, &read);
        if (read != substr.length())
        {
            return DLB_ADM_STATUS_ERROR;
        }
        pos = current + 1;
        current = timeCode.find(":", pos);
        substr = timeCode.substr(pos, current - pos);
        mTimecode.tc2 = std::stoi(substr, &read);
        if (read != substr.length())
        {
            return DLB_ADM_STATUS_ERROR;
        }
        pos = current + 1;
        current = timeCode.find(":", pos);
        substr = timeCode.substr(pos, current - pos);
        mTimecode.tc3 = std::stoi(substr, &read);
        if (read != substr.length())
        {
            return DLB_ADM_STATUS_ERROR;
        }
        pos = current + 1;
        substr = timeCode.substr(pos);          
        mTimecode.tc4 = std::stoi(substr, &read);
        if (read != substr.length())
        {
            return DLB_ADM_STATUS_ERROR;
        }

        return DLB_ADM_STATUS_OK;
    }

    std::string DolbyeInfo::GetSmpteTimeCodeStr() const
    {
        char smpteTimeCode[12];
        std::sprintf(smpteTimeCode, "%02X:%02X:%02X:%02X", mTimecode.tc1, mTimecode.tc2, mTimecode.tc3, mTimecode.tc4);

        return std::string(smpteTimeCode);
    }

}