/************************************************************************
 * dlb_st2110
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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