/************************************************************************
 * dlb_st2110
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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

void newServiceCallBack(AoipService &newService)
{
	cout << "New Service" << endl << "=== =======" << endl << newService;
}


void PrintUsage(void)
{
	fprintf(stderr, "dlb_aoip_discovery_main <INTERFACE> v%s\n", VERSION);
	fprintf(stderr, "Copyright Dolby Laboratories Inc., 2021. All rights reserved.\n\n");
	fprintf(stderr, "<INTERFACE>               Interface to listen for SAP and use for mDNS\n");
}

int main(int argc, char *argv[])
{
	AoipSystem system;

	if (argc > 2)
	{
		PrintUsage();
		exit(-1);
	}

	if (argc == 2)
	{
		system.interface = argv[1];
	}
	else
	{
		system.interface = "enp3s0f0";
		printf("Using enp3s0f0\n");
	}

	system.name = "dlb_discovery";

	MClock::CheckTaiOffset();

	system.samplingFrequency = 48000;
	try
	{
		AoipServices aoipService(system, newServiceCallBack);
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
