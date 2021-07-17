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

#ifndef DLB_ADM_XML_READER_STACK_H
#define DLB_ADM_XML_READER_STACK_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "EntityDescriptor.h"

#include <vector>
#include <map>

namespace DlbAdm
{

    struct XMLReaderStackEntry
    {
        typedef std::map<std::string, std::string> AttributeValueMap;

        EntityDescriptor     entityDescriptor;
        dlb_adm_entity_id    entityId;
        bool                 idFinal;
        AttributeValueMap    deferredAttributes;

        XMLReaderStackEntry();
        XMLReaderStackEntry(const XMLReaderStackEntry &x);

        XMLReaderStackEntry &operator=(const XMLReaderStackEntry &x);
    };

    class XMLReaderStack
    {
    public:

        void Push(const XMLReaderStackEntry &e);

        int Pop();

        int Top(XMLReaderStackEntry *&d);

        int Top2(XMLReaderStackEntry *&top, XMLReaderStackEntry *&nextToTop);

        int Bottom(XMLReaderStackEntry *&bottom);

        size_t Size() const;

        bool Empty() const;

    private:
        std::vector<XMLReaderStackEntry> mStack;
    };

}

#endif
