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


#define LOGGING


#include "dlb_st2110_logging.h"


INITIALIZE_EASYLOGGINGPP

ELCustomLoggers customLogs;


void SuppressLogging(void)
{
  setenv("RIVERMAX_LOG_LEVEL", "6", 1); // No Rivermax logging
  setenv("VMA_TRACELEVEL", "0", 1); // No VMA logging  
}

void InitLogging(int argc, char *argv[], const std::string filename)
{

  START_EASYLOGGINGPP(argc, argv);
    // Configure Logging
	el::Configurations defaultConf;
  defaultConf.setToDefault();
  defaultConf.set(el::Level::Global,
            el::ConfigurationType::PerformanceTracking, "true");  

  if (filename.size() > 0)
  {
    defaultConf.set(el::Level::Global,
            el::ConfigurationType::Filename, filename);
		defaultConf.set(el::Level::Global,
            el::ConfigurationType::ToFile, "true");
	}
	else
	{
		defaultConf.setGlobally(
	            el::ConfigurationType::ToFile, "false");
		defaultConf.set(el::Level::Global,
	            el::ConfigurationType::ToFile, "false");		
	}
  el::Loggers::reconfigureLogger("default", defaultConf);
  el::Loggers::reconfigureLogger(RAV_LOG, defaultConf);
  el::Loggers::reconfigureLogger(SAP_LOG, defaultConf);
  el::Loggers::reconfigureLogger(RECEIVE_LOG, defaultConf);
  el::Loggers::reconfigureLogger(HARDWARE_LOG, defaultConf);
  el::Loggers::reconfigureLogger(SERVICES_LOG, defaultConf);
  //defaultConf.set(el::Level::Global,
  //          el::ConfigurationType::Performance_Tracking, "true");  
  el::Loggers::reconfigureLogger(TRANS_LOG, defaultConf);

	LOG(INFO) << "Logging aoipSystem started";
}

// Used by null logging or logging that is switched off
NullStream nullStream;

