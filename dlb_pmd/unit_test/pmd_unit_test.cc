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
 * @file pmd_unit_test.cc
 * @brief Unit tests for the PMD API
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

/**
 * @brief main entry point for program
 */
int                        /** @return exit status, 0 for success, non-zero on error */
main
    (int argc              /**< [in] command-line argument count */
    ,char **argv           /**< [in] array of command-line arguments */
    )
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
