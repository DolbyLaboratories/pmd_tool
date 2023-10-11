
if(${ARCH} MATCHES amd64)
    target_include_directories(xerces_3_2_0_lib
        PUBLIC
            arm64
    )
else()
    target_include_directories(xerces_3_2_0_lib
        PUBLIC
            x86
    )
endif()

find_package( CURL REQUIRED )

target_link_libraries(xerces_3_2_0_lib
    PRIVATE
        nsl 
        pthread 
        ${CURL_LIBRARIES}
        icui18n
        icuuc
        icudata  
)

target_compile_definitions(xerces_3_2_0_lib
    PUBLIC
        HAVE_CONFIG_H=1
)