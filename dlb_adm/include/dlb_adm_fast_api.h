/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

#ifndef DLB_ADM_FAST_API_H
#define DLB_ADM_FAST_API_H

#include "dlb_adm/include/dlb_adm_data_types.h"
#include "dlb_adm/include/dlb_adm_lib_dll.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Audio programme label
 */
typedef struct
{
    char language_tag       [DLB_ADM_DATA_LANG_SZ];
    char presentation_label [DLB_ADM_DATA_NAME_SZ];
}dlb_adm_presentation_label;

typedef struct
{
    dlb_adm_entity_id element_id;
    dlb_adm_entity_id content_group_id;
}dlb_adm_element_ids;


/**
 * @brief Add audio element with all dependencies requered by S-ADM emission profile (audio content, audio tracks, etc.)
 * 
 * NOTE: dlb_adm_data_names should be pre-allocated (dlb_adm_core_model_query_names_memory_size() should be used to get memory size)
 * and configured with dlb_adm_core_model_configure_names()
 *
 * @param model          core model instance
 * @param speaker_config speaker config of audio element (2.0, 5.1, etc)
 * @param content_kind   audio element content kind (dialogue, complete main, etc)
 * @param language_tag   audio content language tag
 * @param sources        list of audio element sources ids
 * @param num_sources    number of sources
 * @param names          pre-allocated structure used for names and labels storing  
 * @return entity id if audio element was added, DLB_ADM_NULL_ENTITY_ID otherwise
 */
DLB_ADM_DLL_ENTRY
dlb_adm_element_ids
dlb_adm_add_audio_element
    (dlb_adm_core_model         *model
    ,DLB_ADM_SPEAKER_CONFIG      speaker_config
    ,DLB_ADM_CONTENT_KIND        content_kind
    ,char                        language_tag[DLB_ADM_DATA_LANG_SZ]    
    ,unsigned int               *sources
    ,unsigned int                num_sources
    ,dlb_adm_data_names         *names  
    );


/**
 * @brief Add audio programme
 *
 * @param model          core model instance
 * @param language_tag   audio programme language tag
 * @param labels         audio programme labels
 * @param num_labels     number of labels
 * @param names          initialized container ready to store required number of names and labels
 * @return entity id if audio programme was added, DLB_ADM_NULL_ENTITY_ID otherwise
 */
DLB_ADM_DLL_ENTRY
dlb_adm_entity_id
dlb_adm_add_audio_programme
    (dlb_adm_core_model         *model
    ,char                        programme_name[DLB_ADM_DATA_NAME_SZ]
    ,char                        language_tag[DLB_ADM_DATA_LANG_SZ]
    ,dlb_adm_presentation_label *labels
    ,unsigned int                num_labels
    ,dlb_adm_data_names         *names
    );


/**
 * @brief Add audio element to audio programme
 *
 * @param model               core model instance
 * @param audio_programm_id   audio programme id
 * @param audio_content_id    audio content id associated with audio element
 * @param audio_element_id    audio element id
 * @return DLB_ADM_STATUS_OK on success
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_add_element_to_programme
    (dlb_adm_core_model         *model
    ,dlb_adm_entity_id           audio_programm_id
    ,dlb_adm_entity_id           audio_content_id
    ,dlb_adm_entity_id           audio_element_id
    );

#ifdef __cplusplus
}
#endif

#endif /* DLB_ADM_FAST_API_H */