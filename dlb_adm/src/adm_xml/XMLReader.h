/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2021, Dolby Laboratories Inc.
 * Copyright (c) 2020-2021, Dolby International AB.
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

#ifndef DLB_ADM_XML_READER_H
#define DLB_ADM_XML_READER_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "XMLBuffer.h"
#include "XMLReaderStack.h"

#include <cstdio>
#include <boost/noncopyable.hpp>

namespace DlbAdm
{

    struct AttributeDescriptor;
    class XMLContainer;

    class XMLReader : public boost::noncopyable
    {
    public:
        XMLReader(XMLContainer &container, FILE *f, bool isCommon = false);
        XMLReader(XMLContainer &container, FILE *f, const char *traceFilePath, bool isCommon = false);
        XMLReader(XMLContainer &container, const char *stringBuffer, size_t characterCount, bool isCommon = false);
        XMLReader(XMLContainer &container, const char *stringBuffer, size_t characterCount, const char *traceFilePath, bool isCommon = false);
        ~XMLReader();

        char *GetLine();

        int Element(const char *tag, const char *text);

        int Attribute(const char *tag, const char *attribute, const char *value);

    private:
        static const size_t LINE_BUFFER_SIZE = 4096;

        enum class XML_TAG_STATE
        {
            NOT_STARTED,
            IN_PROGRESS,
            COMPLETE
        };

        void Start();

        int MakeGenericComponentId(XMLReaderStackEntry *component);

        int SetValue(XMLReaderStackEntry &entry, DLB_ADM_TAG attributeTag,         const std::string &valueString);
        int SetValue(XMLReaderStackEntry &entry, const AttributeDescriptor &desc,  const std::string &valueString);

        XMLContainer        &mContainer;
        XMLReaderStack       mStack;
        FILE                *mInputFile;
        FILE                *mTraceFile;
        XMLBuffer            mBuffer;
        char                 mLineBuffer[LINE_BUFFER_SIZE];
        XML_TAG_STATE        mXmlTagState;
        bool                 mIsCommon;
    };

}

#endif
