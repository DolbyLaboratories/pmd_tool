target_sources(portaudio_19_7
    PRIVATE
        pa_allocation.c
        pa_converters.c
        pa_cpuload.c
        pa_debugprint.c
        pa_dither.c
        pa_front.c
        pa_process.c
        pa_ringbuffer.c
        pa_stream.c
        pa_trace.c
)

target_include_directories(portaudio_19_7
    PRIVATE
        .
)

