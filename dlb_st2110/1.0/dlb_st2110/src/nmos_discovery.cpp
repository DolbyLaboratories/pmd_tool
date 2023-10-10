/************************************************************************
 * dlb_st2110
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#include "dlb_st2110.h"
#include "nmos_discovery.h"
#include "dlb_nmos_node_api.h"
#include "dlb_st2110_logging.h"
#include "mclock.h"

using namespace std;


/******************* Helpers ***************************/

#define MAX_URL_SIZE 2048


static
std::string uchar_to_hex(unsigned char c)
{
    char s[3];
    snprintf(s,3,"%02x", c);
    return(string(s));
}

static
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

// This ugly converion is required to allow nmos to be independent of the rest of the 2110 stuff
// Restructuring is required inherited class common to both
static void CopyStreamInfo(nmos::StreamInfo &dst, const StreamInfo &src)
{
    dst.streamName = src.streamName;
    switch(src.streamType)
    {
    case AES67:
        dst.streamType = nmos::AES67;
        break;
    case AM824:
        dst.streamType = nmos::AM824;
        break;
    case SMPTE2110_41:
        dst.streamType = nmos::SMPTE2110_41;
        break;
    default:
        throw std::runtime_error("Unknown Stream Type");
    }
    dst.payloadType = src.payloadType;
    dst.dstIpStr = src.dstIpStr;
    dst.srcIpStr = src.srcIpStr;
    dst.port = src.port;
    dst.sessionId = src.sessionId;
    dst.ssrc = src.ssrc;
    dst.samplingFrequency = src.samplingFrequency;
    dst.latency = src.latency;
    dst.audio.numChannels = src.audio.numChannels;
    dst.audio.payloadBytesPerSample = src.audio.payloadBytesPerSample;
    dst.audio.samplesPerPacket = src.audio.samplesPerPacket;
    dst.metadata.maxPayloadSizeBytes = src.metadata.maxPayloadSizeBytes;
    dst.metadata.packetTimeMs = src.metadata.packetTimeMs;
    for (uint32_t dit : src.metadata.dataItemTypes)
    {
        dst.metadata.dataItemTypes.push_back(dit);
    }
    dst.sourceIndex = src.sourceIndex;
}


/******************* Call backs *********************/

void NmosDiscovery::NmosNodeCallBack(
    const nmos::DlbNmosSdpMap &sdpList,        /** New Complete list of Audio Stream SDPs, This provides 1 API option. Next 3 provide an alternative API */
    const nmos::DlbNmosSdpMap &addedSdpList,   /** List of SDPs that are associated with new streams added since last callback */
    const nmos::DlbNmosSdpMap &changedSdpList, /** List of SDPs that are associated with streams modified since last callback */
    const nmos::DlbNmosSdpMap &removedSdpList  /** List of SDPs that are associated with streams that have been removed since last callback */
    )
{
    for (auto &pair : addedSdpList)
    {
        Sdp2110 sdp(pair.second);
        if (sdp.Valid())
        {
            StreamInfo newStreamInfo(sdp.GetStreamInfo());
            SdpSystemInfo newSdpSystemInfo(sdp.GetSystemInfo());
            AoipService newService(AOIP_SERVICE_NMOS, newStreamInfo, newSdpSystemInfo);
            callBacks.addRxServiceCallBack(AOIP_SERVICE_NMOS, newService);
        }      
    }

    for (auto &pair : changedSdpList)
    {
        Sdp2110 sdp(pair.second);
        if (sdp.Valid())
        {
            StreamInfo newStreamInfo(sdp.GetStreamInfo());
            SdpSystemInfo newSdpSystemInfo(sdp.GetSystemInfo());
            AoipService newService(AOIP_SERVICE_NMOS, newStreamInfo, newSdpSystemInfo);
            callBacks.updateRxServiceCallBack(AOIP_SERVICE_NMOS, newService);
        }      
    }

    for (auto &pair : removedSdpList)
    {
        Sdp2110 sdp(pair.second);
        if (sdp.Valid())
        {
            callBacks.removeRxServiceCallBack(AOIP_SERVICE_NMOS, sdp.GetStreamInfo().streamName);
        }
    }
}




NmosDiscovery::NmosDiscovery(const AoipDiscovery::CallBacks &newCallBacks, AoipSystem& newSystem): AoipDiscovery(newCallBacks, newSystem)
{

    // This is requires as there a check for first-time initilization in callback

    /* Setup Node CallBacks */
    /*
    callBackInfo = std::make_shared<NmosNodeStreamListCallBackInfo>();
    callBackInfo->callBack = NmosNodeCallBackPub;
    callBackInfo->userData = (void *)this;
    */

    /* Create Node */
    nmosNodeCallBacks = std::make_shared<nmos::CallBacks>();
    nmosNodeCallBacks->nmosNodeStreamListCallBack = 
    [this](const nmos::DlbNmosSdpMap &sdpList, const nmos::DlbNmosSdpMap &addedSdpList, const nmos::DlbNmosSdpMap &changedSdpList, const nmos::DlbNmosSdpMap &removedSdpList)
    {
        NmosNodeCallBack(sdpList, addedSdpList, changedSdpList, removedSdpList);
    };
    nmosNodeCallBacks->nmosNodeConnectionReqCallBack = callBacks.connectionReqCallBack;

    // Create Node UUID from Mac Address and Name
    std::string uuid = createUUID(newSystem.name, newSystem.manageInterface.interfaceName);

    try
    {
        nmosNode = std::make_shared<nmos::DlbNmosNode>(newSystem.name, uuid, newSystem.manageInterface.ipStr, newSystem.nmosRegistry, *nmosNodeCallBacks);
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    // Check to see if we are finding nmos Registry automatically or if a registry name has been given.

    // Hold up thread until node has started
    // Choose a sleep period to avoid crushing CPU
    // but not cause delay in startup
    unsigned int timeOutCount = 20; // 1 sec timeout
    while(!nmosNode->Started())
    {
        MClock::TimePoint wakeTime;
        MClock::Duration sleepTime;
        sleepTime.setMilliseconds(50);
        wakeTime.SetNow();
        wakeTime = wakeTime + sleepTime;
        wakeTime.SleepUntil();
        if (timeOutCount-- == 0)
        {
            throw runtime_error("NMOS Node Failed to start");
        }
    }
}

void NmosDiscovery::AddTxService(StreamInfo& streamInfo)
{
    nmos::StreamInfo nmosStreamInfo;
    CopyStreamInfo(nmosStreamInfo, streamInfo);
    nmosNode->AddFlow(nmosStreamInfo);
}

void NmosDiscovery::UpdateTxService(StreamInfo& streamInfo)
{
    nmos::StreamInfo nmosStreamInfo;
    // Note that only packet time modification is currently supported
    CopyStreamInfo(nmosStreamInfo, streamInfo);
    nmosNode->ModifyFlowPacketTime(nmosStreamInfo);
}

void NmosDiscovery::RemoveTxService(string serviceName)
{
    throw std::runtime_error("Not Implemented");
}

void NmosDiscovery::SetInputService(string serviceName)
{
    nmosNode->SetInputStream(serviceName);
}
