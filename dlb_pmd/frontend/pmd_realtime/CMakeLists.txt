#/************************************************************************
# * Copyright (c) 2023-2025, Dolby Laboratories Inc.
# * Copyright (c) 2025-2025, Dolby International AB.
# * All rights reserved.
# * 
# * Redistribution and use in source and binary forms, with or without
# * modification, are permitted provided that the following conditions
# * are met:
# * 
# * 1. Redistributions of source code must retain the above copyright
# *    notice, this list of conditions and the following disclaimer.
# *
# * 2. Redistributions in binary form must reproduce the above
# *    copyright notice, this list of conditions and the following
# *    disclaimer in the documentation and/or other materials provided
# *    with the distribution.
# *
# * 3. Neither the name of the copyright holder nor the names of its
# *    contributors may be used to endorse or promote products derived
# *    from this software without specific prior written permission.
# *
# * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# * 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# * OF THE POSSIBILITY OF SUCH DAMAGE.
# **********************************************************************/
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