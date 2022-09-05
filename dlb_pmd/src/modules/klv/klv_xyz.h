/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
 * @file klv_xyz.h
 * @brief defines reading and writing Dynamic Update (XYZ) payloads
 * (so called because they update the X, Y and Z co-ordinates of an audio object
 * at a given time).
 */

#ifndef KLV_XYZ_H_
#define KLV_XYZ_H_

#include "pmd_xyz.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"


/*                                           bitpos    len */

#define UPDATE_SAMPLE_TIME                       0,       6
#define UPDATE_OBJ_ID(i)                  (i*42+6),      12
#define UPDATE_XPOS(i)                   (i*42+18),      10
#define UPDATE_YPOS(i)                   (i*42+28),      10
#define UPDATE_ZPOS(i)                   (i*42+38),      10

#define UPDATE_PAYLOAD_SIZE(num_objs) (num_objs ? ((6 + 42*(num_objs) + 7)/8) : 0)

#define COUNT_UPDATES(num_bits) (((num_bits) - 6) / 42)

/**
 * @brief write an audio object update to the KLV output stream
 *
 * Note that this writes *all* objects at the given update time
 */
static inline
int
klv_xyz_write
    (klv_writer *w
    ,unsigned int time         /**< [in] starting time from vsync in 32-sample blocks */
    )
{
    if (klv_write_local_key_opened(w))
    {
        dlb_pmd_model *model = w->model;
        pmd_xyz *update = model->xyz_list;
        pmd_element *elements = model->element_list;
        uint8_t *wp = w->wp;

        unsigned int space_for_updates = COUNT_UPDATES(klv_writer_space(w)*8);
        unsigned int i = 0;
        unsigned int written = 0;

        memset(wp, '\0', klv_writer_space(w));

        set_(wp, UPDATE_SAMPLE_TIME, time);
        while (i < model->num_xyz && space_for_updates)
        {
            if (   (!pmd_xyz_set_test(&model->write_state.xyz_written, i))
                && (update->time == time))
            {
                uint16_t obj_id = elements[update->obj_idx].id;
                
                TRACE(("        XYZ: %u\n", obj_id));                
                set_(wp, UPDATE_OBJ_ID(written),   obj_id    );
                set_(wp, UPDATE_XPOS(written),     update->x);
                set_(wp, UPDATE_YPOS(written),     update->y);
                set_(wp, UPDATE_ZPOS(written),     update->z);
                pmd_xyz_set_add(&model->write_state.xyz_written, i);
                --space_for_updates;
                ++written;
            }
            ++update;
            ++i;
        }
        w->wp += UPDATE_PAYLOAD_SIZE(written);
    }
    return 0;
}


/**
 * @brief extract an audio object update from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_xyz_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    uint8_t *rp = r->rp;
    uint16_t obj_id;
    uint16_t idx;
    pmd_xyz update;
    int total = COUNT_UPDATES(payload_length*8);
    int count = 0;

    update.time = (unsigned int)get_(rp, UPDATE_SAMPLE_TIME);   /* No validation necessary */
    while (count < total)
    {
        dlb_pmd_payload_status ps;
        
        obj_id      = (uint16_t)get_(rp, UPDATE_OBJ_ID(count));
        update.x    = (pmd_position)get_(rp, UPDATE_XPOS(count));
        update.y    = (pmd_position)get_(rp, UPDATE_YPOS(count));
        update.z    = (pmd_position)get_(rp, UPDATE_ZPOS(count));

        TRACE(("        XYZ: %u\n", obj_id));
        ps = pmd_validate_audio_element_id(obj_id);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status,
                                "Invalid audio element id %u for object referenced in XYZ payload\n",
                                (unsigned int)obj_id);
            return 1;
        }
        if (!pmd_idmap_lookup(r->element_ids, obj_id, &idx))
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, read_status,
                                "XYZ payload refers to unknown element %u\n",
                                (unsigned int)obj_id);
            return 1;
        }
        update.obj_idx = idx;

        ps = pmd_validate_encoded_position(update.x);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Invalid encoded x position value %u for audio object %u update\n",
                                (unsigned int)update.x, (unsigned int)obj_id);
            return 1;
        }

        ps = pmd_validate_encoded_position(update.y);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Invalid encoded y position value %u for audio object %u update\n",
                                (unsigned int)update.y, (unsigned int)obj_id);
            return 1;
        }

        ps = pmd_validate_encoded_position(update.z);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Invalid encoded z position value %u for audio object %u update\n",
                                (unsigned int)update.z, (unsigned int)obj_id);
            return 1;
        }

        if (pmd_model_add_update(r->model, &update))
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY, read_status, "could not add XYZ payload to model\n");
            return 1;
        }
        ++count;
    }
    
    r->rp = rp + UPDATE_PAYLOAD_SIZE(count);
    return 0;
}


#endif /* KLV_XYZ_H_ */
