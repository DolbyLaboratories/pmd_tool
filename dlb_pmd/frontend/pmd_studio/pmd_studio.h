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
 * @file pmd_studio.h
 * @brief global state structure for PMD studio app
 */

#ifndef __PMD_STUDIO_H__
#define __PMD_STUDIO_H__


#include "dlb_pmd_api.h"
#include "ui.h"
#include "model.h"


/**
 * @brief abstract pmd_studio type
 */ 
typedef struct pmd_studio pmd_studio;



/* make the studio accessible everywhere */
pmd_studio *studio;


dlb_pmd_model *
getModel
    (pmd_studio *studio);

/**
 * @brief reinitialize studio model to initial state
 */
void
pmd_studio_reset
    (pmd_studio *studio
    );


/* @brief get current object count */
static 
unsigned int 
getCurrentObjectCount
    (pmd_studio *studio
    );


/* @brief set current object count */
static 
void
setCurrentObjectCount
    (pmd_studio *studio
    ,int current
    );


/* @brief get imported object count */
static 
unsigned int 
getImportObjectCount
    (pmd_studio *studio
    );


/* @brief set imported object count */
static 
void
setImportObjectCount
    (pmd_studio *studio
    ,int current
    );


/* @brief increase current object count */

static 
void 
increaseCurrentObjectCount
    (pmd_studio *studio
    );


/* @brief get current presentation count */
static 
unsigned int 
getCurrentPresentationCount
    (pmd_studio *studio
    );


/* @brief set current presentation count */
static 
void 
setCurrentPresentationCount
    (pmd_studio *studio
    ,int current                
    );


/* @brief get import presentation count */
static 
int 
getImportPresentationCount
    (pmd_studio *studio
    );


/* @brief set import presentation count */
static 
void 
setImportPresentationCount
    (pmd_studio *studio
    ,int current
    );


/* @brief increase current presentation count */
static 
void 
increaseCurrentPresentationCount
    (pmd_studio *studio
    );


/* @brief get UI checkbox */
static 
uiCheckbox * 
getCheckBox
    (pmd_studio *studio
    ,int i
    ,int j
    );


/**
 * @brief read file to populate studio's model
 */
void
pmd_studio_open_file
    (pmd_studio *studio
    ,const char *filename
    );


/**
 * @brief write studio's model to file
 */
void
pmd_studio_save_file
    (pmd_studio *studio
    ,const char *filename
    ,dlb_pmd_bool sadm
    );


/**
 * @brief populate UI defs after reading a model from file
 */
void
pmd_studio_import
    (pmd_studio *studio
    );


/**
 * @brief rebuild model based on pmd_studio stored information
 */
void
pmd_studio_update_model
    (pmd_studio *studio
    );


#endif /* __PMD_STUDIO_H__ */
