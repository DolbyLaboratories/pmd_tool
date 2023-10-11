target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

target_sources(xerces_3_2_0_lib
    PRIVATE
        BinHTTPInputStreamCommon.cpp
)

if(UNIX)
    add_subdirectory(Socket)
    add_subdirectory(Curl)

    if(${OS} MATCHES osx)
        add_subdirectory(MacOSURLAccessCF)
    endif()
else()
    add_subdirectory(WinSock)
endif()