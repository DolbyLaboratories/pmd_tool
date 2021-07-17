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

#ifndef PMD_THREAD_H_
#define PMD_THREAD_H_

/**
 * @brief Worker function pointer type for simple thread
 */
typedef void *(*pmd_thread_func)(void *arg);


/* ---------------------------------------------------------------------------------- */
#if defined(_MSC_VER)
#include <windows.h>

#if _MSC_VER < 1900 && !defined(inline)
#  define inline __inline
#endif

typedef struct
{
    HANDLE           handle; /**< Thread handle */
    unsigned int     id;     /**< Thread ID */
    pmd_thread_func  entry;  /**< Thread entry point function */
    void            *arg;    /**< Thread user state pointer */
} pmd_thread;

    
/**
 * @brief Wrapper function for starting thread.
 */
static inline
unsigned int __stdcall
thread_trampoline
    (void *arg
    )
{
    pmd_thread *thread = (pmd_thread *)arg;
    (thread->entry)(thread->arg);
    return 0;
}


static inline
int /** @return Return 0 on success. */
pmd_thread_init
    (pmd_thread       *thread   /**< [in] Thread to initialize */
    ,pmd_thread_func   entry    /**< [in] Thread entry point function */
    ,void             *arg      /**< [in] Pointer to user state */
    )
{
    BOOL res;

    thread->entry = entry;
    thread->arg = arg;

    thread->handle = (HANDLE)_beginthreadex(NULL,
                                            0,
                                            thread_trampoline,
                                            thread,
                                            CREATE_SUSPENDED,
                                            &thread->id);
    return 0;
}


/**
 * @brief Start thread.
 */
static inline
int                      /** @return Return 0 on success. */
pmd_thread_start
    (pmd_thread  *thread /**< [in] Thread */
    )
{
    if ((DWORD)-1 == ResumeThread(thread->handle))
    {
        CloseHandle(thread->handle);
        thread->handle = INVALID_HANDLE_VALUE;
        return -1;
    }
    return 0;
}
    

/**
 * @brief Join thread.
 */
static inline
int /** @return Return 0 on success. */
pmd_thread_join
    (pmd_thread  *thread /**< [in] Thread */
    ,void       **retval /**< [out] Thread return value */
    )
{
    DWORD status;

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
    return 0;
}


/**
 * @brief Cleanup thread.
 */
static inline
void
pmd_thread_finish
    (pmd_thread *thread   /**< [in] thread to clean up */
    )
{
    CloseHandle(thread->handle);
}


/* ---------------------------------------------------------------------------------- */
#elif defined (__linux__) || defined (__APPLE__)

#include <pthread.h>
#include <errno.h>
#include <stdio.h>

typedef struct
{
    pmd_thread_func  entry;    /**< Thread entry point function */
    void            *arg;      /**< Thread user state pointer */
    pthread_t        tid;      /**< Thread ID */
    int              started;  /**< start signal received? */
    pthread_mutex_t  lock;     /**< condition variable mutex */
    pthread_cond_t   wait;     /**< wait to start thread */

} pmd_thread;
    

/**
 * @brief Wrapper function for starting thread.
 */
static inline
void *         /** @return Return value when thread exits */
thread_trampoline
    (void *arg /**< [in] User state pointer */
    )
{
    pmd_thread *thread = (pmd_thread *)arg;
    void *ret = NULL;
    
    pthread_mutex_lock(&thread->lock);
    if (!thread->started)
    {
        pthread_cond_wait(&thread->wait, &thread->lock);
    }
    pthread_mutex_unlock(&thread->lock);
    ret = thread->entry(thread->arg);

    return ret;
}


/**
 * @brief Initialize thread.
 */
static inline
int /** @return Return 0 on success. */
pmd_thread_init
    (pmd_thread          *thread /**< [in] Thread to initialize */
    ,pmd_thread_func      entry  /**< [in] Thread entry point function */
    ,void                *arg    /**< [in] Pointer to user state */
    )
{
    int res;

    thread->entry = entry;
    thread->arg = arg;
    thread->started = 0;

    res = pthread_mutex_init(&thread->lock, NULL);
    if (0 != res)
    {
        return -1;
    }
    
    res = pthread_cond_init(&thread->wait, NULL);
    if (0 != res)
    {
        goto cleanup1;
    }
    
    res = pthread_create(&thread->tid, NULL, thread_trampoline, thread);
    if (0 != res)
    {
        printf("pthread_create failed (%d)", res);
        goto cleanup2;
    }
    
    return 0;

  cleanup2:
    pthread_mutex_destroy(&thread->lock);
  cleanup1:
    pthread_cond_destroy(&thread->wait);
    return -1;
}


/**
 * @brief Start thread.
 */
static inline
int                      /** @return Return 0 on success. */
pmd_thread_start
    (pmd_thread *thread /**< [in] Thread */
    )
{
    int ret;
    thread->started = 1;
    ret = pthread_cond_signal(&thread->wait);
    if (0 != ret)
    {
        printf("could not signal condition var: %u (%s)\n",
               errno, strerror(errno));
        return 1;
    }
    return 0;
}


/**
 * @brief Join thread.
 */
static inline
int                        /** @return Return 0 on success. */
pmd_thread_join
    (pmd_thread    *thread /**< [in] Thread */
    ,void         **retval /**< [out] Thread return value */
    )
{
    int res = 0;

    if (NULL != retval)
    {
        *retval = NULL;
    }
    
    res = pthread_join(thread->tid, retval);
    if (0 != res)
    {
        printf("could not join thread: %u (%s)\n",
               errno, strerror(errno));
    }

    return res;
}


/**
 * @brief Cleanup thread.
 *
 * we assume that thread has been terminated via pthread_join
 */
static inline
int /** @return Return 0 on success. */
pmd_thread_finish
   (pmd_thread *thread
    )
{
    pthread_cond_destroy(&thread->wait);
    pthread_mutex_destroy(&thread->lock);
    return 0;
}



#endif /* defined (__linux__) || defined (__APPLE__) */

#endif /* PMD_THREAD_H_ */
