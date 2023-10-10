target_sources(xerces_3_2_0_lib
    PRIVATE
        XIncludeDOMDocumentProcessor.cpp
        XIncludeLocation.cpp
        XIncludeUtils.cpp
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)
