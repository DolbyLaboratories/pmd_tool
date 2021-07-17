/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2019 by Dolby Laboratories,
 *                Copyright (C) 2017-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
