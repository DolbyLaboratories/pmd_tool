/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

#include "AdmIdTranslator.h"
#include "AdmId.h"

#include <cctype>

#ifdef _MSC_VER
#define PRIX64 "I64X"
#else
#include <cinttypes>
#endif

namespace DlbAdm
{

    static const char *ENTITY_ID_PREFIXES[] =
    {
        "FF_",  /* DLB_ADM_ENTITY_TYPE_FRAME_FORMAT */
        "TP_",  /* DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT */
        "APR_", /* DLB_ADM_ENTITY_TYPE_PROGRAMME */
        "ACO_", /* DLB_ADM_ENTITY_TYPE_CONTENT */
        "AO_",  /* DLB_ADM_ENTITY_TYPE_OBJECT */
        "AP_",  /* DLB_ADM_ENTITY_TYPE_PACK_FORMAT */
        "AS_",  /* DLB_ADM_ENTITY_TYPE_STREAM_FORMAT */
        "AC_",  /* DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT */
        "AT_",  /* DLB_ADM_ENTITY_TYPE_TRACK_FORMAT */
        "AB_",  /* DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT */
        "AVS_", /* DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET */
        "ATU_", /* DLB_ADM_ENTITY_TYPE_TRACK_UID */
        "AFC_", /* DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET */
    };

    static DLB_ADM_ENTITY_TYPE FindIdType(const char *s, size_t *f)
    {
        char prefix[5];
        size_t i = 2;
        size_t l;
        int t;

        if (s[i] != '_')
        {
            if (s[++i] != '_')
            {
                return DLB_ADM_ENTITY_TYPE_ILLEGAL;
            }
        }
        *f = l = i + 1;
        for (i = 0; i < l; i++)
        {
            prefix[i] = s[i];
        }
        prefix[l] = '\0';

        for (t = DLB_ADM_ENTITY_TYPE_FIRST_WITH_ID, i = 0; t <= DLB_ADM_ENTITY_TYPE_LAST_WITH_ID; t++, i++)
        {
            if (!strcmp(prefix, ENTITY_ID_PREFIXES[i]))
            {
                return static_cast<DLB_ADM_ENTITY_TYPE>(t);
            }
        }

        return DLB_ADM_ENTITY_TYPE_ILLEGAL;
    }

    static uint8_t hexValue(char ch)
    {
        uint8_t v = 0;

        switch (ch)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            v = static_cast<uint8_t>(ch - '0');
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            v = static_cast<uint8_t>((ch - 'a') + 10);
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            v = static_cast<uint8_t>((ch - 'A') + 10);
            break;

        default:
            break;
        }

        return v;
    }

    static uint64_t readHex(const char *s, size_t *start, size_t *count, size_t limit = ADM_ID_MAX_LEN)
    {
        uint64_t x = 0ull;
        size_t i = *start;
        size_t n = 0;
        char ch = s[i];

        while (::isxdigit(ch))
        {
            x = (x << 4) + hexValue(ch);
            ch = s[++i];
            if (++n >= limit)
            {
                ch = '\0';
                break;
            }
        }

        *start = (ch == '\0' ? i : i + 1);
        *count = n;

        return x;
    }

    DLB_ADM_ENTITY_TYPE AdmIdTranslator::GetEntityType(dlb_adm_entity_id id) const
    {
        DLB_ADM_ENTITY_TYPE entityType = static_cast<DLB_ADM_ENTITY_TYPE>(id >> ENTITY_TYPE_SHIFT);

        if (entityType < DLB_ADM_ENTITY_TYPE_FIRST || entityType > DLB_ADM_ENTITY_TYPE_LAST)
        {
            entityType = DLB_ADM_ENTITY_TYPE_ILLEGAL;
        }

        return entityType;
    }

    dlb_adm_entity_id AdmIdTranslator::Translate(const char *id) const
    {
        dlb_adm_entity_id numericId = 0ull;

        if (id == nullptr)
        {
            return numericId;
        }

        bool ok = false;
        bool generic = false;
        size_t i = 0u;
        size_t n;
        DLB_ADM_ENTITY_TYPE entityType = FindIdType(id, &i);
        DLB_ADM_AUDIO_TYPE audioType = DLB_ADM_AUDIO_TYPE_NONE;
        uint16_t xw = 0u;
        uint32_t z = 0u;
        uint64_t ff = 0ull;
        uint8_t pp = 0u;

        switch (entityType)
        {
        case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
            ff = readHex(id, &i, &n);
            if (n > 0 && ff <= MASK_48)
            {
                ok = true;
                if (id[i] != '\0')
                {
                    pp = static_cast<uint8_t>(readHex(id, &i, &n));
                    ok = (n == 2);
                }
            }
            break;

        case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
        case DLB_ADM_ENTITY_TYPE_CONTENT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
        case DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET:
            xw = static_cast<uint16_t>(readHex(id, &i, &n));
            ok = (n == 4);
            break;

        case DLB_ADM_ENTITY_TYPE_PACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_STREAM_FORMAT:
        case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>(readHex(id, &i, &n, 4));
            if (n == 4 && audioType > DLB_ADM_AUDIO_TYPE_NONE && audioType <= DLB_ADM_AUDIO_TYPE_LAST_STD)  // We don't do custom...
            {
                xw = static_cast<uint16_t>(readHex(id, &i, &n));
                ok = (n == 4);
            }
            break;

        case DLB_ADM_ENTITY_TYPE_TRACK_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>(readHex(id, &i, &n, 4));
            if (n == 4 && audioType > DLB_ADM_AUDIO_TYPE_NONE && audioType <= DLB_ADM_AUDIO_TYPE_LAST_STD)  // We don't do custom...
            {
                xw = static_cast<uint16_t>(readHex(id, &i, &n));
                if (n == 4)
                {
                    z = static_cast<uint32_t>(readHex(id, &i, &n));
                    ok = (n == 2);
                }
            }
            break;

        case DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>(readHex(id, &i, &n, 4));
            if (n == 4 && audioType > DLB_ADM_AUDIO_TYPE_NONE && audioType <= DLB_ADM_AUDIO_TYPE_LAST_STD)  // We don't do custom...
            {
                xw = static_cast<uint16_t>(readHex(id, &i, &n));
                if (n == 4)
                {
                    z = static_cast<uint32_t>(readHex(id, &i, &n));
                    ok = (n == 8);
                }
            }
            break;

        case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
            xw = static_cast<uint16_t>(readHex(id, &i, &n));
            if (n == 4)
            {
                z = static_cast<uint32_t>(readHex(id, &i, &n));
                ok = (n == 4);
            }
            break;

        case DLB_ADM_ENTITY_TYPE_TRACK_UID:
            z = static_cast<uint32_t>(readHex(id, &i, &n));
            if (n == 8)
            {
                generic = true;
                ok = true;
            }
            break;

        default:
            break;
        }

        if (ok)
        {
            if (generic)
            {
                numericId = ConstructGenericId(entityType, z);
            } 
            else
            {
                numericId = ConstructId(entityType, audioType, xw, z, ff, pp);
            }
        }

        return numericId;
    }

    dlb_adm_entity_id AdmIdTranslator::Translate(const std::string &id) const
    {
        return Translate(id.data());
    }

    static char *writePrefix(char *buf, DLB_ADM_ENTITY_TYPE entityType)
    {
        size_t prefixIndex = static_cast<size_t>(entityType) - static_cast<size_t>(DLB_ADM_ENTITY_TYPE_FIRST_WITH_ID);
        std::string prefix = ENTITY_ID_PREFIXES[prefixIndex];

        sprintf(buf, "%s", prefix.data());
        return buf + prefix.size();
    }

    template <class T>
    static char *writeHex(char *buf, T value, bool separator)
    {
        unsigned int n = 2 * sizeof(T);

        if (separator)
        {
            sprintf(buf, "%0*X_", n, value);
            ++n;
        } 
        else
        {
            sprintf(buf, "%0*X", n, value);
        }

        return buf + n;
    }

    static char *writeAudioType(char *buf, DLB_ADM_AUDIO_TYPE audioType)
    {
        return writeHex(buf, static_cast<uint16_t>(audioType), false);
    }

    static char *writeFFHex(char *buf, uint64_t ff, bool separator)
    {
        unsigned int n = FF_HEX_WIDTH;

        if (separator)
        {
            sprintf(buf, "%0*" PRIX64 "_", FF_HEX_WIDTH, ff);
            ++n;
        } 
        else
        {
            sprintf(buf, "%0*" PRIX64, FF_HEX_WIDTH, ff);
        }

        return buf + n;
    }

    std::string AdmIdTranslator::Translate(dlb_adm_entity_id id) const
    {
        char str[ADM_ID_MAX_LEN * 2];   // Perhaps overly cautious...
        char *buf = str;
        DLB_ADM_ENTITY_TYPE entityType = static_cast<DLB_ADM_ENTITY_TYPE>(id >> ENTITY_TYPE_SHIFT);
        DLB_ADM_AUDIO_TYPE audioType = DLB_ADM_AUDIO_TYPE_NONE;
        uint16_t xw = 0u;
        uint8_t z8 = 0u;
        uint16_t z16 = 0u;
        uint32_t z32 = 0u;
        uint64_t ff = 0ull;
        uint8_t pp = 0u;
        std::string stringId = "";
        bool ok = true;

        ::memset(str, 0, sizeof(str));

        switch (entityType)
        {
        case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
            pp = static_cast<uint8_t>((id >> FRAME_PART_SHIFT) & MASK_08);
            ff = id & MASK_48;
            buf = writePrefix(buf, entityType);
            buf = writeFFHex(buf, ff, pp > 0);
            if (pp > 0)
            {
                buf = writeHex(buf, pp, false);
            }
            break;

        case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
        case DLB_ADM_ENTITY_TYPE_CONTENT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
        case DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET:
            xw = static_cast<uint16_t>((id >> X_W_SHIFT) & MASK_16);
            buf = writePrefix(buf, entityType);
            buf = writeHex(buf, xw, false);
            break;

        case DLB_ADM_ENTITY_TYPE_PACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_STREAM_FORMAT:
        case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
            xw = static_cast<uint16_t>((id >> X_W_SHIFT) & MASK_16);
            buf = writePrefix(buf, entityType);
            buf = writeAudioType(buf, audioType);
            buf = writeHex(buf, xw, false);
            break;

        case DLB_ADM_ENTITY_TYPE_TRACK_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
            xw = static_cast<uint16_t>((id >> X_W_SHIFT) & MASK_16);
            z8 = static_cast<uint8_t>(id & MASK_08);
            buf = writePrefix(buf, entityType);
            buf = writeAudioType(buf, audioType);
            buf = writeHex(buf, xw, true);
            buf = writeHex(buf, z8, false);
            break;

        case DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>((id >> AUDIO_TYPE_SHIFT) & MASK_08);
            xw = static_cast<uint16_t>((id >> X_W_SHIFT) & MASK_16);
            z32 = static_cast<uint32_t>(id & MASK_32);
            buf = writePrefix(buf, entityType);
            buf = writeAudioType(buf, audioType);
            buf = writeHex(buf, xw, true);
            buf = writeHex(buf, z32, false);
            break;

        case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
            xw = static_cast<uint16_t>((id >> X_W_SHIFT) & MASK_16);
            z16 = static_cast<uint16_t>(id & MASK_16);
            buf = writePrefix(buf, entityType);
            buf = writeHex(buf, xw, true);
            buf = writeHex(buf, z16, false);
            break;

        case DLB_ADM_ENTITY_TYPE_TRACK_UID:
            z32 = static_cast<uint32_t>(id & MASK_32);
            buf = writePrefix(buf, entityType);
            buf = writeHex(buf, z32, false);
            break;

        default:
            ok = false;
            break;
        }

        if (ok)
        {
            stringId = str;
        }

        return stringId;
    }

    bool AdmIdTranslator::IsGenericEntityType(DLB_ADM_ENTITY_TYPE entityType) const
    {
        bool test1 = (entityType >= DLB_ADM_ENTITY_TYPE_TOPLEVEL      && entityType <= DLB_ADM_ENTITY_TYPE_LAST);
        bool test2 = (entityType <  DLB_ADM_ENTITY_TYPE_FIRST_WITH_ID || entityType >  DLB_ADM_ENTITY_TYPE_LAST_WITH_ID);
        bool test3 = (entityType == DLB_ADM_ENTITY_TYPE_TRACK_UID);

        return (test1 && test2) || test3;
    }

    bool AdmIdTranslator::SubcomponentIdReferencesComponent(const dlb_adm_entity_id parentId, const dlb_adm_entity_id subcomponentId) const
    {
        bool res = false;
        DLB_ADM_ENTITY_TYPE parentType       = GetEntityType(parentId);
        DLB_ADM_ENTITY_TYPE subcomponentType = GetEntityType(subcomponentId);

        if  (   (parentType == DLB_ADM_ENTITY_TYPE_OBJECT         &&  subcomponentType == DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET)
            // TODO: finish implementation for pair DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT and BLOCK_FORMAT (if needed)
            )
        {
            uint32_t parentSeqNumber;
            uint32_t subcomponentSeqNumber;
            this->DeconstructUntypedId(parentId,       nullptr, &parentSeqNumber,      nullptr);
            this->DeconstructUntypedId(subcomponentId, nullptr, &subcomponentSeqNumber, nullptr);

            res = (parentSeqNumber == subcomponentSeqNumber);
        }

        return res;
    }

    dlb_adm_entity_id AdmIdTranslator::ConstructGenericId(DLB_ADM_ENTITY_TYPE entityType, uint32_t sequenceNumber) const
    {
        dlb_adm_entity_id numericId = 0ull;

        if (IsGenericEntityType(entityType))
        {
            numericId =
                (static_cast<uint64_t>(entityType) << ENTITY_TYPE_SHIFT) |
                (static_cast<uint64_t>(sequenceNumber));
        }

        return numericId;
    }

    dlb_adm_entity_id AdmIdTranslator::ConstructUntypedId(DLB_ADM_ENTITY_TYPE entityType, uint32_t sequenceNumber, uint32_t subSequenceNumber) const
    {
        dlb_adm_entity_id id = DLB_ADM_NULL_ENTITY_ID;

        switch (entityType)
        {
        case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
        case DLB_ADM_ENTITY_TYPE_CONTENT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
        case DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET:
            id = ConstructId(entityType, DLB_ADM_AUDIO_TYPE_NONE, static_cast<uint16_t>(sequenceNumber), 0, 0, 0);
            break;

        case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
            id = ConstructId(entityType, DLB_ADM_AUDIO_TYPE_NONE, 0, 0, sequenceNumber, static_cast<uint8_t>(subSequenceNumber));
            break;

        case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
            id = ConstructId(entityType, DLB_ADM_AUDIO_TYPE_NONE, static_cast<uint16_t>(sequenceNumber), static_cast<uint16_t>(subSequenceNumber), 0 , 0);
            break;

        default:
            break;
        }

        return id;
    }

    dlb_adm_entity_id AdmIdTranslator::ConstructTypedId(DLB_ADM_ENTITY_TYPE entityType, DLB_ADM_AUDIO_TYPE audioType, uint32_t sequenceNumber, uint32_t subSequenceNumber) const
    {
        dlb_adm_entity_id id = DLB_ADM_NULL_ENTITY_ID;

        switch (entityType)
        {
        case DLB_ADM_ENTITY_TYPE_PACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_STREAM_FORMAT:
        case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
            id = ConstructId(entityType, audioType, static_cast<uint16_t>(sequenceNumber), 0, 0, 0);
            break;

        case DLB_ADM_ENTITY_TYPE_TRACK_FORMAT:
            id = ConstructId(entityType, audioType, static_cast<uint16_t>(sequenceNumber), subSequenceNumber, 0, 0);
            break;

        default:
            break;
        }

        return id;
    }

    dlb_adm_entity_id AdmIdTranslator::ConstructSubcomponentId(dlb_adm_entity_id parentId, uint32_t sequenceNumber) const
    {
        dlb_adm_entity_id id = DLB_ADM_NULL_ENTITY_ID;
        DLB_ADM_ENTITY_TYPE parentType = GetEntityType(parentId);
        DLB_ADM_AUDIO_TYPE audioType;
        uint16_t xw;

        switch (parentType)
        {
        case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
            audioType = static_cast<DLB_ADM_AUDIO_TYPE>((parentId >> AUDIO_TYPE_SHIFT) & MASK_08);
            xw = static_cast<uint16_t>((parentId >> X_W_SHIFT) & MASK_16);
            id = ConstructId(DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, audioType, xw, sequenceNumber, 0, 0);
            break;

        case DLB_ADM_ENTITY_TYPE_OBJECT:
            xw = static_cast<uint16_t>((parentId >> X_W_SHIFT) & MASK_16);
            id = ConstructId(DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, DLB_ADM_AUDIO_TYPE_NONE, xw, sequenceNumber, 0, 0);
            break;

        default:
            break;
        }

        return id;
    }

    void AdmIdTranslator::DeconstructUntypedId(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE *entityType, uint32_t *sequenceNumber, uint32_t *subSequenceNumber) const
    {
        DLB_ADM_ENTITY_TYPE ent = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(id));
        uint32_t seq = 0;
        uint32_t sub = 0;

        switch (ent)
        {
        case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
        case DLB_ADM_ENTITY_TYPE_CONTENT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
        case DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET:
            // extract xw
            seq = static_cast<uint32_t>((id >> X_W_SHIFT) & MASK_16);
            break;

        case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
            // extract ff
            seq = static_cast<uint32_t>(id & MASK_48);
            // extract pp
            sub = static_cast<uint32_t>((id >> FRAME_PART_SHIFT) & MASK_08);
            break;

        case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
            // extract xw
            seq = static_cast<uint32_t>((id >> X_W_SHIFT) & MASK_16);
            // extract z
            sub = static_cast<uint32_t>(id  & MASK_16);
            break;

        default:
            ent = DLB_ADM_ENTITY_TYPE_ILLEGAL;
            break;
        }

        if (entityType != nullptr)
        {
            *entityType = ent;
        }
        if (sequenceNumber != nullptr)
        {
            *sequenceNumber = seq;
        }
        if (subSequenceNumber != nullptr)
        {
            *subSequenceNumber = sub;
        }
    }

    dlb_adm_entity_id AdmIdTranslator::ConstructId(DLB_ADM_ENTITY_TYPE entityType, DLB_ADM_AUDIO_TYPE audioType, uint16_t xw, uint32_t z, uint64_t ff, uint8_t pp) const
    {
        dlb_adm_entity_id numericId = 0ull;

        switch (entityType)
        {
        case DLB_ADM_ENTITY_TYPE_FRAME_FORMAT:
            numericId =
                (static_cast<uint64_t>(entityType) << ENTITY_TYPE_SHIFT) |
                (static_cast<uint64_t>(pp)         << FRAME_PART_SHIFT)  |
                (ff & MASK_48);
            break;

        case DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_PROGRAMME:
        case DLB_ADM_ENTITY_TYPE_CONTENT:
        case DLB_ADM_ENTITY_TYPE_OBJECT:
        case DLB_ADM_ENTITY_TYPE_FORMAT_CUSTOM_SET:
            numericId =
                (static_cast<uint64_t>(entityType) << ENTITY_TYPE_SHIFT) |
                (static_cast<uint64_t>(xw)         << X_W_SHIFT);
            break;

        case DLB_ADM_ENTITY_TYPE_PACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_STREAM_FORMAT:
        case DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT:
            numericId =
                (static_cast<uint64_t>(entityType) << ENTITY_TYPE_SHIFT) |
                (static_cast<uint64_t>(audioType)  << AUDIO_TYPE_SHIFT)  |
                (static_cast<uint64_t>(xw)         << X_W_SHIFT);
            break;

        case DLB_ADM_ENTITY_TYPE_TRACK_FORMAT:
        case DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT:
            numericId =
                (static_cast<uint64_t>(entityType) << ENTITY_TYPE_SHIFT) |
                (static_cast<uint64_t>(audioType)  << AUDIO_TYPE_SHIFT)  |
                (static_cast<uint64_t>(xw)         << X_W_SHIFT)         |
                (static_cast<uint64_t>(z));
            break;

        case DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET:
            numericId =
                (static_cast<uint64_t>(entityType) << ENTITY_TYPE_SHIFT) |
                (static_cast<uint64_t>(xw)         << X_W_SHIFT)         |
                (static_cast<uint64_t>(z));
            break;

        default:
            numericId = ConstructGenericId(entityType, z);
            break;
        }

        return numericId;
    }

}
