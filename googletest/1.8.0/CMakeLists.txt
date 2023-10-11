add_library(gtest)

add_subdirectory(include)
add_subdirectory(src)

target_include_directories(gtest
    SYSTEM
    PUBLIC
        include
        include/gtest
    PRIVATE
        .
)

target_compile_definitions(gtest
    PUBLIC
        GTEST_HAS_PTHREAD=0 
)

set_target_properties(gtest PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

if(CMAKE_HOST_UNIX AND NOT CMAKE_HOST_APPLE)
    target_compile_definitions(gtest
        PUBLIC
            GTEST_LANG_CXX11=0
    )
else()
    target_compile_definitions(gtest
        PUBLIC
            GTEST_LANG_CXX11=1
    )
endif()
