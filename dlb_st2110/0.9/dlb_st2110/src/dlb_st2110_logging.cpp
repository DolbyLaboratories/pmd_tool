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

