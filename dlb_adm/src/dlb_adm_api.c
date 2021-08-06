/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm_api_pvt.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef _WIN32
#define MAX_COMMON_DEFS_PATH_LEN (260)
#else
#define MAX_COMMON_DEFS_PATH_LEN (1023)
#endif

#define MAX_COMMON_DEFS_PATH_BUFFER_SIZE (MAX_COMMON_DEFS_PATH_LEN + 1)

static char the_adm_common_defs_path[MAX_COMMON_DEFS_PATH_BUFFER_SIZE];

static dlb_adm_bool common_defs_path_initialized = DLB_ADM_FALSE;

const char *
dlb_adm_get_common_defs_path
    (
    )
{
    if (!common_defs_path_initialized)
    {
        snprintf(the_adm_common_defs_path, MAX_COMMON_DEFS_PATH_LEN, "%s", "common_adm_def_v9.xml");
        common_defs_path_initialized = DLB_ADM_TRUE;
    }

    return the_adm_common_defs_path;
}

int
dlb_adm_configure
    (const dlb_adm_library_config   *config
    )
{
    if (config == NULL)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    if (config->path_to_common_defs != NULL)
    {
        size_t n = strlen(config->path_to_common_defs);

        if (n > MAX_COMMON_DEFS_PATH_LEN)
        {
            return DLB_ADM_STATUS_ERROR;
        }

        strcpy(the_adm_common_defs_path, config->path_to_common_defs);
        common_defs_path_initialized = DLB_ADM_TRUE;
    }

    return DLB_ADM_STATUS_OK;
}

DLB_ADM_OBJECT_CLASS
dlb_adm_translate_content_kind
    (DLB_ADM_CONTENT_KIND contentKind
    )
{
    DLB_ADM_OBJECT_CLASS c = DLB_ADM_OBJECT_CLASS_GENERIC;

    switch (contentKind)
    {
    case DLB_ADM_CONTENT_KIND_NK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_NK_MUSIC:
    case DLB_ADM_CONTENT_KIND_NK_EFFECTS:
        c = DLB_ADM_OBJECT_CLASS_GENERIC;
        break;

    case DLB_ADM_CONTENT_KIND_DK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_DK_DIALOGUE:
    case DLB_ADM_CONTENT_KIND_DK_COMMENTARY:
        c = DLB_ADM_OBJECT_CLASS_DIALOG;
        break;

    case DLB_ADM_CONTENT_KIND_DK_VOICEOVER:
        c = DLB_ADM_OBJECT_CLASS_VOICEOVER;
        break;

    case DLB_ADM_CONTENT_KIND_DK_SUBTITLE:
        c = DLB_ADM_OBJECT_CLASS_SUBTITLE;
        break;

    case DLB_ADM_CONTENT_KIND_DK_DESCRIPTION:
        c = DLB_ADM_OBJECT_CLASS_VDS;
        break;

    case DLB_ADM_CONTENT_KIND_DK_EMERGENCY:
        c = DLB_ADM_OBJECT_CLASS_EMERGENCY_ALERT;
        break;

    case DLB_ADM_CONTENT_KIND_MK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN:
    case DLB_ADM_CONTENT_KIND_MK_MIXED:
    case DLB_ADM_CONTENT_KIND_MK_HEARING_IMPAIRED:
        c = DLB_ADM_OBJECT_CLASS_GENERIC;
        break;

    default:
        break;
    }

    return c;
}

int
dlb_adm_core_model_query_names_memory_size
    (size_t                     *sz
    ,size_t                      max_name_sz
    ,size_t                      name_limit
    )
{
    /* TODO: alignment? */

    size_t string_ptr_array_size;
    size_t string_storage_size;

    if (sz == NULL)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((max_name_sz < DLB_ADM_DATA_NAME_SZ_MIN) || (name_limit == 0))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    string_ptr_array_size = name_limit * sizeof(char *);
    string_storage_size = name_limit * (max_name_sz + DLB_ADM_DATA_LANG_SZ);
    *sz = string_ptr_array_size * 2 + string_storage_size;  /* name + language == 2 */

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_configure_names
    (dlb_adm_data_names         *names
    ,size_t                      name_limit
    ,char                       *memory
    ,size_t                      memory_size
    )
{
    /* TODO: alignment? */

    size_t min_memory_size;
    size_t string_ptr_array_size;
    size_t pair_size;
    size_t name_size;
    size_t i;
    char *p = memory;

    if ((names == NULL) || (memory == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    min_memory_size = name_limit * (2 * sizeof(char *) + DLB_ADM_DATA_NAME_SZ_MIN + DLB_ADM_DATA_LANG_SZ);
    if (name_limit == 0 || memory_size < min_memory_size)
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }
    memset(names, 0, sizeof(*names));
    memset(memory, 0, memory_size);

    string_ptr_array_size = name_limit * sizeof(char *);
    names->names = (char **)p;
    p += string_ptr_array_size;
    names->langs = (char **)p;
    p += string_ptr_array_size;
    names->string_storage = p;
    names->string_storage_size = (unsigned int)(memory_size - 2 * string_ptr_array_size);
    pair_size = names->string_storage_size / name_limit;
    name_size = pair_size - DLB_ADM_DATA_LANG_SZ;
    names->name_limit = (unsigned int)name_limit;
    names->max_name_size = (unsigned int)name_size;
    for (i = 0/*, p = names->string_storage*/; i < name_limit; i++)
    {
        names->names[i] = p;
        p += name_size;
        names->langs[i] = p;
        p += DLB_ADM_DATA_LANG_SZ;
    }

    return DLB_ADM_STATUS_OK;
}

static
void
add_name
    (dlb_adm_data_names         *names
    ,const char                 *name
    ,const char                 *lang
    )
{
    snprintf(names->names[names->name_count], names->max_name_size, "%s", name);
    if ((lang != NULL) && (*lang != '\0'))
    {
        snprintf(names->langs[names->name_count], DLB_ADM_DATA_LANG_SZ, "%s", lang);
    }
    names->name_count++;
}

int
dlb_adm_core_model_add_name
    (dlb_adm_data_names         *names
    ,const char                 *name
    ,const char                 *lang
    )
{
    int status = DLB_ADM_STATUS_OUT_OF_MEMORY;

    if ((names == NULL) || (name == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (*name == '\0')
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }

    if (names->name_count > 0)  // Only one name allowed
    {
        return DLB_ADM_STATUS_ERROR;
    }

    if (names->name_count < names->name_limit)
    {
        add_name(names, name, lang);
        status = DLB_ADM_STATUS_OK;
    }

    return status;
}

int
dlb_adm_core_model_add_label
    (dlb_adm_data_names         *names
    ,const char                 *label
    ,const char                 *lang
    )
{
    int status = DLB_ADM_STATUS_OUT_OF_MEMORY;

    if ((names == NULL) || (label == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (*label == '\0')
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }

    if (names->name_count < names->name_limit)
    {
        add_name(names, label, lang);
        names->label_count++;
        status = DLB_ADM_STATUS_OK;
    }

    return status;
}

int
dlb_adm_core_model_has_name
    (dlb_adm_bool               *has_name
    ,dlb_adm_data_names         *names
    )
{
    if ((has_name == NULL) || (names == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *has_name = (names->name_count > names->label_count);

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_clear_names
    (dlb_adm_data_names         *names
    )
{
    if (names == NULL)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    memset(names->string_storage, 0, names->string_storage_size);
    names->name_count = 0;
    names->label_count = 0;

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_query_element_data_memory_size
    (size_t                     *sz
    ,dlb_adm_channel_count       channel_capacity
    )
{
    static const size_t channel_sz =
        sizeof(dlb_adm_data_target) +
        sizeof(dlb_adm_data_audio_track) +
        sizeof(dlb_adm_data_source) +
        sizeof(dlb_adm_data_block_update);

    if (sz == NULL)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *sz = 0;

    if (channel_capacity == 0)
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }
    *sz = channel_sz * channel_capacity;

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_configure_element_data
    (dlb_adm_data_audio_element_data    *element_data
    ,dlb_adm_channel_count               channel_capacity
    ,uint8_t                            *memory
    )
{
    uint8_t *p = memory;
    size_t memory_sz;
    int status;
    
    if ((element_data == NULL) || (memory == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = dlb_adm_core_model_query_element_data_memory_size(&memory_sz, channel_capacity);
    if (status != DLB_ADM_STATUS_OK)
    {
        return status;
    }
    
    memset(element_data, 0, sizeof(*element_data));
    memset(memory, 0, memory_sz);

    element_data->targets = (dlb_adm_data_target *)p;
    p += sizeof(dlb_adm_data_target) * channel_capacity;
    element_data->audio_tracks = (dlb_adm_data_audio_track *)p;
    p += sizeof(dlb_adm_data_audio_track) * channel_capacity;
    element_data->sources = (dlb_adm_data_source *)p;
    p += sizeof(dlb_adm_data_source) * channel_capacity;
    element_data->block_updates = (dlb_adm_data_block_update *)p;

    element_data->channel_capacity = channel_capacity;
    element_data->array_storage = memory;

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_clear_element_data
    (dlb_adm_data_audio_element_data    *element_data
    )
{
    if (element_data == NULL)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return dlb_adm_core_model_configure_element_data(
	    element_data, element_data->channel_capacity, element_data->array_storage);
}

int
dlb_adm_core_model_query_presentation_data_memory_size
    (size_t                     *sz
    ,dlb_adm_element_count       element_capacity
    )
{
    static const size_t element_sz =
        sizeof(dlb_adm_data_content_group) +
        sizeof(dlb_adm_data_element_group) +
        sizeof(dlb_adm_data_audio_element);

    if (sz == NULL)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *sz = 0;

    if (element_capacity == 0)
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }
    *sz = element_sz * element_capacity;

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_configure_presentation_data
    (dlb_adm_data_presentation_data     *presentation_data
    ,dlb_adm_element_count               element_capacity
    ,uint8_t                            *memory
    )
{
    uint8_t *p = memory;
    size_t memory_sz;
    int status;

    if ((presentation_data == NULL) || (memory == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = dlb_adm_core_model_query_presentation_data_memory_size(&memory_sz, element_capacity);
    if (status != DLB_ADM_STATUS_OK)
    {
        return status;
    }

    memset(presentation_data, 0, sizeof(*presentation_data));
    memset(memory, 0, memory_sz);

    presentation_data->content_groups = (dlb_adm_data_content_group *)p;
    p += sizeof(dlb_adm_data_content_group) * element_capacity;
    presentation_data->element_groups = (dlb_adm_data_element_group *)p;
    p += sizeof(dlb_adm_data_element_group) * element_capacity;
    presentation_data->audio_elements = (dlb_adm_data_audio_element *)p;
    /*p += sizeof(dlb_adm_data_audio_element) * element_capacity;*/

    presentation_data->element_capacity = element_capacity;
    presentation_data->array_storage = memory;

    return DLB_ADM_STATUS_OK;
}

int
dlb_adm_core_model_clear_presentation_data
    (dlb_adm_data_presentation_data     *presentation_data
    )
{
    if (presentation_data == NULL)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return dlb_adm_core_model_configure_presentation_data(
        presentation_data, presentation_data->element_capacity, presentation_data->array_storage);
}

#define FIRST_CHUNK_LEN (1023)
#define FIRST_CHUNK_SIZE (FIRST_CHUNK_LEN + 1)

int
dlb_adm_file_is_sadm_xml
    (dlb_adm_bool           *is_sadm    /**< [out] the result */
    ,const char             *filename   /**< [in]  the file to check */
    )
{
    char buffer[FIRST_CHUNK_SIZE];
    size_t count;
    FILE *f;

    if ((is_sadm == NULL) || (filename == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *is_sadm = DLB_ADM_FALSE;

    if (filename[0] == '\0')
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    f = fopen(filename, "r");
    if (f == NULL)
    {
        return DLB_ADM_STATUS_NOT_FOUND;
    }

    memset(buffer, 0, sizeof(buffer));
    count = fread(buffer, 1, FIRST_CHUNK_LEN, f);
    (void)fclose(f);
    if (count == FIRST_CHUNK_LEN)
    {
        static const char *tag1 = "<frame>";
        static const char *tag2 = "<frameheader>";
        const size_t tag1_len = strlen(tag1);
        char *p = buffer;
        size_t i;

        for (i = 0; i < FIRST_CHUNK_LEN; i++)
        {
            char c = *p;
            *p++ = (char)tolower(c);
        }

        p = strstr(buffer, tag1);
        if (NULL != p)
        {
            if (NULL != strstr(p + tag1_len, tag2))
            {
                *is_sadm = DLB_ADM_TRUE;
            }
        }
    }

    return DLB_ADM_STATUS_OK;
}
