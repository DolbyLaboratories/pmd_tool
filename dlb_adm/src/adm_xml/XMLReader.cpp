/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#include "XMLReader.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include "XMLContainer.h"
#include "RelationshipDescriptor.h"
#include "AttributeDescriptor.h"
#include "EntityRecord.h"
#include "dlb_adm/include/dlb_adm_api.h"

#include <string.h>
#include <algorithm>

#ifdef NDEBUG
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)
#else
static int retstat(int s)
{
    return s;   // Put a breakpoint here
}
#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return retstat(s)
#endif

namespace DlbAdm
{

    XMLReader::XMLReader(XMLContainer &container, FILE *f, bool isCommon)
        : mContainer(container)
        , mStack()
        , mInputFile(f)
        , mTraceFile(nullptr)
        , mBuffer()
        , mXmlTagState(XML_TAG_STATE::NOT_STARTED)
        , mIsCommon(isCommon)
     {
         Start();
     }

     XMLReader::XMLReader(XMLContainer &container, FILE *f, const char *traceFilePath, bool isCommon)
         : mContainer(container)
         , mStack()
         , mInputFile(f)
         , mTraceFile(nullptr)
         , mBuffer()
         , mXmlTagState(XML_TAG_STATE::NOT_STARTED)
         , mIsCommon(isCommon)
     {
         if (traceFilePath)
         {
             mTraceFile = ::fopen(traceFilePath, "w");
         }
         Start();
     }

     XMLReader::XMLReader(XMLContainer &container, const char *stringBuffer, size_t characterCount, bool isCommon)
         : mContainer(container)
         , mStack()
         , mInputFile(nullptr)
         , mTraceFile(nullptr)
         , mBuffer(stringBuffer, characterCount)
         , mXmlTagState(XML_TAG_STATE::NOT_STARTED)
         , mIsCommon(isCommon)
     {
         Start();
     }

     XMLReader::XMLReader(XMLContainer &container, const char *stringBuffer, size_t characterCount, const char *traceFilePath, bool isCommon)
         : mContainer(container)
         , mStack()
         , mInputFile(nullptr)
         , mTraceFile(nullptr)
         , mBuffer(stringBuffer, characterCount)
         , mXmlTagState(XML_TAG_STATE::NOT_STARTED)
         , mIsCommon(isCommon)
     {
         if (traceFilePath)
         {
             mTraceFile = ::fopen(traceFilePath, "w");
         }
         Start();
     }

     XMLReader::~XMLReader()
     {
         if (mTraceFile != nullptr)
         {
             ::fclose(mTraceFile);
             mTraceFile = nullptr;
         }
         mInputFile = nullptr;
     }

     char *XMLReader::GetLine()
     {
         char *s = nullptr;

         if (mInputFile != nullptr)
         {
             if (::feof(mInputFile))
             {
                 return nullptr;
             }
             s = ::fgets(mLineBuffer, LINE_BUFFER_SIZE, mInputFile);
         } 
         else
         {
             if (mBuffer.GetLine(mLineBuffer, LINE_BUFFER_SIZE) > 0)
             {
                 s = mLineBuffer;
             }
         }

         return s;
     }

     int XMLReader::Element(const char *tag, const char *text)
     {
         bool opening = (text == nullptr);
         bool handled = false;
         int status;

         // TODO: temporary hack for unhandled entities!
         {
             EntityDescriptor d;

             status = GetEntityDescriptor(d, tag);
             if (status == DLB_ADM_STATUS_NOT_FOUND)
             {
                 goto unhandled;
             }
         }

         if (mXmlTagState == XML_TAG_STATE::IN_PROGRESS && ::strcmp(tag, "xml") != 0)
         {
             mXmlTagState = XML_TAG_STATE::COMPLETE;
             status = Element("xml", "");
             CHECK_STATUS(status);
         }

         if (opening)
         {
             XMLReaderStackEntry *top;
             XMLReaderStackEntry next;
             RelationshipDescriptor rd;

             status = mStack.Top(top);
             CHECK_STATUS(status);
             status = GetEntityDescriptor(next.entityDescriptor, tag);
             CHECK_STATUS(status);
             if (!next.entityDescriptor.hasADMIdOrRef)
             {
                 status = MakeGenericComponentId(&next);
                 CHECK_STATUS(status);
             }

             status = GetRelationshipDescriptor(rd, top->entityDescriptor.entityType, next.entityDescriptor.entityType);
             CHECK_STATUS(status);
             switch (rd.relationship)
             {
             case ENTITY_RELATIONSHIP::CONTAINS:
                 if (next.idFinal)
                 {
                     status = mContainer.AddEntity(next.entityId);
                     CHECK_STATUS(status);
                     status = mContainer.AddRelationship(top->entityId, next.entityId);
                     CHECK_STATUS(status);
                 }
                 break;

             case ENTITY_RELATIONSHIP::REFERENCES:
                 break;

             default:
                 return DLB_ADM_STATUS_ERROR;
             }

             mStack.Push(next);
             handled = true;
         }
         else   // closing
         {
             XMLReaderStackEntry *child;
             XMLReaderStackEntry *parent;
             RelationshipDescriptor rd;

             status = mStack.Top2(child, parent);
             CHECK_STATUS(status);
             status = GetRelationshipDescriptor(rd, parent->entityDescriptor.entityType, child->entityDescriptor.entityType);
             CHECK_STATUS(status);
             switch (rd.relationship)
             {
             case ENTITY_RELATIONSHIP::CONTAINS:
                 if (!child->entityDescriptor.xmlTypeComposite &&
                      child->entityDescriptor.distinguishedTag != DLB_ADM_TAG_UNKNOWN)  // We have a value to process
                 {
                     status = SetValue(*child, child->entityDescriptor.distinguishedTag, text);
                     CHECK_STATUS(status);
                 }
                 if (mIsCommon)
                 {
                     status = mContainer.SetIsCommon(child->entityId);
                     CHECK_STATUS(status);
                 }
                 break;

             case ENTITY_RELATIONSHIP::REFERENCES:
                 status = dlb_adm_read_entity_id(&child->entityId, text, ::strlen(text) + 1);
                 CHECK_STATUS(status);
                 status = mContainer.AddEntity(child->entityId);
                 CHECK_STATUS(status);
                 status = mContainer.AddRelationship(parent->entityId, child->entityId);
                 CHECK_STATUS(status);
                 break;

             default:
                 return DLB_ADM_STATUS_ERROR;
             }

             status = mStack.Pop();
             CHECK_STATUS(status);
             handled = true;
         }

     unhandled:
         if (mTraceFile != nullptr && !handled)
         {
             if (opening)
             {
                 ::fprintf(mTraceFile, "OPEN('%s')\n", tag);
             }
             else
             {
                 ::fprintf(mTraceFile, "CLOSE('%s', '%s')\n", tag, text);
             }
         }

         return DLB_ADM_STATUS_OK;
     }

     int XMLReader::Attribute(const char *tag, const char *attribute, const char *value)
     {
         XMLReaderStackEntry *child;
         XMLReaderStackEntry *parent;
         AttributeDescriptor ad;
         bool handled = false;
         int status;

        // TODO: temporary hack for unhandled entities!
        {
            EntityDescriptor ed;

            status = GetEntityDescriptor(ed, tag);
            if (status == DLB_ADM_STATUS_NOT_FOUND)
            {
                goto unhandled;
            }
         }

        if (::strcmp(tag, "xml") == 0 && mXmlTagState == XML_TAG_STATE::NOT_STARTED)
        {
            status = Element(tag, nullptr);
            CHECK_STATUS(status);
            mXmlTagState = XML_TAG_STATE::IN_PROGRESS;
        }

        status = mStack.Top2(child, parent);
        CHECK_STATUS(status);
        if (std::string(tag) != child->entityDescriptor.name)
        {
            return DLB_ADM_STATUS_ERROR;
        }

        status = GetAttributeDescriptor(ad, child->entityDescriptor.entityType, attribute);
        if (status == DLB_ADM_STATUS_NOT_FOUND &&
            parent->entityDescriptor.entityType == DLB_ADM_ENTITY_TYPE_CHANGED_IDS &&
            ::strcmp(attribute, "status") == 0)
        {
            // Special case for "status" attribute for entities referred to by the changedIDs element
            // TODO: work out how to do something useful with the information
            handled = true;
        } 
        else if (status == DLB_ADM_STATUS_OK)
        {
            if (child->idFinal)
            {
                status = SetValue(*child, ad, value);
                CHECK_STATUS(status);
            }
            else
            {
                if (ad.attributeTag == child->entityDescriptor.distinguishedTag)
                {
                    status = dlb_adm_read_entity_id(&child->entityId, value, ::strlen(value) + 1);
                    CHECK_STATUS(status);
                    child->idFinal = true;
                    status = mContainer.AddEntity(child->entityId);
                    CHECK_STATUS(status);
                    status = mContainer.AddRelationship(parent->entityId, child->entityId);
                    CHECK_STATUS(status);
                    status = SetValue(*child, ad, value);
                    CHECK_STATUS(status);

                    try
                    {
                        std::for_each(
                            child->deferredAttributes.begin(),
                            child->deferredAttributes.end(),
                            [this, tag](const XMLReaderStackEntry::AttributeValueMap::value_type &kv)
                            {
                                DLB_ADM_STATUS s = static_cast<DLB_ADM_STATUS>(
                                    this->Attribute(tag, kv.first.c_str(), kv.second.c_str()));
                                if (s != DLB_ADM_STATUS_OK)
                                {
                                    throw s;
                                }
                            }
                        );
                    }
                    catch (DLB_ADM_STATUS &s)
                    {
                        return s;
                    }
                }
                else
                {
                    child->deferredAttributes[attribute] = value;
                }
            }
            handled = true;
        }
        else if (status != DLB_ADM_STATUS_NOT_FOUND)   // TODO: LATER this will be an error
        {
            return status;
        }

     unhandled:
         if (mTraceFile != nullptr && !handled)
         {
             ::fprintf(mTraceFile, "ATTR('%s', '%s', '%s')\n", tag, attribute, value);
         }

         return DLB_ADM_STATUS_OK;
     }

     void XMLReader::Start()
     {
         XMLReaderStackEntry topLevel;
         int status;

         topLevel.entityId = mContainer.GetTopLevelID();
         status = GetEntityDescriptor(topLevel.entityDescriptor, DLB_ADM_ENTITY_TYPE_TOPLEVEL);
         if (status != DLB_ADM_STATUS_OK)
         {
             // TODO: Throwing out of a constructor is not a great thing...
             throw DLB_ADM_STATUS_ERROR;
         }
         topLevel.idFinal = true;
         mStack.Push(topLevel);

         EntityRecord e;
         status = mContainer.GetEntity(e, topLevel.entityId);
         if (status == DLB_ADM_STATUS_NOT_FOUND)
         {
             status = mContainer.AddEntity(topLevel.entityId);
         }
     }

     int XMLReader::MakeGenericComponentId(XMLReaderStackEntry *component)
     {
         int status = DLB_ADM_STATUS_OK;
         AdmIdTranslator translator;

         component->entityId = mContainer.GetGenericID(component->entityDescriptor.entityType);
         if (component->entityId == DLB_ADM_NULL_ENTITY_ID)
         {
             status = DLB_ADM_STATUS_ERROR;
         }
         else
         {
             component->idFinal = true;
         }

         return status;
     }

     int XMLReader::SetValue(XMLReaderStackEntry &entry, DLB_ADM_TAG attributeTag, const std::string &valueString)
     {
         AttributeDescriptor d;
         int status;

         status = GetAttributeDescriptor(d, attributeTag);
         CHECK_STATUS(status);
         return SetValue(entry, d, valueString);
     }

     int XMLReader::SetValue(XMLReaderStackEntry &entry, const AttributeDescriptor &desc, const std::string &valueString)
     {
         if (!entry.idFinal)
         {
             return DLB_ADM_STATUS_ERROR;
         }

         AttributeValue v;
         int status;

         switch (desc.attributeValueType)
         {
         case DLB_ADM_VALUE_TYPE_BOOL:
         {
             dlb_adm_bool lv;
             status = ParseValue(lv, valueString);
             CHECK_STATUS(status);
             v = lv;
             break;
         }

         case DLB_ADM_VALUE_TYPE_UINT:
         {
             dlb_adm_uint lv;
             status = ParseValue(lv, valueString);
             CHECK_STATUS(status);
             v = lv;
             break;
         }

         case DLB_ADM_VALUE_TYPE_INT:
         {
             dlb_adm_int lv;
             status = ParseValue(lv, valueString);
             CHECK_STATUS(status);
             v = lv;
             break;
         }

         case DLB_ADM_VALUE_TYPE_FLOAT:
         {
             dlb_adm_float lv;
             status = ParseValue(lv, valueString);
             CHECK_STATUS(status);
             v = lv;
             break;
         }

         case DLB_ADM_VALUE_TYPE_AUDIO_TYPE:
         {
             DLB_ADM_AUDIO_TYPE lv;
             status = ParseValue(lv, valueString);
             CHECK_STATUS(status);
             v = lv;
             break;
         }

         case DLB_ADM_VALUE_TYPE_TIME:
         {
             dlb_adm_time lv;
             status = ParseValue(lv, valueString);
             CHECK_STATUS(status);
             v = lv;
             break;
         }

         case DLB_ADM_VALUE_TYPE_STRING:
         {
             v = valueString;
             break;
         }

         default:
             return DLB_ADM_STATUS_ERROR;
         }

         status = mContainer.SetValue(entry.entityId, desc.attributeTag, v);

         return status;
     }

 }
