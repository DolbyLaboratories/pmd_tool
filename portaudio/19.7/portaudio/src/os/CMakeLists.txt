if(${OS} MATCHES win32)
    target_sources(portaudio_19_7
        PRIVATE
            win/pa_win_coinitialize.c
            win/pa_win_hostapis.c
            win/pa_win_waveformat.c
            win/pa_win_wdmks_utils.c
            win/pa_x86_plain_converters.c
            win/pa_win_util.c
    )

    target_include_directories(portaudio_19_7
    PRIVATE
        win
    )


else()
    target_sources(portaudio_19_7
        PRIVATE
            unix/pa_unix_hostapis.c
            unix/pa_unix_util.c
    )

    target_include_directories(portaudio_19_7
        PRIVATE
            unix
    )
endif()