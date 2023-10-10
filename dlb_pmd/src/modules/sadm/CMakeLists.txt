set(PMD_MODULES_SADM_SOURCES
    pmd_bitset.h
    pmd_core_model_generator.c
    pmd_core_model_generator.h
    pmd_core_model_ingester.c
    pmd_core_model_ingester.h
    pmd_core_model_types.h
    pmd_sadm_read_buffer.c
    pmd_sadm_read_file.c
    pmd_sadm_write_buffer.c
    pmd_sadm_write_file.c
    pmd_speaker_blkupdt.h
    pmd_speaker_position.h
    sadm_bitstream_encoder.c
    sadm_bitstream_encoder.h
    sadm_bitstream_decoder.c
    sadm_bitstream_decoder.h
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_MODULES_SADM_SOURCES}
)

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_MODULES_SADM_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_MODULES_SADM_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()