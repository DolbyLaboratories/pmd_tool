/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed temp_directory_path function from filesystem utilities.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (c) 2019-2025, Dolby Laboratories Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BST_FILESYSTEM_H
#define BST_FILESYSTEM_H

// Provide bst::filesystem::path, etc. using either std:: or boost:: symbols

#if !defined(BST_FILESYSTEM_STD) && !defined(BST_FILESYSTEM_STD_EXPERIMENTAL) && !defined(BST_FILESYSTEM_MICROSOFT_TR2) && !defined(BST_FILESYSTEM_BOOST)

#if defined(__GNUC__)

#if __GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 3)
#define BST_FILESYSTEM_STD_EXPERIMENTAL
#else
#define BST_FILESYSTEM_BOOST
#endif

#elif defined(_MSC_VER)

#if _MSC_VER >= 1910
// From VS2017, /std:c++17 switch is introduced, but this is only indicated in __cplusplus if /Zc:__cplusplus is also specified
#if __cplusplus >= 201703L
#define BST_FILESYSTEM_STD
#else
#define BST_FILESYSTEM_STD_EXPERIMENTAL
#if _MSC_VER >= 1920
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#endif
#elif _MSC_VER >= 1900
// VS2015
#define BST_FILESYSTEM_STD_EXPERIMENTAL
#else
// Earlier
#define BST_FILESYSTEM_MICROSOFT_TR2
#endif

#else

// Default to C++17
#define BST_FILESYSTEM_STD

#endif

#endif

#if defined(BST_FILESYSTEM_STD)

#include <filesystem>
namespace bst_filesystem = std::filesystem;

#elif defined(BST_FILESYSTEM_STD_EXPERIMENTAL)

#include <experimental/filesystem>
namespace bst_filesystem = std::experimental::filesystem;

#elif defined(BST_FILESYSTEM_MICROSOFT_TR2)

#include <filesystem>
namespace bst_filesystem = std::tr2::sys;

#elif defined(BST_FILESYSTEM_BOOST)

#include <boost/filesystem.hpp>
namespace bst_filesystem = boost::filesystem;

#endif

namespace bst
{
    namespace filesystem
    {
        // Subset of symbols that can be used across all the possible implementations

        // Note that with older implementations, e.g. Microsoft TR2 (and Boost.Filesystem v2), path does not have constructors and string() member functions that convert between the source/string character type and the native encoding
        using bst_filesystem::path;
        using bst_filesystem::filesystem_error;
        using bst_filesystem::directory_entry;
        using bst_filesystem::directory_iterator;
        using bst_filesystem::recursive_directory_iterator;
        using bst_filesystem::exists;
        using bst_filesystem::is_regular_file;
        using bst_filesystem::is_directory;
        using bst_filesystem::file_size;
        using bst_filesystem::create_directory;
        using bst_filesystem::remove_all;
    }
}

#endif
