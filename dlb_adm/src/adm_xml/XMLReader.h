/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
