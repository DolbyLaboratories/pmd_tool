/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file pmd_tool_main.c
 * @brief reader/writer program from Professional Metadata library
 */


#include "pmd_tool.h"


int
main
    (      int   argc
    ,const char *argv[]
    )
{
    int   res = 0;
    Args  args;

    if (!dlb_pmd_tool_parse_cmdline_args(&args, argc, argv))
    {
        dlb_pmd_tool_usage(&args);
        return -1;
    }

    res = dlb_pmd_tool_process(&args);

    return res;
}
