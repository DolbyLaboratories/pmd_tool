/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
 * @file Test_PresentationConfig.cc
 * @brief Test that bad presentation configs are caught
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"
#include "dlb_pmd_xml_string.h"
#include "dlb_pmd_generate.h"

extern "C"
{
#include "src/model/pmd_model.h"
}

#include "TestModel.hh"
#include "gtest/gtest.h"

#include <sstream>

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_PRESENTATION_CONFIG_TESTS


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

static const char *speaker_config_names[NUM_PMD_SPEAKER_CONFIGS] =
{
    "2.0", "3.0", "5.1", "5.1.2", "5.1.4", "7.1.4", "9.1.6",
    "Portable Speaker", "Portable Headphone"
};


static const char *object_class_codes[PMD_CLASS_RESERVED] =
{
    "D", "VDS", "VO", "O", "SS", "EA", "EI"
};
    


#ifdef todo
static
std::string
swap_cm_me
    (bool complete_main
    ,std::string orig
    )
{
    std::string res = orig;

    if (complete_main)
    {
        size_t x = orig.find("CM", 0);
        if (std::string::npos == x)
        {
            ADD_FAILURE() << "Bad original config";
        }
        else
        {
            res.replace(x, 2, "ME");
        }
    }
    else
    {
        size_t x = orig.find("ME", 0);
        if (std::string::npos == x)
        {
            ADD_FAILURE() << "Bad original config";
        }
        else
        {
            res.replace(x, 2, "CM");
        }
    }
    return res;
}
#endif


static
void
test_bad_config
    (std::string& pmdxml
    ,size_t pos
    ,size_t len
    ,std::string& orig_config
    ,std::string bad_config
    )
{
    TestModel m2;
    std::string bad = pmdxml;
    bad.replace(pos, len, bad_config);
                
    if (!dlb_xmlpmd_string_read((const char*)bad.data(), bad.length(), m2, DLB_PMD_XML_STRICT,
                                NULL, NULL, NULL))
    {
        /* we succeeded parsing a dodgy presentation config */
        ADD_FAILURE() << "Bad presentation config \"" << orig_config << "\""
                      << " was accepted (original: \"" << bad_config << "\")";
    }
}

     
struct presentation_config_content
{
    dlb_pmd_speaker_config speaker_config_;
    bool complete_main_;
    bool any_objects_;
    unsigned int object_counts_[PMD_CLASS_RESERVED];
    
    presentation_config_content(TestModel& m)
        : complete_main_(false)
        , any_objects_(false)
    {
        memset(object_counts_, 0, sizeof(object_counts_));

        dlb_pmd_element_id elements[DLB_PMD_MAX_PRESENTATION_ELEMENTS];
        dlb_pmd_presentation_iterator it;
        dlb_pmd_presentation pres;
        dlb_pmd_object object;
    
        if (   dlb_pmd_presentation_iterator_init(&it, m)
            || dlb_pmd_presentation_iterator_next(&it, &pres,
                                                  DLB_PMD_MAX_PRESENTATION_ELEMENTS, elements))
        {
            ADD_FAILURE() << "bad model generated";
        }
        else
        {
            speaker_config_ = pres.config;
            unsigned int objcount = 0;
            for (unsigned int i = 0; i != pres.num_elements; ++i)
            {
                if (!dlb_pmd_object_lookup(m, pres.elements[i], &object))
                {
                    object_counts_[object.object_class] += 1;
                    objcount += 1;
                }
            }
            complete_main_ = !object_counts_[PMD_CLASS_DIALOG];
            any_objects_ = !!objcount;
        }
    }
};

    
static
std::string
generate_xml
    (TestModel& m
    )
{
    static const size_t BUFFER_SIZE = 10 * 1024 * 1024;
    std::string buffer(BUFFER_SIZE, 0);
    size_t size = BUFFER_SIZE;
    
    if (dlb_xmlpmd_string_write(m, (char*)buffer.data(), &size))
    {
        ADD_FAILURE() << "Could not generate XML";
        return "";
    }
    return buffer;
}


static
void
find_presentation_config
    (std::string& buffer
    ,size_t& start
    ,size_t& end
    )
{
    start = buffer.find("<Config>", 0);
    end = buffer.find("</Config>", start);
            
    if (std::string::npos == start || std::string::npos == end)
    {
        ADD_FAILURE() << "Could not find Presentation Config string";
    }
    start += 8;  /* length of "<Config>" */
}


#ifndef DISABLE_PRESENTATION_CONFIG_TESTS
class PresentationConfigTest: public ::testing::TestWithParam<int> {};


/*
 * Test presentation config XML string.  We should already know that
 * the presentation config string auto-generated by the pmd xml writer
 * code works properly by virtue of the other tests.
 *
 * The following tests attempt to show that bad config strings are
 * rejected.
 */
TEST_P(PresentationConfigTest, presentation_config_string)
{
    unsigned int num_objs = (unsigned int)GetParam();
    unsigned int seed = num_objs * 0x77777777u;
    dlb_pmd_metadata_count counts;
    TestModel m;

    memset(&counts, '\0', sizeof(counts));
    counts.num_signals       = PMD_GENERATE_RANDOM_NUMBER;
    counts.num_beds          = 1;
    counts.num_objects       = num_objs;
    counts.num_presentations = 1;

    if (dlb_pmd_generate_random(m, &counts, seed, 0, 0))
    {
        ADD_FAILURE() << "Could not generate random model";        
    }
    else
    {
        presentation_config_content pcc(m);
        std::string pmdxml = generate_xml(m);
        size_t start;
        size_t end;
        find_presentation_config(pmdxml, start, end);
        size_t len = end - start;

        std::string orig_config(pmdxml, start, len);
        std::ostringstream new_config;    
        /* test empty config */
        test_bad_config(pmdxml, start, len, orig_config, new_config.str());

        if (pcc.speaker_config_ < DLB_PMD_SPEAKER_CONFIG_PORTABLE && pcc.any_objects_)
        {

            new_config << speaker_config_names[pcc.speaker_config_];

            test_bad_config(pmdxml, start, len, orig_config, new_config.str());
            
#ifdef todo
            /* swap CM/ME ? */
            new_config = swap_cm_me(pcc.complete_main_, orig_config);
            test_bad_config(pmdxml, start, len, orig_config, new_config);
#endif
            
            /* generate an incorrect speaker count */
            new_config << (pcc.complete_main_ ? " CM" : " ME");

            for (unsigned int i = 0; i != PMD_CLASS_RESERVED; ++i)
            {
                unsigned int count = pcc.object_counts_[i];
                if (i == (num_objs % PMD_CLASS_RESERVED))
                {
                    count += 1;
                }
                
                if (count > 0)
                {
                    new_config << " + ";
                    if (count > 1)
                    {
                        new_config << count;
                    }
                    new_config << object_class_codes[i];
                }
            }
            test_bad_config(pmdxml, start, len, orig_config, new_config.str());
        }
    }
}



INSTANTIATE_TEST_CASE_P(PMD_PresentationConfig, PresentationConfigTest,
                        testing::Range(1, DLB_PMD_MAX_AUDIO_ELEMENTS));
#endif


