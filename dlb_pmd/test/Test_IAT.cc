/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#include "PrngKiss.hh"
#include "TestModel.hh"

#include "src/model/pmd_model.h"
#include "src/modules/xml/xml_eidr.h"
#include "dlb_pmd_api.h"    

#include "gtest/gtest.h"


class IAT_PayloadTest: public ::testing::TestWithParam<std::tr1::tuple<int, int, int, int> > {};

static inline
bool
single_channel_pcm_test
    (TestModel::TestType t
    )
{
    switch (t)
    {
        case TestModel::TEST_PCM_CHAN_2398:  return true;
        case TestModel::TEST_PCM_CHAN_2400:  return true;
        case TestModel::TEST_PCM_CHAN_2500:  return true;
        case TestModel::TEST_PCM_CHAN_2997:  return true;
        case TestModel::TEST_PCM_CHAN_3000:  return true;
        case TestModel::TEST_PCM_CHAN_5000:  return true;
        case TestModel::TEST_PCM_CHAN_5994:  return true;
        case TestModel::TEST_PCM_CHAN_6000:  return true;
        case TestModel::TEST_PCM_CHAN_10000: return true;
        case TestModel::TEST_PCM_CHAN_11988: return true;
        case TestModel::TEST_PCM_CHAN_12000: return true;
        default:
            return false;
    }
}



static inline
dlb_pmd_success
add_iat_content_id
    (TestModel& m
    ,PrngKiss& prng
    ,unsigned int options
    ,unsigned int e
    ,unsigned int full
    )
{
    uint8_t tmp[256];
    unsigned int len;

    if (options & 0x01)
    {
        switch (e % 0x1f)
        {
            case 0:
                snprintf((char*)tmp, sizeof(tmp), "%08x-%04x-%04x-%04x-%04x%04x%04x",
                         prng.next(),
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff);
                if (dlb_pmd_iat_content_id_uuid(m, (char*)tmp)) return 1;
                break;
            case 1:
                snprintf((char*)tmp, sizeof(tmp), "10.%u/%04x-%04x-%04x-%04x-%04x",
                         EIDR_SUB_PREFIX,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff,
                         prng.next() & 0xffff);
                if (dlb_pmd_iat_content_id_eidr(m, (char*)tmp)) return 1;
                break;
            case 2:
                snprintf((char*)tmp, sizeof(tmp), "a1b2c3d4e5f");
                if (dlb_pmd_iat_content_id_ad_id(m, (char*)tmp)) return 1;
                break;
            default:
                len = full
                    ? (PMD_IAT_CONTENT_ID_SPACE-1)
                    : (prng.next() % (PMD_IAT_CONTENT_ID_SPACE-1));
                memset(tmp, '\0', sizeof(tmp));
                prng.gen_rand_bytearray(tmp, len, e&1);
                dlb_pmd_content_id_type id = (dlb_pmd_content_id_type)(e % 0x1f);
                if (dlb_pmd_iat_content_id_raw(m, id, len, tmp)) return 1;
                break;
        }
    }
    return 0;
}


static inline
dlb_pmd_success
add_iat_distribution_id
    (TestModel& m
    ,PrngKiss& prng
    ,unsigned int options
    ,unsigned int e
    ,unsigned int full
    )
{
    uint8_t tmp[256];
    unsigned int len;

    if (options & 2)
    {
        switch (e % 7)
        {
            case 0:
                if (dlb_pmd_iat_distribution_id_atsc3(m,
                                                      prng.next() & 0xffff,
                                                      prng.next() & 0x03ff,
                                                      prng.next() & 0x03ff))
                    return 1;
                break;
            default:
                len = full
                    ? (PMD_IAT_DISTRIBUTION_ID_SPACE-1)
                    : (prng.next() % (PMD_IAT_DISTRIBUTION_ID_SPACE-1));
                memset(tmp, '\0', sizeof(tmp));
                prng.gen_rand_bytearray(tmp, len, e&1);
                dlb_pmd_distribution_id_type id = (dlb_pmd_distribution_id_type)(e % 7);
                if (dlb_pmd_iat_distribution_id_raw(m, id, len, tmp)) return 1;
                break;
        }
    }
    return 0;
}
        

static inline
dlb_pmd_success
add_iat_offset
    (TestModel& m
    ,PrngKiss& prng
    ,unsigned int options
    ,unsigned int e
    )
{
    (void)e;
    if (options & 4)
    {
        if (dlb_pmd_iat_set_offset(m, prng.next() & 0x07ff)) return 1;
    }
    return 0;
}
           

static inline
dlb_pmd_success
add_iat_validity_duration
    (TestModel& m
    ,PrngKiss& prng
    ,unsigned int options
    ,unsigned int e
    )
{
    (void)e;
    if (options & 8)
    {
        if (dlb_pmd_iat_set_validity_duration(m, prng.next() & 0x07ff)) return 1;
    }
    return 0;
}
           

static inline
dlb_pmd_success
add_iat_user_data
    (TestModel& m
    ,TestModel::TestType t
    ,PrngKiss& prng
    ,unsigned int options
    ,unsigned int e
    ,unsigned int full
    )
{
    uint8_t tmp[PMD_IAT_USER_DATA_SPACE];
    if (options & 16)
    {
        unsigned int len = full
            ? (PMD_IAT_USER_DATA_SPACE-1)
            : (prng.next() % (PMD_IAT_USER_DATA_SPACE-1));

        if (single_channel_pcm_test(t))
        {
            /* we only have a limited amound of space in single-channel mode,
             * and *can't* stuff 255 bytes of user data and 255 bytes of
             * extension data
             */
            len = prng.next() % 32;
        }

        memset(tmp, '\0', sizeof(tmp));
        prng.gen_rand_bytearray(tmp, len, e&1);
        if (dlb_pmd_iat_set_user_data(m, len, tmp)) return 1;
    }
    return 0;
}
           

static inline
dlb_pmd_success
add_iat_extension
    (TestModel& m
    ,TestModel::TestType t
    ,PrngKiss& prng
    ,unsigned int options
    ,unsigned int e
    ,unsigned int full
    )
{
    uint8_t tmp[PMD_IAT_EXTENSION_SPACE];
    unsigned int len;
    if (options & 32)
    {
        len = full
            ? (PMD_IAT_EXTENSION_SPACE-1)
            : (prng.next() % (PMD_IAT_EXTENSION_SPACE-1));

        if (single_channel_pcm_test(t))
        {
            /* we only have a limited amound of space in single-channel mode,
             * and *can't* stuff 255 bytes of user data and 255 bytes of
             * extension data
             */
            len = prng.next() % 32;
        }

        memset(tmp, '\0', sizeof(tmp));
        prng.gen_rand_bytearray(tmp, len, e&1);
        if (dlb_pmd_iat_set_extension(m, len, tmp)) return 1;;
    }
    return 0;
}


/* IAT options:
 *   - content_id,              1
 *   - distribution_id,         2
 *   - offset,                  4
 *   - validity_duration,       8
 *   - user_data                16
 *   - extension_data           32
 */
TEST_P(IAT_PayloadTest, payload_testing)
{
    unsigned int options = std::tr1::get<0>(GetParam());
    unsigned int e = std::tr1::get<1>(GetParam());
    unsigned int full = std::tr1::get<2>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<3>(GetParam())];

    TestModel m;
    dlb_pmd_element_id ignore;
    uint8_t tmp[256];
    PrngKiss prng;
    prng.seed(options);

    memset(tmp, '\0', sizeof(tmp));

    if (   m.populate(&ignore)
        || dlb_pmd_set_title(m, "IAT testing")
        || dlb_pmd_iat_add(m, (1ull<<e)-1)
        || add_iat_content_id(m, prng, options, e, full)
        || add_iat_distribution_id(m, prng, options, e, full)
        || add_iat_offset(m, prng, options, e)
        || add_iat_validity_duration(m, prng, options, e)
        || add_iat_user_data(m, t, prng, options, e, full)
        || add_iat_extension(m, t, prng, options, e, full)
       )
    {
        ADD_FAILURE() << "Could not build model: " << dlb_pmd_error(m);
    }
    else
    {
        char tmp[128];
        snprintf(tmp, sizeof(tmp), "IAT_option%d_test%d_full%d", options, e, full);
        m.skip_pcm_samples(options * e + full);
        try
        {
            m.test(t, tmp, options+e);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_IAT, IAT_PayloadTest,
                        testing::Combine(testing::Range(0, 1<<6), /* options */
                                         testing::Range(0, 35),  /* max enum value */
                                         testing::Range(0, 2),
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));

