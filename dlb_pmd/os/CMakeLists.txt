target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()

if(APPLE)
    add_subdirectory(osx)
elseif(WIN32)
    add_subdirectory(windows)
else()
    add_compile_definitions(_GNU_SOURCE)
    add_subdirectory(linux)
endif()