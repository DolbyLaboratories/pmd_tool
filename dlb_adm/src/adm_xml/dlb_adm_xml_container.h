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

#ifndef DLB_ADM_XML_CONTAINER_H
#define DLB_ADM_XML_CONTAINER_H

#include "XMLContainer.h"

struct dlb_adm_xml_container : public boost::noncopyable
{
public:
    explicit dlb_adm_xml_container(const dlb_adm_container_counts *counts);
    ~dlb_adm_xml_container();

    DlbAdm::XMLContainer &GetContainer();

    const DlbAdm::XMLContainer &GetContainer() const;

private:
    DlbAdm::XMLContainer mXMLContainer;
};

#endif /* DLB_ADM_XML_CONTAINER_H */
