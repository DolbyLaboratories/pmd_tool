/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/
/**
 * @file pmd_ctrl_c.c
 * @brief linux implementation of the CTRL-C handler
 */

#include "pmd_os.h"

#include <signal.h>
#include <stdlib.h>

/**
 * @brief global variable to set upon receipt of CTRL-C
 */
static volatile int* running = NULL;


/**
 * @brief signal handler
 */
static
void
sighandler
    (int sig
    )
{
    (void)sig;
    if (running)
    {
        *running = 0;
    }
}


void
pmd_ctrlc_handle
    (volatile int *x
    )
{
    running = x;
    signal(SIGINT, sighandler);
}


