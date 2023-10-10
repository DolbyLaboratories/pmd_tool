/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2022 by Dolby Laboratories,
 *                Copyright (C) 2019-2022 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <boost/range/irange.hpp>
#include "nmos/json_fields.h"
#include "cpprest/http_client.h" // for http_client, http_client_config, http_response, etc.
#include "dlb_nmos_node_api.h"

#include "nmos/log_gate.h"
#include "nmos/model.h"
#include "nmos/node_server.h"
#include "nmos/process_utils.h"
#include "nmos/server.h"


#define MAX_URL_SIZE 2048

using namespace std;

typedef std::array<unsigned char, 6> MacAddrByteArray;

bool GetMacAddrByteArray(std::string interfaceName, MacAddrByteArray &byteArray)
{
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    strcpy(s.ifr_name, interfaceName.c_str());
    if (0 == ioctl(fd, SIOCGIFHWADDR, &s))
    {
        for (int i = 0; i < 6; ++i)
        {
          byteArray[i] = s.ifr_addr.sa_data[i];
        }
        return true;
    }
    else
    {
        return false;        
    }
}


void print_sdp_map(const nmos::DlbNmosSdpMap &sdps)
{
    for (auto &pair: sdps) {
        std::string name;
        std::cout << "ID: " << pair.first;
        std::string::size_type pos1 = pair.second.find("s=");
        std::string::size_type pos2 = pair.second.find("\r", pos1);
        if ((pos1 == string::npos) || (pos2 == string::npos))
        {
            name = "";
        }
        else
        {
            name = pair.second.substr(pos1, pos2 - pos1); // the part after the space
        }
        std::cout << "  SDP: " << name << std::endl;
    }
}

static
std::string GetStreamName(const std::string &sdp)
{
    std::string::size_type pos1 = sdp.find("s=");
    std::string::size_type pos2 = sdp.find("\r", pos1);
    return(sdp.substr(pos1 + 2, pos2 - pos1 - 2));
}

void nmosStreamListCallBack(
    const nmos::DlbNmosSdpMap &sdpList, /** New Complete list of Audio Stream SDPs, This provides 1 API option. Next 3 provide an alternative API */
    const nmos::DlbNmosSdpMap &addedSdpList, /** List of SDPs that are associated with new streams added since last callback */
    const nmos::DlbNmosSdpMap &changedSdpList, /** List of SDPs that are associated with streams modified since last callback */
    const nmos::DlbNmosSdpMap &removedSdpList /** List of SDPs that are associated with streams that have been removed since last callback */
)
{
    cout << "Complete SDP list" << endl << "======= === ====" << endl;
    print_sdp_map(sdpList);

    cout << "Added SDP list" << endl << "======= === ====" << endl;
    print_sdp_map(addedSdpList);

    cout << "Changed SDP list" << endl << "======= === ====" << endl;
    print_sdp_map(addedSdpList);

    cout << "Removed SDP list" << endl << "======= === ====" << endl;
    print_sdp_map(removedSdpList);

    cout << endl << endl;
}

static std::string new_input_stream_sdp = "";
static std::mutex new_input_stream_mutex;

void nmosConnectionReqCallBack(const std::string sdp)
{
    cout << "Received Connection Request:" << endl << "SDP" << endl << "===" << endl;
    cout << sdp << endl;
    new_input_stream_mutex.lock();
    new_input_stream_sdp = sdp;
    new_input_stream_mutex.unlock();
}

void nmosRegistryFoundCallBack(const std::string registryUrl)
{
    cout << "Received Found Registry Notification: " << registryUrl << endl;
}

void CreateOutputStreams(nmos::DlbNmosNode *node)
{
    for (auto i : boost::irange(1,2))
    {
        nmos::StreamInfo streamInfo;
        streamInfo.streamName = "audio-stream-" + to_string(i);   /**< Name of stream. Inserted into the SDP */
        if (i % 2)
        {
            streamInfo.streamType = nmos::AES67;                  /**< Indicates the type of stream that is to be transmitted or has been received */
        }
        else
        {
            streamInfo.streamType = nmos::AM824;                  /**< Indicates the type of stream that is to be transmitted or has been received */
        }
        streamInfo.payloadType = 96;                              /**< Dynamic RTP payloads as always used. Must be between 96 and 127 inclusive */
        streamInfo.dstIpStr = "239.1.2." + to_string(i);          /**< Destination IPv4 address */
        streamInfo.srcIpStr = "192.168.2.9";                      /**< Source IPv4 address */
        streamInfo.port = 5004;                                   /**< Destination port on reception. Source and destination on transmit. Use 0 for auto selection on transmit */
        streamInfo.sessionId = 0x1234abcd1234abcd;                /**< Session identifier. Goes into the SDP on the o= line. See RFC 4566 */
        streamInfo.ssrc = 0xabcd1234;                             /**< Synchronization source. Goes in RTP Header. See RFC 3550 */
        streamInfo.samplingFrequency = 48000;                     /**< Sampling Frequency in Hertz */
        streamInfo.audio.numChannels = i * 4;                     /**< Audio stream info. Used if streamType == AES67/SMPTE ST 2110-30 or AM824 or SMPTE ST 2110-31 */
        streamInfo.audio.payloadBytesPerSample = 3;
        if (streamInfo.audio.numChannels > 8)
        {
            streamInfo.audio.samplesPerPacket = 6;
        }
        else
        {
            streamInfo.audio.samplesPerPacket = 48;            
        }

        node->AddFlow(streamInfo);
    }    

    for (auto i : boost::irange(1,2))
    {
        nmos::StreamInfo streamInfo;
        streamInfo.streamName = "metadata-stream-" + to_string(i);   /**< Name of stream. Inserted into the SDP */
        streamInfo.streamType = nmos::SMPTE2110_41;                  /**< Indicates the type of stream that is to be transmitted or has been received */
        streamInfo.payloadType = 96;                                 /**< Dynamic RTP payloads as always used. Must be between 96 and 127 inclusive */
        streamInfo.dstIpStr = "239.1.3." + to_string(i);             /**< Destination IPv4 address */
        streamInfo.srcIpStr = "192.168.2.9";                         /**< Source IPv4 address */
        streamInfo.port = 5004;                                      /**< Destination port on reception. Source and destination on transmit. Use 0 for auto selection on transmit */
        streamInfo.sessionId = 0x1234abcd1234abcd;                   /**< Session identifier. Goes into the SDP on the o= line. See RFC 4566 */
        streamInfo.ssrc = 0xabcd1234;                                /**< Synchronization source. Goes in RTP Header. See RFC 3550 */
        streamInfo.samplingFrequency = 48000;                        /**< Sampling Frequency in Hertz */
        streamInfo.metadata.packetTimeMs = 40;                       /**< The packet repition rate */
        streamInfo.metadata.maxPayloadSizeBytes = 0;                 /**< The maximum possible payload size for the metadata */
        streamInfo.metadata.dataItemTypes = { 0x3FFAD1 };            /**< Data Item Type for stream. See SMPTE ST 2110-41 standard */
        node->AddFlow(streamInfo);
    }
    return;

}


void print_usage(void)
{
    cerr << "dlb_nmos_node_main <Interface> <Registry>" << endl;
    cerr << "Interface = Name of network interface to use" << endl;
}

std::string uchar_to_hex(unsigned char c)
{
    char s[3];
    snprintf(s,3,"%02x", c);
    return(string(s));
}


std::string GetIpStrFromInterface(std::string interfaceName)
{
    std::string ipAddr;
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    if ((((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr).s_addr == 0)
    {
        throw std::runtime_error("Invalid Interface Name - Check interface is connected");
    }
    ipAddr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    return(ipAddr);
}

std::string createUUID(std::string nodeName, std::string interfaceName)
{
    // Create Node UUID from Mac Address and Name
    MacAddrByteArray macAddrByteArray;
    std::string uuid = "";

    if (GetMacAddrByteArray(interfaceName, macAddrByteArray))
    {
        for (unsigned int i = 0 ; i < 6 ; i++)
        {
            uuid += uchar_to_hex(macAddrByteArray[i]);
            if (i == 3)
            {
                uuid += "-";
            }
        }
    }
    else
    {
        // Failed to get Mac Addr
        throw std::runtime_error(std::string("Failed to get Mac Address of ") + interfaceName);
    }
    uuid += "-";
    for (unsigned int i = 6 ; i < 16 ; i++)
    {
        // Use node name for remaining otherwise use fixed integers
        if (i < nodeName.size())
        {
            uuid += uchar_to_hex(unsigned(nodeName[i]));
        }
        else
        {
            uuid += uchar_to_hex(i);
        }
        if ((i == 7) || (i == 9))
        {
            uuid += "-";
        }
    }
    return(uuid);
}


// Parameter 1 = network interface name to use (Required)
// Parameter 2 = Registry address in the form: "http://hostname:port" (Optional, Default = mDNS)

int main(int argc, char*argv[]) {
    int ret = 1;
    int timeToExit, timeRun;
    nmos::CallBacks nmosNodeCallBacks;
    string srcIpStr;
    string registryUrl = "";
    string nodeName = "Dolby Nmos Test Node";
    nmos::StreamInfo streamInfo;

    // Change from s5fps to 50fps
    streamInfo.streamName = "metadata-stream-1";
    streamInfo.streamType = nmos::SMPTE2110_41;
    streamInfo.metadata.packetTimeMs = 20;


    //NmosNodeStreamListCallBackInfo streamListCallBackInfo;
    //NmosNodeConnectionReqCallBackInfo connectionReqCallBackInfo;

    //streamListCallBackInfo.callBack = nmosStreamListCallBack;
    //streamListCallBackInfo.userData = nullptr;

    //connectionReqCallBackInfo.callBack = nmosConnectionReqCallBack;
    //connectionReqCallBackInfo.userData = nullptr;

    if (argc < 2)
    {
        cerr << "Not enough arguments" << endl;
        print_usage();
        exit(-1);
    }

    if (!strcmp(argv[1], "-h") ||
        !strcmp(argv[1], "-help") ||
        !strcmp(argv[1], "--help"))
    {
        print_usage();
        exit(0);        
    }

    srcIpStr = GetIpStrFromInterface(argv[1]);

    if (argc > 2)
    {
        registryUrl = argv[2];
    }

    string uuid = createUUID(nodeName, string(argv[1]));
    cout << "uuid:<" << uuid << ">" << endl;
    nmosNodeCallBacks.nmosNodeStreamListCallBack = nmosStreamListCallBack;
    nmosNodeCallBacks.nmosNodeConnectionReqCallBack = nmosConnectionReqCallBack;
    // Create NMOS node
    nmos::DlbNmosNode *node = new nmos::DlbNmosNode(nodeName, uuid, srcIpStr, registryUrl, nmosNodeCallBacks);

    while(!node->Started())
    {
        sleep(1);
    }

    CreateOutputStreams(node);
    

    //timeToExit = 20;
    timeRun = 0;
    timeToExit = -1;
    while (timeToExit != 0)
    {
        //node->PrintSdps();
        if (timeToExit > 0)
        {
            cout << "Exit in " << timeToExit-- << " seconds" << endl;
        }
        
        if (timeRun == 30)
        {
//            cout << "Setting Input Stream...";
//            node->SetInputStream(string("emsfp-coax-rx_0-1-0"));
//            node->SetInputStream(string("dp591-playout"));
            cout << "Modifying Flow...";
            node->ModifyFlowPacketTime(streamInfo);
            cout << "Done" << endl;
        }
        
        new_input_stream_mutex.lock();
        if (!new_input_stream_sdp.empty())
        {
            /* Shut down nmos node */
            /* Restart with new receive stream */
            delete node;
            node = new nmos::DlbNmosNode(nodeName, uuid, srcIpStr, registryUrl, nmosNodeCallBacks);
            while(!node->Started())
            {
                sleep(1);
            }
            CreateOutputStreams(node);
            string inputStreamName = GetStreamName(new_input_stream_sdp);
            sleep(5);
            node->SetInputStream(inputStreamName);
            new_input_stream_sdp.clear();
        }
        new_input_stream_mutex.unlock();
        timeRun++;
        cout << "Time: " << timeRun << endl;
        sleep(1);
    }
    ret = 0;
    return ret;
}