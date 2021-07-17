/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef _DLB_AOIP_DISCOVERY_H_
#define _DLB_AOIP_DISCOVERY_H_

#include "dlb_st2110.h"
//#include "dlb_aoip_services.h"

#define MAX_MTU 1500

class AoipServices;

class AoipDiscovery
{
public:
	AoipDiscovery(AoipServices &newServices, AoipSystem &newSystem) : owner(newServices), system(newSystem) {};
    virtual ~AoipDiscovery() {};
	virtual void Poll(void) = 0;
	virtual void AddTxService(StreamInfo& streamInfo) = 0;
	virtual void UpdateTxService(StreamInfo& streamInfo) = 0;
	virtual void RemoveTxService(std::string serviceName) = 0;

protected:
	AoipServices &owner;
	AoipSystem system;

};

#endif // _DLB_AOIP_DISCOVERY_H_
