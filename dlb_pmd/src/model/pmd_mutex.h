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

#ifndef PMD_MUTEX_INC_
#define PMD_MUTEX_INC_

#if defined(_MSC_VER)
#include <windows.h>

#if !defined(inline)
#  define inline __inline
#endif


typedef struct
{
    CRITICAL_SECTION mutex; 
} pmd_mutex;
    

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



#elif defined (__linux__) || defined (__APPLE__)
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

typedef struct
{
    pthread_mutex_t mutex;
} pmd_mutex;
    

static inline
void
pmd_mutex_init
    (pmd_mutex *mutex
    )
{
    int rc = pthread_mutex_init(&mutex->mutex, NULL);
    if (0 != rc)
    {
        printf("Error: could not initialize mutex: %u, %s",
               errno, strerror(errno));
    }
}


static inline
void
pmd_mutex_finish
    (pmd_mutex *mutex
    )
{
    int rc = pthread_mutex_destroy(&mutex->mutex);
    if (0 != rc)
    {
        printf("Error: could not destroy mutex: %u, %s",
               errno, strerror(errno));
    }
}


static inline
void
pmd_mutex_lock
    (pmd_mutex *mutex
    )
{
    int rc = pthread_mutex_lock(&mutex->mutex);
    if (0 != rc)
    {
        printf("Error: could not lock mutex: %u, %s",
               errno, strerror(errno));
    }
}


static inline
void
pmd_mutex_unlock
    (pmd_mutex *mutex
    )
{
    int rc = pthread_mutex_unlock(&mutex->mutex);
    if (0 != rc)
    {
        printf("Error: could not unlock mutex: %u, %s",
               errno, strerror(errno));
    }
}

#endif /* defined (__linux__) || defined (__APPLE__) */

#endif /* PMD_MUTEX_INC_ */
