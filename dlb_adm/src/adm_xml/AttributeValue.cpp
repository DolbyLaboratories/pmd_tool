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

#include "AttributeValue.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <iostream>
#include <stdio.h>

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

    DLB_ADM_VALUE_TYPE GetValueType::operator()(const std::string &) const
    {
        return DLB_ADM_VALUE_TYPE_STRING;
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

    int ParseValue(dlb_adm_time &value, const std::string &valueString)
    {
        (void)value;
        (void)valueString;
        return DLB_ADM_STATUS_ERROR;
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
        void operator()(const std::string &v) const;

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

        ::sprintf(buffer, "%.2f", v);   // TODO -- figure out how to do different precisions as desired

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

    void StreamOut::operator()(const std::string &v) const
    {
        mOstream << v;
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
