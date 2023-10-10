add_library(dlb_wave)

target_link_libraries(dlb_wave
    PRIVATE
        dlb_octfile
        dlb_buffer
)

target_include_directories(dlb_wave
    PUBLIC
        include
        ..
)

add_subdirectory(src)