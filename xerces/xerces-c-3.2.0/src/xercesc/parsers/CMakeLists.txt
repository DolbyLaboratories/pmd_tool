target_sources(xerces_3_2_0_lib
    PRIVATE
        AbstractDOMParser.cpp
        DOMLSParserImpl.cpp
        SAX2XMLFilterImpl.cpp
        SAX2XMLReaderImpl.cpp
        SAXParser.cpp
        XercesDOMParser.cpp
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)