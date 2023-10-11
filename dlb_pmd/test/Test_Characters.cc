/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

/**
 * @file Test_Characters.cc
 * @brief character encoding tests
 *
 * Check that we can encode and decode PMD strings in XML properly
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"    

#include "TestXmlWriter.hh"
#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_CHARACTER_TESTS


#ifndef DISABLE_CHARACTER_TESTS
/**
 * @brief write a unicode character to buffer
 */
static
int                       /** @return 0 on failure, 1 on success */
write_utf8_char
    (char **bufptr        /**< [in] buffer to write to */
    ,char *end            /**< [in] 1st character after buffer end */
    ,unsigned int unicode /**< [in] unicode character to write */
    )
{
    char *buf = *bufptr;
    if (unicode < 128)
    {
        if (buf >= end)
        {
            return 0;
        }
        *buf++ = (char)unicode;
    }
    else if (unicode < 0x800)
    {
        if (buf+1 >= end)
        {
            return 0;
        }
        *buf++ = 0xC0 | ((unicode >> 6) & 0x1F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else if (unicode < 0x10000)
    {
        if (buf+2 >= end)
        {
            return 0;
        }
        *buf++ = 0xE0 | ((unicode >> 12) & 0x0F);
        *buf++ = 0x80 | ((unicode >> 6) & 0x3F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else if (unicode < 0x110000)
    {
        if (buf+3 >= end)
        {
            return 0;
        }
        *buf++ = 0xF0 | ((unicode >> 18) & 0x07);
        *buf++ = 0x80 | ((unicode >> 12) & 0x3F);
        *buf++ = 0x80 | ((unicode >> 6) & 0x3F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else
    {
        return 0;
    }
    *bufptr = buf;
    return 1;
}


TEST(CharacterTest, test_ampersand_chars)
{
    char bedname[64];
    char objname[64];
    char presname[64];
    char title[64];

    TestModel m;

    dlb_pmd_presentation_id p = m.new_presid();
    dlb_pmd_element_id bed = m.new_elid();    
    dlb_pmd_element_id obj = m.new_elid();

    snprintf(bedname, sizeof(bedname), "Bed &<>\"\'");
    snprintf(objname, sizeof(objname), "Obj &<>\"\'");
    snprintf(presname, sizeof(presname), "Pres &<>\"\'");
    snprintf(title, sizeof(title), "XML ampersand encoding testing &<>\"\'");

    if (   dlb_pmd_set_title(m, title)
        || dlb_pmd_add_signals(m, 3)
        || dlb_pmd_add_bed(m, bed,  bedname, DLB_PMD_SPEAKER_CONFIG_2_0, 1, 0)
        || dlb_pmd_add_dialog(m, obj, objname, 1, 0)
        || dlb_pmd_add_presentation2(m, p, "eng", presname, "eng", DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
    {
        ADD_FAILURE() << "Could not populate model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            m.test(TestModel::TEST_XML, "XML_Ampersand_Codes", 1);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}



TEST(CharacterTest, test_illegal_chars)
{
    TestModel m;
    
    dlb_pmd_presentation_id p = m.new_presid();
    dlb_pmd_element_id bed = m.new_elid();    
    dlb_pmd_element_id obj = m.new_elid();
    char tmpbuf[32];
    char *tmp = tmpbuf;
    char *end = (char*)tmp + sizeof(tmp);
    unsigned int i;

    /* ignore ascii control codes */
    for (i = 1; i < ' '; ++i)
    {
        if (i != '\t' && i != '\r' && i != '\n')
        {
            snprintf(tmp, sizeof(tmpbuf), "%c", i);
            if (!dlb_pmd_set_title(m, tmp))
            {
                ADD_FAILURE() << "Should not allow illegal character in title, but did " << i;
            }        

            if (!dlb_pmd_add_bed(m, bed, tmp, DLB_PMD_SPEAKER_CONFIG_2_0, 1, 0))
            {
                ADD_FAILURE() << "Should not allow illegal character in bed name, but did" << i;
            }

            if (!dlb_pmd_add_dialog(m, obj, tmp, 1, 0))
            {
                ADD_FAILURE() << "Should not allow illegal character in obj name, but did" << i;
            }            

            if (!dlb_pmd_add_presentation2(m, p, "eng", tmp, "eng", DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
            {
                ADD_FAILURE() << "Should not allow illegal character in pres name, but did" << i;
            }            
        }
    }

    /* now consider illegal unicode ranges */
    static struct 
    {
        unsigned int min;
        unsigned int max;
    } illegal_ranges[3] =
      {
          { 0xd800,   0xdfff },
          { 0xfffe,   0xffff },
          { 0x110000, 0x110004 },  /* everything above this is illegal, but too much to test */
      };
    
    for (i = 0; i < 3; ++i)
    {
        for (unsigned int j = illegal_ranges[i].min; j < illegal_ranges[i].max; j += 4)
        {
            if (write_utf8_char(&tmp, end, j)
                && !dlb_pmd_set_title(m, tmpbuf))
            {
                ADD_FAILURE() << "Should not allow illegal character in title, but did " << j;
            }        
        
            if (write_utf8_char(&tmp, end, j+1)
                && !dlb_pmd_add_bed(m, bed, tmpbuf, DLB_PMD_SPEAKER_CONFIG_2_0, 1, 0))
            {
                ADD_FAILURE() << "Should not allow illegal character in bed name, but did " << j;
            }
            
            if (write_utf8_char(&tmp, end, j+2)
                && !dlb_pmd_add_dialog(m, obj, tmpbuf, 1, 0))
            {
                ADD_FAILURE() << "Should not allow illegal character in obj name, but did " << j;
            }            
        
            if (write_utf8_char(&tmp, end, j+3)
                && !dlb_pmd_add_presentation2(m, p, "eng", tmpbuf, "eng", DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
            {
                ADD_FAILURE() << "Should not allow illegal character in pres name, but did " << j;
            }            
        }
    }
}


static
void
write_xml_string
    (XMLTestWriter& w
    ,unsigned int unicode
    )
{
    char wierdname1[128];
    char wierdname2[128];
    char wierdname3[128];
    char wierdname4[128];

    snprintf(wierdname1, sizeof(wierdname1), "&amp;&lt;&quot;&gt;&apos;&#%u;&#x%x;",
             unicode, unicode+1);
    snprintf(wierdname2, sizeof(wierdname2), "&amp;&lt;&quot;&gt;&apos;&#%u;&#x%x;",
             unicode+2, unicode+3);
    snprintf(wierdname3, sizeof(wierdname3), "&amp;&lt;&quot;&gt;&apos;&#%u;&#x%x;",
             unicode+4, unicode+5);
    snprintf(wierdname4, sizeof(wierdname4), "&amp;&lt;&quot;&gt;&apos;&#%u;&#x%x;",
             unicode+6, unicode+7);

    w << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<Smpte2109>\n"
      << "  <ContainerConfig>\n"
      << "    <SampleOffset>0</SampleOffset>\n"
      << "    <DynamicTags/>\n"
      << "  </ContainerConfig>\n"
      << "  <ProfessionalMetadata version=\"11.0\">\n"
      << "    <Title>XML escape-character testing: " << wierdname1 << "</Title>\n"
      << "    <AudioSignals>\n"
      << "      <AudioSignal id=\"1\">\n"
      << "        <Name>&amp;&quot;&gt;&apos;&lt;</Name>\n"
      << "      </AudioSignal>\n"
      << "      <AudioSignal id=\"2\">\n"
      << "        <Name>" << wierdname2 << "</Name>\n"
      << "      </AudioSignal>\n"
      << "    </AudioSignals>\n"
      << "    <AudioElements>\n"
      << "      <AudioBed id=\"1\">\n"
      << "        <Name>Bed " << wierdname3 << "</Name>\n"
      << "        <SpeakerConfig>2.0</SpeakerConfig>\n"
      << "          <OutputTargets>\n"
      << "          <OutputTarget id=\"Left\">\n"
      << "            <AudioSignals>\n"
      << "              <ID>1</ID>\n"
      << "            </AudioSignals>\n"
      << "          </OutputTarget>\n"
      << "          <OutputTarget id=\"Right\">\n"
      << "            <AudioSignals>\n"
      << "              <ID>2</ID>\n"
      << "            </AudioSignals>\n"
      << "          </OutputTarget>\n"
      << "        </OutputTargets>\n"
      << "      </AudioBed>\n"
      << "    </AudioElements>\n"
      << "    <Presentations>\n"
      << "      <Presentation id=\"1\">\n"
      << "        <Name language=\"eng\">Pres " << wierdname4 << "</Name>\n"
      << "        <Config>5.1 CM</Config>\n"
      << "        <Language>eng</Language>\n"
      << "        <Element>1</Element>\n"
      << "      </Presentation>\n"
      << "    </Presentations>\n"
      << "  </ProfessionalMetadata>\n"
      << "</Smpte2109>\n";
}


TEST(CharacterTest, good_unicode_parse_strings1)
{
    /* the test string uses 8 sequential character codes */
    for (unsigned int unicode = 128; unicode < 0xd7f9; unicode += 8)
    {
        XMLTestWriter w;
        TestModel m;

        write_xml_string(w, unicode);
        if (!w.ingest(m))
        {
            ADD_FAILURE() << "Could not read XML string: " << w.error();
        }
        else
        {
            try
            {
                m.test(TestModel::TEST_XML, "XML_Good_Unicodes1", 1);
            }
            catch (TestModel::failure& f)
            {
                ADD_FAILURE() << f.msg;
            }
        }
    }
}


TEST(CharacterTest, good_unicode_parse_strings2)
{
    /* each test string tests eight sequential codes */
    for (unsigned int unicode = 0xe000; unicode < 0xfff7; unicode += 8)
    {
        XMLTestWriter w;
        TestModel m;

        write_xml_string(w, unicode);
        if (!w.ingest(m))
        {
            ADD_FAILURE() << "Could not read XML string: " << w.error();
        }
        else
        {
            try
            {
                m.test(TestModel::TEST_XML, "XML_Good_Unicodes2", 1);
            }
            catch (TestModel::failure& f)
            {
                ADD_FAILURE() << f.msg;
            }
        }
    }
}


TEST(CharacterTest, good_unicode_parse_strings3)
{
    /* each test string tests eight sequential codes */
    for (unsigned int unicode = 0x10000; unicode < 0x10fff9; unicode += 8)
    {
        XMLTestWriter w;
        TestModel m;

        write_xml_string(w, unicode);
        if (!w.ingest(m))
        {
            ADD_FAILURE() << "Could not read XML string: " << w.error();
        }
        else
        {
            try
            {
                m.test(TestModel::TEST_XML, "XML_Good_Unicodes3", 1);
            }
            catch (TestModel::failure& f)
            {
                ADD_FAILURE() << f.msg;
            }
        }
    }
}


TEST(CharacterTest, bad_unicode_test_strings1)
{
    for (unsigned int unicode = 0xd800; unicode < 0xdfff; ++unicode)
    {
        XMLTestWriter w;
        TestModel m;

        write_xml_string(w, unicode);
        if (w.ingest(m))
        {
            ADD_FAILURE() << "Should not have read XML string: " << unicode;
        }
    }
}


TEST(CharacterTest, bad_unicode_test_strings2)
{
    for (unsigned int unicode = 0xfffe; unicode < 0xffff; ++unicode)
    {
        XMLTestWriter w;
        TestModel m;

        write_xml_string(w, unicode);
        if (w.ingest(m))
        {
            ADD_FAILURE() << "Should not have read XML string: " << unicode;
        }
    }
}


TEST(CharacterTest, bad_unicode_test_strings3)
{
    /* we can't go through the entire 32-bit illegal values */
    for (unsigned int unicode = 0x110000; unicode < 0x7fffffff; unicode += 0x111111)
    {
        XMLTestWriter w;
        TestModel m;

        write_xml_string(w, unicode);
        if (w.ingest(m))
        {
            ADD_FAILURE() << "Should not have read XML string: " << unicode;
        }
    }
}
#endif
