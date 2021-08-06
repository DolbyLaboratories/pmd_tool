/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#ifndef DLB_ADM_ATTRIBUTE_VALUE_H
#define DLB_ADM_ATTRIBUTE_VALUE_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/get.hpp>

#include <string>
#include <ostream>

namespace DlbAdm
{

    typedef boost::variant<
        dlb_adm_bool,
        dlb_adm_uint,
        dlb_adm_int,
        dlb_adm_float,
        DLB_ADM_AUDIO_TYPE,
        dlb_adm_time,
        std::string
    > AttributeValue;

    class GetValueType : public boost::static_visitor<DLB_ADM_VALUE_TYPE>
    {
    public:
        DLB_ADM_VALUE_TYPE operator()(dlb_adm_bool v) const;
        DLB_ADM_VALUE_TYPE operator()(dlb_adm_uint v) const;
        DLB_ADM_VALUE_TYPE operator()(dlb_adm_int v) const;
        DLB_ADM_VALUE_TYPE operator()(dlb_adm_float v) const;
        DLB_ADM_VALUE_TYPE operator()(DLB_ADM_AUDIO_TYPE v) const;
        DLB_ADM_VALUE_TYPE operator()(const dlb_adm_time &v) const;
        DLB_ADM_VALUE_TYPE operator()(const std::string &v) const;
    };

    int ParseValue(dlb_adm_bool       &value, const std::string &valueString);
    int ParseValue(dlb_adm_uint       &value, const std::string &valueString);
    int ParseValue(dlb_adm_int        &value, const std::string &valueString);
    int ParseValue(dlb_adm_float      &value, const std::string &valueString);
    int ParseValue(DLB_ADM_AUDIO_TYPE &value, const std::string &valueString);
    int ParseValue(dlb_adm_time       &value, const std::string &valueString);

    std::ostream &operator<<(std::ostream &os, const AttributeValue &value);

}

#endif
