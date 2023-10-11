if(UNIX)
    target_sources(xerces_3_2_0_lib
        PRIVATE
            PosixFileMgr.cpp
    )
else()
    target_sources(xerces_3_2_0_lib
        PRIVATE
            WindowsFileMgr.cpp
    )
endif()

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)