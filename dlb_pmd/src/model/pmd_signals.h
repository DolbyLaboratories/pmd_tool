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

#ifndef PMD_SIGNALS_INC_
#define PMD_SIGNALS_INC_

#include <string.h>
#include "pmd_types.h"

/**
 * @def MAX_AUDIO_SIGNALS
 * @brief upper limit on number of audio signals we can specify
 */
#define MAX_AUDIO_SIGNALS (256)


/**
 * @brief datatype representing the list of stored signals
 */
typedef struct
{
    uint8_t sigbitmap[MAX_AUDIO_SIGNALS / 8];
} pmd_signals;


/**
 * @brief initialize signals datatype
 */
static inline
void
pmd_signals_init
    (pmd_signals *signals
    )
{
    memset(signals->sigbitmap, '\0', sizeof(signals->sigbitmap));
}


/**
 * @brief copy signals
 */
static inline
void
pmd_signals_copy
    (pmd_signals *dest
    ,pmd_signals *src
    )
{
    memcpy(dest, src, sizeof(src->sigbitmap));
}


/**
 * @brief add a signal to the list of known signals
 */
static inline
void
pmd_signals_add
    (pmd_signals *signals
    ,unsigned int id
    )
{
    signals->sigbitmap[(id)/8] |= (1<<((id) & 7));
}


/**
 * @brief remove a signal from the list of known signals
 */
static inline
void
pmd_signals_remove
    (pmd_signals *signals
    ,unsigned int id
    )
{
    signals->sigbitmap[(id)/8] &= ~(1<<((id) & 7));
}


/**
 * @brief check whether or not a given signal is in the list of known signals
 */
static inline
pmd_bool
pmd_signals_test
    (const pmd_signals *signals
    ,unsigned int id
    )
{
    return signals->sigbitmap[(id)/8] & (1<<((id) & 7));
}


/**
 * @brief return the max signal
 */
static inline
unsigned int
pmd_signals_max
    (const pmd_signals *signals
    )
{
    int i = sizeof(signals->sigbitmap) - 1;
    int j;

    while (i >= 0)
    {
       if (signals->sigbitmap[i])
       {
           j = 7;
           while (j >= 0)
           {
               if (signals->sigbitmap[i] & (1<<j))
               {
                   return (i * 8) + j;
               }
               --j;
           }
       }
       --i;
    }
    return 0;
}


/**
 * @brief return the number of signals
 */
static inline
unsigned int
pmd_signals_count
    (const pmd_signals *signals
    )
{
    int i = sizeof(signals->sigbitmap) - 1;
    unsigned int count = 0;
    int j;

    while (i >= 0)
    {
        uint8_t byte = signals->sigbitmap[i];
        if (byte)
        {
           j = 7;
           while (j >= 0)
           {
               if (byte & (1<<j))
               {
                   ++count;
               }
               --j;
           }
       }
       --i;
    }
    return count;
}


/**
 * @brief remove one signal set from another
 *
 * Note that it only removes elements that are common to both
 * signal and subtrahend
 */
static inline
void
pmd_signals_subtract
    (pmd_signals *signals
    ,pmd_signals *subtrahend
    )
{
    unsigned int i;
    for (i = 0; i != sizeof(signals->sigbitmap); ++i)
    {
        uint8_t byte = signals->sigbitmap[i];
        uint8_t mask = byte & subtrahend->sigbitmap[i];
        signals->sigbitmap[i] = byte - mask;
    }
}




#endif /* PMD_SIGNALS_INC_ */
