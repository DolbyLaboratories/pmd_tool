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

#ifndef DLB_ADM_XML_BUFFER_H
#define DLB_ADM_XML_BUFFER_H

#include <cstddef>

namespace DlbAdm
{

    /**
     * @brief Manage access to a character buffer so we can retrieve text
     * one line at a time; essentially, this implements the idea of sgets(),
     * which would be similar to fgets(), but for character string input
     * instead of reading from a file.  Does not copy the original buffer,
     * which must remain available through the lifetime of the XMLBuffer
     * instance.
     */
    class XMLBuffer
    {
    public:
        XMLBuffer();
        XMLBuffer(const char *buffer, size_t characterCount);
        XMLBuffer(const XMLBuffer &x);
        ~XMLBuffer();

        XMLBuffer &operator=(const XMLBuffer &x);

        size_t GetLine(char *lineBuffer, size_t maxChars);  // Does not add NUL termination!

    private:
        const char *mCurrent;
        const char *mEnd;
    };

}

#endif
