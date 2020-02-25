/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#ifndef PMD_APD_H
#define PMD_APD_H

#include <assert.h>
#include "pmd_abd_aod.h"

/**
 * @file pmd_apd.h
 * @brief implementation of Audio Presentation Description (APD) data structure
 */


/**
 * @def WORD_BITS (32)
 * @brief symbolic constant noting that there are 32 bits in a uint32_t
 */
#define WORD_BITS (32)


/**
 * @brief datatype representing the list of stored signals
 */
typedef struct
{
    uint32_t bitmap[(MAX_AUDIO_ELEMENTS+WORD_BITS-1) / WORD_BITS];
} pmd_elements;


/**
 * @brief initialize signals datatype
 */
static inline
void
pmd_elements_init
    (pmd_elements *elements
    )
{
    memset(elements->bitmap, '\0', sizeof(elements->bitmap));
}


/**
 * @brief copy elements
 */
static inline
void
pmd_elements_copy
    (pmd_elements *dest
    ,pmd_elements *src
    )
{
    memcpy(dest, src, sizeof(src->bitmap));
}


/**
 * @brief add an element to the list of known elements
 */
static inline
void
pmd_elements_add
    (pmd_elements *elements
    ,unsigned int id
    )
{
    elements->bitmap[(id)/WORD_BITS] |= (1<<((id) & (WORD_BITS-1)));
}


/**
 * @brief remove an element from the list of known elements
 */
static inline
void
pmd_elements_remove
    (pmd_elements *elements
    ,unsigned int id
    )
{
    elements->bitmap[(id)/WORD_BITS] &= ~(1<<((id) & (WORD_BITS-1)));
}


/**
 * @brief check whether or not a given signal is in the list of known signals
 */
static inline
pmd_bool
pmd_elements_test
    (pmd_elements *elements
    ,unsigned int id
    )
{
    return (elements->bitmap[(id)/WORD_BITS] & (1<<((id) & (WORD_BITS-1)))) != 0;
}


/**
 * @brief type of PMD presentation identifier
 *
 * Like PMD audio elements, PMD presentations are identified using an
 * integer, in the range 1 - 511
 */
typedef uint16_t pmd_presentation_id;


/**
 * @def PMD_MAX_PRESNAME_LEN
 * @brief maximum length of a presentation name
 */
#define PMD_MAX_PRESNAME_LEN (64)


/**
 * @brief type of PMD presentation
 *
 * In PMD, an Audio Presentation, or Audio Preset, is a sub-selection
 * of all available audio program components belonging to one PMD
 * audio program.  A presentation can be considered the "Next
 * Generation Audio" equivalent of an 'audio service' in legacy
 * systems, which each utilized complete mixes (e.g., 'SAP', or
 * 'VDS').
 *
 * The various fields refer to the indices within the model's
 * audio element array.
 */
typedef struct
{
    pmd_langcode pres_lang;           /**< language of audio in output presentation */

    pmd_presentation_id id;           /**< unique identifier */
    dlb_pmd_speaker_config config;    /**< output speaker configuration */
    unsigned int num_elements;        /**< number of elements in presentation */
    pmd_elements elements;            /**< bitmap of PMD elements present */
    unsigned int num_names;           /**< number of names for presentation */
    uint16_t     names[DLB_PMD_MAX_PRESENTATION_NAMES]; /**< indices of global presentation name table */
} pmd_apd;


/**
 * @brief helper struct to iterate through elements in a presentation
 */
typedef struct
{
    uint32_t word;                /**< copy of current word */
    unsigned int wi;              /**< word index in the element array */
    unsigned int bi;              /**< bit index in word */
    unsigned int found;           /**< number of elements already found */
    const pmd_apd *pres;          /**< presentation being iterated */
} pmd_apd_iterator;
    

/**
 * @brief initialize presentation element iterator helper
 */
static inline
void
pmd_apd_iterator_init
    (pmd_apd_iterator *pi /**< [out] structure to init */
    ,const pmd_apd *p     /**< [in] presentation to iterate */
    )
{
    pi->word = p->elements.bitmap[0];
    pi->pres = p;
    pi->wi = 0;
    pi->bi = 0;
    pi->found = 0;
}


/**
 * @brief return index of next element in the list, if any
 */
static inline
pmd_bool                           /** @return 1 if element exists, 0 otherwise */
pmd_apd_iterator_next
    (pmd_apd_iterator *pi          /**< [in] iterator structure */
    ,unsigned int *idx             /**< [out] next element index */
    )
{
    unsigned int word;

    if (pi->found < pi->pres->num_elements)
    {
        word = pi->word;
        if (!word)
        {
            const uint32_t *elements = &pi->pres->elements.bitmap[pi->wi];
            do
            {
                ++pi->wi;
                ++elements;
                word = *elements;
            }
            while (!word);
            pi->bi = 0;
            assert(pi->wi < sizeof(pi->pres->elements)/sizeof(uint32_t));
        }

        while (!(word&1))
        {
            pi->bi += 1;
            word = word >> 1;
        }

        *idx = (pi->wi * 32) + pi->bi;
        pi->word = word >> 1;
        pi->bi += 1;
        pi->found += 1;
        return 1;
    }
    return 0;
}


/**
 * @brief finish iterator
 */
static inline
void
pmd_apd_iterator_finish
    (pmd_apd_iterator *pi /**< [in] iterator structure */
    )
{
    pi->pres = NULL;
    pi->word = 0;
    pi->wi = 0;
    pi->bi = 0;
}


#endif /* PMD_APD_H */
