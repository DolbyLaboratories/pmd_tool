/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#ifndef DLB_ADM_API_H
#define DLB_ADM_API_H

#include "dlb_adm/include/dlb_adm_data_types.h"
#include "dlb_adm/include/dlb_adm_lib_dll.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Library configuration */

DLB_ADM_DLL_ENTRY
int
dlb_adm_configure
    (const dlb_adm_library_config   *config
    );


/* Entity ID translation and construction */

DLB_ADM_DLL_ENTRY
int
dlb_adm_read_entity_id
    (dlb_adm_entity_id          *id
    ,const char                 *s
    ,size_t                      len
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_write_entity_id
    (char                       *s
    ,size_t                      len
    ,dlb_adm_entity_id           id
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_construct_generic_entity_id
    (dlb_adm_entity_id          *id
    ,DLB_ADM_ENTITY_TYPE         t
    ,uint32_t                    n
    );


/* Entity attributes */

DLB_ADM_DLL_ENTRY
int
dlb_adm_get_attribute_tag
    (DLB_ADM_TAG                *tag
    ,DLB_ADM_ENTITY_TYPE         entity_type
    ,const char                 *attribute_name
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_get_attribute_value_type
    (DLB_ADM_VALUE_TYPE         *value_type
    ,DLB_ADM_TAG                 tag
    );


/* ADM container */

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_query_memory_size
    (size_t                         *sz
    ,const dlb_adm_container_counts *counts
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_open
    (dlb_adm_xml_container             **p_container
    ,const dlb_adm_container_counts     *counts
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_open_from_core_model
    (dlb_adm_xml_container             **p_container
    ,const dlb_adm_core_model           *core_model
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_close
    (dlb_adm_xml_container     **p_container
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_load_common_definitions
    (dlb_adm_xml_container      *container
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_add_reference
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_add_relationship
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           from_id
    ,dlb_adm_entity_id           to_id
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_bool_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_bool                value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_uint_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_uint                value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_int_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_int                 value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_float_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_float               value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_audio_type_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,DLB_ADM_AUDIO_TYPE          value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_time_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,const dlb_adm_time         *value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_string_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,const char                 *value
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_bool_value
    (dlb_adm_bool                   *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_uint_value
    (dlb_adm_uint                   *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_int_value
    (dlb_adm_int                    *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_float_value
    (dlb_adm_float                  *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_audio_type_value
    (DLB_ADM_AUDIO_TYPE             *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_time_value
    (dlb_adm_time                   *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_get_string_value
    (char                           *buffer
    ,size_t                          buffer_size
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id               id
    ,DLB_ADM_TAG                     tag
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_set_mutable
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,dlb_adm_bool                is_mutable
    );

#ifdef FUTURE
DLB_ADM_DLL_ENTRY
int
dlb_adm_container_clear
    (dlb_adm_xml_container      *container
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_clear_all
    (dlb_adm_xml_container      *container
    );
#endif


/* XML Reader */

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_read_xml_buffer
    (dlb_adm_xml_container      *container
    ,const char                 *xml_buffer
    ,size_t                      character_count
    ,dlb_adm_bool                use_common_defs
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_read_xml_file
    (dlb_adm_xml_container      *container
    ,const char                 *file_path
    ,dlb_adm_bool                use_common_defs
    );

/* XML Model Flattener */

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_flatten
    (dlb_adm_xml_container      *container
    ,dlb_adm_xml_container      *flattened_container
    );

/* XML Writer */

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_write_xml_buffer
    (dlb_adm_xml_container          *container
    ,dlb_adm_write_buffer_callback   callback
    ,void                           *callback_arg
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_container_write_xml_file
    (dlb_adm_xml_container      *container
    ,const char                 *file_path
    );


/* ADM core model */

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_query_memory_size
    (size_t                             *sz
    ,const dlb_adm_core_model_counts    *counts
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_open
    (dlb_adm_core_model                **p_model
    ,const dlb_adm_core_model_counts    *counts
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_open_from_xml_container
    (dlb_adm_core_model        **p_model
    ,dlb_adm_xml_container      *container
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_ingest_xml_container
    (dlb_adm_core_model         *model
    ,dlb_adm_xml_container      *container
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_ingest_common_definitions_container
    (dlb_adm_core_model         *model
    ,dlb_adm_xml_container      *container
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_profile
    (dlb_adm_core_model         *model
    ,DLB_ADM_PROFILE             profile
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_has_profile
    (const dlb_adm_core_model   *model          /**< [in] The model to test */
    ,const DLB_ADM_PROFILE       profile        /**< [in] The profile to test against */
    ,dlb_adm_bool               *has_profile    /**< [out] Whether the specific profile is set in Core Model */
    );

/**
 * @brief Return the size of memory needed to configure a dlb_adm_data_names
 * struct for a maximum number of name/language pairs.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_query_names_memory_size
    (size_t                     *sz                         /**< [out] size of memory needed */
    ,size_t                      max_name_sz                /**< [in]  maximum length for a name, plus 1 for NUL termination */
    ,size_t                      name_limit                 /**< [in]  maximum capacity for name/language pairs */
    );

/**
 * @brief Configure a dlb_adm_data_names struct to hold model entity names using
 * client-supplied external storage.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_configure_names
    (dlb_adm_data_names         *names                      /**< [out] names struct to format */
    ,size_t                      name_limit                 /**< [in]  maximum capacity for name/language pairs */
    ,char                       *memory                     /**< [in]  memory to format for #names struct fields */
    ,size_t                      memory_size                /**< [in]  number of bytes in #memory */
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_name
    (dlb_adm_data_names         *names
    ,const char                 *name
    ,const char                 *lang
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_label
    (dlb_adm_data_names         *names
    ,const char                 *label
    ,const char                 *lang
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_has_name
    (dlb_adm_bool               *has_name
    ,dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_clear_names
    (dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_get_names
    (const dlb_adm_core_model   *model
    ,dlb_adm_data_names         *names
    ,dlb_adm_entity_id           entity_id
    );

/**
 * @brief Test whether an entity with the given #id exists in the #model.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_entity_exists
    (const dlb_adm_core_model   *model              /**< [in]  The model to test */
    ,dlb_adm_entity_id           id                 /**< [in]  The entity ID to test */
    ,dlb_adm_bool               *exists             /**< [out] Whether the entity exists in the model */
    );


/* Constructing the model */

/**
 * @brief Add a Source instance to the model.  If the #source id field has the value DLB_ADM_NULL_ENTITY_ID,
 * a new ID is created for the Source instance and is returned in the #source id field.  Thus, you can either
 * give a specific ID for the new Source or have the model create one and return it to you.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_source
    (dlb_adm_core_model         *model              /**< [in]     The model to which to add the Source instance */
    ,dlb_adm_data_source        *source             /**< [in/out] Descriptor for the new Source instance */
    );

/**
 * @brief Add one or more Source instances to the model in a contiguous channel group, starting at
 * #start_channel.  For each entry in #source_id_array, if its value is DLB_ADM_NULL_ENTITY_ID, a
 * new ID is created for the corresponding Source instance and is returned in that entry.  Thus, you
 * can either specify the IDs to be used for the new Source instances, or have the model create and
 * return them to you.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_sources
    (dlb_adm_core_model         *model              /**< [in]     The model to which to add the Source instances */
    ,dlb_adm_source_group_id     group_id           /**< [in]     Common group ID for the Source instances */
    ,dlb_adm_channel_number      start_channel      /**< [in]     First channel in the group */
    ,size_t                      channel_count      /**< [in]     Number of channels in the group */
    ,dlb_adm_entity_id          *source_id_array    /**< [in/out] Array of entity IDs for the new Source instances */
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_source_group
    (dlb_adm_core_model         *model              /**< [in]     The model to which to add the SourceGroup instance */
    ,dlb_adm_data_source_group  *source_group       /**< [in/out] Descriptor for the new SourceGroup instance */
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_target
    (dlb_adm_core_model         *model
    ,dlb_adm_data_target        *target
    ,dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_target_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_target_group  *target_group
    ,dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_audio_track
    (dlb_adm_core_model         *model
    ,dlb_adm_data_audio_track   *audio_track
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_audio_element
    (dlb_adm_core_model         *model
    ,dlb_adm_data_audio_element *audio_element
    ,dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_element_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_element_group *element_group
    ,dlb_adm_data_names         *names
    );

/**
 * @brief Add a AlternativeValueSet instance to the model.
 * - If #parent_id is DLB_ADM_NULL_ENTITY_ID, #alt_val_set.id must be the correct, full entity ID for the set.
 * - Otherwise, #alt_val_set.id must be DLB_ADM_NULL_ENTITY_ID, and #parent_id must contain the entity ID for an
 *   existing AudioElement. On exit, #alt_val_set.id will be the full entity ID for the AlternativeValueSet.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_alt_value_set
    (dlb_adm_core_model         *model          /**< [in]     The model to which to add the AlternativeValueSet instance */
    ,dlb_adm_entity_id           parent_id      /**< [in]     The entity ID of the parent AudioElement (see note above) */
    ,dlb_adm_data_alt_value_set *alt_val_set    /**< [in/out] Descriptor for the new AlternativeValueSet instance */
    ,dlb_adm_data_names         *labels         /**< [in]     Labels of AlternativeValueSet */
    );

/**
 * @brief Add a ComplementaryElement instance to the model.
 * For Complementary Leader ComplementaryElement with audio_element_id == complementary_leader_id should be created
 * for storing audioComplementaryObjectGroupLabel
 *   #comp_elem.id should be genereted as generic id for entity type DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF
 *       or if comp_elem.id is equal to DLB_ADM_NULL_ENTITY_ID, sequenceNumber for complementary element should be set
 *   #comp_elem.audio_element_id should be valid audio element id
 *   #comp_elem.complementary_leader_id should be valid audio element id
 *   #labels nullptr for ordinary ComplementaryElement, contains labels for ComplementaryLeader element
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_complementary_element
    (dlb_adm_core_model                 *model             /**< [in] The model to which to add the CompelemntaryElement instance */
    ,dlb_adm_data_complementary_element *comp_elem         /**< [in] Descriptor for the new CompelemntaryElement instance */
    ,uint32_t                            sequenceNumber    /**< [in] Sequence number of CompelemntaryElement instance */
    ,dlb_adm_data_names                 *labels            /**< [in] Labels of audioComplementaryObjectGroupLabel */
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_content_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_content_group *content_group
    ,dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_presentation
    (dlb_adm_core_model         *model
    ,dlb_adm_data_presentation  *presentation
    ,dlb_adm_data_names         *names
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_frame_format
    (dlb_adm_core_model         *model
    ,dlb_adm_data_frame_format  *frame_format
    );

/**
 * @brief Add a BlockUpdate instance to the model.
 * - If #parent_id is DLB_ADM_NULL_ENTITY_ID, #update.id must be the correct, full entity ID for the update.
 * - Otherwise, #update.id must be DLB_ADM_NULL_ENTITY_ID, and #parent_id must contain the entity ID for an
 *   existing TargetGroup.  On exit, #update.id will be the full entity ID for the update.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_block_update
    (dlb_adm_core_model         *model              /**< [in]     The model to which to add the BlockUpdate instance */
    ,dlb_adm_entity_id           parent_id          /**< [in]     The entity ID of the parent TargetGroup (see note above) */
    ,dlb_adm_data_block_update  *block_update       /**< [in/out] Descriptor for the new BlockUpdate instance */
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_source_relation
    (dlb_adm_core_model     *model
    ,dlb_adm_entity_id       source_group_id
    ,dlb_adm_entity_id       source_id
    ,dlb_adm_entity_id       audio_track_id
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_element_relation
    (dlb_adm_core_model     *model
    ,dlb_adm_entity_id       audio_element_id
    ,dlb_adm_entity_id       target_group_id
    ,dlb_adm_entity_id       target_id
    ,dlb_adm_entity_id       audio_track_id
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_add_presentation_relation
    (dlb_adm_core_model     *model
    ,dlb_adm_entity_id       presentation_id
    ,dlb_adm_entity_id       content_group_id
    ,dlb_adm_entity_id       element_group_id
    ,dlb_adm_entity_id       audio_element_id
    ,dlb_adm_entity_id       alt_value_set_id
    ,dlb_adm_entity_id       complementary_ref_id
    );

/**
 * @brief Restore the model to the empty state.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_clear
    (dlb_adm_core_model         *model
    );


/* Traversing the model */

/**
 * @brief Is the model empty?
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_is_empty
    (dlb_adm_core_model         *model
    ,dlb_adm_bool               *is_empty
    );

/**
 * @brief Return the size of memory needed to configure a dlb_adm_data_audio_element_data
 * struct for a maximum number of channels.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_query_element_data_memory_size
    (size_t                     *sz                         /**< [out] size of memory needed */
    ,dlb_adm_channel_count       channel_capacity           /**< [in]  maximum number of channels */
    ,dlb_adm_alt_val_count       alt_val_capacity           /**< [in]  maximum number of AlternativeValueSets */
    );

/**
 * @brief Configure a dlb_adm_data_audio_element_data struct using client-supplied external
 * storage.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_configure_element_data
    (dlb_adm_data_audio_element_data    *element_data       /**< [out] struct to configure */
    ,dlb_adm_channel_count               channel_capacity   /**< [in]  maximum number of channels */
    ,dlb_adm_alt_val_count               alt_val_capacity   /**< [in]  maximum number of alternative value sets */
    ,uint8_t                            *memory             /**< [in]  memory to format for #element_data struct fields */
    );

/**
 * @brief Clear the data in a dlb_adm_data_audio_element_data struct.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_clear_element_data
    (dlb_adm_data_audio_element_data    *element_data       /**< [out] struct to clear */
    );

/**
 * @brief In #model, access all the components of the AudioElement identified by #audio_element_id,
 * translate them into C structures, and copy them into #element_data.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_get_element_data
    (dlb_adm_data_audio_element_data    *element_data       /**< [out] struct to fill, must have sufficient capacity */
    ,const dlb_adm_core_model           *model              /**< [in]  core model instance to query */
    ,dlb_adm_entity_id                   audio_element_id   /**< [in]  ID of AudioElement for which to get data */
    );

/**
 * @brief Return the size of memory needed to configure a dlb_adm_data_presentation_data
 * struct for a maximum number of audio elements.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_query_presentation_data_memory_size
    (size_t                     *sz                         /**< [out] size of memory needed */
    ,dlb_adm_element_count       element_capacity           /**< [in]  maximum number of audio elements */
    );

/**
 * @brief Configure a dlb_adm_data_presentation_data struct using client-supplied external
 * storage.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_configure_presentation_data
    (dlb_adm_data_presentation_data     *presentation_data  /**< [out] struct to configure */
    ,dlb_adm_element_count               element_capacity   /**< [in]  maximum number of audio elements */
    ,uint8_t                            *memory             /**< [in]  memory to format for #presentation_data struct fields */
    );

/**
 * @brief Clear the data in a dlb_adm_data_presentation_data struct.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_clear_presentation_data
    (dlb_adm_data_presentation_data     *presentation_data  /**< [out] struct to clear */
    );

/**
 * @brief In #model, access all the components of the Presentation identified by #presentation_id,
 * translate them into C structures, and copy them into #presentation_data.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_get_presentation_data
    (dlb_adm_data_presentation_data     *presentation_data  /**< [out] struct to fill, must have sufficient capacity */
    ,const dlb_adm_core_model           *model              /**< [in]  core model instance to query */
    ,dlb_adm_entity_id                   presentation_id    /**< [in]  ID of Presentation for which to get data */
    );

/**
 * @brief Count the number of entities in #model of a particular #entity_type.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_count_entities
    (const dlb_adm_core_model   *model
    ,DLB_ADM_ENTITY_TYPE         entity_type
    ,size_t                     *count
    );

typedef
int
(*dlb_adm_for_each_callback)
    (const dlb_adm_core_model   *model
    ,dlb_adm_entity_id           entity_id
    ,void                       *callback_arg
    );

/**
 * @brief Call a callback function for each entity ID in #model of a particular
 * #entity_type.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_for_each_entity_id
    (const dlb_adm_core_model   *model
    ,DLB_ADM_ENTITY_TYPE         entity_type
    ,dlb_adm_for_each_callback   callback
    ,void                       *callback_arg
    );

/**
 * @brief Call a callback function for each audio element entity ID in #model.
 *
 * Note: we can't just use #dlb_adm_core_model_for_each_entity_id(), because
 * both ElementGroup and AudioElement use the same type of ID.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_for_each_audio_element_id
    (const dlb_adm_core_model   *model
    ,dlb_adm_for_each_callback   callback
    ,void                       *callback_arg
    );

typedef
int
(*dlb_adm_source_callback)
    (const dlb_adm_core_model   *model
    ,const dlb_adm_data_source  *source
    ,void                       *callback_arg
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_for_each_source
    (const dlb_adm_core_model   *model
    ,dlb_adm_source_callback     callback
    ,void                       *callback_arg
    );

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_get_flow_id
    (const dlb_adm_core_model   *model  /**< [in]  model to query */
    ,char                       *uuid   /**< [out] uuid buffer */
    ,size_t                      sz     /**< [in]  size in bytes of uuid buffer */
    );


/* Closing the model */

DLB_ADM_DLL_ENTRY
int
dlb_adm_core_model_close
    (dlb_adm_core_model    **p_model
    );


/* Miscellaneous */

/**
 * @brief Convert #gain to a dimensionless value in dB.
 */
DLB_ADM_DLL_ENTRY
dlb_adm_gain_value
dlb_adm_gain_in_decibels
    (dlb_adm_data_gain       gain   /**< [in] Dimensioned (dB or linear) gain value to convert */
    );

/**
 * @brief Is the file named by the path in #filename a S-ADM xml file?
 *
 * Note: #is_sadm true indicates a high probability it is actually S-ADM; #is_sadm false indicates
 * it is definitely not.
 */
DLB_ADM_DLL_ENTRY
int
dlb_adm_file_is_sadm_xml
    (dlb_adm_bool           *is_sadm    /**< [out] the result */
    ,const char             *filename   /**< [in]  the file to check */
    );


#ifdef __cplusplus
}
#endif

#endif /* DLB_ADM_API_H */
