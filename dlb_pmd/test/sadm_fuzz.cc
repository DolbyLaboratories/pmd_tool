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
 * @file sadm_fuzz.cc
 * @brief simple app to repeatedly generate random serial ADM models and make sure
 * the different aspects of dlb_pmd_lib 'work'
 *
 * @todo add the ability to generate broken xml to check the parser
 */

#include "TestModelRandom.hh"
#include <cstdlib>
#include <iostream>

#ifdef _MSC_VER
/* ignore 'unreachable code' warning, just in case we want to add a
 * termination condition to the main for loop in the future
 */
__pragma(warning(disable:4702))
#endif


/**
 * @brief main entry point for program
 */
int                        /** @return exit status, 0 for success, non-zero on error */
main
    (int argc              /**< [in] command-line argument count */
    ,const char **argv     /**< [in] array of command-line arguments */
    )
{
    unsigned int seed = 0x12345678u;
    uint64_t count = 0;
    int retval = 0;
    
    if (argc > 1)
    {
        char *endp;
        seed = (unsigned int)strtol(argv[1], &endp, 0);
    }

    try
    {
        std::cout << "PMD sADM Fuzzer" << std::endl;
        std::cout << "Copyright (c) Dolby Labs 2019" << std::endl;

        for (;;)
        {
            TestModelRandom m(seed, TestModelRandom::SADM);

            std::cout << "fuzz iteration " << count << std::endl;
            
            try
            {
                m.test(TestModel::TEST_SADM, "sadm_fuzz", (int)(count & 0x7fffffff));
            }
            catch (TestModel::failure& f)
            {
                std::cout << "Test failed at seed " << seed << std::endl
                          << f.msg << std::endl
                          << dlb_pmd_error(m) << std::endl;
                throw;
            }
            seed = seed * 77777777;
            count += 1;
        }
    }
    catch(...)
    {
        retval = -1;
    }

    return retval;
}

