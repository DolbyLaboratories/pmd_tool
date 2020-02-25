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

#include "dlb_pmd_api.h"

#ifndef PMD_OS_H_
#define PMD_OS_H_

#ifdef __cplusplus
extern "C" {
#endif
    
/* -------------------------- SEMAPHORES ------------------------------- */


/**
 * @brief abstract type of cross-platform semaphore
 *
 * Note that this is a forward-declaration; the actual structure is
 * defined by one of the OS-specific #include files below.
 */
typedef struct pmd_semaphore pmd_semaphore;


/**
 * @brief prototype of the semaphore initialisation function
 */
static inline
dlb_pmd_success                  /** @return PMD_SUCCESS on success; PMD_FAIL otherwise */
pmd_semaphore_init
    (pmd_semaphore *sem          /**< [in] semaphore to initialize */
    ,const char *name            /**< [in] name of semaphore, if OS supports names */
    ,unsigned int init_value     /**< [in] initial value of semaphore */
    );


/**
 * @brief finish a semaphore; release its resources to the OS
 */
static inline
void
pmd_semaphore_finish
    (pmd_semaphore *sem          /**< [in] semaphore to release to OS */
    );


/**
 * @brief wait for semaphore to signal
 */
static inline
dlb_pmd_success                  /** @return PMD_SUCCESS when semaphore acquired */
pmd_semaphore_wait
    (pmd_semaphore *sem          /**< [in] semaphore to block for */
    );


/**
 * @brief signal a semaphore is available
 */
static inline
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
pmd_semaphore_signal
    (pmd_semaphore *sem         /**< [in] semaphore to signal */
    );


/* -------------------------- MUTEXES ------------------------------- */


/**
 * @brief abstract type of PMD mutex abstraction.
 *
 * Note that the actual structure will be defined by one of the #include
 * files below.
 */
typedef struct pmd_mutex pmd_mutex;


/**
 * @brief initialise the mutex
 */
static inline
void
pmd_mutex_init
    (pmd_mutex *mutex         /**< [in] mutex to initialize */
    );


/**
 * @brief finish mutex and return resources to OS
 */
static inline
void
pmd_mutex_finish
    (pmd_mutex *mutex        /**< [in] mutex to finish */
    );


/**
 * @brief wait to acquire a mutex
 */
static inline
void
pmd_mutex_lock
    (pmd_mutex *mutex       /**< [in] mutex to block on */
    );


/**
 * @brief if mutex is immediately acquirable, acquire it, otherwise don't
 */
static inline
dlb_pmd_bool                /** @return 1 if mutex was acquired, 0 otherwise */
pmd_mutex_try_lock
    (pmd_mutex *mutex       /**< [in] mutex to try acquiring */
    );


/**
 * @brief release the mutex
 */
static inline
void
pmd_mutex_unlock
    (pmd_mutex *mutex       /**< [in] mutex to release */
    );



/* -------------------------- THREADING ------------------------------- */


/**
 * @brief Worker function pointer type for simple thread
 */
typedef void *(*pmd_thread_func)(void *arg);


/**
 * @brief PMD thread priorities
 *
 * Typically dpf threads should be run at normal priority; however
 * there are times when threads need to be run at preferred or background
 * priorities.  Use the dpf_thread_priority_init function to create
 * such a thread.
 *
 * @note Beware, however, that on some systems (such as linux)
 * you may need to run as root user to effect these changes.
 *
 * @note Some systems may not implement all priorities
 */
typedef enum
{
    PMD_THREAD_PRIORITY_BACKGROUND, /**< run only when nothing else can run */
    PMD_THREAD_PRIORITY_NORMAL,     /**< allow OS to schedule as it sees fit */
    PMD_THREAD_PRIORITY_REALTIME,   /**< force a task to complete when started */
    PMD_THREAD_PRIORITY_MAX,        /**< highest realtime priority */
    PMD_THREAD_PRIORITY_CUSTOM      /**< custom priority */
} pmd_thread_priority;


/**
 * @brief common thread type
 *
 * Note that the actual structure will be defined later on, in one of the
 * OS-specific implementation #includes below.
 */
typedef struct pmd_thread pmd_thread;


/**
 * @brief initialize a thread.
 *
 * Threads are created 'stopped'. They have to be separately started using the
 * #pmd_thread_start function.
 */
static inline
dlb_pmd_success                     /** @return Return 0 on success. */
pmd_thread_init
    (pmd_thread          *thread    /**< [in] Thread to initialize */
    ,pmd_thread_func      entry     /**< [in] Thread entry point function */
    ,void                *arg       /**< [in] Pointer to user state */
    ,pmd_thread_priority  prio      /**< [in] requested priority */
    ,int                  policy    /**< [in] scheduling policy (used if pri is PMD_THREAD_PRIORITY_CUSTOM) */
    ,int                  priority  /**< [in] scheduling priority (used if pri is PMD_THREAD_PRIORITY_CUSTOM) */
    );


/**
 * @brief set a thread's name (for debugging) if the OS supports it
 */
static inline
dlb_pmd_success                    /** @return Return PMD_SUCCESS on success. */
pmd_thread_set_name
    (pmd_thread    *thread         /**< [in] Thread */
    ,const char    *name           /**< [in] New name (16 bytes long max) */
    );


/**
 * @brief Start thread.
 */
static inline
dlb_pmd_success                    /** @return Return 0 on success. */
pmd_thread_start
    (pmd_thread *thread            /**< [in] Thread */
    );


/**
 * @brief Join thread.
 */
static inline
dlb_pmd_success                    /** @return Return 0 on success. */
pmd_thread_join
    (pmd_thread    *thread         /**< [in] Thread */
    ,void         **retval         /**< [out] Thread return value */
    );


/**
 * @brief Cleanup thread.
 *
 * we assume that thread has been terminated via pthread_join
 */
static inline
void
pmd_thread_finish
    (pmd_thread    *thread         /**< [in] Thread */
    );


/* ---------------------------- CTRL-C handling ------------------- */


/**
 * @brief set up a CTRL-C handler; it will set the given pointer to
 * value 0 if CTRL-C is detected.
 */
void
pmd_ctrlc_handle
    (volatile int *x             /**< [in] pointer to set to 0 upon
                                   * CTRL-C detection */
    );



/* ---------------------------- OS implementations ------------------- */


/*
 * The following #includes provide definitions
 */
#if defined (_MSC_VER)
#  include "windows/pmd_mutex.h"
#  include "windows/pmd_semaphore.h"
#  include "windows/pmd_thread.h"
#elif defined (__linux__)
#  include "linux/pmd_mutex.h"
#  include "linux/pmd_semaphore.h"
#  include "linux/pmd_thread.h"
#elif defined (__APPLE__)
#  include "osx/pmd_mutex.h"
#  include "osx/pmd_semaphore.h"
#  include "osx/pmd_thread.h"
#else
#  error unsupported OS
#endif

#ifdef __cplusplus
}
#endif
   
#endif /* PMD_OS_H_ */

