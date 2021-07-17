/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/


#ifndef DLB_ST2110_LOGGING_H_
#define DLB_ST2110_LOGGING_H_

#include <ostream>

class NullStream : public std::ostream
{
public:
  NullStream() : std::ostream(nullptr) {}
  NullStream(const NullStream &) : std::ostream(nullptr) {}
};

template <class T>
const NullStream &operator<<(NullStream &&os, const T &value)
{ 
  return os;
}

// This is required to suppress loggin as well as enable it so always used
void SuppressLogging(void);
void InitLogging(int argc, char *argv[], const std::string filename);

#ifdef LOGGING

#include "easylogging++.h"

#define RAV_LOG "Ravenna"
#define SAP_LOG "Sap"
#define TRANS_LOG "Transmitter"
#define RECEIVE_LOG "Receiver"
#define HARDWARE_LOG "Hardware"
#define SERVICES_LOG "Services"

class ELCustomLoggers
{
  el::Logger* ravennaLogger;
  el::Logger* sapLogger;
  el::Logger* transmitterLogger;
  el::Logger* receiverLogger;
  el::Logger* hardwareLogger;
  el::Logger* servicesLogger;


public:
  ELCustomLoggers()
  {
      ravennaLogger = el::Loggers::getLogger(RAV_LOG);
      sapLogger = el::Loggers::getLogger(SAP_LOG);
      transmitterLogger = el::Loggers::getLogger(TRANS_LOG);
      receiverLogger = el::Loggers::getLogger(RECEIVE_LOG);
      hardwareLogger = el::Loggers::getLogger(HARDWARE_LOG);
      servicesLogger = el::Loggers::getLogger(SERVICES_LOG);
  }

  ~ELCustomLoggers()
  {
      el::Loggers::unregisterLogger(RAV_LOG);
      el::Loggers::unregisterLogger(SAP_LOG);
      el::Loggers::unregisterLogger(TRANS_LOG);
      el::Loggers::unregisterLogger(RECEIVE_LOG);
      el::Loggers::unregisterLogger(HARDWARE_LOG);
      el::Loggers::unregisterLogger(SERVICES_LOG);
  }
};

#else

extern NullStream nullStream;

#define	START_EASYLOGGINGPP(argc, argv)
#define INITIALIZE_EASYLOGGINGPP

#define LOG(LEVEL) nullStream
#define CLOG(a,b) nullStream
#define CVLOG(a,b) nullStream
#define CLOG_IF(a,b,c) nullStream
#define CVLOG_IF(a,b,c) nullStream
#define CLOG_EVERY_N(a,b,c) nullStream
#define CVLOG_EVERY_N(a,b,c) nullStream
#define CVLOG_AFTER_N(a,b,c) nullStream
#define CVLOG_N_TIMES(a,b,c) nullStream
#define VLOG(vlevel) nullStream
#define LOG_IF(condition, LEVEL) nullStream
#define VLOG_IF(condition, vlevel) nullStream
#define LOG_EVERY_N(n, LEVEL) nullStream
#define VLOG_EVERY_N(n, vlevel) nullStream
#define LOG_AFTER_N(n, LEVEL) nullStream
#define VLOG_AFTER_N(n, vlevel) nullStream
#define LOG_N_TIMES(n, LEVEL) nullStream
#define VLOG_N_TIMES(n, vlevel) nullStream

typedef int ELCustomLoggers;

#endif // LOGGING

#endif // DLB_ST2110_LOGGING_H_