/************************************************************************
 * dlb_st2110
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#include <rivermax_api.h>

#include "dlb_st2110_hardware.h"
#include "dlb_st2110_logging.h"

#define RMAX_CPUELT(_cpu)  ((_cpu) / RMAX_NCPUBITS)
#define RMAX_CPUMASK(_cpu) ((rmax_cpu_mask_t) 1 << ((_cpu) % RMAX_NCPUBITS))
#define RMAX_CPU_SET(_cpu, _cpusetp) \
    do { \
        size_t _cpu2 = (_cpu); \
        if (_cpu2 < (8 * sizeof (rmax_cpu_set_t))) { \
            (((rmax_cpu_mask_t *)((_cpusetp)->rmax_bits))[RMAX_CPUELT(_cpu2)] |= \
                                      RMAX_CPUMASK(_cpu2)); \
        } \
    } while (0)


static std::string exec(std::string cmd)
    {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string PmcGet(const char *variable, const char *key)
{
    char key_with_ws[256];

    strncpy(key_with_ws,"\t", 256);
    strncat(key_with_ws, key, 256);
    strncat(key_with_ws, " ", 256);
    std::string cfgText = exec(std::string("sudo pmc -u -b 0 'GET ") + variable + std::string("'"));
    if (cfgText.length() == 0)
    {
        throw std::runtime_error("PmcGet Failed");
    }
    std::string::size_type start = cfgText.rfind(key_with_ws) + strlen(key_with_ws);
    std::string::size_type end = cfgText.find("\n",start);
    std::string result = cfgText.substr(start,end-start);
    // remove all white space
    result.erase(remove_if(result.begin(),result.end(), isspace), result.end());
    return(result);
}

ST2110Hardware::ST2110Hardware(std::string localInterface, unsigned int fs)
{
    std::string tmpStr;

    int fd;
    struct ifreq ifr;
    struct timespec time;


    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, localInterface.c_str(), IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    if ((((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr).s_addr == 0)
    {
        throw std::runtime_error("Invalid Interface Name - Check interface is connected");
    }
    srcIpStr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

    /* display result */
    CLOG(INFO, HARDWARE_LOG) << "Local IP Address Selected: " << srcIpStr;
    interface = localInterface;
    CLOG(INFO, HARDWARE_LOG) << "Interface Selected: " << interface;
    tmpStr = PmcGet("DOMAIN", "domainNumber");
    domain = std::stoi(tmpStr, NULL, 10);
    CLOG(INFO, HARDWARE_LOG) << "Using PTP Domain: " << domain;
    tmpStr = PmcGet("TIME_STATUS_NP", "gmPresent");
    if (!tmpStr.compare("true"))
    {
        synched = true;
        CLOG(INFO, HARDWARE_LOG) << "PTP Grandmaster is present";
    }
    else
    {
        synched = false;
        CLOG(INFO, HARDWARE_LOG) << "PTP Grandmaster is not present";
    }
    gmIdentity = PmcGet("TIME_STATUS_NP", "gmIdentity");
    // remove punctuation
    gmIdentity.erase(remove_if(gmIdentity.begin(), gmIdentity.end(), ispunct), gmIdentity.end());
    // Insert hyphens and 
    if (gmIdentity.size() != 16)
    {
        throw std::runtime_error("Grandmaster identity badly configured");
    }
    gmIdentity.insert(2,"-");
    gmIdentity.insert(5,"-");
    gmIdentity.insert(8,"-");
    gmIdentity.insert(11,"-");
    gmIdentity.insert(14,"-");
    gmIdentity.insert(17,"-");
    gmIdentity.insert(20,"-");
    CLOG(INFO, HARDWARE_LOG) << "Grandmaster Identity: " << gmIdentity;
    samplingFrequency = fs;
    CLOG(INFO, HARDWARE_LOG) << "Sampling Frequency: " << samplingFrequency;
    // Initialize Rivermax
    RiverMaxInit();

    // Seed random number generator used for ssrc etc.
    clock_gettime(CLOCK_MONOTONIC, &time);
    srand((unsigned int)time.tv_nsec & 0xffffffff);
    CLOG(INFO, HARDWARE_LOG) << "RiverMax Initialized, Random Number Generator Seeded, Hardware Initialization complete";
}

ST2110Hardware::~ST2110Hardware(void)
{
    rmax_status_t rmaxStatus;
    struct timespec sleepTime;
    unsigned int timeOut = 100;

    // sleep time is 10ms
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 10000000;
    rmaxStatus = rmax_cleanup();
    // Wait for a certain length of time for system to not be busy
    // If stuck then timeout with error
    while((rmaxStatus == RMAX_ERR_BUSY) && timeOut > 0)
    {
        clock_nanosleep(CLOCK_MONOTONIC, 0, &sleepTime, NULL);
        rmaxStatus = rmax_cleanup();
        timeOut--;
    }
    if (timeOut == 0)
    {
        CLOG(WARNING, HARDWARE_LOG) << "rmax_cleanup timed out";
    }
    else
    {
        CLOG(INFO, HARDWARE_LOG) << "rmax_cleanup complete";
    }
    RmaxError("rmax_cleanup", rmaxStatus);
}


void ST2110Hardware::RiverMaxInit(void)
{

    // Initialize Rivermax
    rmax_init_config init_config;
    int internal_thread_core = 1;


    memset(&init_config, 0, sizeof(init_config));

    if (internal_thread_core > 0) {
        RMAX_CPU_SET(internal_thread_core, &init_config.cpu_mask);
        init_config.flags |= RIVERMAX_CPU_MASK;
    } else {
        std::cout << "Warning - no CPU affinity set!!!\n";
    }

    rmax_status_t status = rmax_init(&init_config);
    RmaxError("rmax_init", status);
}

rmax_status_t rmaxErrorCodes[NUM_RMAX_ERR_CODES] = 
{   RMAX_ERR_NO_HW_RESOURCES,                
    RMAX_ERR_NO_FREE_CHUNK,
    RMAX_ERR_NO_CHUNK_TO_SEND,
    RMAX_ERR_HW_SEND_QUEUE_FULL,
    RMAX_ERR_NO_MEMORY,
    RMAX_ERR_NOT_INITIALAZED,
    RMAX_ERR_NOT_IMPLEMENTED, 
    RMAX_ERR_NO_DEVICE,
    RMAX_ERR_BUSY,
    RMAX_ERR_CANCELLED,
    RMAX_ERR_HW_COMPLETION_ISSUE,
    RMAX_ERR_LICENSE_ISSUE,
    RMAX_ERR_UNKNOWN_ISSUE,
    RMAX_ERR_NO_ATTACH,
    RMAX_ERR_STEERING_ISSUE,
    RMAX_ERR_METHOD_NOT_SUPPORTED_BY_STREAM,
    RMAX_ERR_CHECKSUM_ISSUE,
    RMAX_ERR_DESTINATION_UNREACHABLE,
    RMAX_ERR_MEMORY_REGISTRATION,
    RMAX_ERR_NO_DEPENDENCY,
    RMAX_ERR_EXCEEDS_LIMIT,
    RMAX_ERR_UNSUPPORTED,
    RMAX_INVALID_PARAMETER_MIX
};

const char rmaxErrorMessages[NUM_RMAX_ERR_CODES][RMAX_ERR_MSG_LEN] = 
{
    "NO HW RESOURCES",                
    "NO FREE CHUNK",
    "NO CHUNK TO SEND",
    "HW SEND QUEUE FULL",
    "NO MEMORY",
    "NOT INITIALAZED",
    "NOT IMPLEMENTED", 
    "NO DEVICE",
    "BUSY",
    "CANCELLED",
    "HW COMPLETION ISSUE",
    "LICENSE ISSUE",
    "UNKNOWN ISSUE",
    "NO ATTACH",
    "STEERING ISSUE",
    "METHOD NOT SUPPORTED BY STREAM",
    "CHECKSUM ISSUE",
    "DESTINATION UNREACHABLE",
    "MEMORY REGISTRATION",
    "NO DEPENDENCY",
    "EXCEEDS LIMIT",
    "UNSUPPORTED",
    "RMAX INVALID PARAMETER MIX"
};

void ST2110Hardware::RmaxError(const char *msg, rmax_status_t error)
{
    const unsigned int maxErrorMsgSize = 256;
    char errorMsg[maxErrorMsgSize];
    unsigned int i;


    if ((error == RMAX_OK) || (error == RMAX_SIGNAL))
    {
        return;
    }
    snprintf(errorMsg, maxErrorMsgSize, "Uknown Error Message #%u", error);
    if ((error >= RMAX_ERR_INVALID_PARAM_1) && (error <= RMAX_ERR_INVALID_PARAM_10))
    {
        snprintf(errorMsg, maxErrorMsgSize, "Error: %s, Invalid Parameter %u\n", msg, error - RMAX_ERR_INVALID_PARAM_1 + 1);
    }
    else
    {
        for (i = 0 ; i < NUM_RMAX_ERR_CODES ; i++)
        {
            if (error == rmaxErrorCodes[i])
            {
                snprintf(errorMsg, maxErrorMsgSize, "Error: %s, %s\n", msg, rmaxErrorMessages[i]);
            }
        }
    }
    throw(std::runtime_error(errorMsg));
}
