target_sources(dlb_pmd
    PRIVATE
        pmd_ctrl_c.c
)

target_sources(dlb_pmd_studio
    PRIVATE
        pmd_ctrl_c.c
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            pmd_ctrl_c.c
    )
endif()