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
    ,void **ptr               /**< [in] the entity being referred to */
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

