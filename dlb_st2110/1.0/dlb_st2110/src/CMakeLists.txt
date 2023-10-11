target_compile_definitions(dlb_st2110
    PUBLIC
        ELPP_NO_DEFAULT_LOG_FILE=1
)

target_compile_features(dlb_st2110 PUBLIC cxx_std_17)

target_include_directories(dlb_st2110
    PRIVATE
        .
)

target_sources(dlb_st2110
    PRIVATE
        dlb_st2110_hardware.cpp
        dlb_st2110_transmitter.cpp
        dlb_st2110_receiver.cpp
        dlb_aoip_services.cpp
        dlb_st2110_sdp.cpp
        mclock.cpp
        sap_discovery.cpp
        rav_discovery.cpp
        nmos_discovery.cpp
        easylogging++.cpp
        dlb_st2110_logging.cpp
        audio_buffer.cpp
)