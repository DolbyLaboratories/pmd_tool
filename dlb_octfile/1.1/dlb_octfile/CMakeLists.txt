add_library(dlb_octfile)

target_include_directories(dlb_octfile
    PUBLIC
        include
        ..
)

add_subdirectory(src)