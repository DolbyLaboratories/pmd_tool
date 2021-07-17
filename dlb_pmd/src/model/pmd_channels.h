/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2019 by Dolby Laboratories,
 *                Copyright (C) 2016-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_CHANNELS_H
#define PMD_CHANNELS_H


#include <stdint.h>

#include "dlb_pmd_types.h"

/**
 * @file pmd_channels.h
 * @brief internal bit positions used to represent channels
 */

/**
 * @brief type of pmd target speaker
 *
 * PMD Channel metadata routes incoming PCM tracks (or 'signals')
 * to specific speakers.
 *
 * The pmd_speaker is the log of the above PMD channel ptrs 
 */
typedef uint8_t pmd_speaker;


/**
 * @brief channel assignment
 *
 * Need to use ITU names 
 */
#define PMD_CHANNEL_L    (1 << (PMD_SPEAKER_L-1))   /**< Left */
#define PMD_CHANNEL_R    (1 << (PMD_SPEAKER_R-1))   /**< Right */
#define PMD_CHANNEL_C    (1 << (PMD_SPEAKER_C-1))   /**< Center */
#define PMD_CHANNEL_LFE  (1 << (PMD_SPEAKER_LFE-1)) /**< Low-frequency effects */
#define PMD_CHANNEL_LS   (1 << (PMD_SPEAKER_LS-1))  /**< Left surround */
#define PMD_CHANNEL_RS   (1 << (PMD_SPEAKER_RS-1))  /**< Right surround */
#define PMD_CHANNEL_LRS  (1 << (PMD_SPEAKER_LRS-1)) /**< Left rear surround */
#define PMD_CHANNEL_RRS  (1 << (PMD_SPEAKER_RRS-1)) /**< Right rear surround */

#define PMD_CHANNEL_LTF  (1 << (PMD_SPEAKER_LTF-1)) /**< Left top front */
#define PMD_CHANNEL_RTF  (1 << (PMD_SPEAKER_RTF-1)) /**< Right top front */
#define PMD_CHANNEL_LTM  (1 << (PMD_SPEAKER_LTM-1)) /**< Left top middle */
#define PMD_CHANNEL_RTM  (1 << (PMD_SPEAKER_RTM-1)) /**< Right top middle */
#define PMD_CHANNEL_LTR  (1 << (PMD_SPEAKER_LTR-1)) /**< Left top rear */
#define PMD_CHANNEL_RTR  (1 << (PMD_SPEAKER_RTR-1)) /**< Right top rear */

#define PMD_CHANNEL_LFW  (1 << (PMD_SPEAKER_LFW-1)) /**< Left front wide */
#define PMD_CHANNEL_RFW  (1 << (PMD_SPEAKER_RFW-1)) /**< Right front wide */


/**
 * @brief type of channel set
 *
 * A "channel set" is a group of channel signals that are
 * intended to be reproduced together.
 *
 * This is implemented as a bitmap of channels, where each bit
 * position indicates presence or absence of a particular channel
 * position.  The above set of symbolic constants indicate the
 * complete set of well-known channel positions.
 */
typedef uint16_t pmd_channel_set;


/**
 * @brief well-known channel sets
 */
enum
{
    PMD_CHANNELSET_STEREO = PMD_CHANNEL_L
                          | PMD_CHANNEL_R,
    PMD_CHANNELSET_3_0    = PMD_CHANNELSET_STEREO
                          | PMD_CHANNEL_C,
    PMD_CHANNELSET_5_1    = PMD_CHANNELSET_3_0
                          | PMD_CHANNEL_LFE
                          | PMD_CHANNEL_LS
                          | PMD_CHANNEL_RS,
    PMD_CHANNELSET_5_1_2  = PMD_CHANNELSET_5_1
                          | PMD_CHANNEL_LTM
                          | PMD_CHANNEL_RTM,
    PMD_CHANNELSET_5_1_4  = PMD_CHANNELSET_5_1
                          | PMD_CHANNEL_LTF
                          | PMD_CHANNEL_RTF
                          | PMD_CHANNEL_LTR
                          | PMD_CHANNEL_RTR,
    PMD_CHANNELSET_5_1_6  = PMD_CHANNELSET_5_1
                          | PMD_CHANNEL_LTF
                          | PMD_CHANNEL_RTF
                          | PMD_CHANNEL_LTM
                          | PMD_CHANNEL_RTM
                          | PMD_CHANNEL_LTR
                          | PMD_CHANNEL_RTR,
    PMD_CHANNELSET_7_1    = PMD_CHANNELSET_5_1
                          | PMD_CHANNEL_LRS
                          | PMD_CHANNEL_RRS,
    PMD_CHANNELSET_7_1_2  = PMD_CHANNELSET_7_1
                          | PMD_CHANNEL_LTM
                          | PMD_CHANNEL_RTM,
    PMD_CHANNELSET_7_1_4  = PMD_CHANNELSET_7_1
                          | PMD_CHANNEL_LTF
                          | PMD_CHANNEL_RTF
                          | PMD_CHANNEL_LTR
                          | PMD_CHANNEL_RTR,
    PMD_CHANNELSET_7_1_6  = PMD_CHANNELSET_7_1
                          | PMD_CHANNEL_LTF
                          | PMD_CHANNEL_RTF
                          | PMD_CHANNEL_LTM
                          | PMD_CHANNEL_RTM
                          | PMD_CHANNEL_LTR
                          | PMD_CHANNEL_RTR,
    PMD_CHANNELSET_9_1    = PMD_CHANNELSET_7_1
                          | PMD_CHANNEL_LFW
                          | PMD_CHANNEL_RFW,
    PMD_CHANNELSET_9_1_6  = PMD_CHANNELSET_9_1
                          | PMD_CHANNEL_LTF
                          | PMD_CHANNEL_RTF
                          | PMD_CHANNEL_LTM
                          | PMD_CHANNEL_RTM
                          | PMD_CHANNEL_LTR
                          | PMD_CHANNEL_RTR,
};



#endif /* PMD_CHANNELS_H */
