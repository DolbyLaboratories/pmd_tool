target_sources(xerces_3_2_0_lib
    PRIVATE
        DOMException.cpp
        DOMLSException.cpp
        DOMRangeException.cpp
        DOMXPathException.cpp
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

add_subdirectory(impl)