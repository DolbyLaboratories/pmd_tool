target_sources(xerces_3_2_0_lib
    PRIVATE
        stricmp.c
        strnicmp.c
        towlower.c
        towupper.c
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

add_subdirectory(xercesc)