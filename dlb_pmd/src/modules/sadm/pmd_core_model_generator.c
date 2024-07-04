/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021-2024, Dolby Laboratories Inc.
 * Copyright (c) 2021-2024, Dolby International AB.
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

#include "pmd_core_model_generator.h"
#include "pmd_error_helper.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "xml_uuid.h"

#include <string.h>
#include <stdio.h>

#include "pmd_speaker_blkupdt.h"
#include "pmd_bitset.h"

#ifdef NDEBUG
#define FAILURE PMD_FAIL
#define CHECK_STATUS(s)         if ((s) != DLB_ADM_STATUS_OK) return (s)
#else
static int ret_status(int s)
{
    return s;           // Put a breakpoint here
}
static dlb_pmd_success ret_fail()
{
    return PMD_FAIL;    // Put a breakpoint here
}
#define FAILURE ret_fail()
#define CHECK_STATUS(s)         if ((s) != DLB_ADM_STATUS_OK) return ret_status(s)
#endif

#define CHECK_STATUS_SUCCESS(s) if ((s) != DLB_ADM_STATUS_OK) return FAILURE
#define CHECK_SUCCESS(s)        if ((s) != PMD_SUCCESS) return FAILURE

/**
 * @def MAX_BED_CHANNEL_COUNT
 * @brief maximum channel count for an Atmos bed
 */
#define MAX_BED_CHANNEL_COUNT (9 + 1 + 6)

/**
 * @def MAX_NAME_COUNT
 * @brief maximum number of name + labels for one presentation
 */
#define MAX_NAME_COUNT DLB_PMD_MAX_PRESENTATION_NAMES

/**
 * @def NAMES_ARRAY_SIZE
 * @brief maximum possible number of names/labels + language codes for any object type
 */
#define NAMES_ARRAY_SIZE (MAX_NAME_COUNT * 2)

/**
 * @def NAMES_STORAGE_SIZE
 * @brief bytes needed to store all the names/labels/language codes
 */
#define NAMES_STORAGE_SIZE (MAX_NAME_COUNT * (DLB_ADM_DATA_NAME_SZ + DLB_ADM_DATA_LANG_SZ))

/**
 * @def ID_STRING_SIZE
 * @brief maximum size of an ADM identifier in string form, plus one for NUL termination
 */
#define ID_STRING_SIZE (21)

/**
 * @def PMD_SOURCE_GROUP_ID
 * @brief the ID number for the PMD "audio interface" source group
 */
#define PMD_SOURCE_GROUP_ID (1)

/**
 * @def PMD_SOURCE_GROUP_NAME
 * @brief the name for the PMD "audio interface" source group
 */
static const char PMD_SOURCE_GROUP_NAME[] = "Audio Interface";

/**
 * @def SPEAKER_CONFIG_CHANNEL_TOTAL
 * @brief the sum of the channel counts for all of the standard bed speaker configurations
 * (2.0 + 3.0 + 5.1 + ...); the sum of the numbers in the SPEAKER_CONFIG_COUNT table, below.
 */
#define SPEAKER_CONFIG_CHANNEL_TOTAL (72)

#define PMD_BED_CLASS_COUNT (3)

#define PMD_LANG_SIZE (4)

/**
 * @brief languages of presentations labels
 *
 */
typedef struct dlb_pmd_pres_language_tag
{
    char language[PMD_LANG_SIZE];                   /**< ISO-639-1/2 language of name */
} dlb_pmd_pres_language_tag;

/**
 * @brief array of presentation labels language tags
 */
typedef dlb_pmd_pres_language_tag language_tags[DLB_PMD_MAX_PRESENTATION_NAMES];


/**
 * @brief Core model generator
 */
struct pmd_core_model_generator
{
    dlb_adm_core_model  *core_model;                                        /* The core model instance to generate */
    const dlb_pmd_model *pmd_model;                                         /* The PMD model instance to parse */

    dlb_adm_entity_id    bed_target_groups[NUM_PMD_SPEAKER_CONFIGS + 1];    /* TargetGroup IDs for standard bed channels */
    dlb_adm_entity_id    bed_targets[SPEAKER_CONFIG_CHANNEL_TOTAL];         /* Target IDs for standard bed channels */

    dlb_adm_entity_id    source_group_id;                                   /* The SourceGroup ID for the PMD "audio interface" */
    dlb_adm_entity_id    sources[DLB_PMD_MAX_SIGNALS + 1];                  /* Source IDs for audio signals */
                                                                            /* - (signals start at 1, we'll just never use 0) */
    dlb_adm_data_names   names;                                             /* Names data structure */
    char                *names_memory;                                      /* Memory for names data structure */

    dlb_adm_entity_id    current_audio_element_id;                          /* The ID for the current audio element during parsing */
    language_tags        lang_tags;                                         /* Language tags used in presentation labels */
    unsigned int         num_lang_tag;                                      /* Language tags amount */
};

/**
 * @brief define a faux 7.0.4 index value to use for the tables below
 */
#define PMD_SPEAKER_CONFIG_7_0_4 (DLB_PMD_SPEAKER_CONFIG_NONE)

/**
 * @brief table of speaker counts for PMD speaker configs
 */
static size_t SPEAKER_CONFIG_COUNT[] =
{
    2, 3, 6, 8, 10, 12, 16, 2, 2, 11
};

/**
 * @brief table of indices for PMD speaker config channel blocks in
 * a table of size SPEAKER_CONFIG_CHANNEL_TOTAL
 */
static size_t SPEAKER_CONFIG_CHANNEL_BLOCK_INDEX[] =
{
    0, 2, 5, 11, 19, 29, 41, 57, 59, 61
};

/**
 * @brief table of TargetGroup names for PMD speaker configs
 */
static const char *speaker_config_names[] =
{
    "RoomCentric_2.0",
    "RoomCentric_3.0",
    "RoomCentric_5.1",
    "RoomCentric_5.1.2",
    "RoomCentric_5.1.4",
    "RoomCentric_7.1.4",
    "RoomCentric_9.1.6",
    "PortableSpeakers",
    "Headphones",
    "RoomCentric_7.0.4"
};

/**
 * @brief Translation table for PMD to S-ADM speaker config values
 */
static const DLB_ADM_SPEAKER_CONFIG PMD_TO_ADM_SPEAKER_CONFIGS[] =
{
    /* DLB_PMD_SPEAKER_CONFIG_2_0 */        DLB_ADM_SPEAKER_CONFIG_2_0,
    /* DLB_PMD_SPEAKER_CONFIG_3_0 */        DLB_ADM_SPEAKER_CONFIG_3_0,
    /* DLB_PMD_SPEAKER_CONFIG_5_1 */        DLB_ADM_SPEAKER_CONFIG_5_1,
    /* DLB_PMD_SPEAKER_CONFIG_5_1_2 */      DLB_ADM_SPEAKER_CONFIG_5_1_2,
    /* DLB_PMD_SPEAKER_CONFIG_5_1_4 */      DLB_ADM_SPEAKER_CONFIG_5_1_4,
    /* DLB_PMD_SPEAKER_CONFIG_7_1_4 */      DLB_ADM_SPEAKER_CONFIG_7_1_4,
    /* DLB_PMD_SPEAKER_CONFIG_9_1_6 */      DLB_ADM_SPEAKER_CONFIG_9_1_6,
    /* DLB_PMD_SPEAKER_CONFIG_PORTABLE */   DLB_ADM_SPEAKER_CONFIG_PORTABLE,
    /* DLB_PMD_SPEAKER_CONFIG_HEADPHONE */  DLB_ADM_SPEAKER_CONFIG_HEADPHONE,
    /* DLB_PMD_SPEAKER_CONFIG_NONE*/        DLB_ADM_SPEAKER_CONFIG_NONE
};

/**
 * @brief Bed class postfix added to bed name
*/
static const char* PMD_BED_CLASS_POSTFIX[PMD_BED_CLASS_COUNT] = 
{
    "$[ME]",
    "$[CM]",
    "$[BM]"
};

/**
 * @brief Translate PMD bed class to S-ADM CONTENT_KIND
 */
static const DLB_ADM_CONTENT_KIND PMD_TO_BED_CLASS_ADM_CONTENT_KIND[PMD_BED_CLASS_COUNT] = 
{
    /* $[ME] */ DLB_ADM_CONTENT_KIND_NK_UNDEFINED,
    /* $[CM] */ DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN,
    /* $[BM] */ DLB_ADM_CONTENT_KIND_MK_VISUALLY_IMPAIRED
};

/**
 * @brief Initialize the core model generator instance
 */
static
int                         /** @return 0 on success and non-zero otherwise */
pmd_core_model_generator_init
    (pmd_core_model_generator   *g
    ,dlb_adm_core_model         *core_model
    ,const dlb_pmd_model        *pmd_model
    )
{
    dlb_adm_data_source_group source_group;
    dlb_adm_bool hasProfile = DLB_ADM_FALSE;
    int status;

    if ((g == NULL) || (core_model == NULL) || (pmd_model == NULL))
    {
        return DLB_ADM_STATUS_ERROR;
    }
    g->core_model = core_model;
    g->pmd_model = pmd_model;

    /* Clear any old information */
    memset(g->bed_target_groups, 0, sizeof(g->bed_target_groups));
    memset(g->bed_targets, 0, sizeof(g->bed_targets));
    memset(g->sources, 0, sizeof(g->sources));
    g->source_group_id = DLB_ADM_NULL_ENTITY_ID;
    g->current_audio_element_id = DLB_ADM_NULL_ENTITY_ID;

    /* Add the PMD source group */
    memset(&source_group, 0, sizeof(source_group));
    source_group.group_id = PMD_SOURCE_GROUP_ID;
    snprintf(source_group.name, sizeof(source_group.name), "%s", PMD_SOURCE_GROUP_NAME);
    status = dlb_adm_core_model_add_source_group(core_model, &source_group);
    CHECK_STATUS(status);
    g->source_group_id = source_group.id;

    /* Add Common Definition references */
    status = dlb_adm_core_model_has_profile(g->core_model
                                           ,DLB_ADM_PROFILE_SADM_EMISSION_PROFILE
                                           ,&hasProfile);
    CHECK_STATUS(status);

    if (hasProfile)
    {
        /* Mark AudioPackFormat -> TargetGroup */
        g->bed_target_groups[DLB_PMD_SPEAKER_CONFIG_2_0] = 0x801000200000000;   // AP_00010002
        g->bed_target_groups[DLB_PMD_SPEAKER_CONFIG_5_1] = 0x801000300000000;   // AP_00010003
        g->bed_target_groups[DLB_PMD_SPEAKER_CONFIG_5_1_4] = 0x801000500000000; // AP_00010005

        /* Mark AudioChannelFormat -> Target */
        /* 2.0 channels */
        g->bed_targets[0] = 0xA01000100000000;    // AC_00010001
        g->bed_targets[1] = 0xA01000200000000;    // AC_00010002
        /* 5.1 channels */
        g->bed_targets[5] = 0xA01000100000000;    // AC_00010001
        g->bed_targets[6] = 0xA01000200000000;    // AC_00010002
        g->bed_targets[7] = 0xA01000300000000;    // AC_00010003
        g->bed_targets[8] = 0xA01000400000000;    // AC_00010004
        g->bed_targets[9] = 0xA01000500000000;    // AC_00010005
        g->bed_targets[10] = 0xA01000600000000;    // AC_00010006
        /* 5.1.4 channels */
        g->bed_targets[19] = 0xA01000100000000;    // AC_00010001
        g->bed_targets[20] = 0xA01000200000000;    // AC_00010002
        g->bed_targets[21] = 0xA01000300000000;    // AC_00010003
        g->bed_targets[22] = 0xA01000400000000;    // AC_00010004
        g->bed_targets[23] = 0xA01000500000000;    // AC_00010005
        g->bed_targets[24] = 0xA01000600000000;    // AC_00010006
        g->bed_targets[25] = 0xA01000D00000000;    // AC_0001000D
        g->bed_targets[26] = 0xA01000F00000000;    // AC_0001000F
        g->bed_targets[27] = 0xA01001000000000;    // AC_00010010
        g->bed_targets[28] = 0xA01001200000000;    // AC_00010012
    }
    return status;
}

dlb_pmd_success
pmd_core_model_generator_query_memory_size
    (size_t                     *sz
    )
{
    size_t names_sz;
    int status;

    if (sz == NULL)
    {
        return PMD_FAIL;
    }
    status = dlb_adm_core_model_query_names_memory_size(&names_sz, DLB_ADM_DATA_NAME_SZ, MAX_NAME_COUNT);
    CHECK_STATUS_SUCCESS(status);
    *sz = sizeof(pmd_core_model_generator) + names_sz;

    return PMD_SUCCESS;
}

dlb_pmd_success
pmd_core_model_generator_open
    (pmd_core_model_generator  **p_generator
    ,void                       *memory
    )
{
    pmd_core_model_generator *g = (pmd_core_model_generator *)memory;
    size_t names_sz;
    int status;

    if ((p_generator == NULL) || (memory == NULL))
    {
        return PMD_FAIL;
    }
    *p_generator = NULL;

    status = dlb_adm_core_model_query_names_memory_size(&names_sz, DLB_ADM_DATA_NAME_SZ, MAX_NAME_COUNT);
    CHECK_STATUS_SUCCESS(status);

    memset(g, 0, sizeof(*g));
    g->names_memory = ((char *)g) + sizeof(*g);
    status = dlb_adm_core_model_configure_names(&g->names, MAX_NAME_COUNT, g->names_memory, names_sz);
    CHECK_STATUS_SUCCESS(status);

    *p_generator = g;

    return PMD_SUCCESS;
}

/**
 * @brief generate the required form of a presentation id
 *
 * This is derived from the id of the corresponding PMD presentation
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_presentation_id
    (dlb_pmd_presentation_id     pid    /**< [in] presentation identifier */
    ,dlb_adm_entity_id          *id     /**< [out] core model ID to generate */
    )
{
    char id_string[ID_STRING_SIZE];
    int status;

    snprintf(id_string, sizeof(id_string), "APR_%04x", 0x1000 + pid);
    status = dlb_adm_read_entity_id(id, id_string, sizeof(id_string));
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif
    return status;
}

/**
 * @brief generate the required form of a content group id
 *
 * This is derived from the id of the corresponding PMD element
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_content_group_id
    (dlb_pmd_element_id          eid    /**< [in] element identifier */
    ,dlb_adm_entity_id          *id     /**< [out] core model ID to generate */
    )
{
    char id_string[ID_STRING_SIZE];
    int status;

    snprintf(id_string, sizeof(id_string), "ACO_%04x", 0x1000 + eid);
    status = dlb_adm_read_entity_id(id, id_string, sizeof(id_string));
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif
    return status;
}

/**
* @brief generate the required form of a alternative value set id
*
* This is derived from the id of the corresponding PMD element
*/
static
int                         /** @return 0 on success and non-zero otherwise */
generate_alternative_value_set_id
    (dlb_pmd_element_id          peid   /**< [in] parent element identifier */
    ,unsigned int                sn     /**< [in] sequence number */
    ,dlb_adm_entity_id          *id     /**< [out] core model ID to generate */
    )
{
    char id_string[ID_STRING_SIZE];
    int status;

    snprintf(id_string, sizeof(id_string), "AVS_%04x_%04x", 0x1000 + peid, 0x0000 + sn);
    status = dlb_adm_read_entity_id(id, id_string, sizeof(id_string));
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif
    return status;
}

/**
 * @brief generate the required form of an element group or audio element id
 *
 * This is derived from the id of the corresponding PMD element
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_element_id
    (dlb_pmd_element_id          eid    /**< [in] element identifier */
    ,dlb_adm_entity_id          *id     /**< [out] core model ID to generate */
    )
{
    char id_string[ID_STRING_SIZE];
    int status;

    snprintf(id_string, sizeof(id_string), "AO_%04x", 0x1000 + eid);
    status = dlb_adm_read_entity_id(id, id_string, sizeof(id_string));
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif
    return status;
}

/**
 * @brief generate target group id
 *
 * In DLB S-ADM, we use the following naming convention:
 *
 *   AC_yyyyxxxx
 *
 *  where <yyyy> is a 4-digit hex code with following values:
 *     0x0001 - DirectSpeakers (ABD direct)
 *     0x0002 - Matrix         (ABD derived)
 *     0x0003 - Objects        (AOD)
 *
 *  and where <xxxx> depends on <yyyy>
 *
 *  yyyy     xxxx
 *  0x0001   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0002   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0003   0x1001-0x1fff, corresponds to 0x1000 + object id
 *
 * Note that we generate a single target group for each speaker config;
 * i.e., if we have two 5.1.2 beds, then they will both share the same
 * target group (and associated targets).  An object, on the other hand,
 * is uniquely associated with its own target group.
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_target_group_id
    (DLB_ADM_AUDIO_TYPE          type   /**< [in] core model audio type */
    ,dlb_pmd_element_id          eid    /**< [in] PMD element identifier */
    ,dlb_pmd_speaker_config      cfg    /**< [in] PMD speaker config */
    ,dlb_adm_entity_id          *id     /**< [out] core model ID to generate */
    )
{
    unsigned int yyyy = (unsigned int)type;
    unsigned int xxxx = (type < DLB_ADM_AUDIO_TYPE_OBJECTS)
        ? (unsigned int)cfg + 0x1001    /* PMDLIB-180: cfg is 0-based, target group ID must be 1001-based */
        : (unsigned int)eid + 0x1000;   /* element ID starts at 1, so it is OK like this */
    char id_string[ID_STRING_SIZE];
    int status;

    snprintf(id_string, sizeof(id_string), "AP_%04x%04x", yyyy, xxxx);
    status = dlb_adm_read_entity_id(id, id_string, sizeof(id_string));
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif
    return status;
}

/**
 * @brief generate target id
 *
 * In DLB S-ADM, we use the following naming convention:
 *
 *   AC_yyyyxxxx
 *
 *  where <yyyy> is a 4-digit hex code with following values:
 *     0x0001 - DirectSpeakers (ABD direct)
 *     0x0002 - Matrix         (ABD derived)
 *     0x0003 - Objects        AOD
 *
 *  and where <xxxx> depends on <yyyy>
 *
 *  yyyy     xxxx
 *  0x0001   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0002   0x1001-0x1010, corresponds to particular speaker positions
 *  0x0003   0x1001-0x1fff, corresponds to 0x1000 + object id
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_target_id
    (DLB_ADM_AUDIO_TYPE          type       /**< [in] core model audio type */
    ,dlb_pmd_element_id          eid        /**< [in] PMD element identifier (for objects) */
    ,dlb_pmd_speaker             speaker    /**< [in] speaker position (for beds) */
    ,dlb_adm_entity_id          *id         /**< [out] core model ID to generate */
    )
{
    unsigned int yyyy = (unsigned int)type;
    unsigned int xxxx = (type < DLB_ADM_AUDIO_TYPE_OBJECTS)
        ? (unsigned int)speaker + 0x1000
        : (unsigned int)eid + 0x1000;
    char id_string[ID_STRING_SIZE];
    int status;

    snprintf(id_string, sizeof(id_string), "AC_%04x%04x", yyyy, xxxx);
    status = dlb_adm_read_entity_id(id, id_string, sizeof(id_string));
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif
    return status;
}

/**
 * @brief generate the Source object for the given signal, or re-use the one generated earlier
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_source
    (pmd_core_model_generator   *g      /**< [in] PMD -> core model converter */
    ,dlb_pmd_signal              s      /**< [in] PMD signal for which to get a Source ID */
    ,dlb_adm_entity_id          *id     /**< [out] Returned Source ID */
    )
{
    if ((g == NULL) || (id == NULL))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (g->sources[s] == DLB_ADM_NULL_ENTITY_ID)
    {
        dlb_adm_data_source source;
        int status;

        memset(&source, 0, sizeof(source));
        status = dlb_adm_construct_generic_entity_id(&source.id, DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, s);
        source.group_id = PMD_SOURCE_GROUP_ID;
        source.channel = s;
        status = dlb_adm_core_model_add_source(g->core_model, &source);
        CHECK_STATUS(status);
        g->sources[s] = source.id;
    }

    *id = g->sources[s];

    return DLB_ADM_STATUS_OK;
}

/**
 * @brief Translate a PMD "bed class" from name to a core model "content kind" value
 */
static
DLB_ADM_CONTENT_KIND
translate_bed_class_to_content_kind
    (const char * name
    )
{
    DLB_ADM_CONTENT_KIND kind = DLB_ADM_CONTENT_KIND_MK_MIXED;
    char * kind_ptr = NULL;
    unsigned int i;
    for (i = 0; i < PMD_BED_CLASS_COUNT; ++i)
    {
        kind_ptr = strstr (name, PMD_BED_CLASS_POSTFIX[i]);
        if (kind_ptr != NULL)
        {
            strncpy(kind_ptr, "\0", 1);
            kind = PMD_TO_BED_CLASS_ADM_CONTENT_KIND[i];
            break;
        }
    }

    return kind;
}

/**
 * @brief Translate a PMD "object class" value to a core model "content kind" value
 */
static
DLB_ADM_CONTENT_KIND
translate_object_class_to_content_kind
    (dlb_pmd_object_class object_class
    )
{
    DLB_ADM_CONTENT_KIND kind = DLB_ADM_CONTENT_KIND_UNKNOWN;

    switch (object_class)
    {
    case PMD_CLASS_DIALOG:
        kind = DLB_ADM_CONTENT_KIND_DK_DIALOGUE;
        break;
    case PMD_CLASS_VDS:
        kind = DLB_ADM_CONTENT_KIND_DK_DESCRIPTION;
        break;
    case PMD_CLASS_VOICEOVER:
        kind = DLB_ADM_CONTENT_KIND_DK_VOICEOVER;
        break;
    case PMD_CLASS_GENERIC:
        kind = DLB_ADM_CONTENT_KIND_MK_UNDEFINED;
        break;
    case PMD_CLASS_SUBTITLE:
        kind = DLB_ADM_CONTENT_KIND_DK_SUBTITLE;
        break;
    case PMD_CLASS_EMERGENCY_ALERT:
    case PMD_CLASS_EMERGENCY_INFO:
        kind = DLB_ADM_CONTENT_KIND_DK_EMERGENCY;
        break;
    default:
        break;
    }

    return kind;
}

/**
 * @brief Translate a PMD "object class" value to a core model "object class" value
 */
static
DLB_ADM_OBJECT_CLASS
translate_object_class
    (dlb_pmd_object_class object_class
    )
{
    DLB_ADM_OBJECT_CLASS adm_object_class = DLB_ADM_OBJECT_CLASS_GENERIC;

    switch (object_class)
    {
    case PMD_CLASS_DIALOG:
        adm_object_class = DLB_ADM_OBJECT_CLASS_DIALOG;
        break;
    case PMD_CLASS_VDS:
        adm_object_class = DLB_ADM_OBJECT_CLASS_VDS;
        break;
    case PMD_CLASS_VOICEOVER:
        adm_object_class = DLB_ADM_OBJECT_CLASS_VOICEOVER;
        break;
    case PMD_CLASS_SUBTITLE:
        adm_object_class = DLB_ADM_OBJECT_CLASS_SUBTITLE;
        break;
    case PMD_CLASS_EMERGENCY_ALERT:
        adm_object_class = DLB_ADM_OBJECT_CLASS_EMERGENCY_ALERT;
        break;
    case PMD_CLASS_EMERGENCY_INFO:
        adm_object_class = DLB_ADM_OBJECT_CLASS_EMERGENCY_INFO;
        break;
    default:
        break;
    }

    return adm_object_class;
}

/**
 * @brief Is a PMD bed actually 7.0.4?  PMD does not have a speaker config value for 7.0.4, but 7.0.4 is
 * a use case for S-ADM.  In PMD, we represent this config by declaring the bed to be 7.1.4, but giving
 * it no source for the LFE channel.  This function checks for this special case.
 */
static
dlb_pmd_bool
is_bed_7_0_4
    (dlb_pmd_bed *bed
    )
{
    dlb_pmd_bool bed_is_7_0_4 = (bed->num_sources == 11);   /* Preliminary guess */
    dlb_pmd_source *source;
    size_t i;

    if (bed_is_7_0_4)                       /* Now check all the sources */
    {
        dlb_pmd_bool got_L = PMD_FALSE;
        dlb_pmd_bool got_R = PMD_FALSE;
        dlb_pmd_bool got_C = PMD_FALSE;
        dlb_pmd_bool got_Lss = PMD_FALSE;
        dlb_pmd_bool got_Rss = PMD_FALSE;
        dlb_pmd_bool got_Lrs = PMD_FALSE;
        dlb_pmd_bool got_Rrs = PMD_FALSE;
        dlb_pmd_bool got_LTF = PMD_FALSE;
        dlb_pmd_bool got_RTF = PMD_FALSE;
        dlb_pmd_bool got_LTR = PMD_FALSE;
        dlb_pmd_bool got_RTR = PMD_FALSE;

        source = bed->sources;
        for (i = 0; i < bed->num_sources && bed_is_7_0_4; i++, source++)
        {
            switch (source->target)
            {
            case PMD_SPEAKER_L:
                if (got_L)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_L = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_R:
                if (got_R)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_R = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_C:
                if (got_C)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_C = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LS:
                if (got_Lss)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Lss = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RS:
                if (got_Rss)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Rss = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LRS:
                if (got_Lrs)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Lrs = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RRS:
                if (got_Rrs)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_Rrs = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LTF:
                if (got_LTF)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_LTF = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RTF:
                if (got_RTF)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_RTF = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_LTR:
                if (got_LTR)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_LTR = PMD_TRUE;
                }
                break;

            case PMD_SPEAKER_RTR:
                if (got_RTR)
                {
                    bed_is_7_0_4 = PMD_FALSE;
                }
                else
                {
                    got_RTR = PMD_TRUE;
                }
                break;

            default:
                bed_is_7_0_4 = PMD_FALSE;
                break;
            }
        }
    }

    return bed_is_7_0_4;
}

/**
 * @brief Generate the core library objects for a bed channel Target, or re-use the existing
 * ones for that bed channel.
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_bed_target_objects
    (pmd_core_model_generator   *g                  /**< [in] PMD -> core model converter */
    ,dlb_pmd_bed                *bed                /**< [in] PMD bed object */
    ,dlb_pmd_bool                bed_is_7_0_4       /**< [in] Special case for 7.0.4? */
    ,size_t                      channel_index      /**< [in] Index for this channel in the bed */
    ,dlb_adm_entity_id           target_group_id    /**< [in] TargetGroup ID for the bed */
    ,dlb_adm_entity_id          *target_id          /**< [in/out] Target ID for this channel */
    )
{
    dlb_adm_entity_id source_id;
    dlb_adm_data_audio_track audio_track;
    dlb_adm_entity_id track_id;
    int status;

    /* Error checks */
    if (channel_index >= bed->num_sources)
    {
        (void)bed_is_7_0_4; /* TODO: use this here? */
        return DLB_ADM_STATUS_ERROR;
    }
    if (g->current_audio_element_id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    /* Target */
    if (*target_id == DLB_ADM_NULL_ENTITY_ID)
    {
        dlb_adm_data_target target;
        dlb_pmd_speaker spkr = bed->sources[channel_index].target;
        dlb_pmd_bool alt_spkrs = (bed->config > DLB_PMD_SPEAKER_CONFIG_5_1_4);
        const speaker_blkupdt *sb = find_speaker_blkupdt(&spkr, alt_spkrs);
        dlb_adm_bool exists;

        memset(&target, 0, sizeof(target));
        status = generate_target_id(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, bed->id, spkr, &target.id);
        CHECK_STATUS_SUCCESS(status);
        status = dlb_adm_core_model_entity_exists(g->core_model, target.id, &exists);
        CHECK_STATUS_SUCCESS(status);
        if (!exists)
        {
            dlb_adm_data_block_update block_update;

            status = dlb_adm_core_model_clear_names(&g->names);
            CHECK_STATUS_SUCCESS(status);
            status = dlb_adm_core_model_add_name(&g->names, sb->name, "");
            CHECK_STATUS_SUCCESS(status);

            target.audio_type = DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS;
            snprintf(target.speaker_label, sizeof(target.speaker_label), "%s", sb->label);
            status = dlb_adm_core_model_add_target(g->core_model, &target, &g->names);
            CHECK_STATUS_SUCCESS(status);

            memset(&block_update, 0, sizeof(block_update));
            block_update.cartesian = DLB_ADM_TRUE;
            block_update.position[DLB_ADM_COORDINATE_X] = sb->x;
            block_update.position[DLB_ADM_COORDINATE_Y] = sb->y;
            block_update.position[DLB_ADM_COORDINATE_Z] = sb->z;
            block_update.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
            block_update.gain.gain_value = 0.0;
            status = dlb_adm_core_model_add_block_update(g->core_model, target.id, &block_update);
            CHECK_STATUS_SUCCESS(status);
        }
        *target_id = target.id;
    }

    /* AudioTrack */
    memset(&audio_track, 0, sizeof(audio_track));
    /* use auto-generated ID */
    status = dlb_adm_core_model_add_audio_track(g->core_model, &audio_track);
    CHECK_STATUS_SUCCESS(status);
    track_id = audio_track.id;

    /* Generate source and add source relation */
    status = generate_source(g, bed->sources[channel_index].source, &source_id);
    CHECK_STATUS(status);
    status = dlb_adm_core_model_add_source_relation(
        g->core_model, g->source_group_id, source_id, track_id);
    CHECK_STATUS(status);

    /* Add the element relation */
    status = dlb_adm_core_model_add_element_relation(
        g->core_model, g->current_audio_element_id, target_group_id, *target_id, track_id);
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif

    return status;
}

/**
 * @brief Generate the core library objects for a bed TargetGroup, or re-use the existing ones for
 * the bed's speaker configuration.
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_bed_target_group_objects
    (pmd_core_model_generator   *g                  /**< [in] PMD -> core model converter */
    ,dlb_pmd_bed                *bed                /**< [in] PMD bed object */
    ,dlb_pmd_bool                bed_is_7_0_4       /**< [in] Special case for 7.0.4? */
    )
{
    dlb_pmd_speaker_config cfg = (bed_is_7_0_4 ? PMD_SPEAKER_CONFIG_7_0_4 : bed->config);
    size_t channel_count = SPEAKER_CONFIG_COUNT[cfg];
    size_t block_index = SPEAKER_CONFIG_CHANNEL_BLOCK_INDEX[cfg];
    dlb_adm_entity_id *targets = &g->bed_targets[block_index];
    dlb_adm_entity_id target_group_id;
    int status;
    size_t i;

    /* Add the target group (or re-use the one generated earlier) */
    if (g->bed_target_groups[cfg] == DLB_ADM_NULL_ENTITY_ID)
    {
        dlb_adm_data_target_group target_group;

        status = dlb_adm_core_model_clear_names(&g->names);
        CHECK_STATUS_SUCCESS(status);
        status = dlb_adm_core_model_add_name(&g->names, speaker_config_names[cfg], "");
        CHECK_STATUS_SUCCESS(status);

        memset(&target_group, 0, sizeof(target_group));
        target_group.speaker_config = PMD_TO_ADM_SPEAKER_CONFIGS[cfg];
        status = generate_target_group_id(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS, bed->id, cfg, &target_group.id);
        CHECK_STATUS(status);
        status = dlb_adm_core_model_add_target_group(g->core_model, &target_group, &g->names);
        CHECK_STATUS(status);
        g->bed_target_groups[cfg] = target_group.id;
    }
    target_group_id = g->bed_target_groups[cfg];

    /* Add the target objects, per channel */
    for (i = 0; i < channel_count; i++) /* TODO: is there any special handling for 7.0.4? */
    {
        status = generate_bed_target_objects(g, bed, bed_is_7_0_4, i, target_group_id, &targets[i]);
        CHECK_STATUS(status);
    }

    return DLB_ADM_STATUS_OK;
}

/**
 * @brief generate core model objects for a PMD bed element
 */
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
generate_bed_objects
    (pmd_core_model_generator   *g          /**< [in] PMD -> core model converter */
    ,dlb_pmd_bed                *bed        /**< [in] PMD bed object */
    )
{
    dlb_pmd_bool bed_is_7_0_4 = PMD_FALSE;
    int status;
    size_t i;

    /* Error checks */
    if (bed->num_sources != (uint8_t)SPEAKER_CONFIG_COUNT[bed->config])
    {
        bed_is_7_0_4 = is_bed_7_0_4(bed);      /* Special case for 7.0.4 */

        if (!bed_is_7_0_4)
        {
            error(g->pmd_model, "ADM beds must have exactly one source per target speaker\n");
            return FAILURE;
        }
    }
    if (bed->bed_type != PMD_BED_ORIGINAL)
    {
        error(g->pmd_model, "ADM beds cannot be derived\n");
        return FAILURE;
    }
    if (bed->config > DLB_PMD_SPEAKER_CONFIG_9_1_6)
    {
        error(g->pmd_model, "ADM beds cannot have portable or headphone config\n");
        return FAILURE;
    }

    for (i = bed->num_sources - 1; i > 0; i--)
    {
        if (bed->sources[i].gain != bed->sources[0].gain)
        {
            error(g->pmd_model, "sources of ADM beds must all have the same gain\n");
            return FAILURE;
        }
    }

    /* Add the TargetGroup objects */
    status = generate_bed_target_group_objects(g, bed, bed_is_7_0_4);
    CHECK_STATUS_SUCCESS(status);

    return PMD_SUCCESS;
}

/**
 * @brief generate the core model objects for a PMD object element
 */
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
generate_nonbed_objects
    (pmd_core_model_generator   *g          /**< [in] PMD -> core model converter */
    ,dlb_pmd_object             *obj        /**< [in] PMD non-bed object */
    )
{
    dlb_adm_data_audio_track audio_track;
    dlb_adm_data_target_group target_group;
    dlb_adm_data_target target;
    dlb_adm_entity_id source_id;
    dlb_adm_data_block_update block_update;
    int status;

    /* Error checks */
    if (g->current_audio_element_id == DLB_ADM_NULL_ENTITY_ID)
    {
        return FAILURE;
    }
    if (obj->dynamic_updates)   /* TODO: allow dynamic updates! */
    {
        error(g->pmd_model, "S-ADM generator failure: object %u has dynamic updates, "
              "which are not supported\n", obj->id);
        return FAILURE;
    }
    if (obj->size != 0.0f)
    {
        error(g->pmd_model, "S-ADM generator failure: object %u has non-point size (%.*f), "
              "which is not supported\n", obj->id, g->pmd_model->coordinate_print_precision, obj->size);
        return FAILURE;
    }
    if (obj->size_3d != PMD_FALSE)
    {
        error(g->pmd_model, "S-ADM generator failure: object %u specifies 3D objects "
              "which are not supported\n", obj->id);
        return FAILURE;
    }
    if (obj->diverge)
    {
        error(g->pmd_model, "S-ADM generator failure: object %u specifies divergence "
              "which is not supported\n", obj->id);
        return FAILURE;
    }

    /* Set up names */
    status = dlb_adm_core_model_clear_names(&g->names);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_add_name(&g->names, obj->name, "");
    CHECK_STATUS_SUCCESS(status);

    /* Add the AudioTrack */
    memset(&audio_track, 0, sizeof(audio_track));
    /* use auto-generated ID */
    status = dlb_adm_core_model_add_audio_track(g->core_model, &audio_track);
    CHECK_STATUS_SUCCESS(status);

    /* Add the TargetGroup */
    memset(&target_group, 0, sizeof(target_group));
    target_group.audio_type = DLB_ADM_AUDIO_TYPE_OBJECTS;
    status = generate_target_group_id(
        DLB_ADM_AUDIO_TYPE_OBJECTS, obj->id, DLB_PMD_SPEAKER_CONFIG_NONE, &target_group.id);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_add_target_group(g->core_model, &target_group, &g->names);
    CHECK_STATUS_SUCCESS(status);

    /* Add the Target */
    memset(&target, 0, sizeof(target));
    target.audio_type = DLB_ADM_AUDIO_TYPE_OBJECTS;
    status = generate_target_id(target.audio_type, obj->id, PMD_SPEAKER_NULL, &target.id);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_add_target(g->core_model, &target, &g->names);
    CHECK_STATUS_SUCCESS(status);

    /* Generate source and add source relation */
    status = generate_source(g, obj->source, &source_id);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_add_source_relation(
        g->core_model, g->source_group_id, source_id, audio_track.id);
    CHECK_STATUS_SUCCESS(status);

    /* Add the BlockUpdate */
    memset(&block_update, 0, sizeof(block_update));
    block_update.cartesian = DLB_ADM_TRUE;
    block_update.position[DLB_ADM_COORDINATE_X] = obj->x;
    block_update.position[DLB_ADM_COORDINATE_Y] = obj->y;
    block_update.position[DLB_ADM_COORDINATE_Z] = obj->z;
    block_update.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    block_update.gain.gain_value = 0.0;
    block_update.has_time = PMD_TRUE;
    /* TODO: Add proper start and duration */
    block_update.duration.fraction_numerator = 1920;
    block_update.duration.fraction_denominator = 48000;
    status = dlb_adm_core_model_add_block_update(g->core_model, target.id, &block_update);
    CHECK_STATUS_SUCCESS(status);

    /* Add the element relation */
    status = dlb_adm_core_model_add_element_relation(
        g->core_model, g->current_audio_element_id, target_group.id, target.id, audio_track.id);
    CHECK_STATUS_SUCCESS(status);

    return PMD_SUCCESS;
}

static
void
generate_content_label
    (dlb_pmd_element_id          element_id /**< [in] PMD element ID for which to generate core model objects */
    ,unsigned int                is_bed     /**< [in] Indicate if element is bed */
    ,DLB_ADM_CONTENT_KIND        content_kind
    ,dlb_pmd_pres_language_tag*  language_tag
    ,unsigned int                channel_number
    ,char*                       content_label
    )
{
    if (is_bed)
    {
        strcpy(content_label, "Bed ");
        switch (channel_number)
        {
            case 2: strcat(content_label, "2.0"); break;
            case 6: strcat(content_label, "5.1"); break;
            case 10: strcat(content_label, "5.1.4"); break;
            case 12: strcat(content_label, "7.1.4"); break;
            case 16: strcat(content_label, "9.1.6"); break;
            default: break;
        }
    }
    else
    {
        switch (content_kind)
        {
            case DLB_ADM_CONTENT_KIND_DK_UNDEFINED:
            case DLB_ADM_CONTENT_KIND_DK_DIALOGUE:
                strcpy(content_label, "Dialogue "); break;
            case DLB_ADM_CONTENT_KIND_DK_VOICEOVER:
                strcpy(content_label, "Voiceover "); break;
            case DLB_ADM_CONTENT_KIND_DK_SUBTITLE:
                strcpy(content_label, "Spoken Subtitles "); break;
            case DLB_ADM_CONTENT_KIND_DK_DESCRIPTION:
                strcpy(content_label, "Audio Description "); break;
            case DLB_ADM_CONTENT_KIND_DK_COMMENTARY:
                strcpy(content_label, "Commentary "); break;
            case DLB_ADM_CONTENT_KIND_DK_EMERGENCY:
                strcpy(content_label, "Emergency "); break;
            default:
                strcpy(content_label, "Unknown "); break;
        }
        strcat(content_label, language_tag->language);
    }
}

/**
 * @brief get content language as language of presentation where this content is used
 * If content isn't referenced by any presentation, language tag is set to "und"
 * 
 * NOTE: For consistency with ADM last presentation with given content is used
 */
static
void
get_content_language
    (pmd_core_model_generator   *g
    ,dlb_pmd_presentation_id     pres_id
    ,dlb_pmd_pres_language_tag  *content_language
    )
{
    dlb_pmd_element_id elements[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_presentation pres;
    if (pres_id == 0)
    {
        strcpy(content_language->language, "und");
        return;
    }

    dlb_pmd_presentation_lookup(g->pmd_model, pres_id, &pres, DLB_PMD_MAX_AUDIO_ELEMENTS, elements);
    strcpy(content_language->language, pres.audio_language);
}

/**
 * @brief generate core model objects for a PMD element
 */
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
generate_element_objects
    (pmd_core_model_generator   *g          /**< [in] PMD -> core model converter */
    ,dlb_pmd_element_id          element_id /**< [in] PMD element ID for which to generate core model objects */
    ,dlb_pmd_presentation_id     pres_id    /**< [in] PMD presentation ID which contain current element */
    )
{
    dlb_adm_data_content_group content_group;
    dlb_adm_data_audio_element audio_element;
    dlb_pmd_source sources[MAX_BED_CHANNEL_COUNT];
    dlb_pmd_bool is_bed;
    dlb_pmd_bool exists;
    dlb_pmd_object object;
    dlb_pmd_bed bed;
    dlb_pmd_success success;
    int status;
    unsigned int i = 0;
    char content_label[DLB_PMD_TITLE_SIZE];
    dlb_pmd_pres_language_tag content_language;
    unsigned int number_of_channels = 0;

    memset(&content_group, 0, sizeof(content_group));
    memset(&audio_element, 0, sizeof(audio_element));

    status = generate_content_group_id(element_id, &content_group.id);
    CHECK_STATUS_SUCCESS(status);
    status = generate_element_id(element_id, &audio_element.id);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_entity_exists(g->core_model, audio_element.id, &exists);
    CHECK_STATUS_SUCCESS(status);
    if (!exists)
    {
        audio_element.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
        if (!dlb_pmd_bed_lookup(g->pmd_model, element_id, &bed, MAX_BED_CHANNEL_COUNT, sources))
        {

            content_group.content_kind = translate_bed_class_to_content_kind(bed.name);
            audio_element.gain.gain_value = bed.sources[0].gain;    /* we'll check other gains for equality later */
            audio_element.object_class = DLB_ADM_OBJECT_CLASS_NONE;
            is_bed = PMD_TRUE;
            number_of_channels = bed.num_sources;
        }
        else if (!dlb_pmd_object_lookup(g->pmd_model, element_id, &object))
        {
            content_group.content_kind = translate_object_class_to_content_kind(object.object_class);
            audio_element.gain.gain_value = object.source_gain;
            audio_element.object_class = translate_object_class(object.object_class);
            is_bed = PMD_FALSE;
        }
        else
        {
            return FAILURE;
        }

        status = dlb_adm_core_model_clear_names(&g->names);
        CHECK_STATUS_SUCCESS(status);
        status = dlb_adm_core_model_add_name(&g->names, (is_bed ? bed.name : object.name), "");
        CHECK_STATUS_SUCCESS(status);

        status = dlb_adm_core_model_add_audio_element(g->core_model, &audio_element, &g->names);
        CHECK_STATUS_SUCCESS(status);

        get_content_language(g, pres_id, &content_language);
        memset(content_label, 0, DLB_PMD_TITLE_SIZE);
        generate_content_label(element_id, is_bed, content_group.content_kind, &content_language, number_of_channels, content_label);
        for (i = 0; i < g->num_lang_tag; i++)
        {
            status = dlb_adm_core_model_add_label(&g->names, content_label, g->lang_tags[i].language);
            CHECK_STATUS_SUCCESS(status);
        }

        status = dlb_adm_core_model_add_content_group(g->core_model, &content_group, &g->names);
        CHECK_STATUS_SUCCESS(status);

        g->current_audio_element_id = audio_element.id;
        if (is_bed)
        {
            success = generate_bed_objects(g, &bed);
        }
        else
        {
            success = generate_nonbed_objects(g, &object);
        }
        g->current_audio_element_id = DLB_ADM_NULL_ENTITY_ID;
        CHECK_SUCCESS(success);
    }

    return PMD_SUCCESS;
}

/**
* @brief generate core model alternative value sets for a PMD element
*/
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
generate_alternative_value_set
    (pmd_core_model_generator   *g                 /**< [in] PMD -> core model converter */
    ,dlb_pmd_element_id          element_id        /**< [in] PMD element ID for which to generate alternative value set */
    ,dlb_pmd_element_id          parent_element_id /**< [in] parent PMD element ID for alternative value set */
    )
{
    dlb_adm_data_audio_element audio_element;
    dlb_adm_data_alt_value_set alt_set;
    dlb_pmd_source sources[MAX_BED_CHANNEL_COUNT];
    dlb_pmd_bool exists;
    dlb_pmd_object object;
    dlb_pmd_bed bed;
    int status;

    memset(&audio_element, 0, sizeof(audio_element));
    memset(&alt_set, 0, sizeof(alt_set));

    status = generate_element_id(parent_element_id, &audio_element.id);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_entity_exists(g->core_model, audio_element.id, &exists);
    CHECK_STATUS_SUCCESS(status);
    if (!exists)
    {
        return FAILURE;
    }

    alt_set.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    if (!dlb_pmd_bed_lookup(g->pmd_model, element_id, &bed, MAX_BED_CHANNEL_COUNT, sources))
    {
        alt_set.has_gain = DLB_ADM_TRUE;
        alt_set.gain.gain_value = bed.sources[0].gain;    /* we'll check other gains for equality later */
    }
    else if (!dlb_pmd_object_lookup(g->pmd_model, element_id, &object))
    {
        alt_set.has_gain = DLB_ADM_TRUE;
        alt_set.gain.gain_value = object.source_gain;
        alt_set.has_position_offset = DLB_ADM_TRUE;
        alt_set.cartesian = DLB_ADM_TRUE;
        alt_set.position[DLB_ADM_COORDINATE_X] = object.x;
        alt_set.position[DLB_ADM_COORDINATE_Y] = object.y;
        alt_set.position[DLB_ADM_COORDINATE_Z] = object.z;
    }

    status = dlb_adm_core_model_clear_names(&g->names);
    CHECK_STATUS_SUCCESS(status);
    status = dlb_adm_core_model_add_alt_value_set(g->core_model, audio_element.id, &alt_set, &g->names);
    CHECK_STATUS_SUCCESS(status);

    return PMD_SUCCESS;
}

/* @brief Compare if elements have same source channels(channel index and target speaker config),
   gains could be different 
*/
static
dlb_pmd_bool
has_same_channels
    (unsigned int    num_left_sources
    ,dlb_pmd_source *left
    ,unsigned int    num_right_sources
    ,dlb_pmd_source *right
    )
{
    unsigned int i;

    if (num_left_sources != num_right_sources)
    {
        return PMD_FALSE;
    }

    for (i = 0; i < num_left_sources; ++i)
    {
        if (left[i].source != right[i].source)
        {
            return PMD_FALSE;
        }

        if (left[i].target != right[i].target)
        {
            return PMD_FALSE;
        }
    }

    return PMD_TRUE;
}

/**
* @brief check if exist bed with same sources
*
* unique_id - audio element id which is added to CoreModel. To this audio element will be added checked element data as alternative value set
*/
static
dlb_pmd_bool
check_if_bed_has_same_source
    (const dlb_pmd_model *model                  /**<[in] PMD model */
    ,dlb_pmd_element_id  *unique_elements        /**<[in] array with bed ids which will be added as AudioObjects */
    ,unsigned int         unum                   /**<[in] number of elements in array */
    ,dlb_pmd_element_id  *unique_id              /**<[out] parent audio element id */
    ,unsigned int         num_sources            /**<[in] number of sources of checked audio element */
    ,dlb_pmd_source      *sources                /**<[in] sources of checked audio element */
    )
{
    unsigned int uid = 0;

    for (uid = 0; uid < unum; uid++)
    {
        dlb_pmd_source usources[MAX_BED_CHANNEL_COUNT];
        dlb_pmd_bed ubed;

        memset(usources, 0, sizeof(usources));
        if (dlb_pmd_bed_lookup(model, unique_elements[uid], &ubed, MAX_BED_CHANNEL_COUNT, usources) == PMD_SUCCESS)
        {

            if (has_same_channels(num_sources, sources, ubed.num_sources, usources))
            {
                *unique_id = unique_elements[uid];
                return PMD_TRUE;
            }
        }
    }

    return PMD_FALSE;
}

/**
* @brief check if exist object with same sources
*
* unique_id - audio element id which is added to CoreModel. To this audio element will be added checked element data as alternative value set
*/
static
dlb_pmd_bool
check_if_object_has_same_source
    (const dlb_pmd_model *model                  /**<[in] PMD model */
    ,dlb_pmd_element_id  *unique_elements        /**<[in] array with object ids which will be added as AudioObjects */
    ,unsigned int         unum                   /**<[in] number of elements in array */
    ,dlb_pmd_element_id  *unique_id              /**<[out] parent audio element id */
    ,dlb_pmd_signal       source                 /**<[in] sources of checked audio element */
    )
{
    unsigned int uid = 0;

    for (uid = 0; uid < unum; uid++)
    {
        dlb_pmd_object uobject;

        if (dlb_pmd_object_lookup(model, unique_elements[uid], &uobject) == PMD_SUCCESS)
        {
            if (uobject.source == source)
            {
                *unique_id = unique_elements[uid];
                return PMD_TRUE;
            }
        }
    }

    return PMD_FALSE;
}

static
void
populate_lang_tag
    (pmd_core_model_generator *g
    ,dlb_pmd_presentation             pmd_pres
    ,dlb_pmd_presentation_id   *pres_id 
    )
{
    unsigned int i;

    g->num_lang_tag = pmd_pres.num_names;
    for (i = 0; i < pmd_pres.num_names; ++i)
    {
        strncpy(g->lang_tags[i].language, pmd_pres.names[i].language, PMD_LANG_SIZE);
        g->lang_tags[i].language[PMD_LANG_SIZE - 1] = '\0';
    }  
}

/**
 * @brief Populates lang_tags structure in pmd_core_model_generator with language 
 * tags used in presentations labels
 */
static
dlb_pmd_success
dlb_pmd_initialize_language_tags
    (pmd_core_model_generator *g
    ,dlb_pmd_element_id        id
    ,dlb_pmd_presentation_id   *pres_id
    ,unsigned int               num_sources
    ,dlb_pmd_source            *sources
    )
{
    dlb_pmd_presentation_iterator    pi;
    dlb_pmd_presentation             pmd_pres;
    dlb_pmd_element_id               elements[DLB_PMD_MAX_AUDIO_ELEMENTS];
    unsigned int i, el;
    int status;

    g->num_lang_tag = 0;
    *pres_id = 0;

    /* Get info from first presentation, if element is not present in any presentation*/
    status = dlb_pmd_presentation_lookup(g->pmd_model, 1, &pmd_pres, DLB_PMD_MAX_AUDIO_ELEMENTS, elements);
    if (status != PMD_SUCCESS)
    {
        return PMD_SUCCESS;
    }
    populate_lang_tag(g, pmd_pres, pres_id);

    if (dlb_pmd_presentation_iterator_init(&pi, g->pmd_model))
    {
        return FAILURE;
    }
    
    /* for consistency with ADM the last presentation with current audio element is used */
    while (!dlb_pmd_presentation_iterator_next(&pi, &pmd_pres, DLB_PMD_MAX_AUDIO_ELEMENTS, elements))
    {
        for (el = 0; el < pmd_pres.num_elements; ++el)
        {
            dlb_pmd_bed bed;
            dlb_pmd_source element_sources[MAX_BED_CHANNEL_COUNT];

            /* element is referenced by presentation */
            if (elements[el] == id)
            {
                populate_lang_tag(g, pmd_pres, pres_id);
                *pres_id = pmd_pres.id;
                break;
            }

            if (num_sources > 1)
            {
                /* check if model has alternative value bed which is referenced by presentation */
                if (dlb_pmd_bed_lookup(g->pmd_model, elements[el], &bed, MAX_BED_CHANNEL_COUNT, element_sources) == PMD_SUCCESS)
                {
                    if (has_same_channels(num_sources, sources, bed.num_sources, bed.sources))
                    {
                        populate_lang_tag(g, pmd_pres, pres_id);
                        *pres_id = pmd_pres.id;
                        break;
                    }
                }
            }
            else
            {
                dlb_pmd_object object;
                /* check if model has alternative value object which is referenced by presentation */
                if (dlb_pmd_object_lookup(g->pmd_model, elements[el], &object) == PMD_SUCCESS)
                {
                    if (object.source == sources[0].source)
                    {
                        populate_lang_tag(g, pmd_pres, pres_id);
                        *pres_id = pmd_pres.id;
                        break;
                    }
                }
            }
        }
    }

    return PMD_SUCCESS;
}


/**
 * @brief generate a core model element for each PMD element
 */
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
generate_elements
    (pmd_core_model_generator *g    /**< [in] PMD -> core model converter */
    )
{
    dlb_pmd_bed_iterator     bi;
    dlb_pmd_element_id       unique_elements[DLB_PMD_MAX_AUDIO_ELEMENTS];
    dlb_pmd_bed              bed;
    dlb_pmd_object_iterator  oi;
    dlb_pmd_object           object;
    dlb_pmd_source           sources[MAX_BED_CHANNEL_COUNT];
    dlb_pmd_presentation_id  pres_id = 0;
    unsigned int             unum = 0;
    unsigned int             i;
    unsigned int             channel_check[DLB_PMD_MAX_SIGNALS];
    int                      status;


    memset(channel_check, 0, sizeof(channel_check));
    if (dlb_pmd_bed_iterator_init(&bi, g->pmd_model))
    {
        return FAILURE;
    }

    memset(sources, 0, sizeof(sources));
    memset(unique_elements, 0, sizeof(unique_elements));

    while (!dlb_pmd_bed_iterator_next(&bi, &bed, MAX_BED_CHANNEL_COUNT, sources))
    {
        status = dlb_pmd_initialize_language_tags(g, bed.id, &pres_id, bed.num_sources, bed.sources);
        CHECK_STATUS_SUCCESS(status);

        dlb_pmd_element_id main_id;
        if (check_if_bed_has_same_source(g->pmd_model, unique_elements, unum, &main_id, bed.num_sources, sources))
        {
            generate_alternative_value_set(g, bed.id, main_id);
        }
        else
        {
            if (generate_element_objects(g, bed.id, pres_id))
            {
                return FAILURE;
            }
            unique_elements[unum] = bed.id;
            unum++;

            for (i = 0; i < bed.num_sources; ++i)
            {
                if (bed.sources[i].source >= DLB_PMD_MAX_SIGNALS)
                {
                    return FAILURE;
                }

                ++channel_check[bed.sources[i].source - 1];
            }
        }
    }

    if (dlb_pmd_object_iterator_init(&oi, g->pmd_model))
    {
        return FAILURE;
    }

    while (!dlb_pmd_object_iterator_next(&oi, &object))
    {
        dlb_pmd_source sources[1];
        sources[0].source = object.source_gain;
        status = dlb_pmd_initialize_language_tags(g, object.id, &pres_id, 1, sources);
        CHECK_STATUS_SUCCESS(status);
        
        dlb_pmd_element_id main_id;
        if (check_if_object_has_same_source(g->pmd_model, unique_elements, unum, &main_id, object.source))
        {
            generate_alternative_value_set(g, object.id, main_id);
        }
        else
        {
            if (generate_element_objects(g, object.id, pres_id))
            {
                return FAILURE;
            }
            unique_elements[unum] = object.id;
            unum++;

            if (object.source >= DLB_PMD_MAX_SIGNALS)
            {
                return FAILURE;
            }

            ++channel_check[object.source - 1];
        }
    }

    for (i = 0; i < DLB_PMD_MAX_SIGNALS; ++i)
    {
        if (channel_check[i] > 1)
        {
            return FAILURE;
        }
    }

    return PMD_SUCCESS;
}

/**
 * @brief check whether the presentation is able to be converted to a S-ADM programme
 *
 * In Dolby's profile of serial ADM, programme speaker configs are
 * determined by the multi-channel pack formats.  This means that the
 * presentation's speaker config *must* match the speaker config of
 * each bed.
 */
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
check_presentation_config
    (pmd_core_model_generator   *g      /**< [in] PMD -> core model converter */
    ,dlb_pmd_presentation       *pres   /**< [in] PMD presentation to check */
    )
{
    dlb_pmd_source sources[MAX_BED_CHANNEL_COUNT];
    dlb_pmd_bed bed;
    unsigned int i;

    for (i = 0; i != pres->num_elements; i++)
    {
        if (!dlb_pmd_bed_lookup(g->pmd_model, pres->elements[i], &bed, MAX_BED_CHANNEL_COUNT, sources))
        {
            if (pres->config != bed.config)
            {
                error(g->pmd_model, "S-ADM generation failure: presentation %u has config %u, "
                      "but its bed (id %u) has a different config %u: not supported\n",
                      pres->id, pres->config, bed.id, bed.config);
                return FAILURE;
            }
        }
    }

    return PMD_SUCCESS;
}

/**
* @brief add presentation relation for alternative value set
*
*/
static
int
add_avs_presentation_relation
    (pmd_core_model_generator   *g                     /**< [in] PMD -> core model converter */
    ,dlb_adm_entity_id           presentation_id       /**< [in] presentation id */
    ,dlb_pmd_element_id          parent_element_id     /**< [in] parent audio element id */
    ,unsigned int                sequence_num          /**< [in] sequence number of current audio element */
    )
{
    dlb_adm_entity_id content_group_id;
    dlb_adm_entity_id avs_id;
    dlb_adm_entity_id audio_element_id;
    int status;

    status = generate_content_group_id(parent_element_id, &content_group_id);
    CHECK_STATUS(status);
    status = generate_alternative_value_set_id(parent_element_id, sequence_num, &avs_id);
    CHECK_STATUS(status);
    status = generate_element_id(parent_element_id, &audio_element_id);
    CHECK_STATUS(status);

    status = dlb_adm_core_model_add_presentation_relation(
        g->core_model, presentation_id, content_group_id, DLB_ADM_NULL_ENTITY_ID, audio_element_id, avs_id, DLB_ADM_NULL_ENTITY_ID);
    CHECK_STATUS(status);

    return status;
}

static
int
add_presentation_relation
    (pmd_core_model_generator   *g
    ,dlb_adm_entity_id           presentation_id
    ,dlb_pmd_element_id          element_id
    )
{
    dlb_adm_entity_id content_group_id;
    dlb_adm_entity_id audio_element_id;
    int status;

    status = generate_content_group_id(element_id, &content_group_id);
    CHECK_STATUS(status);
    status = generate_element_id(element_id, &audio_element_id);
    CHECK_STATUS(status);

    status = dlb_adm_core_model_add_presentation_relation(
        g->core_model, presentation_id, content_group_id, DLB_ADM_NULL_ENTITY_ID, audio_element_id, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    CHECK_STATUS(status);

    return status;
}

/**
* @brief check if audio element has same sources as another audio element in pmd model
*
* parent_element_id - element which will be added to Core model as AudioObject
* sequence_number - the number of element with same sources in model (it's needed to create alternative value set id)
*/
static
dlb_pmd_bool             /** @return PMD_TRUE(=1) if avs should be added and PMD_FALSE(=0) otherwise */
check_if_avs_should_be_added
    (const dlb_pmd_model *model                  /**< [in] PMD model */
    ,dlb_pmd_element_id   element_id             /**< [in] audio element */
    ,dlb_pmd_element_id  *parent_element_id      /**< [out] parent audio element */
    ,unsigned int        *sequence_number        /**< [out] audio element serial number*/
    )
{
    dlb_pmd_bed_iterator             bi;
    dlb_pmd_bed                      bed, current_bed;
    dlb_pmd_object_iterator          oi;
    dlb_pmd_object                   object, current_object;
    dlb_pmd_source                   sources[MAX_BED_CHANNEL_COUNT];
    dlb_pmd_source                   current_bed_sources[MAX_BED_CHANNEL_COUNT];
    unsigned int                     index = 0;

    memset(current_bed_sources, 0, sizeof(current_bed_sources));
    if (dlb_pmd_bed_lookup(model, element_id, &current_bed, MAX_BED_CHANNEL_COUNT, current_bed_sources) == PMD_SUCCESS)
    {

        if (dlb_pmd_bed_iterator_init(&bi, model))
        {
            return FAILURE;
        }

        memset(sources, 0, sizeof(sources));
        while (!dlb_pmd_bed_iterator_next(&bi, &bed, MAX_BED_CHANNEL_COUNT, sources))
        {
            if (bed.id == element_id)
            {
                *sequence_number = index;
                return (index == 0) ? PMD_FALSE : PMD_TRUE;
            }
 
            if (has_same_channels(bed.num_sources, sources, current_bed.num_sources, current_bed_sources))
            {
                if (index == 0)
                {
                    *parent_element_id = bed.id;
                }
                index++;
            }
        }
    }
    else if(dlb_pmd_object_lookup(model, element_id, &current_object) == PMD_SUCCESS)
    {
        if (dlb_pmd_object_iterator_init(&oi, model))
        {
            return FAILURE;
        }

        while (!dlb_pmd_object_iterator_next(&oi, &object))
        {
            if (object.id == element_id)
            {
                *sequence_number = index;
                return (index == 0) ? PMD_FALSE : PMD_TRUE;
            }

            if (object.source == current_object.source)
            {
                if (index == 0)
                {
                    *parent_element_id = object.id;
                }
                index++;
            }
        }
    }

    return PMD_FALSE;
}

/**
 * @brief generate a core model presentation for each PMD presentation
 */
static
dlb_pmd_success             /** @return PMD_SUCCESS(=0) on success and PMD_FAIL(=1) otherwise */
generate_presentations
    (pmd_core_model_generator *g    /**< [in] PMD -> core model converter */
    )
{
    dlb_pmd_presentation_iterator    pi;
    dlb_pmd_presentation             pmd_pres;
    dlb_pmd_element_id               elements[DLB_PMD_MAX_AUDIO_ELEMENTS];
    uint8_t                          element_set[(DLB_PMD_MAX_AUDIO_ELEMENTS + (BYTE_BITS - 1)) / BYTE_BITS];
    dlb_pmd_bed_iterator             bi;
    dlb_pmd_bed                      bed;
    dlb_pmd_object_iterator          oi;
    dlb_pmd_object                   object;
    dlb_pmd_source                   sources[MAX_BED_CHANNEL_COUNT];
    unsigned int i;
    int status;

    if (dlb_pmd_presentation_iterator_init(&pi, g->pmd_model))
    {
        return FAILURE;
    }

    memset(element_set, 0, sizeof(element_set));
    while (!dlb_pmd_presentation_iterator_next(&pi, &pmd_pres, DLB_PMD_MAX_AUDIO_ELEMENTS, elements))
    {
        dlb_adm_data_presentation adm_pres;

        if (check_presentation_config(g, &pmd_pres))
        {
            return FAILURE;
        }

        memset(&adm_pres, 0, sizeof(adm_pres));
        status = generate_presentation_id(pmd_pres.id, &adm_pres.id);
        CHECK_STATUS_SUCCESS(status);
        if (pmd_pres.num_names > 0)
        {
            status = dlb_adm_core_model_clear_names(&g->names);
            CHECK_STATUS_SUCCESS(status);
            status = dlb_adm_core_model_add_name(&g->names, pmd_pres.names[0].text, pmd_pres.audio_language);
            CHECK_STATUS_SUCCESS(status);
            for (i = 0; i < pmd_pres.num_names; i++)
            {
                status = dlb_adm_core_model_add_label(&g->names, pmd_pres.names[i].text, pmd_pres.names[i].language);
                CHECK_STATUS_SUCCESS(status);
            }
        }
        else
        {
            char pres_name[DLB_PMD_NAME_ARRAY_SIZE];

            snprintf(pres_name, sizeof(pres_name), "Pres-%u", (uint32_t)pmd_pres.id);
            status = dlb_adm_core_model_add_name(&g->names, pres_name, pmd_pres.audio_language);
            CHECK_STATUS_SUCCESS(status);
        }

        status = dlb_adm_core_model_add_presentation(g->core_model, &adm_pres, &g->names);
        CHECK_STATUS_SUCCESS(status);
        for (i = 0; i != pmd_pres.num_elements; i++)
        {
            unsigned int sequence_number = 0;
            dlb_pmd_element_id parent_element_id;
            dlb_pmd_element_id element_id = pmd_pres.elements[i];

            /* Keep track of all the elements contained in presentations */
            set_element_bit(element_set, element_id);

            if (check_if_avs_should_be_added(g->pmd_model, element_id, &parent_element_id, &sequence_number))
            {
                status = add_avs_presentation_relation(g, adm_pres.id, parent_element_id, sequence_number);
                CHECK_STATUS_SUCCESS(status);
            }
            else
            {
                status = add_presentation_relation(g, adm_pres.id, element_id);
                CHECK_STATUS_SUCCESS(status);
            }
        }
    }

    /*
     * Some elements may not be contained in a presentation.  Iterate over all the beds
     * and objects and add a presentation relation for each orphan; we collect them all
     * under the NULL presentation ID.
     *
     * One could argue these elements are not needed and could be ignored.  However,
     * ignoring them is probably not in line with the metadata author's expectations,
     * and it makes testing unnecessarily difficult.
     */

    if (dlb_pmd_bed_iterator_init(&bi, g->pmd_model))
    {
        return FAILURE;
    }

    memset(sources, 0, sizeof(sources));
    while (!dlb_pmd_bed_iterator_next(&bi, &bed, MAX_BED_CHANNEL_COUNT, sources))
    {
        if (!get_element_bit(element_set, bed.id))
        {
            unsigned int sequence_number = 0;
            dlb_pmd_element_id parent_element_id;
            if (!check_if_avs_should_be_added(g->pmd_model, bed.id, &parent_element_id, &sequence_number))
            {
                status = add_presentation_relation(g, DLB_ADM_NULL_ENTITY_ID, bed.id);
                CHECK_STATUS_SUCCESS(status);
            }
        }
    }

    if (dlb_pmd_object_iterator_init(&oi, g->pmd_model))
    {
        return FAILURE;
    }

    while (!dlb_pmd_object_iterator_next(&oi, &object))
    {
        if (!get_element_bit(element_set, object.id))
        {
            unsigned int sequence_number = 0;
            dlb_pmd_element_id parent_element_id;
            if (!check_if_avs_should_be_added(g->pmd_model, object.id, &parent_element_id, &sequence_number))
            {
                status = add_presentation_relation(g, DLB_ADM_NULL_ENTITY_ID, object.id);
                CHECK_STATUS_SUCCESS(status);
            }
        }
    }

    return PMD_SUCCESS;
}

/**
 * @brief generate frameFormat elements
 */
static
int                         /** @return 0 on success and non-zero otherwise */
generate_frame_format
    (pmd_core_model_generator *g    /**< [in] PMD -> core model converter */
    )
{
    dlb_adm_data_frame_format frame_format;
    dlb_pmd_identity_and_timing iat;
    dlb_pmd_success iat_success = dlb_pmd_iat_lookup(g->pmd_model, &iat);
    int status;

    memset(&frame_format, 0, sizeof(frame_format));
    snprintf(frame_format.type,     sizeof(frame_format.type),     "%s", "full");
    snprintf(frame_format.timeReference, sizeof(frame_format.timeReference), "%s", "local");
    frame_format.duration.fraction_numerator = 1920;
    frame_format.duration.fraction_denominator = 48000;

    if (iat_success == PMD_SUCCESS && iat.content_id.size > 0)
    {
        write_uuid(iat.content_id.data, frame_format.flow_id);
    }

    status = dlb_adm_core_model_add_frame_format(g->core_model, &frame_format);
#ifndef NDEBUG
    CHECK_STATUS(status);
#endif

    return status;
}

dlb_pmd_success
pmd_core_model_generator_generate
    (pmd_core_model_generator   *generator
    ,dlb_adm_core_model         *core_model
    ,const dlb_pmd_model        *pmd_model
    )
{
    dlb_pmd_success success = PMD_SUCCESS;

    if (pmd_core_model_generator_init(generator, core_model, pmd_model) ||
        generate_elements(generator)                                    ||
        generate_presentations(generator)                               ||
        generate_frame_format(generator)
        )
    {
        success = PMD_FAIL;
    }

    if (generator != NULL)
    {
        generator->pmd_model = NULL;
        generator->core_model = NULL;
    }

    return success;
}

dlb_pmd_success
pmd_core_model_generator_close
    (pmd_core_model_generator  **p_generator
    )
{
    pmd_core_model_generator *g;
    int status;

    if ((p_generator == NULL) || (*p_generator == NULL))
    {
        return PMD_FAIL;
    }
    g = *p_generator;

    status = dlb_adm_core_model_clear_names(&g->names);
    CHECK_STATUS_SUCCESS(status);
    memset(g, 0, sizeof(*g));
    *p_generator = NULL;

    return PMD_SUCCESS;
}
