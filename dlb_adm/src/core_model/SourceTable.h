/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_SOURCE_TABLE_H
#define DLB_ADM_SOURCE_TABLE_H

#include "SourceRecord.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    struct SourceTable_PK {};
    struct SourceTable_AudioTrack {};

    typedef multi_index_container <
        SourceRecord,
        indexed_by <
            // Primary key (PK)
            ordered_unique<tag<SourceTable_PK>, identity<SourceRecord> >,
            // AudioTrack
            ordered_unique<tag<SourceTable_AudioTrack>, member<SourceRecord, dlb_adm_entity_id, &SourceRecord::audioTrackID> >
        >
    > SourceTable;

    typedef SourceTable::index<SourceTable_PK>::type SourceTable_PKIndex;
    typedef SourceTable::index<SourceTable_AudioTrack>::type SourceTable_AudioTrackIndex;

}

#endif  // DLB_ADM_SOURCE_TABLE_H
