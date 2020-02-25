/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

/**
 * @file dlb_sadm_model.h
 * @brief internal data structure to store Dolby-constrained Serial ADM data
 */

#ifndef DLB_SADM_MODEL_H_
#define DLB_SADM_MODEL_H_

#include "dlb_pmd_api.h"

#include <math.h> 
#if defined(_MSC_VER) && !defined(INFINITY)
#  include <float.h>
#  define INFINITY (-logf(0.0f))
#  define isinf(x) (!_finite(x))
#endif


/**
 * @brief abstract type of DLB-constrained serial ADM model
 */
typedef struct dlb_sadm_model dlb_sadm_model;


/**
 * @brief enumeration of ADM dialogue types
 */
typedef enum
{
    /* non-dialogue content kind "NK" */
    DLB_SADM_CONTENT_NK               = 0, 
    DLB_SADM_CONTENT_NK_UNDEFINED     = 0,
    DLB_SADM_CONTENT_NK_MUSIC         = 1,
    DLB_SADM_CONTENT_NK_EFFECT        = 2,

    /* dialogue content kind "DK" */
    DLB_SADM_CONTENT_DK               = 10,
    DLB_SADM_CONTENT_DK_UNDEF         = 10,
    DLB_SADM_CONTENT_DK_DIALOGUE      = 11,
    DLB_SADM_CONTENT_DK_VOICEOVER     = 12,
    DLB_SADM_CONTENT_DK_SPOKEN_SUB    = 13,
    DLB_SADM_CONTENT_DK_AUD_DESC      = 14,
    DLB_SADM_CONTENT_DK_COMMENTARY    = 15,
    DLB_SADM_CONTENT_DK_EMERGENCY     = 16,

    /* mixed content kind "MK" */
    DLB_SADM_CONTENT_MK               = 20,
    DLB_SADM_CONTENT_MK_COMPLETE_MAIN = 21,
    DLB_SADM_CONTENT_MK_MIXED         = 22,
    DLB_SADM_CONTENT_MK_HEARING_IMP   = 23,    
} dlb_sadm_content_type;


/**
 * @brief enumration of different sADM 'audio object' types
 */
typedef enum
{
    DLB_SADM_PACKFMT_TYPE_DIRECT_SPEAKERS = 1,  /**< normal beds */
    DLB_SADM_PACKFMT_TYPE_MATRIX = 2,           /**< derived beds */
    DLB_SADM_PACKFMT_TYPE_OBJECT = 3,           /**< objects */
} dlb_sadm_packfmt_type;


/**
 * @brief enumeration of audio channel format speaker identification
 */
typedef enum
{
    DLB_SADM_SPEAKER_L   = 1,  /**< Room Centric Left */
    DLB_SADM_SPEAKER_R   = 2,  /**< Room Centric Right */
    DLB_SADM_SPEAKER_C   = 3,  /**< Room Centric Center */
    DLB_SADM_SPEAKER_LFE = 4,  /**< Room Centric LFE  */
    DLB_SADM_SPEAKER_LS  = 5,  /**< Room Centric Left Side Surround */
    DLB_SADM_SPEAKER_RS  = 6,  /**< Room Centric Right Side Surround  */
    DLB_SADM_SPEAKER_LRS = 7,  /**< Room Centric Left Rear Surround */
    DLB_SADM_SPEAKER_RRS = 8,  /**< Room Centric Right Rear Surround */
    DLB_SADM_SPEAKER_LTF = 9,  /**< Room Centric Left Top Front */
    DLB_SADM_SPEAKER_RTF = 10, /**< Room Centric Right Top Front */
    DLB_SADM_SPEAKER_LTM = 11, /**< Room Centric Left Top Middle */
    DLB_SADM_SPEAKER_RTM = 12, /**< Room Centric Right Top Middle */
    DLB_SADM_SPEAKER_LTR = 13, /**< Room Centric Left Top Rear */
    DLB_SADM_SPEAKER_RTR = 14, /**< Room Centric Right Top Rear */
    DLB_SADM_SPEAKER_LFW = 15, /**< Room Centric Left Wide */
    DLB_SADM_SPEAKER_RFW = 16, /**< Room Centric Right Wide */
} dlb_sadm_speaker;


/**
 * @def DLB_SADM_CFG_H_CHANNEL_MASK (0x0f)
 * @brief number of horizontal channels in speaker config
 */
#define DLB_SADM_CFG_H_CHANNEL_MASK (0x0f)   


/**
 * @def DLB_SADM_CFG_V_CHANNEL_MASK (0x0f)
 * @brief number of vertical channels in speaker config
 */
#define DLB_SADM_CFG_V_CHANNEL_MASK (0xf0)


/**
 * @def DLB_SADM_CFG_MAKE(h,v)
 * @brief generate speaker configuration value form horizontal and vertical channel counts
 */
#define DLB_SADM_CFG_MAKE(is_cart,v,h) (((is_cart)<<8) | ((v) << 4) | (0x ## h))


/**
 * @brief known SADM channel configurations
 */
typedef enum
{
    /* speaker configurations defined in BS.2051, using polar coords */
    DLB_SADM_CFG_SS_START = 0,
    DLB_SADM_CFG_SS_A = DLB_SADM_CFG_MAKE(0,0,2),  /**< 2.0   (polar coord) */
    DLB_SADM_CFG_SS_B = DLB_SADM_CFG_MAKE(0,0,5),  /**< 5.0   (polar coord) */
    DLB_SADM_CFG_SS_D = DLB_SADM_CFG_MAKE(0,4,5),  /**< 5.0.4 (polar coord) */
    DLB_SADM_CFG_SS_J = DLB_SADM_CFG_MAKE(0,4,7),  /**< 7.0.4 (polar coord) */
    DLB_SADM_CFG_SS_END = 0x100,

    /* common  */
    DLB_SADM_CFG_COMMON_START = 0x100,
    DLB_SADM_CFG_2_0  = DLB_SADM_CFG_MAKE(1,0,2),  /**< 2.0   (cartesian coord) */
    DLB_SADM_CFG_5_0  = DLB_SADM_CFG_MAKE(1,0,5),  /**< 5.0   (cartesian coord) */
    DLB_SADM_CFG_5_1  = DLB_SADM_CFG_MAKE(1,0,6),  /**< 5.1   (cartesian coord) */
    DLB_SADM_CFG_5_0_4= DLB_SADM_CFG_MAKE(1,4,5),  /**< 5.0.4 (cartesian coord) */
    DLB_SADM_CFG_5_1_4= DLB_SADM_CFG_MAKE(1,4,6),  /**< 5.1.4 (cartesian coord) */
    DLB_SADM_CFG_7_0_4= DLB_SADM_CFG_MAKE(1,4,7),  /**< 7.0.4 (cartesian coord) */
    DLB_SADM_CFG_7_1_4= DLB_SADM_CFG_MAKE(1,4,8),  /**< 7.1.4 (cartesian coord) */
    DLB_SADM_CFG_9_0_6= DLB_SADM_CFG_MAKE(1,6,9),  /**< 9.0.6 (cartesian coord) */
    DLB_SADM_CFG_9_1_6= DLB_SADM_CFG_MAKE(1,6,a),  /**< 9.0.6 (cartesian coord) */
    DLB_SADM_CFG_COMMON_END = 0x200,
} dlb_sadm_speaker_config;


/**
 * @brief an sADM reference
 */
typedef struct
{
    unsigned char data[DLB_PMD_NAME_ARRAY_SIZE];
} dlb_sadm_id;


/**
 * @brief an sADM element name
 */
typedef struct
{
    unsigned char data[DLB_PMD_NAME_ARRAY_SIZE];
} dlb_sadm_name;


/**
 * @brief type of an ID reference
 */
typedef void* dlb_sadm_idref;


/**
 * @brief an array (or list) of ID references
 */
typedef struct
{
    dlb_sadm_idref *array;
    unsigned int num;
    unsigned int max;
} dlb_sadm_idref_array;


/**
 * @brief sAMD block format
 */ 
typedef struct
{
    dlb_sadm_id id;
    unsigned char speaker_label[DLB_PMD_NAME_ARRAY_SIZE];
    dlb_pmd_bool cartesian_coordinates;
    float azimuth_or_x;
    float elevation_or_y;
    float distance_or_z;
} dlb_sadm_block_format;

    
/**
 * @brief sADM audio channel format
 */
typedef struct
{
    dlb_sadm_id id;
    dlb_sadm_name name;
    dlb_sadm_idref_array blkfmts;
} dlb_sadm_channel_format;


/**
 * @brief sADM packing format
 */
typedef struct
{
    dlb_sadm_id id;
    dlb_sadm_name name;
    dlb_sadm_packfmt_type type;
    dlb_sadm_idref_array chanfmts;
} dlb_sadm_pack_format;


/**
 * @brief type of an sADM audio channel
 */
typedef struct
{
    dlb_sadm_id id;            /**< sADM identifier string */
    dlb_sadm_idref chanfmt;    /**< channel format */
    dlb_sadm_idref packfmt;    /**< packing format */
    unsigned int channel_idx;  /**< channel index, 1 based */
} dlb_sadm_track_uid;


/**
 * @brief type of ADM audio object
 */
typedef struct
{
    dlb_sadm_id id;
    dlb_sadm_name name;
    float gain;                       /**< gain in dB */
    dlb_sadm_idref pack_format;
    dlb_sadm_idref_array track_uids;
} dlb_sadm_object;


/**
 * @brief sADM audioContent entity
 */ 
typedef struct
{
    dlb_sadm_id id;
    dlb_sadm_name name;
    unsigned int dialogue_value;
    dlb_sadm_content_type type;
    dlb_sadm_idref object; /* can this be a list? */
} dlb_sadm_content;


/**
 * @brief sADM programme label
 */
typedef struct
{
    char language[4];
    dlb_sadm_name name;
} dlb_sadm_programme_label;


/**
 * @brief sADM audio programme
 */
typedef struct
{
    dlb_sadm_id id;
    dlb_sadm_name name;
    char language[4];
    dlb_sadm_idref_array contents;

    unsigned int num_labels;
    dlb_sadm_programme_label *labels;
} dlb_sadm_programme;

    
/**
 * @brief type of structure detailing model storage limits
 */
typedef struct
{
    size_t num_programmes;
    size_t num_contents;
    size_t num_objects;
    size_t num_packfmts;
    size_t num_chanfmts;
    size_t num_blkfmts;
    size_t num_track_uids;

    size_t max_programme_labels;   /**< max labels per programme */
    size_t max_programme_contents; /**< max objects per presentation */
    size_t max_object_track_uids;  /**< max track_uids per object */
    size_t max_packfmt_chanfmts;   /**< max chan formats per pack format */
    size_t max_chanfmt_blkfmts;    /**< max block formats per channel format */
} dlb_sadm_counts;
    

/**
 * @brief type of an sADM reference
 */
typedef enum
{
    DLB_SADM_PROGRAMME = 0,
    DLB_SADM_CONTENT   = 1,
    DLB_SADM_CHANFMT   = 2,
    DLB_SADM_OBJECT    = 3,
    DLB_SADM_PACKFMT   = 4,
    DLB_SADM_TRACKUID  = 5,
    DLB_SADM_BLOCKFMT  = 6
} dlb_sadm_idref_type;


/**
 * @brief information pertaining to an instance of an undefined reference
 */
typedef struct
{
    const unsigned char *id;
    dlb_sadm_idref_type type;
    unsigned int lineno;
} dlb_sadm_undefined_ref;
    

/**
 * @brief get some defaults
 */
void
dlb_sadm_get_default_counts
    (dlb_sadm_counts *limits   /**< [in] struct to populate with defaults */
    );


/**
 * @brief return memory required for a given set of limits
 */
size_t                         /** @return size of memory required */
dlb_sadm_query_memory
    (dlb_sadm_counts *limits   /**< [in] limits struct, use NULL for defaults */
    );


/**
 * @brief initialize an sADM model from the given memroy
 */
dlb_pmd_success
dlb_sadm_init
    (dlb_sadm_counts *limits   /**< [in] limits struct, use NULL for defaults */
    ,void *mem                 /**< [in] allocated memory chunk to use */
    ,dlb_sadm_model **m        /**< [out] created model */
    );


/**
 * @brief tidy up an instance of an sADM model 
 */
void
dlb_sadm_finish
    (dlb_sadm_model *m         /**< [in] model to tidy up */
    );


/**
 * @brief reinitialize the sADM model
 */
void
dlb_sadm_reinit
    (dlb_sadm_model *m         /**< [in] model to reinitialise */
    );


/**
 * @brief set the model's error string
 */
void
dlb_sadm_set_error
    (const dlb_sadm_model *model     /**< [in] model to set */
    ,const char *fmt                 /**< [in] error message */
    ,...
    );


/**
 * @brief clear out model's error string
 */
void
dlb_sadm_error_reset
    (const dlb_sadm_model *model     /**< [in] model to set */
    );


/**
 * @brief get model's limits
 */
dlb_pmd_success                  /** @return PMD_SUCCESS on succes, PMD_FAIL otherwise */
dlb_sadm_model_limits
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_counts* limits     /**< [out] limits structure to popua;ate */
    );


/**
 * @brief get model's current entity counts
 */
dlb_pmd_success                  /** @return PMD_SUCCESS on succes, PMD_FAIL otherwise */
dlb_sadm_model_counts
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_counts*counts
    );


/**
 * @brief add (or overwrite) an audio programme
 */
dlb_pmd_success                /** @return PMD_SUCCESS on succes, PMD_FAIL otherwise */
dlb_sadm_set_programme
    (dlb_sadm_model *model     /**< [in] model to augment */
    ,dlb_sadm_programme *p     /**< [in] programme to add */
    ,dlb_sadm_idref *idref     /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio programme from its idref
 */
dlb_pmd_success                        /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_programme_lookup
    (const dlb_sadm_model *model       /**< [in] model to query */
    ,dlb_sadm_idref idref              /**< [in] idref to lookup */
    ,dlb_sadm_idref_array *contents    /**< [in] idref array to use for programme's contents */
    ,dlb_sadm_programme_label *labels  /**< [in] labels array to use for programme's labels */
    ,unsigned int num_labels           /**< [in] size of #labels array */
    ,dlb_sadm_programme *p             /**< [out] populated programme struct */
    );

/**
 * @brief type of an audio programme iterator
 */
typedef struct
{
    const dlb_sadm_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_sadm_programme_iterator;
    

/**
 * @brief initialize an audio programme iterator
 */
dlb_pmd_success                      /** @return PMD_SUCCESS on succes, PMD_FAIL otherwise */
dlb_sadm_programme_iterator_init
    (dlb_sadm_programme_iterator *it /**< [in] iterator */
    ,const dlb_sadm_model *m         /**< [in] sADM model to iterate over */
    );


/**
 * @brief return next audio programme
 */
dlb_pmd_success                       /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_programme_iterator_next
   (dlb_sadm_programme_iterator *it   /**< [in] iterator */
   ,dlb_sadm_programme *prog          /**< [in/out] programme to populate */
   ,dlb_sadm_idref_array *contents    /**< [in] idref array to use for programme's contents */
   ,dlb_sadm_programme_label *labels  /**< [in] labels array to use for programme's labels */
   ,unsigned int num_labels           /**< [in] size of #labels array */
   );


/**
 * @brief add (or overwrite) an audio content
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_set_content
    (dlb_sadm_model *model     /**< [in] model to augment */
    ,dlb_sadm_content *c       /**< [in] audioContent to add */
    ,dlb_sadm_idref *idref     /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio content from its idref
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_content_lookup
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_idref idref
    ,dlb_sadm_content *c
    );


/**
 * @brief type of an audio content iterator
 */
typedef struct
{
    const dlb_sadm_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_sadm_content_iterator;
    

/**
 * @brief initialize an audio content iterator
 */
dlb_pmd_success                      /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_content_iterator_init
    (dlb_sadm_content_iterator *it    /**< [in] iterator */
    ,const dlb_sadm_model *m
    );


/**
 * @brief return next audio content
 */
dlb_pmd_success                      /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_content_iterator_next
   (dlb_sadm_content_iterator *it    /**< [in] iterator */
   ,dlb_sadm_content *content        /**< [in/out] content struct to populate */
   );


/**
 * @brief add (or overwrite) an audio object
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_set_object
    (dlb_sadm_model *model     /**< [in] model to augment */
    ,dlb_sadm_object *object   /**< [in] audio object to add */
    ,dlb_sadm_idref *idref     /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio object from its idref
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_object_lookup
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_idref idref
    ,dlb_sadm_object *object
    ,dlb_sadm_idref_array *track_uids
    );


/**
 * @brief type of sADM audio object iterator
 */
typedef struct
{
    const dlb_sadm_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_sadm_object_iterator;
    

/**
 * @brief initialize an audio object iterator
 */
dlb_pmd_success                     /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_object_iterator_init
    (dlb_sadm_object_iterator *it
    ,const dlb_sadm_model *m
    );


/**
 * @brief return next audio object
 */
dlb_pmd_success                      /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_object_iterator_next
   (dlb_sadm_object_iterator *it     /**< [in] iterator */
   ,dlb_sadm_object *object          /**< [in/out] signal id to populate */
   ,dlb_sadm_idref_array *track_uids /**< [in] an array of idrefs to populate in #object */
   );


/**
 * @brief add (or overwrite) an audio pack format
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_set_pack_format
    (dlb_sadm_model *model     /**< [in] model to augment */
    ,dlb_sadm_pack_format *p   /**< [in] pack format to add */
    ,dlb_sadm_idref *idref     /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio pack format from its idref
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_pack_format_lookup
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_idref idref
    ,dlb_sadm_pack_format *p
    ,dlb_sadm_idref_array *chanfmts
    );


/**
 * @brief type of sADM audio pack format iterator
 */
typedef struct
{
    const dlb_sadm_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_sadm_pack_format_iterator;
    

/**
 * @brief initialize an audio pack format iterator
 */
dlb_pmd_success                     /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_pack_format_iterator_init
    (dlb_sadm_pack_format_iterator *it
    ,const dlb_sadm_model *m
    );


/**
 * @brief return next audio pack format
 */
dlb_pmd_success                        /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_pack_format_iterator_next
   (dlb_sadm_pack_format_iterator *it  /**< [in] iterator */
   ,dlb_sadm_pack_format *packfmt      /**< [in] packfmt struct to populate */
   ,dlb_sadm_idref_array *chanfmts      /**< [in] an array of idrefs to populate in #packfmt */
   );


/**
 * @brief add (or overwrite) an audio channel format
 */
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_set_channel_format
    (dlb_sadm_model *model      /**< [in] model to augment */
    ,dlb_sadm_channel_format *c /**< [in] channel format to add */
    ,dlb_sadm_idref *idref      /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio channel format from its idref
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_channel_format_lookup
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_idref idref
    ,dlb_sadm_channel_format *c
    ,dlb_sadm_idref_array *blkfmts
    );


/**
 * @brief type of sADM audio channel format iterator
 */
typedef struct
{
    const dlb_sadm_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_sadm_channel_format_iterator;
    

/**
 * @brief initialize an channel format iterator
 */
dlb_pmd_success                     /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_channel_format_iterator_init
    (dlb_sadm_channel_format_iterator *it
    ,const dlb_sadm_model *m
    );


/**
 * @brief return next audio channel format
 */
dlb_pmd_success                           /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_channel_format_iterator_next
   (dlb_sadm_channel_format_iterator *it  /**< [in] iterator */
   ,dlb_sadm_channel_format *chanfmt      /**< [in] packfmt struct to populate */
   ,dlb_sadm_idref_array *blkfmts         /**< [in] an array of idrefs to populate in #chanfmt */
   );


/**
 * @brief add (or overwrite) an audio block format
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_set_block_format
    (dlb_sadm_model *model     /**< [in] model to augment */
    ,dlb_sadm_block_format *b  /**< [in] block format to add */
    ,dlb_sadm_idref *idref     /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio block format from its idref
 */
dlb_pmd_success                  /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_block_format_lookup
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_idref idref
    ,dlb_sadm_block_format *b
    );


/**
 * @brief add (or overwrite) an audio track UID
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_set_track_uid
    (dlb_sadm_model *model     /**< [in] model to augment */
    ,dlb_sadm_track_uid *u     /**< [in] track UID to add */
    ,dlb_sadm_idref *idref     /**< [in/out] place to return idref of new entry, or NULL */
    );


/**
 * @brief look up and audio track UID from its idref
 */
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_track_uid_lookup
    (const dlb_sadm_model *model /**< [in] model to query */
    ,dlb_sadm_idref idref
    ,dlb_sadm_track_uid *u
    );


/**
 * @brief type of an audio track UID iterator
 */
typedef struct
{
    const dlb_sadm_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_sadm_track_uid_iterator;
    

/**
 * @brief initialize an audio track_uid iterator
 */
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_track_uid_iterator_init
    (dlb_sadm_track_uid_iterator *it /**< [in] iterator */
    ,const dlb_sadm_model *m         /**< [in] model to iterate over */
    );


/**
 * @brief return next audio track UID
 */
dlb_pmd_success                     /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_track_uid_iterator_next
   (dlb_sadm_track_uid_iterator *it /**< [in] iterator */
   ,dlb_sadm_track_uid *track_uid   /**< [in/out] track_uid to populate */
   );


/**
 * @brief return error message
 */
const char *                     /** @return error message */
dlb_sadm_error
    (const dlb_sadm_model *model /**< [in] model to query */
    );


/**
 * @brief lookup a generic reference
 */
dlb_pmd_success                  /** @return PMD_SUCCESS on success, PMD_FAIL otherwise */
dlb_sadm_lookup_reference
    (const dlb_sadm_model *model /**< [in] model to query */
    ,const unsigned char *id     /**< [in] idref name */
    ,dlb_sadm_idref_type type    /**< [in] idref type */
    ,unsigned int lineno         /**< [in] optional line number (or 0) */
    ,dlb_sadm_idref *ref         /**< [out] discovered reference */
    );


/**
 * @brief is the reference defined, or just a forward reference?
 */
dlb_pmd_success                  /** @return PMD_SUCCESS if idref is defined, PMD_FAIL otherwise */
dlb_sadm_idref_defined
    (dlb_sadm_idref *ref         /**< [in] reference */
    );


/**
 * @brief populate an array of as-yet undefined references in model
 */
size_t                             /** @return number of undefined references */
dlb_sadm_get_undefined_references
    (const dlb_sadm_model *model   /**< [in] model to query */
    ,dlb_sadm_undefined_ref *undef /**< [in] array of reference entries to populate */
    ,size_t capacity               /**< [in] capacity of #undef */
    );


#endif /* DLB_SADM_MODEL_H_ */
