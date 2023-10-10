set(PMD_MODULES_KLV_SOURCES
    klv_aen.h
    klv_apn.h
    klv_eep.h
    klv_etd.h
    klv_pld.h
    klv_version.h
    pmd_klv_reader.c
    klv.h
    klv_aod.h
    klv_bitfield_helpers.h
    klv_esd.h
    klv_hed.h
    klv_reader.h
    klv_writer.h
    pmd_crc32.c
    pmd_klv_writer.c
    klv_abd.h
    klv_apd.h
    klv_container_config.h
    klv_esn.h
    klv_iat.h
    klv_speaker_config.h
    klv_xyz.h
    pmd_crc32.h
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_MODULES_KLV_SOURCES}
)

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_MODULES_KLV_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_MODULES_KLV_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()