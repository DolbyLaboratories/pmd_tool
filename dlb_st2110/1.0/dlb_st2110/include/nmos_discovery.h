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

#ifndef _NMOS_DISCOVERY_H_
#define _NMOS_DISCOVERY_H_

#include <memory>
#include <map>
#include <string>

#include "dlb_aoip_discovery.h"
#include "dlb_st2110_api.h"
#include "dlb_st2110.h"
#include "dlb_nmos_node_api.h"


/* Forward Definitions */
/*
namespace nmos
{
    struct CallBacks;
    class DlbNmosNode;
    typedef std::map<std::string, std::string> DlbNmosSdpMap;
}
*/

/* Constants */

/* Classes */

class NmosDiscovery : public AoipDiscovery
{
private:

    std::shared_ptr<nmos::CallBacks> nmosNodeCallBacks;
    std::shared_ptr<nmos::DlbNmosNode> nmosNode;
    char rxRtspPacket[MAX_MTU];
    char txRtspPacket[MAX_MTU];

public:
	NmosDiscovery(const AoipDiscovery::CallBacks &newCallBacks, AoipSystem &newSystem);
	void Poll(void) {}
	void AddTxService(StreamInfo& streamInfo);
	void UpdateTxService(StreamInfo& streamInfo);
	void RemoveTxService(std::string serviceName);
	void SetInputService(std::string serviceName);

	~NmosDiscovery()
	{
	}

	void NmosNodeCallBack(
    const nmos::DlbNmosSdpMap &sdpList, /** New Complete list of Audio Stream SDPs, This provides 1 API option. Next 3 provide an alternative API */
    const nmos::DlbNmosSdpMap &addedSdpList, /** List of SDPs that are associated with new streams added since last callback */
    const nmos::DlbNmosSdpMap &changedSdpList, /** List of SDPs that are associated with streams modified since last callback */
    const nmos::DlbNmosSdpMap &removedSdpList /** List of SDPs that are associated with streams that have been removed since last callback */
	);

};

#endif // _NMOS_DISCOVERY_H_