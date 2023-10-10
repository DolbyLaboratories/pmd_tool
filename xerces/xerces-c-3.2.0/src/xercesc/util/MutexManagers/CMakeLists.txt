target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

if(UNIX)
    target_sources(xerces_3_2_0_lib
        PRIVATE
            PosixMutexMgr.cpp
    )
else()
    target_sources(xerces_3_2_0_lib
        PRIVATE
            WindowsMutexMgr.cpp
    )
endif()