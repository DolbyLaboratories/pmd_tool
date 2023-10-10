add_library(dlb_adm)

target_include_directories(dlb_adm
    PUBLIC
        include
        adm_common_definitions
        ..
)

set_target_properties(dlb_adm PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

target_link_libraries(dlb_adm
    PRIVATE
        dlb_xml
        boost_1_75
)

target_compile_definitions(dlb_adm
    PUBLIC
        BOOST_DATE_TIME_NO_LIB
)

add_subdirectory(src)
add_subdirectory(test)
