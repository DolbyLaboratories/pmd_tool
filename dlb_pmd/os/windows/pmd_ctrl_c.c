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

/**
 * @file pmd_ctrl_c.c
 * @brief windows implementation of the CTRL-C handler
 */


#include <windows.h>
#include <stdio.h>

/**
 * @brief global variable to set upon receipt of CTRL-C
 */
static volatile int* running = NULL;


/**
 * @brief windows console control handler
 */
static
BOOL WINAPI 
ctrl_handler
    (DWORD fdwCtrlType
    )
{
    if (CTRL_C_EVENT == fdwCtrlType)
    {
        if (running)
        { 
            *running = 0;
        }
        return TRUE;
    }
    return FALSE;
}


void
pmd_ctrlc_handle
    (volatile int *x
    )
{
    if (!SetConsoleCtrlHandler(ctrl_handler, TRUE))
    {
        printf("Could not set ctrl-c handler\n");
    }
    running = x;
}

