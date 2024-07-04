/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018-2019, Dolby Laboratories Inc.
 * Copyright (c) 2018-2019, Dolby International AB.
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
 * @file pmd_random_generator.c
 * @brief Generate a random model
 */

#include "pmd_error_helper.h"
#include "pmd_language.h"

#include "prng_kiss.h"
#include "dlb_pmd_generate.h"
#include "xml_hex.h"
#include "xml_uuid.h"
#include "xml_eidr.h"
#include "xml_ad_id.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
__pragma(warning(disable:4244))
#endif


/**
 * @def MAX_BITSET_WORD32
 * @brief upper limit on size of population we can randomly generate
 *
 * When generating a random population, we use bitmaps of 32-bit words.
 * This value specifies the maximum number of such words we will allow.
 */
#define MAX_BITSET_WORD32 (128)


/**
 * @def WORD32_BITS
 * @brief number of bits in a 32-bit word
 */
#define WORD32_BITS (32)


/**
 * @def MIN(a,b)
 * @brief return minimum of a and b
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))


/**
 * @brief table of speaker counts for PMD speaker configs
 */
static unsigned int SPEAKER_CONFIG_COUNT[NUM_PMD_SPEAKER_CONFIGS] =
{
    2, 3, 6, 8, 10, 12, 16, 2, 2
};


static unsigned int SPEAKER_CONFIG_CHANNELS[NUM_PMD_SPEAKER_CONFIGS][16] =
{
    /* 2.0 */     { 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    /* 3.0 */     { 1,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    /* 5.1 */     { 1,  2,  3,  4,  5,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    /* 5.1.2 */   { 1,  2,  3,  4,  5,  6, 11, 12,  0,  0,  0,  0,  0,  0,  0,  0 },
    /* 5.1.4 */   { 1,  2,  3,  4,  5,  6,  9, 10, 13, 14,  0,  0,  0,  0,  0,  0 },
    /* 7.1.4 */   { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 13, 14,  0,  0,  0,  0 },
    /* 9.1.6 */   { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16 },
};


/**
 * @brief working information in random generator
 */
typedef struct
{
    dlb_pmd_signal signals[DLB_PMD_MAX_SIGNALS];
    dlb_pmd_element_id bed_ids[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_element_id object_ids[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_element_id element_ids[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_element_id dynamic_object_ids[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_presentation_id presentation_ids[DLB_PMD_MAX_PRESENTATIONS];
    unsigned int eep_ids[DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS];
    unsigned int etd_ids[DLB_PMD_MAX_ED2_TURNAROUNDS];

    unsigned int num_signals;           /**< number of audio signals */
    unsigned int num_beds;              /**< number of beds */
    unsigned int num_objects;           /**< number of objects */
    unsigned int num_dynamic_objects;   /**< number objects which can be updated */
    unsigned int num_presentations;     /**< number of presentations */
    unsigned int num_eep;               /**< number of EAC3 encoding parameters */
    
    prng kiss;
    uint32_t binomial_coefficients[WORD32_BITS+1][WORD32_BITS+1];
    uint32_t bitset[MAX_BITSET_WORD32];
    unsigned int max_citizen;
    unsigned int next_citizen;
} generator;


#ifndef NDEBUG
/** @return the number of 1 bits */
static
unsigned int
pop_count
    (uint32_t a
    )
{
#if defined(__GNUC__)
    return (unsigned int) __builtin_popcount(a);
#elif defined (_MSC_VER)
    return __popcnt(a);
#else
    /* default implementation is lifted from the intrinsics DLB_Uones32 function */
    a = a - ((a >> 1u) & 0x55555555);
    a = (((a >> 2u) & 0x33333333) + (a & 0x33333333));
    a = (((a >> 4u) + a) & 0x0f0f0f0f);
    a += (a >> 8u);
    a += (a >> 16u);
    return a & 0x3f;
#endif
}
#endif

/**
 * @brief initialize generator helper struct
 */
static
void
generator_init
    (generator *g
    ,unsigned int *seed
    )
{
    int i;
    int j;

    memset(g, '\0', sizeof(*g));
    prng_init(&g->kiss);
    if (*seed)
    {
        prng_seed(&g->kiss, *seed);
    }
    else
    {
        *seed = g->kiss.x;
    }

    /* generate table of binomial_coefficients_ using Pascal's Triangle */
    memset(g->binomial_coefficients, 0, sizeof(g->binomial_coefficients));
    for (i = 0; i < WORD32_BITS+1; ++i)
    {
        g->binomial_coefficients[i][0] = 1;
        g->binomial_coefficients[i][i] = 1;
    }

    for (i = 1; i < WORD32_BITS+1; ++i)
    {
        g->binomial_coefficients[i][0] = 1;
        g->binomial_coefficients[i][i] = 1;
        for (j = 1; j < WORD32_BITS+1; ++j)
        {
            g->binomial_coefficients[i][j] =
                g->binomial_coefficients[i-1][j-1] +
                g->binomial_coefficients[i-1][j];
        }
    }
}


/**
 * @brief generate a pesudo-random unsigned integer between 0 and an
 * upper limit
 */
static
dlb_pmd_bool        /** @return random boolean */
generate_bool
    (generator *g   /**< [in] generator */
    )
{
    return prng_next(&g->kiss) & 1;
}


/**
 * @brief generate a pesudo-random unsigned integer between 0 and an
 * upper limit
 */
static
unsigned int               /** @return random unsigned int */
generate_uint
    (generator *g          /**< [in] generator */
    ,unsigned int limit    /**< [in] upper-bound (exclusive) */
    )
{
    return limit ? (prng_next(&g->kiss) % limit) : 0;
}


/**
 * @brief generate a pesudo-random float between upper and lower bounds
 */
static
float               /** @return pseudo-random float between min and max */
generate_float
    (generator *g   /**< [in] generator */
    ,float min      /**< [in] lower-bound (inclusive) */
    ,float max      /**< [in] upper-bound (inclusive) */
    )
{
    uint32_t rand = prng_next(&g->kiss);
    float range = max - min;
    float val  = min + ((float)rand / (float)(~0u)) * range;
    return val;
}


/**
 * @brief generate a pseudo-random gain
 */
static
float               /** @return pseudo-random gain */
generate_gain
    (generator *g   /**< [in] generator */
    )
{
    int x = (int)generate_uint(g, 0x40);
    if (0 == x) return -INFINITY;
    return (x - 0x33) * 0.5f;
}


/**
 * @brief generate a language
 */
static
void
generate_language
    (generator *g      /**< [in] generator */
    ,char (*lang)[4]   /**< [in/out] - place to store selected language */
    )
{
    unsigned int count = pmd_langcode_count();
    unsigned int rand = generate_uint(g, count);
    if (pmd_langcode_select(rand, lang))
    {
        abort();
    }
}


/**
 * @brief generate a random sequence of bytes
 */
static
void
generate_rand_bytes
    (generator *g
    ,uint8_t *array
    ,size_t size
    )
{
    unsigned int i;

    for (i = 0; i != size; ++i)
    {
        array[i] = generate_uint(g, 256);
    }
}


/**
 * @brief generate some random text
 */
static
void
generate_text
    (generator *g       /**< [in] generator */
    ,uint8_t   *s       /**< [in] array to populate */
    ,size_t     size    /**< [in] capacity of array */
    ,dlb_pmd_bool ascii /**< [in] restrict alphabet to ascii printable? */
    )
{
    if (ascii)
    {
        prng_ascii(&g->kiss, s, size);
    }
    else
    {
        prng_utf8(&g->kiss, s, size);
    }
}


/**
 * @brief generate a random word containing #num_bits bits.
 *
 * This is based on the idea of 'enumerative coding', in which you
 * order all the N-bit words containing exactly k-bits set in
 * ascending order.  The enumerative coding of the N-bit word is
 * just the position in that ascending order of that word.
 */ 
static
uint32_t
generate_random_bitset32
    (generator *g
    ,unsigned total_bits
    ,unsigned int num_bits
    )
{
    assert(num_bits <= total_bits);
    assert(total_bits <= WORD32_BITS);
    if (num_bits == WORD32_BITS)
    {
        return ~0u;
    }
    else if (num_bits == total_bits)
    {
        return (1u << total_bits)-1;
    }
    else if (num_bits == 0)
    {
        return 0;
    }
    else
    {
        uint32_t limit = g->binomial_coefficients[total_bits][num_bits]-1;
        uint32_t random = prng_next(&g->kiss) % limit;
        uint32_t result = 0;
        uint32_t bit;
        int count = num_bits;
        
        for (bit = total_bits-1; count > 0; --bit)
        {
            uint32_t bc = g->binomial_coefficients[bit][num_bits];
            if (random >= bc)
            {
                random -= bc;
                result |= 1u << bit;
                count -= 1;
            }
        }
        return result;
    }
}


/**
 * @brief for debugging, verify that the number of set bits is as we required
 */
static inline
void
VERIFY_POP_COUNT
    (uint32_t *bitset
    ,unsigned int num_words
    ,unsigned int target_count
    )
{
    (void)bitset;
    (void)num_words;
    (void)target_count;
#ifndef NDEBUG
    {
        unsigned int setbits = 0;
        unsigned int i;

        for (i = 0; i != num_words; ++i)
        {
            setbits += pop_count(bitset[i]);
        }
        assert(setbits == target_count);
    }
#endif
}


/**
 * @brief generate a random population of set number of members out of a maximum
 */
static
void
generate_random_population
    (generator *g          /**< generator */
    ,unsigned int maximum  /**< extent of array holding population */
    ,unsigned int count    /**< number of things to add to population */
    )
{
    /* note that we can only have 32 bits per word; 256 bits require 8 words.
     * we want to distribute num bits across each 8 words equally to simplify
     * computation.
     */
    unsigned int num_words = (maximum+WORD32_BITS-1) / WORD32_BITS;
    unsigned int last_word_bits;
    float avg_wordbits = (float)count / (float)num_words;
    unsigned int bits_per_word[MAX_BITSET_WORD32];
    unsigned int i;

    memset(bits_per_word, '\0', sizeof(bits_per_word));
    memset(g->bitset, '\0', sizeof(g->bitset));

    last_word_bits = maximum % WORD32_BITS;
    if (!last_word_bits)
    {
        last_word_bits = WORD32_BITS;
    }
    
    assert(count <= maximum);
    if (maximum < MAX_BITSET_WORD32 * WORD32_BITS)
    {
        /* step 1: distribute count equally across all words */
        float cumulative = 0.0f;
        for (i = 0; i != num_words; ++i)
        {
            float next = cumulative + avg_wordbits;
            bits_per_word[i] = floorf(next) - floorf(cumulative);
            cumulative = next;
        }

        /* step 1b: check for cumulative rounding errors, and correct */
        if (floorf(cumulative) != count)
        {
            assert(floorf(cumulative)+1 == count);
            for (i = 0; i != num_words-1; ++i)
            {
                if (bits_per_word[i] < WORD32_BITS)
                {
                    bits_per_word[i] += 1;
                    break;
                }
            }
            assert(i != num_words-1);
        }

        /* step 1c: check if we've exceeded the capacity of the last word
         * (which may not require a full 32-bits, and if so, redistribute the
         * excess bits)
         */
        if (bits_per_word[num_words-1] > last_word_bits)
        {
            unsigned int redistrib = bits_per_word[num_words-1] - last_word_bits;
            while (redistrib)
            {
                for (i = 0; i < num_words-1 && redistrib; ++i)
                {
                    if (bits_per_word[i] < WORD32_BITS)
                    {
                        bits_per_word[i] += 1;
                        bits_per_word[num_words-1] -= 1;
                        redistrib -= 1;
                    }
                }
            }
        }
        
        /* step 2: generate random bitsets for each word, requiring that each word
         * contains exactly the number of set bits we allocated previously */
        memset(g->bitset, '\0', sizeof(g->bitset));
        for (i = 0; i < num_words; ++i)
        {
            unsigned int total_bits = (i == num_words-1) ? last_word_bits : WORD32_BITS;
            g->bitset[i] = generate_random_bitset32(g, total_bits, bits_per_word[i]);
        }

        VERIFY_POP_COUNT(g->bitset, num_words, count);

        g->max_citizen = maximum;
        g->next_citizen = 0;
    }
}


/**
 * @brief get the index of the next set bit in the current population
 */
static
unsigned int             /** @return the ordinal of the next citizen of the population */
generate_next_citizen
    (generator *g        /**< [in] generator, after having generated a propulation */
    )
{
    unsigned int word;
    unsigned int bit;

    assert(g->next_citizen < MAX_BITSET_WORD32 * WORD32_BITS);

    while (g->next_citizen < MAX_BITSET_WORD32 * WORD32_BITS)
    {
        word = g->next_citizen / WORD32_BITS;
        bit  = g->next_citizen % WORD32_BITS;

        g->next_citizen += 1;
        assert(g->next_citizen <= g->max_citizen);
        if (g->bitset[word] & (1u << bit))
        {
            return (g->next_citizen - 1);
        }
    }
    /* shouldn't get here */
    abort();
}


/**
 * @brief randomly select the signal ids we want to use
 */
static
void
generate_signal_ids
    (generator *g
    ,unsigned int num
    )
{
    unsigned int i;

    generate_random_population(g, DLB_PMD_MAX_SIGNALS, num);
    for (i = 0; i < num; ++i)
    {
        g->signals[i] = generate_next_citizen(g)+1;
    }
}


/**
 * @brief randomly select the bed and object ids we want to use
 */
static
void
generate_element_ids
    (generator *g
    ,unsigned int num_beds
    ,unsigned int num_objects
    )
{
    unsigned int num_elements = num_beds + num_objects;
    unsigned int bc = 0;
    unsigned int oc = 0;
    unsigned int i;

    generate_random_population(g, DLB_PMD_MAX_AUDIO_ELEMENTS, num_elements);
    for (i = 0; i < num_elements; ++i)
    {
        unsigned int element_id = generate_next_citizen(g)+1;

        g->element_ids[i] = element_id;
        if (num_beds && num_objects)
        {
            if (prng_next(&g->kiss) & 1)
            {
                g->bed_ids[bc++] = element_id;
                g->num_beds += 1;
                num_beds -= 1;
            }
            else
            {
                g->object_ids[oc++] = element_id;
                g->num_objects += 1;
                num_objects -= 1;

                if (generate_bool(g))
                {
                    g->dynamic_object_ids[g->num_dynamic_objects] = element_id;
                    g->num_dynamic_objects += 1;
                }
            }
        }
        else if (num_beds)
        {
            g->bed_ids[bc++] = element_id;
            g->num_beds += 1;
            num_beds -= 1;
        }
        else
        {
            g->object_ids[oc++] = element_id;
            g->num_objects += 1;
            num_objects -= 1;
            if (generate_bool(g))
            {
                g->dynamic_object_ids[g->num_dynamic_objects] = element_id;
                g->num_dynamic_objects += 1;
            }
        }
    }
}
        

/**
 * @brief randomly select the presentation ids we want to use
 */
static
void
generate_presentation_ids
    (generator *g
    ,unsigned int num
    )
{
    unsigned int i;

    generate_random_population(g, DLB_PMD_MAX_PRESENTATIONS, num);
    for (i = 0; i < num; ++i)
    {
        g->presentation_ids[i] = generate_next_citizen(g)+1;
    }
    g->num_presentations = num;
}


/**
 * @brief randomly select the EAC3 encoder parameter ids we want to use
 */
static
void
generate_eep_ids
    (generator *g
    ,unsigned int num
    )
{
    unsigned int i;

    generate_random_population(g, DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS, num);
    for (i = 0; i < num; ++i)
    {
        g->eep_ids[i] = generate_next_citizen(g)+1;
    }
    g->num_eep = num;
}


/**
 * @brief randomly select the ED2 turnaround ids we want to use
 */
static
void
generate_etd_ids
    (generator *g
    ,unsigned int num
    )
{
    unsigned int i;

    generate_random_population(g, DLB_PMD_MAX_ED2_TURNAROUNDS, num);
    for (i = 0; i < num; ++i)
    {
        g->etd_ids[i] = generate_next_citizen(g)+1;
    }
}


/**
 * @brief randomly select a bed element
 */
static inline
dlb_pmd_element_id
generate_bed_selection
    (generator *g
    ,dlb_pmd_bed_type type
    ,unsigned int num_beds
    )
{
    if (PMD_BED_DERIVED == type)
    {
        unsigned int choice = generate_uint(g, num_beds);
        return (dlb_pmd_element_id)g->bed_ids[choice];
    }
    return 0;
}


/**
 * @brief randomly select a signal
 */
static inline
dlb_pmd_signal
generate_signal_selection
    (generator *g
    )
{
    unsigned int choice = generate_uint(g, g->num_signals);
    return (dlb_pmd_signal)g->signals[choice];
}

/**
 * @brief randomly select a presentation
 */
static inline
dlb_pmd_presentation_id
generate_presentation_selection
    (generator *g
    )
{
    unsigned int choice = generate_uint(g, g->num_presentations);
    return (dlb_pmd_presentation_id)g->presentation_ids[choice];
}


/**
 * @brief randomly select an EAC3 Encoding Parameters selection
 */
static inline
uint16_t
generate_eac3_selection
    (generator *g
    )
{
    unsigned int choice = generate_uint(g, g->num_eep);
    return (uint16_t)g->eep_ids[choice];
}


/**
 * @brief add the randomly generated list of signal ids to the model
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_sigs
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    unsigned int i;

    for (i = 0; i < count; ++i)
    {
        if (dlb_pmd_add_signal(model, g->signals[i]))
        {
            return 1;
        }
    }
    g->num_signals = count;
    return 0;
}


/**
 * @brief return number of speakers in a speaker config
 *
 * used when determining the sources available for a derived bed.
 */
static
unsigned int
num_speakers
    (dlb_pmd_speaker_config cfg
    )
{
    unsigned int num = 0;

    switch (cfg)
    {
        case DLB_PMD_SPEAKER_CONFIG_PORTABLE:
        case DLB_PMD_SPEAKER_CONFIG_HEADPHONE:
        case DLB_PMD_SPEAKER_CONFIG_2_0:       num = 2;     break;
        case DLB_PMD_SPEAKER_CONFIG_3_0:       num = 3;     break;
        case DLB_PMD_SPEAKER_CONFIG_5_1:       num = 6;     break;
        case DLB_PMD_SPEAKER_CONFIG_5_1_2:     num = 8;     break;
        case DLB_PMD_SPEAKER_CONFIG_5_1_4:     num = 10;    break;
        case DLB_PMD_SPEAKER_CONFIG_7_1_4:     num = 12;    break;
        case DLB_PMD_SPEAKER_CONFIG_9_1_6:     num = 16;    break;
        default: abort();
    }

    return num;
}


/**
 * @brief genearate the required random beds and add to the model
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_beds
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    ,dlb_pmd_bool ascii
    ,dlb_pmd_bool sadm
    )
{
    dlb_pmd_source sources[DLB_PMD_MAX_BED_SOURCES];
    unsigned int source_limit;
    dlb_pmd_bed bed;

    unsigned int i;
    unsigned int j;

    for (i = 0; i < count; ++i)
    {
        source_limit = PMD_NUM_SPEAKERS;
        memset(sources, '\0', sizeof(sources));
        memset(&bed, '\0', sizeof(bed));

        bed.id     = g->bed_ids[i];
        bed.sources = sources;
        if (sadm)
        {
            bed.config = (dlb_pmd_speaker_config)generate_uint(g, DLB_PMD_SPEAKER_CONFIG_9_1_6);
            bed.num_sources = SPEAKER_CONFIG_COUNT[bed.config];
            bed.bed_type = PMD_BED_ORIGINAL;
            bed.source_id = 0;

            for (j = 0; j < bed.num_sources; ++j)
            {
                bed.sources[j].target = (dlb_pmd_speaker)SPEAKER_CONFIG_CHANNELS[bed.config][j];
                bed.sources[j].source = generate_signal_selection(g);            
                bed.sources[j].gain   = 0.0f;  /* no bed gains in dlb-sADM */
            }
        }
        else
        {
            bed.config = (dlb_pmd_speaker_config)generate_uint(g, NUM_PMD_SPEAKER_CONFIGS);
            bed.bed_type    = (dlb_pmd_bed_type)(i > 0 && generate_bool(g));
            bed.source_id   = generate_bed_selection(g, bed.bed_type, i);
            bed.num_sources = 1 + generate_uint(g, DLB_PMD_MAX_BED_SOURCES);

            if (PMD_BED_DERIVED == bed.bed_type)
            {
                dlb_pmd_bed bedorg;
                dlb_pmd_bed_lookup(model, bed.source_id, &bedorg, DLB_PMD_MAX_BED_SOURCES, sources);
                source_limit = num_speakers(bedorg.config);
            }

            for (j = 0; j < bed.num_sources; ++j)
            {
                bed.sources[j].target = (dlb_pmd_speaker)(1 + generate_uint(g, source_limit-1));
                bed.sources[j].source = generate_signal_selection(g);            
                bed.sources[j].gain   = generate_gain(g);
            }
        }

        generate_text(g, (uint8_t*)bed.name, DLB_PMD_MAX_NAME_LENGTH, ascii);
        if (dlb_pmd_set_bed(model, &bed))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief genearate the required random objects and add to the model
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_objs
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    ,dlb_pmd_bool ascii
    ,dlb_pmd_bool sadm
    )
{
    dlb_pmd_element_id *dynobjs = g->dynamic_object_ids;
    dlb_pmd_object obj;
    unsigned int i;

    for (i = 0; i < count; ++i)
    {
        memset(&obj, '\0', sizeof(obj));

        obj.id              = g->object_ids[i];
        obj.x               = generate_float(g, -1.0f, 1.0f);
        obj.y               = generate_float(g, -1.0f, 1.0f);
        obj.z               = generate_float(g, -1.0f, 1.0f);
        obj.source          = generate_signal_selection(g);
        obj.source_gain     = generate_gain(g);

        if (sadm)
        {
            obj.object_class    = generate_uint(g, (unsigned int)PMD_CLASS_EMERGENCY_INFO);
            obj.size            = 0.0f;
            obj.size_3d         = 0;
            obj.diverge         = 0;
            obj.dynamic_updates = 0;
        }
        else
        {
            obj.object_class    = generate_uint(g, (unsigned int)PMD_CLASS_RESERVED);
            obj.size            = generate_float(g,  0.0f, 1.0f);
            obj.size_3d         = generate_bool(g);
            obj.diverge         = generate_bool(g);

            /* has this already been tagged as a dynamic object? */
            while (*dynobjs && *dynobjs > obj.id)
            {
                ++dynobjs;
            }
            obj.dynamic_updates = (*dynobjs == obj.id);
        }
        generate_text(g, (uint8_t*)obj.name, DLB_PMD_MAX_NAME_LENGTH, ascii);
        if (dlb_pmd_set_object(model, &obj))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief trim the presentation to meet requirements for sADM
 *
 * In Dolby Serial ADM, we restict presentations to consist of exactly
 * one bed, and then set the presentation's config to match that of
 * that bed.
 */
static
dlb_pmd_success                  /** @return 0 on success, 1 on failure */
prune_sadm_presentation
    (dlb_pmd_model *model        /**< [in] PMD model */
    ,dlb_pmd_presentation *pres  /**< [in] newly generated presentation to prune */
    )
{
    dlb_pmd_source sources[DLB_PMD_MAX_BED_SOURCES];
    dlb_pmd_bool found_sadm_bed = 0; 
    dlb_pmd_bed bed;
    unsigned int count = pres->num_elements;
    unsigned int i;
    unsigned int elements_to_move;

    for (i = 0; i < count; ++i)
    {
        if (!dlb_pmd_bed_lookup(model, pres->elements[i], &bed, DLB_PMD_MAX_BED_SOURCES, sources))
        {
            if (found_sadm_bed)
            {
                /* throw this one away: set it to 0 for cleanup later */
                pres->elements[i] = 0;
                pres->num_elements -= 1;
            }
            else
            {
                found_sadm_bed = 1;
                pres->config = bed.config;
            }
        }
    }

    if (!found_sadm_bed)
    {
        /* This was an object-only presentation anyway, which we don't support.
         * Don't add it to the model */
        return PMD_FAIL;
    }
    else
    {
        /* working backwards from the end, remove the empty spaces */
        elements_to_move = (pres->elements[count-1] > 0);
        for (i = count-2; i < ~0u; --i)
        {
            if ((pres->elements[i] == 0) && elements_to_move)
            {
                memmove(&pres->elements[i], &pres->elements[i+1],
                        sizeof(pres->elements[i]) * elements_to_move);
            }
            else
            {
                elements_to_move += 1;
            }
        }
        return PMD_SUCCESS;
    }
}


/**
 * @brief genearate the required random presentations and add to the model
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_pres
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    ,dlb_pmd_bool ascii
    ,dlb_pmd_bool sadm
    )
{
    dlb_pmd_element_id elements[DLB_PMD_MAX_PRESENTATION_ELEMENTS];
    dlb_pmd_presentation presentation;
    unsigned int max_names = model->limits.max_presentation_names;
    unsigned int total_elements = g->num_beds + g->num_objects;
    unsigned int i;
    unsigned int j;

    /* each presentation must have at least one name */
    assert(max_names >= count);
    max_names -= count;  /* reserve one name per presentation */
    for (i = 0; i < count; ++i)
    {
        unsigned int element_limit = g->num_beds + g->num_objects;
        unsigned int name_limit;
        
        if (element_limit > DLB_PMD_MAX_PRESENTATION_ELEMENTS)
        {
            element_limit = DLB_PMD_MAX_PRESENTATION_ELEMENTS;
        }

        memset(&presentation, '\0', sizeof(presentation));
        memset(elements, '\0', sizeof(elements));
        
        presentation.id           = g->presentation_ids[i];
        presentation.num_elements = 1 + generate_uint(g, element_limit-1);
        presentation.elements     = elements;
        presentation.num_names    = 1;

        presentation.config = (dlb_pmd_speaker_config)generate_uint(g, NUM_PMD_SPEAKER_CONFIGS);

        name_limit = DLB_PMD_MAX_PRESENTATION_NAMES;
        if (name_limit > max_names)
        {
            name_limit = max_names;
        }
        if (name_limit)
        {
            presentation.num_names    = 1 + generate_uint(g, name_limit-1);
            max_names -= (presentation.num_names - 1);
        }

        generate_language(g, &presentation.audio_language);
        generate_random_population(g, total_elements, presentation.num_elements);
        for (j = 0; j < presentation.num_elements; ++j)
        {
            unsigned int citizen = generate_next_citizen(g);
            assert(citizen < total_elements);
            presentation.elements[j] = g->element_ids[citizen];
        }

        /* make sure we generate unique names for the languages */
        generate_random_population(g, pmd_langcode_count(), presentation.num_names);
        for (j = 0; j < presentation.num_names; ++j)
        {
            unsigned int citizen = generate_next_citizen(g);
            if (pmd_langcode_select(citizen, &presentation.names[j].language))
            {
                abort();
            }
            generate_text(g, (uint8_t*)presentation.names[j].text, DLB_PMD_MAX_NAME_LENGTH, ascii);
        }

        if (!sadm || !prune_sadm_presentation(model, &presentation))
        {
            if (dlb_pmd_set_presentation(model, &presentation))
            {
                return PMD_FAIL;
            }
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief generate a LUFS value
 */
static inline
float
generate_lufs
    (generator *g
    )
{
    return generate_float(g, DLB_PMD_LUFS_MIN, DLB_PMD_LUFS_MAX);
}


/**
 * @brief generate a LU value
 */
static inline
float
generate_lu
    (generator *g
    )
{
    return generate_float(g, DLB_PMD_LU_MIN, DLB_PMD_LU_MAX);
}


/**
 * @brief generate pseudo-random presentation loudness information
 *
 * We can only have one loudness per presentation. 
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_pld
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    dlb_pmd_loudness pld;
    unsigned int i;

    generate_random_population(g, g->num_presentations, count);

    for (i = 0; i != count; ++i)
    {
        unsigned int citizen = generate_next_citizen(g);

        memset(&pld, '\0', sizeof(pld));
    
        pld.presid            = g->presentation_ids[citizen];
        pld.loud_prac_type    = (dlb_pmd_loudness_practice)generate_uint(g,16);
        pld.b_loudcorr_gating = generate_bool(g);
        pld.loudcorr_gating   = (dlb_pmd_dialgate_practice)generate_uint(g,4);
        pld.loudcorr_type     = (dlb_pmd_correction_type)generate_uint(g,2);
        pld.b_loudrelgat      = generate_bool(g);
        pld.loudrelgat        = generate_lufs(g);
        pld.b_loudspchgat     = generate_bool(g);
        pld.loudspchgat       = generate_lufs(g);
        pld.loudspch_gating   = (dlb_pmd_dialgate_practice)generate_uint(g,4);
        pld.b_loudstrm3s      = generate_bool(g);
        pld.loudstrm3s        = generate_lufs(g);
        pld.b_max_loudstrm3s  = generate_bool(g);
        pld.max_loudstrm3s    = generate_lufs(g);
        pld.b_truepk          = generate_bool(g);
        pld.truepk            = generate_lufs(g);
        pld.b_max_truepk      = generate_bool(g);
        pld.max_truepk        = generate_lufs(g);

        pld.b_prgmbndy        = generate_bool(g);
        pld.prgmbndy          = generate_uint(g, 9) + 1;
        pld.b_prgmbndy_offset = generate_bool(g) && pld.b_prgmbndy;
        pld.prgmbndy_offset   = generate_uint(g, 2048);
        pld.b_lra             = generate_bool(g);
        pld.lra               = generate_lu(g);
        pld.lra_prac_type     = (dlb_pmd_loudness_range_practice)generate_uint(g,2);
        pld.b_loudmntry       = generate_bool(g);
        pld.loudmntry         = generate_lufs(g);
        pld.b_max_loudmntry   = generate_bool(g);
        pld.max_loudmntry     = generate_lufs(g);
        
        pld.extension.size    = generate_uint(g, PMD_EXTENSION_MAX_BYTES * 8);
        generate_rand_bytes(g, pld.extension.data, sizeof(pld.extension.data));
        
        if (dlb_pmd_set_loudness(model, &pld))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief generate pseudo-random EAC3 encoding parameters information
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_eep
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    unsigned int presentation_limit;
    dlb_pmd_eac3 eac3;
    unsigned int i;
    unsigned int j;

    presentation_limit = PMD_EEP_MAX_PRESENTATIONS;
    if (presentation_limit > g->num_presentations)
    {
        presentation_limit = g->num_presentations;
    }

    for (i = 0; i != count; ++i)
    {
        memset(&eac3, '\0', sizeof(eac3));

        eac3.id = g->eep_ids[i];

        if (generate_bool(g))
        {
            eac3.b_encoder_params = 1;
            eac3.dynrng_prof = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
            eac3.compr_prof  = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
            eac3.surround90  = generate_bool(g);
            eac3.hmixlev     = (dlb_pmd_hmixlev)generate_uint(g, 31);
        }

        if (generate_bool(g))
        {
            eac3.b_bitstream_params = 1;
            eac3.bsmod         = (dlb_pmd_bsmod)generate_uint(g, PMD_NUM_BSMOD);
            eac3.dsurmod       = (dlb_pmd_surmod)generate_uint(g, PMD_NUM_DSURMOD);
            eac3.dialnorm      = (dlb_pmd_dialnorm)generate_uint(g, DLB_PMD_MAX_DIALNORM-1)+1;
            eac3.dmixmod       = (dlb_pmd_prefdmix)generate_uint(g, PMD_NUM_PREFDMIX);
            eac3.ltrtcmixlev   = (dlb_pmd_cmixlev)generate_uint(g, PMD_NUM_CMIX_LEVEL);
            eac3.ltrtsurmixlev = (dlb_pmd_surmixlev)generate_uint(g, PMD_NUM_SURMIX_LEVEL);
            eac3.lorocmixlev   = (dlb_pmd_cmixlev)generate_uint(g, PMD_NUM_CMIX_LEVEL);
            eac3.lorosurmixlev = (dlb_pmd_surmixlev)generate_uint(g, PMD_NUM_SURMIX_LEVEL);
        }

        if (generate_bool(g))
        {
            eac3.b_drc_params    = 1;
            eac3.drc_port_spkr   = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
            eac3.drc_port_hphone = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
            eac3.drc_flat_panl   = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
            eac3.drc_home_thtr   = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
            eac3.drc_ddplus      = (dlb_pmd_compr)generate_uint(g, PMD_NUM_COMPR);
        }
        
        eac3.num_presentations = generate_uint(g, presentation_limit);
        if (eac3.num_presentations)
        {
            generate_random_population(g, presentation_limit, eac3.num_presentations);
            for (j = 0; j != eac3.num_presentations; ++j)
            {
                unsigned int citizen = generate_next_citizen(g);
                eac3.presentations[j] = g->presentation_ids[citizen];
            }
        }
        
        if (dlb_pmd_set_eac3(model, &eac3))
        {
            return PMD_FAIL;
        }
    }
    return PMD_SUCCESS;
}


/**
 * @brief generate pseudo-random ED2 turnarounds
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_etd
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    dlb_pmd_ed2_turnaround etd;
    unsigned int limit = g->num_eep * g->num_presentations;
    unsigned int i;
    unsigned int j;

    if (limit > PMD_ETD_MAX_PRESENTATIONS)
    {
        limit = PMD_ETD_MAX_PRESENTATIONS;
    }

    for (i = 0; i != count; ++i)
    {
        memset(&etd, '\0', sizeof(etd));
        
        etd.id = g->etd_ids[i];

        etd.ed2_presentations = generate_uint(g, limit+1);
        etd.ed2_framerate     = (dlb_pmd_frame_rate)generate_uint(g, DLB_PMD_FRAMERATE_3000+1);
        for (j = 0; j != etd.ed2_presentations; ++j)
        {
            etd.ed2_turnarounds[j].presid = generate_presentation_selection(g);
            etd.ed2_turnarounds[j].eepid  =  generate_eac3_selection(g);
        }

        etd.de_presentations  = generate_uint(g, limit+1);
        etd.de_framerate      = (dlb_pmd_frame_rate)generate_uint(g, DLB_PMD_FRAMERATE_3000+1);
        etd.pgm_config        = (dlb_pmd_de_program_config)generate_uint(g, PMD_DE_PGMCFG_71S+1);
        for (j = 0; j != etd.de_presentations; ++j)
        {
            etd.de_turnarounds[j].presid = generate_presentation_selection(g);
            etd.de_turnarounds[j].eepid  = generate_eac3_selection(g);
        }

        if (dlb_pmd_set_ed2_turnaround(model, &etd))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief generate pseudo-random updates
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_xyz
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    dlb_pmd_update update;
    unsigned int population_max;
    unsigned int citizen;
    unsigned int obj;
    unsigned int time;
    unsigned int i;

    if (g->num_dynamic_objects > 0)
    {
        /* we want to randomly distribute the updates across two dimensions:
         * the set of objects, and the set of times, 0 - 63 (times 32)
         */
        population_max = g->num_dynamic_objects * 64;
        generate_random_population(g, population_max, count);
        
        for (i = 0; i != count; ++i)
        {
            memset(&update, '\0', sizeof(update));
            
            citizen = generate_next_citizen(g);
            obj = citizen / 64;
            time = citizen % 64;
            
            update.sample_offset = 32 * time;
            update.id = (dlb_pmd_element_id)g->dynamic_object_ids[obj];
            update.x = generate_float(g, -1.0f, 1.0f);
            update.y = generate_float(g, -1.0f, 1.0f);
            update.z = generate_float(g, -1.0f, 1.0f);
            
            if (dlb_pmd_set_update(model, &update))
            {
                return 1;
            }
        }
    }
    return 0;
}


/**
 * @brief helper function to generate an IAT's content identifier
 */
static
void
generate_iat_content_id
    (generator *g
    ,dlb_pmd_content_id *cid
    )
{
    char tmp[128];

    cid->type = (dlb_pmd_content_id_type)generate_uint(g, 4);
    switch (cid->type)
    {
        case PMD_IAT_CONTENT_ID_UUID:
            (void)snprintf(tmp, sizeof(tmp),
                     "%08x-%04x-%04x-%04x-%012x",
                     generate_uint(g, ~0u),
                     generate_uint(g, 0xffff),
                     generate_uint(g, 0xffff),
                     generate_uint(g, 0xffff),
                     generate_uint(g, ~0u));
            (void)read_uuid(tmp, cid->data);
            cid->size = 16;
            break;

        case PMD_IAT_CONTENT_ID_EIDR:
            (void)snprintf(tmp, sizeof(tmp),
                     "10.5240/%04x-%04x-%04x-%04x-%04x",
                     generate_uint(g, 0xffff),
                     generate_uint(g, 0xffff),
                     generate_uint(g, 0xffff),
                     generate_uint(g, 0xffff),
                     generate_uint(g, 0xffff));
            (void)read_eidr(tmp, cid->data);
            cid->size = 12;
            break;
            
        case PMD_IAT_CONTENT_ID_AD_ID:
        default:
            (void)snprintf(tmp, sizeof(tmp), "a1b2c3d4e5f");
            (void)read_ad_id(tmp, cid->data);
            cid->size = 11;
            break;
    }
}


/**
 * @brief helper function to generate an IAT's distribution identifier
 */
static
void
generate_iat_distribution_id
    (generator *g
    ,dlb_pmd_distribution_id *did
    )
{
    uint16_t bsid  = generate_uint(g, 0xffff);
    uint16_t majno = generate_uint(g, 1024);
    uint16_t minno = generate_uint(g, 1024);

    did->type = PMD_IAT_DISTRIBUTION_ID_ATSC3;
    did->size = 5;

    did->data[0] = (bsid >> 8) & 0xff;
    did->data[1] = bsid & 0xff;
    did->data[2] = 0xf0 | ((majno >> 6) & 0x0f);
    did->data[3] = ((majno & 0x3f) << 2) | ((minno >> 8) & 0x3);
    did->data[4] = (minno & 0xff);
}


/**
 * @brief generate pseudo-random IAT
 */
static
dlb_pmd_success             /** @return 0 on success, 1 on failure */
generate_iat
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    dlb_pmd_identity_and_timing iat;
    unsigned int i;
    
    for (i = 0; i < count; ++i)
    {
        memset(&iat, '\0', sizeof(iat));

        generate_iat_content_id(g, &iat.content_id);
        generate_iat_distribution_id(g, &iat.distribution_id);
        
        iat.timestamp                 = generate_uint(g, 2048);
        iat.offset.present            = generate_bool(g);
        iat.offset.offset             = generate_uint(g, 2048);
        iat.validity_duration.present = generate_bool(g);
        iat.validity_duration.vdur    = generate_uint(g, 2048);

        iat.user_data.size = generate_uint(g, sizeof(iat.user_data.data));
        generate_rand_bytes(g, iat.user_data.data, iat.user_data.size);

        iat.extension.size = generate_uint(g, sizeof(iat.extension.data));
        generate_rand_bytes(g, iat.extension.data, iat.extension.size);
        iat.extension.size *= 8; /* it's a bitcount, not a byte count */

        if (dlb_pmd_set_iat(model, &iat))
        {
            return 1;
        }
    }

    return 0;
}


/**
 * @brief generate pseudo-random Headphone Element descriptions
 */
static
dlb_pmd_success
generate_hed
    (generator *g
    ,dlb_pmd_model *model
    ,unsigned int count
    )
{
    dlb_pmd_headphone hed;
    unsigned int i;

    generate_random_population(g, g->num_beds + g->num_objects, count);
    for (i = 0; i < count; ++i)
    {
        unsigned int citizen;
        
        memset(&hed, '\0', sizeof(hed));

        citizen = generate_next_citizen(g);
        hed.audio_element_id      = (dlb_pmd_element_id)g->element_ids[citizen];
        hed.head_tracking_enabled = generate_bool(g);
        hed.render_mode           = generate_uint(g, 128);
        hed.channel_mask          = generate_uint(g, (1u << (PMD_NUM_SPEAKERS-1))-1);

        if (dlb_pmd_set_headphone_element(model, &hed))
        {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief check that a given entity count constraint is acceptable
 */
static
void
check_count
    (prng *p
    ,unsigned int *count
    ,unsigned int min
    ,unsigned int max
    )
{
    assert(min <= max);

    if (*count > max)
    {
        if (*count == PMD_GENERATE_RANDOM_NUMBER)
        {
            *count = min + prng_next(p) % (max-min+1);
        }
        else
        {
            *count = max - min;
        }
    }
    else if (*count < min)
    {
        *count = min;
    }
}


dlb_pmd_success
dlb_pmd_generate_random
    (dlb_pmd_model          *model
    ,dlb_pmd_metadata_count *countsin
    ,unsigned int            seed
    ,dlb_pmd_bool            ascii_strings
    ,dlb_pmd_bool            sadm
    )
{
    dlb_pmd_metadata_count *limits;
    dlb_pmd_metadata_count counts;    
    dlb_pmd_success result;
    unsigned int entity_limit;
    generator *g;
    char title[128];

    FUNCTION_PROLOGUE(model);
    CHECK_PTRARG(model, countsin);
    
    counts = *countsin;
    if (sadm)
    {
        /* restrict random counts to beds, objects, presentations only */
        counts.num_loudness = 0;
        counts.num_iat = 0;
        counts.num_eac3 = 0;
        counts.num_ed2_turnarounds = 0;
        counts.num_headphone_desc = 0;
    }

    g = malloc(sizeof(generator));
    generator_init(g, &seed);

    snprintf(title, sizeof(title), "Randomly generated from seed %u", seed);

    limits = &model->limits.max;

    check_count(&g->kiss, &counts.num_signals,         1, limits->num_signals);

    entity_limit = MIN(model->limits.max_elements, limits->num_beds);
    check_count(&g->kiss, &counts.num_beds,            0, entity_limit);

    entity_limit = MIN(model->limits.max_elements - counts.num_beds, limits->num_objects);
    check_count(&g->kiss, &counts.num_objects,         0, entity_limit);
    if (0 == counts.num_beds && 0 == counts.num_objects)
    {
        if (generate_bool(g))
        {
            counts.num_beds = 1;
        }
        else
        {
            counts.num_objects = 1;
        }
    }

    generate_signal_ids(g, counts.num_signals);
    generate_element_ids(g, counts.num_beds, counts.num_objects);

    entity_limit = MIN(g->num_dynamic_objects * 64, limits->num_updates);
    check_count(&g->kiss, &counts.num_updates,         0, entity_limit);
    check_count(&g->kiss, &counts.num_presentations,   1, limits->num_presentations);

    entity_limit = MIN(counts.num_presentations, limits->num_loudness);
    check_count(&g->kiss, &counts.num_loudness,        0, entity_limit);
    check_count(&g->kiss, &counts.num_eac3,            0, limits->num_eac3);
    check_count(&g->kiss, &counts.num_ed2_turnarounds, 0, limits->num_ed2_turnarounds);
    check_count(&g->kiss, &counts.num_iat,             0, limits->num_iat);

    entity_limit = MIN(counts.num_beds + counts.num_objects, limits->num_headphone_desc);
    check_count(&g->kiss, &counts.num_headphone_desc,  0, entity_limit);

    generate_presentation_ids(g, counts.num_presentations);
    generate_eep_ids(g, counts.num_eac3);
    generate_etd_ids(g, counts.num_ed2_turnarounds);

    result = dlb_pmd_set_title(model, title)    
        || generate_sigs(g, model, counts.num_signals)
        || generate_beds(g, model, counts.num_beds, ascii_strings, sadm)
        || generate_objs(g, model, counts.num_objects, ascii_strings, sadm)
        || generate_pres(g, model, counts.num_presentations, ascii_strings, sadm)
        || generate_pld (g, model, counts.num_loudness)
        || generate_eep (g, model, counts.num_eac3)
        || generate_etd (g, model, counts.num_ed2_turnarounds)
        || generate_xyz (g, model, counts.num_updates)
        || generate_hed (g, model, counts.num_headphone_desc)
        || generate_iat (g, model, counts.num_iat)
        ;

    free(g);
    return result;
}

