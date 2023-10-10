
add_library(xerces_3_2_0_lib)

target_compile_definitions(xerces_3_2_0_lib
    PUBLIC
        XERCES_BUILDING_LIBRARY=1
)

if(${OS} MATCHES win32)
    add_subdirectory(windows)
elseif(${OS} MATCHES osx)
    add_subdirectory(osx)
else()
    add_subdirectory(linux)
endif()

add_subdirectory(src)