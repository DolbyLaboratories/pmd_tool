target_sources(xerces_3_2_0_lib
    PRIVATE
        DTDAttDef.cpp
        DTDAttDefList.cpp
        DTDElementDecl.cpp
        DTDEntityDecl.cpp
        DTDGrammar.cpp
        DTDScanner.cpp
        DTDValidator.cpp
        XMLDTDDescriptionImpl.cpp    
)

target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)
