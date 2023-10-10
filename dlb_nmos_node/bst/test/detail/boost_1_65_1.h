#ifndef BST_TEST_DETAIL_BOOST_1_65_1_H
#define BST_TEST_DETAIL_BOOST_1_65_1_H

// Map some of the Boost Test library (1.65.1) to BST_*

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
#define BOOST_CHECK_TRY_IMPL                                                             \
do {                                                                                    \
    try {                                                                               \
        BOOST_TEST_PASSPOINT();                                                         \
/**/

#define BOOST_CHECK_CATCH_IMPL( E, P, postfix, TL )                                      \
        BOOST_TEST_TOOL_IMPL( 2, false, "exception " BOOST_STRINGIZE(E) " expected but not raised", \
                              TL, CHECK_MSG, _ );                                       \
    } catch( E const& ex ) {                                                            \
        ::boost::unit_test::ut_detail::ignore_unused_variable_warning( ex );            \
        BOOST_TEST_TOOL_IMPL( 2, P, \
                              "exception \"" BOOST_STRINGIZE( E )"\" raised as expected" postfix,           \
                              TL, CHECK_MSG, _  );                                      \
    }                                                                                   \
} while( ::boost::test_tools::tt_detail::dummy_cond() )                                 \
/**/

#define BOOST_WARN_TRY            BOOST_CHECK_TRY_IMPL
#define BOOST_CHECK_TRY           BOOST_CHECK_TRY_IMPL
#define BOOST_REQUIRE_TRY         BOOST_CHECK_TRY_IMPL

#define BOOST_WARN_CATCH( E )     BOOST_CHECK_CATCH_IMPL( E, true, "", WARN )
#define BOOST_CHECK_CATCH( E )    BOOST_CHECK_CATCH_IMPL( E, true, "", CHECK )
#define BOOST_REQUIRE_CATCH( E )  BOOST_CHECK_CATCH_IMPL( E, true, "", REQUIRE )
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
// there are two choices for where to put the boost_test_print_type overload for std::wstring;
// in std (undefined behaviour), or as below (implementation detail and potential for ODR violation?)
namespace boost {
    namespace test_tools {
        namespace tt_detail {
            namespace impl {
                inline std::ostream& boost_test_print_type(std::ostream& ostr, std::wstring const& t) {
                    ostr << std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(t);
                    return ostr;
                }
            }
        }
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
