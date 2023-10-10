
if(${OS} MATCHES linux)
    target_sources(portaudio_19_7
        PRIVATE
            alsa/pa_linux_alsa.c
            jack/pa_jack.c
            oss/pa_unix_oss.c
    )
elseif(${OS} MATCHES osx)
    target_sources(portaudio_19_7
        PRIVATE
            coreaudio/pa_mac_core_blocking.c
            coreaudio/pa_mac_core.c
            coreaudio/pa_mac_core_utilities.c
    )
elseif(${OS} MATCHES win32)
    target_sources(portaudio_19_7
        PRIVATE
            dsound/pa_win_ds.c
            dsound/pa_win_ds_dynlink.c
            wasapi/pa_win_wasapi.c
            wdmks/pa_win_wdmks.c
            wmme/pa_win_wmme.c
    )

endif()