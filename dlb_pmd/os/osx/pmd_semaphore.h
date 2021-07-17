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

#ifndef PMD_SEMAPHORE_
#define PMD_SEMAPHORE_

#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#define SEMAPHORE_NAME_MAX    (31)

struct pmd_semaphore
{
    sem_t *sem;
    char name[SEMAPHORE_NAME_MAX];
};

    
static inline
dlb_pmd_success
pmd_semaphore_init
    (pmd_semaphore *sem
    ,const char *name
    ,unsigned int init_value
    )
{
    uint64_t tid;
    char *c;
    
    pthread_threadid_np(NULL, &tid);

    /* mac OS requires / to prefix named semaphores */
    snprintf(sem->name, SEMAPHORE_NAME_MAX, "/%" PRIu64 "_%s", tid, name);
    c = sem->name;
    while (c != NULL)
    {
        c = strchr(c, ' ');
        if (NULL != c)
        {
            *c = '_';
        }
    }

    sem_unlink(sem->name);
    sem->sem = sem_open(sem->name, O_EXCL | O_CREAT, S_IRWXU | S_IRWXG, init_value);
    if (sem->sem == SEM_FAILED)
    {
        printf("sem_open(%s) failed (%s)", sem->name, strerror(errno));
        return PMD_FAIL;
    }
    /* and make sure nobody else can share it via the name */
    sem_unlink(sem->name);
    return PMD_SUCCESS;
}


static inline
void
pmd_semaphore_finish
    (pmd_semaphore *sem
    )
{
    if (sem->sem)
    {
        if (sem_close(sem->sem) != 0)
        {
            printf("sem_close failed (%s)", strerror(errno));
        }
        sem->sem = NULL;
    }
}


static inline
dlb_pmd_success
pmd_semaphore_wait
    (pmd_semaphore *sem
    )
{
    if (sem_wait(sem->sem) != 0)
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
    if (sem_post(sem->sem) != 0)
    {
        printf("sem_post failed (%s)", strerror(errno));
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}


#endif /* PMD_SEMAPHORE_ */
