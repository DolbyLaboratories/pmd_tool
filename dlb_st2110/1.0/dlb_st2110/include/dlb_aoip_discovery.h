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

#ifndef _DLB_AOIP_DISCOVERY_H_
#define _DLB_AOIP_DISCOVERY_H_

#include <functional>

#include "dlb_st2110.h"

#define MAX_MTU 1500

class AoipServices;

class AoipDiscovery
{
public:

    typedef std::function<void(const AoipServiceType serviceType, const AoipService &newService)> AddRxServiceCallBack;
    typedef std::function<void(const AoipServiceType serviceType, const AoipService &updatedService)> UpdateRxServiceCallBack;
    typedef std::function<void(const AoipServiceType serviceType, const std::string serviceName)> RemoveRxServiceCallBack;
    typedef std::function<void(const std::string sdp)> ConnectionReqCallBack;

    struct CallBacks
    {
        AddRxServiceCallBack addRxServiceCallBack;
        UpdateRxServiceCallBack updateRxServiceCallBack;
        RemoveRxServiceCallBack removeRxServiceCallBack;
        ConnectionReqCallBack connectionReqCallBack;
    };

	AoipDiscovery(const CallBacks &newCallBacks, AoipSystem &newSystem ) : callBacks(newCallBacks), system(newSystem) {};
    virtual ~AoipDiscovery() {};
	virtual void Poll(void) = 0;
	virtual void AddTxService(StreamInfo& streamInfo) = 0;
	virtual void UpdateTxService(StreamInfo& streamInfo) = 0;
	virtual void RemoveTxService(std::string serviceName) = 0;
    virtual void SetInputService(std::string serviceName) = 0;

protected:
    CallBacks callBacks;
	AoipSystem system;

};

#endif // _DLB_AOIP_DISCOVERY_H_
