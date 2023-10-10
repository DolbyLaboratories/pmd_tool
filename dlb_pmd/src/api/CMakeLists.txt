set(PMD_API_SOURCES
    dlb_pmd.c
    dlb_pmd_api_write.c
    dlb_pmd_api_version.h
    dlb_pmd_api_read.c
    dlb_pmd_equal.c
    dlb_pmd_metadata_set.c
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_API_SOURCES}
)


target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_API_SOURCES}
)


target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_API_SOURCES}
    )


    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()