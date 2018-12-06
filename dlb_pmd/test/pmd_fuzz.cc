/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#include "TestModel.hh"
#include "XmlSchema.hh"
#include <cstdlib>
#include <iostream>

#ifdef _MSC_VER
/* ignore 'unreachable code' warning, just in case we want to add a
 * termination condition to the main for loop in the future
 */
__pragma(warning(disable:4702))
#endif

static
void
try_test
    (TestModel& m
    ,unsigned int seed
    ,uint64_t count
    ,TestModel::TestType tt
    )
{
    try
    {
        m.test(tt, "fuzz", (int)(count & 0x7fffffff));
    }
    catch (TestModel::failure& f)
    {
        std::cout << "Test failed at seed " << seed << std::endl
                  << f.msg << std::endl
                  << dlb_pmd_error(m) << std::endl;
    }
}


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
    
    if (argc > 1)
    {
        char *endp;
        seed = (unsigned int)strtol(argv[1], &endp, 0);
    }

    XmlSchema::initialize();

    std::cout << "PMD Fuzzer" << std::endl;
    std::cout << "Copyright (c) Dolby Labs 2018" << std::endl;

    for (;;)
    {
        TestModel m;
        m.generate_random(seed);
        m.pcm_single_frame();

        std::cout << "fuzz iteration " << count << std::endl;

        try_test(m, seed, count, TestModel::TEST_XML);
        try_test(m, seed, count, TestModel::TEST_KLV);
        try_test(m, seed, count, TestModel::TEST_MDSET);

        /* todo: test PCM/ED2 if the number of beds/objects is small enough */

        seed = seed * 77777777;
        count += 1;
    }

    XmlSchema::finalize();
    return 0;
}

