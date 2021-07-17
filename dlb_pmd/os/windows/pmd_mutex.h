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

#ifndef PMD_MUTEX_INC_
#define PMD_MUTEX_INC_

#include <windows.h>

#if _MSC_VER < 1900 && !defined(inline)
#  define inline __inline
#endif


struct pmd_mutex
{
    CRITICAL_SECTION mutex; 
};
    

static inline
void
pmd_mutex_init
    (pmd_mutex *mutex
    )
{
    InitializeCriticalSection(&mutex->mutex);
}


static inline
void
pmd_mutex_finish
    (pmd_mutex *mutex
    )
{
    DeleteCriticalSection(&mutex->mutex);
}


static inline
void
pmd_mutex_lock
    (pmd_mutex *mutex
    )
{
    EnterCriticalSection(&mutex->mutex);
}


static inline
void
pmd_mutex_unlock
    (pmd_mutex *mutex
    )
{
    LeaveCriticalSection(&mutex->mutex);
}


#endif /* PMD_MUTEX_INC_ */
