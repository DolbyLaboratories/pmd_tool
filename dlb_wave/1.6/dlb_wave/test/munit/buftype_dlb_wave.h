/******************************************************************************  
 * This program is protected under international and U.S. copyright laws as  
 * an unpublished work. This program is confidential and proprietary to the  
 * copyright owners. Reproduction or disclosure, in whole or in part, or the  
 * production of derivative works therefrom without the express permission of  
 * the copyright owners is prohibited.  
 *  
 *                  Copyright (C) 2015 by Dolby Laboratories.  
 *                            All rights reserved.  
 ******************************************************************************/

#ifndef BUFTYPE_DLB_WAVE_H
#define BUFTYPE_DLB_WAVE_H

#include "dlb_wave/include/dlb_wave.h"
#include "buftype.h"

const char *get_dlb_wave_buffer_type(buffer_type *bt, const dlb_wave_format *p_fmt, unsigned format_flags);

#endif /* BUFTYPE_DLB_WAVE_H */
