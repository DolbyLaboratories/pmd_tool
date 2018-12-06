/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018 by Dolby Laboratories,
 *                Copyright (C) 2018 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/
/**
 * @file Test_ESD.cc
 * @brief Test ED2 Stream Arrangement is correct
 */


#include "dlb_pmd_api.h"
#include "dlb_pmd_ed2.h"
#include "TestModel.hh"
#include "gtest/gtest.h"


//#define MAX_ESD_TEST_BEDS (6)
#define MAX_ESD_TEST_BEDS (4)
#define MAX_ESD_CHANNELS (64)

/**
 * @brief data structure used to represent the composition of a stream
 */
typedef struct
{
    unsigned int num_20_beds;     /* number of stereo beds */
    unsigned int num_40_beds;     /* for 3_0 cases with a stubbed 4th channel */
    dlb_pmd_bool is_51x_bed;      /* includes 5_1_2 and 5_1_4 */
    dlb_pmd_bool is_71x_bed;      /* includes 7_1 and 7_1_4 */
    dlb_pmd_bool is_91x_bed;      /* for 9_1_6 cases which occupy 2 full streams.
                                     num_heights will tell us if it is the first or second half */
    unsigned int num_heights;     /* number of height channels in this stream */
    unsigned int num_objs;        /* number of objects in this stream */
    unsigned int num_filler_objs; /* number of dummy objects in this stream.
                                     this is done to make sure atleast 4 objects exist */
} stream_composition;


/**
 * @brief data structure used to represent a single ESD test
 */
typedef struct
{
    dlb_pmd_speaker_config beds[MAX_ESD_TEST_BEDS];
    unsigned int stream_channel_counts[8];
    int pcm_reorder[64];
    unsigned int num_beds;
    unsigned int num_objs;
    unsigned int channel_count;
    unsigned int beds_20;
    unsigned int beds_30;
    unsigned int beds_51;
    unsigned int beds_512;
    unsigned int beds_514;
    unsigned int beds_714;
    unsigned int beds_916;

    stream_composition stream_comp[8];
    unsigned int expected_stream_count;
    unsigned int unused_slots;
} esd_test;


static inline
void
esd_test_init
    (esd_test *test
    )
{
    memset(test, '\0', sizeof(*test));
}


static inline
void
esd_test_add_beds
    (esd_test *test
    ,unsigned int num_beds
    ,unsigned int combo
    )
{
    unsigned int i;

    test->num_beds = num_beds;
    for (i = 0; i != num_beds; ++i)
    {
        test->beds[i] = (dlb_pmd_speaker_config)(combo % NUM_PMD_SPEAKER_CONFIGS);

        switch (test->beds[i])
        {
            case DLB_PMD_SPEAKER_CONFIG_2_0:
            case DLB_PMD_SPEAKER_CONFIG_PORTABLE:
            case DLB_PMD_SPEAKER_CONFIG_HEADPHONE:
                test->beds_20 += 1;
                test->channel_count += 2;
                break;

            case DLB_PMD_SPEAKER_CONFIG_3_0:
                test->beds_30 += 1;
                test->channel_count += 4;  /* we waste 1 channel to make a '4' program */
                break;
                
            case DLB_PMD_SPEAKER_CONFIG_5_1:
                test->beds_51 += 1;
                test->channel_count += 6;
                break;
                    
            case DLB_PMD_SPEAKER_CONFIG_5_1_2:
                test->beds_512 += 1;
                test->channel_count += 8;
                break;
                
            case DLB_PMD_SPEAKER_CONFIG_5_1_4:
                test->beds_514 += 1;
                test->channel_count += 10;
                break;

            case DLB_PMD_SPEAKER_CONFIG_7_1_4:
                test->beds_714 += 1;
                test->channel_count += 12;
                break;

            case DLB_PMD_SPEAKER_CONFIG_9_1_6:
                test->beds_916 += 1;
                test->channel_count += 16;
                break;
            default:
                break;
        }
        combo = combo / NUM_PMD_SPEAKER_CONFIGS;
    }
    
    test->expected_stream_count
        = (test->beds_20 + 3)/4
        + (test->beds_30 + 1)/2
        + test->beds_51
        + (2 * test->beds_512)
        + (2 * test->beds_514)
        + (2 * test->beds_714)
        + (2 * test->beds_916);
    
    test->unused_slots
        = ((test->beds_20 % 4) ? 2 * (4 - (test->beds_20%4)) : 0)
        + ((test->beds_30 % 2) ? 4 : 0)
        + (2 * test->beds_51)
        + (8 * test->beds_512)
        + (6 * test->beds_514)
        + (4 * test->beds_714);
}


static inline
void
esd_test_add_objs
    (esd_test *test
    ,unsigned int num_obj
    )
{
    test->num_objs = num_obj;
    test->channel_count += num_obj;

    if (num_obj > test->unused_slots)
    {
        num_obj -= test->unused_slots;
        test->unused_slots = 0;
    }
    else
    {
        test->unused_slots -= num_obj;
        num_obj = 0;
    }
    test->expected_stream_count += (num_obj + 7)/8;
}


static inline
dlb_pmd_success /* return 0 on success, 1 on failure */
esd_test_gen_model
    (esd_test *test
    ,TestModel& m
    )
{
    const int *mapend = &test->pcm_reorder[sizeof(test->pcm_reorder)/sizeof(int)];
    unsigned int stereo_beds = test->beds_20;
    unsigned int num_30_beds = test->beds_30;
    unsigned int current_stream = 0;
    int *map_stereo_cluster = NULL;
    int *map_30_cluster = NULL;
    int done_30_cluster;
    unsigned int ch;
    unsigned int i;
    int *map = test->pcm_reorder;
    unsigned int slots_in_stream;
    unsigned int added;

    /* can only handle 1 cluster of 4 beds in this test */
    assert(stereo_beds < 5);
    memset(test->stream_channel_counts, '\0', sizeof(test->stream_channel_counts));
    memset(test->stream_comp, '\0', sizeof(test->stream_comp));

    if (dlb_pmd_set_title(m, "ESD testing")) return 1;
    if (dlb_pmd_add_signals(m, test->channel_count)) return 1;

    ch = 0;
    current_stream = 0;
    printf("num beds: %u [", test->num_beds);
    for (i = 0; i != test->num_beds; ++i)
    {
        if (dlb_pmd_add_bed(m, m.new_elid(), NULL, test->beds[i], ch+1, 0)) return 1;
        switch (test->beds[i])
        {
            case DLB_PMD_SPEAKER_CONFIG_2_0:
            case DLB_PMD_SPEAKER_CONFIG_PORTABLE:
            case DLB_PMD_SPEAKER_CONFIG_HEADPHONE:
                printf("%s%s", i>0 ? " ": "",
                       test->beds[i] == DLB_PMD_SPEAKER_CONFIG_2_0 ? "2.0"
                       : test->beds[i] == DLB_PMD_SPEAKER_CONFIG_PORTABLE ? "PS"
                       : "PH");
                /* stereo beds are clustered together */
                if (stereo_beds == test->beds_20)
                {
                    map_stereo_cluster = map;
                    map += 2 * stereo_beds;
                    test->stream_channel_counts[current_stream] = 2 * stereo_beds;
                    test->stream_comp[current_stream].num_20_beds = stereo_beds;
                    ++current_stream;
                }
                *map_stereo_cluster++ = ch++;
                *map_stereo_cluster++ = ch++;
                stereo_beds -= 1;
                break;
            case DLB_PMD_SPEAKER_CONFIG_3_0:
                printf("%s3.0", i>0 ? " ": "");
                /* 3.0 beds are clustered together */
                if (map_30_cluster == NULL)
                {
                    map_30_cluster = map;
                    if (num_30_beds > 1)
                    {
                        map += 8;
                        test->stream_channel_counts[current_stream] = 8;
                        test->stream_comp[current_stream].num_40_beds = 2;
                        done_30_cluster = 0;
                    }
                    else
                    {
                        map += 4;
                        test->stream_channel_counts[current_stream] = 4;
                        test->stream_comp[current_stream].num_40_beds = 1;
                        done_30_cluster = 1;
                    }
                    ++current_stream;
                }
                else
                {
                    done_30_cluster = 1;
                }
                *map_30_cluster++ = ch++;
                *map_30_cluster++ = ch++;
                *map_30_cluster++ = ch; /* 3.0 duplicates last channel to make a '4' bed */
                *map_30_cluster++ = ch++;
                num_30_beds -= 1;
                if (done_30_cluster)
                {
                    map_30_cluster = NULL;
                }
                break;

            case DLB_PMD_SPEAKER_CONFIG_5_1:
                printf("%s5.1", i>0 ? " ": "");
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                /* current stream has 6 bed channels comprising a 5.1 bed */
                test->stream_channel_counts[current_stream] = 6;
                test->stream_comp[current_stream].is_51x_bed = 1;
                ++current_stream;
                break;
            case DLB_PMD_SPEAKER_CONFIG_5_1_2:
                printf("%s5.1.2", i>0 ? " ": "");
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                /* current stream has 6 bed channels comprising a 5.1 bed */
                test->stream_channel_counts[current_stream] = 6;
                test->stream_comp[current_stream].is_51x_bed = 1;
                /* 2 heights spill over to the next stream */
                test->stream_channel_counts[current_stream+1] = 2;
                test->stream_comp[current_stream+1].num_heights += 2;
                current_stream += 2;
                break;
            case DLB_PMD_SPEAKER_CONFIG_5_1_4:
                printf("%s5.1.4", i>0 ? " ": "");
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                /* current stream has 6 bed channels comprising a 5.1 bed */
                test->stream_channel_counts[current_stream] = 6;
                test->stream_comp[current_stream].is_51x_bed = 1;
                /* 4 heights spill over to the next stream */
                test->stream_channel_counts[current_stream+1] = 4;
                test->stream_comp[current_stream+1].num_heights += 4;
                current_stream += 2;
                break;
            case DLB_PMD_SPEAKER_CONFIG_7_1_4:
                printf("%s7.1.4", i>0 ? " ": "");
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                /* current stream has 8 bed channels comprising a 7.1 bed */
                test->stream_channel_counts[current_stream] = 8;
                test->stream_comp[current_stream].is_71x_bed = 1;
                /* 4 heights spill over to the next stream */
                test->stream_channel_counts[current_stream+1] = 4;
                test->stream_comp[current_stream+1].num_heights += 4;
                current_stream += 2;
                break;
                
            case DLB_PMD_SPEAKER_CONFIG_9_1_6:
                printf("%s9.1.6", i>0 ? " ": "");
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                *map++ = ch++;
                /* current stream has 8 bed channels comprising part of a 9.1 bed */
                test->stream_channel_counts[current_stream] = 8;
                test->stream_comp[current_stream].is_91x_bed = 1;
                /* remaining 2 bed channels and 6 heights spill over to the next stream */
                test->stream_channel_counts[current_stream+1] = 8;
                test->stream_comp[current_stream+1].is_91x_bed = 1;
                test->stream_comp[current_stream+1].num_heights += 6;
                current_stream += 2;
                break;

            default:
                break;
        }
    }
    printf("] num obj: %u\n", test->num_objs);

    /* adding objects may cause channel map to be shifted and duplicated */
    current_stream = test->expected_stream_count;
    slots_in_stream = 0;
    added = 0;
    for (i = 0; i != test->num_objs; ++i)
    {
        while (!slots_in_stream)
        {
            unsigned int used;
            
            --current_stream;
            used = test->stream_channel_counts[current_stream];
            slots_in_stream = 8 - used;
            if (slots_in_stream > (test->num_objs-i))
            {
                slots_in_stream = (test->num_objs-i);
            }
            if (!slots_in_stream)
            {
                map -= 8;
                continue;
            }
            
            /* objects should be added in order to object-only streams 1st,
             * and only *then* added in reverse order....*/
            if (added)
            {
                map -= test->stream_channel_counts[current_stream+1];
            }
            if (slots_in_stream)
            {
                memmove(map+slots_in_stream, map, (mapend-map-slots_in_stream) * sizeof(int));
            }
            added = 0;
        }
        *map++ = ch+i;
        ++added;
        --slots_in_stream;
        test->stream_channel_counts[current_stream] += 1;
        test->stream_comp[current_stream].num_objs += 1;
        if (dlb_pmd_add_generic_obj2(m, m.new_elid(), NULL, ch+i+1, 0.0f, 0.0f, 0.0f)) return 1;
    }
    /* now make sure there are at least 4 channels per stream, and an
     * even number */
    current_stream = test->expected_stream_count;
    slots_in_stream = test->stream_channel_counts[current_stream];
    i = 0;
    map = test->pcm_reorder;
    for (current_stream = 0; current_stream != test->expected_stream_count; ++current_stream)
    {
        unsigned int used = test->stream_channel_counts[current_stream];
        unsigned int silent = (used < 4) ? (4 - used) : (used & 1);

        map += used;
        if (silent)
        {
            unsigned int j;
            memmove(&map[silent], map, (mapend - (&map[silent])) * sizeof(int));
            for (j = 0; j != silent; ++j)
            {
                map[j] = -1;
            }
            map += silent;
            test->stream_channel_counts[current_stream] += silent;
            test->stream_comp[current_stream].num_filler_objs += silent;
        }
    }
    return 0;
}


/**
 * @brief helper function to shuffle pcm reorder array for an ed2 encoder
 */
static inline
void
esd_test_reorder_for_encoder
    (esd_test *test
    )
{
    unsigned int i;
    int tmp[8];
    int *arr = &test->pcm_reorder[0];
    const int end = sizeof(test->pcm_reorder)/sizeof(int);
    /* Iterate through each stream */
    for (i = 0; i < test->expected_stream_count; i++)
    {
        stream_composition *comp = &test->stream_comp[i];
        unsigned int start = 8 * i; /* start offset of current stream */
        unsigned int num_ch = test->stream_channel_counts[i];

        /* First, interleave the input to ensure that the pcm reorder array has a stride of 8 */
        if (num_ch < 8)
        {
            memmove(&arr[start+8], &arr[start+num_ch], sizeof(int)* (end-start-8));
            /* stub unused channels */
            while(num_ch < 8)
            {
                arr[start+num_ch] = -1;
                num_ch++;
            }
        }
        /* Take a look at the beds, heights and objects in this stream. An important point to
         * note here is that (upto 4) height channels will always be in the right positions
         * as they warrant the creation of a new stream. Only objects can be retrofitted into
         * streams with room for more channels.
         */
        if (  comp->is_51x_bed
           || comp->is_71x_bed
           ||(comp->is_91x_bed && comp->num_heights == 0))
        {
            /* No channel reshuffling is required as
             * another bed cannot coexist with these. The two objects
             * that can be present with a 5.1 bed do not need rearrangement.
             */
        }
        else if (comp->is_91x_bed && comp->num_heights == 6)
        {
            /* This is the second half of the two streams that make
             * up a 9.1.6 bed. This would have a program config of
             * 2_2_2_2 which requires reordering. The expected order
             * is [Pgm0 Pgm2 Pgm3 Pgm1]
             */
            memcpy(&tmp, &arr[start+2], 2 * sizeof(int)); /* back up Pgm1 */
            memmove(&arr[start+2], &arr[start+4], 4 * sizeof(int)); /* slide Pgm2 and Pgm3 ahead */
            memcpy(&arr[start+6], &tmp, 2 * sizeof(int)); /* stick Pgm1 at the end of the stream */
            /* Nothing more to do as the stream is full and cannot contain objects */
        }
        else if (comp->num_40_beds > 0 || comp->num_heights == 4)
        {
            /* A single 4_0 bed does not require rearranging, but two 4_0 beds result
             * in a 4_4 program config with the following expected order.
             * [Pgm0_ch0 Pgm0_ch1 Pgm0_ch2 Pgm0_ch3 Pgm1_ch2 Pgm1_ch3 Pgm1_ch0 Pgm1_ch1]
             * The same memory juggling is applied to a single 4_0 bed as it can be clubbed
             * with upto 4 objects. Note that this 4_0 bed can also be the height channels
             * from a 5.1.4 or 7.1.4 bed.
             */
            memcpy(&tmp, &arr[start+4], 2 * sizeof(int)); /* back up Pgm1 ch0 and ch1 */
            memmove(&arr[start+4], &arr[start+6], 2 * sizeof(int)); /* slide Pgm1 ch2 and ch3 ahead */
            memcpy(&arr[start+6], &tmp, 2 * sizeof(int)); /* stick Pgm1 ch0 and ch1 at the end of the stream */
        }
        else if (comp->num_20_beds > 0 || comp->num_heights == 2)
        {
            /* Single stereo bed does not require rearrangement. However,
             * clustered stereo beds have to be ordered as so:
             * [Pgm0 Pgm2 Pgm3 Pgm1]
             * The same logical ordering applies to stereo beds with objects. Note that this
             * stereo bed can also be the height channels from a 5.1.2 bed.
             */
            memcpy(&tmp, &arr[start+2], 2 * sizeof(int)); /* back up Pgm1 */
            memmove(&arr[start+2], &arr[start+4], 4 * sizeof(int)); /* slide Pgm2 and Pgm3 ahead */
            memcpy(&arr[start+6], &tmp, 2 * sizeof(int)); /* stick Pgm1 at the end of the stream */
        }
    }
}



static inline
unsigned int
power
    (unsigned int base
    ,unsigned int exp
    )
{
    unsigned int res = 1;
    while (exp)
    {
        if (exp & 1) res *= base;
        exp >>= 1;
        base *= base;
    }
    return res;
}


/**
 * @brief run ED2 stream description unit tests
 *
 * This mainly checks that we are correctly arranging streams according
 * to metadata.
 *
 * We simply add up to 4 beds (6 beds would take a very long time, 4^6
 * iterations) and loop, adding 0 - 64 - (channels-for-beds), and
 * check that we partition the streams.
 */

class ESDTest: public ::testing::TestWithParam<int> {};

TEST_P(ESDTest, arrangement_testing)
{
    unsigned int beds = GetParam();

    esd_test test;
    unsigned int objs;
    unsigned int combolimit = power(NUM_PMD_SPEAKER_CONFIGS, beds);
    unsigned int combo;

    for (combo = 0; combo != combolimit; ++combo)
    {
        unsigned int numchan;
        
        esd_test_init(&test);
        esd_test_add_beds(&test, beds, combo);
        numchan = test.channel_count;
        for (objs = 0; objs <= MAX_ESD_CHANNELS-numchan; ++objs)
        {
            dlb_ed2pmd_arrangement ed2arr;
            esd_test test2 = test;
            TestModel m;
            
            memset(test2.pcm_reorder, -1, sizeof(test2.pcm_reorder));
            
            esd_test_add_objs(&test2, objs);
            if (esd_test_gen_model(&test2, m))
            {
                ADD_FAILURE() << "Could not build test " << dlb_pmd_error(m);
                continue;
            }
            
            esd_test_reorder_for_encoder(&test2);
            if (!dlb_ed2pmd_arrange(m, &ed2arr))
            {
                ADD_FAILURE() << "Could not arrange ED2 " << dlb_pmd_error(m);
                continue;
            }
            
            /* simple comparison.... */
            if (ed2arr.num_streams != test2.expected_stream_count)
            {
                ADD_FAILURE() << "stream count not as expected";
                continue;
            }
            
            if (memcmp(test2.pcm_reorder, ed2arr.pcm_reorder,
                       ed2arr.total_channels * sizeof(int)))
            {
                ADD_FAILURE() << "PCM Reordering not as expected";
                continue;
            }
            
            if (memcmp(test2.stream_channel_counts, ed2arr.stream_channel_count,
                       ed2arr.num_streams * sizeof(unsigned int)))
            {
                ADD_FAILURE() << "Stream composition not as expected";
                continue;
            }
        }
    }
}



INSTANTIATE_TEST_CASE_P(PMD_ESD, ESDTest, testing::Range(0, MAX_ESD_TEST_BEDS+1));
