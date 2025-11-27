/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated documentation URLs from specs.amwa.tv to GitHub repositories.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (c) 2019-2025, Dolby Laboratories Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NMOS_CONNECTION_RESOURCES_H
#define NMOS_CONNECTION_RESOURCES_H

#include "nmos/id.h"
#include "nmos/settings.h"

namespace web
{
    class uri;
}

namespace nmos
{
    struct resource;

    // make an absolute URL for the /transportfile endpoint of the specified sender
    // e.g. for use in the manifest_href property of the IS-04 Node API sender
    web::uri make_connection_api_transportfile(const nmos::id& sender_id, const nmos::settings& settings);

    // IS-05 Connection API resources
    // "The UUIDs used to advertise Senders and Receivers in the Connection Management API must match
    // those used in a corresponding IS-04 implementation."
    // See https://github.com/AMWA-TV/nmos-device-connection-management/blob/v1.0/docs/3.1.%20Interoperability%20-%20NMOS%20IS-04.md#sender--receiver-ids
    // Whereas the data of the IS-04 resources corresponds to a particular Node API resource endpoint,
    // each IS-05 resource's data is a json object with an "id" field and a field for each Connection API
    // endpoint of that logical single resource
    // i.e.
    // a "constraints" field, which must have an array value conforming to the constraints-schema,
    // "staged" and "active" fields, which must each have a value conforming to the sender-response-schema or receiver-response-schema,
    // and for senders, also a "transportfile" field, the value of which must be an object, with either
    // "data" and "type" fields, or an "href" field
    // See https://github.com/AMWA-TV/nmos-device-connection-management/blob/v1.1/APIs/schemas/constraints-schema.json
    // and https://github.com/AMWA-TV/nmos-device-connection-management/blob/v1.1/APIs/schemas/sender-response-schema.json
    // and https://github.com/AMWA-TV/nmos-device-connection-management/blob/v1.1/APIs/schemas/receiver-response-schema.json

    // The caller must resolve all instances of "auto" in the /active endpoint into the actual values that will be used!
    // See nmos::resolve_rtp_auto
    nmos::resource make_connection_rtp_sender(const nmos::id& id, bool smpte2022_7);

    web::json::value make_connection_rtp_sender_transportfile(const utility::string_t& transportfile);
    web::json::value make_connection_rtp_sender_transportfile(const web::uri& transportfile);

    // The caller must resolve all instances of "auto" in the /active endpoint into the actual values that will be used!
    // See nmos::resolve_rtp_auto
    // transportfile may be URL or the contents of the SDP file (yeah, yuck)
    nmos::resource make_connection_rtp_sender(const nmos::id& id, bool smpte2022_7, const utility::string_t& transportfile);

    // The caller must resolve all instances of "auto" in the /active endpoint into the actual values that will be used!
    // See nmos::resolve_rtp_auto
    nmos::resource make_connection_rtp_receiver(const nmos::id& id, bool smpte2022_7);

    // Although these functions make "connection" (IS-05) resources, the details are defined by IS-07 Event & Tally
    // so maybe these belong in nmos/events_resources.h or their own file, e.g. nmos/connection_events_resources.h?
    // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/5.2.%20Transport%20-%20Websocket.md#3-connection-management
    nmos::resource make_connection_events_websocket_sender(const nmos::id& id, const nmos::id& device_id, const nmos::id& source_id, const nmos::settings& settings);
    nmos::resource make_connection_events_websocket_receiver(const nmos::id& id, const nmos::settings& settings);

    web::uri make_events_ws_api_connection_uri(const nmos::id& device_id, const nmos::settings& settings);
    web::uri make_events_api_ext_is_07_rest_api_url(const nmos::id& source_id, const nmos::settings& settings);

    utility::string_t make_events_mqtt_broker_topic(const nmos::id& source_id, const nmos::settings& settings);
    utility::string_t make_events_mqtt_connection_status_broker_topic(const nmos::id& connection_id, const nmos::settings& settings);
}

#endif
