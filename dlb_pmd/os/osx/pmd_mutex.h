/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2020 by Dolby Laboratories,
 *                Copyright (C) 2017-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_MUTEX_INC_
#define PMD_MUTEX_INC_

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

struct pmd_mutex
{
    pthread_mutex_t mutex;
};
    

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
dlb_pmd_bool
pmd_mutex_try_lock
    (pmd_mutex *mutex
    )
{
    int rc = pthread_mutex_trylock(&mutex->mutex);
    if (0 != rc)
    {
        return PMD_FALSE;
    }
    return PMD_TRUE;
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


#endif /* PMD_MUTEX_INC_ */
