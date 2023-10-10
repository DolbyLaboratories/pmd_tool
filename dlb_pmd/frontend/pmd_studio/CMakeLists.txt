# ----- pmd_studio -----

target_link_libraries(dlb_pmd_studio
    PRIVATE
        dlb_pmd_tool_lib
        dlb_wave

        libui_alpha4_1
        portaudio_19_7
        ember_slim_lib
        dlb_xml
        boost_1_75
        dlb_adm
)

target_sources(dlb_pmd_studio
    PRIVATE
        model.c
        model.h
        pmd_studio_file_menu.h
        pmd_studio_common_defs.h
        pmd_studio.h
        pmd_studio.cpp
        pmd_studio_audio_beds.h
        pmd_studio_audio_beds.cpp
        pmd_studio_audio_objects.h
        pmd_studio_audio_objects.cpp
        pmd_studio_audio_outputs.h
        pmd_studio_audio_outputs.cpp
        pmd_studio_audio_presentations.h
        pmd_studio_audio_presentations.cpp
        pmd_studio_device.h
        pmd_studio_console.h
        pmd_studio_console.cpp
        pmd_studio_console_emberplus.h
        pmd_studio_console_emberplus.cpp
        pmd_studio_settings.h
        pmd_studio_settings.cpp
        pmd_studio_audio_beds_pvt.h
        pmd_studio_audio_outputs_pvt.h
        pmd_studio_audio_objects_pvt.h
        pmd_studio_audio_presentations_pvt.h
        pmd_studio_pvt.h
        pmd_studio_settings_pvt.h
        portaudio/pmd_studio_device.cpp
        portaudio/pmd_studio_device_settings.cpp
        portaudio/pmd_studio_device_pvt.h
        portaudio/am824_framer.h
)

target_compile_definitions(dlb_pmd_studio
    PUBLIC
        LIMITED_MODE=1
)


target_include_directories(dlb_pmd_studio
    PUBLIC
        portaudio
        ..
        .
)

if(${OS} MATCHES osx)
    target_include_directories(dlb_pmd_studio
        PRIVATE
            Darwin
    )

    target_sources(dlb_pmd_studio
        PRIVATE
            Darwin/pmd_studio_services.cpp
            Darwin/get_app_path.m
    )
elseif(${OS} MATCHES linux)
    target_sources(dlb_pmd_studio
        PRIVATE
            Linux/pmd_studio_services.cpp
    )
endif()


# ----- pmd_studio_rivermax -----

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_link_libraries(dlb_pmd_studio_rivermax
        PRIVATE
            dlb_pmd_tool_lib
            dlb_adm
            dlb_wave
            libui_alpha4_1
            portaudio_19_7
            ember_slim_lib
            dlb_xml
            dlb_st2110
            boost_1_75
    )

    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            model.c
            model.h
            pmd_studio_file_menu.h
            pmd_studio_common_defs.h
            pmd_studio.h
            pmd_studio.cpp
            pmd_studio_audio_beds.h
            pmd_studio_audio_beds.cpp
            pmd_studio_audio_objects.h
            pmd_studio_audio_objects.cpp
            pmd_studio_audio_outputs.h
            pmd_studio_audio_outputs.cpp
            pmd_studio_audio_presentations.h
            pmd_studio_audio_presentations.cpp
            pmd_studio_device.h
            pmd_studio_console.h
            pmd_studio_console.cpp
            pmd_studio_console_emberplus.h
            pmd_studio_console_emberplus.cpp
            pmd_studio_settings.h
            pmd_studio_settings.cpp
            pmd_studio_audio_beds_pvt.h
            pmd_studio_audio_outputs_pvt.h
            pmd_studio_audio_objects_pvt.h
            pmd_studio_audio_presentations_pvt.h
            pmd_studio_pvt.h
            pmd_studio_settings_pvt.h
            rivermax/pmd_studio_device.cpp
            rivermax/pmd_studio_device_settings.cpp
            rivermax/pmd_studio_device_pvt.h
    )


    target_include_directories(dlb_pmd_studio_rivermax
        PUBLIC
            rivermax
            ..
            .
    )

    if(${OS} MATCHES osx)
        target_include_directories(dlb_pmd_studio_rivermax
            PRIVATE
                Darwin
        )

        target_sources(dlb_pmd_studio_rivermax
            PRIVATE
                Darwin/pmd_studio_services.cpp
                Darwin/get_app_path.m
        )
    elseif(${OS} MATCHES linux)
        target_sources(dlb_pmd_studio_rivermax
            PRIVATE
                Linux/pmd_studio_services.cpp
        )
    endif()
endif()


add_subdirectory(ember_connector)