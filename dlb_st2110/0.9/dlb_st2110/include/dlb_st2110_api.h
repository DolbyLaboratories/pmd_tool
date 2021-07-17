/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2020 by Dolby Laboratories,
 *                Copyright (C) 2019-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/** @ingroup st2110 */

/** @file aoip_services.h
 *  @brief Provides ST2110-30/21 receive and transmit services - API
 *  This API is described in this file with some additional structures
 *  defined in dlb_st2110.h
 */


#ifndef _DLB_ST2110_API_H_
#define _DLB_ST2110_API_H_

#include "dlb_st2110.h"
#include "dlb_aoip_services.h"

/**
 * @brief Main Audio Over IP Services Class
 *
 * This class first must be instantiated before any receive streams or transmit streams can be created
 */
class AoipServices
{

public:

/**
 * @brief Main Audio Over IP Service Class Constructor
 *
 * This constructor is the first call to occur 
 */
AoipServices(
	AoipSystem &newSystem, /**< System Details */
	void (*newRxServiceCallBack) (AoipService &)); /**< Callback to register new available streams for reception */

	~AoipServices(void);

/**
 * @brief Main Audio Over IP Service Class Copy Constructor
 *
 * Copy Constructor is deleted as it uses threads that cannot be copied
 */
	AoipServices(AoipServices& copy) = delete;

/**
 * @brief Creates a transmit stream
 * 
 * This function creates a new transmit stream based on the stream information. Once the stream
 * is started callbacks will occur with buffers for the callback function to write the audio
 * samples to be transmitted.
 */
 	void AddTxStream(
		StreamInfo &newStreamInfo,
		ST2110TransmitterCallBackInfo *callBackInfo);
/**
 * @brief Generates a transmit stream
 * 
 * This function generates a new transmit stream based on the stream information. Samples are
 * internally generated instead of being written by callback function. This is to provide an
 * easy way to generate a stream without having to write a callback function.
 */
	void AddTxStream(StreamInfo &newStreamInfo);
/**
 * @brief Start a transmit stream
 * This function starts a previously created transmit stream. Once this function is called callbacks
 * will occur. If the stream is being generated then packets will appear on the network without any
 * callbacks occuring
 */
	void StartTxStream(				
		std::string streamName); /** < Name of stream to be started */

	void AddRxStream(StreamInfo &newStreamInfo, ST2110ReceiverCallBackInfo &callBackInfo);
	void StartRxStream(std::string streamName);
/**
 * @brief Create a bidirectional audio stream
 *
 * This class implements an ST2110 receiver and transmitter or transceiver using a single callback
 * Only one receiver is supported but multiple transmitters may be supplied as a vector. All the streams are
 * fully sychronized. If multiple synchonized streams are required to be received then
 * individual transmitters and receivers should be used with the synchonization handled
 * above this layer. This function must be called before the start function.
 */
	void *AddRxTxStream( 						/**< Returns opaque handle */
		StreamInfo &rxStreamInfo, 				/**< Receive stream information */
	 	std::vector<StreamInfo> &txStreamInfos, /**< List of transmit stream information structures, one for each stream */
	 	float latency, 							/**< Latency in seconds. Normally this should be no more than a second */
	 	ST2110TransceiverCallBackInfo &callBackInfo); /**< Definitions for the application callback function */

/**
 * @brief Starts a bidirectional audio stream
 *
 * This function is called after a bidrectional audio stream is created
 * Callbacks will occur after this function is called with receive and transmit audio buffers
 */
	bool StartRxTxStream( /**< Returns true on success */
		void *hdl); /**< Opaque handle of transmit/receive stream to be started */

/**
 * This function provides information about the available streams for reception to the application.
 * It gives one entry per stream and indicates which services are advertising the stream.
 */
	std::vector<AoipService>& GetAvailableServicesForRx(void);

	/* Stuff for Services, needs to be protected or use friends etc */
	void AddRxService(AoipServiceType serviceType, AoipService &newService);
	void UpdateRxService(AoipServiceType serviceType, AoipService &updatedService);
	void RemoveRxService(AoipServiceType serviceType, std::string serviceName);


private:

	/* private state */
	enum AoipState
	{
		DLB_AOIP_STARTUP = 0,
		DLB_AOIP_ACTIVE,
		DLB_AOIP_SHUTDOWN
	};

	/************************* Constants ***************************/
	// Maximum number of channels supported in a stream
	const unsigned int maxChannels = 64;

	/* private data holding information about receivers and transmitters */
	std::vector<ST2110Receiver> receivers;
	std::vector<AoipService> sapRxServices;
	std::vector<AoipService> ravRxServices;
	std::vector<AoipService> allRxServices; // Made available for queries from upper level
	std::vector<std::shared_ptr<AoipTxStream>> txStreams;
	std::vector<std::shared_ptr<AoipRxTxStream>> rxTxStreams;
	std::vector<std::shared_ptr<AoipRxTxStream::TxCallBackData>> txCallBackData;

	/* General context information */
	/* single thread for services such as Ravenna and SAP */
	std::thread servicesThread;
	AoipState state;
	void (*rxServiceCallBack) (AoipService &);
	AoipSystem system;
	/* Hardware description */
	ST2110Hardware *hardware;
	/* Handles for services */
	SapDiscovery *sap;
	RavDiscovery *rav;

	void ValidateTxStreamInfo(StreamInfo &newStreamInfo);

	void AoipServicesThread(void);

	uint32_t GetHostSrcIpInt(void)
	{
		return(ntohl(inet_addr(system.srcIpStr.c_str())));
	}

	uint32_t GetNetSrcIpInt(void)
	{
		return(inet_addr(system.srcIpStr.c_str()));
	}

	const char *GetSrcIpStr(void)
	{
		return(system.srcIpStr.c_str());
	}

};

#endif // _DLB_ST2110_API_