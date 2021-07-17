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

#ifndef DLB_ADM_XML_WRITER_H
#define DLB_ADM_XML_WRITER_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <string>
#include <sstream>
#include <boost/noncopyable.hpp>

namespace DlbAdm
{

    struct EntityDescriptor;
    struct EntityRecord;
    class XMLContainer;

    class XMLWriter : public boost::noncopyable
    {
    public:
        XMLWriter(dlb_adm_write_buffer_callback bufferCallback, void *callbackArg, XMLContainer &container);
        ~XMLWriter();

        int Write();

    private:
        static const size_t MAX_INDENT = 2048;

        enum class WRITE_METHOD
        {
            IMMEDIATE,
            SIMPLE,
            COMPOUND
        };

        WRITE_METHOD WriteMethod(const EntityDescriptor &d);

        void Indent();

        void Outdent();

        void WriteIndent();

        int WriteNewLine();

        int WriteAttributes(const EntityRecord &e, const EntityDescriptor &d);

        int WriteOpenCloseTag(const EntityRecord &e, const EntityDescriptor &d);

        int WriteOpenTag(const EntityRecord &e, const EntityDescriptor &d);

        int WriteValue(const EntityRecord &e, const EntityDescriptor &d);

        int WriteSubElements(const EntityRecord &e);

        int WriteReferences(const EntityRecord &e);

        void WriteCloseTag(const EntityDescriptor &d);

        int WriteElement(const EntityRecord &e);

        int WriteReference(const EntityRecord &e);

        int WriteBuffer();

        void Flush();

        XMLContainer &mContainer;

        std::ostringstream mOutputStream;
        dlb_adm_write_buffer_callback mBufferCallback;
        void *mCallbackArg;
        char *mWritePos;
        char *mWriteEnd;

        char mIndentBuffer[MAX_INDENT + 1];
        std::string mIndentString;
        size_t mIndentCount;

        bool mWritten;
    };

}

#endif /* DLB_ADM_XML_WRITER_H */
