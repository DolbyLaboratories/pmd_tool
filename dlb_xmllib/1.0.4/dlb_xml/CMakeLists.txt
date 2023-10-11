add_library(dlb_xml)

target_include_directories(dlb_xml
    PUBLIC
        include
        ..
)

add_subdirectory(src)
