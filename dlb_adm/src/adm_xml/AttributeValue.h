/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
