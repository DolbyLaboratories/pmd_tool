/******************************************************************************  
 * This program is protected under international and U.S. copyright laws as  
 * an unpublished work. This program is confidential and proprietary to the  
 * copyright owners. Reproduction or disclosure, in whole or in part, or the  
 * production of derivative works therefrom without the express permission of  
 * the copyright owners is prohibited.  
 *  
 *                  Copyright (C) 2014 by Dolby Laboratories.  
 *                            All rights reserved.  
 ******************************************************************************/

#include "dlb_wave_tests.h"
#include "munit/src/collator/defaults.h"

int main(int argc, char *argv[])
{
    return munit_main(argc, argv, &dlb_wave_test_dir, munit_default_collators);
}

