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
#include <iostream>
#include "dlb_st2110_api.h"

#define VERSION "0.9"

using namespace std;

void newServiceCallBack(const AoipService &newService)
{
	cout << "New Service" << endl << "=== =======" << endl << newService;
}

void newConnectionReqCallBack(const std::string sdp)
{
	cout << "Received new conenction request. This was unexpected." << endl;
	cout << "SDP received:" << endl << sdp << endl;
}


void PrintUsage(void)
{
	fprintf(stderr, "dlb_aoip_discovery_main <INTERFACE> <DOMAIN> v%s\n", VERSION);
	fprintf(stderr, "Copyright Dolby Laboratories Inc., 2021. All rights reserved.\n\n");
	fprintf(stderr, "<INTERFACE>               Interface to listen for SAP and use for mDNS\n");
	fprintf(stderr, "<DOMAIN>                  PTP Domain (default = 0)");
}

int main(int argc, char *argv[])
{
	AoipSystem system;

	if (argc > 3)
	{
		PrintUsage();
		exit(-1);
	}

	AoipPort interface;

	if (argc == 1)
	{
		interface.interfaceName = "enp3s0f0";
		printf("Using enp3s0f0\n");

	}
	else
	{
		interface.interfaceName = argv[1];
		if (argc == 3)
		{
			system.domain = atoi(argv[2]);
			if (system.domain > 127)
			{
				throw std::runtime_error("Invalid PTP domain");
			}
		}
		else
		{
			system.domain = 0;
		}
	}

	system.mediaInterface = system.manageInterface = interface;

	system.name = "dlb_discovery";

	system.samplingFrequency = 48000;



	AoipServices::CallBacks callBacks;

	callBacks.newRxServiceCallBack = newServiceCallBack;
	callBacks.connectionReqCallBack = newConnectionReqCallBack;

	unsigned int services = AOIP_SERVICE_RAVENNA | AOIP_SERVICE_SAP | AOIP_SERVICE_NMOS;

	try
	{
		AoipServices aoipService(system, services, callBacks);
		unsigned int sleepComplete = 0;

		while(sleepComplete == 0)
		{
			sleepComplete = sleep(5);
			vector<AoipService>& rxServices = aoipService.GetAvailableServicesForRx();
			printf("Service List\n");
			printf("======= ====\n");
			for (vector<AoipService>::iterator service = rxServices.begin() ; service != rxServices.end() ; service++)
			{
				if (service->GetType() == AOIP_SERVICE_RAVENNA)
				{
					cout << "Ravenna: ";
				}
				else if (service->GetType() == AOIP_SERVICE_SAP)
				{
					cout << "SAP: ";
				}
				else if (service->GetType() == AOIP_SERVICE_NMOS)
				{
					cout << "NMOS: ";
				}

				cout << service->GetName() << endl;
			}
			cout << endl << endl;
		}
		return(0);
	}

	catch(runtime_error& e)
	{
		cout << "*** Runtime Error Exception ***" << endl;
	  	cout << e.what() << "\n";
	  	exit(-1);
	}
}
