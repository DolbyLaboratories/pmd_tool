/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
