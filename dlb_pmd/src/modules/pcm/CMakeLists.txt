set(PMD_MODULES_PCM_SOURCES
    pmd_bitstream.c
    pmd_bitstream.h
    pmd_pcm_augmentor.c
    pmd_pcm_extractor.c
    pmd_smpte_337m.c
    pmd_smpte_337m.h
)

target_sources(dlb_pmd
    PRIVATE
       ${PMD_MODULES_PCM_SOURCES}
)

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
       ${PMD_MODULES_PCM_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
        ${PMD_MODULES_PCM_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()