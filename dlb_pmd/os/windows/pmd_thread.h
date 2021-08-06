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

#ifndef PMD_THREAD_H_
#define PMD_THREAD_H_

#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>

#include "pmd_mutex.h"
#include "pmd_semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif
    



/**
 * @brief Main thread struct.
 */
struct pmd_thread
{
    HANDLE           handle; /**< Thread handle */
    unsigned int     id;     /**< Thread ID */
    pmd_thread_func  entry;  /**< Thread entry point function */
    void            *arg;    /**< Thread user state pointer */
    unsigned int     stop;   /**< external halt signal */
    pmd_mutex        mutex;  /**< mutex to sync thread-safe access */
    dlb_pmd_bool     joined; /**< has the thread already joined? */
};


/**
 * @brief Wrapper function for starting thread.
 */
static inline
unsigned int __stdcall
thread_trampoline
    (void *arg
    )
{
    struct pmd_thread *thread = (struct pmd_thread *)arg;

    (thread->entry)(thread->arg);
    _endthreadex(0);
    return 0;
}


static inline
dlb_pmd_success
pmd_thread_init
    (pmd_thread          *thread
    ,pmd_thread_func      entry
    ,void                *arg
    ,pmd_thread_priority  priority
    ,int                  policy_custom
    ,int                  priority_custom
    )
{
    BOOL res;

    (void)policy_custom;
    (void)priority_custom;

    thread->entry = entry;
    thread->arg = arg;
    thread->stop = 0;
    thread->joined = 1;

    pmd_mutex_init(&thread->mutex);

    thread->handle = (HANDLE)_beginthreadex(NULL,
                                            0,
                                            thread_trampoline,
                                            thread,
                                            CREATE_SUSPENDED,
                                            &thread->id);
    if (INVALID_HANDLE_VALUE == thread->handle)
    {
        goto cleanup2;
    }

    switch (priority)
    {
        case PMD_THREAD_PRIORITY_REALTIME:
            res = SetThreadPriority(thread->handle, THREAD_PRIORITY_TIME_CRITICAL);
            break;
        case PMD_THREAD_PRIORITY_BACKGROUND:
            res = SetThreadPriority(thread->handle, THREAD_PRIORITY_IDLE);
            break;
            
        default:
            res = 1; /* do nothing, normal priority by default */
            break;
    }
    return PMD_SUCCESS;

cleanup2:
    pmd_mutex_finish(&thread->mutex);
    return PMD_FAIL;
}


static inline
dlb_pmd_success
pmd_thread_set_name
    (pmd_thread    *thread
    ,const char    *name
    )
{
    /* Win32 threads do not have a name. */
	(void)thread;
	(void)name;
    return PMD_SUCCESS;
}


static inline
dlb_pmd_success
pmd_thread_start
    (pmd_thread *thread
    )
{
    dlb_pmd_success result = PMD_SUCCESS;
    
    pmd_mutex_lock(&thread->mutex);
    thread->joined = 0;
    if ((DWORD)-1 == ResumeThread(thread->handle))
    {
        CloseHandle(thread->handle);
        thread->handle = INVALID_HANDLE_VALUE;
        result = PMD_FAIL;
        thread->joined = 1;
    }
    pmd_mutex_unlock(&thread->mutex);
    return result;
}


static inline
dlb_pmd_success
pmd_thread_join
    (pmd_thread    *thread
    ,void         **retval
    )
{
    DWORD status;

    pmd_mutex_lock(&thread->mutex);
    if (!thread->joined)
    {
        for(;;)
        {
            BOOL ok = GetExitCodeThread(thread->handle, &status);
            if (ok)
            {
                if (status != STILL_ACTIVE)
                {
                    break;
                }
                WaitForSingleObject(thread->handle, INFINITE);
            }
        }
        if (NULL != retval)
        {
            *retval = (void*)(UINT_PTR)status;
        }
    }
    thread->joined = 1;
    pmd_mutex_unlock(&thread->mutex);
    return PMD_SUCCESS;
}


static inline
void
pmd_thread_finish
    (pmd_thread *thread
    )
{
    CloseHandle(thread->handle);
    pmd_mutex_finish(&thread->mutex);
}


#ifdef __cplusplus
}
#endif
#endif /* PMD_THREAD_H_ */
