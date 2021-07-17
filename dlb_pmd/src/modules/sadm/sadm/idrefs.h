/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file idrefs.h
 * @brief simple hash table to map SADM identifiers to pointers
 */

#include "sadm/dlb_sadm_model.h"

typedef struct idref_table idref_table;


/**
 * @def FORWARD_REFERENCE
 * @brief macro to indicate that we have a forward reference to an ID
 */
#define FORWARD_REFERENCE ((void*)-1)


/**
 * @brief size of memory to allocate
 */
size_t                          /** @return size in bytes */
idref_table_query_mem
    (dlb_sadm_counts *limits    /**< [in] associated model limits */
    );


/**
 * @brief initialize an idref table
 */
dlb_pmd_success                /** @return PMD_SUCCESS if successful, PMD_FAIL otherwise */
idref_table_init
    (dlb_sadm_counts *limits   /**< [in] model limits */
    ,void *mem                 /**< [in] allocated memory chunk */
    ,idref_table **ptr         /**< [out] new instance of the idref table */
    );


/**
 * @brief finalize an idref table
 */
void
idref_table_finish
    (idref_table *irt         /**< [in] id reference table */
    );


/**
 * @brief reinitialize an idref table
 */
void
idref_table_reinit
    (idref_table *irt         /**< [in] id reference table */
    );


/**
 * @brief insert a new entry into the table
 *
 * Normally this function will overwrite any existing entry for
 * the given key. However, if #ptr is NULL, it will only insert
 * if no key currently exists.
 */
dlb_pmd_success
idref_table_insert
    (idref_table *irt          /**< [in] id reference table */
    ,const unsigned char *id   /**< [in] name */
    ,dlb_sadm_idref_type ty    /**< [in] idref type */
    ,unsigned int lineno       /**< [in] parser line number, or 0 */
    ,void *ptr                 /**< [in] data to insert, or NULL for a forward reference */
    ,dlb_sadm_idref *result    /**< [out] the new (or overwritten) idref */
    );


/**
 * @brief look up a reference
 */
dlb_pmd_success               /** @return PMD_SUCCESS if found, PMD_FAIL otherwise */
idref_table_lookup
    (idref_table *irt         /**< [in] id reference table */
    ,const unsigned char *id  /**< [in] name of reference */
    ,dlb_sadm_idref_type ty   /**< [in] type of reference */
    ,dlb_sadm_idref *idrefp   /**< [out] idref -- may be NULL */
    ,void **ptr               /**< [out] the entity being referred to */
    );


/**
 * @brief check to see if an idref points to nothing
 */
dlb_pmd_bool                  /** @return PMD_TRUE if the reference is null, PMD_FALSE otherwise */
idref_is_null
    (const dlb_sadm_idref i   /**< [in] idref to check */
    );


/**
 * @brief check to see if an idref is for a common definition
 */
dlb_pmd_bool                  /** @return PMD_TRUE if the reference is non-null and refers to a common definition */
idref_is_common_def
    (const dlb_sadm_idref i   /**< [in] idref to check */
    );


/**
 * @brief set whether an idref is for a common definition
 */
dlb_pmd_success               /** @return PMD_SUCCESS if ok, PMD_FAIL otherwise */
idref_set_is_common_def
    (dlb_sadm_idref i         /**< [in/out] idref to modify */
    ,dlb_pmd_bool is_common   /**< [in] value to set */
    );


/**
 * @brief extract raw void* pointer to the struct an idref points to
 */
dlb_pmd_success               /** @return PMD_SUCCESS if ok, PMD_FAIL otherwise */
idref_unpack
    (dlb_sadm_idref i         /**< [in] idref to unpack */
    ,dlb_sadm_idref_type ty   /**< [in] type of reference */
    ,void **rawptr            /**< [out] pointer to the thing referenced */
    );


/**
 * @brief retreive the name of the idref
 */
dlb_pmd_success               /** @return PMD_SUCCESS if fully defined, PMD_FAIL otherwise */
idref_name
    (dlb_sadm_idref i         /**< [in] idref to check */
    ,const char **name        /**< [out] name of idref */
    );


/**
 * @brief is the idref fully defined, or is it a forward reference?
 */
dlb_pmd_success               /** @return PMD_SUCCESS if fully defined, PMD_FAIL otherwise */
idref_defined
    (dlb_sadm_idref i         /**< [in] idref to check */
    );


/**
 * @brief are the two idrefs referring to the same entity?  Note: checks the ids, not the
 *        entity pointer values!
 */
dlb_pmd_bool                  /** @return PMD_TRUE if equal, PMD_FALSE otherwise */
idref_equal
    (dlb_sadm_idref i1        /**< [in] first idref */
    ,dlb_sadm_idref i2        /**< [in] second idref */
    );


/**
 * @brief make sure there are no unresolved forward references
 *
 * An idref may occur before the entity it refers to is defined.
 * This function should be invoked at the end of the parsing process
 * to make sure that every idref is fully resolved.
 */
size_t                                /** @return number undefined references */
idref_table_get_undefined_references
    (idref_table *irt                 /**< [in] id reference table */
    ,dlb_sadm_undefined_ref *undef
    ,size_t capacity
    );

