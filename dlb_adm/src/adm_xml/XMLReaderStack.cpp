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

#include "XMLReaderStack.h"

namespace DlbAdm
{

    XMLReaderStackEntry::XMLReaderStackEntry()
        : entityDescriptor(nullEntityDescriptor)
        , entityId(DLB_ADM_NULL_ENTITY_ID)
        , idFinal(false)
        , deferredAttributes()
    {
        // Empty
    }

    XMLReaderStackEntry::XMLReaderStackEntry(const XMLReaderStackEntry &x)
        : entityDescriptor(x.entityDescriptor)
        , entityId(x.entityId)
        , idFinal(x.idFinal)
        , deferredAttributes(x.deferredAttributes)
    {
        // Empty
    }

    XMLReaderStackEntry &XMLReaderStackEntry::operator=(const XMLReaderStackEntry &x)
    {
        entityDescriptor = x.entityDescriptor;
        entityId = x.entityId;
        idFinal = x.idFinal;
        deferredAttributes = x.deferredAttributes;

        return *this;
    }

    void XMLReaderStack::Push(const XMLReaderStackEntry &e)
    {
        mStack.push_back(e);
    }

    int XMLReaderStack::Pop()
    {
        int status = DLB_ADM_STATUS_ERROR;

        if (mStack.size() > 0)
        {
            mStack.pop_back();
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    int XMLReaderStack::Top(XMLReaderStackEntry *&d)
    {
        int status = DLB_ADM_STATUS_ERROR;

        if (mStack.size() > 0)
        {
            d = &mStack.back();
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    int XMLReaderStack::Top2(XMLReaderStackEntry *&top, XMLReaderStackEntry *&nextToTop)
    {
        int status = DLB_ADM_STATUS_ERROR;
        size_t n = mStack.size();

        if (n >= 2)
        {
            top = &mStack[n - 1];
            nextToTop = &mStack[n - 2];
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    int XMLReaderStack::Bottom(XMLReaderStackEntry *&bottom)
    {
        int status = DLB_ADM_STATUS_ERROR;

        if (mStack.size() > 0)
        {
            bottom = &mStack.front();
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    size_t XMLReaderStack::Size() const
    {
        return mStack.size();
    }

    bool XMLReaderStack::Empty() const
    {
        return mStack.empty();
    }

}
