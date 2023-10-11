add_subdirectory(adm_xml)
add_subdirectory(core_model)
add_subdirectory(adm_identity)
add_subdirectory(adm_transformer)

target_sources(dlb_adm
    PRIVATE
        dlb_adm_api_pp.cpp
        dlb_adm_api.c
        dlb_adm_fast_api.cpp
)

target_include_directories(dlb_adm
    PRIVATE
        .
)
