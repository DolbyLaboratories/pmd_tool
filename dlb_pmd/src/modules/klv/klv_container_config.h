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

/**
 * @file klv_container_config.h
 * @brief defines reading and writing mandatory container config local tag
 */

#ifndef KLV_CONTAINER_CONFIG_INC_H_
#define KLV_CONTAINER_CONFIG_INC_H_

#include "klv_reader.h"


#define KLV_UNIVERSAL_LABEL_SIZE (16)


/**
 * @brief write mandatory container config local tag
 *
 * We never write dynamic tag assignments, so this payload is always
 * one byte long
 */
static inline
int                          /** @return 0 on success, 1 on failure */
klv_container_config_write
    (klv_writer *w
    )
{
    pmd_smpte2109 *smpte2109 = &w->model->smpte2109;
    pmd_dynamic_tag *dtag = smpte2109->dynamic_tags;
    unsigned int count = smpte2109->num_dynamic_tags;
    unsigned int space = 2;
    
    while (count)
    {
        if (dtag->local_tag < 128)       space += 1; else
        if (dtag->local_tag < 0x100)     space += 2; else
#ifdef todo
        if (dtag->local_tag < 0x10000)   space += 3; else
        if (dtag->local_tag < 0x1000000) space += 4; else
#endif
        abort();

        space += KLV_UNIVERSAL_LABEL_SIZE;
        ++dtag;
        --count;
    }

    dtag = smpte2109->dynamic_tags;
    count = smpte2109->num_dynamic_tags;

    if (klv_writer_space(w) >= space)
    {
        *w->wp++ = (smpte2109->sample_offset >> 8) & 0xff;
        *w->wp++ = smpte2109->sample_offset & 0xff;
        while (count)
        {
            w->wp += klv_write_ber(w->wp, dtag->local_tag);
            memcpy(w->wp, dtag->universal_label, KLV_UNIVERSAL_LABEL_SIZE);
            ++dtag;
            w->wp += KLV_UNIVERSAL_LABEL_SIZE;
            --count;
        }
        return 0;
    }
    return 1;
}


/**
 * @brief read a container config
 */
static inline
int                              /** @return 0 on success, 1 on error */
klv_container_config_read
    (klv_reader *r               /**< [in] KLV buffer to read */
    ,int payload_length          /**< [in] bytes in presentation payload */
    )
{
    pmd_smpte2109 *smpte2109 = &r->model->smpte2109;
    pmd_dynamic_tag *dtag = smpte2109->dynamic_tags;
    int count;

    if (!global_testing_version_numbers)
    {
        if (*r->rp != KLV_CONTAINER_CONFIG_VERSION)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, NULL, "Wrong container config version.  Found %u, expected %u\n",
                                *r->rp, KLV_CONTAINER_CONFIG_VERSION);
            return 1;
        }
    }
    smpte2109->sample_offset = (r->rp[0] << 8) | r->rp[1];
    payload_length -= 2;
    r->rp += 2;
    
    count = 0;
    while (payload_length > KLV_UNIVERSAL_LABEL_SIZE)
    {
        unsigned int taglen;
        unsigned int localtag;
        if (klv_read_ber_value(r, &localtag, &taglen, NULL))
        {
            return 1;
        }
        dtag->local_tag = (uint16_t)localtag;
        memcpy(dtag->universal_label, r->rp, KLV_UNIVERSAL_LABEL_SIZE);
        r->rp += KLV_UNIVERSAL_LABEL_SIZE;
        ++count;
        ++dtag;

        if (count > PMD_MAX_DYNAMIC_TAGS)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, NULL, "Too many dynamic tags (%d), only %d supported\n",
                                count, PMD_MAX_DYNAMIC_TAGS);
            return 1;
        }
        payload_length -= taglen + KLV_UNIVERSAL_LABEL_SIZE;
    }
    smpte2109->num_dynamic_tags = count;
    return payload_length;      /* fail if any payload unused */
}


#endif /* KLV_CONTAINER_CONFIG_INC_H_ */

