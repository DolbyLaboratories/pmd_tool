/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
 * @file dlb_pmd_api.h
 * @brief header for programmatic API for Dolby Professional Metadata library
 */

#ifndef DLB_PMD_API_H
#define DLB_PMD_API_H

#include "dlb_pmd_types.h"
#include "dlb_pmd_lib_dll.h"

#include <stddef.h>
#include <stdarg.h>

/**
 * In Visual Studio, turn inline to __inline
 */
#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(inline)
#define inline __inline
#endif


#ifdef __cplusplus
extern "C" {
#endif


/**
  * Mask used for determining which components should be
  * considered during PMD comparison.
  */
enum dlb_pmd_equal_mask
{
    PMD_EQUAL_MASK_SIGNALS         = (0x0001 << 0),
    PMD_EQUAL_MASK_BEDS            = (0x0001 << 1),
    PMD_EQUAL_MASK_OBJECTS         = (0x0001 << 2),
    PMD_EQUAL_MASK_PRESENTATIONS   = (0x0001 << 3),
    PMD_EQUAL_MASK_HEADPHONES      = (0x0001 << 4),
    PMD_EQUAL_MASK_NUM_ED2_SYSTEM  = (0x0001 << 5),
    PMD_EQUAL_MASK_LOUDNESS        = (0x0001 << 6),
    PMD_EQUAL_MASK_IAT             = (0x0001 << 7),
    PMD_EQUAL_MASK_EAC3            = (0x0001 << 8),
    PMD_EQUAL_MASK_ED2_SYSTEM      = (0x0001 << 9),
    PMD_EQUAL_MASK_ED2_TURNAROUNDS = (0x0001 << 10),
    PMD_EQUAL_MASK_ED2_UPDATES     = (0x0001 << 11)
};

/**
 * @brief return error string
 *
 * If any of the API methods return a failure, this method will
 * return a text string that gives more information.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
const char *                    /** @return error string, or NULL if none */
dlb_pmd_error
    (const dlb_pmd_model *model /**< [in] model pertaining to the error */
    );


/**
 * @brief return library implementation version numbers
 *
 * Changes to major version reflects breakage in compatibility with
 * previous versions.  Alternatively, it may signify a significant
 * rewrite or internal change which the maintainers will wish to
 * headline with a new number.
 *
 * Changes to minor version typically reflect feature additions that
 * are backwards compatible with previous versions.
 *
 * Changes to release numbers indicate things like fixes, tidy-ups,
 * comments or documentation etc.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_library_version
    (unsigned int *epoch    /**< [out] library implementation epoch number */
    ,unsigned int *maj      /**< [out] library implementation major number */
    ,unsigned int *min      /**< [out] library implementation minor number */
    ,unsigned int *build    /**< [out] library implementation build number */
    ,unsigned int *bs_maj   /**< [out] library supported bitstream major version */
    ,unsigned int *bs_min   /**< [out] library supported bitstream minor version */
    );
    

/**
 * @brief initialize constraints to maximum values
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_max_constraints
    (dlb_pmd_model_constraints  *c                      /**< [out] model size constraints */
    ,dlb_pmd_bool                use_adm_common_defs    /**< [in]  use ADM common definitions */
    );
    

/**
 * @brief establish how much memory the client needs to allocate
 */
DLB_PMD_DLL_ENTRY
size_t                  /** @return size of memory to allocate in bytes */
dlb_pmd_query_mem
    (void
    );


/**
 * @brief establish how much memory the client needs to allocate, given
 * some maximum entity counts. Use the same max counts when allocating
 * the model using #dlb_pmd_init_constrained
 *
 * The constraints must be such that there are:
 *    - at least one bed or object
 *    - at least one presentation
 *    - at least one signal
 *    - at most one IAT and ED2 system
 *    - at least one presentation name per presentation
 *
 * If the constraints do not satisfy these conditions, this function will
 * return 0.
 */
DLB_PMD_DLL_ENTRY
size_t                                  /** @return size of memory to allocate in bytes */
dlb_pmd_query_mem_constrained
    (const dlb_pmd_model_constraints *c /**< [in] model size constraints */
    );


/**
 * @brief establish how much memory the client needs to allocate, given
 * a required maximum profile and level.  Use the same profile and memory
 * values when initialising the model using #dlb_pmd_init_profile.
 * If the profile is unknown, returns 0.
 */
DLB_PMD_DLL_ENTRY
size_t                       /** @return size of memory to allocate in bytes, or 0 if profile unknown */
dlb_pmd_query_mem_profile
    (unsigned int   profile  /**< [in] profile number */
    ,unsigned int   level    /**< [in] profile level number */
    );


/**
 * @brief initialize a region of memory to be a dlb_pmd model
 *
 * Note that this assumes that the memory provided is at least of size
 * established by the #dlb_pmd_query_mem function.  if the memory pointer
 * is NULL, memory of appropriate size will be allocated via malloc(),
 * and #dlb_pmd_finish must be called to free it.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_init
    (dlb_pmd_model **model      /**< [out] newly created model structure */
    ,void *mem                  /**< [in] memory for the model */
    );


/**
 * @brief initialize a region of memory to be a dlb_pmd model with the given
 * maximum entity limits
 *
 * Note that this assumes that the memory provided is at least of size
 * established by the #dlb_pmd_query_mem_constrained function.  if the
 * memory pointer is NULL, memory of appropriate size will be allocated
 * via malloc(), and #dlb_pmd_finish must be called to free it.
 *
 * The constraints must be such that there are:
 *    - at least one bed or object
 *    - at least one presentation
 *    - at least one signal
 *    - at most one IAT and ED2 system
 *    - at least one presentation name per presentation
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_init_constrained
    (      dlb_pmd_model **model        /**< [out] newly created model structure */
    ,const dlb_pmd_model_constraints *c /**< [in] model size constraints */
    ,      void *mem                    /**< [in] memory for the model */
    );


/**
 * @brief initialize a region of memory to be a dlb_pmd model with the given
 * maximum entity limits
 *
 * Note that this assumes that the memory provided is at least of size
 * established by the #dlb_pmd_query_mem_profile function
 *
 * Invalid parameters cause undefined behaviour.  If the profile is unknown,
 * the model is set to NULL.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_init_profile
    (dlb_pmd_model **model        /**< [out] newly created model structure */
    ,unsigned int   profile       /**< [in] profile number */
    ,unsigned int   level         /**< [in] profile level number */
    ,void *mem                    /**< [in] memory for the model */
    );


/**
 * @brief retrieve maximum constraints of a model
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_get_constraints
    (const dlb_pmd_model *model     /**< [in] PMD model */
    ,dlb_pmd_model_constraints *c   /**< [in/out] space to write model's constraints */
    );
    

/**
 * @brief reinitialize a dlb_pmd model
 *
 * This resets a model back to its newly-minted state. 
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return PMD_SUCCESS always */
dlb_pmd_reset
    (dlb_pmd_model *model      /**< [in] reinitialize model structure */
    );
    

/**
 * @brief add an error callback to a dlb_pmd model
 *
 * The function will be called when there is an error. 
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return 0 if succeeded, 1 if not */
dlb_pmd_set_error_callback
    (dlb_pmd_model                      *model  /**< [in] model */
    ,dlb_pmd_model_error_callback        fn     /**< [in] callback function pointer, or NULL */
    ,dlb_pmd_model_error_callback_arg    cbarg  /**< [in] callback function argument, may be NULL */
    );


/**
 * @brief tidy up any internal resources allocated during #dlb_pmd_init
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_finish
    (dlb_pmd_model *model       /**< [in] model to clean up */
    );


/**
 * @brief overwrite one model with the content of another
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return 0 if succeeded, 1 if not (i.e., not
                                 * enough space in destination model for entirety
                                 * of source model */
dlb_pmd_copy
    (      dlb_pmd_model *dest /**< [in] model to overwrite */
    ,const dlb_pmd_model *src  /**< [in] source model */
    );


/**
 * A standard mask that defines which components of PMD should be considered
 * during comparison.
 *
 * @see dlb_pmd_equal_mask
 */
extern DLB_PMD_DLL_ENTRY const uint32_t PMD_COMPARE_MASK;


/**
 * @brief test whether two models are equal
 *
 * More general approach than existing dlb_pmd_equal2
 * uses bit mask, allowing to choose exact components for comparison.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return 0 if equal, 1 otherwise */
dlb_pmd_equal3
    (const dlb_pmd_model *m1             /**< [in] 1st model to compare */
    ,const dlb_pmd_model *m2             /**< [in] 2nd model to compare */
    ,      dlb_pmd_bool ignore_names     /**< [in] ignore APN and AEN? */
    ,      uint32_t components_to_check  /**< [in] u32 variable where bits are set
                                           *       according to which compoments we
                                           *       want to be included in comparison.
                                           *       Uses dlb_pmd_equal_mask
                                           */
    );

/**
 * @brief test whether two models are equal
 *
 * Note that when testing PCM cases, we may not read all updates,
 * so allow the ability to ignore update checking.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return 0 if equal, 1 otherwise */
dlb_pmd_equal2
    (const dlb_pmd_model *m1            /**< [in] 1st model to compare */
    ,const dlb_pmd_model *m2            /**< [in] 2nd model to compare */
    ,      dlb_pmd_bool ignore_updates  /**< [in] ignore updates? */
    ,      dlb_pmd_bool ignore_names    /**< [in] ignore APN and AEN? */
    ,      dlb_pmd_bool minimal         /**< [in] only check beds, objects, models
                                          *       implies #ignore_updates and #ignore_names
                                          */
    );


static inline
dlb_pmd_success                   /** @return 0 if equal, 1 otherwise */
dlb_pmd_equal
    (const dlb_pmd_model *m1            /**< [in] 1st model to compare */
    ,const dlb_pmd_model *m2            /**< [in] 2nd model to compare */
    ,      dlb_pmd_bool ignore_updates  /**< [in] ignore updates? */
    ,      dlb_pmd_bool minimal         /**< [in] only check beds, objects, models */
    )
{
    return dlb_pmd_equal2(m1, m2, ignore_updates, 0, minimal);
}
    

/**
 * @brief apply a channel remapping
 *
 * (Useful when undoing the effect of an ED2 stream arrangement after a test)
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_remap_channels
    (dlb_pmd_model *m     /**< [in] model to augment */
    ,int *map             /**< [in] array of mappings, must be large enough */
    );



   
/** ------------------------ PMD READ API --------------------------- */    


/**
 * @brief read the SMPTE 2109 sample offset
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return PMD_SUCCESS on success, PMD_FAIL on failure */       
dlb_pmd_smpte2109_sample_offset
    (const dlb_pmd_model *m      /**< [in] model to query */
    ,      uint16_t *so          /**< [out] SMPTE 2109 sample offset */
    );


/**
 * @brief request the PMD bitstream revision
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                        /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_version
    (const dlb_pmd_model *model        /**< [in] model to query */
    ,      unsigned char *maj          /**< [out] bitstream major version */
    ,      unsigned char *min          /**< [out] bitstream minor version */
    );
    

/**
 * @brief read a model's title
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_title
    (const dlb_pmd_model *model /**< [in] model to query */
    ,const char **title         /**< [out] title */
    );


/**
 * @brief read the model's profile constraints, if any
 *
 * Note that if the model has *no* profile constraint, then this function
 * will return profile number 0, level 0.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_profile
    (const dlb_pmd_model *model    /**< [in] model to query */
    ,      unsigned int  *profile  /**< [out] profile number */
    ,      unsigned int  *level    /**< [out] profile level number */
    );


/**
 * @brief return the number of each kind of entity in the model
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 on success, nonzero on failure */
dlb_pmd_count_entities
    (const dlb_pmd_model *model    /**< [in] model to query */
    ,dlb_pmd_metadata_count *count /**< [out] entity counts */
    );
    

/**
 * @brief return the number of input signals (channels)
 *
 * Note that this will just be the maximum track number even if some
 * of the channels are not used in the metadata.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                     /** @return number of audio signals, 0 on error */
dlb_pmd_num_signals
    (const dlb_pmd_model *model  /**< [in] model to query */
    );
    

/**
 * @brief PMD signal iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int pos;
    unsigned int count;
} dlb_pmd_signal_iterator;


/**
 * @brief initialize a signal iterator
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 on success, 1 on failure */
dlb_pmd_signal_iterator_init
    (dlb_pmd_signal_iterator *it      /**< [in/out] iterator to initialize */
    ,const dlb_pmd_model *model       /**< [in] model to iterate */
    );
    

/**
 * @brief read the next signal out of the iterator
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 if another bed found, 1 otherwise */
dlb_pmd_signal_iterator_next
   (dlb_pmd_signal_iterator *it    /**< [in] iterator */
   ,dlb_pmd_signal *sig            /**< [in/out] signal id to populate */
   );


/**
 * @brief return number of audio bed elements
 */
DLB_PMD_DLL_ENTRY
unsigned int                   /** @return number of audio elements, 0 on error */
dlb_pmd_num_beds
   (const dlb_pmd_model *model /**< [in] model to query */
   );


/**
 * @brief lookup a bed element identfier
 *
 * Given an element identifier #id, determine if there exists a
 * bed element within the model with that identifier, and if so,
 * populate the #bed field. The #bed parameter must exist, because
 * it will be overwritten with the model's information about the
 * bed with the given #id.
 *
 * Note that this function does not allocate -- therefore in addition
 * to the actual #dlb_pmd_bed structure, we also pass in memory for
 * the array of sources that belong to the presentation.  The #sources
 * array is mandatory, and should be sized no less than
 * #DLB_PMD_MAX_BED_SOURCES.  This parameter is used to provide the
 * memory for the sources field of #bed, so bed->sources will point
 * to #sources upon successful return.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 if bed exists, 1 otherwise */
dlb_pmd_bed_lookup
    (const dlb_pmd_model *model       /**< [in] model to query */
    ,dlb_pmd_element_id id            /**< [in] bed id to look up */
    ,dlb_pmd_bed *bed                 /**< [in/out] bed structure to populate */
    ,unsigned int num_sources         /**< [in] size of #sources array to use in #bed struct */
    ,dlb_pmd_source *sources          /**< [in] memory to use for the sources array in bed struct */
    );

   
/**
 * @brief PMD bed iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_bed_iterator;
    

/**
 * @brief initialize a bed iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 on success, 1 on failure */
dlb_pmd_bed_iterator_init
    (dlb_pmd_bed_iterator *it         /**< [in/out] iterator to initialize */
    ,const dlb_pmd_model *model       /**< [in] model to iterate */
    );
    

/**
 * @brief read the next bed out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 if another bed found, 1 otherwise */
dlb_pmd_bed_iterator_next
   (dlb_pmd_bed_iterator *it       /**< [in] iterator */
   ,dlb_pmd_bed *bed               /**< [in/out] bed structure to populate */
   ,unsigned int num_sources       /**< [in] size of #sources array to use in #bed struct */
   ,dlb_pmd_source *sources        /**< [in] memory to use for the sources array in bed struct */
   );
    

/**
 * @brief return number of audio object elements
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                   /** @return number of audio elements, 0 on error */
dlb_pmd_num_objects
   (const dlb_pmd_model *model /**< [in] model to query */
   );


/**
 * @brief lookup an object element identfier
 *
 * Given an element identifier #id, determine if there exists an
 * object element within the model with that identifier, and if so,
 * populate the #object field.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                     /** @return 0 if object exists, 1 otherwise */
dlb_pmd_object_lookup
    (const dlb_pmd_model *model     /**< [in] model to query */
    ,dlb_pmd_element_id id          /**< [in] object id to look up */
    ,dlb_pmd_object *object         /**< [in/out] object structure to populate */
    );

   
/**
 * @brief PMD object iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_object_iterator;
    

/**
 * @brief initialize an object iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 on success, 1 on failure */
dlb_pmd_object_iterator_init
    (dlb_pmd_object_iterator *it   /**< [in] iterator to initialize */
    ,const dlb_pmd_model *model    /**< [in] model to iterate */
    );
    

/**
 * @brief read the next object out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return 0 if another object found, 1 otherwise */
dlb_pmd_object_iterator_next
   (dlb_pmd_object_iterator *it  /**< [in] iterator */
   ,dlb_pmd_object *object       /**< [in/out] object structure to populate */
   );


/**
 * @brief return number of object updates
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                     /** @return number of ED2 turnaround descriptions, 0 on error */
dlb_pmd_num_updates
    (const dlb_pmd_model *model  /**< [in] model to query */
    );


/**
 * @brief lookup an update
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 if update exists, 1 otherwise */
dlb_pmd_update_lookup
    (const dlb_pmd_model *model    /**< [in] model to query */
    ,unsigned int sample_offset    /**< [in] update time */
    ,dlb_pmd_element_id id         /**< [in] element id to look up */
    ,dlb_pmd_update *u             /**< [in/out] update structure to populate */
    );

   
/**
 * @brief PMD update iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_update_iterator;
    

/**
 * @brief initialize an update iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                      /** @return 0 on success, 1 on failure */
dlb_pmd_update_iterator_init
    (dlb_pmd_update_iterator *it     /**< [in] iterator to initialize */
    ,const dlb_pmd_model *model      /**< [in] model to iterate */
    );

    
/**
 * @brief read the next update out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return 0 if another update found, 1 otherwise */
dlb_pmd_update_iterator_next
   (dlb_pmd_update_iterator *it  /**< [in] iterator */
   ,dlb_pmd_update *u            /**< [in/out] update structure to populate */
   );


/**
 * @brief return number of presentations
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                     /** @return number of presentations, 0 on error */
dlb_pmd_num_presentations
    (const dlb_pmd_model *model  /**< [in] model to query */
    );


/**
 * @brief lookup a presentation identfier
 *
 * Given a presentation identifier #id, determine if there exists a
 * presentation within the model with that identifier, and if so,
 * populate the #p parameter. The #p parameter must exist, because it
 * will be overwritten with the model's information about that
 * presentation.
 *
 * Note that this function does not allocate -- therefore in addition
 * to the actual #dlb_pmd_presentation structure #p, we also pass in
 * memory for the array of elements that belong to the presentation.
 * The #elements array is mandatory, and should be sized no less than
 * #DLB_PMD_MAX_PRESENTATION_ELEMENTS.  This parameter is used to provide
 * the memory for the elements field of #p, so p->elements will point to
 * #elements upon successful return.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 if presentation exists, 1 otherwise */
dlb_pmd_presentation_lookup
    (const dlb_pmd_model * model      /**< [in] model to query */
    ,dlb_pmd_presentation_id id       /**< [in] presentation id to look up */
    ,dlb_pmd_presentation *p          /**< [in/out] presentation structure to populate */
    ,unsigned int num_elements        /**< [in] number of elements in #elements array */
    ,dlb_pmd_element_id *elements     /**< [in] array of elements to use as elements field in #p */
    );


/**
 * @brief pick a default presentation
 *
 * It may happen that a user wants a presentation that does not exist
 * in metadata, in that case the app may wish to select a 'default'
 * presentation.  Although the notion of default presentation does not
 * exist in the PMD spec, we take it to mean the first presentation
 * added to the model (either via XML or KLV bitstream)
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                      /** @return 0 on success, 1 otherwise */
dlb_pmd_default_presentation
    (const dlb_pmd_model *model      /**< [in] model to query */
    ,dlb_pmd_presentation *p         /**< [in/out] presentation structure to populate */
    ,unsigned int num_elements       /**< [in] number of elements in #elements array */
    ,dlb_pmd_element_id *elements    /**< [in] array of elements to use as elements field in #p */
    );


/**
 * @brief PMD presentation iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_presentation_iterator;
    

/**
 * @brief initialize a presentation iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return 0 on success, 1 on failure */
dlb_pmd_presentation_iterator_init
    (dlb_pmd_presentation_iterator *it  /**< [in] iterator to initialize */
    ,const dlb_pmd_model *model         /**< [in] model to iterate */
    );

    
/**
 * @brief read the next presentation out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                        /** @return 0 if another presentation found, 1 otherwise */
dlb_pmd_presentation_iterator_next
    (dlb_pmd_presentation_iterator *it /**< [in] iterator */
    ,dlb_pmd_presentation *p           /**< [in/out] presentation structure to populate */
    ,unsigned int num_elements         /**< [in] number of elements in #elements array */
    ,dlb_pmd_element_id *elements      /**< [in] array of elements to use as elements field in #p */
   );


/**
 * @brief return number of presentation loudness descriptions
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                    /** @return number of loudness descriptions, 0 on error */
dlb_pmd_num_loudness
    (const dlb_pmd_model *model /**< [in] model to query */
    );


/**
 * @brief lookup the loudness for a presentation identfier
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 if loudness exists, 1 otherwise */
dlb_pmd_loudness_lookup
    (const dlb_pmd_model *model       /**< [in] model to query */
    ,dlb_pmd_presentation_id id       /**< [in] presentation id to look up */
    ,dlb_pmd_loudness *p              /**< [in/out] loudness structure to populate */
    );

   
/**
 * @brief PMD loudness iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_loudness_iterator;
    

/**
 * @brief initialize a loudness iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return 0 on success, 1 on failure */
dlb_pmd_loudness_iterator_init
    (dlb_pmd_loudness_iterator *it      /**< [in] iterator to initialize */
    ,const dlb_pmd_model *model         /**< [in] model to iterate */
    );

    
/**
 * @brief read the next loudness out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 if another loudness found, 1 otherwise */
dlb_pmd_loudness_iterator_next
   (dlb_pmd_loudness_iterator *it  /**< [in] iterator */
   ,dlb_pmd_loudness *loudness     /**< [in/out] loudness structure to populate */
   );


/**
 * @brief return number of Identity and Timing payloads (0 or 1)
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                          /** @return 1 if IAT present, 0 otherwise */
dlb_pmd_num_iat
    (const dlb_pmd_model *model       /**< [in] model to query */
    );


/**
 * @brief return the IAT information, if it exists
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 if successful, 1 otherwise */
dlb_pmd_iat_lookup
    (const dlb_pmd_model *model       /**< [in] model to query */
    ,dlb_pmd_identity_and_timing *iat /**< [in/out] IAT structure to populate */
    );


/**
 * @brief return number of EAC3 encoding parameter descriptions
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                        /** @return number of loudness descriptions, 0 on error */
dlb_pmd_num_eac3
    (const dlb_pmd_model *model     /**< [in] model to query */
    );


/**
 * @brief lookup an EAC3 encoding parameter description
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return 0 if EAC3 encoder parameter exists, 1 otherwise */
dlb_pmd_eac3_lookup
    (const dlb_pmd_model *model  /**< [in] model to query */
    ,unsigned int id             /**< [in] EAC3 encoding parameter id to look up */
    ,dlb_pmd_eac3 *eep           /**< [in/out] loudness structure to populate */
    );

   
/**
 * @brief PMD EAC3 encoding parameter iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_eac3_iterator;
    

/**
 * @brief initialize an EAC3 encoding parameter iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 on success, 1 on failure */
dlb_pmd_eac3_iterator_init
    (dlb_pmd_eac3_iterator *it     /**< [in] iterator to initialize */
    ,const dlb_pmd_model *model    /**< [in] model to iterate */
    );

    
/**
 * @brief read the next EAC3 encoding parameter description out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return 0 if another EAC3 encoding param desc found, 1 otherwise */
dlb_pmd_eac3_iterator_next
   (dlb_pmd_eac3_iterator *it  /**< [in] iterator */
   ,dlb_pmd_eac3 *eac3         /**< [in/out] EAC3 encoding parameter structure to populate */
   );


/**
 * @brief return number of ED2 turnarounds
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                       /** @return number of ED2 turnaround descriptions, 0 on error */
dlb_pmd_num_ed2_turnarounds
    (const dlb_pmd_model *model    /**< [in] model to query */
    );


/**
 * @brief lookup an ED2 turnarond identfier
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return 0 if ED2 turnaround exists, 1 otherwise */
dlb_pmd_ed2_turnaround_lookup
    (const dlb_pmd_model *model  /**< [in] model to query */
    ,unsigned int id             /**< [in] presentation id to look up */
    ,dlb_pmd_ed2_turnaround *etd /**< [in/out] ED2 turnaround structure to populate */
    );

   
/**
 * @brief PMD ED2 turnaround iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_ed2_turnaround_iterator;
    

/**
 * @brief initialize an ED2 turnaround iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                           /** @return 0 on success, 1 on failure */
dlb_pmd_ed2_turnaround_iterator_init
    (dlb_pmd_ed2_turnaround_iterator *it  /**< [in] iterator to initialize */
    ,const dlb_pmd_model *model           /**< [in] model to iterate */
    );

    
/**
 * @brief read the next ED2 turnaround out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 if another ED2 turnaround found, 1 otherwise */
dlb_pmd_ed2_turnaround_iterator_next
   (dlb_pmd_ed2_turnaround_iterator *it  /**< [in] iterator */
   ,dlb_pmd_ed2_turnaround *etd          /**< [in/out] ED2 turnaround structure to populate */
   );


/**
 * @brief return the number of ED2 system descriptions present
 *
 * This will be 0 or 1. 
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                        /** @return number of ED2 stream descriptions, 0 on error */
dlb_pmd_num_ed2_system
    (const dlb_pmd_model *model     /**< [in] model to query */
    );


/**
 * @brief read the model's ED2 system information, if it exists
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return 0 on success, 1 otherwise */
dlb_pmd_ed2_system_lookup
    (const dlb_pmd_model *model  /**< [in] model to query */
    ,dlb_pmd_ed2_system *sys     /**< [in] ED2 system to populate */
    );
    

/**
 * @brief return number of headphone element descriptors
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
unsigned int                     /** @return number of headphone_element_descriptions, 0 on error */
dlb_pmd_num_headphone_element_desc
    (const dlb_pmd_model *model  /**< [in] model to query */
    );


/**
 * @brief lookup a headphone element description 
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 if bed exists, 1 otherwise */
dlb_pmd_hed_lookup
    (const dlb_pmd_model *model       /**< [in] model to query */
    ,dlb_pmd_element_id id            /**< [in] element id to look up */
    ,dlb_pmd_headphone *hed           /**< [in/out] headphone element structure to populate */
    );

   
/**
 * @brief PMD headphone element descriptor iterator type
 */
typedef struct
{
    const dlb_pmd_model *model;
    unsigned int count;
} dlb_pmd_hed_iterator;
    

/**
 * @brief initialize a headphone element description iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return 0 on success, 1 on failure */
dlb_pmd_hed_iterator_init
    (dlb_pmd_hed_iterator *it         /**< [in/out] iterator to initialize */
    ,const dlb_pmd_model *model       /**< [in] model to iterate */
    );
    

/**
 * @brief read the next headphone element descriptor out of the iterator
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 if another bed found, 1 otherwise */
dlb_pmd_hed_iterator_next
   (dlb_pmd_hed_iterator *it       /**< [in] iterator */
   ,dlb_pmd_headphone *hed         /**< [in/out] headphone element structure to populate */
   );


/** ------------------------ PMD Write API --------------------------- */    


/**
 * @brief alter the SMPTE 2109 sample offset
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success            /** @return PMD_SUCCESS on success, PMD_FAIL on failure */       
dlb_pmd_set_smpte2109_sample_offset
    (dlb_pmd_model *m      /**< [in] model to populate */
    ,uint16_t so           /**< [in] starting sample offset */
    );


/**
 * @brief add a SMPTE 2109 dynamic tag
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success              /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_remap_local_tag
    (dlb_pmd_model *m        /**< [in] model to populate */
    ,uint16_t localtag       /**< [in] local tag to remap */
    ,const uint8_t *ul       /**< [in] universal label */
    );


/**
 * @brief set model title
 *
 * The title is used to generate default names for ED2 streams.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success          /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_title
    (dlb_pmd_model *m    /**< [in] model to populate */
    ,const char *title   /**< [in] name of model */
    );


/**
 * @brief set model profile
 *
 * The (optional) profile information specifies constraints on the
 * number of 'things' that can be stored in the model. These are
 * intended to reflect real-world usage scenarios for different
 * applications.
 *
 * This method will fail if the specified profile and level are not
 * currently understood by the library.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success              /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_profile
    (dlb_pmd_model *model    /**< [in] model to populate */
    ,unsigned int   profile  /**< [in] profile number */
    ,unsigned int   level    /**< [in] profile level number */
    );


/**
 * @brief unset a model profile
 */
static inline
dlb_pmd_success              /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_unset_profile
    (dlb_pmd_model *model    /**< [in] model to query */
    )
{
    return dlb_pmd_profile(model, 0, 0);
}


/**
 * @brief programatically add a signal into the model
 *
 * This adds a single signal to the model.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_signal
    (dlb_pmd_model *m            /**< [in] model to populate */
    ,dlb_pmd_signal s            /**< [in] PCM channel position, 1-based, 1-255 */
    );


/**
 * @brief programatically add a set of signals into the model
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_signals
    (dlb_pmd_model *m            /**< [in] model to populate */
    ,unsigned int num_signals    /**< [in] number of signals to add */
    );


/**
 * @brief programmatically insert a bed into model to augment
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_bed
    (dlb_pmd_model *m            /**< [in] model to augment */
    ,dlb_pmd_element_id id       /**< [in] desired element identifier */
    ,const char *name            /**< [in] bed name, or NULL */
    ,dlb_pmd_speaker_config cfg  /**< [in] channel config of object to add */
    ,unsigned int first_signal   /**< [in] 1st signal number, 1 based */
    ,int origin                  /**< [in] id of originating bed if derived, 0 otherwise */
    );


/**
 * @brief update or add a bed element
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_bed
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_bed   *bed           /**< [in] overwrite/add bed */
    );


/**
 * @brief programmatically insert a generic object into model to augment
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * @note in the following, object divergence indicates that the energy
 * from an object is smeared across front channels. When done, it is
 * typically done with dialog channels, for example.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_object
    (dlb_pmd_model *m             /**< [in] model to augment */
    ,dlb_pmd_element_id id        /**< [in] desired element id */
    ,const char *name             /**< [in] object name, or NULL */
    ,dlb_pmd_object_class cls     /**< [in] class of object */
    ,unsigned int signal          /**< [in] PCM track number, 1 based */
    ,dlb_pmd_coordinate x         /**< [in] X-coordinate (left-right) */
    ,dlb_pmd_coordinate y         /**< [in] Y-coordinate (front-back) */
    ,dlb_pmd_coordinate z         /**< [in] Z-coordinate (up-down) */
    ,dlb_pmd_gain gain            /**< [in] source signal gain */
    ,dlb_pmd_size size            /**< [in] object size */
    ,dlb_pmd_bool size_vertical   /**< [in] object flat or spherical? */
    ,dlb_pmd_bool dynamic_update  /**< [in] object static position or dynamic? */
    ,dlb_pmd_bool diverge         /**< [in] object divergence? */
    );


/**
 * @brief update or add an object element
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_object
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_object *object       /**< [in] overwrite/add object */
    );


/**
 * @brief programmatically insert a generic object into model to augment
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * @note in the following, object divergence indicates that the energy
 * from an object is smeared across front channels. When done, it is
 * typically done with dialog channels, for example.
 *
 * Invalid parameters cause undefined behaviour.
 */
static inline
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_generic_obj
    (dlb_pmd_model *m             /**< [in] model to augment */
    ,dlb_pmd_element_id id        /**< [in] desired element id */
    ,const char *name             /**< [in] object name, or NULL */
    ,unsigned int signal          /**< [in] PCM track number, 1 based */
    ,dlb_pmd_coordinate x         /**< [in] X-coordinate (left-right) */
    ,dlb_pmd_coordinate y         /**< [in] Y-coordinate (front-back) */
    ,dlb_pmd_coordinate z         /**< [in] Z-coordinate (up-down) */
    ,dlb_pmd_gain gain            /**< [in] source signal gain */
    ,dlb_pmd_size size            /**< [in] object size */
    ,dlb_pmd_bool size_vertical   /**< [in] object flat or spherical? */
    ,dlb_pmd_bool dynamic_update  /**< [in] object static position or dynamic? */
    ,dlb_pmd_bool diverge         /**< [in] object divergence? */
    )
{
    return dlb_pmd_add_object(m, id, name, PMD_CLASS_GENERIC, signal, x, y, z,
                              gain, size, size_vertical, dynamic_update, diverge);
}


/**
 * @brief programmatically insert an abbreviated generic object into model to augment
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * Invalid parameters cause undefined behaviour. 
 */
static inline
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_generic_obj2
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,dlb_pmd_element_id id    /**< [in] desired element id */
    ,const char *name         /**< [in] object name, or NULL */
    ,unsigned int signal      /**< [in] PCM track number, 1 based */
    ,dlb_pmd_coordinate x     /**< [in] X-coordinate (left-right) */
    ,dlb_pmd_coordinate y     /**< [in] Y-coordinate (front-back) */
    ,dlb_pmd_coordinate z     /**< [in] Z-coordinate (up-down) */
    )
{
    return dlb_pmd_add_object(m, id, name, PMD_CLASS_GENERIC, signal, x, y, z, 0.0, 0.0, 0, 0, 0);
}
    

/**
 * @brief programmatically insert an abbreviated dialogue object into model to augment
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * @note divergence controls how far from center a channel is spread
 * across the fronts. This is a value between 0.0 and 1.0 that
 * indicates how far the energy spreads from the center in both
 * directions. So, a divergence of 0.0 means no spread, all energy is
 * located at the center. A divergence of 0.5 means that the channel
 * is spread from x = -0.5 to x = 0.5. A divergence of 1.0 means that
 * the energy is spread from x = -1.0 to x = 1.0.
 *
 * Invalid parameters cause undefined behaviour. 
 */
static inline
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_dialog
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,dlb_pmd_element_id id    /**< [in] desired element id */
    ,const char *name         /**< [in] object name, or NULL */
    ,unsigned int signal      /**< [in] PCM track number, 1 based */
    ,float divergence         /**< [in] divergence, 0 - 1.0 */
    )
{
    dlb_pmd_bool diverge = 0;
    float x = 0.0;

    if (divergence < 0.0f || divergence > 1.0f) return PMD_FALSE;

    x -= divergence;
    diverge = x != 0.0f;

    return dlb_pmd_add_object(m, id, name, PMD_CLASS_DIALOG, signal, x,
                              1.0, 0, 0.0, 0.0, 0, 0, diverge);
}


/**
 * @brief programmatically insert an abbreviated Video Description
 * Service (VDS) object into the model
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * @note divergence controls how far from center a channel is spread
 * across the fronts. This is a value between 0.0 and 1.0 that
 * indicates how far the energy spreads from the center in both
 * directions. So, a divergence of 0.0 means no spread, all energy is
 * located at the center. A divergence of 0.5 means that the channel
 * is spread from x = -0.5 to x = 0.5. A divergence of 1.0 means that
 * the energy is spread from x = -1.0 to x = 1.0.
 *
 * Invalid parameters cause undefined behaviour. 
 */
static inline
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_vds
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,dlb_pmd_element_id id    /**< [in] desired element id */
    ,const char *name         /**< [in] object name, or NULL */
    ,unsigned int signal      /**< [in] PCM track number, 1 based */
    ,float divergence         /**< [in] divergence, 0 - 1.0 */
    )
{
    dlb_pmd_bool diverge = 0;
    float x = 0.0;

    if (divergence < 0.0f || divergence > 1.0f) return PMD_FALSE;

    x -= divergence;
    diverge = x != 0.0f;

    return dlb_pmd_add_object(m, id, name, PMD_CLASS_VDS, signal,
                              x, 1.0, 0.0, 0.0, 0.0, 0, 0, diverge);
}


/**
 * @brief programmatically insert an abbreviated voiceover object into model
 *
 * Note that this assumes that the signals are inserted into the model
 * in increasing order, numbered 1,2,3,... etc.
 *
 * @note divergence controls how far from center a channel is spread
 * across the fronts. This is a value between 0.0 and 1.0 that
 * indicates how far the energy spreads from the center in both
 * directions. So, a divergence of 0.0 means no spread, all energy is
 * located at the center. A divergence of 0.5 means that the channel
 * is spread from x = -0.5 to x = 0.5. A divergence of 1.0 means that
 * the energy is spread from x = -1.0 to x = 1.0.
 *
 * Invalid parameters cause undefined behaviour. 
 */
static inline
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_voiceover
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,dlb_pmd_element_id id    /**< [in] desired element id */
    ,const char *name         /**< [in] object name, or NULL */
    ,unsigned int signal      /**< [in] PCM track number, 1 based */
    ,float divergence         /**< [in] divergence, 0 - 1.0 */
    )
{
    dlb_pmd_bool diverge = 0;
    float x = 0.0;

    if (divergence < 0.0f || divergence > 1.0f) return PMD_FALSE;

    x -= divergence;
    diverge = x != 0.0f;

    return dlb_pmd_add_object(m, id, name, PMD_CLASS_VOICEOVER, signal,
                              x, 1.0, 0.0, 0.0, 0.0, 0, 0, diverge);
}


/**
 * @brief programmatically insert a presentation into model to augment
 *
 * This assumes that the presentations are inserted in order
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_presentation
    (dlb_pmd_model *m           /**< [in] model to augment */
    ,dlb_pmd_presentation_id id /**< [in] desired presentation id */
    ,const char *lang           /**< [in] ISO 639-1 or ISO 639-2 (B or T) language code */
    ,const char *name           /**< [in] name in UTF-8 */
    ,const char *namelang       /**< [in] language of name */
    ,dlb_pmd_speaker_config cfg /**< [in] channel config of object to add */
    ,int num_elements           /**< [in] number of elements in presentation */
    ,dlb_pmd_element_id *els    /**< [in] array of element ids of size #num_elements */
    );


/**
 * @brief helper function to allow presentation to be built in one command line
 *
 * This technique supports a maximum of 32 elements; if more needed use
 * #dlb_pmd_add_presentation directly
 *
 * Invalid parameters cause undefined behaviour.
 */
static inline
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_presentation2
    (dlb_pmd_model *m           /**< [in] model to augment */
    ,dlb_pmd_presentation_id id /**< [in] desired presentation id */
    ,const char *lang           /**< [in] ISO 639-1 or ISO 639-2 (B or T) language code */
    ,const char *name           /**< [in] name in UTF-8 */
    ,const char *namelang       /**< [in] language of name */
    ,dlb_pmd_speaker_config cfg /**< [in] channel config of object to add */
    ,int num_elements           /**< [in] number of elements in presentation */
    ,...                        /**< [in] sequence of element ids */
    )
{
    va_list ep;
    dlb_pmd_element_id eids[32];
    int i;

    if (num_elements > (int)(sizeof(eids)/sizeof(eids[0])))
    {
        return PMD_FAIL;
    }

    va_start(ep, num_elements);
    for (i = 0; i != num_elements; ++i)
    {
        eids[i] = (dlb_pmd_element_id)va_arg(ep, int);
    }
    va_end(ep);
    return dlb_pmd_add_presentation(m, id, lang, name, namelang, cfg, num_elements, eids);
}


/**
 * @brief add a presentation name
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_presentation_name
    (dlb_pmd_model           *m    /**< [in] model to augment */
    ,dlb_pmd_presentation_id  id   /**< [in] presentation to name */
    ,const char              *lang /**< [in] language of name */
    ,const char              *name /**< [in] name in UTF-8 */
    );


/**
 * @brief update or add a presentation
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_presentation
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_presentation *p      /**< [in] overwrite/add presentation */
    );


/**
 * @brief programmatically add an update into model to augment
 *
 * Note that PCM+PMD workflows restrict the first PMD timeslice to ABD,AOD,
 * APD and HED payloads only.  Since no XYZ (update) payload may occur in
 * this timeslice, any updated with time < 5 will be ignored.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_update
    (dlb_pmd_model *m       /**< [in] model to augment */
    ,dlb_pmd_element_id id  /**< [in] id of object element to update (must have dynamic_updates) */
    ,unsigned int time      /**< [in] num 32-sample blocks since object was described */
    ,dlb_pmd_coordinate x   /**< [in] new X coordinate */
    ,dlb_pmd_coordinate y   /**< [in] new Y coordinate */
    ,dlb_pmd_coordinate z   /**< [in] new Z coordinate */
    );


/**
 * @brief update or add an object update
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_update
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_update *u            /**< [in] overwrite/add object update */
    );


/**
 * @brief programmatically add some trivial EAC3 encoding parameters into model
 *
 * The EAC3 encoding parameters payload has three optional components:
 *   - encoder parameters,
 *   - bitstream parameters and
 *   - DRC parameters
 *
 * This function assumes none of these components are present, they must be added
 * by the pertinent function.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success             /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_eac3_encoding_parameters
    (dlb_pmd_model *m       /**< [in] model to augment */
    ,int id                 /**< [in] desired EAC3 encoding parameter id */
    );


/**
 * @brief add encoder params to EAC3 encoding parameters
 *
 * The encoder params are an optional part of the EAC3 encoding parameters payload,
 * and are not added by #dlb_pmd_add_eac3_encoding_parameters.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_eep_add_encoder_params
    (dlb_pmd_model *m           /**< [in] model to augment */
    ,unsigned int   id          /**< [in] eac3 encoding parameters id */
    ,dlb_pmd_compr  dynrng_prof /**< [in] compression profile for dynrng DRC gain
                                  * words for DD+ output */
    ,dlb_pmd_compr  compr_prof  /**< [in] RF mode (heavy) compression profile */
    ,dlb_pmd_bool   surround90  /**< [in] 90-degree phase shift in surrounds? */
    ,unsigned char  hmixlev     /**< [in] Height mix level */
    );


/**
 * @brief add optional bitstream info to EAC3 encoding parameters
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                       /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_eep_add_bitstream_params
    (dlb_pmd_model    *m              /**< [in] model to augment */
    ,unsigned int      id             /**< [in] eac3 encoding parameters id*/
    ,dlb_pmd_bsmod     bsmod          /**< [in] bitstream mode */
    ,dlb_pmd_surmod    dsurmod        /**< [in] Dolby surround mode status */
    ,dlb_pmd_dialnorm  dialnorm       /**< [in] dialogue normalization */
    ,dlb_pmd_prefdmix  dmixmod        /**< [in] preferred downmix mode */
    ,dlb_pmd_cmixlev   ltrtcmixlev    /**< [in] Center downmix for LtRt */
    ,dlb_pmd_surmixlev ltrtsurmixlev  /**< [in] Surround downmix level for LtRt */
    ,dlb_pmd_cmixlev   lorocmixlev    /**< [in] Center downmix for LoRo */
    ,dlb_pmd_surmixlev lorosurmixlev  /**< [in] Surround downmix level for LoRo */
    );


/**
 * @brief add optional DRC info to EAC3 encoding parameters
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_eep_add_drc_params
    (dlb_pmd_model *m          /**< [in] model to augment */
    ,unsigned int   id         /**< [in] eac3 encoding parameters id*/
    ,dlb_pmd_compr  port_spkr  /**< [in] DRC for portable speakers */
    ,dlb_pmd_compr  port_hp    /**< [in] DRC for portable headphones */
    ,dlb_pmd_compr  flat_panl  /**< [in] DRC for flat panel TV */
    ,dlb_pmd_compr  home_thtr  /**< [in] DRC for home theaters */
    ,dlb_pmd_compr  ddplus     /**< [in] DRC for DD+ */
    );


/**
 * @brief add another presentation to the EAC3 encoding parameters
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_eep_add_presentation
    (dlb_pmd_model *m              /**< [in] model to augment */
    ,unsigned int   id             /**< [in] eac3 encoding parameters id */
    ,unsigned int   pres_id        /**< [in] id of presentation to add */
    );


/**
 * @brief update or add an EAC3 encoding parameters payload
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_eac3
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_eac3 *e              /**< [in] overwrite/add EAC3 encoding parameters */
    );


/**
 * @brief add optional ED2 turnaround struct
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_etd
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,int id                   /**< [in] desired ED2 turnaround id */
    );


/**
 * @brief add ED2 turnaround info to an ED2 turnaround struct
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                     /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_etd_add_ed2
    (dlb_pmd_model     *m           /**< [in] model to augment */
    ,int                etd         /**< [in] ED2 turnaround id */
    ,dlb_pmd_frame_rate framerate   /**< [in] ED2 frame rate */
    );


/**
 * @brief add an optional presentation to Turnaround
 *
 * Each turnaround payload can be associated with several different
 * presentations; each presentation is (re)encoded according to the
 * associated EAC3 encoding parameters struct before being turned
 * around and retransmitted.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_etd_add_ed2_presentation
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,unsigned int   id        /**< [in] ED2 turnaround id */
    ,unsigned int   pres_id   /**< [in] presentation id */
    ,unsigned int   apm_id    /**< [in] AC3 program metadata id */
    );


/**
 * @brief helper function to add a complete ED2 turnaround
 *
 * Invalid parameters cause undefined behaviour. 
 */
static inline
dlb_pmd_success                        /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_ed2_turnaround
    (dlb_pmd_model     *m              /**< [in] model to augment */
    ,int                id             /**< [in] desired ED2 turnaround id */
    ,dlb_pmd_frame_rate framerate      /**< [in] ED2 frame rate */
    ,unsigned int       num_pres       /**< [in] number of presentations */
    ,...                               /**< list of pres_id, apm_id pairs */
    )
{
    unsigned int i;
    va_list ep;

    if (dlb_pmd_add_etd(m, id)) return 1;
    if (dlb_pmd_etd_add_ed2(m, id, framerate)) return 1;
    va_start(ep, num_pres);
    for (i = 0; i != num_pres; ++i)
    {
        int presid = va_arg(ep, int);
        int apmid = va_arg(ep, int);
        if (dlb_pmd_etd_add_ed2_presentation(m, id, presid, apmid))
        {
            return PMD_FAIL;
        }
    }
    va_end(ep);
    return PMD_SUCCESS;
}


/**
 * @brief add DE turnaround info to an ED2 turnaround
 * 
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                           /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_etd_add_de
    (dlb_pmd_model            *m          /**< [in] model to augment */
    ,int                       etd        /**< [in] ED2 turnaround id */
    ,dlb_pmd_frame_rate        framerate  /**< [in] DE frame rate */
    ,dlb_pmd_de_program_config pgm_config /**< [in] DE program config */
    );


/**
 * @brief add DE presentation list to Turnaround
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_etd_add_de_presentation
    (dlb_pmd_model *m             /**< [in] model to augment */
    ,unsigned int  id             /**< [in] ED2 turnaround id */
    ,unsigned int  pres_id        /**< [in] presentation id */
    ,unsigned int  apm_id         /**< [in] AC3 program metadata id */
    );


/**
 * @brief helper function to add a complete DE turnaround
 * 
 * Invalid parameters cause undefined behaviour.
 */
static inline
dlb_pmd_success                           /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_de_turnaround
    (dlb_pmd_model            *m          /**< [in] model to augment */
    ,int                       id         /**< [in] desired ED2 turnaround id */
    ,dlb_pmd_frame_rate        framerate  /**< [in] DE frame rate */
    ,dlb_pmd_de_program_config pgm_config /**< [in] DE program config */
    ,unsigned int              num_pres   /**< [in] number of presentations */
    ,...                                  /**< list of pres_id, apm_id pairs */
    )
{
    unsigned int i;
    va_list ep;

    if (dlb_pmd_add_etd(m, id)) return 1;
    if (dlb_pmd_etd_add_de(m, id, framerate, pgm_config)) return 1;

    va_start(ep, num_pres);
    for (i = 0; i != num_pres; ++i)
    {
        int presid = va_arg(ep, int);
        int apmid = va_arg(ep, int);
        if (dlb_pmd_etd_add_de_presentation(m, id, presid, apmid))
        {
            return PMD_FAIL;
        }
    }
    va_end(ep);
    return PMD_SUCCESS;
}


/**
 * @brief update or add an ED2 turnaround
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_ed2_turnaround
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_ed2_turnaround *e    /**< [in] overwrite/add ED2 turnaround */
    );


/**
 * @brief add an IAT
 *
 * Invalid parameters cause undefined behaviour. 
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_add
    (dlb_pmd_model *m              /**< [in] model to augment */
    ,uint64_t       timestamp      /**< [in] timestamp - only mandatory field (35 bits) */
    );


/**
 * @brief set IAT content ID to a UUID
 *
 * Set a 'human-readable' UUID as Content ID
 *    xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_content_id_uuid
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,const char    *uuid      /**< [in] uuid */
    );


/**
 * @brief set IAT content ID to an EIDR
 *
 * [10.]5240[/]XXXX[-]XXXX[-]XXXX[-]XXXX[-]XXXX[-][C]
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_content_id_eidr
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,const char    *eidr      /**< [in] eidr */
    );


/**
 * @brief set IAT content ID to an Ad-ID
 *
 * 11 character code consistig of letters and numbers, with an optional
 * 22th character, H or D.
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_content_id_ad_id
    (dlb_pmd_model *m          /**< [in] model to augment */
    ,const char    *ad_id      /**< [in] Ad-ID */
    );


/**
 * @brief set IAT content ID to a binary value
 *
 * We give an up-to 16 character byte array plus a type
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                     /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_content_id_raw
    (dlb_pmd_model          *m      /**< [in] model to augment */
    ,dlb_pmd_content_id_type type   /**< [in] content id 'type' : 3 - 0x1e */
    ,size_t                  len    /**< [in] length of data, up to 16 */
    ,uint8_t                *data   /**< [in] up to 16 bytes of data */
    );


/**
 * @brief set IAT distribution ID to an ATSC3 VP1 Channel ID
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_distribution_id_atsc3
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,uint16_t       bsid      /**< [in] 16-bit bsid */
    ,uint16_t       majno     /**< [in] 10-bit major channel number */
    ,uint16_t       minno     /**< [in] 10-bit minor channel number */
    );


/**
 * @brief set IAT distribution ID to a binary value
 *
 * We give an up-to 32 character byte array plus a type
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                        /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_distribution_id_raw
    (dlb_pmd_model *m                  /**< [in] model to augment */
    ,dlb_pmd_distribution_id_type type /**< distribution id type */
    ,size_t len                        /**< byte length */
    ,uint8_t *data                     /**< raw byte array */
    );


/**
 * @brief add optional offset field to IAT
 * 
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_set_offset
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,uint16_t       offset    /**< [in] 11-bit offset */
    );


/**
 * @brief add optional validity duration field to IAT
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success               /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_set_validity_duration
    (dlb_pmd_model *m         /**< [in] model to augment */
    ,uint16_t       vdur      /**< [in] 11-bit validity duration */
    );


/**
 * @brief add optional user data to IAT
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_set_user_data
    (dlb_pmd_model *m           /**< [in] model to augment */
    ,size_t         size        /**< [in] user data bytes */
    ,uint8_t       *data        /**< [in] array of user data bytes */
    );


/**
 * @brief add optional extension to IAT
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success           /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_iat_set_extension
    (dlb_pmd_model *m     /**< [in] model to augment */
    ,size_t         size  /**< [in] extension bytes */
    ,uint8_t       *data  /**< [in] array of extensionbytes */
    );


/**
 * @brief update or add Identity and Timing information
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                        /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_iat
    (dlb_pmd_model *m                  /**< [in] model to augment */
    ,dlb_pmd_identity_and_timing *iat  /**< [in] overwrite/add identity and timing */
    );


/**
 * @brief add loudness payload
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_loudness
    (      dlb_pmd_model    *m     /**< [in] model to augment */
    ,const dlb_pmd_loudness *pld   /**< [in] presentation loudness struct */
    );


#if 0
/**
 * @brief add an element name
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                 /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_element_name
    (dlb_pmd_model        *m    /**< [in] model to augment */
    ,dlb_pmd_element_id    id   /**< [in] element (bed or object) to name */
    ,const char           *name /**< [in] name in UTF-8 */
    );
    

/**
 * @brief add an ED2 stream name
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success           /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_add_ed2_stream_name
    (dlb_pmd_model *m     /**< [in] model to augment */
    ,unsigned int   idx   /**< [in] ED2 stream index 0 - 15 */
    ,const char    *lang  /**< [in] language of name */
    ,const char    *name  /**< [in] name in UTF-8 */
    );
#endif


/**
 * @brief update or add an ED2 system
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_ed2_system
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_ed2_system *sys      /**< [in] overwrite/add ED2 system */
    );


/**
 * @brief add a headphone element description
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_set_headphone_element
    (      dlb_pmd_model *m             /**< [in] model to augment */
    ,const dlb_pmd_headphone *hed       /**< [in] headphone element to add */
    );


/* ----------------------- apply updates ------------------------ */


/**
 * @brief apply all updates to a model
 *
 * The scope of an update is just a single video frame.  That is, by
 * the time the video frame has elapsed, all updates will have been
 * applied to the model, and all objects will be at their final
 * positions.  If we then genearate a second frame of audio, we want
 * to make sure that the position of the objects at the start of the
 * second frame matches the positions at the end of the previous
 * frame.  Otherwise, we would hear the objects repeatedly jump back
 * to their starting positions and move over and over again.
 *
 * This has the effect of adjusting all the relevant object positions
 * and deleting the updates from the model.
 *
 * If the model has an IAT, this will also update the IAT's timestamp.
 * 
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_apply_updates
    (dlb_pmd_model *m             /**< [in] model to augment */
    ,dlb_pmd_frame_rate rate      /**< [in] video frame rate */
    );


/**
 * @brief prune away signals not used by the elements in the model
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_prune_unused_signals
    (dlb_pmd_model *m             /**< [in] model to augment */
    );
   

/* ----------------------- metadata sets ------------------------ */


/**
 * @brief determine memory requirements for a metadata set
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
size_t                              /** @return size to malloc, or 0 on error */
dlb_pmd_metadata_set_query_memory
    (const dlb_pmd_model *model     /**< [in] model from which to generate metadata set */
    );


/**
 * @brief return maximum memory requirements for a metadata set
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
size_t                              /** @return size to malloc, or 0 on error */
dlb_pmd_metadata_set_max_memory
    (void
    );


/**
 * @brief generate a metadata set from a given model
 *
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                   /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_create_metadata_set
    (const dlb_pmd_model *model   /**< [in] model from which to generate metadata set */
    ,void *memory                 /**< [in] pre-allocated memory to use */
    ,dlb_pmd_metadata_set **mdset /**< [out] constructed metadata set */
    );


/**
 * @brief ingest an entire metadata set
 *
 * This will erase all content from a model and replace it with the
 * metadata set.
 * 
 * Invalid parameters cause undefined behaviour.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_ingest_metadata_set
    (      dlb_pmd_model        *m      /**< [in] model to create */
    ,const dlb_pmd_metadata_set *mdset  /**< [in] metadata set */
    );


/** -----------------  payload set read/write status  ------------------------- */


/**
 * @brief Initialize a payload set status record
 *
 * Initialize a payload set status record, using the given array for the XYZ payloads.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_initialize_payload_set_status
    (dlb_pmd_payload_set_status     *payload_set_status /**< [in/out] Payload set status record to initialize */
    ,dlb_pmd_payload_status_record  *xyz_status         /**< [in]  Array of XYZ payload status records to use, may be NULL */
    ,unsigned int                    xyz_count          /**< [in]  Number of XYZ payload status records in the array */
    );


/**
 * @brief Initialize a payload set status record, including callback information
 *
 * Initialize a payload set status record, using the given array for the XYZ payloads.  The callback will be called
 * at the end of payload set processing.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                                         /** @return PMD_SUCCESS on success, PMD_FAIL on failure */
dlb_pmd_initialize_payload_set_status_with_callback
    (dlb_pmd_payload_set_status             *payload_set_status /**< [in/out] Payload set status record to initialize */
    ,dlb_pmd_payload_status_record          *xyz_status         /**< [in]  Array of XYZ payload status records to use, may be NULL */
    ,unsigned int                            xyz_count          /**< [in]  Number of XYZ payload status records in the array */
    ,void                                   *callback_arg       /**< [in]  Special-purpose argument to callback function */
    ,dlb_pmd_payload_set_status_callback     callback           /**< [in]  Callback function pointer */
    );


/**
 * @brief Clear (reset) the data in a payload set status record
 */
DLB_PMD_DLL_ENTRY
void
dlb_pmd_clear_payload_set_status
    (dlb_pmd_payload_set_status *payload_set_status     /**< [in/out] Payload set status record to clear */
    );



#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_API_H */
