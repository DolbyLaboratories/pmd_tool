# ----- pmd_tool_lib -----

target_link_libraries(dlb_pmd_tool_lib
PRIVATE
    dlb_pmd
    dlb_adm
    dlb_wave
    dlb_buffer
    dlb_octfile
)

target_sources(dlb_pmd_tool_lib
PRIVATE
    pmd_tool_build_version.h
    pmd_tool.c
    xml.h
    xml.c
    pmd_tool_klv.h
    pmd_tool_klv.c
    pcm.h
    pcm.c
    pmd_tool_main.c
)


target_include_directories(dlb_pmd_tool_lib
    PUBLIC
        ..
        .
)

# ----- pmd_tool -----

target_link_libraries(dlb_pmd_tool
PRIVATE
    dlb_pmd_tool_lib
)

target_sources(dlb_pmd_tool
PRIVATE
    pmd_tool_main.c
)


target_include_directories(dlb_pmd_tool_lib
    PUBLIC
        ..
        .
)