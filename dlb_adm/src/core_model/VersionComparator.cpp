/************************************************************************
 * dlb_adm
 * Copyright (c) 2025, Dolby Laboratories Inc.
 * Copyright (c) 2025, Dolby International AB.
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

#include "VersionComparator.h"
#include <algorithm>

bool compareVersion(std::string version_1, std::string version_2)
{
    std::string s1;
    std::string s2;
    int diff = std::count(version_1.begin(), version_1.end(), '.') - std::count(version_2.begin(), version_2.end(), '.');

    if (diff > 0)
    {
        s1 = version_1;
        s2 = version_2;
    }
    else
    {
        s2 = version_1;
        s1 = version_2;
    }
    // s1 > s2

    if (s2.at(s2.length() - 1) == '.')
    {
        s2 += std::string("0");
    }

    if (s1.at(s1.length() - 1) == '.')
    {
        s1 += std::string("0");
    }

    for (int i = 0; i < abs(diff); ++i)
    {
        s2 += std::string(".0");
    }

    return s1 == s2;
}
