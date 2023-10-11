add_library(dlb_socket)

target_include_directories(dlb_socket
    PUBLIC
        include
        ..
)

set(OS_IMPLEMENTATION_DIR include/posix)

if(WIN32)
    set(OS_IMPLEMENTATION_DIR include/win32)
elseif(APPLE)
    set(OS_IMPLEMENTATION_DIR include/osx)
endif()

target_include_directories(dlb_socket
    PUBLIC
        ${OS_IMPLEMENTATION_DIR}
)

add_subdirectory(src)