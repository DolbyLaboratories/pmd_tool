/************************************************************************
 * dlb_adm
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
