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
set(PMD_MODEL_SOURCES
    pmd_model.h
    pmd_model.c
    pmd_model_combo.c
    pmd_profile.h
    pmd_error_helper.h
    pmd_bitstream_version.h
    pmd_channels.h
    pmd_idmap.h
    pmd_language.h
    pmd_language.c
    pmd_signals.h
    pmd_strings.h
    pmd_xyz_set.h
    pmd_types.h
    pmd_smpte2109.h
    pmd_abd_aod.h
    pmd_aen.h
    pmd_apd.h
    pmd_apn.h
    pmd_eep.h
    pmd_esd.h
    pmd_etd.h
    pmd_hed.h
    pmd_iat.h
    pmd_pld.h
    pmd_xyz.h
    DlbPmdModelWrapper.cpp
)

target_sources(dlb_pmd
    PRIVATE
        ${PMD_MODEL_SOURCES}
)   

target_include_directories(dlb_pmd
    PRIVATE
        .
)

target_sources(dlb_pmd_studio
    PRIVATE
        ${PMD_MODEL_SOURCES}
)

target_include_directories(dlb_pmd_studio
    PRIVATE
        .
)

if(BUILD_PMD_STUDIO_RIVERMAX)
    target_sources(dlb_pmd_studio_rivermax
        PRIVATE
            ${PMD_MODEL_SOURCES}
    )

    target_include_directories(dlb_pmd_studio_rivermax
        PRIVATE
            .
    )
endif()