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

#include <rivermax_api.h>

#include "dlb_st2110_hardware.h"
#include "dlb_st2110_logging.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <sys/timex.h>
#include <regex>


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

bool ST2110Hardware::PmcGet(const std::string variable, const std::string key, std::string &result)
{
    std::string cfgText = exec(std::string("pmc -u -b 0 -d ") + std::to_string(domain) + std::string(" 'GET ") + variable + std::string("'"));
    if (cfgText.length() == 0)
    {
        return false;
    }
    std::regex r("\\b" + key + "\\b"); // the pattern \b matches a word boundary
    std::smatch m;

    std::regex_search(cfgText, m, r);
    if (m.size() != 1)
    {
        return false;
    }
    auto keyEnd = m[0].second - cfgText.begin() + 1;
    auto start = cfgText.find_first_not_of(" \t", keyEnd);
    std::string::size_type end = cfgText.find_first_of("\n \t",start);
    if (end == std::string::npos)
    {
        end = cfgText.size();
    }
    result = cfgText.substr(start,end-start);
    // remove all white space
    result.erase(remove_if(result.begin(),result.end(), isspace), result.end());
    if (result.length() == 0)
    {
        return false;
    }
    return true;
}

void ST2110Hardware::CheckTaiOffset(void)
{
    unsigned int offset, offsetFromPmc;
    std::string tmpStr;

    if (!PmcGet("TIME_PROPERTIES_DATA_SET", "currentUtcOffset", tmpStr) ||
        (tmpStr.find_first_not_of("0123456789") != std::string::npos))
    {
        throw(std::runtime_error("Failed getting TIME_PROPERTIES_DATA_SET from PTP client"));
    }
    offsetFromPmc = std::stoi(tmpStr, NULL, 10);

    offset = get_tai_offset();
    if (offset != offsetFromPmc)
    {
        CLOG(WARNING, HARDWARE_LOG) << "UTC Offset wrong in kernal, was " << offset << ", setting it to " << offsetFromPmc << " from PMC" << std::endl;
        set_tai_offset(offsetFromPmc);
        offset = get_tai_offset();
        CLOG(INFO, HARDWARE_LOG) << "New UTC-TAI offset in kernel: " << offset << std::endl;
    }
    else
    {
        CLOG(INFO, HARDWARE_LOG) << "UTC Offset correctly set in kernal as " << offset << " seconds" << std::endl;
    }
}

unsigned int ST2110Hardware::get_tai_offset(void)
{
    struct timex t = { 0 };

    if (adjtimex(&t) == -1)
    {
        throw std::runtime_error("adjtimex failed");
    }
    return t.tai;
}

void ST2110Hardware::set_tai_offset(unsigned int offset)
{
    struct timex t = { 0 };

    t.modes = ADJ_TAI;
    t.constant = offset;
    if (adjtimex(&t) == -1)
    {
        throw std::runtime_error("adjtimex failed");
    }
}



ST2110Hardware::ST2110Hardware(unsigned int newDomain): domain(newDomain)
{
    struct timespec time;
    std::string tmpStr;

    // Initialize Rivermax
    RiverMaxInit();

    // Seed random number generator used for ssrc etc.
    clock_gettime(CLOCK_MONOTONIC, &time);
    srand((unsigned int)time.tv_nsec & 0xffffffff);
    CLOG(INFO, HARDWARE_LOG) << "RiverMax Initialized, Random Number Generator Seeded, Hardware Initialization complete";
    
    CLOG(INFO, HARDWARE_LOG) << "Using PTP Domain: " << domain;

    if ((!PmcGet("DOMAIN", "domainNumber", tmpStr)) ||
        (tmpStr.find_first_not_of("0123456789") != std::string::npos))
    {
        throw std::runtime_error("PTP Domain error - Check configuration");
    }
    const unsigned int domainCheck = std::stoi(tmpStr);
    if (domainCheck != domain)
    {
        throw std::runtime_error("PTP Domain error - Check configuration");
    }

    if (!PmcGet("TIME_STATUS_NP", "gmPresent", tmpStr))
    {
        throw std::runtime_error("PTP gmPresent error - Check configuration");
    }
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
    if (!PmcGet("TIME_STATUS_NP", "gmIdentity", tmpStr))
    {
        throw std::runtime_error("Error getting PTP GM identity - Check configuration");
    }

    // remove punctuation
    tmpStr.erase(remove_if(tmpStr.begin(), tmpStr.end(), ispunct), tmpStr.end());
    // Insert hyphens and 
    if (tmpStr.size() != 16)
    {
        throw std::runtime_error("Grandmaster identity badly configured");
    }
    tmpStr.insert(2,"-");
    tmpStr.insert(5,"-");
    tmpStr.insert(8,"-");
    tmpStr.insert(11,"-");
    tmpStr.insert(14,"-");
    tmpStr.insert(17,"-");
    tmpStr.insert(20,"-");
    gmIdentity = tmpStr;
    CLOG(INFO, HARDWARE_LOG) << "Grandmaster Identity: " << gmIdentity;

    CheckTaiOffset();
}

void ST2110Hardware::GetPTPInfo(std::string &gm, unsigned int &dom, bool &sync)
{
    gm = gmIdentity;
    dom = domain;
    sync = synched;
}

bool GetMacAddrByteArray(std::string interfaceName, MacAddrByteArray &byteArray)
{
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    strcpy(s.ifr_name, interfaceName.c_str());
    if (0 == ioctl(fd, SIOCGIFHWADDR, &s))
    {
        for (int i = 0; i < 6; ++i)
        {
          byteArray[i] = s.ifr_addr.sa_data[i];
        }
        return true;
    }
    else
    {
        return false;        
    }
}

std::string ST2110Hardware::GetIpStrFromInterface(std::string interfaceName)
{
    std::string ipAddr;
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    if ((((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr).s_addr == 0)
    {
        throw std::runtime_error("Invalid Interface Name - Check interface is connected");
    }
    ipAddr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    CLOG(INFO, HARDWARE_LOG) << "IP address for " << interfaceName << " is " << ipAddr;
    return ipAddr;
}

uint32_t ST2110Hardware::GetHostIpIntFromInterface(std::string interfaceName)
{
    return(ntohl(inet_addr(GetIpStrFromInterface(interfaceName).c_str())));
}

uint32_t ST2110Hardware::GetNetIpIntFromInterface(std::string interfaceName)
{
    return(inet_addr(GetIpStrFromInterface(interfaceName).c_str()));
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
    GetErrorMsg("rmax_cleanup", rmaxStatus);
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
    GetErrorMsg("rmax_init", status);
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

void ST2110Hardware::GetErrorMsg(const char *msg, rmax_status_t error)
{
    const unsigned int maxErrorMsgSize = 256 + RMAX_ERR_MSG_LEN;
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
                const char *rmaxMsg = rmaxErrorMessages[i]; // suppresses truncation warning
                snprintf(errorMsg, maxErrorMsgSize, "Error: %s, %s\n", msg, rmaxMsg);
            }
        }
    }
    throw(std::runtime_error(errorMsg));
}
