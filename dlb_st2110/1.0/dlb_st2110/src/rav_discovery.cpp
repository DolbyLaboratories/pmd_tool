/************************************************************************
 * dlb_st2110
 * Copyright (c) 2021, Dolby Laboratories Inc.
 * Copyright (c) 2021, Dolby International AB.
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
#include "rav_discovery.h"

#include "dlb_st2110_logging.h"


using namespace std;

/******************* Helpers ***************************/

const char sep[] = " :=/;,\r";

static std::vector<std::string> LineToFields(std::string line)
{
    std::vector<std::string> fields;

    size_t beforeFirst = 0, first, firstSep;
    while(beforeFirst < line.length())
    {
        first = line.find_first_not_of(sep, beforeFirst);
        firstSep = line.find_first_of(sep, first);
        if (firstSep > first)
        {
            fields.push_back(line.substr(first, firstSep - first));
        }
        beforeFirst = firstSep;
    }
    return(fields);
}

static bool GetField(const std::vector<std::string> &fields, unsigned int selection, std::string &out)
{
    if (selection >= fields.size())
    {
        return(true);
    }
    else
    {
        out = fields[selection];
        return(false);
    }
}

static string ToLowerStr(string mixedCaseString)
{
	for_each(mixedCaseString.begin(), mixedCaseString.end(), [](char & c)
	{
	    c = tolower(c);
	});
	return(mixedCaseString);
}

/******************* Call backs *********************/

static void RavAvahiClientCallback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata)
{
	RavDiscovery *rav = (RavDiscovery *) userdata;
    /* Called whenever the client or server state changes */
    if (state == AVAHI_CLIENT_FAILURE)
    {
    	throw runtime_error("Server connection failure: " + string(avahi_strerror(avahi_client_errno(c))));
    }

    if (state == AVAHI_CLIENT_S_RUNNING)
    {
    	// Check to see if client is initialized
    	rav->CreateRavennaServices(c);
    }
}

static void RavAvahiEntryGroupCallback(AvahiEntryGroup* g, AvahiEntryGroupState state, void* userdata)
{

        /* Called whenever the entry group state changes */
    switch (state)
    {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
            /* The entry group has been established successfully */
            CLOG(INFO, RAV_LOG) << "Entry Group Established";
            break;
        case AVAHI_ENTRY_GROUP_COLLISION : {
        	throw runtime_error("Service Name Collison - Resolution not implemented");
            //char *n;
            /* A service name collision with a remote service
             * happened. Let's pick a new name */
            //n = avahi_alternative_service_name("My_Name");
            //name = n;
            //fprintf(stderr, "Service name collision, would rename service to '%s'\n", n);
            //printf("Instead exiting\n");
            /* And recreate the services */
            /* we don't know how to do this so exit */
            //exit(-2);
            break;
        }
        case AVAHI_ENTRY_GROUP_FAILURE :
            throw runtime_error("Entry group failure: " + string(avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g)))));
            /* Some kind of failure happened while we were registering our services */
            break;
        case AVAHI_ENTRY_GROUP_UNCOMMITED:
            break;
        case AVAHI_ENTRY_GROUP_REGISTERING:
            CLOG(INFO, RAV_LOG) << "Entry Group Registering";
            break;
        default:
            throw runtime_error("Unknown Avahi Entry Group State");
    }

}



static void RavResolverCallback(
    AvahiServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
    AvahiLookupResultFlags flags,
    AVAHI_GCC_UNUSED void* userdata)
{
    RavDiscovery *rav = (RavDiscovery *)userdata;	
    /* Called whenever a service has been resolved successfully or timed out */
    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            CLOG(WARNING, RAV_LOG) << "Avahi resolver failed to resolve service" << name << " of type " << type << \
            " in domain " << domain << "\n" << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r)));
            break;
        case AVAHI_RESOLVER_FOUND: {
            char a[AVAHI_ADDRESS_STR_MAX];
            avahi_address_snprint(a, sizeof(a), address);
            CLOG(INFO, RAV_LOG) << "Avahi resolved service " << name << " of type " << type << " in domain " << domain;
           /* This check is required because we could get the same service via different interfaces */
            if (!rav->RxServiceExists(name) && !rav->TxServiceExists(name))
            {
            	rav->SendDescribe(a , port, name);
            	rav->ReceiveRtspResponse();
            }
        }
    }
    avahi_service_resolver_free(r);
}


static void RavAvahiBrowseCallback(
    AvahiServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata)
{
    RavDiscovery *rav = (RavDiscovery *)userdata;
    /* Called whenever a new services becomes available on the LAN or is removed from the LAN */
    switch (event)
    {
    case AVAHI_BROWSER_FAILURE:
        throw runtime_error("Avahi Browser:" + string(avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b)))));
        break;
    case AVAHI_BROWSER_NEW:
    	rav->CreateResolver(interface, protocol, name, type, domain);
        break;
    case AVAHI_BROWSER_REMOVE:
        CLOG(INFO, RAV_LOG) << "Avahi Browser: REMOVE: service " << name << " of type " << type << " in domain " << domain;
        break;
    case AVAHI_BROWSER_ALL_FOR_NOW:
		CLOG(INFO, RAV_LOG) << "Avahi Browser: ALL_FOR_NOW";
		break;
    case AVAHI_BROWSER_CACHE_EXHAUSTED:
        CLOG(INFO, RAV_LOG) << "Avahi Browser: CACHE_EXHAUSTED";
        break;
    }
}

void RavDiscovery::CreateResolver(AvahiIfIndex interface, AvahiProtocol protocol, const char *name, const char *type, const char *domain)
{
	CLOG(INFO, RAV_LOG) << "Avahi Browser NEW: service " << string(name) << " of type " << string(type) << " in domain " << string(domain);
    if (!(avahi_service_resolver_new(avahiClient, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, (AvahiLookupFlags)0, RavResolverCallback, (void *)this)))
    {
    	throw runtime_error(string("Failed to resolve service : ") + string(name) + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }
}


bool RavDiscovery::RxServiceExists(string name)
{
	for (std::vector<AoipService>::iterator service = ravRxServices.begin() ; service != ravRxServices.end() ; service++)
    {
    	if (name == service->GetName())
        {
            return(true);
        }
    }
    return(false);
}

bool RavDiscovery::TxServiceExists(string name)
{
	for (std::vector<shared_ptr<RavTxService>>::iterator service = ravTxServices.begin() ; service != ravTxServices.end() ; service++)
    {
    	if (name == (*service)->GetName())
        {
            return(true);
        }
    }
    return(false);
}

void RavDiscovery::SendDescribe(string address, uint16_t port, string name)
{
	std::stringstream ss;
	const int opt = 1;

	ss << "DESCRIBE rtsp://" << address << ":" << to_string(port) <<"/by-name/" << name << " RTSP/1.0" << "\r\n";
	ss << "CSeq: " << rtspSeq << "\r\n\r\n";

    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    int res = inet_pton(AF_INET, address.c_str(), (void *) (&(remote.sin_addr.s_addr)));

    if (res < 0)
    {
        throw runtime_error("Send Describe: Can't set remote->sin_addr.s_addr");
    }
    else if (res == 0)
    {
        throw runtime_error(string("Error: Invalid address ") + address);
    }

    rtspTxSock = socket(AF_INET, SOCK_STREAM, 0);

    if (rtspTxSock == -1)
    { 
    	throw runtime_error(string("Failed to create Ravenna Tx Socket: ") + strerror(errno));
    } 

    res = setsockopt(rtspTxSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (res != 0)
    {
    	throw runtime_error(string("Failed to set SO_REUSEADDR on Ravenna Tx Socket: ") + strerror(errno));    	
    }

    remote.sin_port = htons(port);
    if (connect(rtspTxSock,
                (struct sockaddr *) &remote, sizeof(struct sockaddr)) == -1)
    {
        throw runtime_error(string("Error connecting to ") + address + string(" ") + to_string(port));
    }
    send(rtspTxSock, (void *)ss.str().c_str(), ss.str().length(), 0);
}


RavDiscovery::RavDiscovery(const CallBacks &newCallBacks, AoipSystem& newSystem): AoipDiscovery(newCallBacks, newSystem)
{
    struct sockaddr_in servaddr;
    int res;
    const int opt = 1;

   	// This is requires as there a check for first-time initilization in callback
   	avahiClient = nullptr;

    /* Allocate main loop object */
    if (!(avahiPollObject = avahi_simple_poll_new()))
    {
        throw runtime_error("Failed to create Ravenna/Avahi polling object");
    }
    /* Allocate a new client */
    avahi_client_new(avahi_simple_poll_get(avahiPollObject),
    						 (AvahiClientFlags)0, RavAvahiClientCallback, (void *)this , &res);
    // Avahi Client initialized in callback
    if (avahiClient == nullptr)
    {
        throw runtime_error("Failed to create client:" + string(avahi_strerror(res)));
    }
    /* Create the service browser */

    avahiServiceBrowser = avahi_service_browser_new(avahiClient, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
     "_ravenna_session._sub._rtsp._tcp", NULL, (AvahiLookupFlags)0, RavAvahiBrowseCallback, (void *)this);
        
    if (avahiServiceBrowser == nullptr)
    {
    	throw runtime_error("Failed to create service browser:" + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }

    rtspSeq = 1;
    // Create socket for incoming connections
    // Outgoing socket created on as needed basis 
    rtspRxSock = socket(AF_INET, SOCK_STREAM, 0); 
    if (rtspRxSock == -1)
    { 
    	throw runtime_error(string("Failed to create Ravenna Rx Socket: ") + strerror(errno));    	
    } 
    //memset(&servaddr, 0, sizeof(servaddr)); 
  
    res = setsockopt(rtspRxSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (res != 0)
    {
    	throw runtime_error(string("Failed to set SO_REUSEADDR on Ravenna Rx Socket: ") + strerror(errno));    	
    }

    // assign IP, PORT 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    //servaddr.sin_addr.s_addr = GetNetIpInt(system.srcIpStr); 
    servaddr.sin_port = htons(ravennaRtspPort); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(rtspRxSock, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
    	throw runtime_error("Failed to create receive socket for Ravenna connectivity");     
    } 
  
    // Now server is ready to listen and verification 
    if ((listen(rtspRxSock, 5)) != 0)
    { 
    	throw runtime_error("Failed to start RTSP server"); 		
    } 
}

void RavDiscovery::CreateRavennaServices(AvahiClient *newAvahiClient)
{
    int ret;

    if (avahiClient == nullptr)
    {
    	avahiClient = newAvahiClient;
    }

    /* create group */
    mainAvahiGroup = avahi_entry_group_new(avahiClient, RavAvahiEntryGroupCallback, NULL);

    if (mainAvahiGroup == NULL)
    {
        throw runtime_error("mdns_server:: add_service() failed avahi_entry_group_new(): " + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }

    std::stringstream ss;
    ss << system.name;

    ret = avahi_entry_group_add_service(
      mainAvahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {},
      ss.str().c_str(), "_http._tcp", nullptr, nullptr,
      ravennaRtspPort, nullptr);
    if (ret < 0)
    {
    	throw runtime_error("mdns_server::failed to add service _http._tcp " + string(avahi_strerror(ret)));
    }

    ret = avahi_entry_group_add_service_subtype(
      mainAvahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {}, // index interface
      ss.str().c_str(), "_http._tcp", nullptr,
      "_ravenna_session._sub._http._tcp");
    if (ret < 0)
    {
        throw runtime_error("avahi failed to add subtype _ravenna_session._sub._http._tcp: " + string(avahi_strerror(ret)));
    }

    ret = avahi_entry_group_add_service(
      mainAvahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {},
      ss.str().c_str(), "_rtsp._tcp", nullptr, nullptr,
      ravennaRtspPort, nullptr);
    if (ret < 0)
    {
        throw runtime_error("avahi failed to add service _rtsp._tcp: " + string(avahi_strerror(ret)));
    }

    ret = avahi_entry_group_add_service_subtype(
      mainAvahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {}, // index interface
      ss.str().c_str(), "_rtsp._tcp", nullptr,
      "_ravenna._sub._rtsp._tcp");
    if (ret < 0)
    {
        throw runtime_error("avahi failed to add subtype _ravenna_session._sub._rtsp._tcp: " + string(avahi_strerror(ret)));
    }

    /* Tell the server to register the service */
    ret = avahi_entry_group_commit(mainAvahiGroup);
    if (ret < 0)
    {
       	throw runtime_error("failed to commit entry group " + string(avahi_strerror(ret)));
    }
}

void RavDiscovery::AddTxService(StreamInfo& streamInfo)
{
	shared_ptr<RavTxService> tmpPtr = make_shared<RavTxService>(system, streamInfo, avahiClient);
	ravTxServices.push_back(tmpPtr);
}

void RavDiscovery::UpdateTxService(StreamInfo& streamInfo)
{
	bool found = false;
	for (vector<shared_ptr<RavTxService>>::iterator service = ravTxServices.begin() ; service != ravTxServices.end() ; service++)
	{
		if (!streamInfo.streamName.compare((*service)->GetName()))
		{
			ravTxServices.erase(service);
			shared_ptr<RavTxService> tmpPtr = make_shared<RavTxService>(system, streamInfo, avahiClient);
			ravTxServices.push_back(tmpPtr);
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw runtime_error("Tried to update Rav Tx Service that doesn't exist");
	}
}

void RavDiscovery::RemoveTxService(string serviceName)
{
	bool found = false;
	for (vector<shared_ptr<RavTxService>>::iterator service = ravTxServices.begin() ; service != ravTxServices.end() ; service++)
	{
		if (!serviceName.compare((*service)->GetName()))
		{
			ravTxServices.erase(service);
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw runtime_error("Tried to remove Rav Tx Service that doesn't exist");
	}
}

void RavDiscovery::ProcessRxRtspPacket(int connfd)
{
	// print buffer which contains the client contents 
    std::istringstream msg(rxRtspPacket);
    std::string line;
    std::vector<std::string> fields;
    std::string method, rtspHeader, cseqStr;
    std::string streamName = "";

   // First get the Sequence number as it is common
    int cseq = -1;
    while(!msg.eof() && (cseq < 0))
    {
        std::getline(msg,line);
        fields = LineToFields(line);
		GetField(fields, 0, rtspHeader);
    	if (ToLowerStr(rtspHeader) == "cseq")
    	{
    		if (!GetField(fields, 1, cseqStr))
    		{
    			cseq = stoi(cseqStr);
    		}
    	}
    }

    // rewind
    msg.seekg(0);
    std::getline(msg,line);
	fields = LineToFields(line);

    if ((cseq < 0) || (GetField(fields, 0, method)))
    {
    	// Ignore packet
    	return;
    }

	// search for name which is used by some methods
    // Its immediately after "by-name" which must be present so we need at least 2 fields
	bool nameFailed = true;
    if (fields.size() > 1)
    {
    	for (unsigned int i = 0 ; (i < (fields.size() - 1)) && nameFailed ; i++)
    	{
    		if (fields[i] == "by-name")
    		{
    			nameFailed = GetField(fields, i + 1, streamName);
    		}
    	}
    }

    // Only understand describe packets at the moment but will add others later
    if ((method == "DESCRIBE") || (method == "SETUP"))
    {
        if (nameFailed || (streamName.size() == 0))
        {
            return;
        }

        // Check to see if DESCRIBE matches any of the services we are 
        for (vector<shared_ptr<RavTxService>>::iterator txService = ravTxServices.begin() ; txService != ravTxServices.end() ; txService++)
        {
        	if (!streamName.compare((*txService)->GetName()))
        	{
        		if (method == "DESCRIBE")
        		{
    				CLOG(INFO, RAV_LOG) << "Received DESCRIBE for " << streamName;
        			(*txService)->SendRtspDescribeResponse(connfd, cseq);
        		}
        		else
        		{
    				CLOG(INFO, RAV_LOG) << "Received SETUP for " << streamName;        			
        			(*txService)->SendRtspSetupResponse(connfd, cseq);
        		}
        	}
        }
    }
    else if (method == "OPTIONS")
    {
    	SendRtspOptionsResponse(connfd, cseq);
    	CLOG(INFO, RAV_LOG) << "Recevied OPTIONS for " << streamName;
    }
}

void RavDiscovery::SendRtspOptionsResponse(int connfd, unsigned int cseq)
{
	// Send Response
    std::stringstream ss;
    ss  << "RTSP/1.0 200 OK\r\n"
        << "CSeq: " << cseq << "\r\n"
        << "Public: DESCRIBE, SETUP\r\n\r\n";
    CLOG(INFO, RAV_LOG) << "Sending Options Response ";
    send(connfd, (void *)ss.str().c_str(), ss.str().length(), 0);
}


void RavDiscovery::ReceiveRtspPacket(void) 
{ 
    int packetWaiting;
    fd_set rfds;
    struct timeval rxPollTimeOut;
    int connfd;
    struct sockaddr_in cli;
    int len;
    char cliAddr[INET_ADDRSTRLEN];
    int bytesRead;


    // see if there is data to be read
    FD_ZERO(&rfds);
    FD_SET(rtspRxSock, &rfds);
    rxPollTimeOut.tv_sec = 0;
    rxPollTimeOut.tv_usec = 100000; // 100ms polling interval, allows 100ms tx accuracy
    packetWaiting = select(rtspRxSock + 1, &rfds, nullptr, nullptr, &rxPollTimeOut);

    if (packetWaiting < 1)
    {
        return;
    }
    len = sizeof(cli); 
    // Accept the data packet from client and verification 
    connfd = accept(rtspRxSock, (struct sockaddr *)&cli, (socklen_t *)&len); 
    if (connfd < 0)
    {
    	// Don't throw, allow a retry
        return;
    } 

	inet_ntop(AF_INET, &(cli.sin_addr), cliAddr, INET_ADDRSTRLEN);
    CLOG(INFO, RAV_LOG) << "Opening connection with " << cliAddr;

    do
    {
    	bytesRead = read(connfd, rxRtspPacket, sizeof(rxRtspPacket));
		if (bytesRead > 0)
		{
			ProcessRxRtspPacket(connfd);
			FD_ZERO(&rfds);
    		FD_SET(connfd, &rfds);
    		rxPollTimeOut.tv_sec = 0;
    		rxPollTimeOut.tv_usec = 100000; // 100ms polling interval, allows 100ms tx accuracy
    		packetWaiting = select(connfd + 1, &rfds, nullptr, nullptr, &rxPollTimeOut);
    	}
	}
	while((bytesRead > 0) && (packetWaiting > 0));
	#ifdef LOGGING
	if (bytesRead < 1)
	{
		if (bytesRead == 0)
		{
			CLOG(INFO, RAV_LOG) << "Closing connection with - FIN";
		}
		else
		{
			CLOG(INFO, RAV_LOG) << "Closing connection with - RST";			
		}
    }
    else
    {
    	if (packetWaiting == 0)
    	{
    		CLOG(INFO, RAV_LOG) << "Closing connection with " << cliAddr << "- Timeout";
    	}
    	else
    	{
    		CLOG(INFO, RAV_LOG) << "Closing connection with " << cliAddr << "- Error: " << strerror(errno);
    	}
    }
    #endif
    close(connfd);
}

int RavDiscovery::ReceiveRtspResponse(void)
{
    int packetWaiting;
    fd_set rfds;
    struct timeval rxPollTimeOut;
    DescribeResponseParser respParser(rtspSeq++);
    ssize_t packetSize;

    // Check to see if we have a connection open
    if (rtspTxSock == 0)
    {
    	return(0);
    }

    // see if there is data to be read
    FD_ZERO(&rfds);
    FD_SET(rtspTxSock, &rfds);
    rxPollTimeOut.tv_sec = 0;
    rxPollTimeOut.tv_usec = 500000;
    // Wait for 500ms for party to respond
    packetWaiting = select(rtspTxSock + 1, &rfds, nullptr, nullptr, &rxPollTimeOut);

    // Simply wait a period of time and see if we get a response
    // If so response in before timeout then give up and close socket
    if (!packetWaiting)
    {
    	close(rtspTxSock);
        return(-1);
    }

    packetSize = read(rtspTxSock, rxRtspPacket, sizeof(rxRtspPacket));
    // Process if for any state change
    if (packetSize > 0)
    {
   	    respParser.ProcessPacket(rxRtspPacket, MAX_MTU);
    }

    while(!respParser.Finished())
    {
	    FD_ZERO(&rfds);
	    FD_SET(rtspTxSock, &rfds);
	    rxPollTimeOut.tv_sec = 0;
	    rxPollTimeOut.tv_usec = 500000; // 500ms polling interval, allows 100ms tx accuracy
	    packetWaiting = select(rtspTxSock + 1, &rfds, nullptr, nullptr, &rxPollTimeOut);
	    if (!packetWaiting)
	    {
	    	close(rtspTxSock);
	        return(0);
	    }
    	// read the message from client and copy it in buffer 
    	if (read(rtspTxSock, rxRtspPacket, sizeof(rxRtspPacket)) == 0)
        {
            CLOG(WARNING, RAV_LOG) << "Packet reported waiting but read failed";
            close(rtspTxSock);
            return(0);
        }
    	respParser.ProcessPacket(rxRtspPacket, MAX_MTU);
    }

    if (respParser.GotSdp())
    {
    	StreamInfo newStreamInfo(respParser.GetSdp().GetStreamInfo());
		SdpSystemInfo newSdpSystemInfo(respParser.GetSdp().GetSystemInfo());
		AoipService newService(AOIP_SERVICE_RAVENNA, newStreamInfo, newSdpSystemInfo);
		ravRxServices.push_back(newService);
        if (callBacks.addRxServiceCallBack)
        {
		  callBacks.addRxServiceCallBack(AOIP_SERVICE_RAVENNA, newService);
        }
    }
    // read the message from client and copy it in buffer 
    // Close socket as no further interaction is allowed
    close(rtspTxSock);

    return(1);
    }


void RavDiscovery::Poll(void)
{
	avahi_simple_poll_iterate(avahiPollObject, 100); // 100ms = sleep time
    ReceiveRtspPacket(); // returns after 100ms
}



/****************** DescribeResponseParser *******************/


void RavDiscovery::DescribeResponseParser::ProcessPacket(const char *pkt, unsigned int len)
{
	// print buffer which contains the client contents 
    std::istringstream msg(string(pkt, len));
    std::string line;
    std::vector<std::string> fields;
    std::string method, rtspHeader;
  	string contentLengthStr, contentType, contentSubType;


    // Check protocol in first packet
    if (packetCounter == 0)
    {
    	std::string protocol, version, statusCodeStr, status;

	    std::getline(msg,line);
		fields = LineToFields(line);

	    if ((GetField(fields, 0, protocol)) ||
	    	(GetField(fields, 1, version)) ||
	    	(GetField(fields, 2, statusCodeStr)) ||
	    	(GetField(fields, 3, status)))
	    {
	    	finished = true;
	    	return;
	    }
    	if ((protocol != "RTSP") ||
    		(version != "1.0") ||
    		(statusCodeStr != "200") ||
    		(status != "OK"))
	    {
	    	finished = true;
	    	return;
	    }
	}

	while(!msg.eof())
	{
    	std::getline(msg,line);
		fields = LineToFields(line);

		if ((line.length() > 1) &&
			(line[1] == '='))
		{
			sdpText = sdpText + line + string("\n");
		}
		else
		{
			if (!GetField(fields, 0, rtspHeader))
			{
				if (rtspHeader == "content-type")
				{
					GetField(fields, 1, contentType);
					GetField(fields, 2, contentSubType);
			  		if ((contentType != "application") ||
						(contentSubType != "sdp"))
			  		{
			  			finished = true;
			  			return;
			  		}
				}
				else if (rtspHeader == "content-length")
				{
					GetField(fields, 1, contentLengthStr);
					contentLength = stoi(contentLengthStr);
				}
				else if (ToLowerStr(rtspHeader) == "cseq")
				{
					string rxCseqStr;
					GetField(fields, 1, rxCseqStr);
					if ((int)cseq != stoi(rxCseqStr))
					{
						finished = true;
						return;
					}
				}
			}
		}
	}

	if (sdpText.size() > 0)
	{
		// Trim SDP if required
		if (sdpText.size() > contentLength)
		{
			sdpText = sdpText.substr(0, contentLength - 1);
		}
		sdp.SetText(sdpText.c_str());
		// SDP only allowed in one packet so assume finished if we got something
		finished = true;
	}
	packetCounter++;
}


/************************** RavTxService ************************/

std::string name1 = "MyStreamName";


AvahiEntryGroup *add_mdns_service(AvahiClient *client)
{
    AvahiEntryGroup *group;
    std::stringstream ss;
    ss << name1;

    /* create group */
    group = avahi_entry_group_new(client, RavAvahiEntryGroupCallback, NULL);

    if (group == NULL)
    {
        throw runtime_error("Failed avahi_entry_group_new(): " + string(avahi_strerror(avahi_client_errno(client))));
    }

    int ret = avahi_entry_group_add_service(
      group, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {},
      ss.str().c_str(), "_rtsp._tcp", nullptr, nullptr,
      ravennaRtspPort, nullptr); // 554 = common RTSP port
    if (ret < 0)
    {
        throw runtime_error("failed to add service _rtsp._tcp: " + string(avahi_strerror(ret)));
    }


    ret = avahi_entry_group_add_service_subtype(
        group, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {}, // index interface
        ss.str().c_str(), "_rtsp._tcp", nullptr,
        "_ravenna_session._sub._rtsp._tcp");
    if (ret < 0)
    {
        throw runtime_error("failed to add subtype _ravenna_session._sub._rtsp._tcp: " + string(avahi_strerror(ret)));
    }

    /* Tell the server to register the service */
    ret = avahi_entry_group_commit(group);
    if (ret < 0)
    {
        throw runtime_error("failed to commit entry group: " + string(avahi_strerror(ret)));
    }
    return(group);
}



RavDiscovery::RavTxService::RavTxService(AoipSystem& system, StreamInfo& streamInfo, AvahiClient *avahiClient)
{
	SdpSystemInfo sdpSystemInfo;

	name = streamInfo.streamName;
	sessionId = streamInfo.sessionId;
	sdpSystemInfo.domain = system.domain;
	sdpSystemInfo.gmIdentity = system.gmIdentity;

   	Sdp2110 sdp(streamInfo, sdpSystemInfo);
   	// Generate SDP and keep it for later
   	sdpText = sdp.GetText(false); // no rmax here

   	CLOG(INFO, RAV_LOG) << "Creating entry group for " << name;

   	if (avahi_client_get_state(avahiClient) != AVAHI_CLIENT_S_RUNNING)
   	{
    	throw runtime_error("mdns_server:: add_service() failed client is not running");
  	}

  	//add_mdns_service(avahiClient);
  	//return;

    /* create group */
    avahiGroup = avahi_entry_group_new(avahiClient, RavAvahiEntryGroupCallback, NULL);

    if (avahiGroup == NULL)
    {
        throw runtime_error("Failed avahi_entry_group_new():"  + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }

    int ret = avahi_entry_group_add_service(
      avahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {},
      name.c_str(), "_rtsp._tcp", nullptr, nullptr,
      ravennaRtspPort, nullptr); // 554 = common RTSP port
    if (ret < 0)
    {
        throw runtime_error("Failed to add service _rtsp._tcp" + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }

    ret = avahi_entry_group_add_service_subtype(
        avahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, {}, // index interface
        name.c_str(), "_rtsp._tcp", nullptr,
        "_ravenna_session._sub._rtsp._tcp");
    if (ret < 0)
    {
    	throw runtime_error("Failed to add subtype _ravenna_session._sub._rtsp._tcp" + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }

    /* Tell the server to register the service */
    ret = avahi_entry_group_commit(avahiGroup);
    if (ret < 0)
    {
    	throw runtime_error("Failed to commit entry group" + string(avahi_strerror(avahi_client_errno(avahiClient))));
    }
}

RavDiscovery::RavTxService::~RavTxService()
{
	avahi_entry_group_free(avahiGroup);
}

string RavDiscovery::RavTxService::GetName(void)
{
	return(name);
}

void RavDiscovery::RavTxService::SendRtspDescribeResponse(int connfd, unsigned int cseq)
{
	// Send Response
    std::stringstream ss;
    ss  << "RTSP/1.0 200 OK\r\n"
        << "CSeq: " << cseq << "\r\n"
        << "Content-Length: " << sdpText.length() << "\r\n"
        << "Content-Type: application/sdp\r\n"
        << "\r\n"
        << sdpText;
    CLOG(INFO, RAV_LOG) << "Sending Response to DESCRIBE for stream " << GetName();
    send(connfd, (void *)ss.str().c_str(), ss.str().length(), 0);
}

void RavDiscovery::RavTxService::SendRtspSetupResponse(int connfd, unsigned int cseq)
{
	// Send Response
    std::stringstream ss;
    std::time_t t = std::time(nullptr);


    ss  << "RTSP/1.0 200 OK\r\n"
        << "CSeq: " << cseq << "\r\n"
        << "Date: " << std::put_time(std::gmtime(&t), "%c %Z") << "\r\n"
        << "Session: " << sessionId << "\r\n"
        << "Transport: RTP/AVP;multicast;client_port=5004\r\n\r\n";
    CLOG(INFO, RAV_LOG) << "Sending Response to SETUP for stream " << GetName();
    send(connfd, (void *)ss.str().c_str(), ss.str().length(), 0);
}
