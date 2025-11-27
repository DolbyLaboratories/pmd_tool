/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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


#ifndef EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_BEDS_PVT_H
#define EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_BEDS_PVT_H

extern "C"{
    #include "dlb_pmd_api.h"
    #include "ui.h"
}
#include "pmd_studio_audio_beds.h"
#include "pmd_studio_common_defs.h"

#define MAX_BED_SOURCES (16)
/* Definitions */



/**
 * BedClassifiers
 * As bed classifiers arent supported in PMD, they are implemented as unique
 * name suffixes. The only exception is "DEFAULT", which is equivalent to no
 * suffix.
*/

enum class BedClassifier
{
    DEFAULT,
    COMPLETE_MAIN,
    MUSIC_AND_EFFECTS,
    BROADCAST_MIX,
    MUSIC_AND_EFFECT_LEGACY,
    LAST
};

// BedClassifier suffix and description definitions. 
// Vector of [classifier, suffix, description] tuples
const std::vector<std::tuple<const BedClassifier, const std::string, const std::string>> BED_CLASSIFIER_TAG_MAP {
//  CLASSIFIER                          SUFFIX      DESCRIPTION
    {BedClassifier::COMPLETE_MAIN,           "$[CM]",    "Complete Main"},
    {BedClassifier::MUSIC_AND_EFFECTS,       "$[ME]",    "Music & Effects"},
    {BedClassifier::BROADCAST_MIX,           "$[BM]",    "Broadcast Mix"},
    {BedClassifier::MUSIC_AND_EFFECT_LEGACY, "$[ML]",    "Music & Effects (legacy)"}
};

const std::string CARTESIAN_TAG = "$[C]";

// Scans for and extracts bed classifier strings from label. 
// Returns [classifier, prefix] (where prefix is the remainder of the input post-extraction)
std::tuple<BedClassifier, std::string, int> 
parsePMDBedLabel
    (std::string label
    );

// Generates PMD label - combines label with unique bed classifier suffix
std::string 
generatePMDBedLabel
    (std::string   label
    ,BedClassifier bedClass
    ,int           is_cartesian
    );

const float config_mix_coefs[NUM_PMD_SPEAKER_CONFIGS] = 
{
	1.0, 1.0
};

typedef struct
{
    pmd_studio_audio_beds *audio_beds;

    dlb_pmd_bool   enabled;
    dlb_pmd_bed    bed;
    dlb_pmd_source sources[MAX_BED_SOURCES];
    uiLabel *label;
    uiCombobox *cfg;
    uiEntry *name;
    uiCombobox *gain;
    uiCombobox *start;
    uiCombobox *classifier;
    uiCheckbox *enable;
    uiCheckbox *cartesian;

} pmd_studio_audio_bed;


struct pmd_studio_audio_beds
{
	pmd_studio *studio;

    uiWindow *window;
    pmd_studio_audio_bed beds[MAX_AUDIO_BEDS];
    uiButton *add_bed_button;
    unsigned int bed_count;
    uiGrid *grid;
    dlb_pmd_element_id bed_eids[MAX_AUDIO_BEDS];
    unsigned int bed_labels[MAX_AUDIO_BEDS];
};
    

#endif //EMBERPLUS_MXBOLT_PMD_STUDIO_AUDIO_BEDS_PVT_H
