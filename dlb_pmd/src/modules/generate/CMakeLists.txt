set(PMD_MODULES_GENERATE_SOURCES
    pmd_random_generator.c
    prng_kiss.h
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_MODULES_GENERATE_SOURCES}
)

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_MODULES_GENERATE_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_MODULES_GENERATE_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()