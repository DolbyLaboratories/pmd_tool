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
target_sources(dlb_adm
    PRIVATE
        AlternativeValueSet.cpp
        AlternativeValueSet.h
        AudioElement.cpp
        AudioElement.h
        AudioObjectInteraction.cpp
        AudioObjectInteraction.h
        AudioTrack.cpp
        AudioTrack.h
        BlockUpdate.cpp
        BlockUpdate.h
        ContentGroup.cpp
        ContentGroup.h
        ComplementaryElement.cpp
        ComplementaryElement.h
        core_model_defs.h
        CoreModel.cpp
        CoreModel.h
        DolbyeEncoderParameters.cpp
        DolbyeEncoderParameters.h
        DolbyeInfo.cpp
        DolbyeInfo.h
        DolbyeProgram.cpp
        DolbyeProgram.h
        dlb_adm_core_model.cpp
        dlb_adm_core_model.h
        ElementGroup.cpp
        ElementGroup.h
        ElementRecord.cpp
        ElementRecord.h
        ElementTable.cpp
        ElementTable.h
        EntityName.cpp
        EntityName.h
        FrameFormat.cpp
        FrameFormat.h
        Gain.cpp
        Gain.h
        LoudnessMetadata.cpp
        LoudnessMetadata.h
        ModelEntity.cpp
        ModelEntity.h
        ModelEntityContainer.cpp
        ModelEntityContainer.h
        Position.cpp
        Position.h
        Presentation.cpp
        Presentation.h
        PresentationRecord.cpp
        PresentationRecord.h
        PresentationTable.cpp
        PresentationTable.h
        ProfileDescriptor.cpp
        ProfileDescriptor.h
        Source.cpp
        Source.h
        SourceGroup.cpp
        SourceGroup.h
        SourceRecord.cpp
        SourceRecord.h
        SourceTable.h
        Target.cpp
        Target.h
        TargetGroup.cpp
        TargetGroup.h
        VersionComparator.cpp
        VersionComparator.h        
        UpdateRecord.cpp
        UpdateRecord.h
        UpdateTable.cpp
        UpdateTable.h
)

target_include_directories(dlb_adm
    PRIVATE
        .
)
