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

#include <windows.h>
#include <assert.h>

struct pmd_semaphore
{
    HANDLE handle;
};

    
static inline
dlb_pmd_success
pmd_semaphore_init
    (pmd_semaphore *sem
    ,const char *name
    ,unsigned int init_value
    )
{
    dlb_pmd_success ok = PMD_FAIL;
    (void)name;

    sem->handle = CreateSemaphore(NULL, init_value, INT_MAX, NULL);
    if (sem->handle)
    {
        ok = PMD_SUCCESS;
    } 

    return ok;
}


static inline
void
pmd_semaphore_finish
    (pmd_semaphore *sem
    )
{
    if (sem->handle)
    {
        CloseHandle(sem->handle);
        sem->handle = NULL;
    }
}


static inline
dlb_pmd_success
pmd_semaphore_wait
    (pmd_semaphore *sem
    )
{
    dlb_pmd_success ok = PMD_SUCCESS;
    DWORD res = WaitForSingleObject(sem->handle, INFINITE);
    if (res == WAIT_FAILED)
    {
        ok = PMD_FAIL;
    }
    return ok;
}


static inline
dlb_pmd_success
pmd_semaphore_signal
    (pmd_semaphore *sem
    )
{
    dlb_pmd_success ok = PMD_FAIL;
    DWORD res = ReleaseSemaphore(sem->handle, 1, NULL);
    if (0 == res)
    {
        ok = PMD_SUCCESS;
    }
    return ok;
}


#endif /* PMD_SEMAPHORE_ */

