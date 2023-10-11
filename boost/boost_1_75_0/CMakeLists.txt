add_library(boost_1_75 INTERFACE)

target_include_directories(boost_1_75
    INTERFACE
        .
)

target_compile_definitions(boost_1_75
    INTERFACE
        BOOST_ALL_NO_LIB
)

target_link_libraries(boost_1_75
    INTERFACE
        $<$<NOT:$<PLATFORM_ID:Windows>>:rt>
        $<$<NOT:$<PLATFORM_ID:Windows>>:pthread>  
)
