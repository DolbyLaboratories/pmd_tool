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

#include "XMLContainer.h"
#include "dlb_adm/src/dlb_adm_api_pvt.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include "dlb_adm/src/adm_identity/AdmIdSequenceMap.h"
#include "XMLReader.h"
#include "XMLWriter.h"

#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

#include "dlb_xml/include/dlb_xml.h"

#ifdef __cplusplus
}
#endif

#include <cstdio>
#include <map>
#include <iostream>

namespace DlbAdm
{

    XMLContainer::XMLContainer()
        : mEntityDB()
        , mRelationshipDB()
        , mSequenceMap(new AdmIdSequenceMap())
    {
        // Empty
    }

    XMLContainer::~XMLContainer()
    {
        // Empty
    }

    int XMLContainer::AddEntity(const dlb_adm_entity_id &id)
    {
        return mEntityDB.Add(id);
    }

    int XMLContainer::GetEntity(DlbAdm::EntityRecord &e, const dlb_adm_entity_id &id)
    {
        return mEntityDB.Get(e, id);
    }

    int XMLContainer::AddRelationship(const dlb_adm_entity_id &fromID, const dlb_adm_entity_id &toID)
    {
        return mRelationshipDB.Add(fromID, toID);
    }

    int XMLContainer::SetValue(const dlb_adm_entity_id &id, DLB_ADM_TAG tag, const DlbAdm::AttributeValue &value)
    {
        return mEntityDB.SetValue(id, tag, value);
    }

    int XMLContainer::GetValue(AttributeValue &value, const dlb_adm_entity_id &id, DLB_ADM_TAG tag) const
    {
        return mEntityDB.GetValue(value, id, tag);
    }

    int XMLContainer::SetMutable(const dlb_adm_entity_id &id, dlb_adm_bool isMutable)
    {
        return mEntityDB.SetMutable(id, isMutable);
    }

    int XMLContainer::SetIsCommon(const dlb_adm_entity_id &id)
    {
        return mEntityDB.SetIsCommon(id);
    }

    int XMLContainer::ForEachEntity(DLB_ADM_ENTITY_TYPE entityType, EntityDB::EntityCallbackFn callbackFn, EntityDB::EntityFilterFn filterFn)
    {
        return mEntityDB.ForEach(entityType, callbackFn, filterFn);
    }

    int XMLContainer::ForEachRelationship(RelationshipDB::RelationshipCallbackFn callbackFn)
    {
        return mRelationshipDB.ForEach(callbackFn);
    }

    int XMLContainer::ForEachRelationship(const dlb_adm_entity_id &id, ENTITY_RELATIONSHIP r, RelationshipDB::RelationshipCallbackFn callbackFn, RelationshipDB::RelationshipFilterFn filterFn)
    {
        return mRelationshipDB.ForEach(id, r, callbackFn, filterFn);
    }

    int XMLContainer::ForEachRelationship(const dlb_adm_entity_id &id, DLB_ADM_ENTITY_TYPE entityType, RelationshipDB::RelationshipCallbackFn callbackFn, RelationshipDB::RelationshipFilterFn filterFn)
    {
        return mRelationshipDB.ForEach(id, entityType, callbackFn, filterFn);
    }

    bool XMLContainer::RelationshipExists(const dlb_adm_entity_id &fromID, const dlb_adm_entity_id &toID)
    {
	return mRelationshipDB.Exists(fromID, toID);
    }
    
    bool XMLContainer::RelationshipExists(const dlb_adm_entity_id &id, DLB_ADM_ENTITY_TYPE entityType)
    {
        return mRelationshipDB.Exists(id, entityType);
    }

    size_t XMLContainer::RelationshipCount(const dlb_adm_entity_id &id, DLB_ADM_ENTITY_TYPE entityType)
    {
        return mRelationshipDB.Count(id, entityType);
    }

    int XMLContainer::ForEachAttribute(const dlb_adm_entity_id &id, EntityDB::AttributeCallbackFn callbackFn)
    {
        return mEntityDB.ForEach(id, callbackFn);
    }


    /* XML Reader */

    static char *LineCallback(void *p_context)
    {
        return reinterpret_cast<XMLReader *>(p_context)->GetLine();
    }

    static int ElementCallback(void *p_context, char *tag, char *text)
    {
        int status = reinterpret_cast<XMLReader *>(p_context)->Element(tag, text);
        return status;
    }

    static int AttributeCallback(void *p_context, char *tag, char *attribute, char *value)
    {
        int status = reinterpret_cast<XMLReader *>(p_context)->Attribute(tag, attribute, value);
        return status;
    }

    int XMLContainer::ReadXmlBuffer(const char *xmlBuffer, size_t characterCount, dlb_adm_bool useCommonDefs)
    {
        if (xmlBuffer == nullptr)
        {
            return DLB_ADM_STATUS_NULL_POINTER;
        }

        if (characterCount == 0)
        {
            return DLB_ADM_STATUS_OUT_OF_RANGE;
        }

        if (useCommonDefs)
        {
            int status = LoadCommonDefs();

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        XMLReader reader(*this, xmlBuffer, characterCount);
        int status = ::dlb_xml_parse(&reader, &LineCallback, &ElementCallback, &AttributeCallback);

        return status ? DLB_ADM_STATUS_ERROR : DLB_ADM_STATUS_OK;
    }

    int XMLContainer::ReadXmlFile(const char *filePath, dlb_adm_bool useCommonDefs)
    {
        if (filePath == nullptr)
        {
            return DLB_ADM_STATUS_NULL_POINTER;
        }

        if (useCommonDefs)
        {
            int status = LoadCommonDefs();

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        FILE *f = ::fopen(filePath, "r");   // TODO: use C++ istream?

        if (f == nullptr)
        {
            return DLB_ADM_STATUS_NOT_FOUND;
        }

        int status;
        XMLReader reader(*this, f);

        status = ::dlb_xml_parse(&reader, &LineCallback, &ElementCallback, &AttributeCallback);
        fclose(f);

        return status ? DLB_ADM_STATUS_ERROR : DLB_ADM_STATUS_OK;
    }

    int XMLContainer::WriteXmlBuffer(dlb_adm_write_buffer_callback bufferCallback, void *callbackArg)
    {
        XMLWriter writer(bufferCallback, callbackArg, *this);
        int status = writer.Write();
        return status;
    }

    // Helpers for WriteXmlFile() (borrowed from dlb_sadm_write_file.c)

    struct xml_buffer
    {
        FILE *fp;
        char line[4096];
    };

    static
    int
    get_buffer
        (void *arg
        ,char *pos
        ,char **buf
        ,size_t *capacity
        )
    {
        xml_buffer *xbuf = (xml_buffer *)arg;

        if (NULL == xbuf->fp)
        {
            return 0;
        }
    
        if (NULL != pos)
        {
            ptrdiff_t len = pos - xbuf->line;
            if (len < 0 || len > (ptrdiff_t)sizeof(xbuf->line))
            {
                return 0;
            }

            fwrite(xbuf->line, 1, len, xbuf->fp);
        }
    
        if (NULL != buf)
        {
            *buf = xbuf->line;
            *capacity = sizeof(xbuf->line);
        }

        return 1;
    }

    int XMLContainer::WriteXmlFile(const char *filePath)
    {
        int status = DLB_ADM_STATUS_ERROR;
        xml_buffer xbuf;

        ::memset(&xbuf, 0, sizeof(xbuf));
        xbuf.fp = ::fopen(filePath, "w");
        if (xbuf.fp != nullptr)
        {
            XMLWriter writer(get_buffer, &xbuf, *this);
            status = writer.Write();
            ::fclose(xbuf.fp);
        }

        return status;
    }

    dlb_adm_entity_id XMLContainer::GetTopLevelID()
    {
        return AdmIdTranslator().ConstructGenericId(DLB_ADM_ENTITY_TYPE_TOPLEVEL, 0x1001);
    }

    dlb_adm_entity_id XMLContainer::GetGenericID(DLB_ADM_ENTITY_TYPE entityType)
    {
        return AdmIdTranslator().ConstructGenericId(entityType, mSequenceMap->GetSequenceNumber(entityType));
    }

    int XMLContainer::LoadCommonDefs()
    {
        const char *filePath = dlb_adm_get_common_defs_path();
        FILE *f = ::fopen(filePath, "r");

        if (f == nullptr)
        {
            return DLB_ADM_STATUS_NOT_FOUND;
        }
        int status;
        XMLReader reader(*this, f, "C:\\temp\\trace_common.out", true);

        status = ::dlb_xml_parse(&reader, &LineCallback, &ElementCallback, &AttributeCallback);
        fclose(f);

        return status ? DLB_ADM_STATUS_ERROR : DLB_ADM_STATUS_OK;
    }

}
