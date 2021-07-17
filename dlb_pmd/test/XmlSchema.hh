/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018 by Dolby Laboratories,
 *                Copyright (C) 2018 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file XmlSchema.hh
 *
 * @brief encapsulates the PMD XML Schema checker.
 *
 * This is used to test that generated XML is correct with respect to the schema
 */


class XmlSchema
{
public:
    static void initialize();
    static void finalize();
    

    /**
     * @brief test that the XML in the given buffer matches the schema set
     *
     * Note that this is a NO-OP if #test_xml_schema_set has not been called,
     * and will return 0 by default.
     */
    static bool test(const uint8_t *buffer, size_t size);

private:
    XmlSchema();
    ~XmlSchema();
};

    


