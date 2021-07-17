/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

int main(int argc, char **argv)
{
    int result = 0;

    try
    {
        ::testing::InitGoogleTest(&argc, argv);
        result = RUN_ALL_TESTS();
    }
    catch (...)
    {
        result = -1;
    }

    return result;
}
