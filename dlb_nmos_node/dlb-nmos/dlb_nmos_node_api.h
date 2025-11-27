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
#ifndef _NMOS_NODE_API_H_
#define _NMOS_NODE_API_H_

#include <fstream>
#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <mutex>

class websocket_endpoint;


namespace nmos
{
    struct node_model;
    struct node_call_backs;
    namespace experimental
    {
        class log_gate;
        struct log_model;
    }

    typedef std::map<std::string, std::string> DlbNmosSdpMap;

    typedef std::function<void(const DlbNmosSdpMap &sdpList, /** New Complete list of Audio Stream SDPs, This provides 1 API option. Next 3 provide an alternative API */
     const DlbNmosSdpMap &addedSdpList,                       /** List of SDPs that are associated with new streams added since last callback */
     const DlbNmosSdpMap &changedSdpList,                     /** List of SDPs that are associated with streams modified since last callback */
     const DlbNmosSdpMap &removedSdpList                      /** List of SDPs that are associated with streams that have been removed since last callback */
    )> NmosNodeStreamListCallBack;
    typedef std::function<void(const std::string sdp)> NmosNodeConnectionReqCallBack; /** This callback provides a request to the application to connect a source specified in the SDP*/
    typedef std::function<void(const std::string registryUrl)> NmosNodeRegistryFoundCallBack; /** This callback provides a request to the application to connect a source specified in the SDP*/

    struct CallBacks
    {
        NmosNodeStreamListCallBack nmosNodeStreamListCallBack;
        NmosNodeConnectionReqCallBack nmosNodeConnectionReqCallBack;
        NmosNodeRegistryFoundCallBack nmosNodeRegistryFoundCallBack;
    };

}

namespace pplx
{
    class cancellation_token_source;
    class cancellation_token;
}

bool FetchSDP(std::string url, std::string &sdp, pplx::cancellation_token &ct);


namespace nmos
{

    enum StreamType
    {
        STREAM_TYPE_NONE,
        AES67,              /**< AES67 or SMPTE ST 2110-30, Only support linear PCM */
        AM824,              /**< AM824 or SMPTE ST 2110-31, Provides transparent AES3 transport. Channel status is set to Data Mode and Professional */
        SMPTE2110_41        /**< SMPTE ST 2110-41, not yet supported */
    };


    struct AudioStreamInfo
    {
        unsigned int numChannels;           /**< Number of mono audio channels in the stream */
        unsigned int payloadBytesPerSample; /**< Number of bytes per audio sample */
        unsigned int samplesPerPacket;      /**< Number of samples in an ethernet packet. Typically 48 for 8 channels or less */
    };

    /**
     * @brief Defines the stream information specific for metadata streams.
     * This is mutually exclusive with the metadata stream information.
     * That is, a stream must either be an audio or a metadata stream but not
     * both. This structure is relevant for SMPTE ST 2110-41 streams
     */
    struct MetadataStreamInfo
    {
        float packetTimeMs;                 /**< The packet repition rate */
        unsigned int maxPayloadSizeBytes;   /**< The maximum possible payload size for the metadata */
        std::vector<uint32_t> dataItemTypes;/**< Data Item Type for stream. See SMPTE ST 2110-41 standard */
    };



    struct StreamInfo
    {
        std::string streamName;                 /**< Name of stream. Inserted into the SDP */
        StreamType streamType;                  /**< Indicates the type of stream that is to be transmitted or has been received */
        unsigned int payloadType;               /**< Dynamic RTP payloads as always used. Must be between 96 and 127 inclusive */
        std::string dstIpStr;                   /**< Destination IPv4 address */
        std::string srcIpStr;                   /**< Source IPv4 address */
        uint16_t port;                          /**< Destination port on reception. Source and destination on transmit. Use 0 for auto selection on transmit */
        uint64_t sessionId;                     /**< Session identifier. Goes into the SDP on the o= line. See RFC 4566 */
        uint32_t ssrc;                          /**< Synchronization source. Goes in RTP Header. See RFC 3550 */
        std::vector<std::string> channelLabels; /**< Channel labels to place in the i= line on the transmit side. Not support for receive */
        unsigned int samplingFrequency;         /**< Sampling Frequency in Hertz */
        float latency;                          /**< Latency to be used for stream in seconds */
        AudioStreamInfo audio;           /**< Audio stream info. Used if streamType == AES67/SMPTE ST 2110-30 or AM824 or SMPTE ST 2110-31 */
        MetadataStreamInfo metadata;            /**< Metadata stream info. Used if streamtype is SMPTE ST 2110-41 */
        unsigned int sourceIndex;               /**< Index into stream multiplex. Only used with multiple transmitters. Otherwise assumed to be 0 */
    };

    class DlbNmosNode
    {
    private:

        nmos::CallBacks callBacks;
        std::shared_ptr<nmos::node_call_backs> nmosNodeCallBacks;
        std::shared_ptr<std::thread> server_thread;
        std::thread::native_handle_type thread_handle;

    	std::shared_ptr<nmos::node_model> node_model;

        std::mutex node_lock;
        std::mutex server_exit_lock;

        std::shared_ptr<nmos::experimental::log_model> log_model;

        // Streams for logging, initially configured to write errors to stderr and to discard the access log
        std::filebuf error_log_buf;
        std::ostream error_log;
        std::filebuf access_log_buf;
        std::ostream access_log;

        // Logging should all go through this logging gateway
        std::shared_ptr<nmos::experimental::log_gate> gate; 

        unsigned int num_flows;
        std::string srcIpStr;

        std::shared_ptr<pplx::cancellation_token_source> cts1;
        std::shared_ptr<pplx::cancellation_token_source> cts2;
        std::shared_ptr<websocket_endpoint> ws_endpoint;

        std::string registryUrl;
        std::string nodeName;
        std::string inputStreamName;
        std::string uuidStr;
        DlbNmosSdpMap sdps;

        void StartServer(void);
        void TerminateServer(void);

        bool StartSenderMonitor(void);

        bool SetRegistry(std::string newRegistryUrl);

    	public:

    	DlbNmosNode(std::string newNodeName, std::string newUuidStr, std::string newSrcIpStr, std::string newRegistryUrl, nmos::CallBacks newCallBacks);
        ~DlbNmosNode(void);

        void AddFlow(StreamInfo &streamInfo);
        void ModifyFlowPacketTime(StreamInfo &streamInfo);
        void SetInputStream(std::string inputStream);
        void PrintSdps(void);

        bool Started();

        /* Callbacks that are not part of the API */
        void GotWsSenderMessage(std::string msg);
    };
}
#endif // _NMOS_NODE_API_H_