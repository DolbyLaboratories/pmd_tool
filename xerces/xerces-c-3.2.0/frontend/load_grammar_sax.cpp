// file      : load-grammar-sax.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : not copyrighted - public domain

// This program uses Xerces-C++ SAX2 parser to load a set of schema files
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
#include <xercesc/util/XercesVersion.hpp>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

using namespace std;
using namespace xercesc;

class error_handler: public ErrorHandler
{
public:
  error_handler () : failed_ (false) {}

  bool
  failed () const { return failed_; }

  enum severity {s_warning, s_error, s_fatal};

  virtual void
  warning (const SAXParseException&);

  virtual void
  error (const SAXParseException&);

  virtual void
  fatalError (const SAXParseException&);

  virtual void
  resetErrors () { failed_ = false; }

  void
  handle (const SAXParseException&, severity);

private:
  bool failed_;
};

auto_ptr<SAX2XMLReader>
create_parser (XMLGrammarPool* pool)
{
  auto_ptr<SAX2XMLReader> parser (
    pool
    ? XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager,
                                         pool)
    : XMLReaderFactory::createXMLReader ());

  // Commonly useful configuration.
  //
  parser->setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
  parser->setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, true);
  parser->setFeature (XMLUni::fgSAX2CoreValidation, true);

  // Enable validation.
  //
  parser->setFeature (XMLUni::fgXercesSchema, true);
  parser->setFeature (XMLUni::fgXercesSchemaFullChecking, true);
  parser->setFeature (XMLUni::fgXercesValidationErrorAsFatal, true);

  // Use the loaded grammar during parsing.
  //
  parser->setFeature (XMLUni::fgXercesUseCachedGrammarInParse, true);

  // Don't load schemas from any other source (e.g., from XML document's
  // xsi:schemaLocation attributes).
  //
  parser->setFeature (XMLUni::fgXercesLoadSchema, false);

  // Xerces-C++ 3.1.0 is the first version with working multi import
  // support.
  //
#if _XERCES_VERSION >= 30100
  parser->setFeature (XMLUni::fgXercesHandleMultipleImports, true);
#endif

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

    // Load the schemas into the grammar pool.
    //
    {
      auto_ptr<SAX2XMLReader> parser (create_parser (gp.get ()));

      error_handler eh;
      parser->setErrorHandler (&eh);

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
    auto_ptr<SAX2XMLReader> parser (create_parser (gp.get ()));

    error_handler eh;
    parser->setErrorHandler (&eh);

    for (; i < argc; ++i)
    {
      cerr << "parsing " << argv[i] << endl;
      parser->parse (argv[i]);

      if (eh.failed ())
      {
        r = 1;
        break;
      }
    }

    break;
  }

  XMLPlatformUtils::Terminate ();

  return r;
}

void error_handler::
warning (const SAXParseException& e)
{
  handle (e, s_warning);
}

void error_handler::
error (const SAXParseException& e)
{
  failed_ = true;
  handle (e, s_error);
}

void error_handler::
fatalError (const SAXParseException& e)
{
  failed_ = true;
  handle (e, s_fatal);
}

void error_handler::
handle (const SAXParseException& e, severity s)
{
  const XMLCh* xid (e.getPublicId ());

  if (xid == 0)
    xid = e.getSystemId ();

  char* id (XMLString::transcode (xid));
  char* msg (XMLString::transcode (e.getMessage ()));

  cerr << id << ":" << e.getLineNumber () << ":" << e.getColumnNumber ()
       << " " << (s == s_warning ? "warning: " : "error: ") << msg << endl;

  XMLString::release (&id);
  XMLString::release (&msg);
}
