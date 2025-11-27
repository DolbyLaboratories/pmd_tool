/************************************************************************
 * dlb_nmos_node
 * Copyright (c) 2019-2023, Dolby Laboratories Inc.
 * Copyright (c) 2019-2023, Dolby International AB.
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

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/find_first_of.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/join.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <signal.h>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include "cpprest/http_client.h" // for http_client, http_client_config, http_response, etc.
#include "pplx/pplx_utils.h" // for pplx::complete_after, etc.
#include "pplx/pplxcancellation_token.h"
#include "cpprest/host_utils.h"
#ifdef HAVE_LLDP
#include "lldp/lldp_manager.h"
#endif
#include "nmos/capabilities.h"
#include "nmos/channels.h"
#include "nmos/channelmapping_resources.h"
#include "nmos/clock_name.h"
#include "nmos/colorspace.h"
#include "nmos/components.h" // for nmos::chroma_subsampling
#include "nmos/connection_resources.h"
#include "nmos/connection_api.h"
#include "nmos/connection_events_activation.h"
#include "nmos/events_resources.h"
#include "nmos/group_hint.h"
#include "nmos/interlace_mode.h"
#ifdef HAVE_LLDP
#include "nmos/lldp_manager.h"
#endif
#include "nmos/media_type.h"
#include "nmos/model.h"
#include "nmos/node_interfaces.h"
#include "nmos/node_resource.h"
#include "nmos/node_resources.h"
#include "nmos/node_server.h"
#include "nmos/random.h"
#include "nmos/sdp_utils.h"
#include "nmos/slog.h"
#include "nmos/system_resources.h"
#include "nmos/transfer_characteristic.h"
#include "nmos/transport.h"
#include "nmos/log_gate.h"
#include "nmos/process_utils.h"
#include "nmos/server.h"
#include "nmos/activation_mode.h"
#include "nmos/is05_versions.h"
#include "nmos/mdns.h"
#include "sdp/sdp.h"

#include "dlb_nmos_node_api.h"
#include "ws_endpoint.h"
#include "node_implementation.h"


// example node implementation details
namespace impl
{
    // custom logging category for the example node implementation thread
    namespace categories
    {
        const nmos::category node_implementation{ "node_implementation" };
    }

    // the different kinds of 'port' (standing for the format/media type/event type) implemented by the example node
    // each 'port' of the example node has a source, flow, sender and compatible receiver
    DEFINE_STRING_ENUM(port)
    namespace ports
    {
        // video/raw
        const port video{ U("v") };
        // audio/L24
        const port audio{ U("a") };
        // video/smpte291
        const port data{ U("d") };
        // video/SMPTE2022-6
        const port mux{ U("m") };

        const std::vector<port> rtp{ video, audio, data, mux };
    }

    const std::vector<nmos::channel> channels_repeat{
        { U("Left Channel"), nmos::channel_symbols::L },
        { U("Right Channel"), nmos::channel_symbols::R },
        { U("Center Channel"), nmos::channel_symbols::C },
        { U("Low Frequency Effects Channel"), nmos::channel_symbols::LFE }
    };

    // generate repeatable ids for the example node's resources
    nmos::id make_id(const nmos::id& seed_id, const nmos::type& type, const port& port = {}, int index = 0);
    std::vector<nmos::id> make_ids(const nmos::id& seed_id, const nmos::type& type, const port& port, int how_many);
    std::vector<nmos::id> make_ids(const nmos::id& seed_id, const nmos::type& type, const std::vector<port>& ports, int how_many);

    // add a helpful suffix to the label of a sub-resource for the example node
    void set_label_description(nmos::resource& resource, const port& port, int index);

    // add an example "natural grouping" hint to a sender or receiver
    void insert_group_hint(nmos::resource& resource, const port& port, int index);

}

static void WsGotMessage(const std::string &msg, websocketpp::frame::opcode::value opcode, void *user_data){
    if (opcode == websocketpp::frame::opcode::text) {
        nmos::DlbNmosNode *node = (nmos::DlbNmosNode *)user_data;
        node->GotWsSenderMessage(msg);
    }
}

// Small utility function to extract name from SDP string

static
std::string GetStreamName(const std::string &sdp)
{
    std::string::size_type pos1 = sdp.find("s=");
    std::string::size_type pos2 = sdp.find("\r", pos1);
    return(sdp.substr(pos1 + 2, pos2 - pos1 - 2));
}


nmos::DlbNmosNode::DlbNmosNode(std::string newNodeName, std::string newUuidStr, std::string newSrcIpStr, std::string newRegistryUrl, nmos::CallBacks newCallBacks):
    callBacks(newCallBacks),
    error_log(std::cerr.rdbuf()),
    access_log(&access_log_buf),
    srcIpStr(newSrcIpStr),
    registryUrl(newRegistryUrl),
    nodeName(newNodeName),
    uuidStr(newUuidStr)

{
    node_model = std::make_shared<nmos::node_model>();
    log_model = std::make_shared<nmos::experimental::log_model>();
    gate = std::make_shared<nmos::experimental::log_gate>(error_log, access_log, *log_model);
    // ensures that server thread does not immediately exit
    server_exit_lock.lock();

    // Callback is bypassed i.e. straight through with no owned function
    // These callbacks must be created before the server thread is started below
    // as the thread can potentially use the callbacks
    nmosNodeCallBacks = std::make_shared<nmos::node_call_backs>();
    nmosNodeCallBacks->nmosNodeConnectionReqCallBack = callBacks.nmosNodeConnectionReqCallBack;
    nmosNodeCallBacks->nmosNodeRegistryFoundCallBack = 
    [this](std::string registryQueryUrl)
    {
        web::uri_builder builder(registryQueryUrl);
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Found Registry CallBack Received:" << registryQueryUrl;
        std::string registryUrl = "http://" + builder.host() + std::string(":") + std::to_string(builder.port());
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Setting Registry CallBack:" << registryUrl;
        SetRegistry(registryUrl);
    };

    server_thread = std::make_shared<std::thread>(std::thread(&nmos::DlbNmosNode::StartServer, this));
    thread_handle = server_thread->native_handle();
    
    // Initialize ASIO
    ws_endpoint = std::make_shared<websocket_endpoint>();

    // Cancellation tokens for http requests
    cts1 = std::make_shared<pplx::cancellation_token_source>();
    cts2 = std::make_shared<pplx::cancellation_token_source>();

    num_flows = 0;
}

bool nmos::DlbNmosNode::SetRegistry(std::string newRegistryUrl)
{
    bool result = false;
    node_lock.lock();
    if (newRegistryUrl != registryUrl)
    {
        registryUrl = newRegistryUrl;
        try
        {
            result = StartSenderMonitor();
        }
        catch (const std::exception& e)
        {
            node_lock.unlock();
            return false;
        }

    }
    node_lock.unlock();
    return result;
}

bool nmos::DlbNmosNode::StartSenderMonitor(void)
{
    web::uri_builder builder(U("/x-nmos/query/v1.2/subscriptions"));

    web::json::value subscribeJsonObject;
    subscribeJsonObject[nmos::fields::max_update_rate_ms] = web::json::value::number(100);
    subscribeJsonObject[nmos::fields::params] = web::json::value::object();
    subscribeJsonObject[nmos::fields::persist] = web::json::value::boolean(true);
    subscribeJsonObject[nmos::fields::resource_path] = web::json::value::string(U("/senders"));

    pplx::cancellation_token ct = cts1->get_token();

    try
    {
        auto subscribeClient = web::http::client::http_client(utility::conversions::to_string_t(registryUrl));
        auto subscribeRequest = subscribeClient.request(web::http::methods::POST, builder.to_string(), subscribeJsonObject, ct);
        subscribeRequest.wait();
        auto response = subscribeRequest.get();
        // Get the response.
        // Check the status code.
        if ((response.status_code() != 201) && // Created
            (response.status_code() != 200)) { // OK
            throw std::runtime_error("Returned " + std::to_string(response.status_code()));
        }
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "parsing subscription response";
        response.extract_json()
        .then([this](web::json::value subscribeResponseJsonObj) {
            slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Received subscription response";
            auto senderSubHref = subscribeResponseJsonObj[nmos::fields::ws_href].as_string();

            ws_endpoint->connect(senderSubHref, WsGotMessage, this);            
        });
    }
    catch (const std::exception &e) {
        slog::log<slog::severities::severe>(*gate, SLOG_FLF) << "Starting Flow Monitor Failed. Exception: " << e.what();
        return(false);
    }
    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Successfully started Sender Monitor";
    return(true);
}

void nmos::DlbNmosNode::GotWsSenderMessage(std::string msg)
{
    DlbNmosSdpMap addedSdps;
    DlbNmosSdpMap changedSdps;
    DlbNmosSdpMap removedSdps;
    pplx::cancellation_token ct = cts2->get_token();

    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Got WS Sender Message: ";
    
    utility::string_t msgUtilString = utility::conversions::to_string_t(msg);
    // Construct JSON object
    std::error_code ec;
    web::json::value msgJsonObj = web::json::value::parse(msgUtilString, ec);
    if (!ec) {
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Parsing succeeded";
    }
    else {
        slog::log<slog::severities::severe>(*gate, SLOG_FLF) << "Parsing failed";        
    }
    web::json::value dataJsonObj = msgJsonObj[U("grain")][U("data")];
    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "type " << dataJsonObj.type();
    auto sender_array = dataJsonObj.as_array();
    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Received " << sender_array.size() << " senders";

    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Deleting Senders";
    for (unsigned int i = 0 ; i < sender_array.size() ; i++) {
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Sender Array item #" << i + 1;
        if (sender_array[i].has_field(U("pre"))) {
            auto jsonObject = sender_array[i].as_object();
            auto Id = jsonObject[U("pre")][nmos::fields::id].as_string();
            slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Id: " << Id;
            DlbNmosSdpMap::iterator forDel = sdps.find(Id);
            if (forDel != sdps.end()) {
                slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Deleting " << Id;
                removedSdps[Id] = forDel->second;
                sdps.erase(forDel);
            }
        }
    }

    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Adding Senders";
    for (unsigned int i = 0 ; i < sender_array.size() ; i++) {
        std::string sdp;
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Sender Array item #" << i + 1;
        if (sender_array[i].has_field(U("post"))) {
            auto jsonObject = sender_array[i].as_object();
            auto manifestHref = jsonObject[U("post")][nmos::fields::manifest_href].as_string();
            auto subscriptionObj = jsonObject[U("post")][nmos::fields::subscription].as_object();
            auto senderActive = subscriptionObj[nmos::fields::active].as_bool();
            auto Id = jsonObject[U("post")][nmos::fields::id].as_string();
            slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Href: " << manifestHref;
            if (senderActive && FetchSDP(manifestHref, sdp, ct)) {
                sdps[Id] = sdp;
                DlbNmosSdpMap::iterator hasChanged = removedSdps.find(Id);
                if (hasChanged != removedSdps.end())
                {
                    changedSdps[Id] = sdp;
                    removedSdps.erase(Id);
                }
                else
                {
                    addedSdps[Id] = sdp;
                }
            }
            
        }
    }
    if (callBacks.nmosNodeStreamListCallBack)
    {
        (callBacks.nmosNodeStreamListCallBack)(sdps, addedSdps, changedSdps, removedSdps);
    }
}

void nmos::DlbNmosNode::PrintSdps(void)
{
    for (auto &pair: sdps) {
        std::cout << "ID: " << pair.first;
        std::string::size_type pos1 = pair.second.find("s=");
        std::string::size_type pos2 = pair.second.find("\r", pos1);
        std::string name = pair.second.substr(pos1, pos2 - pos1); // the part after the space
        std::cout << "  SDP: " << name << std::endl;
    }
}


bool FetchSDP(std::string url, std::string &sdp, pplx::cancellation_token &ct)
{
    web::http::client::http_client client(url);
    auto getSdp = client.request(web::http::methods::GET, ct)
    .then([](web::http::http_response response) {
        // Check the status code.
        if (response.status_code() != 200) {
            return(pplx::create_task([]() {return(utf8string(""));}));
        }
        return(response.extract_utf8string(true));
    })

    .then([&sdp](utf8string fetchedSdp) {
        sdp = fetchedSdp;
    });

    // Wait for the concurrent tasks to finish.
    try {
        getSdp.wait();
    } catch (const std::exception &e) {
        return(false);
    }
    if (sdp.empty())
    {
        return(false);
    }
    else
    {
        return(true);
    }
}

void nmos::DlbNmosNode::SetInputStream(std::string newInputStreamName)
{
    inputStreamName = newInputStreamName;
    // Set Sender ID in receiver according to SDPs received over websocket
    if (Started())
    {
        for (auto &pair: sdps) {
            std::string sdpStreamName = GetStreamName(pair.second);
            // See if the selected input stream matches the SDP received over WS
            // If so then set the receiver sender ID accordingly
            if (sdpStreamName == inputStreamName)
            {
                // Use a lambda to create scope for the read lock
                // Note that the 
                auto get_receiver = [&]()
                {
                    //nmos::resources &connection_resources = node_model->connection_resources;
                    auto lock = node_model->read_lock();
                    auto& by_type = node_model->node_resources.get<nmos::tags::type>();
                    auto receivers = by_type.equal_range(nmos::details::has_data(nmos::types::receiver));
                    // Should only be one receiver
                    return(receivers.first);
                };
                auto receiver = get_receiver();

                // Create Patch according to
                // https://raw.githubusercontent.com/AMWA-TV/nmos-device-connection-management/v1.1.x/APIs/schemas/receiver-stage-schema.json
                web::json::value new_patch = web::json::value_of({
                { nmos::fields::sender_id, web::json::value::null()},
                { nmos::fields::master_enable, true },
                { nmos::fields::activation, web::json::value_of({
                    { nmos::fields::mode, web::json::value::null() },
                    { nmos::fields::requested_time, web::json::value::null() }
                }) },
                { nmos::fields::transport_file, web::json::value::object()},
                { nmos::fields::transport_params, web::json::value::object() }
                });

                const auto request_time = nmos::tai_now(); // use for requested time

                new_patch[nmos::fields::activation][nmos::fields::mode] = web::json::value::string(nmos::activation_modes::activate_immediate.name);
                new_patch[nmos::fields::activation][nmos::fields::requested_time] = web::json::value::string(nmos::make_version(request_time));
                new_patch[nmos::fields::sender_id] = web::json::value(pair.first);
                new_patch[nmos::fields::transport_params] = nmos::get_session_description_transport_params(sdp::parse_session_description(pair.second));
                // Remove any secondary legs (2022-7) as we assume that we are connecting to the primary
                if (new_patch[nmos::fields::transport_params].is_array() && (new_patch[nmos::fields::transport_params].size() > 1))
                {
                    while(new_patch[nmos::fields::transport_params].size() > 1)
                    {
                        new_patch[nmos::fields::transport_params].erase(1);
                    }
                }

                // The schema validation will fail with an interface value  of "auto"
                // As this field seems to unimportant remove it here
                new_patch[nmos::fields::transport_params][0].erase(nmos::fields::interface_ip);

                new_patch[nmos::fields::transport_file] = web::json::value_of({{ nmos::fields::data, web::json::value::null() }, { nmos::fields::type, web::json::value::null() }});
                new_patch[nmos::fields::transport_file][nmos::fields::data] = web::json::value(pair.second);
                new_patch[nmos::fields::transport_file][nmos::fields::type] = web::json::value(U("application/sdp"));
            
                auto version = nmos::is05_versions::v1_1;
                web::http::http_response res;
                nmos::transport_file_parser file_parser = make_node_implementation_transport_file_parser();
                nmos::details::connection_resource_patch_validator patch_validator = make_node_implementation_patch_validator();
                nmos::details::handle_connection_resource_patch(res, *node_model, version, { receiver->id, nmos::types::receiver }, new_patch,
                                                                file_parser,
                                                                patch_validator, *gate);
            }
        }
    }
}


bool nmos::DlbNmosNode::Started()
{
    // If a device exists then node is considered to be ready for service
    const auto device = find_resource_if(node_model->node_resources, nmos::types::device, [](const nmos::resource& resource) { return true;});
    return (node_model->node_resources.end() != device);
}

void nmos::DlbNmosNode::AddFlow(StreamInfo &streamInfo)
{

    auto lock = node_model->write_lock();
    unsigned int bit_depth = streamInfo.audio.payloadBytesPerSample * 8;

    // any delay between updates to the model resources is unnecessary
    // this just serves as a slightly more realistic example!
    const unsigned int delay_millis{ 10 };

    const auto seed_id = nmos::experimental::fields::seed_id(node_model->settings);
    bool audio = ((streamInfo.streamType == AES67) || (streamInfo.streamType == AM824));
    const auto &port = (audio) ? impl::ports::audio : impl::ports::data;

    // it is important that the model be locked before inserting, updating or deleting a resource
    // and that the the node behaviour thread be notified after doing so
    nmos::node_model &model = *node_model;
    const auto insert_resource_after = [&model, &lock](unsigned int milliseconds, nmos::resources& resources, nmos::resource&& resource, slog::base_gate& gate)
    {
        if (nmos::details::wait_for(model.shutdown_condition, lock, std::chrono::milliseconds(milliseconds), [&] { return model.shutdown; })) return false;

        const std::pair<nmos::id, nmos::type> id_type{ resource.id, resource.type };
        const bool success = insert_resource(resources, std::move(resource)).second;

        if (success)
            slog::log<slog::severities::info>(gate, SLOG_FLF) << "Updated model with " << id_type;
        else
            slog::log<slog::severities::severe>(gate, SLOG_FLF) << "Model update error: " << id_type;

        slog::log<slog::severities::too_much_info>(gate, SLOG_FLF) << "Notifying node behaviour thread"; // and anyone else who cares...
        model.notify();

        return success;
    };

    unsigned index = num_flows++;

    /* make new source flow and sender */
    const auto source_id = impl::make_id(seed_id, nmos::types::source, port, index);
    const auto flow_id = impl::make_id(seed_id, nmos::types::flow, port, index);
    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "DLB: Creating Audio Sender Id: " << seed_id << "," << index;
    const auto sender_id = impl::make_id(seed_id, nmos::types::sender, port, index);
    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "DLB: Sender Id: " << sender_id;
    // Assume one device
    const auto device = find_resource_if(node_model->node_resources, nmos::types::device, [](const nmos::resource& resource) { return true;});
    nmos::id device_id;
    if (node_model->node_resources.end() == device)
    {
        throw std::runtime_error("No device");
        return;
    }
    else
    {
        device_id = device->id;
    }

    const int channel_count = streamInfo.audio.numChannels;
    
    const auto resolve_auto = make_node_implementation_auto_resolver(node_model->settings);
    const auto set_transportfile = make_node_implementation_transportfile_setter(model.node_resources, model.settings);

    const auto host_interfaces = nmos::get_host_interfaces(node_model->settings);
    const auto interfaces = nmos::experimental::node_interfaces(host_interfaces);

    const auto& primary_interface = host_interfaces.front();
    const auto interface_names = std::vector<utility::string_t>{ primary_interface.name };

    std::vector<utility::string_t> host_interface_name_for_stream = {primary_interface.name};
    //host_interface_name_for_stream.push_back(primary_interface.name);
    
    for (auto &interface : host_interfaces)
    {
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << interface.index << ":  " << interface.name << " Physical Address: " << interface.physical_address << "domain: " << interface.domain;
        for (auto &address : interface.addresses)
        {
            unsigned int count = 1;
            slog::log<slog::severities::info>(*gate, SLOG_FLF) << "       address#" << count++ << " " << address;
            if (address == streamInfo.srcIpStr)
            {
                host_interface_name_for_stream.clear();
                host_interface_name_for_stream.push_back(interface.name);
                slog::log<slog::severities::info>(*gate, SLOG_FLF) << " Found src IP" << address << " Using host interface " << interface.name;
            }
        }
    }



    // Create one audio source
    // Use grain rate to convey packet time
    nmos::resource source;
    if(audio)
    {
        const auto channels = boost::copy_range<std::vector<nmos::channel>>(boost::irange(0, channel_count) | boost::adaptors::transformed([&](const int& index)
        {
            return impl::channels_repeat[index % (int)impl::channels_repeat.size()];
        }));
        const nmos::rational packet_time{ streamInfo.samplingFrequency/streamInfo.audio.samplesPerPacket, 1 };
        source = nmos::make_audio_source(source_id, device_id, nmos::clock_names::clk0, packet_time, channels, node_model->settings); // grain_rate is optional so setting it to 0 omits it
    }
    else
    {
        const nmos::rational packet_time{1000, unsigned(std::round(streamInfo.metadata.packetTimeMs))};
        source = nmos::make_data_source(source_id, device_id, nmos::clock_names::clk0, packet_time, node_model->settings);
    }
    impl::set_label_description(source, port, index);

    nmos::resource flow;
    {
        switch(streamInfo.streamType)
        {
        case AES67:
            flow = nmos::make_raw_audio_flow(flow_id, source_id, device_id, streamInfo.samplingFrequency, bit_depth, node_model->settings);
            break;
        case AM824:
            flow = nmos::make_coded_audio_flow(flow_id, source_id, device_id, streamInfo.samplingFrequency, nmos::media_types::audio_AM824, node_model->settings);
            // Needs bit depth key for sdp parsing even though not used
            flow.data[nmos::fields::bit_depth] = 24;
            break;
        case SMPTE2110_41:
            flow = nmos::make_data_flow(flow_id, source_id, device_id, nmos::media_types::application_st2110_41, node_model->settings);
            if (streamInfo.metadata.dataItemTypes.empty())
            {
                throw std::runtime_error("No DIT supplied for SMPTE ST 2110-41 stream");
            }
            flow.data[nmos::fields::dit] = streamInfo.metadata.dataItemTypes.front(); // Only supporting single DIT for now, others are ignored
            flow.data[nmos::fields::sample_rate] = make_rational(streamInfo.samplingFrequency);
            break;
        default:
            throw std::runtime_error("Unsupported streamType");
        }
    }
    impl::set_label_description(flow, port, index);

    // set_transportfile needs to find the matching source and flow for the sender, so insert these first
    if (!insert_resource_after(delay_millis, node_model->node_resources, std::move(source), *gate)) return;
    if (!insert_resource_after(delay_millis, node_model->node_resources, std::move(flow), *gate)) return;

    auto sender = nmos::make_sender(sender_id, flow_id, device_id, host_interface_name_for_stream, model.settings);
    impl::set_label_description(sender, port, index);
    sender.data[nmos::fields::label] = web::json::value::string(streamInfo.streamName);
    impl::insert_group_hint(sender, port, index);

    auto connection_sender = nmos::make_connection_rtp_sender(sender_id, false); // smpte2022_7 = false
    connection_sender.data[nmos::fields::endpoint_constraints][0][nmos::fields::source_ip] = web::json::value_of({
        { nmos::fields::constraint_enum, web::json::value_from_elements(primary_interface.addresses) }
    });

    // initialize this sender enabled, just to enable the IS-05-01 test suite to run immediately
    connection_sender.data[nmos::fields::endpoint_active][nmos::fields::master_enable] = connection_sender.data[nmos::fields::endpoint_staged][nmos::fields::master_enable] = web::json::value::boolean(true);
    resolve_auto(sender, connection_sender, connection_sender.data[nmos::fields::endpoint_active][nmos::fields::transport_params]);

    connection_sender.data[nmos::fields::endpoint_active][nmos::fields::transport_params][0][nmos::fields::destination_ip] = web::json::value(streamInfo.dstIpStr);

    set_transportfile(sender, connection_sender, connection_sender.data[nmos::fields::endpoint_transportfile]);
    nmos::set_resource_subscription(sender, nmos::fields::master_enable(connection_sender.data[nmos::fields::endpoint_active]), {}, nmos::tai_now());

    if (!insert_resource_after(delay_millis, node_model->node_resources, std::move(sender), *gate)) return;
    if (!insert_resource_after(delay_millis, node_model->connection_resources, std::move(connection_sender), *gate)) return;

}

void nmos::DlbNmosNode::ModifyFlowPacketTime(StreamInfo &streamInfo)
{
    auto lock = node_model->write_lock();
    auto& node_resources = node_model->node_resources;
    auto& connection_resources = node_model->connection_resources;
    auto& settings = node_model->settings;

    const bool audio = ((streamInfo.streamType == AES67) || (streamInfo.streamType == AM824));
    web::json::value packet_time;

    // Find resources
    const auto sender = find_resource_if(node_resources, nmos::types::sender, [streamInfo](const nmos::resource& res)
        {
            //std::cout << "Id: " << nmos::fields::id(res.data) << std::endl;
            //std::cout << "Label: " << nmos::fields::label(res.data) << std::endl;
            auto sender_label = nmos::fields::label(res.data);
            return sender_label == streamInfo.streamName;
        });
    const auto sender_id = sender->id;
    auto connection_sender = nmos::find_resource(connection_resources, { sender_id, nmos::types::sender });
    const auto flow_id = nmos::fields::flow_id(sender->data).as_string();
    auto flow = nmos::find_resource(node_resources, { flow_id, nmos::types::flow });
    const auto source_id = nmos::fields::source_id(flow->data);
    auto source = nmos::find_resource(node_resources, { source_id, nmos::types::source });

    // Check we have found the resources we need
    if ((sender == node_resources.end()) ||
        (connection_sender == connection_resources.end()) ||
        (flow == node_resources.end()) ||
        (source == node_resources.end())) { return; }

    // Modify packet time
    if (audio)
    {
       packet_time = make_rational(streamInfo.samplingFrequency/streamInfo.audio.samplesPerPacket, 1);
    }
    else
    {
        packet_time = make_rational(1000, unsigned(std::round(streamInfo.metadata.packetTimeMs)));
    }

    nmos::modify_resource(node_resources, source_id, [packet_time](nmos::resource& source)
    {
        nmos::fields::grain_rate(source.data) = packet_time;
    });

    // Set transport file according to new settings
    const auto set_transportfile = make_node_implementation_transportfile_setter(node_resources, settings);

    auto ts = nmos::fields::endpoint_transportfile(connection_sender->data);

    set_transportfile(*sender, *connection_sender, ts);

    node_model->notify();
}


void nmos::DlbNmosNode::StartServer(void)
{
    // Construct our data models including mutexes to protect them

    try
    {

        // Settings can be passed on the command-line, directly or in a configuration file, and a few may be changed dynamically by PATCH to /settings/all on the Settings API
        //
        // * "logging_level": integer value, between 40 (least verbose, only fatal messages) and -40 (most verbose)
        // * "registry_address": used to construct request URLs for registry APIs (if not discovered via DNS-SD)
        //
        // E.g.
        //
        // # ./nmos-cpp-node "{\"logging_level\":-40}"
        // # ./nmos-cpp-node config.json
        // # curl -X PATCH -H "Content-Type: application/json" http://localhost:3209/settings/all -d "{\"logging_level\":-40}"
        // # curl -X PATCH -H "Content-Type: application/json" http://localhost:3209/settings/all -T config.json

        //std::string settings = R"({"logging_level": 0, "domain": "local.", "http_port": 8080, "events_ws_port": 8081, "host_address": "192.168.2.9", "label": "Dolby PMD Studio" })";
        std::string settings = R"({"logging_level": 40, "http_port": 8080, "events_ws_port": 8081 })";

        std::error_code error;
        node_model->settings = web::json::value::parse(utility::s2us(settings), error);
    
        std::vector<utility::string_t> addresses = { srcIpStr };
        web::json::insert(node_model->settings, std::make_pair(nmos::fields::host_addresses, web::json::value_from_elements(addresses)));
        web::json::insert(node_model->settings, std::make_pair(nmos::fields::host_address, srcIpStr));

        web::json::insert(node_model->settings, std::make_pair(nmos::fields::label, nodeName));

        // Also use node name to make repeatable UUIDs
        web::json::insert(node_model->settings, std::make_pair(nmos::experimental::fields::seed_id, uuidStr));


        // Check to see if we are using mDNS or not
        if (registryUrl.empty())
        {
            // mDNS
            web::json::insert(node_model->settings, std::make_pair(nmos::fields::domain, "local."));
        }
        else
        {
            // Disabling discovery and using a fixed setting
            web::json::insert(node_model->settings, std::make_pair(nmos::fields::domain, ""));
            web::uri_builder builder(registryUrl);

            web::json::insert(node_model->settings, std::make_pair(nmos::fields::registry_address, builder.host()));
            web::json::insert(node_model->settings, std::make_pair(nmos::fields::registration_port, builder.port()));
            web::json::insert(node_model->settings, std::make_pair(nmos::fields::highest_pri, nmos::service_priorities::no_priority));
        }

        if (error || !node_model->settings.is_object())
        {
            slog::log<slog::severities::severe>(*gate, SLOG_FLF) << "Bad settings [" << error << "]";
        }

        // Prepare run-time default settings (different than header defaults)

        nmos::insert_node_default_settings(node_model->settings);

        // copy to the logging settings
        // hmm, this is a bit icky, but simplest for now
        log_model->settings = node_model->settings;

        // the logging level is a special case because we want to turn it into an atomic value
        // that can be read by logging statements without locking the mutex protecting the settings
        log_model->level = nmos::fields::logging_level(log_model->settings);

        // Reconfigure the logging streams according to settings
        // (obviously, until this point, the logging gateway has its default behaviour...)

        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Starting DLB nmos-cpp node";
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Settings:" << node_model->settings.serialize();

        if (!nmos::fields::error_log(node_model->settings).empty())
        {
            error_log_buf.open(nmos::fields::error_log(node_model->settings), std::ios_base::out | std::ios_base::app);
            auto lock = log_model->write_lock();
            error_log.rdbuf(&error_log_buf);
        }

        if (!nmos::fields::access_log(node_model->settings).empty())
        {
            access_log_buf.open(nmos::fields::access_log(node_model->settings), std::ios_base::out | std::ios_base::app);
            auto lock = log_model->write_lock();
            access_log.rdbuf(&access_log_buf);
        }

        // Log the process ID and initial settings

        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Process ID: " << nmos::details::get_process_id();
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Build settings: " << nmos::get_build_settings_info();
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Initial settings: " << node_model->settings.serialize();

        // Set up the callbacks between the node server and the underlying implementation
        //nmos::NmosNodeConnectionReqCallBackInfo nmosCallBackInfo{connectionReqCallBackInfo.callBack, connectionReqCallBackInfo.userData};

        auto node_implementation = make_node_implementation(*node_model, *gate, *nmosNodeCallBacks);

        // Set up the node server

        auto node_server = nmos::experimental::make_node_server(*node_model, node_implementation, *log_model, *gate);

        if (!nmos::experimental::fields::http_trace(node_model->settings))
        {
            // Disable TRACE method

            for (auto& http_listener : node_server.http_listeners)
            {
                http_listener.support(web::http::methods::TRCE, [](web::http::http_request req) { req.reply(web::http::status_codes::MethodNotAllowed); });
            }
        }

        // Add the underlying implementation, which will set up the node resources, etc.

        node_server.thread_functions.push_back([&] { node_implementation_thread(*node_model, *gate); });

        // Open the API ports and start up node operation (including the DNS-SD advertisements)

        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Preparing for connections";

        nmos::server_guard node_server_guard(node_server);

        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Ready for connections";

        // Open Websocket with Registry if registry name has been provided
        if (!registryUrl.empty())
        {
            if (!StartSenderMonitor())
            {
                throw web::websockets::websocket_exception("Starting Websocket connection to registry failed");
            }
        }

        //Wait for a process termination signal
        //nmos::details::wait_term_signal();
        server_exit_lock.lock();
        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Server Thread got exit lock";

        slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Closing connections";
    }
    catch (const web::json::json_exception& e)
    {
        // most likely from incorrect types in the command line settings
        slog::log<slog::severities::error>(*gate, SLOG_FLF) << "JSON error: " << e.what();
    }
    catch (const web::http::http_exception& e)
    {
        slog::log<slog::severities::error>(*gate, SLOG_FLF) << "HTTP error: " << e.what() << " [" << e.error_code() << "]";
    }
    catch (const web::websockets::websocket_exception& e)
    {
        slog::log<slog::severities::error>(*gate, SLOG_FLF) << "WebSocket error: " << e.what() << " [" << e.error_code() << "]";
    }
    catch (const std::system_error& e)
    {
        slog::log<slog::severities::error>(*gate, SLOG_FLF) << "System error: " << e.what() << " [" << e.code() << "]";
    }
    catch (const std::runtime_error& e)
    {
        slog::log<slog::severities::error>(*gate, SLOG_FLF) << "Implementation error: " << e.what();
        exit(-1);
    }
    catch (const std::exception& e)
    {
        slog::log<slog::severities::error>(*gate, SLOG_FLF) << "Unexpected exception: " << e.what();
    }
    catch (...)
    {
        slog::log<slog::severities::severe>(*gate, SLOG_FLF) << "Unexpected unknown exception";
    }
    slog::log<slog::severities::info>(*gate, SLOG_FLF) << "Stopping nmos-cpp node";
}

void nmos::DlbNmosNode::TerminateServer(void)
{
    server_exit_lock.unlock();
    server_thread->join();
}

nmos::DlbNmosNode::~DlbNmosNode()
{
    TerminateServer();
    cts1->cancel();
    cts2->cancel();
}
