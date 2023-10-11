target_sources(dlb_adm
    PRIVATE
        XMLConstants.h
        XMLGenerator.cpp
        XMLGenerator.h
        XMLIngester.cpp
        XMLIngester.h
        VersionComparator.cpp
        VersionComparator.h
)

target_include_directories(dlb_adm
    PRIVATE
        .
)
