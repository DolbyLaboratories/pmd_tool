/************************************************************************
 * dlb_pmd
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

/**
 * @file Test_ABD_AOD_APD.cc
 * @brief Test different combinations of beds, objects and presentations
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"
#include "src/model/pmd_model.h"

extern "C"
{
#include "pmd_test_langs.h"
}

#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_ABD_AOD_APD_TESTS

/**
 * @brief number of channels in each different speaker config
 */
static const unsigned int BED_SIZES[ NUM_PMD_SPEAKER_CONFIGS ] =
{
    2,  /* 2.0 */
    3,  /* 3.0 */
    6,  /* 5.1 */
    8,  /* 5.1.2 */
    10, /* 5.1.4 */
    12, /* 7.1.4 */
    16, /* 9.1.6 */
    2,  /* portable speaker */
    2   /* portable headphones */
};


static const int MAX_BED_CHANS = 16;

/* In theory we could add more beds, objects and presentations, but it
 * doesn't seem likely in practice, and we only have a max of 255 input
 * signals
 */
static const int MAX_BASIC_BEDS = 4;
static const int MAX_BASIC_OBJS = 32;
static const int MAX_BASIC_PRES = 16;

static const int MAX_BEDS = 128;
static const int MAX_OBJS = 128;
static const int MAX_PRES = 511;

static const int GUARD_SAMPLES = 32;
static const int PMD_BLOCK_SIZE = 160;
static const int MAX_DATA_BYTES_PAIR = (PMD_BLOCK_SIZE - 6) * 5;
static const int MAX_DATA_BYTES_CHAN = ((PMD_BLOCK_SIZE - 8) * 5)/2;

static const int BLOCK_OVERHEAD = (16  /* UL */
                                   + 4 /* BER length field */
                                   + 4 /* container config*/
                                   + 2 /* body marker*/
                                   + 4 /* version*/
                                   + 6 /* CRC */
                                   );

static const int PCM_FACTOR = 2;

/**
 * @brief report available bits
 */
struct BitCapacity
{
    unsigned int mtx0_bits;    /**< number of bits for MTx(0), ABD,AOD,APN,HED only */
    unsigned int other_bits;   /**< bits available in all other time slices */

    BitCapacity(unsigned int m0, unsigned int ob) : mtx0_bits(m0), other_bits(ob) {};
};



class ElementTest: public ::testing::TestWithParam<std::tr1::tuple<int, int, int, int> > {};


/**
 * @brief estimate how much actual data is available in a given test config
 */
static BitCapacity estimate_pcmklv_capacity(unsigned int frame_length, bool ispair)
{
    unsigned int num_blocks = frame_length / PMD_BLOCK_SIZE;
    unsigned int block_bytes = (ispair ? MAX_DATA_BYTES_PAIR : MAX_DATA_BYTES_CHAN) - BLOCK_OVERHEAD;
    unsigned int mtx0_bits = block_bytes * 8;
    unsigned int num_bits = (num_blocks-1) * block_bytes * 8;
    unsigned int guard_bits = (ispair ? (GUARD_SAMPLES * 40) : (GUARD_SAMPLES * 20));

    /* simulate removal of GUARD_SAMPLES worth of space from 1st and last blocks */
    num_bits -= guard_bits;
    mtx0_bits -= guard_bits;

    /* remove the tag/length overhead for the ABD, AOD, APD, AEN and APN payloads, assume
     * in each block */
    num_bits -= (5 * 16 * (num_blocks-1));
    mtx0_bits -= (4 * 16); /* only ABD, AOD, APD, HED */
  
    mtx0_bits = (unsigned int)(mtx0_bits * .9);
    num_bits = (unsigned int)(num_bits * .9);
    
    return BitCapacity(mtx0_bits, num_bits);
}


/**
 * @brief estimate how many bits are available in a single frame
 *
 * All beds, objects and elements must be transmitted in a single frame.
 */
static BitCapacity estimate_test_capacity(TestModel::TestType t)
{
    switch (t)
    {
        case TestModel::TEST_XML:            return BitCapacity(0, ~0u);  /* unlimited */
        case TestModel::TEST_MDSET:          return BitCapacity(0, ~0u);
        case TestModel::TEST_KLV:            return BitCapacity(0, ~0u);

        case TestModel::TEST_PCM_PAIR_2398:  return estimate_pcmklv_capacity(2002, true);
        case TestModel::TEST_PCM_PAIR_2400:  return estimate_pcmklv_capacity(2000, true);
        case TestModel::TEST_PCM_PAIR_2500:  return estimate_pcmklv_capacity(1920, true);
        case TestModel::TEST_PCM_PAIR_2997:  return estimate_pcmklv_capacity(1601, true);
        case TestModel::TEST_PCM_PAIR_3000:  return estimate_pcmklv_capacity(1600, true);
        case TestModel::TEST_PCM_PAIR_5000:  return estimate_pcmklv_capacity( 960, true);
        case TestModel::TEST_PCM_PAIR_5994:  return estimate_pcmklv_capacity( 800, true);
        case TestModel::TEST_PCM_PAIR_6000:  return estimate_pcmklv_capacity( 800, true);
        case TestModel::TEST_PCM_PAIR_10000: return estimate_pcmklv_capacity( 480, true);
        case TestModel::TEST_PCM_PAIR_11988: return estimate_pcmklv_capacity( 400, true);
        case TestModel::TEST_PCM_PAIR_12000: return estimate_pcmklv_capacity( 400, true);

        case TestModel::TEST_PCM_CHAN_2398:  return estimate_pcmklv_capacity(2002, false);
        case TestModel::TEST_PCM_CHAN_2400:  return estimate_pcmklv_capacity(2000, false);
        case TestModel::TEST_PCM_CHAN_2500:  return estimate_pcmklv_capacity(1920, false);
        case TestModel::TEST_PCM_CHAN_2997:  return estimate_pcmklv_capacity(1601, false);
        case TestModel::TEST_PCM_CHAN_3000:  return estimate_pcmklv_capacity(1600, false);
        case TestModel::TEST_PCM_CHAN_5000:  return estimate_pcmklv_capacity( 960, false);
        case TestModel::TEST_PCM_CHAN_5994:  return estimate_pcmklv_capacity( 800, false);
        case TestModel::TEST_PCM_CHAN_6000:  return estimate_pcmklv_capacity( 800, false);
        case TestModel::TEST_PCM_CHAN_10000: return estimate_pcmklv_capacity( 480, false);
        case TestModel::TEST_PCM_CHAN_11988: return estimate_pcmklv_capacity( 400, false);
        case TestModel::TEST_PCM_CHAN_12000: return estimate_pcmklv_capacity( 400, false);

        case TestModel::TEST_SADM:         return BitCapacity(0, ~0u);
        default:
            return BitCapacity(0, ~0u);
    }
}


/**
 * @brief estimate number of bits required by a test config (excluding tag/len bytes)
 */
static BitCapacity estimate_bit_requirement(unsigned int num_beds,
                                            unsigned int num_objs,
                                            unsigned int num_pres)
{
    unsigned int num_elements = (num_beds + num_objs + num_pres) % MAX_PRESENTATION_ELEMENTS;
        
    if (num_beds + num_objs + num_pres >= 111)
    {
        num_elements = 6;
    }
    if (num_elements > num_beds + num_objs)
    {
        num_elements = num_beds + num_objs;
    }

    unsigned int bed_bits  = (18 + (MAX_BED_CHANS+1)*20);
    unsigned int obj_bits  = 68;
    unsigned int pres_bits = (29 + (num_elements+1)*12);

#define ROUND_TO_BYTES(bits) (((bits + 7)/8)*8)
    unsigned int mtx0_bits
        = ROUND_TO_BYTES(bed_bits * num_beds)
        + ROUND_TO_BYTES(obj_bits * num_objs)
        + ROUND_TO_BYTES(pres_bits * num_pres);

    /* simulate the effect of not having enough space for one complete 'thing'
     * at end of block by adding requirement for one additional thing of biggest
     * size
     */
    assert(bed_bits > obj_bits);
    unsigned int max_item = bed_bits;
    max_item = max_item > pres_bits ? max_item : pres_bits;
    mtx0_bits += max_item;
    
    static const int PRES_NAME_LENGTH = 13; /* "TEST_PRES_%u" */
    static const int ELEMENT_NAME_LENGTH = 10; /* "Object %u" */
    unsigned int name_bits
        = ROUND_TO_BYTES((20 + 8*ELEMENT_NAME_LENGTH) * (num_beds + num_objs))
        + ROUND_TO_BYTES((32 + 8*PRES_NAME_LENGTH) * num_pres * 2);
    /* we test with dual-named presentations */
    
    return BitCapacity(mtx0_bits, name_bits);
}


/* Because the data can be very large, not all PCM data rates will be
 * able to transport the data completely. We adjust the test
 * parameters to account for this.
 */
static void configure_test_params(TestModel& m,
                                  TestModel::TestType t,
                                  unsigned int& num_beds,
                                  unsigned int& num_objs,
                                  unsigned int& num_pres)
{
    if (num_pres > DLB_PMD_MAX_PRESENTATIONS)
    {
        num_pres = DLB_PMD_MAX_PRESENTATIONS;
    }

    BitCapacity req = estimate_bit_requirement(num_beds, num_objs, num_pres);
    BitCapacity capacity = estimate_test_capacity(t);
    unsigned int req_bits;
    unsigned int available;
    
    for (;;)
    {
        /* remove MTx(0).  capacity.mtx0_bits indicates bits available in 1s */
        if (req.mtx0_bits > capacity.mtx0_bits)
        {
            req.mtx0_bits -= capacity.mtx0_bits;
        }
        else
        {
            req.mtx0_bits = 0;
        }
        /* now that 1st block is accounted for, consider all remaining bits in
         * all other blocks */
        
        req_bits = req.mtx0_bits + req.other_bits;
        available = capacity.other_bits;
        if (req_bits <= available)
        {
            return;
        }

        if (req.mtx0_bits < available)
        {
            m.minimal_check();
            return;
        }
            
        num_beds /= 2;
        num_objs /= 2;
        num_pres /= 2;
        if (num_pres == 0) num_pres = 1;
        
        if (num_beds == 0 && num_objs == 0)
        {
            switch (((unsigned int)t) % 3)
            {
                case 0:
                    num_beds = 1;
                    break;
                case 1:
                    num_objs = 1;
                    break;
                case 2:
                    num_beds = 1;
                    num_objs = 1;
            }
        }
        req = estimate_bit_requirement(num_beds, num_objs, num_pres);
    }
}
                                  

static bool generate_sadm_model(TestModel& m,
                                unsigned int num_beds, unsigned int num_objs,
                                unsigned int num_pres)
{
    dlb_pmd_element_id bed_elements[MAX_BEDS];
    dlb_pmd_element_id obj_elements[MAX_OBJS];
    dlb_pmd_speaker_config bed_configs[MAX_BEDS];
    unsigned int option = (num_beds + num_objs + num_pres);
    unsigned int num_bed_signals = 0;
    int bedcfg_limit;
    unsigned int ch = 0;
    bool merge_beds = false;
    unsigned int i;
    unsigned int j;

    /* remove compiler warnings */
    (void)ISO_639_1_CODES;
    (void)ISO_639_2t_CODES;
    
    bedcfg_limit = (num_beds > 16)
        ? (num_beds > 40)
        ? ((int)DLB_PMD_SPEAKER_CONFIG_2_0+1)
        : ((int)DLB_PMD_SPEAKER_CONFIG_5_1+1)
        : (int)DLB_PMD_SPEAKER_CONFIG_7_1_4+1;
    

    /* count required number of signals */
    for (i = 0; i != num_beds; ++i)
    {
        dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)((option+i) % bedcfg_limit);
        num_bed_signals += BED_SIZES[cfg];
    }

    /* if too many signals, merge the beds */
    if (num_bed_signals + num_objs > 255)
    {
        merge_beds = true;
        num_bed_signals = 0;

        for (i = 0; i != num_beds; ++i)
        {
            dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)((option+i) % bedcfg_limit);
            
            if (BED_SIZES[cfg] > num_bed_signals)
            {
                num_bed_signals = BED_SIZES[cfg];
            }
        }
    }

    assert(num_bed_signals + num_objs <= 255);
        
    printf("test elements: %u beds, %u objs %u presentations\n",
           num_beds, num_objs, num_pres);
    
    if (   dlb_pmd_set_title(m, "element testing")
        || dlb_pmd_add_signals(m, num_bed_signals + num_objs))
    {
        return true;
    }
    
    /* bed elements will have ids 1 to beds */
    for (i = 0; i != num_beds; ++i)
    {
        dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)((option+i) % bedcfg_limit);
        dlb_pmd_element_id id;
        char bedname[64];

        /* skip 3 element ids */
        m.new_elid();
        m.new_elid();
        m.new_elid();
        id = m.new_elid();
        snprintf(bedname, sizeof(bedname), "TestBedName %u", id);
        if (dlb_pmd_add_bed(m, id, bedname, cfg, ch+1, 0))
        {
            return true;        
        }
        if (!merge_beds)
        {
            ch += BED_SIZES[cfg];
        }
        bed_elements[i] = id;
        bed_configs[i] = cfg;
    }
    
    /* object elements will have ids num_beds+1 to num_beds+num_objs */
    for (i = 0; i != num_objs; ++i)
    {
        char objname[64];
        float x = (float)(num_objs+num_beds+num_pres) / (float)(MAX_BEDS + MAX_OBJS + MAX_PRES);
        float y = (x > 0.5f) ? x - 0.1f : x + 0.1f;
        float z = (x > 0.5f) ? x - 0.2f : x + 0.2f;
        dlb_pmd_element_id id;
        m.new_elid();
        m.new_elid();
        m.new_elid();
        id = m.new_elid();
        snprintf(objname, sizeof(objname), "TestObjName %u", id);
        if (dlb_pmd_add_object(m, id, objname,
                               (dlb_pmd_object_class)(i % PMD_CLASS_EMERGENCY_INFO),
                               num_bed_signals+i+1, x, y, z, 0, 0, 0, 0, 0))
        {
            return true;
        }
        obj_elements[i] = id;
    }
    
    for (i = 0; i != num_pres; ++i)
    {
        /* sADM presentations must have a single bed, and its presentation config
         * must match speaker config of that bed
         */
        const char *lang1 = ISO_639_2b_CODES[(option+i) % NUM_ISO_639_2b_CODES];
        const char *lang2 = ISO_639_2b_CODES[(option+i+1) % NUM_ISO_639_2b_CODES];
        dlb_pmd_element_id pres_elements[MAX_PRESENTATION_ELEMENTS];
        dlb_pmd_presentation_id presid;
        dlb_pmd_element_id bedid = bed_elements[i % num_beds];
        dlb_pmd_speaker_config cfg = bed_configs[i % num_beds];
        char name[64];

        unsigned int num_elements = option % MAX_PRESENTATION_ELEMENTS;
        
        if (num_elements > 1 + num_objs)
        {
            num_elements = 1 + num_objs;
        }
        
        if (!num_elements)
        {
            num_elements = 1;
        }
        
        pres_elements[0] = bedid;
        for (j = 1; j < num_elements; ++j)
        {
            pres_elements[j] = obj_elements[(j-1) % num_objs];
        }

        presid = m.new_presid();
        snprintf(name, sizeof(name), "TEST_PRES_%u", presid);
        if (   dlb_pmd_add_presentation(m, presid, lang1, name, lang1,
                                        cfg, num_elements, pres_elements)
            || dlb_pmd_add_presentation_name(m, presid, lang2, name))
        {
            return true;
        }
    }
    return false;
}


static bool generate_pmd_model(TestModel& m,
                               unsigned int num_beds, unsigned int num_objs,
                               unsigned int num_pres)
{
    dlb_pmd_element_id pres_elements[MAX_PRESENTATION_ELEMENTS];
    unsigned int option = (num_beds + num_objs + num_pres);
    unsigned int num_bed_signals = 0;
    int bedcfg_limit;
    unsigned int ch = 0;
    bool merge_beds = false;
    unsigned int i;

    /* remove compiler warnings */
    (void)ISO_639_1_CODES;
    (void)ISO_639_2t_CODES;
    
    for (i = 0; i != MAX_PRESENTATION_ELEMENTS; ++i)
    {
        /* in this test, we identify elements using ids that are
         * multiples of 4 */
        pres_elements[i] = (dlb_pmd_element_id)((i+1)*4);
    }

    bedcfg_limit = (num_beds > 16)
        ? (num_beds > 40)
        ? ((int)DLB_PMD_SPEAKER_CONFIG_2_0+1)
        : ((int)DLB_PMD_SPEAKER_CONFIG_5_1+1)
        : (int)DLB_PMD_SPEAKER_CONFIG_9_1_6+1;
    

    /* count required number of signals */
    for (i = 0; i != num_beds; ++i)
    {
        dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)((option+i) % bedcfg_limit);
        num_bed_signals += BED_SIZES[cfg];
    }

    /* if too many signals, merge the beds */
    if (num_bed_signals + num_objs > 255)
    {
        merge_beds = true;
        num_bed_signals = 0;

        for (i = 0; i != num_beds; ++i)
        {
            dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)((option+i) % bedcfg_limit);
            
            if (BED_SIZES[cfg] > num_bed_signals)
            {
                num_bed_signals = BED_SIZES[cfg];
            }
        }
    }

    assert(num_bed_signals + num_objs <= 255);
        
    printf("test elements: %u beds, %u objs %u presentations\n",
           num_beds, num_objs, num_pres);
    
    if (   dlb_pmd_set_title(m, "element testing")
        || dlb_pmd_add_signals(m, num_bed_signals + num_objs))
    {
        return true;
    }
    
    /* bed elements will have ids 1 to beds */
    for (i = 0; i != num_beds; ++i)
    {
        dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)((option+i) % bedcfg_limit);
        dlb_pmd_element_id id;
        char bedname[64];

        /* skip 3 element ids */
        m.new_elid();
        m.new_elid();
        m.new_elid();
        id = m.new_elid();
        snprintf(bedname, sizeof(bedname), "TestBedName %u", id);
        if (dlb_pmd_add_bed(m, id, bedname, cfg, ch+1, 0))
        {
            return true;        
        }
        if (!merge_beds)
        {
            ch += BED_SIZES[cfg];
        }
    }
    
    /* object elements will have ids num_beds+1 to num_beds+num_objs */
    for (i = 0; i != num_objs; ++i)
    {
        char objname[64];
        float x = (float)(num_objs+num_beds+num_pres) / (float)(MAX_BEDS + MAX_OBJS + MAX_PRES);
        float y = (x > 0.5f) ? x - 0.1f : x + 0.1f;
        float z = (x > 0.5f) ? x - 0.2f : x + 0.2f;
        dlb_pmd_element_id id;
        m.new_elid();
        m.new_elid();
        m.new_elid();
        id = m.new_elid();
        snprintf(objname, sizeof(objname), "TestObjName %u", id);
        if (dlb_pmd_add_object(m, id, objname,
                               (dlb_pmd_object_class)(i % PMD_CLASS_RESERVED),
                               num_bed_signals+i+1, x, y, z, 0, 0, 0, 0, 0))
        {
            return true;
        }
    }
    
    for (i = 0; i != num_pres; ++i)
    {
        const char *lang1 = ISO_639_2b_CODES[(option+i) % NUM_ISO_639_2b_CODES];
        const char *lang2 = ISO_639_2b_CODES[(option+i+1) % NUM_ISO_639_2b_CODES];
        unsigned int num_elements = option % MAX_PRESENTATION_ELEMENTS;
        dlb_pmd_presentation_id presid;
        dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)(option % bedcfg_limit);
        char name[64];

        if (num_beds + num_objs + num_pres >= 111)
        {
            num_elements = 6;
        }
        
        if (num_elements > num_beds + num_objs)
        {
            num_elements = num_beds + num_objs;
        }
        
        if (!num_elements)
        {
            num_elements = 1;
        }
        
        presid = m.new_presid();
        snprintf(name, sizeof(name), "TEST_PRES_%u", presid);
        if (   dlb_pmd_add_presentation(m, presid, lang1, name, lang1,
                                        cfg, num_elements, pres_elements)
            || dlb_pmd_add_presentation_name(m, presid, lang2, name))
        {
            return true;
        }
    }
    return false;
}


static bool generate_model(TestModel& m,
                           unsigned int num_beds, unsigned int num_objs,
                           unsigned int num_pres,
                           bool sadm)
{
    if (sadm)
    {
        return generate_sadm_model(m, num_beds, num_objs, num_pres);
    }
    else
    {
        return generate_pmd_model(m, num_beds, num_objs, num_pres);
    }
}


#ifndef DISABLE_ABD_AOD_APD_TESTS
TEST_P(ElementTest, combo_testing)
{
    unsigned int num_beds = std::tr1::get<0>(GetParam()) * 4;
    unsigned int num_objs = std::tr1::get<1>(GetParam()) * 4;
    unsigned int num_pres = std::tr1::get<2>(GetParam()) * 16;
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<3>(GetParam())];
    TestModel m;

    /* limit selves to a single frame when PCM testing */
    m.pcm_single_frame();
    
    configure_test_params(m, t, num_beds, num_objs, num_pres);
    if (generate_model(m, num_beds, num_objs, num_pres, t >= TestModel::TEST_SADM))
    {
        ADD_FAILURE() << "Could not generate model: " << dlb_pmd_error(m);
    }
    else
    {
        char tmp[128];
        snprintf(tmp, sizeof(tmp), "Elements_beds%u_objs%u_pres%u",
                 num_beds, num_objs, num_pres);
        m.skip_pcm_samples(((num_beds + num_objs + num_pres)/4) * 7);
        try
        {
            m.test(t, tmp, num_pres);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_ElementPresPCM, ElementTest,
           testing::Combine(testing::Range(MAX_BASIC_BEDS/4, MAX_BEDS/4+1),
                            testing::Range(MAX_BASIC_OBJS/4, MAX_OBJS/4+1),
                            testing::Range(MAX_BASIC_PRES/4, MAX_PRES/16+1, 2),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::TEST_PCM_CHAN_12000+1)));

INSTANTIATE_TEST_CASE_P(PMD_SADM_ElementPres, ElementTest,
           testing::Combine(testing::Range(MAX_BASIC_BEDS/4, MAX_BEDS/4+1),
                            testing::Range(MAX_BASIC_OBJS/4, MAX_OBJS/4+1),
                            testing::Range(MAX_BASIC_PRES/4, MAX_PRES/16+1, 2),
                            testing::Range((int)TestModel::TEST_SADM,
                                           TestModel::TEST_SADM+1)));


/* sADM is far less bandwith-efficient than PMD, so we have to cut down the
 * range of tests
 */
INSTANTIATE_TEST_CASE_P(PMD_SADM_ElementPresPCM, ElementTest,
           testing::Combine(testing::Range(1, MAX_BASIC_BEDS/4+1),
                            testing::Range(1, MAX_BASIC_OBJS/4+1),
                            testing::Range(1, MAX_BASIC_PRES/32+1),     // TODO: range is [1, 1], is that what we want?
                            testing::Range((int)TestModel::TEST_SADM_PCM_PAIR_2398,
                                           (int)TestModel::TEST_SADM_PCM_CHAN_3000+1)));
#endif
