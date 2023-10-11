target_sources(xerces_3_2_0_lib
    PRIVATE
        Dummy.cpp
        InputSource.cpp
        SAXException.cpp
        SAXParseException.cpp
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)