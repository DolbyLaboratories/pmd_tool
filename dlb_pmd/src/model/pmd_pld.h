/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2019 by Dolby Laboratories,
 *                Copyright (C) 2018-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/


#ifndef PMD_PLD_H
#define PMD_PLD_H

/**
 * @file pmd_pld.h
 * @brief how the PMD model represents Presentation Loudness Descriptions (PLD)
 */

#include <math.h>
#include "pmd_apd.h"


/**
 * @brief bitmap of loudness optional information
 */
typedef unsigned long pmd_loudness_options;

#define PMD_PLD_OPT_LOUDCORR_DIALGATE   (0x0001)
#define PMD_PLD_OPT_LOUDCORR_TYPE       (0x0002)
#define PMD_PLD_OPT_LOUDRELGAT          (0x0004)
#define PMD_PLD_OPT_LOUDSPCHGAT         (0x0008)
#define PMD_PLD_OPT_LOUDSTRM3S          (0x0010)
#define PMD_PLD_OPT_MAX_LOUDSTRM3S      (0x0020)
#define PMD_PLD_OPT_TRUEPK              (0x0040)
#define PMD_PLD_OPT_MAX_TRUEPK          (0x0080)
#define PMD_PLD_OPT_PRGMBNDY            (0x0100)
#define PMD_PLD_OPT_PRGMBNDY_OFFSET     (0x0200)
#define PMD_PLD_OPT_LRA                 (0x0400)
#define PMD_PLD_OPT_LOUDMNTRY           (0x0800)
#define PMD_PLD_OPT_MAX_LOUDMNTRY       (0x1000)
#define PMD_PLD_OPT_EXTENSION           (0x2000)


/**
 * @brief enumerate known loudness practice types
 */
typedef enum
{
    PMD_LPT_NI                  = 0,  /**< Loudness regulation compliance not indicated */
    PMD_LPT_ATSC                = 1,  /**< Loudness according to ATSC A/85 */
    PMD_LPT_EBU                 = 2,  /**< Loudness according to EBU R126 */
    PMD_LPT_ARIB                = 3,  /**< Loudness according to ARIB TR-B32 */
    PMD_LPT_FREETV              = 4,  /**< Loudness according to Free TV OP-59 */
    PMD_LPT_MANUAL              = 14, /**< Manual */
    PMD_LPT_CONSUMER_LEVELLER   = 15, /**< consumer leveller */
} pmd_loudness_practice;


/**
 * @brief validate a loudness practice value
 */
static inline
dlb_pmd_payload_status              /** @return validation status */
validate_pmd_loudness_practice
    (pmd_loudness_practice practice /**< [in] PMD loudness practice value */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (practice > PMD_LPT_CONSUMER_LEVELLER)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (practice > PMD_LPT_FREETV && practice < PMD_LPT_MANUAL)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


/**
 * @brief enumerate known dialog gating practice methods
 */
typedef enum
{
    PMD_DGPT_NI,      /**< dialog gating method not indicated */
    PMD_DGPT_CENTER,  /**< dialog gating applied to center channel, or LR in stereo */
    PMD_DGPT_FRONT,   /**< dialog gating applied to all front (main) channels */
    PMD_DGPT_MANUAL   /**< dialog gating method - manual */
} pmd_dialgate_practice;


/**
 * @brief loudness correction type
 */
typedef enum
{
    PMD_CORRTY_FILE_BASED,  /**< corrected with infinite lookahead (file-based) */
    PMD_CORRTY_REALTIME     /**< corrected with finite lookahead (realtime) */
} pmd_correction_type;


/**
 * @brief loudness value
 *
 * This type is used to record 5 different loudness measurements
 * (in LUFS): relative-gated, speech-gated, short-term-3s, true-peak
 * and momentary loudness.
 *
 * - Relative-Gated measurements determine the integrated loudness of an
 * audio programme, measured according to Recommendation ITU-R
 * BS.1770, without any gain adjustments due to dialnorm or DRC.
 *
 * - Speech-Gated measurements determine the integrated dialog-based
 * loudness of the entire audio programme, measured according to
 * formula (2) of Recommendation ITU-R BS.1770 with dialog-gating.
 * This represents the dialog-based loudness without gain adjustments
 * due to dialnorm or DRC.
 *
 * - Short-Term-3s measurements determine the loudness of the
 * preceding 3 seconds of the audio programme, measured according to
 * ITU-R BS.1771, without gain adjustments due to dialnorm or DRC.
 * 
 * - True peak is measured according ot Annex 2 of Recommendation
 *   ITU-R BS.1770.
 *
 * - momentary loudness measurements, measured according to 
 * Recommendation ITU-R BS.1771, without any gain adjustments due to
 * dialnorm or DRC.
 */
typedef unsigned int pmd_loudness_value;


/**
 * @brief encode LUFS
 */
static inline
pmd_loudness_value
pmd_encode_lufs
    (float lufs
    )
{
    return 1024 + (unsigned int)floorf(lufs * 10.0f + 0.5f);
}


/**
 * @brief convert encoded LUFS to LUFS value
 */
static inline
float
pmd_decode_lufs
    (pmd_loudness_value v
    )
{
    return ((float)v - 1024) / 10.0f;
}


/**
 * @brief true peak sample value
 *
 * indicates the true peak sample value of the programme measured
 * since the previous value was sent in the bitstream.
 *
 * This is encoded exactly the same way as loudness measurements 
 */
typedef signed int pmd_true_peak;


/**
 * @brief encode TruePeak (dBTP)
 */
static inline
pmd_loudness_value      /** @return encoded loudness value */
pmd_encode_truepk
    (float lufs         /**< [in] loudness measurement in LUFS */
    )
{
    return 1024 + (unsigned int)floor(lufs * 10.0f + 0.5f);
}


/**
 * @brief convert encoded truepeak to dBTP value
 */
static inline
float                         /** @return loudness measurement in LUFS */
pmd_decode_truepk
    (pmd_loudness_value v     /**< [in] encoded measurement */
    )
{
    return ((float)v - 1024) / 10.0f;
}


/**
 * @brief program boundary
 *
 * Indicates the number of frames between the current frame and
 * the next frame that contains the boundary between two different
 * audio programmes.  This data may be used to determine when
 * to begin and end the measurement of the loudness parameters specified
 * in the payload.   This value is restricted to the range +/-[2,512]
 * (i.e., including both 2 and 512).
 *
 * If negative, this indicates number of frames since previous program boundary.
 * If positive, this indicates number of frames until next program boundary.
 */
typedef signed short pmd_programme_boundary;


/**
 * Valid program boundary exponent values
 */
#define PMD_PROGRAMME_BOUNDARY_NEG_MIN (-9)
#define PMD_PROGRAMME_BOUNDARY_NEG_MAX (-1)
#define PMD_PROGRAMME_BOUNDARY_POS_MIN (1)
#define PMD_PROGRAMME_BOUNDARY_POS_MAX (9)


/**
 * @brief validate a programme boundary value
 */
static inline
dlb_pmd_payload_status          /** @return validation status */
validate_pmd_programme_boundary
(pmd_programme_boundary bndry   /**< [in] PMD loudness practice value */
)
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;

    if ((bndry >= PMD_PROGRAMME_BOUNDARY_NEG_MIN && bndry <= PMD_PROGRAMME_BOUNDARY_NEG_MAX) ||
        (bndry >= PMD_PROGRAMME_BOUNDARY_POS_MIN && bndry <= PMD_PROGRAMME_BOUNDARY_POS_MAX))
    {
        status = DLB_PMD_PAYLOAD_STATUS_OK;
    }

    return status;
}


/**
 * @brief offset in samples to the next boundary
 */
typedef unsigned int pmd_programme_boundary_offset;


/**
 * @brief loudness range
 *
 * indicates the loudness range of the programme, as specified in
 * EBU Tech Document 3342.
 *
 * This takes values 0 - 1023
 */
typedef unsigned int pmd_loudness_range;


/**
 * @brief encode a loudness range value
 */
static inline
pmd_loudness_range   /** @return encoded loudness range */
pmd_encode_lra
    (float lu        /**< [in] loudness range in LU */
    )
{
    return (pmd_loudness_range)floorf((lu * 10.0f) + 0.5f);
}


/**
 * @brief recover loudness range from encoded value
 */
static inline
float                          /** @return loudness range in LU */
pmd_decode_lra
    (pmd_loudness_range lra    /**< [in] encoded loudness range */
    )
{
    return (float)lra / 10.0f;
}


/**
 * @brief indicates method used to compute loudness range
 */
typedef enum
{
    PMD_LRP1,  /**< Loudness Range as per EBU Tech 3342 v1 */
    PMD_LRP2   /**< Loudness Range as per EBU Tech 3342 v2 */
} pmd_loudness_range_practice;


/**
 * @brief validate a loudness range practice value
 */
static inline
dlb_pmd_payload_status          /** @return validation status */
validate_pmd_loudness_range_practice
(pmd_loudness_range_practice practice   /**< [in] PMD loudness practice value */
)
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (practice > PMD_LRP2)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}
    

/**
 * @file pmd_presentation_loudness.h
 * @brief implementation of PMD lPresentation loudness data structure
 *
 *  This structure is defined in ETSI TS 103 190-1 v1.2.1, section 4.2.14.3
 */
typedef struct
{
    pmd_presentation_id  presid;            /**< presentation idx */
    pmd_loudness_options options;           /**< bitmap of various options */

    pmd_loudness_practice lpt;              /**< Loudness practice type */
    pmd_dialgate_practice dpt;              /**< Loudness correction dialog gating practice type */
    pmd_correction_type corrty;             /**< Loudness correction type: file-based or realtime */
    pmd_loudness_value lrg;                 /**< Loudness value, relative-gated */
    pmd_loudness_value lsg;                 /**< Loudness value, speech-gated */
    pmd_dialgate_practice sdpt;             /**< Dialog gating practice type for speech-gating */
    pmd_loudness_value l3g;                 /**< Loudness value, short-term 3 seconds */
    pmd_loudness_value l3g_max;             /**< Max loudness value, short-term 3 seconds */
    pmd_true_peak tpk;                      /**< true peak loudness */
    pmd_true_peak tpk_max;                  /**< maximum true peak loudness */
    pmd_programme_boundary prgmbndy;        /**< if > 0, there are 2^prgmbndy frames until
                                              *          next programme boundary
                                              *  if < 0, there have been 2^prgmbndy frames
                                              *          since previous programme boundary
                                              *  valid values: 1 - 9 (expressing ranges 2 - 512)
                                              */
    pmd_programme_boundary_offset prgmbndy_offset;
    pmd_loudness_range lra;                 /**< Loudness range */
    pmd_loudness_range_practice lrap;       /**< Loudness range practice type */
    pmd_loudness_value ml;                  /**< Loudness value, momentary loudness */
    pmd_loudness_value ml_max;              /**< Loudness value, maximum momentary loudness */
    unsigned int extension_bits;            /**< number of extension bits */
    uint8_t extension[PMD_EXTENSION_MAX_BYTES]; /**< array holding extension bits */
} pmd_pld;


#endif /* PMD_PLD_H */

