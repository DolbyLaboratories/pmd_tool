/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
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

#include "XMLWriter.h"
#include "EntityDescriptor.h"
#include "EntityRecord.h"
#include "AttributeDescriptor.h"
#include "RelationshipRecord.h"
#include "XMLContainer.h"
#include "dlb_adm_api.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

#include <string.h>
#include <locale.h>

#ifdef NDEBUG
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)
#else
static int retstat(int s)
{
    return s;   // Put a breakpoint here
}
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return retstat(s)
#endif

namespace DlbAdm
{

    XMLWriter::XMLWriter(dlb_adm_write_buffer_callback bufferCallback, void *callbackArg, XMLContainer &container)
        : mContainer(container)
        , mOutputStream()
        , mBufferCallback(bufferCallback)
        , mCallbackArg(callbackArg)
        , mWritePos(nullptr)
        , mWriteEnd(nullptr)
        , mIndentBuffer()
        , mIndentString()
        , mIndentCount(0)
        , mWritten(false)
    {
        ::memset(mIndentBuffer, ' ', sizeof(mIndentBuffer));
        mIndentBuffer[MAX_INDENT] = '\0';
        mIndentString = mIndentBuffer;
    }

    XMLWriter::~XMLWriter()
    {
        ::memset(mIndentBuffer, 0, sizeof(mIndentBuffer));
        mWritten = true;
    }

    int XMLWriter::Write()
    {
        int status = (mWritten ? DLB_ADM_STATUS_ERROR : DLB_ADM_STATUS_OK);

        if (status == DLB_ADM_STATUS_OK)
        {
            char *l = ::setlocale(LC_ALL, NULL);
            bool restoreLocale = (::strcmp(l, "C") != 0);
            std::string savedLocale = l;

            try
            {
                if (restoreLocale)
                {
                    ::setlocale(LC_ALL, "C");
                }

                status = mContainer.ForEachRelationship
                (
                    mContainer.GetTopLevelID(),
                    ENTITY_RELATIONSHIP::CONTAINS,
                    [&](const RelationshipRecord &rr)
                    {
                        EntityRecord er;
                        int s;

                        s = mContainer.GetEntity(er, rr.toId);
                        CHECK_STATUS(s);
                        s = WriteElement(er);

                        return s;
                    }
                );
                Flush();
                mWritten = true;

                if (restoreLocale)
                {
                    ::setlocale(LC_ALL, savedLocale.c_str());
                }
            }
            catch (...)
            {
                if (restoreLocale)
                {
                    ::setlocale(LC_ALL, savedLocale.c_str());
                }
                mWritten = true;
                throw;
            }
        }

        return status;
    }

    XMLWriter::WRITE_METHOD XMLWriter::WriteMethod(const EntityDescriptor &d)
    {
        WRITE_METHOD m = WRITE_METHOD::COMPOUND;

        if (!d.xmlTypeComposite)
        {
            if (d.distinguishedTag == DLB_ADM_TAG_UNKNOWN)
            {
                m = WRITE_METHOD::IMMEDIATE;
            } 
            else
            {
                m = WRITE_METHOD::SIMPLE;
            }
        }

        return m;
    }

    void XMLWriter::Indent()
    {
        mIndentCount += 2;
    }

    void XMLWriter::Outdent()
    {
        if (mIndentCount > 1)
        {
            mIndentCount -= 2;
        } 
        else
        {
            mIndentCount = 0;
        }
    }

    void XMLWriter::WriteIndent()
    {
        size_t n = mIndentCount;

        if (n > MAX_INDENT)
        {
            n = MAX_INDENT;
        }

        mOutputStream << mIndentString.substr(0, n);
    }

    int XMLWriter::WriteNewLine()
    {
        mOutputStream << std::endl;
        return WriteBuffer();
    }

    int XMLWriter::WriteAttributes(const EntityRecord &e, const EntityDescriptor &d)
    {
        int status;

        if (d.xmlTypeComposite && d.distinguishedTag != DLB_ADM_TAG_UNKNOWN)
        {
            AttributeDescriptor ad;

            status = GetAttributeDescriptor(ad, d.distinguishedTag);
            CHECK_STATUS(status);
            if (d.hasADMIdOrRef)
            {
                static const size_t bufferSize = 32;
                char buffer[bufferSize];

                status = dlb_adm_write_entity_id(buffer, bufferSize, e.id);
                CHECK_STATUS(status);
                mOutputStream << ' ' << ad.attributeName << "=\"" << buffer << '\"';
            } 
            else
            {
                AttributeValue v;

                status = mContainer.GetValue(v, e.id, d.distinguishedTag);
                CHECK_STATUS(status);
                mOutputStream << ' ' << ad.attributeName << "=\"" << v << '\"';
            }
        }

        status = mContainer.ForEachAttribute
        (
            e.id,
            [&](const dlb_adm_entity_id &, DLB_ADM_TAG tag, const AttributeValue &value)
            {
                if (tag != d.distinguishedTag)
                {
                    AttributeDescriptor ad;
                    int s = GetAttributeDescriptor(ad, tag);
                    CHECK_STATUS(s);
                    mOutputStream << ' ' << ad.attributeName << "=\"" << value << '\"';
                }

                return static_cast<int>(DLB_ADM_STATUS_OK);
            }
        );
        CHECK_STATUS(status);

        return status;
    }

    int XMLWriter::WriteOpenCloseTag(const EntityRecord &e, const EntityDescriptor &d)
    {
        std::string opening = "<";
        std::string closing = "/>";
        int status;

        // precondition: output stream is positioned at beginning of line

        if (d.entityType == DLB_ADM_ENTITY_TYPE_XML)
        {
            opening = "<?";
            closing = "?>";
        }

        WriteIndent();
        mOutputStream << opening << d.name;
        status = WriteAttributes(e, d);
        CHECK_STATUS(status);
        mOutputStream << closing;

        return status;
    }

    int XMLWriter::WriteOpenTag(const EntityRecord &e, const EntityDescriptor &d)
    {
        int status;

        // precondition: output stream is positioned at beginning of line

        WriteIndent();
        mOutputStream << '<' << d.name;
        status = WriteAttributes(e, d);
        CHECK_STATUS(status);
        mOutputStream << '>';

        return status;
    }

    int XMLWriter::WriteValue(const EntityRecord &e, const EntityDescriptor &d)
    {
        AttributeValue v;
        int status;

        status = mContainer.GetValue(v, e.id, d.distinguishedTag);
        CHECK_STATUS(status);
        mOutputStream << v;

        return status;
    }

    int XMLWriter::WriteSubElements(const EntityRecord &e)
    {
        return mContainer.ForEachRelationship
        (
            e.id,
            ENTITY_RELATIONSHIP::CONTAINS,
            [this](const RelationshipRecord &r)
            {
                EntityRecord subelement;
                int status = mContainer.GetEntity(subelement, r.toId);
                CHECK_STATUS(status);
                return WriteElement(subelement);
            }
        );
    }

    int XMLWriter::WriteReferences(const EntityRecord &e)
    {
        return mContainer.ForEachRelationship
        (
            e.id,
            ENTITY_RELATIONSHIP::REFERENCES,
            [this](const RelationshipRecord &r)
            {
                EntityRecord subelement;
                int status = mContainer.GetEntity(subelement, r.toId);
                CHECK_STATUS(status);
                return WriteReference(subelement);
            }
        );
    }

    void XMLWriter::WriteCloseTag(const EntityDescriptor &d)
    {
        mOutputStream << "</" << d.name << '>';
    }

    int XMLWriter::WriteElement(const EntityRecord &e)
    {
        if (e.status == EntityRecord::STATUS::COMMON_DEFINITION)
        {
            return DLB_ADM_STATUS_OK;
        }

        EntityDescriptor d;
        int status;

        status = GetEntityDescriptor(d, static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(e.id)), false);
        CHECK_STATUS(status);

        WRITE_METHOD m = WriteMethod(d);
        switch (m)
        {
        case WRITE_METHOD::IMMEDIATE:
            status = WriteOpenCloseTag(e, d);
            CHECK_STATUS(status);
            break;

        case WRITE_METHOD::SIMPLE:
            status = WriteOpenTag(e, d);
            CHECK_STATUS(status);
            status = WriteValue(e, d);
            CHECK_STATUS(status);
            WriteCloseTag(d);
            break;

        case WRITE_METHOD::COMPOUND:
            status = WriteOpenTag(e, d);
            CHECK_STATUS(status);
            status = WriteNewLine();
            CHECK_STATUS(status);
            Indent();
            status = WriteSubElements(e);
            CHECK_STATUS(status);
            status = WriteReferences(e);
            CHECK_STATUS(status);
            Outdent();
            WriteIndent();
            WriteCloseTag(d);
            break;

        default:
            return DLB_ADM_STATUS_ERROR;
        }
        status = WriteNewLine();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLWriter::WriteReference(const EntityRecord &e)
    {
        static const size_t BUF_LEN = 32;
        char buffer[BUF_LEN];
        EntityDescriptor d;
        int status;

        status = GetEntityDescriptor(d, static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(e.id)), true);
        CHECK_STATUS(status);
        status = dlb_adm_write_entity_id(buffer, BUF_LEN, e.id);
        CHECK_STATUS(status);

        WriteIndent();
        mOutputStream << '<' << d.name << '>' << buffer << "</" << d.name << '>';
        status = WriteNewLine();
#ifndef NDEBUG
        CHECK_STATUS(status);
#endif
        return status;
    }

    int XMLWriter::WriteBuffer()
    {
        int status = DLB_ADM_STATUS_OUT_OF_MEMORY;
        size_t len = mOutputStream.str().size();
        bool good = (mWritePos + len < mWriteEnd);

        if (!good)
        {
            char *buf = NULL;
            size_t capacity;

            if (mBufferCallback(mCallbackArg, mWritePos, &buf, &capacity))
            {
                mWritePos = buf;
                mWriteEnd = buf + capacity;
                good = (mWritePos + len < mWriteEnd);
            }
        }

        if (good)
        {
            sprintf(mWritePos, "%s", mOutputStream.str().c_str());
            mWritePos += len;
            mOutputStream.str("");
            mOutputStream.clear();
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    void XMLWriter::Flush()
    {
        mBufferCallback(mCallbackArg, mWritePos, NULL, NULL);
    }

}

