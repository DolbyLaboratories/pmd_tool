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

#ifndef PMD_STUDIO_AUDIO_BEDS_H_
#define PMD_STUDIO_AUDIO_BEDS_H_


#include "ui.h"
#include "dlb_pmd_api.h"
#include "pmd_studio_limits.h"
#include "pmd_studio.h"

#include <stdio.h>

#define MAX_BED_SOURCES (16)

typedef struct
{
    pmd_studio    *studio;
    dlb_pmd_bool   enabled;
    dlb_pmd_bed    bed;
    dlb_pmd_source sources[MAX_BED_SOURCES];

    uiCombobox *cfg;
    uiEntry *name;
    uiCombobox *start;
    uiCheckbox *enable;

} pmd_studio_audio_bed;


typedef struct
{
    uiWindow *window;
    pmd_studio_audio_bed beds[MAX_AUDIO_BEDS];
    unsigned int bed_count;
    uiGrid *grid;
} pmd_studio_audio_beds;
    
uiGrid *grid_bed;
static pmd_studio_audio_beds *ab;

static
void
onEnableBed
    (uiCheckbox *en
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    abed->enabled = (dlb_pmd_bool)uiCheckboxChecked(en);
    pmd_studio_update_model(abed->studio);
}


static
void
onBedConfigUpdated
    (uiCombobox *c
    ,void *data
    )
{
    static uint8_t SPEAKER_COUNTS[NUM_PMD_SPEAKER_CONFIGS] =  {2, 3, 6, 8, 10, 12, 16, 2, 2};
    static unsigned int SPEAKER_CONFIG_CHANNELS[NUM_PMD_SPEAKER_CONFIGS][16] =
    {
        /* 2.0 */     { 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 3.0 */     { 1,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 5.1 */     { 1,  2,  3,  4,  5,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 5.1.2 */   { 1,  2,  3,  4,  5,  6, 11, 12,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* 5.1.4 */   { 1,  2,  3,  4,  5,  6,  9, 10, 13, 14,  0,  0,  0,  0,  0,  0 },
        /* 7.1.4 */   { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 13, 14,  0,  0,  0,  0 },
        /* 9.1.6 */   { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16 },
        /* portable */{ 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        /* headphn */ { 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    };

    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    unsigned int i;
    int idx = uiComboboxSelected(c);
    
    abed->bed.config = DLB_PMD_SPEAKER_CONFIG_2_0;
    if (idx > 0)
    {
        abed->bed.config = (dlb_pmd_speaker_config)idx;
        abed->bed.num_sources = SPEAKER_COUNTS[abed->bed.config];
        /* todo: check that 1st channel + num sources fits into max signal range */
        for (i = 0; i != abed->bed.num_sources; ++i)
        {
            abed->bed.sources[i].target = SPEAKER_CONFIG_CHANNELS[abed->bed.config][i];
            abed->bed.sources[i].source = (dlb_pmd_signal)(abed->bed.sources[0].source + i);
            abed->bed.sources[i].gain = 0.0f;
        }

        pmd_studio_update_model(abed->studio);
    }
}


static
void
onBedStartUpdated
    (uiCombobox *c
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    unsigned int i;

    abed->bed.sources[0].source = (dlb_pmd_signal)(1 + uiComboboxSelected(c));
    for (i = 1; i != abed->bed.num_sources; ++i)
    {
        abed->bed.sources[i].source = (dlb_pmd_signal)(abed->bed.sources[0].source + i);
    }

    pmd_studio_update_model(abed->studio);
}


static
void
onBedNameUpdated
    (uiEntry *e
    ,void *data
    )
{
    pmd_studio_audio_bed *abed = (pmd_studio_audio_bed *)data;
    snprintf(abed->bed.name, sizeof(abed->bed.name), "%s", uiEntryText(e));
    pmd_studio_update_model(abed->studio);
}


static
void
pmd_studio_audio_bed_init
    (pmd_studio_audio_bed *abed
    )
{
    abed->enabled         = 0;
    abed->bed.config      = DLB_PMD_SPEAKER_CONFIG_2_0;
    abed->bed.bed_type    = PMD_BED_ORIGINAL;
    abed->bed.source_id   = 0;
    abed->bed.num_sources = 2;
    abed->bed.sources     = abed->sources;
    abed->bed.sources[0].target = PMD_SPEAKER_L;
    abed->bed.sources[0].source = 1;
    abed->bed.sources[0].gain   = 0.0f;
    abed->bed.sources[1].target = PMD_SPEAKER_R;
    abed->bed.sources[1].source = 2;
    abed->bed.sources[1].gain   = 0.0f;
    snprintf((char*)abed->bed.name, sizeof(abed->bed.name), "Bed %u", abed->bed.id);
}


static inline
dlb_pmd_success
add_audio_bed
    (uiGrid *gbed
    ,pmd_studio_audio_bed *abed
    ,unsigned int top
    ,pmd_studio *s
    )
{
    char tmp[32];
    unsigned int i;

    abed->studio          = s;
    abed->bed.id          = (dlb_pmd_element_id)top;
    pmd_studio_audio_bed_init(abed);
    
    snprintf(tmp, sizeof(tmp), "%d", top);
    uiGridAppend(gbed, uiControl(uiNewLabel(tmp)), 0, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->enable = uiNewCheckbox("");
    uiCheckboxOnToggled(abed->enable, onEnableBed, abed);
    uiGridAppend(gbed, uiControl(abed->enable), 1, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->cfg = uiNewCombobox();
    uiComboboxAppend(abed->cfg, "2.0");
    uiComboboxAppend(abed->cfg, "3.0");
    uiComboboxAppend(abed->cfg, "5.1");
    uiComboboxAppend(abed->cfg, "5.1.2");
    uiComboboxAppend(abed->cfg, "5.1.4");
    uiComboboxAppend(abed->cfg, "7.1.4");    
    uiComboboxAppend(abed->cfg, "9.1.4");
    uiComboboxSetSelected(abed->cfg, (int)abed->bed.config); 
    uiComboboxOnSelected(abed->cfg, onBedConfigUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->cfg), 2, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    abed->start = uiNewCombobox();    
    for (i = 1; i != MAX_AUDIO_SIGNALS; ++i)
    {
        snprintf(tmp, sizeof(tmp), "%d", i);
        uiComboboxAppend(abed->start, tmp);
    }
    uiComboboxSetSelected(abed->start, 0); 
    uiComboboxOnSelected(abed->start, onBedStartUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->start), 3, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

    uiGridAppend(gbed, uiControl(uiNewLabel("  ")), 4, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
    abed->name = uiNewEntry();
    uiEntrySetText(abed->name, (const char*)abed->bed.name);
    uiEntryOnChanged(abed->name, onBedNameUpdated, abed);
    uiGridAppend(gbed, uiControl(abed->name), 5, top, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
    return PMD_SUCCESS;
}

static inline
void
add_grid_title
    (uiGrid *gbed
    ,const char *txt
    ,int left
    ,int top
    )
{
    uiGridAppend(gbed, uiControl(uiNewLabel(txt)), left, top, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
}


static inline
dlb_pmd_success
pmd_studio_audio_beds_init
    (pmd_studio_audio_beds *ab1
    ,uiWindow *win
    ,uiBox *box
    ,pmd_studio *s
    )
{
    unsigned int i;
    uiBox *vbox;
    
    ab = ab1;
    ab->window = win;
    
    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);
    
    uiBoxAppend(vbox, uiControl(uiNewLabel("Audio Beds")), 0);
    uiBoxAppend(vbox, uiControl(uiNewHorizontalSeparator()), 0);

    grid_bed = uiNewGrid();
    uiGridSetPadded(grid_bed, 1);
    uiBoxAppend(vbox, uiControl(grid_bed), 0);

    add_grid_title(grid_bed, "En",     1, 0);
    add_grid_title(grid_bed, "Config", 2, 0);
    add_grid_title(grid_bed, "Start",  3, 0);
    add_grid_title(grid_bed, "End",    4, 0);    
    add_grid_title(grid_bed, "Name",   5, 0);
    
    for (i = 0; i != MAX_AUDIO_BEDS; ++i)
    {
        if (add_audio_bed(grid_bed, &ab->beds[i], i+1, s))
        {
            return PMD_FAIL;
        }
    }

    uiBoxAppend(box, uiControl(vbox), 0);
    uiBoxAppend(vbox, uiControl(uiNewLabel("")), 0);
    return PMD_SUCCESS;
}


static
void
pmd_studio_audio_beds_refresh_ui
    (pmd_studio_audio_beds *abeds
    )
{
    pmd_studio_audio_bed *abed;
    unsigned int i;
    abed = abeds->beds;
    for (i = 0; i != MAX_AUDIO_BEDS; ++i, ++abed)
    {
        uiCheckboxSetChecked (abed->enable,  abed->enabled);
        uiComboboxSetSelected(abed->cfg,     (int)abed->bed.config);
        uiComboboxSetSelected(abed->start,   abed->bed.sources[0].source - 1);
        uiEntrySetText(abed->name, abed->bed.name);
    }
}


static
dlb_pmd_success
pmd_studio_audio_beds_import
    (pmd_studio_audio_beds *abeds
    ,dlb_pmd_model *m
    )
{
    pmd_studio_audio_bed *abed;
    dlb_pmd_bed_iterator bi;

    if (dlb_pmd_bed_iterator_init(&bi, m))
    {
        return PMD_FAIL;        
    }

    abed = abeds->beds;
    while (abed < &abeds->beds[MAX_AUDIO_BEDS])
    {
        if (dlb_pmd_bed_iterator_next(&bi, &abed->bed, MAX_BED_SOURCES, abed->sources))
        {
            break;
        }
        
        abed->enabled = 1;
        ++abed;
    }
    return PMD_SUCCESS;
}


static
void
pmd_studio_audio_beds_reset
    (pmd_studio_audio_beds *abeds
    )
{
    pmd_studio_audio_bed *abed;
    unsigned int i;
    
    abed = abeds->beds;
    for (i = 0; i != INIT_AUDIO_BEDS; ++i, ++abed)
    {
        pmd_studio_audio_bed_init(abed);
    }
}




static inline
void
pmd_studio_audio_beds_finish
    (pmd_studio_audio_beds *abeds
    )
{
    (void)abeds;
}


#endif /* PMD_STUDIO_AUDIO_BEDS_H_ */
