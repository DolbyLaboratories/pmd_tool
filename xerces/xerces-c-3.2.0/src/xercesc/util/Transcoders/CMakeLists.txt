target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

if(UNIX)
    if(${OS} MATCHES osx)
        add_subdirectory(MacOSUnicodeConverter)
    else()
        add_subdirectory(ICU)
        add_subdirectory(IconvGNU)
        add_subdirectory(Iconv)
    endif()
else()
    add_subdirectory(Win32)
endif()