add_library(dlb_st2110)

target_compile_features(dlb_st2110 PUBLIC cxx_std_17)

target_include_directories(dlb_st2110
    PUBLIC 
        include
        ${RIVERMAX_API_INCLUDE_DIR}
        ..
)

target_link_libraries(dlb_st2110
    PRIVATE
        dlb_nmos_node    
        rivermax
        stdc++fs
        avahi-client
        avahi-common
)

add_subdirectory(src)