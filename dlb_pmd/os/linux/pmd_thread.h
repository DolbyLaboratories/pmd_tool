/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef PMD_THREAD_H_
#define PMD_THREAD_H_

#include <pthread.h>
#include <string.h>
#include <stdio.h>

#include "pmd_mutex.h"
#include "pmd_semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif
    

#if defined (__clang__)
#  define CAN_SET_NAME 0
#elif defined(__APPLE__) && defined(__MACH__)
#  define CAN_SET_NAME 1    
#elif defined(__GNUC__)
#  define CAN_SET_NAME 1
#else
#  define CAN_SET_NAME 0
#endif    


/**
 * @brief Main thread struct.
 */
struct pmd_thread
{
    char name[16];           /**< Thread name (for debugging) */
    pmd_thread_func  entry;  /**< Thread entry point function */
    void            *arg;    /**< Thread user state pointer */
    pthread_t        tid;    /**< Thread ID */
    pmd_semaphore    wait;   /**< Semaphore for starting thread */
    pmd_mutex        mutex;  /**< Mutex to sync thread-safe access */
    dlb_pmd_bool     joined; /**< Information about whether the thread has been joined already */
    void            *res;    /**< cache result */
};


/**
 * @brief set the thread's name if OS and compiler supports it
 */
static inline
void
pmd_thread_actually_set_name
    (pmd_thread *thread
    )
{
#if CAN_SET_NAME
    if (thread->name[0] != '\0')
    {
        pthread_setname_np(thread->tid, thread->name);
    }
#else
    (void)thread;
#endif
}
    

/**
 * @brief Wrapper function for starting thread.
 */
static inline
void * /** @return Return value when thread exits */
pmd_thread_wrapper
    (void *arg /**< [in] User state pointer */
    )
{
    pmd_thread *thread = (pmd_thread *)arg;
    void *ret = NULL;

    pmd_thread_actually_set_name(thread);
    
    if (0 == pmd_semaphore_wait(&thread->wait))
    {
        ret = thread->entry(thread->arg);
    }
    pmd_semaphore_signal(&thread->wait);

    /* In case we terminate before join, clean up resources
     * via pthread_detach.  If pthread_join detects that the
     * thread has already terminated, it won't clean up.
     *
     * Note that if the mutex is being held, then another
     * thread must be shutting us down via join.  In that
     * case, we don't need to clean up.
     */
    if (0 == pmd_mutex_try_lock(&thread->mutex))
    {
        if (!thread->joined)
        {
            pthread_detach(thread->tid);
            thread->joined = 1;
            thread->res = ret;
        }
        pmd_mutex_unlock(&thread->mutex);
    }
    return ret;
}


static inline
dlb_pmd_success
pmd_thread_init
    (pmd_thread          *thread
    ,pmd_thread_func      entry
    ,void                *arg
    ,pmd_thread_priority  prio
    ,int                  policy
    ,int                  priority
    )
{
    struct sched_param sched;
    int res;

    memset(thread, '\0', sizeof(*thread));

    thread->joined = 0;
    thread->entry = entry;
    thread->arg = arg;

    res = pmd_semaphore_init(&thread->wait, "threadwait", 0);
    if (0 != res)
    {
        return PMD_FAIL;
    }
    
    pmd_mutex_init(&thread->mutex);
    res = pthread_create(&thread->tid, NULL, pmd_thread_wrapper, thread);
    if (0 != res)
    {
        printf("pthread_create failed (%d)", res);
        goto cleanup2;
    }
    
    switch (prio)
    {
        case PMD_THREAD_PRIORITY_MAX:
            sched.sched_priority = sched_get_priority_max(SCHED_RR);
            pthread_setschedparam(thread->tid, SCHED_RR, &sched);
            break;
        case PMD_THREAD_PRIORITY_REALTIME:
            sched.sched_priority =
                (sched_get_priority_max(SCHED_RR)
                 + sched_get_priority_min(SCHED_RR))/2;
            pthread_setschedparam(thread->tid, SCHED_RR, &sched);
            break;
        case PMD_THREAD_PRIORITY_BACKGROUND:
#ifdef SCHED_IDLE
            sched.sched_priority = 
                (sched_get_priority_max(SCHED_IDLE)
                 + sched_get_priority_min(SCHED_IDLE))/2;
            pthread_setschedparam(thread->tid, SCHED_IDLE, &sched);                
            break;
#endif
        case PMD_THREAD_PRIORITY_CUSTOM:
            sched.sched_priority = priority;
            pthread_setschedparam(thread->tid, policy, &sched);
            break;
        case PMD_THREAD_PRIORITY_NORMAL:
        default:
            /* do nothing */
            break;
    }

    return PMD_SUCCESS;

  cleanup2:
    pmd_mutex_finish(&thread->mutex);
    pmd_semaphore_finish(&thread->wait);
    return PMD_FAIL;
}


static inline
dlb_pmd_success
pmd_thread_set_name
    (pmd_thread    *thread
    ,const char    *name
    )
{
    strncpy(thread->name, name, sizeof(thread->name)-1);
    pmd_thread_actually_set_name(thread);
    return PMD_SUCCESS;
}


static inline
dlb_pmd_success
pmd_thread_start
    (pmd_thread *thread
    )
{
    return pmd_semaphore_signal(&thread->wait);
}


static inline
dlb_pmd_success
pmd_thread_join
    (pmd_thread    *thread
    ,void         **retval
    )
{
    int res = 0;

    if (NULL != retval)
    {
        *retval = NULL;
    }
    
    pmd_mutex_lock(&thread->mutex);
    /* avoid joining a thread that has already been joined - not
     * supported in posix */
    if (!thread->joined)
    {
        /* in case it hasn't started, start it! */
        (void)pmd_semaphore_signal(&thread->wait);
        res = pthread_join(thread->tid, retval);
        if (0 == res)
        {
            thread->joined = 1;
        }
    }
    else if (NULL != retval)
    {
        *retval = thread->res;
    }

    pmd_mutex_unlock(&thread->mutex);
    return res;
}


static inline
void
pmd_thread_finish
    (pmd_thread *thread
    )
{
    pmd_semaphore_finish(&thread->wait);
    pmd_mutex_finish(&thread->mutex);
}


#ifdef __cplusplus
}
#endif
#endif /* PMD_THREAD_H_ */
