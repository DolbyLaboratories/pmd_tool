/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "pmd_language.h"
#include "dlb_pmd_api.h"


#define MAKE_ISO_639_CODE(c1,c2,c3) ((c1)<<24 | (c2)<<16 | (c3)<<8)


/**
 * @brief ISO 639-1 and ISO 639-2 3-letter language codes
 */
static uint32_t ISO_639_CODES[] =
{
    MAKE_ISO_639_CODE('a','a','\0'),    /* Afar*/
    MAKE_ISO_639_CODE('a','a','r'),     /* Afar*/
    MAKE_ISO_639_CODE('a','b','\0'),    /* Abkhazian*/
    MAKE_ISO_639_CODE('a','b','k'),     /* Abkhazian*/
    MAKE_ISO_639_CODE('a','c','e'),     /* Achinese*/
    MAKE_ISO_639_CODE('a','c','h'),     /* Acoli*/
    MAKE_ISO_639_CODE('a','d','a'),     /* Adangme*/
    MAKE_ISO_639_CODE('a','d','y'),     /* Adyghe; Adygei*/
    MAKE_ISO_639_CODE('a','e','\0'),    /* Avestan*/
    MAKE_ISO_639_CODE('a','f','\0'),    /* Afrikaans*/
    MAKE_ISO_639_CODE('a','f','a'),     /* Afro-Asiatic languages*/
    MAKE_ISO_639_CODE('a','f','h'),     /* Afrihili*/
    MAKE_ISO_639_CODE('a','f','r'),     /* Afrikaans*/
    MAKE_ISO_639_CODE('a','i','n'),     /* Ainu*/
    MAKE_ISO_639_CODE('a','k','\0'),    /* Akan*/
    MAKE_ISO_639_CODE('a','k','a'),     /* Akan*/
    MAKE_ISO_639_CODE('a','k','k'),     /* Akkadian*/
    MAKE_ISO_639_CODE('a','l','b'),     /* Albanian*/
    MAKE_ISO_639_CODE('a','l','e'),     /* Aleut*/
    MAKE_ISO_639_CODE('a','l','g'),     /* Algonquian languages*/
    MAKE_ISO_639_CODE('a','l','t'),     /* Southern Altai*/
    MAKE_ISO_639_CODE('a','m','\0'),    /* Amharic*/
    MAKE_ISO_639_CODE('a','m','h'),     /* Amharic*/
    MAKE_ISO_639_CODE('a','n','\0'),    /* Aragonese*/
    MAKE_ISO_639_CODE('a','n','g'),     /* "English*/
    MAKE_ISO_639_CODE('a','n','p'),     /* Angika*/
    MAKE_ISO_639_CODE('a','p','a'),     /* Apache languages*/
    MAKE_ISO_639_CODE('a','r','\0'),    /* Arabic*/
    MAKE_ISO_639_CODE('a','r','a'),     /* Arabic*/
    MAKE_ISO_639_CODE('a','r','c'),     /* Official Aramaic (700-300 BCE); Imperial Aramaic (700-300 BCE)*/
    MAKE_ISO_639_CODE('a','r','g'),     /* Aragonese*/
    MAKE_ISO_639_CODE('a','r','m'),     /* Armenian*/
    MAKE_ISO_639_CODE('a','r','n'),     /* Mapudungun; Mapuche*/
    MAKE_ISO_639_CODE('a','r','p'),     /* Arapaho*/
    MAKE_ISO_639_CODE('a','r','t'),     /* Artificial languages*/
    MAKE_ISO_639_CODE('a','r','w'),     /* Arawak*/
    MAKE_ISO_639_CODE('a','s','\0'),    /* Assamese*/
    MAKE_ISO_639_CODE('a','s','m'),     /* Assamese*/
    MAKE_ISO_639_CODE('a','s','t'),     /* Asturian; Bable; Leonese; Asturleonese*/
    MAKE_ISO_639_CODE('a','t','h'),     /* Athapascan languages*/
    MAKE_ISO_639_CODE('a','u','s'),     /* Australian languages*/
    MAKE_ISO_639_CODE('a','v','\0'),    /* Avaric*/
    MAKE_ISO_639_CODE('a','v','a'),     /* Avaric*/
    MAKE_ISO_639_CODE('a','v','e'),     /* Avestan*/
    MAKE_ISO_639_CODE('a','w','a'),     /* Awadhi*/
    MAKE_ISO_639_CODE('a','y','\0'),    /* Aymara*/
    MAKE_ISO_639_CODE('a','y','m'),     /* Aymara*/
    MAKE_ISO_639_CODE('a','z','\0'),    /* Azerbaijani*/
    MAKE_ISO_639_CODE('a','z','e'),     /* Azerbaijani*/
    MAKE_ISO_639_CODE('b','a','\0'),    /* Bashkir*/
    MAKE_ISO_639_CODE('b','a','d'),     /* Banda languages*/
    MAKE_ISO_639_CODE('b','a','i'),     /* Bamileke languages*/
    MAKE_ISO_639_CODE('b','a','k'),     /* Bashkir*/
    MAKE_ISO_639_CODE('b','a','l'),     /* Baluchi*/
    MAKE_ISO_639_CODE('b','a','m'),     /* Bambara*/
    MAKE_ISO_639_CODE('b','a','n'),     /* Balinese*/
    MAKE_ISO_639_CODE('b','a','q'),     /* Basque*/
    MAKE_ISO_639_CODE('b','a','s'),     /* Basa*/
    MAKE_ISO_639_CODE('b','a','t'),     /* Baltic languages*/
    MAKE_ISO_639_CODE('b','e','\0'),    /* Belarusian*/
    MAKE_ISO_639_CODE('b','e','j'),     /* Beja; Bedawiyet*/
    MAKE_ISO_639_CODE('b','e','l'),     /* Belarusian*/
    MAKE_ISO_639_CODE('b','e','m'),     /* Bemba*/
    MAKE_ISO_639_CODE('b','e','n'),     /* Bengali*/
    MAKE_ISO_639_CODE('b','e','r'),     /* Berber languages*/
    MAKE_ISO_639_CODE('b','g','\0'),    /* Bulgarian*/
    MAKE_ISO_639_CODE('b','h','\0'),    /* Bihari languages*/
    MAKE_ISO_639_CODE('b','h','o'),     /* Bhojpuri*/
    MAKE_ISO_639_CODE('b','i','\0'),    /* Bislama*/
    MAKE_ISO_639_CODE('b','i','h'),     /* Bihari languages*/
    MAKE_ISO_639_CODE('b','i','k'),     /* Bikol*/
    MAKE_ISO_639_CODE('b','i','n'),     /* Bini; Edo*/
    MAKE_ISO_639_CODE('b','i','s'),     /* Bislama*/
    MAKE_ISO_639_CODE('b','l','a'),     /* Siksika*/
    MAKE_ISO_639_CODE('b','m','\0'),    /* Bambara*/
    MAKE_ISO_639_CODE('b','n','\0'),    /* Bengali*/
    MAKE_ISO_639_CODE('b','n','t'),     /* Bantu (Other)*/
    MAKE_ISO_639_CODE('b','o','\0'),    /* Tibetan*/
    MAKE_ISO_639_CODE('b','o','d'),     /* Tibetan*/
    MAKE_ISO_639_CODE('b','o','s'),     /* Bosnian*/
    MAKE_ISO_639_CODE('b','r','\0'),    /* Breton*/
    MAKE_ISO_639_CODE('b','r','a'),     /* Braj*/
    MAKE_ISO_639_CODE('b','r','e'),     /* Breton*/
    MAKE_ISO_639_CODE('b','s','\0'),    /* Bosnian*/
    MAKE_ISO_639_CODE('b','t','k'),     /* Batak languages*/
    MAKE_ISO_639_CODE('b','u','a'),     /* Buriat*/
    MAKE_ISO_639_CODE('b','u','g'),     /* Buginese*/
    MAKE_ISO_639_CODE('b','u','l'),     /* Bulgarian*/
    MAKE_ISO_639_CODE('b','u','r'),     /* Burmese*/
    MAKE_ISO_639_CODE('b','y','n'),     /* Blin; Bilin*/
    MAKE_ISO_639_CODE('c','a','\0'),    /* Catalan; Valencian*/
    MAKE_ISO_639_CODE('c','a','d'),     /* Caddo*/
    MAKE_ISO_639_CODE('c','a','i'),     /* Central American Indian languages*/
    MAKE_ISO_639_CODE('c','a','r'),     /* Galibi Carib*/
    MAKE_ISO_639_CODE('c','a','t'),     /* Catalan; Valencian*/
    MAKE_ISO_639_CODE('c','a','u'),     /* Caucasian languages*/
    MAKE_ISO_639_CODE('c','e','\0'),    /* Chechen*/
    MAKE_ISO_639_CODE('c','e','b'),     /* Cebuano*/
    MAKE_ISO_639_CODE('c','e','l'),     /* Celtic languages*/
    MAKE_ISO_639_CODE('c','e','s'),     /* Czech*/
    MAKE_ISO_639_CODE('c','h','\0'),    /* Chamorro*/
    MAKE_ISO_639_CODE('c','h','a'),     /* Chamorro*/
    MAKE_ISO_639_CODE('c','h','b'),     /* Chibcha*/
    MAKE_ISO_639_CODE('c','h','e'),     /* Chechen*/
    MAKE_ISO_639_CODE('c','h','g'),     /* Chagatai*/
    MAKE_ISO_639_CODE('c','h','i'),     /* Chinese*/
    MAKE_ISO_639_CODE('c','h','k'),     /* Chuukese*/
    MAKE_ISO_639_CODE('c','h','m'),     /* Mari*/
    MAKE_ISO_639_CODE('c','h','n'),     /* Chinook jargon*/
    MAKE_ISO_639_CODE('c','h','o'),     /* Choctaw*/
    MAKE_ISO_639_CODE('c','h','p'),     /* Chipewyan; Dene Suline*/
    MAKE_ISO_639_CODE('c','h','r'),     /* Cherokee*/
    MAKE_ISO_639_CODE('c','h','u'),     /* Church Slavic; Old Slavonic; Church Slavonic; Old Bulgarian; Old Church Slavonic*/
    MAKE_ISO_639_CODE('c','h','v'),     /* Chuvash*/
    MAKE_ISO_639_CODE('c','h','y'),     /* Cheyenne*/
    MAKE_ISO_639_CODE('c','m','c'),     /* Chamic languages*/
    MAKE_ISO_639_CODE('c','o','\0'),    /* Corsican*/
    MAKE_ISO_639_CODE('c','o','p'),     /* Coptic*/
    MAKE_ISO_639_CODE('c','o','r'),     /* Cornish*/
    MAKE_ISO_639_CODE('c','o','s'),     /* Corsican*/
    MAKE_ISO_639_CODE('c','p','e'),     /* "Creoles and pidgins*/
    MAKE_ISO_639_CODE('c','p','f'),     /* "Creoles and pidgins*/
    MAKE_ISO_639_CODE('c','p','p'),     /* "Creoles and pidgins*/
    MAKE_ISO_639_CODE('c','r','\0'),    /* Cree*/
    MAKE_ISO_639_CODE('c','r','e'),     /* Cree*/
    MAKE_ISO_639_CODE('c','r','h'),     /* Crimean Tatar; Crimean Turkish*/
    MAKE_ISO_639_CODE('c','r','p'),     /* Creoles and pidgins*/
    MAKE_ISO_639_CODE('c','s','\0'),    /* Czech*/
    MAKE_ISO_639_CODE('c','s','b'),     /* Kashubian*/
    MAKE_ISO_639_CODE('c','u','\0'),    /* Church Slavic; Old Slavonic; Church Slavonic; Old Bulgarian; Old Church Slavonic*/
    MAKE_ISO_639_CODE('c','u','s'),     /* Cushitic languages*/
    MAKE_ISO_639_CODE('c','v','\0'),    /* Chuvash*/
    MAKE_ISO_639_CODE('c','y','\0'),    /* Welsh*/
    MAKE_ISO_639_CODE('c','y','m'),     /* Welsh*/
    MAKE_ISO_639_CODE('c','z','e'),     /* Czech*/
    MAKE_ISO_639_CODE('d','a','\0'),    /* Danish*/
    MAKE_ISO_639_CODE('d','a','k'),     /* Dakota*/
    MAKE_ISO_639_CODE('d','a','n'),     /* Danish*/
    MAKE_ISO_639_CODE('d','a','r'),     /* Dargwa*/
    MAKE_ISO_639_CODE('d','a','y'),     /* Land Dayak languages*/
    MAKE_ISO_639_CODE('d','e','\0'),    /* German*/
    MAKE_ISO_639_CODE('d','e','l'),     /* Delaware*/
    MAKE_ISO_639_CODE('d','e','n'),     /* Slave (Athapascan)*/
    MAKE_ISO_639_CODE('d','e','u'),     /* German*/
    MAKE_ISO_639_CODE('d','g','r'),     /* Dogrib*/
    MAKE_ISO_639_CODE('d','i','n'),     /* Dinka*/
    MAKE_ISO_639_CODE('d','i','v'),     /* Divehi; Dhivehi; Maldivian*/
    MAKE_ISO_639_CODE('d','o','i'),     /* Dogri*/
    MAKE_ISO_639_CODE('d','r','a'),     /* Dravidian languages*/
    MAKE_ISO_639_CODE('d','s','b'),     /* Lower Sorbian*/
    MAKE_ISO_639_CODE('d','u','a'),     /* Duala*/
    MAKE_ISO_639_CODE('d','u','m'),     /* "Dutch*/
    MAKE_ISO_639_CODE('d','u','t'),     /* Dutch; Flemish*/
    MAKE_ISO_639_CODE('d','v','\0'),    /* Divehi; Dhivehi; Maldivian*/
    MAKE_ISO_639_CODE('d','y','u'),     /* Dyula*/
    MAKE_ISO_639_CODE('d','z','\0'),    /* Dzongkha*/
    MAKE_ISO_639_CODE('d','z','o'),     /* Dzongkha*/
    MAKE_ISO_639_CODE('e','e','\0'),    /* Ewe*/
    MAKE_ISO_639_CODE('e','f','i'),     /* Efik*/
    MAKE_ISO_639_CODE('e','g','y'),     /* Egyptian (Ancient)*/
    MAKE_ISO_639_CODE('e','k','a'),     /* Ekajuk*/
    MAKE_ISO_639_CODE('e','l','\0'),    /* "Greek*/
    MAKE_ISO_639_CODE('e','l','l'),     /* "Greek*/
    MAKE_ISO_639_CODE('e','l','x'),     /* Elamite*/
    MAKE_ISO_639_CODE('e','n','\0'),    /* English*/
    MAKE_ISO_639_CODE('e','n','g'),     /* English*/
    MAKE_ISO_639_CODE('e','n','m'),     /* "English*/
    MAKE_ISO_639_CODE('e','o','\0'),    /* Esperanto*/
    MAKE_ISO_639_CODE('e','p','o'),     /* Esperanto*/
    MAKE_ISO_639_CODE('e','s','\0'),    /* Spanish; Castilian*/
    MAKE_ISO_639_CODE('e','s','t'),     /* Estonian*/
    MAKE_ISO_639_CODE('e','t','\0'),    /* Estonian*/
    MAKE_ISO_639_CODE('e','u','\0'),    /* Basque*/
    MAKE_ISO_639_CODE('e','u','s'),     /* Basque*/
    MAKE_ISO_639_CODE('e','w','e'),     /* Ewe*/
    MAKE_ISO_639_CODE('e','w','o'),     /* Ewondo*/
    MAKE_ISO_639_CODE('f','a','\0'),    /* Persian*/
    MAKE_ISO_639_CODE('f','a','n'),     /* Fang*/
    MAKE_ISO_639_CODE('f','a','o'),     /* Faroese*/
    MAKE_ISO_639_CODE('f','a','s'),     /* Persian*/
    MAKE_ISO_639_CODE('f','a','t'),     /* Fanti*/
    MAKE_ISO_639_CODE('f','f','\0'),    /* Fulah*/
    MAKE_ISO_639_CODE('f','i','\0'),    /* Finnish*/
    MAKE_ISO_639_CODE('f','i','j'),     /* Fijian*/
    MAKE_ISO_639_CODE('f','i','l'),     /* Filipino; Pilipino*/
    MAKE_ISO_639_CODE('f','i','n'),     /* Finnish*/
    MAKE_ISO_639_CODE('f','i','u'),     /* Finno-Ugrian languages*/
    MAKE_ISO_639_CODE('f','j','\0'),    /* Fijian*/
    MAKE_ISO_639_CODE('f','o','\0'),    /* Faroese*/
    MAKE_ISO_639_CODE('f','o','n'),     /* Fon*/
    MAKE_ISO_639_CODE('f','r','\0'),    /* French*/
    MAKE_ISO_639_CODE('f','r','a'),     /* French*/
    MAKE_ISO_639_CODE('f','r','e'),     /* French*/
    MAKE_ISO_639_CODE('f','r','m'),     /* "French*/
    MAKE_ISO_639_CODE('f','r','o'),     /* "French*/
    MAKE_ISO_639_CODE('f','r','r'),     /* Northern Frisian*/
    MAKE_ISO_639_CODE('f','r','s'),     /* Eastern Frisian*/
    MAKE_ISO_639_CODE('f','r','y'),     /* Western Frisian*/
    MAKE_ISO_639_CODE('f','u','l'),     /* Fulah*/
    MAKE_ISO_639_CODE('f','u','r'),     /* Friulian*/
    MAKE_ISO_639_CODE('f','y','\0'),    /* Western Frisian*/
    MAKE_ISO_639_CODE('g','a','\0'),    /* Irish*/
    MAKE_ISO_639_CODE('g','a','a'),     /* Ga*/
    MAKE_ISO_639_CODE('g','a','y'),     /* Gayo*/
    MAKE_ISO_639_CODE('g','b','a'),     /* Gbaya*/
    MAKE_ISO_639_CODE('g','d','\0'),    /* Gaelic; Scottish Gaelic*/
    MAKE_ISO_639_CODE('g','e','m'),     /* Germanic languages*/
    MAKE_ISO_639_CODE('g','e','o'),     /* Georgian*/
    MAKE_ISO_639_CODE('g','e','r'),     /* German*/
    MAKE_ISO_639_CODE('g','e','z'),     /* Geez*/
    MAKE_ISO_639_CODE('g','i','l'),     /* Gilbertese*/
    MAKE_ISO_639_CODE('g','l','\0'),    /* Galician*/
    MAKE_ISO_639_CODE('g','l','a'),     /* Gaelic; Scottish Gaelic*/
    MAKE_ISO_639_CODE('g','l','e'),     /* Irish*/
    MAKE_ISO_639_CODE('g','l','g'),     /* Galician*/
    MAKE_ISO_639_CODE('g','l','v'),     /* Manx*/
    MAKE_ISO_639_CODE('g','m','h'),     /* "German*/
    MAKE_ISO_639_CODE('g','n','\0'),    /* Guarani*/
    MAKE_ISO_639_CODE('g','o','h'),     /* "German*/
    MAKE_ISO_639_CODE('g','o','n'),     /* Gondi*/
    MAKE_ISO_639_CODE('g','o','r'),     /* Gorontalo*/
    MAKE_ISO_639_CODE('g','o','t'),     /* Gothic*/
    MAKE_ISO_639_CODE('g','r','b'),     /* Grebo*/
    MAKE_ISO_639_CODE('g','r','c'),     /* "Greek*/
    MAKE_ISO_639_CODE('g','r','e'),     /* "Greek*/
    MAKE_ISO_639_CODE('g','r','n'),     /* Guarani*/
    MAKE_ISO_639_CODE('g','s','w'),     /* Swiss German; Alemannic; Alsatian*/
    MAKE_ISO_639_CODE('g','u','\0'),    /* Gujarati*/
    MAKE_ISO_639_CODE('g','u','j'),     /* Gujarati*/
    MAKE_ISO_639_CODE('g','v','\0'),    /* Manx*/
    MAKE_ISO_639_CODE('g','w','i'),     /* Gwich'in*/
    MAKE_ISO_639_CODE('h','a','\0'),    /* Hausa*/
    MAKE_ISO_639_CODE('h','a','i'),     /* Haida*/
    MAKE_ISO_639_CODE('h','a','t'),     /* Haitian; Haitian Creole*/
    MAKE_ISO_639_CODE('h','a','u'),     /* Hausa*/
    MAKE_ISO_639_CODE('h','a','w'),     /* Hawaiian*/
    MAKE_ISO_639_CODE('h','e','\0'),    /* Hebrew*/
    MAKE_ISO_639_CODE('h','e','b'),     /* Hebrew*/
    MAKE_ISO_639_CODE('h','e','r'),     /* Herero*/
    MAKE_ISO_639_CODE('h','i','\0'),    /* Hindi*/
    MAKE_ISO_639_CODE('h','i','l'),     /* Hiligaynon*/
    MAKE_ISO_639_CODE('h','i','m'),     /* Himachali languages; Western Pahari languages*/
    MAKE_ISO_639_CODE('h','i','n'),     /* Hindi*/
    MAKE_ISO_639_CODE('h','i','t'),     /* Hittite*/
    MAKE_ISO_639_CODE('h','m','n'),     /* Hmong; Mong*/
    MAKE_ISO_639_CODE('h','m','o'),     /* Hiri Motu*/
    MAKE_ISO_639_CODE('h','o','\0'),    /* Hiri Motu*/
    MAKE_ISO_639_CODE('h','r','\0'),    /* Croatian*/
    MAKE_ISO_639_CODE('h','r','v'),     /* Croatian*/
    MAKE_ISO_639_CODE('h','s','b'),     /* Upper Sorbian*/
    MAKE_ISO_639_CODE('h','t','\0'),    /* Haitian; Haitian Creole*/
    MAKE_ISO_639_CODE('h','u','\0'),    /* Hungarian*/
    MAKE_ISO_639_CODE('h','u','n'),     /* Hungarian*/
    MAKE_ISO_639_CODE('h','u','p'),     /* Hupa*/
    MAKE_ISO_639_CODE('h','y','\0'),    /* Armenian*/
    MAKE_ISO_639_CODE('h','y','e'),     /* Armenian*/
    MAKE_ISO_639_CODE('h','z','\0'),    /* Herero*/
    MAKE_ISO_639_CODE('i','a','\0'),    /* Interlingua (International Auxiliary Language Association)*/
    MAKE_ISO_639_CODE('i','b','a'),     /* Iban*/
    MAKE_ISO_639_CODE('i','b','o'),     /* Igbo*/
    MAKE_ISO_639_CODE('i','c','e'),     /* Icelandic*/
    MAKE_ISO_639_CODE('i','d','\0'),    /* Indonesian*/
    MAKE_ISO_639_CODE('i','d','o'),     /* Ido*/
    MAKE_ISO_639_CODE('i','e','\0'),    /* Interlingue; Occidental*/
    MAKE_ISO_639_CODE('i','g','\0'),    /* Igbo*/
    MAKE_ISO_639_CODE('i','i','\0'),    /* Sichuan Yi; Nuosu*/
    MAKE_ISO_639_CODE('i','i','i'),     /* Sichuan Yi; Nuosu*/
    MAKE_ISO_639_CODE('i','j','o'),     /* Ijo languages*/
    MAKE_ISO_639_CODE('i','k','\0'),    /* Inupiaq*/
    MAKE_ISO_639_CODE('i','k','u'),     /* Inuktitut*/
    MAKE_ISO_639_CODE('i','l','e'),     /* Interlingue; Occidental*/
    MAKE_ISO_639_CODE('i','l','o'),     /* Iloko*/
    MAKE_ISO_639_CODE('i','n','a'),     /* Interlingua (International Auxiliary Language Association)*/
    MAKE_ISO_639_CODE('i','n','c'),     /* Indic languages*/
    MAKE_ISO_639_CODE('i','n','d'),     /* Indonesian*/
    MAKE_ISO_639_CODE('i','n','e'),     /* Indo-European languages*/
    MAKE_ISO_639_CODE('i','n','h'),     /* Ingush*/
    MAKE_ISO_639_CODE('i','o','\0'),    /* Ido*/
    MAKE_ISO_639_CODE('i','p','k'),     /* Inupiaq*/
    MAKE_ISO_639_CODE('i','r','a'),     /* Iranian languages*/
    MAKE_ISO_639_CODE('i','r','o'),     /* Iroquoian languages*/
    MAKE_ISO_639_CODE('i','s','\0'),    /* Icelandic*/
    MAKE_ISO_639_CODE('i','s','l'),     /* Icelandic*/
    MAKE_ISO_639_CODE('i','t','\0'),    /* Italian*/
    MAKE_ISO_639_CODE('i','t','a'),     /* Italian*/
    MAKE_ISO_639_CODE('i','u','\0'),    /* Inuktitut*/
    MAKE_ISO_639_CODE('j','a','\0'),    /* Japanese*/
    MAKE_ISO_639_CODE('j','a','v'),     /* Javanese*/
    MAKE_ISO_639_CODE('j','b','o'),     /* Lojban*/
    MAKE_ISO_639_CODE('j','p','n'),     /* Japanese*/
    MAKE_ISO_639_CODE('j','p','r'),     /* Judeo-Persian*/
    MAKE_ISO_639_CODE('j','r','b'),     /* Judeo-Arabic*/
    MAKE_ISO_639_CODE('j','v','\0'),    /* Javanese*/
    MAKE_ISO_639_CODE('k','a','\0'),    /* Georgian*/
    MAKE_ISO_639_CODE('k','a','a'),     /* Kara-Kalpak*/
    MAKE_ISO_639_CODE('k','a','b'),     /* Kabyle*/
    MAKE_ISO_639_CODE('k','a','c'),     /* Kachin; Jingpho*/
    MAKE_ISO_639_CODE('k','a','l'),     /* Kalaallisut; Greenlandic*/
    MAKE_ISO_639_CODE('k','a','m'),     /* Kamba*/
    MAKE_ISO_639_CODE('k','a','n'),     /* Kannada*/
    MAKE_ISO_639_CODE('k','a','r'),     /* Karen languages*/
    MAKE_ISO_639_CODE('k','a','s'),     /* Kashmiri*/
    MAKE_ISO_639_CODE('k','a','t'),     /* Georgian*/
    MAKE_ISO_639_CODE('k','a','u'),     /* Kanuri*/
    MAKE_ISO_639_CODE('k','a','w'),     /* Kawi*/
    MAKE_ISO_639_CODE('k','a','z'),     /* Kazakh*/
    MAKE_ISO_639_CODE('k','b','d'),     /* Kabardian*/
    MAKE_ISO_639_CODE('k','g','\0'),    /* Kongo*/
    MAKE_ISO_639_CODE('k','h','a'),     /* Khasi*/
    MAKE_ISO_639_CODE('k','h','i'),     /* Khoisan languages*/
    MAKE_ISO_639_CODE('k','h','m'),     /* Central Khmer*/
    MAKE_ISO_639_CODE('k','h','o'),     /* Khotanese; Sakan*/
    MAKE_ISO_639_CODE('k','i','\0'),    /* Kikuyu; Gikuyu*/
    MAKE_ISO_639_CODE('k','i','k'),     /* Kikuyu; Gikuyu*/
    MAKE_ISO_639_CODE('k','i','n'),     /* Kinyarwanda*/
    MAKE_ISO_639_CODE('k','i','r'),     /* Kirghiz; Kyrgyz*/
    MAKE_ISO_639_CODE('k','j','\0'),    /* Kuanyama; Kwanyama*/
    MAKE_ISO_639_CODE('k','k','\0'),    /* Kazakh*/
    MAKE_ISO_639_CODE('k','l','\0'),    /* Kalaallisut; Greenlandic*/
    MAKE_ISO_639_CODE('k','m','\0'),    /* Central Khmer*/
    MAKE_ISO_639_CODE('k','m','b'),     /* Kimbundu*/
    MAKE_ISO_639_CODE('k','n','\0'),    /* Kannada*/
    MAKE_ISO_639_CODE('k','o','\0'),    /* Korean*/
    MAKE_ISO_639_CODE('k','o','k'),     /* Konkani*/
    MAKE_ISO_639_CODE('k','o','m'),     /* Komi*/
    MAKE_ISO_639_CODE('k','o','n'),     /* Kongo*/
    MAKE_ISO_639_CODE('k','o','r'),     /* Korean*/
    MAKE_ISO_639_CODE('k','o','s'),     /* Kosraean*/
    MAKE_ISO_639_CODE('k','p','e'),     /* Kpelle*/
    MAKE_ISO_639_CODE('k','r','\0'),    /* Kanuri*/
    MAKE_ISO_639_CODE('k','r','c'),     /* Karachay-Balkar*/
    MAKE_ISO_639_CODE('k','r','l'),     /* Karelian*/
    MAKE_ISO_639_CODE('k','r','o'),     /* Kru languages*/
    MAKE_ISO_639_CODE('k','r','u'),     /* Kurukh*/
    MAKE_ISO_639_CODE('k','s','\0'),    /* Kashmiri*/
    MAKE_ISO_639_CODE('k','u','\0'),    /* Kurdish*/
    MAKE_ISO_639_CODE('k','u','a'),     /* Kuanyama; Kwanyama*/
    MAKE_ISO_639_CODE('k','u','m'),     /* Kumyk*/
    MAKE_ISO_639_CODE('k','u','r'),     /* Kurdish*/
    MAKE_ISO_639_CODE('k','u','t'),     /* Kutenai*/
    MAKE_ISO_639_CODE('k','v','\0'),    /* Komi*/
    MAKE_ISO_639_CODE('k','w','\0'),    /* Cornish*/
    MAKE_ISO_639_CODE('k','y','\0'),    /* Kirghiz; Kyrgyz*/
    MAKE_ISO_639_CODE('l','a','\0'),    /* Latin*/
    MAKE_ISO_639_CODE('l','a','d'),     /* Ladino*/
    MAKE_ISO_639_CODE('l','a','h'),     /* Lahnda*/
    MAKE_ISO_639_CODE('l','a','m'),     /* Lamba*/
    MAKE_ISO_639_CODE('l','a','o'),     /* Lao*/
    MAKE_ISO_639_CODE('l','a','t'),     /* Latin*/
    MAKE_ISO_639_CODE('l','a','v'),     /* Latvian*/
    MAKE_ISO_639_CODE('l','b','\0'),    /* Luxembourgish; Letzeburgesch*/
    MAKE_ISO_639_CODE('l','e','z'),     /* Lezghian*/
    MAKE_ISO_639_CODE('l','g','\0'),    /* Ganda*/
    MAKE_ISO_639_CODE('l','i','\0'),    /* Limburgan; Limburger; Limburgish*/
    MAKE_ISO_639_CODE('l','i','m'),     /* Limburgan; Limburger; Limburgish*/
    MAKE_ISO_639_CODE('l','i','n'),     /* Lingala*/
    MAKE_ISO_639_CODE('l','i','t'),     /* Lithuanian*/
    MAKE_ISO_639_CODE('l','n','\0'),    /* Lingala*/
    MAKE_ISO_639_CODE('l','o','\0'),    /* Lao*/
    MAKE_ISO_639_CODE('l','o','l'),     /* Mongo*/
    MAKE_ISO_639_CODE('l','o','z'),     /* Lozi*/
    MAKE_ISO_639_CODE('l','t','\0'),    /* Lithuanian*/
    MAKE_ISO_639_CODE('l','t','z'),     /* Luxembourgish; Letzeburgesch*/
    MAKE_ISO_639_CODE('l','u','\0'),    /* Luba-Katanga*/
    MAKE_ISO_639_CODE('l','u','a'),     /* Luba-Lulua*/
    MAKE_ISO_639_CODE('l','u','b'),     /* Luba-Katanga*/
    MAKE_ISO_639_CODE('l','u','g'),     /* Ganda*/
    MAKE_ISO_639_CODE('l','u','i'),     /* Luiseno*/
    MAKE_ISO_639_CODE('l','u','n'),     /* Lunda*/
    MAKE_ISO_639_CODE('l','u','o'),     /* Luo (Kenya and Tanzania)*/
    MAKE_ISO_639_CODE('l','u','s'),     /* Lushai*/
    MAKE_ISO_639_CODE('l','v','\0'),    /* Latvian*/
    MAKE_ISO_639_CODE('m','a','c'),     /* Macedonian*/
    MAKE_ISO_639_CODE('m','a','d'),     /* Madurese*/
    MAKE_ISO_639_CODE('m','a','g'),     /* Magahi*/
    MAKE_ISO_639_CODE('m','a','h'),     /* Marshallese*/
    MAKE_ISO_639_CODE('m','a','i'),     /* Maithili*/
    MAKE_ISO_639_CODE('m','a','k'),     /* Makasar*/
    MAKE_ISO_639_CODE('m','a','l'),     /* Malayalam*/
    MAKE_ISO_639_CODE('m','a','n'),     /* Mandingo*/
    MAKE_ISO_639_CODE('m','a','o'),     /* Maori*/
    MAKE_ISO_639_CODE('m','a','p'),     /* Austronesian languages*/
    MAKE_ISO_639_CODE('m','a','r'),     /* Marathi*/
    MAKE_ISO_639_CODE('m','a','s'),     /* Masai*/
    MAKE_ISO_639_CODE('m','a','y'),     /* Malay*/
    MAKE_ISO_639_CODE('m','d','f'),     /* Moksha*/
    MAKE_ISO_639_CODE('m','d','r'),     /* Mandar*/
    MAKE_ISO_639_CODE('m','e','n'),     /* Mende*/
    MAKE_ISO_639_CODE('m','g','\0'),    /* Malagasy*/
    MAKE_ISO_639_CODE('m','g','a'),     /* "Irish*/
    MAKE_ISO_639_CODE('m','h','\0'),    /* Marshallese*/
    MAKE_ISO_639_CODE('m','i','\0'),    /* Maori*/
    MAKE_ISO_639_CODE('m','i','c'),     /* Mi'kmaq; Micmac*/
    MAKE_ISO_639_CODE('m','i','n'),     /* Minangkabau*/
    MAKE_ISO_639_CODE('m','i','s'),     /* Uncoded languages*/
    MAKE_ISO_639_CODE('m','k','\0'),    /* Macedonian*/
    MAKE_ISO_639_CODE('m','k','d'),     /* Macedonian*/
    MAKE_ISO_639_CODE('m','k','h'),     /* Mon-Khmer languages*/
    MAKE_ISO_639_CODE('m','l','\0'),    /* Malayalam*/
    MAKE_ISO_639_CODE('m','l','g'),     /* Malagasy*/
    MAKE_ISO_639_CODE('m','l','t'),     /* Maltese*/
    MAKE_ISO_639_CODE('m','n','\0'),    /* Mongolian*/
    MAKE_ISO_639_CODE('m','n','c'),     /* Manchu*/
    MAKE_ISO_639_CODE('m','n','i'),     /* Manipuri*/
    MAKE_ISO_639_CODE('m','n','o'),     /* Manobo languages*/
    MAKE_ISO_639_CODE('m','o','h'),     /* Mohawk*/
    MAKE_ISO_639_CODE('m','o','n'),     /* Mongolian*/
    MAKE_ISO_639_CODE('m','o','s'),     /* Mossi*/
    MAKE_ISO_639_CODE('m','r','\0'),    /* Marathi*/
    MAKE_ISO_639_CODE('m','r','i'),     /* Maori*/
    MAKE_ISO_639_CODE('m','s','\0'),    /* Malay*/
    MAKE_ISO_639_CODE('m','s','a'),     /* Malay*/
    MAKE_ISO_639_CODE('m','t','\0'),    /* Maltese*/
    MAKE_ISO_639_CODE('m','u','l'),     /* Multiple languages*/
    MAKE_ISO_639_CODE('m','u','n'),     /* Munda languages*/
    MAKE_ISO_639_CODE('m','u','s'),     /* Creek*/
    MAKE_ISO_639_CODE('m','w','l'),     /* Mirandese*/
    MAKE_ISO_639_CODE('m','w','r'),     /* Marwari*/
    MAKE_ISO_639_CODE('m','y','\0'),    /* Burmese*/
    MAKE_ISO_639_CODE('m','y','a'),     /* Burmese*/
    MAKE_ISO_639_CODE('m','y','n'),     /* Mayan languages*/
    MAKE_ISO_639_CODE('m','y','v'),     /* Erzya*/
    MAKE_ISO_639_CODE('n','a','\0'),    /* Nauru*/
    MAKE_ISO_639_CODE('n','a','h'),     /* Nahuatl languages*/
    MAKE_ISO_639_CODE('n','a','i'),     /* North American Indian languages*/
    MAKE_ISO_639_CODE('n','a','p'),     /* Neapolitan*/
    MAKE_ISO_639_CODE('n','a','u'),     /* Nauru*/
    MAKE_ISO_639_CODE('n','a','v'),     /* Navajo; Navaho*/
    MAKE_ISO_639_CODE('n','b','\0'),    /* "BokmÃ¥l*/
    MAKE_ISO_639_CODE('n','b','l'),     /* "Ndebele*/
    MAKE_ISO_639_CODE('n','d','\0'),    /* "Ndebele*/
    MAKE_ISO_639_CODE('n','d','e'),     /* "Ndebele*/
    MAKE_ISO_639_CODE('n','d','o'),     /* Ndonga*/
    MAKE_ISO_639_CODE('n','d','s'),     /* "Low German; Low Saxon; German*/
    MAKE_ISO_639_CODE('n','e','\0'),    /* Nepali*/
    MAKE_ISO_639_CODE('n','e','p'),     /* Nepali*/
    MAKE_ISO_639_CODE('n','e','w'),     /* Nepal Bhasa; Newari*/
    MAKE_ISO_639_CODE('n','g','\0'),    /* Ndonga*/
    MAKE_ISO_639_CODE('n','i','a'),     /* Nias*/
    MAKE_ISO_639_CODE('n','i','c'),     /* Niger-Kordofanian languages*/
    MAKE_ISO_639_CODE('n','i','u'),     /* Niuean*/
    MAKE_ISO_639_CODE('n','l','\0'),    /* Dutch; Flemish*/
    MAKE_ISO_639_CODE('n','l','d'),     /* Dutch; Flemish*/
    MAKE_ISO_639_CODE('n','n','\0'),    /* "Norwegian Nynorsk; Nynorsk*/
    MAKE_ISO_639_CODE('n','n','o'),     /* "Norwegian Nynorsk; Nynorsk*/
    MAKE_ISO_639_CODE('n','o','\0'),    /* Norwegian*/
    MAKE_ISO_639_CODE('n','o','b'),     /* "BokmÃ¥l*/
    MAKE_ISO_639_CODE('n','o','g'),     /* Nogai*/
    MAKE_ISO_639_CODE('n','o','n'),     /* "Norse*/
    MAKE_ISO_639_CODE('n','o','r'),     /* Norwegian*/
    MAKE_ISO_639_CODE('n','q','o'),     /* N'Ko*/
    MAKE_ISO_639_CODE('n','r','\0'),    /* "Ndebele*/
    MAKE_ISO_639_CODE('n','s','o'),     /* Pedi; Sepedi; Northern Sotho*/
    MAKE_ISO_639_CODE('n','u','b'),     /* Nubian languages*/
    MAKE_ISO_639_CODE('n','v','\0'),    /* Navajo; Navaho*/
    MAKE_ISO_639_CODE('n','w','c'),     /* Classical Newari; Old Newari; Classical Nepal Bhasa*/
    MAKE_ISO_639_CODE('n','y','\0'),    /* Chichewa; Chewa; Nyanja*/
    MAKE_ISO_639_CODE('n','y','a'),     /* Chichewa; Chewa; Nyanja*/
    MAKE_ISO_639_CODE('n','y','m'),     /* Nyamwezi*/
    MAKE_ISO_639_CODE('n','y','n'),     /* Nyankole*/
    MAKE_ISO_639_CODE('n','y','o'),     /* Nyoro*/
    MAKE_ISO_639_CODE('n','z','i'),     /* Nzima*/
    MAKE_ISO_639_CODE('o','c','\0'),    /* Occitan (post 1500); ProvenÃ§al*/
    MAKE_ISO_639_CODE('o','c','i'),     /* Occitan (post 1500); ProvenÃ§al*/
    MAKE_ISO_639_CODE('o','j','\0'),    /* Ojibwa*/
    MAKE_ISO_639_CODE('o','j','i'),     /* Ojibwa*/
    MAKE_ISO_639_CODE('o','m','\0'),    /* Oromo*/
    MAKE_ISO_639_CODE('o','r','\0'),    /* Oriya*/
    MAKE_ISO_639_CODE('o','r','i'),     /* Oriya*/
    MAKE_ISO_639_CODE('o','r','m'),     /* Oromo*/
    MAKE_ISO_639_CODE('o','s','\0'),    /* Ossetian; Ossetic*/
    MAKE_ISO_639_CODE('o','s','a'),     /* Osage*/
    MAKE_ISO_639_CODE('o','s','s'),     /* Ossetian; Ossetic*/
    MAKE_ISO_639_CODE('o','t','a'),     /* "Turkish*/
    MAKE_ISO_639_CODE('o','t','o'),     /* Otomian languages*/
    MAKE_ISO_639_CODE('p','a','\0'),    /* Panjabi; Punjabi*/
    MAKE_ISO_639_CODE('p','a','a'),     /* Papuan languages*/
    MAKE_ISO_639_CODE('p','a','g'),     /* Pangasinan*/
    MAKE_ISO_639_CODE('p','a','l'),     /* Pahlavi*/
    MAKE_ISO_639_CODE('p','a','m'),     /* Pampanga; Kapampangan*/
    MAKE_ISO_639_CODE('p','a','n'),     /* Panjabi; Punjabi*/
    MAKE_ISO_639_CODE('p','a','p'),     /* Papiamento*/
    MAKE_ISO_639_CODE('p','a','u'),     /* Palauan*/
    MAKE_ISO_639_CODE('p','e','o'),     /* "Persian*/
    MAKE_ISO_639_CODE('p','e','r'),     /* Persian*/
    MAKE_ISO_639_CODE('p','h','i'),     /* Philippine languages*/
    MAKE_ISO_639_CODE('p','h','n'),     /* Phoenician*/
    MAKE_ISO_639_CODE('p','i','\0'),    /* Pali*/
    MAKE_ISO_639_CODE('p','l','\0'),    /* Polish*/
    MAKE_ISO_639_CODE('p','l','i'),     /* Pali*/
    MAKE_ISO_639_CODE('p','o','l'),     /* Polish*/
    MAKE_ISO_639_CODE('p','o','n'),     /* Pohnpeian*/
    MAKE_ISO_639_CODE('p','o','r'),     /* Portuguese*/
    MAKE_ISO_639_CODE('p','r','a'),     /* Prakrit languages*/
    MAKE_ISO_639_CODE('p','r','o'),     /* "ProvenÃ§al*/
    MAKE_ISO_639_CODE('p','s','\0'),    /* Pushto; Pashto*/
    MAKE_ISO_639_CODE('p','t','\0'),    /* Portuguese*/
    MAKE_ISO_639_CODE('p','u','s'),     /* Pushto; Pashto*/
    MAKE_ISO_639_CODE('q','u','\0'),    /* Quechua*/
    MAKE_ISO_639_CODE('q','u','e'),     /* Quechua*/
    MAKE_ISO_639_CODE('r','a','j'),     /* Rajasthani*/
    MAKE_ISO_639_CODE('r','a','p'),     /* Rapanui*/
    MAKE_ISO_639_CODE('r','a','r'),     /* Rarotongan; Cook Islands Maori*/
    MAKE_ISO_639_CODE('r','m','\0'),    /* Romansh*/
    MAKE_ISO_639_CODE('r','n','\0'),    /* Rundi*/
    MAKE_ISO_639_CODE('r','o','\0'),    /* Romanian; Moldavian; Moldovan*/
    MAKE_ISO_639_CODE('r','o','a'),     /* Romance languages*/
    MAKE_ISO_639_CODE('r','o','h'),     /* Romansh*/
    MAKE_ISO_639_CODE('r','o','m'),     /* Romany*/
    MAKE_ISO_639_CODE('r','o','n'),     /* Romanian; Moldavian; Moldovan*/
    MAKE_ISO_639_CODE('r','u','\0'),    /* Russian*/
    MAKE_ISO_639_CODE('r','u','m'),     /* Romanian; Moldavian; Moldovan*/
    MAKE_ISO_639_CODE('r','u','n'),     /* Rundi*/
    MAKE_ISO_639_CODE('r','u','p'),     /* Aromanian; Arumanian; Macedo-Romanian*/
    MAKE_ISO_639_CODE('r','u','s'),     /* Russian*/
    MAKE_ISO_639_CODE('r','w','\0'),    /* Kinyarwanda*/
    MAKE_ISO_639_CODE('s','a','\0'),    /* Sanskrit*/
    MAKE_ISO_639_CODE('s','a','d'),     /* Sandawe*/
    MAKE_ISO_639_CODE('s','a','g'),     /* Sango*/
    MAKE_ISO_639_CODE('s','a','h'),     /* Yakut*/
    MAKE_ISO_639_CODE('s','a','i'),     /* South American Indian (Other)*/
    MAKE_ISO_639_CODE('s','a','l'),     /* Salishan languages*/
    MAKE_ISO_639_CODE('s','a','m'),     /* Samaritan Aramaic*/
    MAKE_ISO_639_CODE('s','a','n'),     /* Sanskrit*/
    MAKE_ISO_639_CODE('s','a','s'),     /* Sasak*/
    MAKE_ISO_639_CODE('s','a','t'),     /* Santali*/
    MAKE_ISO_639_CODE('s','c','\0'),    /* Sardinian*/
    MAKE_ISO_639_CODE('s','c','n'),     /* Sicilian*/
    MAKE_ISO_639_CODE('s','c','o'),     /* Scots*/
    MAKE_ISO_639_CODE('s','d','\0'),    /* Sindhi*/
    MAKE_ISO_639_CODE('s','e','\0'),    /* Northern Sami*/
    MAKE_ISO_639_CODE('s','e','l'),     /* Selkup*/
    MAKE_ISO_639_CODE('s','e','m'),     /* Semitic languages*/
    MAKE_ISO_639_CODE('s','g','\0'),    /* Sango*/
    MAKE_ISO_639_CODE('s','g','a'),     /* "Irish*/
    MAKE_ISO_639_CODE('s','g','n'),     /* Sign Languages*/
    MAKE_ISO_639_CODE('s','h','n'),     /* Shan*/
    MAKE_ISO_639_CODE('s','i','\0'),    /* Sinhala; Sinhalese*/
    MAKE_ISO_639_CODE('s','i','d'),     /* Sidamo*/
    MAKE_ISO_639_CODE('s','i','n'),     /* Sinhala; Sinhalese*/
    MAKE_ISO_639_CODE('s','i','o'),     /* Siouan languages*/
    MAKE_ISO_639_CODE('s','i','t'),     /* Sino-Tibetan languages*/
    MAKE_ISO_639_CODE('s','k','\0'),    /* Slovak*/
    MAKE_ISO_639_CODE('s','l','\0'),    /* Slovenian*/
    MAKE_ISO_639_CODE('s','l','a'),     /* Slavic languages*/
    MAKE_ISO_639_CODE('s','l','k'),     /* Slovak*/
    MAKE_ISO_639_CODE('s','l','o'),     /* Slovak*/
    MAKE_ISO_639_CODE('s','l','v'),     /* Slovenian*/
    MAKE_ISO_639_CODE('s','m','\0'),    /* Samoan*/
    MAKE_ISO_639_CODE('s','m','a'),     /* Southern Sami*/
    MAKE_ISO_639_CODE('s','m','e'),     /* Northern Sami*/
    MAKE_ISO_639_CODE('s','m','i'),     /* Sami languages*/
    MAKE_ISO_639_CODE('s','m','j'),     /* Lule Sami*/
    MAKE_ISO_639_CODE('s','m','n'),     /* Inari Sami*/
    MAKE_ISO_639_CODE('s','m','o'),     /* Samoan*/
    MAKE_ISO_639_CODE('s','m','s'),     /* Skolt Sami*/
    MAKE_ISO_639_CODE('s','n','\0'),    /* Shona*/
    MAKE_ISO_639_CODE('s','n','a'),     /* Shona*/
    MAKE_ISO_639_CODE('s','n','d'),     /* Sindhi*/
    MAKE_ISO_639_CODE('s','n','k'),     /* Soninke*/
    MAKE_ISO_639_CODE('s','o','\0'),    /* Somali*/
    MAKE_ISO_639_CODE('s','o','g'),     /* Sogdian*/
    MAKE_ISO_639_CODE('s','o','m'),     /* Somali*/
    MAKE_ISO_639_CODE('s','o','n'),     /* Songhai languages*/
    MAKE_ISO_639_CODE('s','o','t'),     /* "Sotho*/
    MAKE_ISO_639_CODE('s','p','a'),     /* Spanish; Castilian*/
    MAKE_ISO_639_CODE('s','q','\0'),    /* Albanian*/
    MAKE_ISO_639_CODE('s','q','i'),     /* Albanian*/
    MAKE_ISO_639_CODE('s','r','\0'),    /* Serbian*/
    MAKE_ISO_639_CODE('s','r','d'),     /* Sardinian*/
    MAKE_ISO_639_CODE('s','r','n'),     /* Sranan Tongo*/
    MAKE_ISO_639_CODE('s','r','p'),     /* Serbian*/
    MAKE_ISO_639_CODE('s','r','r'),     /* Serer*/
    MAKE_ISO_639_CODE('s','s','\0'),    /* Swati*/
    MAKE_ISO_639_CODE('s','s','a'),     /* Nilo-Saharan languages*/
    MAKE_ISO_639_CODE('s','s','w'),     /* Swati*/
    MAKE_ISO_639_CODE('s','t','\0'),    /* "Sotho*/
    MAKE_ISO_639_CODE('s','u','\0'),    /* Sundanese*/
    MAKE_ISO_639_CODE('s','u','k'),     /* Sukuma*/
    MAKE_ISO_639_CODE('s','u','n'),     /* Sundanese*/
    MAKE_ISO_639_CODE('s','u','s'),     /* Susu*/
    MAKE_ISO_639_CODE('s','u','x'),     /* Sumerian*/
    MAKE_ISO_639_CODE('s','v','\0'),    /* Swedish*/
    MAKE_ISO_639_CODE('s','w','\0'),    /* Swahili*/
    MAKE_ISO_639_CODE('s','w','a'),     /* Swahili*/
    MAKE_ISO_639_CODE('s','w','e'),     /* Swedish*/
    MAKE_ISO_639_CODE('s','y','c'),     /* Classical Syriac*/
    MAKE_ISO_639_CODE('s','y','r'),     /* Syriac*/
    MAKE_ISO_639_CODE('t','a','\0'),    /* Tamil*/
    MAKE_ISO_639_CODE('t','a','h'),     /* Tahitian*/
    MAKE_ISO_639_CODE('t','a','i'),     /* Tai languages*/
    MAKE_ISO_639_CODE('t','a','m'),     /* Tamil*/
    MAKE_ISO_639_CODE('t','a','t'),     /* Tatar*/
    MAKE_ISO_639_CODE('t','e','\0'),    /* Telugu*/
    MAKE_ISO_639_CODE('t','e','l'),     /* Telugu*/
    MAKE_ISO_639_CODE('t','e','m'),     /* Timne*/
    MAKE_ISO_639_CODE('t','e','r'),     /* Tereno*/
    MAKE_ISO_639_CODE('t','e','t'),     /* Tetum*/
    MAKE_ISO_639_CODE('t','g','\0'),    /* Tajik*/
    MAKE_ISO_639_CODE('t','g','k'),     /* Tajik*/
    MAKE_ISO_639_CODE('t','g','l'),     /* Tagalog*/
    MAKE_ISO_639_CODE('t','h','\0'),    /* Thai*/
    MAKE_ISO_639_CODE('t','h','a'),     /* Thai*/
    MAKE_ISO_639_CODE('t','i','\0'),    /* Tigrinya*/
    MAKE_ISO_639_CODE('t','i','b'),     /* Tibetan*/
    MAKE_ISO_639_CODE('t','i','g'),     /* Tigre*/
    MAKE_ISO_639_CODE('t','i','r'),     /* Tigrinya*/
    MAKE_ISO_639_CODE('t','i','v'),     /* Tiv*/
    MAKE_ISO_639_CODE('t','k','\0'),    /* Turkmen*/
    MAKE_ISO_639_CODE('t','k','l'),     /* Tokelau*/
    MAKE_ISO_639_CODE('t','l','\0'),    /* Tagalog*/
    MAKE_ISO_639_CODE('t','l','h'),     /* Klingon; tlhIngan-Hol*/
    MAKE_ISO_639_CODE('t','l','i'),     /* Tlingit*/
    MAKE_ISO_639_CODE('t','m','h'),     /* Tamashek*/
    MAKE_ISO_639_CODE('t','n','\0'),    /* Tswana*/
    MAKE_ISO_639_CODE('t','o','\0'),    /* Tonga (Tonga Islands)*/
    MAKE_ISO_639_CODE('t','o','g'),     /* Tonga (Nyasa)*/
    MAKE_ISO_639_CODE('t','o','n'),     /* Tonga (Tonga Islands)*/
    MAKE_ISO_639_CODE('t','p','i'),     /* Tok Pisin*/
    MAKE_ISO_639_CODE('t','r','\0'),    /* Turkish*/
    MAKE_ISO_639_CODE('t','s','\0'),    /* Tsonga*/
    MAKE_ISO_639_CODE('t','s','i'),     /* Tsimshian*/
    MAKE_ISO_639_CODE('t','s','n'),     /* Tswana*/
    MAKE_ISO_639_CODE('t','s','o'),     /* Tsonga*/
    MAKE_ISO_639_CODE('t','t','\0'),    /* Tatar*/
    MAKE_ISO_639_CODE('t','u','k'),     /* Turkmen*/
    MAKE_ISO_639_CODE('t','u','m'),     /* Tumbuka*/
    MAKE_ISO_639_CODE('t','u','p'),     /* Tupi languages*/
    MAKE_ISO_639_CODE('t','u','r'),     /* Turkish*/
    MAKE_ISO_639_CODE('t','u','t'),     /* Altaic languages*/
    MAKE_ISO_639_CODE('t','v','l'),     /* Tuvalu*/
    MAKE_ISO_639_CODE('t','w','\0'),    /* Twi*/
    MAKE_ISO_639_CODE('t','w','i'),     /* Twi*/
    MAKE_ISO_639_CODE('t','y','\0'),    /* Tahitian*/
    MAKE_ISO_639_CODE('t','y','v'),     /* Tuvinian*/
    MAKE_ISO_639_CODE('u','d','m'),     /* Udmurt*/
    MAKE_ISO_639_CODE('u','g','\0'),    /* Uighur; Uyghur*/
    MAKE_ISO_639_CODE('u','g','a'),     /* Ugaritic*/
    MAKE_ISO_639_CODE('u','i','g'),     /* Uighur; Uyghur*/
    MAKE_ISO_639_CODE('u','k','\0'),    /* Ukrainian*/
    MAKE_ISO_639_CODE('u','k','r'),     /* Ukrainian*/
    MAKE_ISO_639_CODE('u','m','b'),     /* Umbundu*/
    MAKE_ISO_639_CODE('u','n','d'),     /* Undetermined*/
    MAKE_ISO_639_CODE('u','r','\0'),    /* Urdu*/
    MAKE_ISO_639_CODE('u','r','d'),     /* Urdu*/
    MAKE_ISO_639_CODE('u','z','\0'),    /* Uzbek*/
    MAKE_ISO_639_CODE('u','z','b'),     /* Uzbek*/
    MAKE_ISO_639_CODE('v','a','i'),     /* Vai*/
    MAKE_ISO_639_CODE('v','e','\0'),    /* Venda*/
    MAKE_ISO_639_CODE('v','e','n'),     /* Venda*/
    MAKE_ISO_639_CODE('v','i','\0'),    /* Vietnamese*/
    MAKE_ISO_639_CODE('v','i','e'),     /* Vietnamese*/
    MAKE_ISO_639_CODE('v','o','\0'),    /* VolapÃ¼k*/
    MAKE_ISO_639_CODE('v','o','l'),     /* VolapÃ¼k*/
    MAKE_ISO_639_CODE('v','o','t'),     /* Votic*/
    MAKE_ISO_639_CODE('w','a','\0'),    /* Walloon*/
    MAKE_ISO_639_CODE('w','a','k'),     /* Wakashan languages*/
    MAKE_ISO_639_CODE('w','a','l'),     /* Walamo*/
    MAKE_ISO_639_CODE('w','a','r'),     /* Waray*/
    MAKE_ISO_639_CODE('w','a','s'),     /* Washo*/
    MAKE_ISO_639_CODE('w','e','l'),     /* Welsh*/
    MAKE_ISO_639_CODE('w','e','n'),     /* Sorbian languages*/
    MAKE_ISO_639_CODE('w','l','n'),     /* Walloon*/
    MAKE_ISO_639_CODE('w','o','\0'),    /* Wolof*/
    MAKE_ISO_639_CODE('w','o','l'),     /* Wolof*/
    MAKE_ISO_639_CODE('x','a','l'),     /* Kalmyk; Oirat*/
    MAKE_ISO_639_CODE('x','h','\0'),    /* Xhosa*/
    MAKE_ISO_639_CODE('x','h','o'),     /* Xhosa*/
    MAKE_ISO_639_CODE('y','a','o'),     /* Yao*/
    MAKE_ISO_639_CODE('y','a','p'),     /* Yapese*/
    MAKE_ISO_639_CODE('y','i','\0'),    /* Yiddish*/
    MAKE_ISO_639_CODE('y','i','d'),     /* Yiddish*/
    MAKE_ISO_639_CODE('y','o','\0'),    /* Yoruba*/
    MAKE_ISO_639_CODE('y','o','r'),     /* Yoruba*/
    MAKE_ISO_639_CODE('y','p','k'),     /* Yupik languages*/
    MAKE_ISO_639_CODE('z','a','\0'),    /* Zhuang; Chuang*/
    MAKE_ISO_639_CODE('z','a','p'),     /* Zapotec*/
    MAKE_ISO_639_CODE('z','b','l'),     /* Blissymbols; Blissymbolics; Bliss*/
    MAKE_ISO_639_CODE('z','e','n'),     /* Zenaga*/
    MAKE_ISO_639_CODE('z','g','h'),     /* Standard Moroccan Tamazight*/
    MAKE_ISO_639_CODE('z','h','\0'),    /* Chinese*/
    MAKE_ISO_639_CODE('z','h','a'),     /* Zhuang; Chuang*/
    MAKE_ISO_639_CODE('z','h','o'),     /* Chinese*/
    MAKE_ISO_639_CODE('z','n','d'),     /* Zande languages*/
    MAKE_ISO_639_CODE('z','u','\0'),    /* Zulu*/
    MAKE_ISO_639_CODE('z','u','l'),     /* Zulu*/
    MAKE_ISO_639_CODE('z','u','n'),     /* Zuni*/
    MAKE_ISO_639_CODE('z','x','x'),     /* No linguistic content; Not applicable*/
    MAKE_ISO_639_CODE('z','z','a'),     /* Zaza; Dimili; Dimli; Kirdki; Kirmanjki; Zazaki*/
};


/**
 * @def NUM_ISO_639_CODES
 * @brief size of ISO_639_CODES table
 */
#define NUM_ISO_639_CODES (sizeof(ISO_639_CODES)/sizeof(ISO_639_CODES[0]))


static
int
compare
    (const void *key
    ,const void *datum
    )
{
    return (int)(*(uint32_t*)key - *(uint32_t*)datum);
}


static
dlb_pmd_success
dlb_pmd_check_iso639
    (pmd_langcode code
    )
{
    return NULL == bsearch(&code,
                           ISO_639_CODES,
                           NUM_ISO_639_CODES,
                           sizeof(uint32_t),
                           compare);
}


dlb_pmd_success
pmd_decode_langcode
    (const char *str 
    ,pmd_langcode *code
    )
{
    if (NULL != str)
    {
        size_t len = strlen(str);
        switch (len)
        {
        case 2:
            *code = (str[0] << 24) | (str[1] << 16);
            if (!dlb_pmd_check_iso639(*code))
            {
                return PMD_SUCCESS;
            }
            break;
        case 3:
            *code = (str[0] << 24) | (str[1] << 16) | (str[2] << 8);
            if (!dlb_pmd_check_iso639(*code))
            {
                return PMD_SUCCESS;
            }
            break;
        default:
            break;
        }
    }
    return PMD_FAIL;
}


void
pmd_langcode_string
    (pmd_langcode code
    ,char (*outptr)[4]
    )
{
    char *out = *outptr;

    out[0] = (char)(code >> 24);
    out[1] = (char)(code >> 16);
    out[2] = (char)(code >>  8);
    out[3] = '\0';
}


unsigned int
pmd_langcode_count
    (void
    )
{
    return NUM_ISO_639_CODES;
}


dlb_pmd_success
pmd_langcode_select
    (unsigned int number
    ,char (*outptr)[4]
    )
{
    if (number < NUM_ISO_639_CODES)
    {
        uint32_t code = ISO_639_CODES[number];
        pmd_langcode_string(code, outptr);
        return PMD_SUCCESS;
    }
    return PMD_FAIL;
}
