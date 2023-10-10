set(SOCKET_IMPL_DIR posix)

if(WIN32)
    set(SOCKET_IMPL_DIR win32)
elseif(APPLE)
    set(SOCKET_IMPL_DIR osx)
endif()

target_include_directories(dlb_socket
    PRIVATE
        ${SOCKET_IMPL_DIR}
)

target_sources(dlb_socket
    PRIVATE
        dlb_socket.c
        dlb_http_client.c
        dlb_http_server.c
        ${SOCKET_IMPL_DIR}/dlb_socket_impl.h
)
