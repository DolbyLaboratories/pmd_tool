target_link_libraries(xerces_3_2_0_lib
    PRIVATE
        pthread 
        curl
        "-framework CoreServices"
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

target_compile_definitions(xerces_3_2_0_lib
    PRIVATE
        HAVE_CONFIG_H=1
)