/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2021 by Dolby Laboratories,
 *                Copyright (C) 2016-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file pmd_test.cc
 * @brief simple tool to iterate a few tests
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "TestModel.hh"
#include "XmlSchema.hh"
#include "gtest/gtest.h"
#include <string.h>
#include <ctime>
#include <iostream>


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
    std::time_t start = std::time(NULL);

    try
    {
        ::testing::InitGoogleTest(&argc, argv);

        XmlSchema::initialize();
        result = RUN_ALL_TESTS();
        XmlSchema::finalize();
    }
    catch(...)
    {
        result = -1;
    }

    std::time_t finish = std::time(NULL);
    double dt = std::difftime(finish, start);
    std::cout << "PMD test application ran for " << dt << " seconds" << std::endl;

    return result;
}


