/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed BST_CHECK_NO_THROW, BST_REQUIRE_NO_THROW, BST_WARN_THROW, and BST_WARN_NO_THROW macros.
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

#ifndef BST_TEST_DETAIL_BOOST_1_57_0_H
#define BST_TEST_DETAIL_BOOST_1_57_0_H

// Map some of the Boost Test library (1.57.0) to BST_*

#ifndef BST_TEST_MAIN

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>

#else

#include "detail/pragma_warnings.h"
#define BOOST_TEST_MAIN
PRAGMA_WARNING_PUSH
PRAGMA_WARNING_DISABLE_UNREACHABLE_CODE
#include <boost/test/included/unit_test.hpp>
PRAGMA_WARNING_POP

#endif

//+ Break BOOST_CHECK_THROW_IMPL in two so the statement can contain commas, etc.
#define BOOST_CHECK_TRY_IMPL                                                                            \
    try {                                                                                               \
        BOOST_TEST_PASSPOINT();                                                                         \
/**/

#define BOOST_CHECK_CATCH_IMPL( E, P, prefix, TL )                                                      \
        BOOST_CHECK_IMPL( false, "exception " BOOST_STRINGIZE( E ) " is expected", TL, CHECK_MSG ); }   \
    catch( E const& ex ) {                                                                              \
        ::boost::unit_test::ut_detail::ignore_unused_variable_warning( ex );                            \
        BOOST_CHECK_IMPL( P, prefix BOOST_STRINGIZE( E ) " is caught", TL, CHECK_MSG );                 \
    }                                                                                                   \
/**/

#define BOOST_WARN_TRY            BOOST_CHECK_TRY_IMPL
#define BOOST_CHECK_TRY           BOOST_CHECK_TRY_IMPL
#define BOOST_REQUIRE_TRY         BOOST_CHECK_TRY_IMPL

#define BOOST_WARN_CATCH( E )     BOOST_CHECK_CATCH_IMPL( E, true, "exception ", WARN )
#define BOOST_CHECK_CATCH( E )    BOOST_CHECK_CATCH_IMPL( E, true, "exception ", CHECK )
#define BOOST_REQUIRE_CATCH( E )  BOOST_CHECK_CATCH_IMPL( E, true, "exception ", REQUIRE )
//- Break BOOST_CHECK_THROW_IMPL in two so the statement can contain commas, etc.

#define BST_TEST_CASE(symbol) BOOST_AUTO_TEST_CASE(symbol)
#define BST_CHECK(expr) BOOST_CHECK(expr)
#define BST_CHECK_EQUAL(expected, actual) BOOST_CHECK_EQUAL(expected, actual)
#define BST_CHECK_NE(expected, actual) BOOST_CHECK_NE(expected, actual)
#define BST_CHECK_LT(expected, actual) BOOST_CHECK_LT(expected, actual)
#define BST_CHECK_LE(expected, actual) BOOST_CHECK_LE(expected, actual)
#define BST_CHECK_GT(expected, actual) BOOST_CHECK_GT(expected, actual)
#define BST_CHECK_GE(expected, actual) BOOST_CHECK_GE(expected, actual)
#define BST_CHECK_THROW(expr, exception) BOOST_CHECK_THROW(expr, exception)
#define BST_REQUIRE(expr) BOOST_REQUIRE(expr)
#define BST_REQUIRE_EQUAL(expected, actual) BOOST_REQUIRE_EQUAL(expected, actual)
#define BST_REQUIRE_NE(expected, actual) BOOST_REQUIRE_NE(expected, actual)
#define BST_REQUIRE_LT(expected, actual) BOOST_REQUIRE_LT(expected, actual)
#define BST_REQUIRE_LE(expected, actual) BOOST_REQUIRE_LE(expected, actual)
#define BST_REQUIRE_GT(expected, actual) BOOST_REQUIRE_GT(expected, actual)
#define BST_REQUIRE_GE(expected, actual) BOOST_REQUIRE_GE(expected, actual)
#define BST_REQUIRE_THROW(expr, exception) BOOST_REQUIRE_THROW(expr, exception) 
#define BST_WARN(expr) BOOST_WARN(expr)
#define BST_WARN_EQUAL(expected, actual) BOOST_WARN_EQUAL(expected, actual)
#define BST_WARN_NE(expected, actual) BOOST_WARN_NE(expected, actual)
#define BST_WARN_LT(expected, actual) BOOST_WARN_LT(expected, actual)
#define BST_WARN_LE(expected, actual) BOOST_WARN_LE(expected, actual)
#define BST_WARN_GT(expected, actual) BOOST_WARN_GT(expected, actual)
#define BST_WARN_GE(expected, actual) BOOST_WARN_GE(expected, actual)

// Explicit STRING macros to work around the different behaviours of the frameworks when comparing two char* or wchar_t*
#define BST_CHECK_STRING_EQUAL(expected, actual) BOOST_CHECK_EQUAL(expected, actual)
#define BST_CHECK_STRING_NE(expected, actual) BOOST_CHECK_NE(expected, actual)
#define BST_REQUIRE_STRING_EQUAL(expected, actual) BOOST_REQUIRE_EQUAL(expected, actual)
#define BST_REQUIRE_STRING_NE(expected, actual) BOOST_REQUIRE_NE(expected, actual)

// Extension to support Boost Test logging of wstring values
#include <codecvt>
#include <locale>
namespace boost {
    namespace test_tools {
        template<>
        struct print_log_value<std::wstring> {
            void operator()(std::ostream& ostr, std::wstring const& t)
            {
                ostr << std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(t);
            }
        };
    }
}

#define BST_WARN_TRY BOOST_WARN_TRY
#define BST_CHECK_TRY BOOST_CHECK_TRY
#define BST_REQUIRE_TRY BOOST_REQUIRE_TRY

#define BST_WARN_CATCH(exception) BOOST_WARN_CATCH(exception)
#define BST_CHECK_CATCH(exception) BOOST_CHECK_CATCH(exception)
#define BST_REQUIRE_CATCH(exception) BOOST_REQUIRE_CATCH(exception)

#include <boost/mpl/list.hpp>
#include <boost/mpl/joint_view.hpp>

#define BST_TEMPLATE_TEST_CASE_1(symbol, T, T1) \
    namespace { typedef boost::mpl::list<T1> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_2(symbol, T, T1, T2) \
    namespace { typedef boost::mpl::list<T1, T2> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_3(symbol, T, T1, T2, T3) \
    namespace { typedef boost::mpl::list<T1, T2, T3> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_4(symbol, T, T1, T2, T3, T4) \
    namespace { typedef boost::mpl::list<T1, T2, T3, T4> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_5(symbol, T, T1, T2, T3, T4, T5) \
    namespace { typedef boost::mpl::list<T1, T2, T3, T4, T5> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_6(symbol, T, T1, T2, T3, T4, T5, T6) \
    namespace { typedef boost::mpl::list<T1, T2, T3, T4, T5, T6> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_7(symbol, T, T1, T2, T3, T4, T5, T6, T7) \
    namespace { typedef boost::mpl::list<T1, T2, T3, T4, T5, T6, T7> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#define BST_TEMPLATE_TEST_CASE_8(symbol, T, T1, T2, T3, T4, T5, T6, T7, T8) \
    namespace { typedef boost::mpl::list<T1, T2, T3, T4, T5, T6, T7, T8> symbol##_types; } \
    BOOST_AUTO_TEST_CASE_TEMPLATE(symbol, T, symbol##_types)

#endif
