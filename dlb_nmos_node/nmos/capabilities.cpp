/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed regex pattern constraint support; simplified constraint
 * matching logic.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (c) 2019-2025, Dolby Laboratories Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "nmos/capabilities.h"

#include <boost/range/adaptor/transformed.hpp>
#include "nmos/json_fields.h"

namespace nmos
{
    web::json::value make_caps_string_constraint(const std::vector<utility::string_t>& enum_values)
    {
        using web::json::value_of;
        using web::json::value_from_elements;
        return value_of({
            { !enum_values.empty() ? nmos::fields::constraint_enum.key : U(""), value_from_elements(enum_values) }
        });
    }

    web::json::value make_caps_integer_constraint(const std::vector<int64_t>& enum_values, int64_t minimum, int64_t maximum)
    {
        using web::json::value_of;
        using web::json::value_from_elements;
        return value_of({
            { !enum_values.empty() ? nmos::fields::constraint_enum.key : U(""), value_from_elements(enum_values) },
            { minimum != no_minimum<int64_t>() ? nmos::fields::constraint_minimum.key : U(""), minimum },
            { maximum != no_maximum<int64_t>() ? nmos::fields::constraint_maximum.key : U(""), maximum }
        });
    }

    web::json::value make_caps_number_constraint(const std::vector<double>& enum_values, double minimum, double maximum)
    {
        using web::json::value_of;
        using web::json::value_from_elements;
        return value_of({
            { !enum_values.empty() ? nmos::fields::constraint_enum.key : U(""), value_from_elements(enum_values) },
            { minimum != no_minimum<double>() ? nmos::fields::constraint_minimum.key : U(""), minimum },
            { maximum != no_maximum<double>() ? nmos::fields::constraint_maximum.key : U(""), maximum }
        });
    }

    web::json::value make_caps_boolean_constraint(const std::vector<bool>& enum_values)
    {
        using web::json::value_of;
        using web::json::value_from_elements;
        return value_of({
            { !enum_values.empty() ? nmos::fields::constraint_enum.key : U(""), value_from_elements(enum_values) }
        });
    }

    web::json::value make_caps_rational_constraint(const std::vector<nmos::rational>& enum_values, const nmos::rational& minimum, const nmos::rational& maximum)
    {
        using web::json::value_of;
        using web::json::value_from_elements;
        return value_of({
            { !enum_values.empty() ? nmos::fields::constraint_enum.key : U(""), value_from_elements(enum_values | boost::adaptors::transformed([](const nmos::rational& r) { return make_rational(r); })) },
            { minimum != no_minimum<nmos::rational>() ? nmos::fields::constraint_minimum.key : U(""), make_rational(minimum) },
            { maximum != no_maximum<nmos::rational>() ? nmos::fields::constraint_maximum.key : U(""), make_rational(maximum) }
        });
    }

    namespace details
    {
        // cf. nmos::details::make_constraints_schema in nmos/connection_api.cpp 
        template <typename T, typename Parse>
        bool match_constraint(const T& value, const web::json::value& constraint, Parse parse)
        {
            if (constraint.has_field(nmos::fields::constraint_enum))
            {
                const auto& enum_values = nmos::fields::constraint_enum(constraint).as_array();
                if (enum_values.end() == std::find_if(enum_values.begin(), enum_values.end(), [&parse, &value](const web::json::value& enum_value)
                {
                    return parse(enum_value) == value;
                }))
                {
                    return false;
                }
            }
            if (constraint.has_field(nmos::fields::constraint_minimum))
            {
                const auto& minimum = nmos::fields::constraint_minimum(constraint);
                if (parse(minimum) > value)
                {
                    return false;
                }
            }
            if (constraint.has_field(nmos::fields::constraint_maximum))
            {
                const auto& maximum = nmos::fields::constraint_maximum(constraint);
                if (parse(maximum) < value)
                {
                    return false;
                }
            }
            return true;
        }
    }

    bool match_string_constraint(const utility::string_t& value, const web::json::value& constraint)
    {
        return details::match_constraint(value, constraint, [](const web::json::value& enum_value)
        {
            return enum_value.as_string();
        });
    }

    bool match_integer_constraint(int64_t value, const web::json::value& constraint)
    {
        return details::match_constraint(value, constraint, [&value](const web::json::value& enum_value)
        {
            return enum_value.as_integer();
        });
    }

    bool match_number_constraint(double value, const web::json::value& constraint)
    {
        return details::match_constraint(value, constraint, [&value](const web::json::value& enum_value)
        {
            return enum_value.as_double();
        });
    }

    bool match_boolean_constraint(bool value, const web::json::value& constraint)
    {
        return details::match_constraint(value, constraint, [&value](const web::json::value& enum_value)
        {
            return enum_value.as_bool();
        });
    }

    bool match_rational_constraint(const nmos::rational& value, const web::json::value& constraint)
    {
        return details::match_constraint(value, constraint, [&value](const web::json::value& enum_value)
        {
            return nmos::parse_rational(enum_value);
        });
    }
}
