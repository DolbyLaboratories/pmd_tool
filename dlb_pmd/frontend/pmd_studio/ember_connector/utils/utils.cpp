/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/
#include "utils.h"

#include <time.h>

unsigned long long msTime()
{
    struct timespec spec;
    int ret;
    if ((ret = clock_gettime(CLOCK_MONOTONIC, &spec)) == 0)
    {
        return spec.tv_sec * 1000LL + spec.tv_nsec / 1000000;
    }

    return 0;
}

