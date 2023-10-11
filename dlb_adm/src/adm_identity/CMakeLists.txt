target_sources(dlb_adm
    PRIVATE
        AdmId.h
        AdmIdSequenceMap.cpp
        AdmIdSequenceMap.h
        AdmIdTranslator.cpp
        AdmIdTranslator.h
)

target_include_directories(dlb_adm
    PRIVATE
        .
)
