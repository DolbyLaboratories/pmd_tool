add_subdirectory(api)
add_subdirectory(model)
add_subdirectory(modules)

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