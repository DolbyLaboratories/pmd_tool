# ---- Portaudio libs ----

add_library(portaudio_19_7)

target_include_directories(portaudio_19_7
    PUBLIC
        include
        ..
)


# ---- OS-specific libs ---- 

set(LIBS rt asound jack pthread m)
set(LIBS_ASIO ${LIBS})

if(APPLE)
    set(LIBS CoreAudio AudioToolbox AudioUnit Cocoa)
    set(LIBS_ASIO ${LIBS})
elseif(WIN32)
    set(LIBS advapi32 ole32)
    set(LIBS_ASIO ${LIBS} user32)
endif()

target_link_libraries(portaudio_19_7
    PRIVATE
        ${LIBS}
)

# ---- OS-specific defs ---- 

set(DEFS "")

if(NOT (${ARCH} MATCHES "other"))
    list(APPEND DEFS PA_LITTLE_ENDIAN=1)
endif()


if(${ARCH} MATCHES x86)
    list(APPEND DEFS
        SIZEOF_SHORT=2
        SIZEOF_INT=4
        SIZEOF_LONG=4
    )
elseif(${ARCH} MATCHES amd64 AND ${OS} MATCHES linux)
    list(APPEND DEFS
        SIZEOF_SHORT=2
        SIZEOF_INT=4
        SIZEOF_LONG=8
    )
elseif(${ARCH} MATCHES amd64 AND ${OS} MATCHES win32)
    list(APPEND DEFS
        SIZEOF_SHORT=2
        SIZEOF_INT=4
        SIZEOF_LONG=4
    )
endif()


if(${OS} MATCHES linux)
    list(APPEND DEFS
        DSTDC_HEADERS=1
        HAVE_SYS_TYPES_H=1
        HAVE_SYS_STAT_H=1
        HAVE_STDLIB_H=1
        DHAVE_STRING_H=1
        HAVE_MEMORY_H=1
        HAVE_STRINGS_H=1
        HAVE_INTTYPES_H=1
        HAVE_STDINT_H=1
        HAVE_UNISTD_H=1
        HAVE_DLFCN_H=1
        HAVE_SYS_SOUNDCARD_H=1
        HAVE_LINUX_SOUNDCARD_H=1
        HAVE_CLOCK_GETTIME=1
        HAVE_NANOSLEEP=1
        PA_USE_ALSA=1
        PA_USE_JACK=1
        PA_USE_OSS=1
        PA_USE_CALLBACK=1
    )
elseif(${OS} MATCHES win32)
    list(APPEND DEFS
        PAWIN_USE_WDMKS_DEVICE_INFO=1
        PA_USE_WMME=1
        PA_USE_DS=0
        PA_USE_WASAPI=1
        PA_USE_WDMKS=1
        PA_USE_CALLBACK=1
    )
elseif(${OS} MATCHES osx)
    list(APPEND DEFS
        DSTDC_HEADERS=1
        HAVE_SYS_TYPES_H=1
        HAVE_SYS_STAT_H=1
        HAVE_STDLIB_H=1
        DHAVE_STRING_H=1
        HAVE_MEMORY_H=1
        HAVE_STRINGS_H=1
        HAVE_INTTYPES_H=1
        HAVE_STDINT_H=1
        HAVE_UNISTD_H=1
        HAVE_DLFCN_H=1
        HAVE_NANOSLEEP=1
        PA_USE_COREAUDIO=1
        PA_USE_CALLBACK=1    
    )
endif()


target_compile_definitions(portaudio_19_7
    PUBLIC
        ${DEFS}
)


add_subdirectory(src)