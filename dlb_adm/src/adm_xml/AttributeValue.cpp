/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2024, Dolby Laboratories Inc.
 * Copyright (c) 2020-2024, Dolby International AB.
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

#include "AttributeValue.h"

#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <stdio.h>

#include <string>

static constexpr uint8_t MINUTES_PER_HOUR = 60;
static constexpr uint8_t SECONDS_PER_MINUTES = 60;
static constexpr uint8_t HMS_SIZE = 2; /* Hours minutes and seconds have exactly 2 digit size */
static constexpr uint8_t ZF_MIN_SIZE = 5;

namespace DlbAdm
{

    // Get the type of a value

    DLB_ADM_VALUE_TYPE GetValueType::operator()(dlb_adm_bool) const
    {
        return DLB_ADM_VALUE_TYPE_BOOL;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(dlb_adm_uint) const
    {
        return DLB_ADM_VALUE_TYPE_UINT;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(dlb_adm_int) const
    {
        return DLB_ADM_VALUE_TYPE_INT;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(dlb_adm_float) const
    {
        return DLB_ADM_VALUE_TYPE_FLOAT;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(DLB_ADM_AUDIO_TYPE) const
    {
        return DLB_ADM_VALUE_TYPE_AUDIO_TYPE;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(const dlb_adm_time &) const
    {
        return DLB_ADM_VALUE_TYPE_TIME;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(const attributeString &) const
    {
        return DLB_ADM_VALUE_TYPE_STRING;
    }

    DLB_ADM_VALUE_TYPE GetValueType::operator()(dlb_adm_entity_id) const
    {
        return DLB_ADM_VALUE_TYPE_REF;
    }


    // Parse a value from a string

    template <typename T>
    int ParseScalarValue(T &value, const std::string &valueString)
    {
        int status = DLB_ADM_STATUS_OK;

        try
        {
            value = boost::lexical_cast<T>(valueString);
        }
        catch (const boost::bad_lexical_cast &)
        {
            status = DLB_ADM_STATUS_ERROR;
        }

        return status;
    }

    int ParseValue(dlb_adm_bool &value, const std::string &valueString)
    {
        int status = DLB_ADM_STATUS_OK;
        std::string lvs(valueString);

        boost::algorithm::to_lower(lvs);
        if (lvs == "true" || valueString == "1")
        {
            value = DLB_ADM_TRUE;
        }
        else if (lvs == "false" || valueString == "0")
        {
            value = DLB_ADM_FALSE;
        }
        else
        {
            status = DLB_ADM_STATUS_ERROR;
        }

        return status;
    }

    int ParseValue(dlb_adm_uint &value, const std::string &valueString)
    {
        return ParseScalarValue(value, valueString);
    }

    int ParseValue(attributeString &value, const std::string &valueString)
    {
        std::size_t i;
        for (i = 0; i < valueString.length() && i < value.size() - 1; ++i)
        {
            value[i] = valueString[i];
        }

        value[i] = '\0';
        return DLB_ADM_STATUS_OK;
    }    

    int ParseValue(dlb_adm_int &value, const std::string &valueString)
    {
        return ParseScalarValue(value, valueString);
    }

    int ParseValue(dlb_adm_float &value, const std::string& valueString)
    {
        return ParseScalarValue(value, valueString);
    }

    int ParseValue(DLB_ADM_AUDIO_TYPE &value, const std::string &valueString)
    {
        uint32_t v;
        int status = ParseScalarValue(v, valueString);

        if (status == DLB_ADM_STATUS_OK)
        {
            value = static_cast<DLB_ADM_AUDIO_TYPE>(v);
        }

        return status;
    }

    int ParseValue(dlb_adm_entity_id &value, const std::string &valueString)
    {
        AdmIdTranslator translator;
        value = translator.Translate(valueString.c_str());

        return (value == 0ull);
    }

    int ParseValue(dlb_adm_time &value, const std::string &valueString)
    {
        /*
        Three possible time formats:
            hh:mm:ss.zzzzzSfffff
            hh:mm:ss.zzzzz
            zzzzzSfffff

            There should be at least 5 of 'z' and 'f' each.
        */

        int status = DLB_ADM_STATUS_OK;

        ::memset(&value, 0, sizeof(dlb_adm_time));

        try
        {
            std::string str = valueString;
            size_t idx = 0;
            size_t cursor;
            size_t read;
            std::string sub;

            // Not short time
            if (boost::algorithm::contains(str, ":"))
            {
                cursor = str.find(":", idx);
                sub = str.substr(idx, cursor - idx);
                value.hours = std::stoi(sub, &read);
                if (read != sub.length() || read != HMS_SIZE)
                {
                    return DLB_ADM_STATUS_ERROR;
                }
                idx = cursor + 1;

                cursor = str.find(":", idx);
                sub = str.substr(idx, cursor - idx);
                value.minutes = std::stoi(sub, &read);
                if (read != sub.length() || read != HMS_SIZE)
                {
                    return DLB_ADM_STATUS_ERROR;
                }
                idx = cursor + 1;

                cursor = str.find(".", idx);
                sub = str.substr(idx, cursor - idx);
                value.seconds = std::stoi(sub, &read);
                if (read != sub.length() || read != HMS_SIZE)
                {
                    return DLB_ADM_STATUS_ERROR;
                }
                idx = cursor + 1;
            }

            if (boost::algorithm::contains(str, "S"))
            {
                cursor = str.find("S", idx);
                sub = str.substr(idx, cursor - idx);
                value.fraction_numerator = std::stoi(sub, &read);
                if (read != sub.length() || read < ZF_MIN_SIZE)
                {
                    return DLB_ADM_STATUS_ERROR;
                }

                sub = str.substr(cursor + 1);
                value.fraction_denominator = std::stoi(sub, &read);
                if (read != sub.length() || read < ZF_MIN_SIZE)
                {
                    return DLB_ADM_STATUS_ERROR;
                }
            }
            else
            {
                sub = str.substr(idx);
                value.fraction_numerator = std::stoi(sub, &read);
                if (read != sub.length() || read < ZF_MIN_SIZE)
                {
                    return DLB_ADM_STATUS_ERROR;
                }
            }
        }
        catch(...)
        {
            status = DLB_ADM_STATUS_ERROR;
        }

        if(status == DLB_ADM_STATUS_OK)
        {
            // verify values
            bool fail = false;
            fail |= value.minutes >= MINUTES_PER_HOUR;
            fail |= value.seconds >= SECONDS_PER_MINUTES;
            if (fail)
            {
                status = DLB_ADM_STATUS_ERROR;
            }

        }
        return status;
    }

    attributeString
    convertToAttributeString
        (const std::string &str
        )
    {
        attributeString attrStr;
        ParseValue(attrStr, str);

        return attrStr;
    }


    // Stream output

    class StreamOut : public boost::static_visitor<void>
    {
    public:
        StreamOut(std::ostream &os) : mOstream(os), mGood(true) {}

        void operator()(dlb_adm_bool v) const;
        void operator()(dlb_adm_uint v) const;
        void operator()(dlb_adm_int v) const;
        void operator()(dlb_adm_float v) const;
        void operator()(DLB_ADM_AUDIO_TYPE v) const;
        void operator()(const dlb_adm_time &v) const;
        void operator()(const attributeString &v) const;
        void operator()(dlb_adm_entity_id v) const;

        bool IsGood() const { return mGood; }

    private:
        std::ostream &mOstream;
        bool mGood;
    };

    void StreamOut::operator()(dlb_adm_bool v) const
    {
        if (v == DLB_ADM_FALSE)
        {
            mOstream << 0;
        }
        else
        {
            mOstream << 1;
        }
    }

    void StreamOut::operator()(dlb_adm_uint v) const
    {
        mOstream << v;
    }

    void StreamOut::operator()(dlb_adm_int v) const
    {
        mOstream << v;
    }

    void StreamOut::operator()(dlb_adm_float v) const
    {
        char buffer[64];

        ::sprintf(buffer, "%.2F", v);   // TODO -- figure out how to do different precisions as desired

        mOstream << buffer;
    }

    void StreamOut::operator()(DLB_ADM_AUDIO_TYPE v) const
    {
        char buffer[32];

        ::sprintf(buffer, "%04X", static_cast<int>(v));

        mOstream << buffer;
    }

    static void normalizeTimeField(uint8_t &larger, uint8_t &smaller, uint8_t divisor)
    {
        if (smaller > divisor)
        {
            larger += (smaller / divisor);
            smaller %= divisor;
        }
    }

    static uint32_t magnitude(uint32_t v, uint32_t &d)
    {
        uint32_t x = 10;
        uint32_t m = 1;

        while (v > x)
        {
            x *= 10;
            ++m;
        }
        d = x;

        return m;
    }

    void StreamOut::operator()(const dlb_adm_time &v) const
    {
        dlb_adm_time timeOut = v;
        char buffer[128];

        normalizeTimeField(timeOut.minutes, timeOut.seconds, 60);
        normalizeTimeField(timeOut.hours, timeOut.minutes, 60);

        ::sprintf(buffer, "%02u:%02u:%02u", timeOut.hours, timeOut.minutes, timeOut.seconds);  // TODO: what if hours > 99?
        mOstream << buffer;

        if (timeOut.fraction_denominator == 0)
        {
            uint32_t d;
            uint32_t m = magnitude(timeOut.fraction_numerator, d);
            float f = static_cast<float>(timeOut.fraction_numerator) / static_cast<float>(d);

            if (m < 5)
            {
                m = 5;
            }
            ::sprintf(buffer, "%-.*f", static_cast<int>(m), f);
            mOstream << (buffer + 1);
        }
        else
        {
            uint32_t dn;
            uint32_t mn = magnitude(timeOut.fraction_numerator, dn);
            uint32_t dd;
            uint32_t md = magnitude(timeOut.fraction_denominator, dd);

            if (mn < md)
            {
                mn = md;
            }
            ::sprintf(buffer, ".%0*uS%*u", mn, timeOut.fraction_numerator, md, timeOut.fraction_denominator);
            mOstream << buffer;
        }
    }

    void StreamOut::operator()(const attributeString &v) const
    {
        mOstream << v.data();
    }

    void StreamOut::operator()(dlb_adm_entity_id v) const
    {
        AdmIdTranslator translator;
        mOstream << translator.Translate(v);
    }

    std::ostream &operator<<(std::ostream &os, const AttributeValue &value)
    {
        StreamOut so(os);
        boost::apply_visitor(so, value);
        if (!so.IsGood())
        {
            std::cerr << "Bad call to operator<<(std::ostream &os, const AttributeValue &value)" << std::endl;
        }
        return os;
    }

}
