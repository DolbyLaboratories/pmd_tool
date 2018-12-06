// file      : load-grammar-dom.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : not copyrighted - public domain

// This program uses Xerces-C++ DOM parser to load a set of schema files
// and then to validate a set of XML documents against these schemas. To
// build this program you will need Xerces-C++ 3.0.0 or later. For more
// information, see:
//
// http://www.codesynthesis.com/~boris/blog/2010/03/15/validating-external-schemas-xerces-cxx/
//

#include <string>
#include <memory>   // std::auto_ptr
#include <cstddef>  // std::size_t
#include <iostream>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

using namespace std;
using namespace xercesc;

class error_handler: public DOMErrorHandler
{
public:
  error_handler () : failed_ (false) {}

  bool
  failed () const { return failed_; }

  virtual bool
  handleError (const xercesc::DOMError&);

private:
  bool failed_;
};

DOMLSParser*
create_parser (XMLGrammarPool* pool)
{
  const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};

  DOMImplementation* impl (
    DOMImplementationRegistry::getDOMImplementation (ls_id));

  DOMLSParser* parser (
    impl->createLSParser (
      DOMImplementationLS::MODE_SYNCHRONOUS,
      0,
      XMLPlatformUtils::fgMemoryManager,
      pool));

  DOMConfiguration* conf (parser->getDomConfig ());

  // Commonly useful configuration.
  //
  conf->setParameter (XMLUni::fgDOMComments, false);
  conf->setParameter (XMLUni::fgDOMDatatypeNormalization, true);
  conf->setParameter (XMLUni::fgDOMEntities, false);
  conf->setParameter (XMLUni::fgDOMNamespaces, true);
  conf->setParameter (XMLUni::fgDOMElementContentWhitespace, false);

  // Enable validation.
  //
  conf->setParameter (XMLUni::fgDOMValidate, true);
  conf->setParameter (XMLUni::fgXercesSchema, true);
  conf->setParameter (XMLUni::fgXercesSchemaFullChecking, false);

  // Use the loaded grammar during parsing.
  //
  conf->setParameter (XMLUni::fgXercesUseCachedGrammarInParse, true);

  // Don't load schemas from any other source (e.g., from XML document's
  // xsi:schemaLocation attributes).
  //
  conf->setParameter (XMLUni::fgXercesLoadSchema, false);

  // Xerces-C++ 3.1.0 is the first version with working multi
  // import support.
  //
#if _XERCES_VERSION >= 30100
  conf->setParameter (XMLUni::fgXercesHandleMultipleImports, true);
#endif

  // We will release the DOM document ourselves.
  //
  conf->setParameter (XMLUni::fgXercesUserAdoptsDOMDocument, true);

  return parser;
}

int
main (int argc, char* argv[])
{
  if (argc < 2)
  {
    cerr << "usage: " << argv[0] << " [test.xsd ... ] [test.xml ...]" << endl;
    return 1;
  }

  XMLPlatformUtils::Initialize ();

  int r (0);

  while (true)
  {
    MemoryManager* mm (XMLPlatformUtils::fgMemoryManager);
    auto_ptr<XMLGrammarPool> gp (new XMLGrammarPoolImpl (mm));

    int i (1);

    // Load the schemas into grammar pool.
    //
    {
      DOMLSParser* parser (create_parser (gp.get ()));

      error_handler eh;
      parser->getDomConfig ()->setParameter (XMLUni::fgDOMErrorHandler, &eh);

      for (; i < argc; ++i)
      {
        string s (argv[i]);
        size_t n (s.size ());

        if (n < 5 || s[n - 4] != '.' || s[n - 3] != 'x' ||
            s[n - 2] != 's' || s[n - 1] != 'd')
          break;

        cerr << "loading " << s << endl;

        if (!parser->loadGrammar (s.c_str (), Grammar::SchemaGrammarType, true))
        {
          cerr << s << ": error: unable to load" << endl;
          r = 1;
          break;
        }

        if (eh.failed ())
        {
          r = 1;
          break;
        }
      }

      parser->release ();

      if (r != 0)
        break;
    }

    // Lock the grammar pool. This is necessary if we plan to use the
    // same grammar pool in multiple threads (this way we can reuse the
    // same grammar in multiple parsers). Locking the pool disallows any
    // modifications to the pool, such as an attempt by one of the threads
    // to cache additional schemas.
    //
    gp->lockPool ();

    // Parse the XML documents.
    //
    DOMLSParser* parser (create_parser (gp.get ()));

    error_handler eh;
    parser->getDomConfig ()->setParameter (XMLUni::fgDOMErrorHandler, &eh);

    for (; i < argc; ++i)
    {
      cerr << "parsing " << argv[i] << endl;
      DOMDocument* doc (parser->parseURI (argv[i]));

      if (doc)
        doc->release ();

      if (eh.failed ())
      {
        r = 1;
        break;
      }
    }

    parser->release ();
    break;
  }

  XMLPlatformUtils::Terminate ();

  return r;
}

bool error_handler::
handleError (const xercesc::DOMError& e)
{
  bool warn (e.getSeverity() == DOMError::DOM_SEVERITY_WARNING);

  if (!warn)
    failed_ = true;

  DOMLocator* loc (e.getLocation ());

  char* uri (XMLString::transcode (loc->getURI ()));
  char* msg (XMLString::transcode (e.getMessage ()));

  cerr << uri << ":"
       << loc->getLineNumber () << ":" << loc->getColumnNumber () << " "
       << (warn ? "warning: " : "error: ") << msg << endl;

  XMLString::release (&uri);
  XMLString::release (&msg);

  return true;
}
