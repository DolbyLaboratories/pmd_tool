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

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm/src/adm_xml/dlb_adm_xml_container.h"
#include "dlb_adm/src/adm_xml/AttributeValue.h"
#include "dlb_adm/src/adm_xml/AttributeDescriptor.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"
#include "dlb_adm/src/core_model/dlb_adm_core_model.h"

#include "dlb_adm/src/core_model/Presentation.h"
#include "dlb_adm/src/core_model/ContentGroup.h"
#include "dlb_adm/src/core_model/ElementGroup.h"
#include "dlb_adm/src/core_model/AudioElement.h"
#include "dlb_adm/src/core_model/AudioTrack.h"
#include "dlb_adm/src/core_model/TargetGroup.h"
#include "dlb_adm/src/core_model/Target.h"
#include "dlb_adm/src/core_model/SourceGroup.h"
#include "dlb_adm/src/core_model/Source.h"
#include "dlb_adm/src/core_model/BlockUpdate.h"
#include "dlb_adm/src/core_model/FrameFormat.h"
#include "dlb_adm/src/core_model/SourceRecord.h"
#include "dlb_adm/src/core_model/UpdateRecord.h"
#include "dlb_adm/src/core_model/ElementRecord.h"
#include "dlb_adm/src/core_model/PresentationRecord.h"

#include "dlb_adm/src/adm_transformer/XMLIngester.h"
#include "dlb_adm/src/adm_transformer/XMLGenerator.h"

#include <functional>

using namespace DlbAdm;

void CheckStatus(int status)
{
    if (status != DLB_ADM_STATUS_OK)
    {
        throw static_cast<DLB_ADM_STATUS>(status);
    }
}

typedef std::function<int()> const& ActionFn;

static
int
unwind_protect
    (ActionFn fn
    )
{
    int status;

    try
    {
        status = fn();
    }
    catch (DLB_ADM_STATUS &s)
    {
        status = s;
    }
    catch (...)
    {
        status = DLB_ADM_STATUS_EXCEPTION;
    }

    return status;
}

int
dlb_adm_read_entity_id
    (dlb_adm_entity_id          *id
    ,const char                 *s
    ,size_t                      len
    )
{
    if ((id == nullptr) || (s == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *id = DLB_ADM_NULL_ENTITY_ID;

    if (len < ADM_ID_MIN_LEN + 1)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        int status = DLB_ADM_STATUS_OK;
        AdmIdTranslator translator;

        *id = translator.Translate(s);
        if (*id == DLB_ADM_NULL_ENTITY_ID)
        {
            status = DLB_ADM_STATUS_INVALID_ARGUMENT;
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_write_entity_id
    (char                       *s
    ,size_t                      len
    ,dlb_adm_entity_id           id
    )
{
    if (s == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (len < ADM_ID_MAX_LEN + 1)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }
    *s = '\0';

    ActionFn f = [&]
    {
        int status = DLB_ADM_STATUS_OK;
        AdmIdTranslator translator;
        std::string str;

        str = translator.Translate(id);
        if (str.empty())
        {
            status = DLB_ADM_STATUS_INVALID_ARGUMENT;
        }
        else
        {
            ::strncpy(s, str.data(), len);
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_construct_generic_entity_id
    (dlb_adm_entity_id          *id
    ,DLB_ADM_ENTITY_TYPE         t
    ,uint32_t                    n
    )
{
    if (id == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *id = DLB_ADM_NULL_ENTITY_ID;

    if (n == 0)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        int status = DLB_ADM_STATUS_OK;
        AdmIdTranslator translator;

        *id = translator.ConstructGenericId(t, n);
        if (*id == DLB_ADM_NULL_ENTITY_ID)
        {
            status = DLB_ADM_STATUS_ERROR;
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_get_attribute_tag
    (DLB_ADM_TAG                *tag
    ,DLB_ADM_ENTITY_TYPE         entity_type
    ,const char                 *attribute_name
    )
{
    AttributeDescriptor d;

    if ((tag == nullptr) || (attribute_name == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *tag = DLB_ADM_TAG_UNKNOWN;

    if ((entity_type < DLB_ADM_ENTITY_TYPE_FIRST_WITH_ID) || (entity_type > DLB_ADM_ENTITY_TYPE_LAST))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        int status = GetAttributeDescriptor(d, entity_type, attribute_name);

        if (status == DLB_ADM_STATUS_OK)
        {
            *tag = d.attributeTag;
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_get_attribute_value_type
    (DLB_ADM_VALUE_TYPE     *value_type
    ,DLB_ADM_TAG             tag)
{
    AttributeDescriptor d;

    if (value_type == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    if ((tag < DLB_ADM_TAG_FIRST) || (tag > DLB_ADM_TAG_LAST))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        int status = GetAttributeDescriptor(d, tag);

        if (status == DLB_ADM_STATUS_OK)
        {
            *value_type = d.attributeValueType;
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_container_query_memory_size
    (size_t                         *sz
    ,const dlb_adm_container_counts *counts
    )
{
    (void)counts;
    (void)sz;
    return DLB_ADM_STATUS_OUT_OF_MEMORY;    // TODO: External memory allocation not yet implemented!
}

int
dlb_adm_container_open
    (dlb_adm_xml_container             **p_container
    ,const dlb_adm_container_counts     *counts
    )
{
    if ((p_container == nullptr) || (counts == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *p_container = nullptr;

    ActionFn f = [&]
    {
        dlb_adm_xml_container *container = new dlb_adm_xml_container(counts);
        if (container == nullptr)
        {
            return DLB_ADM_STATUS_OUT_OF_MEMORY;
        }
        *p_container = container;

        return DLB_ADM_STATUS_OK;
    };

    return unwind_protect(f);
}

int
dlb_adm_container_open_from_core_model
    (dlb_adm_xml_container             **p_container
    ,const dlb_adm_core_model           *core_model
    )
{
    dlb_adm_container_counts counts;

    if ((p_container == nullptr) || (core_model == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *p_container = nullptr;
    ::memset(&counts, 0, sizeof(counts));   // LATER: calculate from core_model

    ActionFn f = [&]
    {
        dlb_adm_xml_container *container = new dlb_adm_xml_container(&counts);
        int status;

        if (container == nullptr)
        {
            return static_cast<int>(DLB_ADM_STATUS_OUT_OF_MEMORY);
        }
        *p_container = container;

        XMLGenerator generator(container, core_model);

        status = generator.GenerateFrame();
        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_container_close
    (dlb_adm_xml_container     **p_container
    )
{
    if ((p_container == nullptr) || (*p_container == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        delete *p_container;
        *p_container = nullptr;

        return DLB_ADM_STATUS_OK;
    };

    return unwind_protect(f);
}

int
dlb_adm_container_add_reference
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    )
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    return unwind_protect([&] { return container->GetContainer().AddEntity(id); });
}

int
dlb_adm_container_add_relationship
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           from_id
    ,dlb_adm_entity_id           to_id
    )
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((from_id == DLB_ADM_NULL_ENTITY_ID) || (to_id == DLB_ADM_NULL_ENTITY_ID))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    return unwind_protect([&] { return container->GetContainer().AddRelationship(from_id, to_id); });
}


// Attribute value setters

static int
SetValue
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,const AttributeValue       &value
    )
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    return unwind_protect([&] { return container->GetContainer().SetValue(id, tag, value); });
}

int
dlb_adm_container_set_bool_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_bool                value
    )
{
    return SetValue(container, id, tag, AttributeValue(value));
}

int
dlb_adm_container_set_uint_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_uint                value
    )
{
    return SetValue(container, id, tag, AttributeValue(value));
}

int
dlb_adm_container_set_int_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_int                 value
    )
{
    return SetValue(container, id, tag, AttributeValue(value));
}

int
dlb_adm_container_set_float_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,dlb_adm_float               value
    )
{
    return SetValue(container, id, tag, AttributeValue(value));
}

int
dlb_adm_container_set_audio_type_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,DLB_ADM_AUDIO_TYPE          value
    )
{
    return SetValue(container, id, tag, AttributeValue(value));
}

int
dlb_adm_container_set_time_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,const dlb_adm_time         *value
    )
{
    if (value == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return SetValue(container, id, tag, AttributeValue(*value));
}

int
dlb_adm_container_set_string_value
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    ,const char                 *value
    )
{
    if (value == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return SetValue(container, id, tag, AttributeValue(std::string(value)));
}


// Attribute value getters

static
int
GetValue
    (AttributeValue             &value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag)
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    return container->GetContainer().GetValue(value, id, tag);
}

template <typename T>
int
GetTypedValue
    (T                          *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    if (value == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        AttributeValue v;
        int status = GetValue(v, container, id, tag);

        if (status == DLB_ADM_STATUS_OK)
        {
            T *pv = boost::get<T>(&v);

            if (pv == nullptr)
            {
                status = DLB_ADM_STATUS_VALUE_TYPE_MISMATCH;
            }
            else
            {
                *value = *pv;
            }
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_container_get_bool_value
    (dlb_adm_bool               *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    return GetTypedValue(value, container, id, tag);
}

int
dlb_adm_container_get_uint_value
    (dlb_adm_uint               *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    return GetTypedValue(value, container, id, tag);
}

int
dlb_adm_container_get_int_value
    (dlb_adm_int                *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    return GetTypedValue(value, container, id, tag);
}

int
dlb_adm_container_get_float_value
    (dlb_adm_float              *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    return GetTypedValue(value, container, id, tag);
}

int
dlb_adm_container_get_audio_type_value
    (DLB_ADM_AUDIO_TYPE         *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    return GetTypedValue(value, container, id, tag);
}

int
dlb_adm_container_get_time_value
    (dlb_adm_time               *value
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    return GetTypedValue(value, container, id, tag);
}

int
dlb_adm_container_get_string_value
    (char                       *buffer
    ,size_t                      buffer_size
    ,const dlb_adm_xml_container    *container
    ,dlb_adm_entity_id           id
    ,DLB_ADM_TAG                 tag
    )
{
    if (buffer == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    if (buffer_size < DLB_ADM_STRING_VALUE_BUFFER_MIN_SIZE)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        AttributeValue v;
        int status = GetValue(v, container, id, tag);

        if (status == DLB_ADM_STATUS_OK)
        {
            std::string *pv = boost::get<std::string>(&v);

            if (pv == nullptr)
            {
                status = DLB_ADM_STATUS_VALUE_TYPE_MISMATCH;
            }
            else
            {
                size_t len = buffer_size - 1;

                buffer[len] = '\0';
                ::strncpy(buffer, pv->data(), len);
            }
        }

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_container_set_mutable
    (dlb_adm_xml_container      *container
    ,dlb_adm_entity_id           id
    ,dlb_adm_bool                is_mutable
    )
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    return unwind_protect([&] { return container->GetContainer().SetMutable(id, is_mutable); });
}


// Clear a container

int
dlb_adm_container_clear
    (dlb_adm_xml_container      *container
    );

int
dlb_adm_container_clear_all
    (dlb_adm_xml_container      *container
    );


/* XML Reader */

int
dlb_adm_container_read_xml_buffer
    (dlb_adm_xml_container      *container
    ,const char                 *xml_buffer
    ,size_t                      character_count
    ,dlb_adm_bool                use_common_defs
    )
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return unwind_protect([&] { return container->GetContainer().ReadXmlBuffer(xml_buffer, character_count, use_common_defs); });
}

int
dlb_adm_container_read_xml_file
    (dlb_adm_xml_container      *container
    ,const char                 *file_path
    ,dlb_adm_bool                use_common_defs
    )
{
    if (container == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return unwind_protect([&] { return container->GetContainer().ReadXmlFile(file_path, use_common_defs); });
}


/* XML Writer */

int
dlb_adm_container_write_xml_buffer
    (dlb_adm_xml_container          *container
    ,dlb_adm_write_buffer_callback   callback
    ,void                           *callback_arg
    )
{
    if ((container == nullptr) || (callback == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return unwind_protect([&] { return container->GetContainer().WriteXmlBuffer(callback, callback_arg); });
}

int
dlb_adm_container_write_xml_file
    (dlb_adm_xml_container      *container
    ,const char                 *file_path
    )
{
    if ((container == nullptr) || (file_path == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return unwind_protect([&] { return container->GetContainer().WriteXmlFile(file_path); });
}


/* ADM core model */

int
dlb_adm_core_model_query_memory_size
    (size_t                             *sz
    ,const dlb_adm_core_model_counts    *counts
    )
{
    if ((sz == nullptr) || (counts == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *sz = 0;

    return DLB_ADM_STATUS_OUT_OF_MEMORY;
}

int
dlb_adm_core_model_open
    (dlb_adm_core_model                **p_model
    ,const dlb_adm_core_model_counts    *counts
    )
{
    if ((p_model == nullptr) || (counts == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *p_model = nullptr;

    ActionFn f = [&]
    {
        dlb_adm_core_model *model = new dlb_adm_core_model(counts);
        if (model == nullptr)
        {
            return DLB_ADM_STATUS_OUT_OF_MEMORY;
        }
        *p_model = model;

        return DLB_ADM_STATUS_OK;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_open_from_xml_container
    (dlb_adm_core_model        **p_model
    ,dlb_adm_xml_container      *container
    )
{
    dlb_adm_core_model_counts counts;
    int status;

    if ((p_model == nullptr) || (container == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    // TODO: LATER set the counts from what's in the container
    ::memset(&counts, 0, sizeof(counts));
    status = dlb_adm_core_model_open(p_model, &counts);
    if (status == DLB_ADM_STATUS_OK)
    {
        ActionFn f = [&]
        {
            XMLIngester ingester((*p_model)->GetCoreModel(), *container);
            return ingester.Ingest();
        };

        status = unwind_protect(f);
    }

    return status;
}

int
dlb_adm_core_model_ingest_xml_container
    (dlb_adm_core_model         *model
    ,dlb_adm_xml_container      *container
    )
{
    int status;

    if ((model == nullptr) || (container == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        XMLIngester ingester(model->GetCoreModel(), *container);
        return ingester.Ingest();
    };

    status = unwind_protect(f);

    return status;
}

static void Translate(const Gain &g, dlb_adm_data_gain &gain)
{
    gain.gain_value = g.GetGainValue();
    gain.gain_unit = static_cast<DLB_ADM_GAIN_UNIT>(g.GetGainUnit());
}

static int Translate(const ModelEntity *e, dlb_adm_data_audio_element &audioElement)
{
    const AudioElement *ae = dynamic_cast<const AudioElement *>(e);

    if (ae == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    audioElement.id = ae->GetEntityID();
    Translate(ae->GetGain(), audioElement.gain);

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_target_group &targetGroup)
{
    const TargetGroup *tg = dynamic_cast<const TargetGroup *>(e);

    if (tg == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    targetGroup.id = tg->GetEntityID();
    targetGroup.speaker_config = tg->GetSpeakerConfig();
    targetGroup.object_class = tg->GetObjectClass();
    targetGroup.is_dynamic = tg->IsDynamic();

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_target &target)
{
    const Target *t = dynamic_cast<const Target *>(e);

    if (t == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    target.id = t->GetEntityID();
    target.audio_type = t->GetAudioType();
    ::strncpy(target.speaker_label, t->GetSpeakerLabel().data(), sizeof(target.speaker_label));

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_audio_track &audioTrack)
{
    const AudioTrack *at = dynamic_cast<const AudioTrack *>(e);

    if (at == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    audioTrack.id = at->GetEntityID();
    audioTrack.sample_rate = at->GetSampleRate();
    audioTrack.bit_depth = at->GetBitDepth();

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_source_group &sourceGroup)
{
    const SourceGroup *sg = dynamic_cast<const SourceGroup *>(e);

    if (sg == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    sourceGroup.id = sg->GetEntityID();
    sourceGroup.group_id = sg->GetSourceGroupID();
    if (sg->HasName())
    {
        EntityName name;

        if (sg->GetName(name, 0))
        {
            ::strncpy(sourceGroup.name, name.GetName().data(), sizeof(sourceGroup.name));
        } 
        else
        {
            return DLB_ADM_STATUS_ERROR;
        }
    }

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_source &source)
{
    const Source *s = dynamic_cast<const Source *>(e);

    if (s == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    source.id = s->GetEntityID();
    source.group_id = s->GetSourceGroupID();
    source.channel = s->GetChannelNumber();

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_block_update &update)
{
    const BlockUpdate *u = dynamic_cast<const BlockUpdate *>(e);

    if (u == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    Position position = u->GetPosition();

    memset(&update, 0, sizeof(update));
    update.id = u->GetEntityID();
    update.cartesian = position.IsCartesian();
    update.position[DLB_ADM_COORDINATE_X] = position.GetCoordinate1();
    update.position[DLB_ADM_COORDINATE_Y] = position.GetCoordinate2();
    update.position[DLB_ADM_COORDINATE_Z] = position.GetCoordinate3();
    Translate(u->GetGain(), update.gain);
    if (u->HasTime())
    {
        update.has_time = DLB_ADM_TRUE;
        u->GetStart(update.start_time);
        u->GetDuration(update.duration);
    }

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_content_group &contentGroup)
{
    const ContentGroup *cg = dynamic_cast<const ContentGroup *>(e);

    if (cg == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    contentGroup.id = cg->GetEntityID();
    contentGroup.content_kind = cg->GetContentKind();

    return DLB_ADM_STATUS_OK;
}

static int Translate(const ModelEntity *e, dlb_adm_data_presentation &presentation)
{
    const Presentation *p = dynamic_cast<const Presentation *>(e);

    if (p == nullptr)
    {
        return DLB_ADM_STATUS_ERROR;
    }

    presentation.id = p->GetEntityID();

    return DLB_ADM_STATUS_OK;
}

template <typename T>
int Translate(const dlb_adm_core_model *model, dlb_adm_entity_id id, T &data)
{
    const ModelEntity *e;

    if (!model->GetCoreModel().GetEntity(id, &e))
    {
        return DLB_ADM_STATUS_ERROR;
    }

    return Translate(e, data);
}

int
dlb_adm_core_model_get_element_data
    (dlb_adm_data_audio_element_data    *element_data
    ,const dlb_adm_core_model           *model
    ,dlb_adm_entity_id                   audio_element_id
    )
{
    int status;

    if ((element_data == nullptr) || (model == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((element_data->channel_capacity == 0) || (audio_element_id == DLB_ADM_NULL_ENTITY_ID))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    status = dlb_adm_core_model_clear_element_data(element_data);
    CheckStatus(status);

    ActionFn f = [&]
    {
        int status;

        CoreModel::ElementCallbackFn elementCallback = [&](const ElementRecord &elementRecord)
        {
            int status = DLB_ADM_STATUS_OK;

            if (element_data->channel_count >= element_data->channel_capacity)
            {
                return static_cast<int>(DLB_ADM_STATUS_OUT_OF_MEMORY);
            }

            dlb_adm_channel_count channelIndex = element_data->channel_count;
            SourceRecord sourceRecord;
            UpdateRecord updateRecord;

            if (!model->GetCoreModel().GetSource(sourceRecord, elementRecord.audioTrackID))
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            if (!model->GetCoreModel().GetBlockUpdate(updateRecord, elementRecord.targetID))
            {
                return static_cast<int>(DLB_ADM_STATUS_ERROR);
            }

            if (channelIndex == 0)
            {
                status = Translate(model, elementRecord.audioElementID, element_data->audio_element);
                CheckStatus(status);
                status = Translate(model, elementRecord.targetGroupID, element_data->target_group);
                CheckStatus(status);
                status = Translate(model, sourceRecord.sourceGroupID, element_data->source_group);
                CheckStatus(status);
            }
            status = Translate(model, elementRecord.targetID, element_data->targets[channelIndex]);
            CheckStatus(status);
            status = Translate(model, elementRecord.audioTrackID, element_data->audio_tracks[channelIndex]);
            CheckStatus(status);
            status = Translate(model, sourceRecord.sourceID, element_data->sources[channelIndex]);
            CheckStatus(status);
            status = Translate(model, updateRecord.GetUpdateID(), element_data->block_updates[channelIndex]);
            CheckStatus(status);

            ++element_data->channel_count;

            return status;
        };
        status = model->GetCoreModel().ForEach(audio_element_id, elementCallback);

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_get_presentation_data
    (dlb_adm_data_presentation_data     *presentation_data  /**< [out] struct to fill, must have sufficient capacity */
    ,const dlb_adm_core_model           *model              /**< [in]  core model instance to query */
    ,dlb_adm_entity_id                   presentation_id    /**< [in]  ID of Presentation for which to get data */
    )
{
    int status;

    if ((presentation_data == nullptr) || (model == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((presentation_data->element_capacity == 0) || (presentation_id == DLB_ADM_NULL_ENTITY_ID))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    status = dlb_adm_core_model_clear_presentation_data(presentation_data);
    CheckStatus(status);

    ActionFn f = [&]
    {
        int status;

        CoreModel::PresentationCallbackFn presentationCallback = [&](const PresentationRecord &presentationRecord)
        {
            int status = DLB_ADM_STATUS_OK;

            if (presentation_data->element_count >= presentation_data->element_capacity)
            {
                return static_cast<int>(DLB_ADM_STATUS_OUT_OF_MEMORY);
            }

            dlb_adm_element_count elementIndex = presentation_data->element_count;

            if (elementIndex == 0)
            {
                status = Translate(model, presentationRecord.presentationID, presentation_data->presentation);
                CheckStatus(status);
            }

            status = Translate(model, presentationRecord.contentGroupID, presentation_data->content_groups[elementIndex]);
            CheckStatus(status);
            if (presentationRecord.elementGroupID != DLB_ADM_NULL_ENTITY_ID)
            {
                status = Translate(model, presentationRecord.elementGroupID, presentation_data->element_groups[elementIndex]);
                CheckStatus(status);
            }
            status = Translate(model, presentationRecord.audioElementID, presentation_data->audio_elements[elementIndex]);
            CheckStatus(status);

            ++presentation_data->element_count;

            return status;
        };
        status = model->GetCoreModel().ForEach(presentation_id, presentationCallback);

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_count_entities
    (const dlb_adm_core_model   *model
    ,DLB_ADM_ENTITY_TYPE         entity_type
    ,size_t                     *count
    )
{
    if ((model == nullptr) || (count == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((entity_type < DLB_ADM_ENTITY_TYPE_FIRST) || (entity_type > DLB_ADM_ENTITY_TYPE_LAST))
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }

    return unwind_protect([&] { *count = model->GetCoreModel().Count(entity_type); return DLB_ADM_STATUS_OK; });
}

int
dlb_adm_core_model_for_each_entity_id
    (const dlb_adm_core_model   *model
    ,DLB_ADM_ENTITY_TYPE         entity_type
    ,dlb_adm_for_each_callback   callback
    ,void                       *callback_arg
    )
{
    if ((model == nullptr) || (callback == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((entity_type < DLB_ADM_ENTITY_TYPE_FIRST) || (entity_type > DLB_ADM_ENTITY_TYPE_LAST))
    {
        return DLB_ADM_STATUS_OUT_OF_RANGE;
    }

    ActionFn f = [&]
    {
        int status;

        CoreModel::EntityCallbackFn callCallback = [&](const ModelEntity *e)
        {
            int status = (*callback)(model, e->GetEntityID(), callback_arg);
            return status;
        };
        status = model->GetCoreModel().ForEach(entity_type, callCallback);

        return status;
    };

    return unwind_protect(f);
}

static bool AudioElementFilter(const ModelEntity *e)
{
    return (dynamic_cast<const AudioElement *>(e) != nullptr);
}

int
dlb_adm_core_model_for_each_audio_element_id
    (const dlb_adm_core_model   *model
    ,dlb_adm_for_each_callback   callback
    ,void                       *callback_arg
    )
{
    if ((model == nullptr) || (callback == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        int status;

        CoreModel::EntityCallbackFn callCallback = [&](const ModelEntity *e)
        {
            int status = (*callback)(model, e->GetEntityID(), callback_arg);
            return status;
        };
        status = model->GetCoreModel().ForEach(DLB_ADM_ENTITY_TYPE_OBJECT, callCallback, AudioElementFilter);

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_for_each_source
    (const dlb_adm_core_model   *model
    ,dlb_adm_source_callback     callback
    ,void                       *callback_arg
    )
{
    if ((model == nullptr) || (callback == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        int status;

        CoreModel::EntityCallbackFn callCallback = [&](const ModelEntity *e)
        {
            dlb_adm_data_source source;
            int status;

            status = Translate(e, source);
            CheckStatus(status);
            status = (*callback)(model, &source, callback_arg);
            return status;
        };
        status = model->GetCoreModel().ForEach(DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, callCallback);

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_get_flow_id
    (const dlb_adm_core_model   *model  /**< [in]  model to query */
    ,char                       *uuid   /**< [out] uuid buffer */
    ,size_t                      sz     /**< [in]  size in bytes of uuid buffer */
    )
{
    if ((model == nullptr) || (uuid == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    ::memset(uuid, 0, sz);

    if (sz < DLB_ADM_DATA_FF_UUID_SZ)
    {
        return DLB_ADM_STATUS_OUT_OF_MEMORY;
    }

    ActionFn f = [&]
    {
        bool found = false;
        int status;

        CoreModel::EntityCallbackFn callback = [&](const ModelEntity *e)
        {
            const FrameFormat *ff = dynamic_cast<const FrameFormat *>(e);
            int status = DLB_ADM_STATUS_ERROR;

            if ((ff != nullptr) && (!found))
            {
                found = true;
                ::strncpy(uuid, ff->GetFlowID().data(), sz);
                status = DLB_ADM_STATUS_OK;
            }

            return status;
        };
        status = model->GetCoreModel().ForEach(DLB_ADM_ENTITY_TYPE_FRAME_FORMAT, callback);

        if (!found)
        {
            status = DLB_ADM_STATUS_NOT_FOUND;
        }
#ifndef NDEBUG
        CheckStatus(status);
#endif

        return status;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_close
    (dlb_adm_core_model    **p_model
    )
{
    if ((p_model == nullptr) || (*p_model == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        delete *p_model;
        *p_model = nullptr;

        return DLB_ADM_STATUS_OK;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_get_names
    (const dlb_adm_core_model   *model
    ,dlb_adm_data_names         *names
    ,dlb_adm_entity_id           entity_id
    )
{
    if ((model == nullptr) || (names == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (entity_id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        const ModelEntity *p;
        int status;

        status = dlb_adm_core_model_clear_names(names);
        CheckStatus(status);
        if (!model->GetCoreModel().GetEntity(entity_id, &p))
        {
            return DLB_ADM_STATUS_ERROR;
        }

        EntityName entityName;
        size_t i = 0;

        if (p->HasName())
        {
            if (!p->GetName(entityName, i))
            {
                return DLB_ADM_STATUS_ERROR;
            }
            status = dlb_adm_core_model_add_name(names, entityName.GetName().c_str(), entityName.GetLanguage().c_str());
            CheckStatus(status);
            ++i;
        }

        for (; i < p->GetNameCount(); ++i)
        {
            if (!p->GetName(entityName, i))
            {
                return DLB_ADM_STATUS_ERROR;
            }
            status = dlb_adm_core_model_add_label(names, entityName.GetName().c_str(), entityName.GetLanguage().c_str());
            CheckStatus(status);
        }

        return DLB_ADM_STATUS_OK;
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_entity_exists
    (const dlb_adm_core_model   *model
    ,dlb_adm_entity_id           id
    ,dlb_adm_bool               *exists
    )
{
    if ((model == nullptr) || (exists == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }
    *exists = DLB_ADM_FALSE;

    if (id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        const ModelEntity *p;

        *exists = model->GetCoreModel().GetEntity(id, &p);

        return DLB_ADM_STATUS_OK;
    };

    return unwind_protect(f);
}

static
int
add_source
    (dlb_adm_core_model         *model
    ,dlb_adm_data_source        *source
    )
{
    CoreModel &coreModel = model->GetCoreModel();

    if (source->id == DLB_ADM_NULL_ENTITY_ID)
    {
        int status = coreModel.GetEntityId(source->id, DLB_ADM_ENTITY_TYPE_AUDIO_TRACK);

        if (status != DLB_ADM_STATUS_OK)
        {
            return status;
        }
    }

    Source coreModelSource(source->id, source->channel, source->group_id);
    bool added = coreModel.AddEntity(coreModelSource);
    return (added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
}

int
dlb_adm_core_model_add_source
    (dlb_adm_core_model         *model
    ,dlb_adm_data_source        *source
    )
{
    if ((model == nullptr) || (source == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((source->group_id == 0) || (source->channel == 0))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        return add_source(model, source);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_sources
    (dlb_adm_core_model         *model
    ,dlb_adm_source_group_id     group_id
    ,dlb_adm_channel_number      start_channel
    ,size_t                      channel_count
    ,dlb_adm_entity_id          *source_id_array
    )
{
    if ((model == nullptr) || (source_id_array == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((group_id == 0) || (start_channel == 0) || (channel_count == 0))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        dlb_adm_channel_number ch;
        size_t i;

        for (i = 0, ch = start_channel; i < channel_count; ++i, ++ch)
        {
            dlb_adm_data_source source;
            int status;

            source.id = source_id_array[i];
            source.group_id = group_id;
            source.channel = ch;
            status = ::add_source(model, &source);
            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
            source_id_array[i] = source.id;
        }

        return static_cast<int>(DLB_ADM_STATUS_OK);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_source_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_source_group  *source_group
    )
{
    if ((model == nullptr) || (source_group == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (source_group->group_id == UNKNOWN_SOURCE_GROUP_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (source_group->id == DLB_ADM_NULL_ENTITY_ID)
        {
            source_group->id = AdmIdTranslator().ConstructUntypedId(
                DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT, static_cast<uint32_t>(source_group->group_id));
        }

        SourceGroup coreModelSourceGroup(source_group->id, source_group->group_id);
        std::string groupName(source_group->name);
        bool added = true;

        if (!groupName.empty())
        {
            added = coreModelSourceGroup.AddName(groupName, "");
        }
        if (added)
        {
            added = coreModel.AddEntity(coreModelSourceGroup);
        }
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

template <class T>
void AddName(T &entity, dlb_adm_data_names &names)
{
    dlb_adm_bool hasName;
    int status;

    status = dlb_adm_core_model_has_name(&hasName, &names);
    if (status != DLB_ADM_STATUS_OK)
    {
        throw status;
    }

    if (hasName)
    {
        if (!entity.AddName(names.names[0], names.langs[0]))
        {
            throw DLB_ADM_STATUS_ERROR;
        }
    }
}

template <class T>
void AddNameAndLabels(T &entity, dlb_adm_data_names &names)
{
    dlb_adm_bool hasName;
    size_t i = 0;
    int status;

    status = dlb_adm_core_model_has_name(&hasName, &names);
    if (status != DLB_ADM_STATUS_OK)
    {
        throw status;
    }

    if (hasName)
    {
        if (!entity.AddName(names.names[i], names.langs[i]))
        {
            throw DLB_ADM_STATUS_ERROR;
        }
        ++i;
    }

    while (i < names.name_count)
    {
        if (!entity.AddLabel(names.names[i], names.langs[i]))
        {
            throw DLB_ADM_STATUS_ERROR;
        }
        ++i;
    }
}

int
dlb_adm_core_model_add_target
    (dlb_adm_core_model         *model
    ,dlb_adm_data_target        *target
    ,dlb_adm_data_names         *names
    )
{
    dlb_adm_data_names no_names;

    if ((model == nullptr) || (target == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((target->audio_type != DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS) &&
        (target->audio_type != DLB_ADM_AUDIO_TYPE_OBJECTS))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    if (names == nullptr)
    {
        ::memset(&no_names, 0, sizeof(no_names));
        names = &no_names;
    }

    if ((names->name_count > 1) || (names->label_count > 0))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (target->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(target->id, DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, target->audio_type);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        Target coreModelTarget(target->id, target->audio_type, target->speaker_label);
        AddName(coreModelTarget, *names);
        bool added = coreModel.AddEntity(coreModelTarget);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_target_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_target_group  *target_group
    ,dlb_adm_data_names         *names
    )
{
    if ((model == nullptr) || (target_group == nullptr) || (names == nullptr))  // Name is required
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (((target_group->speaker_config == DLB_ADM_SPEAKER_CONFIG_NONE) && (target_group->object_class == DLB_ADM_OBJECT_CLASS_NONE)) ||
        ((target_group->speaker_config != DLB_ADM_SPEAKER_CONFIG_NONE) && (target_group->object_class != DLB_ADM_OBJECT_CLASS_NONE)) ||

        (target_group->speaker_config > DLB_ADM_SPEAKER_CONFIG_LAST) ||
        (target_group->object_class   > DLB_ADM_OBJECT_CLASS_LAST)   ||

        (names->name_count  != 1) ||
        (names->label_count != 0))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (target_group->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(target_group->id, DLB_ADM_ENTITY_TYPE_PACK_FORMAT);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        bool added = false;

        if (target_group->speaker_config == DLB_ADM_SPEAKER_CONFIG_NONE)
        {
            TargetGroup coreModelTargetGroup(target_group->id, target_group->object_class, target_group->is_dynamic);
            AddName(coreModelTargetGroup, *names);
            added = coreModel.AddEntity(coreModelTargetGroup);
        } 
        else if (target_group->object_class == DLB_ADM_OBJECT_CLASS_NONE)
        {
            TargetGroup coreModelTargetGroup(target_group->id, target_group->speaker_config);
            AddName(coreModelTargetGroup, *names);
            added = coreModel.AddEntity(coreModelTargetGroup);
        }

        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_audio_track
    (dlb_adm_core_model         *model
    ,dlb_adm_data_audio_track   *audio_track
    )
{
    if ((model == nullptr) || (audio_track == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (audio_track->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(audio_track->id, DLB_ADM_ENTITY_TYPE_TRACK_UID);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        AudioTrack coreModelAudioTrack(audio_track->id, audio_track->sample_rate, audio_track->bit_depth);
        bool added = coreModel.AddEntity(coreModelAudioTrack);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

static
dlb_adm_bool
validate_gain
    (const dlb_adm_data_gain *gain
    )
{
    dlb_adm_bool ok = DLB_ADM_TRUE;

    if ((gain == nullptr) || (gain->gain_unit > DLB_ADM_GAIN_UNIT_LAST))
    {
        ok = DLB_ADM_FALSE;
    }

    return ok;
}

int
dlb_adm_core_model_add_audio_element
    (dlb_adm_core_model         *model
    ,dlb_adm_data_audio_element *audio_element
    ,dlb_adm_data_names         *names
    )
{
    dlb_adm_bool hasName;
    int status;

    if ((model == nullptr) || (audio_element == nullptr) || (names == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = dlb_adm_core_model_has_name(&hasName, names);
    if ((!validate_gain(&audio_element->gain)) || (status != DLB_ADM_STATUS_OK) || (!hasName))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (audio_element->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(audio_element->id, DLB_ADM_ENTITY_TYPE_OBJECT);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        Gain coreModelGain(audio_element->gain);
        AudioElement coreModelElement(audio_element->id, coreModelGain);
        AddNameAndLabels(coreModelElement, *names);
        bool added = coreModel.AddEntity(coreModelElement);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_element_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_element_group *element_group
    ,dlb_adm_data_names         *names
    )
{
    dlb_adm_bool hasName;
    int status;

    if ((model == nullptr) || (element_group == nullptr) || (names == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = dlb_adm_core_model_has_name(&hasName, names);
    if ((!validate_gain(&element_group->gain)) || (status != DLB_ADM_STATUS_OK) || (!hasName))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (element_group->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(element_group->id, DLB_ADM_ENTITY_TYPE_OBJECT);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        Gain coreModelGain(element_group->gain);
        ElementGroup coreModelGroup(element_group->id, coreModelGain);
        AddNameAndLabels(coreModelGroup, *names);
        bool added = coreModel.AddEntity(coreModelGroup);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

static bool validate_content_kind(DLB_ADM_CONTENT_KIND kind)
{
    bool good = false;

    switch (kind)
    {
    case DLB_ADM_CONTENT_KIND_NK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_NK_MUSIC:
    case DLB_ADM_CONTENT_KIND_NK_EFFECTS:
    case DLB_ADM_CONTENT_KIND_DK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_DK_DIALOGUE:
    case DLB_ADM_CONTENT_KIND_DK_VOICEOVER:
    case DLB_ADM_CONTENT_KIND_DK_SUBTITLE:
    case DLB_ADM_CONTENT_KIND_DK_DESCRIPTION:
    case DLB_ADM_CONTENT_KIND_DK_COMMENTARY:
    case DLB_ADM_CONTENT_KIND_DK_EMERGENCY:
    case DLB_ADM_CONTENT_KIND_MK_UNDEFINED:
    case DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN:
    case DLB_ADM_CONTENT_KIND_MK_MIXED:
    case DLB_ADM_CONTENT_KIND_MK_HEARING_IMPAIRED:
    case DLB_ADM_CONTENT_KIND_UNKNOWN:
        good = true;
        break;

    default:
        break;
    }

    return good;
}

int
dlb_adm_core_model_add_content_group
    (dlb_adm_core_model         *model
    ,dlb_adm_data_content_group *content_group
    ,dlb_adm_data_names         *names
    )
{
    dlb_adm_bool hasName;
    int status;

    if ((model == nullptr) || (content_group == nullptr) || (names == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = dlb_adm_core_model_has_name(&hasName, names);
    if ((!validate_content_kind(content_group->content_kind)) || (status != DLB_ADM_STATUS_OK) || (!hasName))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (content_group->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(content_group->id, DLB_ADM_ENTITY_TYPE_CONTENT);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        ContentGroup coreModelGroup(content_group->id, content_group->content_kind);
        AddNameAndLabels(coreModelGroup, *names);
        bool added = coreModel.AddEntity(coreModelGroup);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_presentation
    (dlb_adm_core_model         *model
    ,dlb_adm_data_presentation  *presentation
    ,dlb_adm_data_names         *names
    )
{
    dlb_adm_bool hasName;
    int status;

    if ((model == nullptr) || (presentation == nullptr) || (names == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = dlb_adm_core_model_has_name(&hasName, names);
    if ((status != DLB_ADM_STATUS_OK) || (!hasName))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (presentation->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(presentation->id, DLB_ADM_ENTITY_TYPE_PROGRAMME);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        Presentation coreModelGroup(presentation->id);
        AddNameAndLabels(coreModelGroup, *names);
        bool added = coreModel.AddEntity(coreModelGroup);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_frame_format
    (dlb_adm_core_model         *model
    ,dlb_adm_data_frame_format  *frame_format
    )
{
    if ((model == nullptr) || (frame_format == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    std::string frameType(frame_format->type);

    if ((frameType != std::string("full")) ||
        (std::string(frame_format->start   ).empty()) ||
        (std::string(frame_format->duration).empty()))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (frame_format->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetEntityId(frame_format->id, DLB_ADM_ENTITY_TYPE_FRAME_FORMAT);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        FrameFormat coreModelFrameFormat(
            frame_format->id, frame_format->type, frame_format->start, frame_format->duration, frame_format->flow_id);
        bool added = coreModel.AddEntity(coreModelFrameFormat);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_block_update
    (dlb_adm_core_model         *model
    ,dlb_adm_entity_id           parent_id
    ,dlb_adm_data_block_update  *block_update
    )
{
    int status;

    if ((model == nullptr) || (block_update == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (((parent_id == DLB_ADM_NULL_ENTITY_ID) && (block_update->id == DLB_ADM_NULL_ENTITY_ID)) ||
        ((parent_id != DLB_ADM_NULL_ENTITY_ID) && (block_update->id != DLB_ADM_NULL_ENTITY_ID)))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    if (parent_id != DLB_ADM_NULL_ENTITY_ID)
    {
        dlb_adm_bool exists;

        status = dlb_adm_core_model_entity_exists(model, parent_id, &exists);
        if (status != DLB_ADM_STATUS_OK)
        {
            return status;
        }
        if (!exists)
        {
            return DLB_ADM_STATUS_INVALID_ARGUMENT;
        }
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();

        if (block_update->id == DLB_ADM_NULL_ENTITY_ID)
        {
            int status = coreModel.GetSubcomponentID(block_update->id, parent_id);

            if (status != DLB_ADM_STATUS_OK)
            {
                return status;
            }
        }

        // TODO: consider adding a type-conversion constructor to BlockUpdate for dlb_adm_data_block_update
        Position pos
        (
            block_update->position[DLB_ADM_COORDINATE_X],
            block_update->position[DLB_ADM_COORDINATE_Y],
            block_update->position[DLB_ADM_COORDINATE_Z],
            block_update->cartesian
        );
        Gain gain(block_update->gain);
        BlockUpdate coreBlockUpdate
        (
            block_update->id,
            pos,
            gain,
            block_update->has_time ? &block_update->start_time : nullptr,
            block_update->has_time ? &block_update->duration : nullptr
        );
        bool added = coreModel.AddEntity(coreBlockUpdate);

        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_source_relation
    (dlb_adm_core_model     *model 
    ,dlb_adm_entity_id       source_group_id 
    ,dlb_adm_entity_id       source_id 
    ,dlb_adm_entity_id       audio_track_id 
    )
{
    if (model == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((source_group_id == DLB_ADM_NULL_ENTITY_ID) ||
        (source_id       == DLB_ADM_NULL_ENTITY_ID) ||
        (audio_track_id  == DLB_ADM_NULL_ENTITY_ID))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();
        SourceRecord r(source_group_id, source_id, audio_track_id);
        bool added = coreModel.AddRecord(r);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_element_relation
    (dlb_adm_core_model     *model
    ,dlb_adm_entity_id       audio_element_id
    ,dlb_adm_entity_id       target_group_id
    ,dlb_adm_entity_id       target_id
    ,dlb_adm_entity_id       audio_track_id
    )
{
    if (model == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((audio_element_id == DLB_ADM_NULL_ENTITY_ID) ||
        (target_group_id  == DLB_ADM_NULL_ENTITY_ID) ||
        (target_id        == DLB_ADM_NULL_ENTITY_ID) ||
        (audio_track_id   == DLB_ADM_NULL_ENTITY_ID))
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();
        ElementRecord r(audio_element_id, target_group_id, target_id, audio_track_id);
        bool added = coreModel.AddRecord(r);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_add_presentation_relation
    (dlb_adm_core_model     *model
    ,dlb_adm_entity_id       presentation_id
    ,dlb_adm_entity_id       content_group_id
    ,dlb_adm_entity_id       element_group_id
    ,dlb_adm_entity_id       audio_element_id
    )
{
    if (model == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if ((content_group_id == DLB_ADM_NULL_ENTITY_ID) ||
        (audio_element_id == DLB_ADM_NULL_ENTITY_ID))   // Null presentation_id and/or element_group_id is OK
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    ActionFn f = [&]
    {
        CoreModel &coreModel = model->GetCoreModel();
        PresentationRecord r(presentation_id, content_group_id, audio_element_id, element_group_id);
        bool added = coreModel.AddRecord(r);
        return static_cast<int>(added ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR);
    };

    return unwind_protect(f);
}

int
dlb_adm_core_model_clear
    (dlb_adm_core_model         *model
    )
{
    if (model == nullptr)
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    return unwind_protect([&] { model->GetCoreModel().Clear(); return DLB_ADM_STATUS_OK; });
}

int
dlb_adm_core_model_is_empty
    (dlb_adm_core_model         *model
    ,dlb_adm_bool               *is_empty
    )
{
    bool empty = false;
    int status;

    if ((model == nullptr) || (is_empty == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    status = unwind_protect([&] { empty = model->GetCoreModel().IsEmpty(); return DLB_ADM_STATUS_OK; });
    *is_empty = static_cast<dlb_adm_bool>(empty ? DLB_ADM_TRUE : DLB_ADM_FALSE);

    return status;
}

dlb_adm_gain_value
dlb_adm_gain_in_decibels
    (dlb_adm_data_gain       gain
    )
{
    dlb_adm_gain_value gain_in_db = 0.0;

    if (gain.gain_unit == DLB_ADM_GAIN_UNIT_DB)
    {
        gain_in_db = gain.gain_value;
    } 
    else
    {
        ActionFn convertGain = [&]
        {
            gain_in_db = Gain::LinearToDecibels(gain.gain_value);
            return DLB_ADM_STATUS_OK;
        };
        (void)unwind_protect(convertGain);
    }

    return gain_in_db;
}
