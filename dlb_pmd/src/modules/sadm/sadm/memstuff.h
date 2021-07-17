/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019 by Dolby Laboratories,
 *                Copyright (C) 2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @def ALIGN_TO(size,A)
 * @brief align size to next multiple of A
 */
#define ALIGN_TO(size,A) ((((size) + (A)-1)/(A))*(A))


/**
 * @def ALIGN_TO_MPTR(size)
 * @brief align a size to be a multiple of void*
 */
#define ALIGN_TO_MPTR(size) ALIGN_TO(size, sizeof(void*))

#define MEMREQ(ty, count) ALIGN_TO_MPTR(sizeof(ty) * count)
