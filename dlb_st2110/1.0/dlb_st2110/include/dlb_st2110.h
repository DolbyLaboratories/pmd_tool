/************************************************************************
 * dlb_st2110
 * Copyright (c) 2019-2020, Dolby Laboratories Inc.
 * Copyright (c) 2019-2020, Dolby International AB.
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

#ifndef _DLB_ST2110_STREAM_H_
#define _DLB_ST2110_STREAM_H_

#include <vector>
#include <ostream>
#include <string>
#include "arpa/inet.h"

/************************* Enums ***************************/

/**
 * @brief Defines types of streams that can be transmitted or received
 * 
 * This list the stream types that can be transmitted or received. This does
 * not guarantee that all stream type can be both transmitted or received.
 * Check the top level functionality summary to check what stream support is
 * provided.
 */
enum StreamType
{
	STREAM_TYPE_NONE,
	AES67,				/**< AES67 or SMPTE ST 2110-30, Only support linear PCM */
	AM824,				/**< AM824 or SMPTE ST 2110-31, Provides transparent AES3 transport. Channel status is set to Data Mode and Professional */
	SMPTE2110_41		/**< SMPTE ST 2110-41 */
};

/**
 * @brief Defines the discovery service methods supported
 * 
 * This defines a list of discovery methods that are supported.
 * It may be possible to discover the same stream through multiple
 * discovery methods.
 */
enum AoipServiceType
{
	AOIP_SERVICE_NONE = 0, 		/**< Null discovery method */ 
	AOIP_SERVICE_RAVENNA = 1,	/**< Ravenna discovery method. Multicast DNS + RTSP. See Ravenna Technical Reference for details */
	AOIP_SERVICE_SAP = 2,      /**< SAP as used by Dante systems. See RFC 2974 */
	AOIP_SERVICE_NMOS = 4
};


/**
 * @brief Defines a list of audio formats
 * 
 * This defines a list of audio formats that might be used by streams but also is used to
 * define what format audio is presented back to the user via the calback functions.
 * Transmitters and receivers will convert to and from stream format to the requested
 * format. Unsupported format combinations will throw exceptions. As of the time of writing
 * the 16 and 24 bit formats are well supported but the other formats are not.
 */
enum AoipAudioFormat
{
	DLB_AOIP_AUDIO_FORMAT_NONE = 0,			/**< Null Audio Format */
	DLB_AOIP_AUDIO_FORMAT_8BIT_LPCM = 1,	/**< 8 bit Linear PCM */
	DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM = 2,	/**< 16 bit Linear PCM */
	DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM = 3,	/**< 24 bit Linear PCM */
	DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM = 4,	/**< 32 bit Linear PCM */
	DLB_AOIP_AUDIO_FORMAT_32BIT_FLOAT = 5,	/**< 32 bit IEEE floating point format i.e. float */
	DLB_AOIP_AUDIO_FORMAT_32BIT_LAST = 5,	
};

/**
 * @brief This simple helper function returns the
 * size in bytes of an audio format as defined in AoipAudioFormat.
 */
inline
unsigned int GetAoipBytesPerSample(AoipAudioFormat format)
{
	switch (format)
	{
	case DLB_AOIP_AUDIO_FORMAT_8BIT_LPCM:
		return(1);
	case DLB_AOIP_AUDIO_FORMAT_16BIT_LPCM:
		return(2);
	case DLB_AOIP_AUDIO_FORMAT_24BIT_LPCM:
		return(3);
	case DLB_AOIP_AUDIO_FORMAT_32BIT_LPCM:
		return(4);
	case DLB_AOIP_AUDIO_FORMAT_32BIT_FLOAT:
		return(4);
	default:
		throw std::runtime_error("Unsupported Audio Format");
	}
}


/************************* Callback Structures ***************************/

/**
 * @brief This structure is used by the transceiver function for application
 * callback definition
 * 
 */
struct ST2110TransceiverCallBackInfo
{
	bool (*callBack) 		/**< Function to be called back */
	(void *, 				/**< Opaque pointer to user data */
	 void *, 				/**< Input Audio Pointer */
	 void *, 				/**< Output Audio Pointer */
	 unsigned int, 			/**< Input Audio datasize in bytes */
	 unsigned int); 		/**< Output Audio datasize in bytes */

	void *data; 			/**< Opaque data to be passed back in callback */
	unsigned int blockSize; /**< Preferred block size */
	AoipAudioFormat audioFormat; /**< Audio format to be assumed by callback function */
};

/**
 * @brief This structure is used by the transmitter function for application
 * callback definition
 * 
 */
struct ST2110TransmitterCallBackInfo
{
	bool (*callBack) 		/**< Function to be called back */
	(void *,					/**< Opaque pointer to user data */
	 void *, 				/**< Output Audio Pointer */
	 unsigned int&,		/**< Output Audio datasize requested in bytes, return value indicated datasize written */
	 bool&,					/**< haveTimeStamp, return indicates if audio to be transmitter has a timestamp */
	 uint32_t&); 			/**< 32bit RTP timestamp corresponding to first audio sample in buffer */

	void *data; 			/**< Opaque data to be passed back in callback */
	unsigned int blockSize; /**< Preferred block size */
	AoipAudioFormat audioFormat; /**< Audio format to be assumed by callback function */
};

/**
 * @brief This structure is used by the receiver function for application
 * callback definition
 * 
 */
struct ST2110ReceiverCallBackInfo
{
	              // opaque pointer, data pointer, block siz, haveTimeStamp, timestamp
	bool (*callBack) 		/**< Function to be called back */
	(void *,				/**< Opaque pointer to user data */
	 void *, 				/**< Input Audio Pointer */
	 unsigned int,			/**< Input Audio datasize in bytes */
	 uint32_t);				/**< 32bit RTP timestamp corresponding to first audio sample in buffer */
	void *data;				/**< Opaque data to be passed back in callback */
	unsigned int blockSize; /**< Preferred block size */
	AoipAudioFormat audioFormat; /**< Audio format to be assumed by callback function */
};


/************************* other Structures ***************************/

/**
 * @brief This structure is used to hold system related information that is 
 * also included in an SDP definition.
 * The SDP system info combined with stream information is required to
 * generate an SDP.
 */
class SdpSystemInfo
{
public:
	std::string gmIdentity; /**< PTP GrandMaster Identity */
	unsigned int domain; 	/**< PTP domain */

	SdpSystemInfo() { reset(); }

	void reset(void)
	{
		domain = 0;
	}
};

/**
 * @brief This defines a Audio Over IP System including all the
 * information that is typically shared across streams.
 * To create a stream, this information is needed as well as the stream
 * information itself */

struct AoipPort
{
	std::string interfaceName;			/**< name of host or interface > */
	std::string ipStr;		/**< The IPv4 address> */	
};

class AoipSystem
{
public:
	// System wide stuff that is invariant of input stream
	// Source IP address to receive/send sap packets
	// Stuff that could vary per input stream
	std::string gmIdentity; 			/**< PTP Grandmaster identity */
	unsigned int domain; 				/**< PTP domain */
	unsigned int samplingFrequency;		/**< Sampling Frequency in Hz */
	// Limited to single interface and source IP address at the moment
	std::string name;					/**< Node name used by Multicast DNS discovery */
	AoipPort mediaInterface;
	AoipPort manageInterface;
	std::string nmosRegistry;        /** Can be hostname or IP Address, Blank specifies "Auto" using mDNS **/
 
	void reset(void)
	{
		domain = 0;
		samplingFrequency = 0;
		name = "dlb2110node";
		nmosRegistry = "";
	}

};

/**
 * @brief Defines the stream information specific for audio streams.
 * This is mutually exclusive with the metadata stream information.
 * That is, a stream must either be an audio or a metadata stream but not
 * both. This structure is relevant for AES67/SMPTE ST 2110-30 streams and
 * AM824/SMPTE ST 2110-31 streams.
 */
struct AudioStreamInfo
{
	unsigned int numChannels;			/**< Number of mono audio channels in the stream */
	unsigned int payloadBytesPerSample;	/**< Number of bytes per audio sample */
	unsigned int samplesPerPacket;		/**< Number of samples in an ethernet packet. Typically 48 for 8 channels or less */
};

/**
 * @brief Defines the stream information specific for metadata streams.
 * This is mutually exclusive with the metadata stream information.
 * That is, a stream must either be an audio or a metadata stream but not
 * both. This structure is relevant for SMPTE ST 2110-41 streams
 */
struct MetadataStreamInfo
{
	float packetTimeMs;					/**< The packet repition rate */
	unsigned int maxPayloadSizeBytes;	/**< The maximum possible payload size for the metadata */
	std::vector<uint32_t> dataItemTypes;/**< Data Item Type for stream. See SMPTE ST 2110-41 standard */
};

/**
 * @brief Top level structure for all received and transmitted streams
 * When a stream is transmitted this structure must be populated by the application.
 * When a stream is received this structure is populated for the application to use
 */
class StreamInfo
{

public:
	std::string streamName;					/**< Name of stream. Inserted into the SDP */
	StreamType streamType;					/**< Indicates the type of stream that is to be transmitted or has been received */
	unsigned int payloadType; 				/**< Dynamic RTP payloads as always used. Must be between 96 and 127 inclusive */
	std::string dstIpStr;					/**< Destination IPv4 address */
	std::string srcIpStr;					/**< Source IPv4 address */
	uint16_t port;							/**< Destination port on reception. Source and destination on transmit. Use 0 for auto selection on transmit */
	uint64_t sessionId;						/**< Session identifier. Goes into the SDP on the o= line. See RFC 4566 */
	uint32_t ssrc;							/**< Synchronization source. Goes in RTP Header. See RFC 3550 */
	std::vector<std::string> channelLabels; /**< Channel labels to place in the i= line on the transmit side. Not support for receive */
	unsigned int samplingFrequency;			/**< Sampling Frequency in Hertz */
	float latency; 							/**< Latency to be used for stream in seconds */
    AudioStreamInfo audio;					/**< Audio stream info. Used if streamType == AES67/SMPTE ST 2110-30 or AM824 or SMPTE ST 2110-31 */
    MetadataStreamInfo metadata;			/**< Metadata stream info. Used if streamtype is SMPTE ST 2110-41 */
    unsigned int sourceIndex; 				/**< Index into stream multiplex. Only used with multiple transmitters. Otherwise assumed to be 0 */

    StreamInfo() { reset(); }

/**
 * @brief This function resets a StreamInfo object
 * Normally this function is only called during construction but can be used to return a StreamInfo object to its original state.
 * This is not a valid state but avoids the possibility of stale data being reused.
 */
    void reset(void)
    {
    	streamName = "";
    	streamType = STREAM_TYPE_NONE;
    	payloadType = 0;
    	dstIpStr = "";
    	srcIpStr = "";
    	port = 0;
    	sessionId = 0;
    	ssrc = 0;
    	samplingFrequency = 0;
    	latency = 0.0;
    	audio.numChannels = 0;
    	audio.payloadBytesPerSample = 0;
    	audio.samplesPerPacket = 0;
    	metadata.maxPayloadSizeBytes = 0;
    	metadata.packetTimeMs = 0.0;
    	sourceIndex = 0;
    }

/**
 * @brief This function prints out the contents of a streamInfo object to an ostream
 */
	void Print(std::ostream& os) const
	{
		os << "Stream Name: " << streamName << std::endl;
		os << "Stream Type: ";
		switch(streamType)
		{
		case AES67:
			os << "AES67";
			break;
		case AM824:
			os << "AM824";
			break;
		case SMPTE2110_41:
			os << "SMPTE ST2110-41";
			break;
		default:
			os << "unknown";
		}
		os << std::endl;
		os << "Payload Type: " << payloadType << std::endl;
		os << "Destination IP: " << dstIpStr << std::endl;
		os << "Source IP: " << srcIpStr << std::endl;
		os << "Port: " << port << std::endl;
		os << "Session Id: " << sessionId << std::endl;
		os << "SSRC: " << ssrc << std::endl;
		os << "Sampling Frequency: " << samplingFrequency << std::endl;
		os << "Latency (sec): " << latency << std::endl;
		os << "Source Index: " << sourceIndex << std::endl;

		if ((streamType == AES67) ||
			(streamType == AM824))
		{
			os << "Number of Channels: " << audio.numChannels << std::endl;
			os << "Bytes per Sample: " << audio.payloadBytesPerSample << std::endl;
			os << "Samples per Packet: " << audio.samplesPerPacket << std::endl;
		}
		if (streamType == SMPTE2110_41)
		{
			os << "Packet Time (ms): " << metadata.packetTimeMs << std::endl;
			os << "Max. Payload Size (Bytes): " << metadata.maxPayloadSizeBytes << std::endl;
			os << "Data Item Types: ";
			for (std::vector<uint32_t>::const_iterator i = metadata.dataItemTypes.begin() ; i != metadata.dataItemTypes.end() ; i++)
			{
				os << std::hex << (*i) << ", ";
			}
			os << std::endl;
		}
	}
};

/**
 * @brief This class describes a Audio Over IP Service
 * This includes the description of an available stream and the discovery method (service type)
 * which discovered the service. Typically streams are advertised using multiple discovery
 * methodsand so multiple structures may exist that contain the same information but only
 * differ by the discovery method or service Type.
 * A service description is obtained from the SDP as per RFC 4655. To describe a service the
 * SDP document must to be used to populate the stream information and the SDP system information.
 * The stream Information tends to vary across streams whereas the SDP system information tends to
 * be shared across streams and vary between different systems or networks.
 */
class AoipService
{
	AoipServiceType serviceType;
	StreamInfo streamInfo;
	SdpSystemInfo sdpSystemInfo;

public:

	AoipService()
	{
		serviceType = AOIP_SERVICE_NONE;
		streamInfo.reset();
		sdpSystemInfo.reset();
	}

	AoipService( AoipServiceType newServiceType, StreamInfo& newStreamInfo, SdpSystemInfo& newSdpSystemInfo):
	 serviceType(newServiceType), streamInfo(newStreamInfo), sdpSystemInfo(newSdpSystemInfo) {}

	bool Compare(const AoipService &service2);

	std::string GetName() const
	{
		return(streamInfo.streamName);
	}

	StreamInfo GetStreamInfo() const
	{
		return(streamInfo);
	}

	AoipServiceType GetType() const
	{
		return(serviceType);
	}

	void Print(std::ostream& os) const
	{
		os << "Service" << std::endl << "-------" << std::endl;
		if (serviceType == AOIP_SERVICE_RAVENNA)
		{
			os << "Service Type: Ravenna" << std::endl;
		}
		else if (serviceType == AOIP_SERVICE_SAP)
		{
			os << "Service Type: SAP" << std::endl;
		}
		else
		{
			os << "Service Type: Unknown" << std::endl;
		}
		//os << std::endl << "SDP" << std::endl << "---" << std::endl << sdpText << std::endl;
		streamInfo.Print(os);
		os << "GM Identity: " << sdpSystemInfo.gmIdentity << std::endl;
		os << "Domain: " << sdpSystemInfo.domain << std::endl;
	}
};

/************************** Helper Functions *******************/

/**
 * @brief This function converts an IPv4 address as a string to a 32 integer in host order
 */
inline
uint32_t GetHostIpInt(const std::string &srcIpStr)
{
	return(ntohl(inet_addr(srcIpStr.c_str())));
}

/**
 * @brief This function converts an IPv4 address as a string to a 32 integer in network order
 */
inline
uint32_t GetNetIpInt(const std::string &srcIpStr)
{
	return(inet_addr(srcIpStr.c_str()));
}


/**
 * @brief This function converts a port in host order to network order
 */
inline
uint16_t
GetNetPort( const uint16_t port)
{
	return(htons(port));
}

#endif // AUDIO_MD_STREAM_H