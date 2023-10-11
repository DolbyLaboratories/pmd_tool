set(LIBS 
    dlb_pmd
    dlb_wave
    dlb_socket
    dlb_buffer
    dlb_octfile
    portaudio_19_7
)

set(SOURCES 
    pmd_realtime.c
    args.h
    args.c
    buffer.h
    model.h
    md_reader.h
    md_writer.h
    md_http_sender.h
    md_http_listener.h
    pa.h
    pa.c
    pa_play.h
    pa_play.c
    pa_capture.h
    pa_capture.c
    pa_pipe.h
    pa_pipe.c
)

target_link_libraries(pmd_realtime
    PRIVATE
        ${LIBS}
)


target_sources(pmd_realtime
    PRIVATE
        ${SOURCES}
)

target_include_directories(pmd_realtime
    PUBLIC 
        ..
        .
)

if(WIN32)
    target_link_libraries(pmd_realtime_asio
        PRIVATE
            ${LIBS}
    )


    target_sources(pmd_realtime_asio
        PRIVATE
            ${SOURCES}
    )

    target_include_directories(pmd_realtime_asio
        PUBLIC 
            ..
            .
    )
endif()