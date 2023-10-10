set(PMD_MODULES_XML_SOURCES
    pmd_xml_reader.c
    pmd_xml_writer.c
    pmd_xml_read_file.c
    pmd_xml_read_string.c
    pmd_xml_write_file.c
    pmd_xml_write_string.c
    xml_hex.h
    xml_uuid.h
    xml_eidr.h
    xml_ad_id.h
    xml_cdata.h
    parser_tagstack.h
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_MODULES_XML_SOURCES}
)

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_MODULES_XML_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_MODULES_XML_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()