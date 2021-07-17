/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_SEMAPHORE_H_
#define PMD_SEMAPHORE_H_

#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

struct pmd_semaphore
{
    sem_t sem;
};

    
static inline
dlb_pmd_success
pmd_semaphore_init
    (pmd_semaphore *sem
    ,const char *name
    ,unsigned int init_value
    )
{
    if (sem_init(&sem->sem, 0, init_value) != 0)
    {
        printf("sem_init(%s) failed (%s)", name, strerror(errno));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


static inline
void
pmd_semaphore_finish
    (pmd_semaphore *sem
    )
{
    if (sem_destroy(&sem->sem) != 0)
    {
        printf("sem_destroy failed (%s)", strerror(errno));
    }
}


static inline
dlb_pmd_success
pmd_semaphore_wait
    (pmd_semaphore *sem
    )
{
    if (sem_wait(&sem->sem) != 0)
    {
        printf("sem_wait failed (%s)", strerror(errno));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


static inline
dlb_pmd_success
pmd_semaphore_signal
    (pmd_semaphore *sem
    )
{
    if (sem_post(&sem->sem) != 0)
    {
        printf("sem_post failed (%s)", strerror(errno));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}

#endif /* PMD_SEMAPHORE_H_ */
