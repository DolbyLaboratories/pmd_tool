/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2019 by Dolby Laboratories,
 *                Copyright (C) 2017-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/
#ifndef PMD_TYPES_H
#define PMD_TYPES_H

#include "dlb_pmd_types.h"

/* Maximum size of ulong attribute */
#define MAX_ULONG_LENGTH 16
/* Maximum length of a text value in XML content */
#define XML_MAX_TEXT_VALUE_LENGTH 2048
/* Maximum length of a special xml character entities */
#define XML_SPEC_MAX_LENGTH 6
/* Length of a standard UUID string */
#define UUID_STRING_LENGTH 36


/* backwards compatibility with previous versions */
typedef dlb_pmd_bool pmd_bool;


#endif /* PMD_TYPES_H */
