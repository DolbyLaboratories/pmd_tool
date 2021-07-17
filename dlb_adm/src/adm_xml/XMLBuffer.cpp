/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "XMLBuffer.h"

namespace DlbAdm
{

    XMLBuffer::XMLBuffer()
        : mCurrent(nullptr)
        , mEnd(nullptr)
    {
        // Empty
    }

    XMLBuffer::XMLBuffer(const char *buffer, size_t characterCount)
        : mCurrent(buffer)
        , mEnd(buffer + characterCount)
    {
        // Empty
    }

    XMLBuffer::XMLBuffer(const XMLBuffer &x)
        : mCurrent(x.mCurrent)
        , mEnd(x.mEnd)
    {
        // Empty
    }

    XMLBuffer::~XMLBuffer()
    {
        mEnd = mCurrent = nullptr;
    }

    XMLBuffer &XMLBuffer::operator=(const XMLBuffer &x)
    {
        mCurrent = x.mCurrent;
        mEnd = x.mEnd;

        return *this;
    }

    size_t XMLBuffer::GetLine(char *lineBuffer, size_t maxChars)
    {
        size_t gotCount = 0;

        if ((mEnd > mCurrent) && (maxChars > 0))
        {
            size_t remaining = mEnd - mCurrent;

            if (remaining < maxChars)
            {
                maxChars = remaining;
            }

            char *lineCur = lineBuffer;
            char *lineEnd = lineCur + maxChars;
            char c;

            while (true)
            {
                c = *mCurrent++;
                ++gotCount;
                *lineCur++ = c;
                if ((c == '\n') || (mCurrent == mEnd) || (lineCur == lineEnd))
                {
                    break;
                }
            }
        }

        return gotCount;
    }

}
