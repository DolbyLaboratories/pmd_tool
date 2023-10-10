target_link_libraries(xerces_3_2_0_lib
    PRIVATE
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

target_compile_definitions(xerces_3_2_0_lib
    PRIVATE
        HAVE_CONFIG_H=1
)