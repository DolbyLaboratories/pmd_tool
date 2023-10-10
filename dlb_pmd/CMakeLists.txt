# ---- main dlb_pmd ----

add_library(dlb_pmd)

target_include_directories(dlb_pmd
    PUBLIC
        include
        ..
    PRIVATE
        .
)

target_link_libraries(dlb_pmd
    PRIVATE
        dlb_adm
        dlb_xml
        zlib
)

# ---- pmd_realtime ---- (DEPRECATED)

# ---- pmd_tool ----

add_executable(dlb_pmd_tool)

# ---- pmd_tool_lib -----
add_library(dlb_pmd_tool_lib)

target_include_directories(dlb_pmd_tool_lib
    PUBLIC
        include
        ..
)

# ---- pmd_studio ----

add_executable(dlb_pmd_studio)

target_compile_features(dlb_pmd_studio PUBLIC cxx_std_17)

target_include_directories(dlb_pmd_studio
    PUBLIC
        include
        ..
        PRIVATE
        .
)

# ---- pmd_studio_rivermax (optional) ----

if(BUILD_PMD_STUDIO_RIVERMAX)
    add_executable(dlb_pmd_studio_rivermax)

    target_compile_features(dlb_pmd_studio_rivermax PUBLIC cxx_std_11)

    target_include_directories(dlb_pmd_studio_rivermax
        PUBLIC
            include
            ..
            PRIVATE
            .
    )
endif()

add_subdirectory(os)
add_subdirectory(src)
add_subdirectory(frontend)
add_subdirectory(test)
add_subdirectory(unit_test)