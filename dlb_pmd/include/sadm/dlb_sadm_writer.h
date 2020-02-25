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
 * @file dlb_sadm_writer.h
 * @brief definitions for writing serial ADM to XML format
 */

#ifndef DLB_SADM_WRITER_H
#define DLB_SADM_WRITER_H

#include "dlb_sadm_model.h"
#include "dlb_pmd_xml.h"

#ifdef __cplusplus
extern "C" 
{
#endif    


/**
 * @brief parse incoming sADM (which is another XML format)
 *
 * Note that since sADM is an XML format, we re-use definitions from
 * the PMD XML format.
 */
dlb_pmd_success                        
dlb_sadm_write
   (      dlb_xmlpmd_get_buffer gb  /**< [in] callback when writer needs more output space */
   ,      unsigned int indent       /**< [in] initial indentation level */
   ,      void *cbarg               /**< [in] client-supplied callback argument */
   ,const dlb_sadm_model *model     /**< [in] model to write */
   );


#ifdef __cplusplus
}
#endif


#endif /* DLB_SADM_WRITER_H */

