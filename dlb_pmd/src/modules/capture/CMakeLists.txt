target_sources(dlb_pmd
    PRIVATE
        pmd_capture.c
)

target_sources(dlb_pmd_studio
    PRIVATE
        pmd_capture.c
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            pmd_capture.c
    )
endif()