#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CC = gcc
CCDEP = $(CC)
CFLAGS_debug = -msse2 -g -ggdb3 -O0 -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -msse2 -O3 -ftree-vectorize -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CXX = g++
CXXDEP = $(CXX)
CXXFLAGS_debug = -msse2 -g -ggdb3 -O0 -m32 -c -Wall -std=c++11
CXXFLAGS_release = -msse2 -O3 -ftree-vectorize -m32 -c -Wall -std=c++11
DEFINES_debug = -DHAVE_CONFIG_H=1 -DXERCES_BUILDING_LIBRARY=1
DEFINES_release = -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DXERCES_BUILDING_LIBRARY=1
INCLUDES = -I$(BASE)xerces/xerces-c-3.2.0/osx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter -I$(BASE)xerces/xerces-c-3.2.0/src -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom/impl -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/internal -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/parsers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax2 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/regx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/common -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/xinclude


INPUTS_ARCHIVE_debug = $(TMP)xerces/xerces-c-3.2.0/src/stricmp.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/strnicmp.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/towlower.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/towupper.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMException.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMLSException.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMRangeException.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMXPathException.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrMapImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCDATASectionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCharacterDataImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMChildNode.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCommentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMConfigurationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDeepNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentFragmentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentTypeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityReferenceImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMErrorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationRegistry.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSInputImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSOutputImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSSerializerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLocatorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNamedNodeMapImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIDMap.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIteratorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeVector.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNormalizer.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNotationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMParentNode.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMProcessingInstructionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMRangeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTextImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTreeWalkerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTypeInfoImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathExpressionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathNSResolverImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathResultImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/XSDElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/BinOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdInInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdOutFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/URLInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4DOMLSInput.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4InputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttr.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBuffer.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBufferMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLDTDDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLFormatter.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarPoolImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLNotationDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRecognizer.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRefInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLSchemaDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttribute.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttributeList.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIElement.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIItem.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAnnotation.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeUse.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSComplexTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSElementDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSFacet.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSIDCDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModel.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroup.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSMultiValueFacet.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNamespaceItem.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNotationDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSObject.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSParticle.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSSimpleTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSValue.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSWildcard.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinFileOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinMemOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/DGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ElemStack.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner2.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/MemoryManagerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ReaderMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/SGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ValidationContextImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttrListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttributesImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/WFXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLReader.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScannerResolver.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XProtoType.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSAXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSObjectFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSerializeEngine.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XTemplateSerializer.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/AbstractDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/DOMLSParserImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLFilterImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLReaderImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAXParser.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/XercesDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/Dummy.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/InputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXException.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXParseException.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax2/sax2Dummy.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Base64.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinFileInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinMemInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BitSet.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/DefaultPanicHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/EncodingValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers/PosixFileMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HeaderDummy.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HexBin.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/KVStringPair.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers/PosixMutexMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Mutexes.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PSVIUni.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PanicHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PlatformUtils.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/QName.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/StringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/SynchronizedStringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/TransService.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML256TableTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML88591Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLASCIITranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLAbstractDoubleFloat.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigDecimal.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigInteger.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChar.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDateTime.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDouble.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLEBCDICTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLException.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLFloat.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1047Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1140Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLInitializer.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLNumber.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLString.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLStringTokenizer.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUCS4Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLURL.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF16Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF8Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUni.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUri.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLWin1252Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMemory.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ASCIIRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BMPattern.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BlockRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/CharToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ClosureToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ConcatToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Match.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Op.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/OpFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParenToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParserForXMLSchema.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeTokenMap.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegularExpression.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxParser.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxUtil.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/StringToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Token.xerces_3_2_0_lib_osx_x86_gnu_debug.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/TokenFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnicodeRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnionToken.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLUniCharacter.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDGrammar.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/AllContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMAny.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMBinaryOp.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMUnaryOp.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentLeafNameTypeVector.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentSpecNode.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/DFAContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/Grammar.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/GrammarResolver.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/MixedContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/SimpleContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericFacetValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractStringValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnyURIDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/BooleanDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidatorFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DecimalDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DoubleDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DurationDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ENTITYDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/FloatDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDREFDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ListDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NCNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/QNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/StringDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/TimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/UnionDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/XMLCanRepGroup.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearMonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/ComplexTypeInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/GeneralAttributeCheck.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/NamespaceScope.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaGrammar.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaSymbols.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SubstitutionGroupComparator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/TraverseSchema.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDErrorReporter.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDLocator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XUtil.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesAttGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesElementWildcard.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldActivator.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldValueMap.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Field.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Key.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_KeyRef.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Selector.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Unique.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraint.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraintHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStore.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStoreCache.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcher.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcherStack.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathSymbols.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XercesXPath.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeLocation.xerces_3_2_0_lib_osx_x86_gnu_debug.o\
                       $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeUtils.xerces_3_2_0_lib_osx_x86_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)xerces/xerces-c-3.2.0/src/stricmp.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/strnicmp.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/towlower.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/towupper.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMLSException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMRangeException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMXPathException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrMapImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCDATASectionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCharacterDataImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMChildNode.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCommentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMConfigurationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDeepNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentFragmentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentTypeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityReferenceImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMErrorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationRegistry.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSInputImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSOutputImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSSerializerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLocatorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNamedNodeMapImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIDMap.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIteratorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeVector.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNormalizer.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNotationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMParentNode.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMProcessingInstructionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMRangeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringPool.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTextImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTreeWalkerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTypeInfoImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathExpressionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathNSResolverImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathResultImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/XSDElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/BinOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdInInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdOutFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/URLInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4DOMLSInput.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4InputSource.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttr.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBuffer.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBufferMgr.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLDTDDescription.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLFormatter.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarDescription.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarPoolImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLNotationDecl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRecognizer.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRefInfo.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLSchemaDescription.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttribute.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttributeList.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIElement.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIItem.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAnnotation.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeUse.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSComplexTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSElementDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSFacet.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSIDCDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModel.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroup.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSMultiValueFacet.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNamespaceItem.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNotationDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSObject.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSParticle.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSSimpleTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSValue.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSWildcard.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinFileOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinMemOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/DGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ElemStack.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner2.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/MemoryManagerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ReaderMgr.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/SGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ValidationContextImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttrListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttributesImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/WFXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLReader.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScannerResolver.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XProtoType.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSAXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSObjectFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSerializeEngine.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XTemplateSerializer.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/AbstractDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/DOMLSParserImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLFilterImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLReaderImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAXParser.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/XercesDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/Dummy.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/InputSource.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXParseException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax2/sax2Dummy.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Base64.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinFileInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinMemInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BitSet.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/DefaultPanicHandler.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/EncodingValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers/PosixFileMgr.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HeaderDummy.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HexBin.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/KVStringPair.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers/PosixMutexMgr.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Mutexes.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PSVIUni.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PanicHandler.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PlatformUtils.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/QName.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/StringPool.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/SynchronizedStringPool.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/TransService.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML256TableTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML88591Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLASCIITranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLAbstractDoubleFloat.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigDecimal.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigInteger.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChar.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDateTime.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDouble.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLEBCDICTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLException.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLFloat.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1047Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1140Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLInitializer.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLNumber.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLString.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLStringTokenizer.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUCS4Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLURL.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF16Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF8Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUni.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUri.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLWin1252Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMemory.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ASCIIRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BMPattern.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BlockRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/CharToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ClosureToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ConcatToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Match.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Op.xerces_3_2_0_lib_osx_x86_gnu_release.o $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/OpFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParenToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParserForXMLSchema.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeTokenMap.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegularExpression.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxParser.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxUtil.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/StringToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Token.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/TokenFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnicodeRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnionToken.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLUniCharacter.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDGrammar.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDScanner.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/AllContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMAny.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMBinaryOp.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMUnaryOp.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentLeafNameTypeVector.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentSpecNode.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/DFAContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/Grammar.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/GrammarResolver.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/MixedContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/SimpleContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericFacetValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractStringValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnyURIDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/BooleanDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidatorFactory.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DecimalDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DoubleDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DurationDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ENTITYDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/FloatDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDREFDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ListDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NCNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/QNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/StringDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/TimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/UnionDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/XMLCanRepGroup.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearMonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/ComplexTypeInfo.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/GeneralAttributeCheck.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/NamespaceScope.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaGrammar.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaInfo.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaSymbols.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaValidator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SubstitutionGroupComparator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/TraverseSchema.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDErrorReporter.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDLocator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XUtil.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesAttGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesElementWildcard.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldActivator.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldValueMap.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Field.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Key.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_KeyRef.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Selector.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Unique.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraint.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraintHandler.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStore.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStoreCache.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcher.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcherStack.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathSymbols.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XercesXPath.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeLocation.xerces_3_2_0_lib_osx_x86_gnu_release.o\
                         $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeUtils.xerces_3_2_0_lib_osx_x86_gnu_release.o


default: xerces_3_2_0_lib_debug.a xerces_3_2_0_lib_release.a

xerces_3_2_0_lib_debug.a: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

xerces_3_2_0_lib_release.a: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.xerces_3_2_0_lib_osx_x86_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.xerces_3_2_0_lib_osx_x86_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.xerces_3_2_0_lib_osx_x86_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.xerces_3_2_0_lib_osx_x86_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)xerces/xerces-c-3.2.0/src/stricmp.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/strnicmp.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/towlower.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/towupper.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMException.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMLSException.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMRangeException.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMXPathException.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrMapImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCDATASectionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCharacterDataImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMChildNode.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCommentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMConfigurationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDeepNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentFragmentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentTypeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityReferenceImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMErrorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationRegistry.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSInputImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSOutputImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSSerializerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLocatorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNamedNodeMapImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIDMap.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIteratorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeVector.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNormalizer.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNotationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMParentNode.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMProcessingInstructionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMRangeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTextImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTreeWalkerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTypeInfoImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathExpressionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathNSResolverImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathResultImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/XSDElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/BinOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdInInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdOutFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/URLInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4DOMLSInput.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4InputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttr.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBuffer.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBufferMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLDTDDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLFormatter.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarPoolImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLNotationDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRecognizer.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRefInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLSchemaDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttribute.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttributeList.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIElement.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIItem.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAnnotation.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeUse.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSComplexTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSElementDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSFacet.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSIDCDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroup.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSMultiValueFacet.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNamespaceItem.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNotationDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSObject.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSParticle.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSSimpleTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSValue.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSWildcard.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinFileOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinMemOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/DGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ElemStack.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner2.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/MemoryManagerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ReaderMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/SGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ValidationContextImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttrListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttributesImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/WFXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLReader.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScannerResolver.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XProtoType.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSAXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSObjectFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSerializeEngine.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XTemplateSerializer.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/AbstractDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/DOMLSParserImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLFilterImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLReaderImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAXParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/XercesDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/Dummy.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/InputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXException.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXParseException.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax2/sax2Dummy.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Base64.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinFileInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinMemInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BitSet.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/DefaultPanicHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/EncodingValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers/PosixFileMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HeaderDummy.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HexBin.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/KVStringPair.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers/PosixMutexMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Mutexes.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PSVIUni.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PanicHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PlatformUtils.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/QName.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/StringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/SynchronizedStringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/TransService.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML256TableTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML88591Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLASCIITranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLAbstractDoubleFloat.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigDecimal.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigInteger.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChar.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDateTime.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDouble.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLEBCDICTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLException.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLFloat.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1047Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1140Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLInitializer.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLNumber.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLString.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLStringTokenizer.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUCS4Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLURL.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF16Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF8Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUni.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUri.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLWin1252Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMemory.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ASCIIRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BMPattern.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BlockRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/CharToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ClosureToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ConcatToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Match.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Op.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/OpFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParenToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParserForXMLSchema.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeTokenMap.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegularExpression.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxUtil.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/StringToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Token.xerces_3_2_0_lib_osx_x86_gnu_debug.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/TokenFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnicodeRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnionToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLUniCharacter.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDGrammar.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/AllContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMAny.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMBinaryOp.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMUnaryOp.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentLeafNameTypeVector.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentSpecNode.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/DFAContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/Grammar.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/GrammarResolver.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/MixedContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/SimpleContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericFacetValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractStringValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnyURIDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/BooleanDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidatorFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DecimalDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DoubleDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DurationDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ENTITYDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/FloatDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDREFDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ListDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NCNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/QNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/StringDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/TimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/UnionDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/XMLCanRepGroup.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearMonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/ComplexTypeInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/GeneralAttributeCheck.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/NamespaceScope.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaGrammar.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaSymbols.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SubstitutionGroupComparator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/TraverseSchema.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDErrorReporter.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDLocator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XUtil.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesAttGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesElementWildcard.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldActivator.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldValueMap.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Field.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Key.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_KeyRef.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Selector.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Unique.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraint.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraintHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStore.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStoreCache.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcher.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcherStack.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathSymbols.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XercesXPath.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeLocation.xerces_3_2_0_lib_osx_x86_gnu_debug.d\
                      $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeUtils.xerces_3_2_0_lib_osx_x86_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)xerces/xerces-c-3.2.0/src/stricmp.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/strnicmp.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/towlower.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/towupper.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMLSException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMRangeException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMXPathException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrMapImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCDATASectionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCharacterDataImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMChildNode.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCommentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMConfigurationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDeepNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentFragmentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentTypeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityReferenceImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMErrorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationRegistry.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSInputImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSOutputImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSSerializerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLocatorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNamedNodeMapImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIDMap.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIteratorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeVector.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNormalizer.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNotationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMParentNode.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMProcessingInstructionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMRangeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringPool.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTextImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTreeWalkerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTypeInfoImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathExpressionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathNSResolverImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathResultImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/XSDElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/BinOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdInInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdOutFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/URLInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4DOMLSInput.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4InputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttr.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBuffer.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBufferMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLDTDDescription.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLFormatter.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarDescription.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarPoolImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLNotationDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRecognizer.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRefInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLSchemaDescription.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttribute.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttributeList.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIElement.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIItem.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAnnotation.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeUse.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSComplexTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSElementDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSFacet.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSIDCDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModel.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroup.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSMultiValueFacet.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNamespaceItem.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNotationDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSObject.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSParticle.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSSimpleTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSValue.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSWildcard.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinFileOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinMemOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/DGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ElemStack.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner2.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/MemoryManagerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ReaderMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/SGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ValidationContextImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttrListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttributesImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/WFXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLReader.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScannerResolver.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XProtoType.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSAXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSObjectFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSerializeEngine.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XTemplateSerializer.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/AbstractDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/DOMLSParserImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLFilterImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLReaderImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAXParser.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/XercesDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/Dummy.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/InputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXParseException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax2/sax2Dummy.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Base64.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinFileInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinMemInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BitSet.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/DefaultPanicHandler.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/EncodingValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers/PosixFileMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HeaderDummy.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HexBin.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/KVStringPair.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers/PosixMutexMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Mutexes.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PSVIUni.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PanicHandler.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PlatformUtils.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/QName.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/StringPool.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/SynchronizedStringPool.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/TransService.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML256TableTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML88591Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLASCIITranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLAbstractDoubleFloat.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigDecimal.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigInteger.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChar.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDateTime.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDouble.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLEBCDICTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLException.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLFloat.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1047Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1140Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLInitializer.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLNumber.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLString.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLStringTokenizer.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUCS4Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLURL.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF16Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF8Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUni.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUri.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLWin1252Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMemory.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ASCIIRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BMPattern.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BlockRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/CharToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ClosureToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ConcatToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Match.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Op.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/OpFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParenToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParserForXMLSchema.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeTokenMap.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegularExpression.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxParser.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxUtil.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/StringToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Token.xerces_3_2_0_lib_osx_x86_gnu_release.d $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/TokenFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnicodeRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnionToken.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLUniCharacter.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDGrammar.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/AllContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMAny.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMBinaryOp.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMUnaryOp.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentLeafNameTypeVector.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentSpecNode.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/DFAContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/Grammar.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/GrammarResolver.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/MixedContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/SimpleContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericFacetValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractStringValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnyURIDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/BooleanDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidatorFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DecimalDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DoubleDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DurationDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ENTITYDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/FloatDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDREFDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ListDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NCNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/QNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/StringDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/TimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/UnionDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/XMLCanRepGroup.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearMonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/ComplexTypeInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/GeneralAttributeCheck.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/NamespaceScope.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaGrammar.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaSymbols.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SubstitutionGroupComparator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/TraverseSchema.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDErrorReporter.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDLocator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XUtil.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesAttGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesElementWildcard.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldActivator.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldValueMap.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Field.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Key.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_KeyRef.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Selector.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Unique.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraint.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraintHandler.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStore.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStoreCache.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcher.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcherStack.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathSymbols.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XercesXPath.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeLocation.xerces_3_2_0_lib_osx_x86_gnu_release.d\
                        $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeUtils.xerces_3_2_0_lib_osx_x86_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) xerces_3_2_0_lib_debug.a
	$(RM) xerces_3_2_0_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " xerces_3_2_0_lib_debug.a"
	@echo " xerces_3_2_0_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/stricmp.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/strnicmp.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/towlower.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/towupper.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMLSException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMRangeException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMXPathException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrMapImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCDATASectionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCharacterDataImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMChildNode.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCommentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMConfigurationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDeepNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentFragmentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentTypeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityReferenceImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMErrorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationRegistry.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSInputImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSOutputImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSSerializerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLocatorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNamedNodeMapImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIDMap.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIteratorImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeVector.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNormalizer.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNotationImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMParentNode.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMProcessingInstructionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMRangeImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTextImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTreeWalkerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTypeInfoImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathExpressionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathNSResolverImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathResultImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/XSDElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/BinOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdInInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdOutFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/URLInputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4DOMLSInput.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4InputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttr.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBuffer.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBufferMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLDTDDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLFormatter.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarPoolImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLNotationDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRecognizer.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRefInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLSchemaDescription.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttribute.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttributeList.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIElement.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIItem.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAnnotation.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeUse.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSComplexTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSElementDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSFacet.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSIDCDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroup.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSMultiValueFacet.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNamespaceItem.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNotationDeclaration.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSObject.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSParticle.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSSimpleTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSValue.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSWildcard.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinFileOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinMemOutputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/DGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ElemStack.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner2.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/MemoryManagerImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ReaderMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/SGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ValidationContextImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttrListImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttributesImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/WFXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLReader.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScannerResolver.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XProtoType.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSAXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSObjectFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSerializeEngine.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XTemplateSerializer.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/AbstractDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/DOMLSParserImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLFilterImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLReaderImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAXParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/XercesDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/Dummy.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/InputSource.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXParseException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax2/sax2Dummy.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Base64.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinFileInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinMemInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BitSet.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/DefaultPanicHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/EncodingValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers/PosixFileMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HeaderDummy.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HexBin.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/KVStringPair.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers/PosixMutexMgr.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Mutexes.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PSVIUni.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PanicHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PlatformUtils.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/QName.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/StringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/SynchronizedStringPool.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/TransService.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML256TableTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML88591Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLASCIITranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLAbstractDoubleFloat.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigDecimal.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigInteger.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChar.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDateTime.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDouble.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLEBCDICTranscoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLException.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLFloat.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1047Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1140Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLInitializer.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLNumber.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLString.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLStringTokenizer.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUCS4Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLURL.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF16Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF8Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUni.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUri.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLWin1252Transcoder.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMemory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ASCIIRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BMPattern.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BlockRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/CharToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ClosureToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ConcatToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Match.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Op.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/OpFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParenToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParserForXMLSchema.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeTokenMap.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegularExpression.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxUtil.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/StringToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Token.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/TokenFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnicodeRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnionToken.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLUniCharacter.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDGrammar.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDScanner.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/AllContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMAny.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMBinaryOp.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMUnaryOp.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentLeafNameTypeVector.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentSpecNode.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/DFAContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/Grammar.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/GrammarResolver.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/MixedContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/SimpleContentModel.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericFacetValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractStringValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnyURIDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/BooleanDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidatorFactory.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DecimalDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DoubleDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DurationDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ENTITYDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/FloatDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDREFDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ListDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NCNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/QNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/StringDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/TimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/UnionDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/XMLCanRepGroup.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearMonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/ComplexTypeInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/GeneralAttributeCheck.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/NamespaceScope.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDef.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDefList.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaElementDecl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaGrammar.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaSymbols.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaValidator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SubstitutionGroupComparator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/TraverseSchema.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDDOMParser.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDErrorReporter.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDLocator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XUtil.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesAttGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesElementWildcard.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldActivator.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldValueMap.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Field.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Key.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_KeyRef.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Selector.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Unique.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraint.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraintHandler.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStore.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStoreCache.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcher.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcherStack.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathSymbols.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XercesXPath.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeLocation.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeUtils.xerces_3_2_0_lib_osx_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/stricmp.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/strnicmp.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/towlower.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/towupper.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMLSException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMRangeException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/DOMXPathException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrMapImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMAttrNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCDATASectionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCharacterDataImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMChildNode.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMCommentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMConfigurationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDeepNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentFragmentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMDocumentTypeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMEntityReferenceImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMErrorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMImplementationRegistry.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSInputImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSOutputImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLSSerializerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMLocatorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNamedNodeMapImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIDMap.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeIteratorImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNodeVector.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNormalizer.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMNotationImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMParentNode.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMProcessingInstructionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMRangeImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMStringPool.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTextImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTreeWalkerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMTypeInfoImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathExpressionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathNSResolverImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/DOMXPathResultImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/dom/impl/XSDElementNSImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/BinOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/LocalFileInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/MemBufInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdInInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/StdOutFormatTarget.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/URLInputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4DOMLSInput.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/Wrapper4InputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLAttr.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBuffer.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLBufferMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLDTDDescription.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLFormatter.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarDescription.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLGrammarPoolImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLNotationDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRecognizer.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLRefInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLSchemaDescription.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/XMLValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttribute.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIAttributeList.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIElement.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/PSVIItem.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAnnotation.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSAttributeUse.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSComplexTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSElementDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSFacet.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSIDCDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModel.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroup.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSModelGroupDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSMultiValueFacet.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNamespaceItem.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSNotationDeclaration.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSObject.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSParticle.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSSimpleTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSTypeDefinition.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSValue.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi/XSWildcard.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinFileOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/BinMemOutputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/DGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ElemStack.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/IGXMLScanner2.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/MemoryManagerImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ReaderMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/SGXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/ValidationContextImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttrListImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/VecAttributesImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/WFXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLReader.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XMLScannerResolver.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XProtoType.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSAXMLScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSObjectFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XSerializeEngine.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/internal/XTemplateSerializer.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/AbstractDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/DOMLSParserImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLFilterImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAX2XMLReaderImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/SAXParser.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/parsers/XercesDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/Dummy.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/InputSource.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax/SAXParseException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/sax2/sax2Dummy.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Base64.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinFileInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BinMemInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/BitSet.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/DefaultPanicHandler.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/EncodingValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers/PosixFileMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HeaderDummy.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/HexBin.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/KVStringPair.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers/PosixMutexMgr.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Mutexes.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PSVIUni.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PanicHandler.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/PlatformUtils.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/QName.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/StringPool.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/SynchronizedStringPool.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/TransService.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML256TableTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XML88591Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLASCIITranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLAbstractDoubleFloat.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigDecimal.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLBigInteger.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLChar.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDateTime.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLDouble.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLEBCDICTranscoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLException.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLFloat.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1047Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLIBM1140Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLInitializer.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLMsgLoader.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLNumber.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLString.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLStringTokenizer.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUCS4Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLURL.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF16Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUTF8Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUni.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLUri.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMLWin1252Transcoder.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/XMemory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ASCIIRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BMPattern.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/BlockRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/CharToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ClosureToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ConcatToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Match.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Op.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/OpFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParenToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/ParserForXMLSchema.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RangeTokenMap.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegularExpression.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxParser.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/RegxUtil.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/StringToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/Token.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/TokenFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnicodeRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/UnionToken.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLRangeFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/util/regx/XMLUniCharacter.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDEntityDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDGrammar.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDScanner.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/DTDValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/AllContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMAny.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMBinaryOp.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/CMUnaryOp.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentLeafNameTypeVector.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/ContentSpecNode.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/DFAContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/Grammar.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/GrammarResolver.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/MixedContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/common/SimpleContentModel.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericFacetValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractNumericValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AbstractStringValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/AnyURIDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/BooleanDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DatatypeValidatorFactory.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DateTimeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DecimalDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DoubleDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/DurationDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ENTITYDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/FloatDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/IDREFDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/ListDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/MonthDayDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NCNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/NameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/QNameDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/StringDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/TimeDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/UnionDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/XMLCanRepGroup.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype/YearMonthDatatypeValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/ComplexTypeInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/GeneralAttributeCheck.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/NamespaceScope.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDef.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaAttDefList.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaElementDecl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaGrammar.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaSymbols.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SchemaValidator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/SubstitutionGroupComparator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/TraverseSchema.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDDOMParser.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDErrorReporter.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XSDLocator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XUtil.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesAttGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesElementWildcard.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/XercesGroupInfo.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldActivator.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/FieldValueMap.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Field.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Key.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_KeyRef.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Selector.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IC_Unique.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraint.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/IdentityConstraintHandler.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStore.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/ValueStoreCache.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcher.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathMatcherStack.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XPathSymbols.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity/XercesXPath.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeLocation.xerces_3_2_0_lib_osx_x86_gnu_release.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/src/xercesc/xinclude/XIncludeUtils.xerces_3_2_0_lib_osx_x86_gnu_release.d)


