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

#ifndef _WIN32

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

extern "C"
{
#include "ember.h"
}

TEST(libember_slim_test, dummy)
{
    int i, result;

    for (i = 0; i < 3; i++)
    {
        // Can't use the dummy anymore, there's a real library there!
        //result = ember_dummy(i);
        result = i;
        EXPECT_EQ(i, result);
    }
}

#endif
