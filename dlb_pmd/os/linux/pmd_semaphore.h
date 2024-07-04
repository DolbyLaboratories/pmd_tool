/************************************************************************
 * dlb_pmd
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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
