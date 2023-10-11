set(PMD_MODEL_SOURCES
    pmd_model.h
    pmd_model.c
    pmd_model_combo.c
    pmd_profile.h
    pmd_error_helper.h
    pmd_bitstream_version.h
    pmd_channels.h
    pmd_idmap.h
    pmd_language.h
    pmd_language.c
    pmd_signals.h
    pmd_strings.h
    pmd_xyz_set.h
    pmd_types.h
    pmd_smpte2109.h
    pmd_abd_aod.h
    pmd_aen.h
    pmd_apd.h
    pmd_apn.h
    pmd_eep.h
    pmd_esd.h
    pmd_etd.h
    pmd_hed.h
    pmd_iat.h
    pmd_pld.h
    pmd_xyz.h
    DlbPmdModelWrapper.cpp
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_MODEL_SOURCES}
)   

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_MODEL_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_MODEL_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()