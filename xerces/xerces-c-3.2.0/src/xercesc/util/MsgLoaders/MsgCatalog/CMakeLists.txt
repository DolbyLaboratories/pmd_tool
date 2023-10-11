target_include_directories(xerces_3_2_0_lib
    PUBLIC
        .
)

target_sources(xerces_3_2_0_lib
    PRIVATE
        MsgCatalogLoader.cpp
)