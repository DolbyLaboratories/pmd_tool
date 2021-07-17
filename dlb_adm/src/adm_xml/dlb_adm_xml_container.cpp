/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "dlb_adm_xml_container.h"

using namespace DlbAdm;

dlb_adm_xml_container::dlb_adm_xml_container(const dlb_adm_container_counts *counts)
    : mXMLContainer()
{
    (void)counts;
}

dlb_adm_xml_container::~dlb_adm_xml_container()
{
    // Empty
}

DlbAdm::XMLContainer & dlb_adm_xml_container::GetContainer()
{
    return mXMLContainer;
}

const DlbAdm::XMLContainer &dlb_adm_xml_container::GetContainer() const
{
    return mXMLContainer;
}
