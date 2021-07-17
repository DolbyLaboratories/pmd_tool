/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2020 by Dolby Laboratories,
 *                Copyright (C) 2016-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/* derived from the public-domain load_grammar_sax.cpp by Boris Kolpackov */

#include <string>
#include <memory>   // std::shared_ptr
#include <cstddef>  // std::size_t
#include <iostream>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesVersion.hpp>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "XmlSchema.hh"
#include "dlb_pmd_3_2_1_xsd.h"

using namespace std;
using namespace xercesc;


class Parser
{
private:

    class error_handler: public ErrorHandler
    {
    public:

        error_handler ()
            : failed_ (false)
        {}

        bool failed () const
        {
            return failed_;
        }

        enum severity {s_warning, s_error, s_fatal};

        void warning (const SAXParseException& e)
        {
            handle (e, s_warning);            
        }
        
        void error (const SAXParseException& e)
        {
            failed_ = true;
            handle (e, s_error);
        }
        
        void fatalError (const SAXParseException& e)
        {
            failed_ = true;
            handle (e, s_fatal);
        }
        
        void resetErrors ()
        {
            failed_ = false;
        }

        void handle (const SAXParseException& e, severity s)
        {
            const XMLCh* xid (e.getPublicId ());

            if (xid == 0)
            {
                xid = e.getSystemId ();
            }

            char* id (XMLString::transcode (xid));
            char* msg (XMLString::transcode (e.getMessage ()));

            cerr << id << ":" << e.getLineNumber () << ":" << e.getColumnNumber ()
                 << " " << (s == s_warning ? "warning: " : "error: ") << msg << endl;

            XMLString::release (&id);
            XMLString::release (&msg);
        }
        
    private:
        bool failed_;
    };


    class XMLLifetime
    {
    public:
        XMLLifetime()
        {
            XMLPlatformUtils::Initialize();
        }
        

        ~XMLLifetime()
        {
            XMLPlatformUtils::Terminate();
        }
        
    };

    XMLLifetime xml_lifetime;
    MemoryManager* mm;
    shared_ptr<XMLGrammarPool> gp;
    shared_ptr<SAX2XMLReader> parser;
    error_handler eh;
    bool valid;


    shared_ptr<SAX2XMLReader> create_parser (XMLGrammarPool* pool)
    {
        shared_ptr<SAX2XMLReader> p (
            pool ? XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager,
                                                      pool)
                 : XMLReaderFactory::createXMLReader ());
        
        // Commonly useful configuration.
        //
        p->setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
        p->setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, true);
        p->setFeature (XMLUni::fgSAX2CoreValidation, true);
        
        // Enable validation.
        //
        p->setFeature (XMLUni::fgXercesSchema, true);
        p->setFeature (XMLUni::fgXercesSchemaFullChecking, true);
        p->setFeature (XMLUni::fgXercesValidationErrorAsFatal, true);
        
        // Use the loaded grammar during parsing.
        //
        p->setFeature (XMLUni::fgXercesUseCachedGrammarInParse, true);
        
        // Don't load schemas from any other source (e.g., from XML document's
        // xsi:schemaLocation attributes).
        //
        p->setFeature (XMLUni::fgXercesLoadSchema, false);
        
        // Xerces-C++ 3.1.0 is the first version with working multi import
        // support.
        //
#if _XERCES_VERSION >= 30100
        p->setFeature (XMLUni::fgXercesHandleMultipleImports, true);
#endif
        return p;
    }


public:

    Parser ()
        : mm(XMLPlatformUtils::fgMemoryManager)
        , gp(new XMLGrammarPoolImpl(mm))
        , parser(create_parser(gp.get()))
        , valid(false)
    {
        parser->setErrorHandler(&eh);
        xercesc::MemBufInputSource schemabuf(dlb_pmd_3_2_1_xsd, dlb_pmd_3_2_1_xsd_len,
                                             "PMD schema");
        if (parser->loadGrammar(schemabuf, Grammar::SchemaGrammarType, true))
        {
            valid = true;
        }
        else
        {
            cerr << "error: unable to load schema" << endl;
        }
        gp->lockPool ();
    }
    

    ~Parser ()
    {
    }


    bool match (const uint8_t *buffer, size_t size)
    {
        if (valid)
        {
            MemBufInputSource membuf((const XMLByte*)buffer, size, "membufid", false);
            parser->parse(membuf);
            if (eh.failed())
            {
                return false;
            }
            return true;
        }
        return true;
    }
};

    
static Parser *the_parser = 0;


void XmlSchema::initialize()
{
    the_parser = new Parser();
}


bool XmlSchema::test(const uint8_t *buffer, size_t size)
{
    if (the_parser)
    {
        return the_parser->match(buffer, size);
    }
    return false;
}


void XmlSchema::finalize()
{
    if (the_parser)
    {
        delete the_parser;
        the_parser = 0;
    }
}

