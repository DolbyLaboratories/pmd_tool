/************************************************************************
 * dlb_pmd
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

#include "ember_controller.h"
#include "ember_definitions.h"
#include "utils.h"
#include "dlb_pmd_api.h"
#include <string>
#include <iostream>

using namespace dolby::dlb_pmd::ember;

const unsigned long long    EmberController::ConnectionResponseTimeout = 10000; // 10 seconds
const unsigned long long    EmberController::EmberRegistrationTimeout = 10000; // 5 seconds
const unsigned int          EmberController::SocketResponseLimit = 200; // 200 responses should be enough
const unsigned long long    EmberController::ReconnectionTimeDistance = 30000; // 30 seconds

EmberController::EmberController():
    mEmberConnector{*this}
    , mLastKeepAliveTimeStamp(msTime())
    , mConnectionResponseTimestamp(msTime())
    , mEmberRegistrationTimestamp(0l)
    , mReconnectionTimestamp(msTime())
    , mSocketResponseCounter(0)
    , mConnectionStatus{EMBER_STATE_DISCONNECTED, EMBER_SOCK_ERR_NONE}
    , mSocketConnected(false)
    , mRegistrationCounter(0)
{
}

EmberController::~EmberController()
{
}

dlb_pmd_success EmberController::init()
{
    return mEmberConnector.init();
}

bool EmberController::connect(const std::string &host, int port)
{
    EmberConnectErrorInfo errInfo;
    if (getConnectionStatus().state != EMBER_STATE_DISCONNECTED)
    {
        // _logger->Error("%s Connection requested on an already connected/ing device.", __func__);
        return false;
    }

    // Status: connecting
    changeStatus(EMBER_STATE_CONNECTING);

    // Establish socket
    if (mEmberConnector.performSetupSocket(host, port, errInfo) == false)
    {
        changeStatus(EMBER_STATE_DISCONNECTED, true, errInfo);
    }

    mSocketConnected = false;

    return true;
}

bool EmberController::disconnect(bool hasError, EmberConnectErrorInfo error)
{
    if (getConnectionStatus().state == EMBER_STATE_DISCONNECTED)
    {
        std::cerr << "EmberController - Disconnection requested on an already disconnected device." << std::endl;
        return false;
    }

    mEmberConnector.performDisconnection();

    // Status: disconnected
    if (hasError)
    {
        changeStatus(EMBER_STATE_DISCONNECTED, hasError, error);
    }
    else
    {
        changeStatus(EMBER_STATE_DISCONNECTED);
    }

    mRegistrationCounter = 0;
    mSocketResponseCounter = 0;
    mEmberRegistrationTimestamp = 0l;

    return true;
}

EmberConnectionStatus EmberController::getConnectionStatus() const
{
    return this->mConnectionStatus;
}

/**
 * Abstracted method that handles polling during connecting and 
 * connected states.
 */
dlb_pmd_success EmberController::poll(unsigned int timeoutMsec)
{
    if (getConnectionStatus().state == EMBER_STATE_DISCONNECTED)
    {
        // TODO: Maybe add original functionality for reconnect?
        return PMD_FAIL;
    }

    else if (getConnectionStatus().state == EMBER_STATE_CONNECTING)
    {
        pollWhileConnecting();
        if (this->getConnectionStatus().errInfo){
            return PMD_FAIL;
        }
    }
    // We're already connected
    else if (getConnectionStatus().state == EMBER_STATE_CONNECTED)
    {
        pollWhileConnected();
        if (this->getConnectionStatus().errInfo){
            return PMD_FAIL;
        }
    }
    return PMD_SUCCESS;
}


// bool EmberController::checkRegistrationTimeout()
// {
//     unsigned long long currentTimeStamp = msTime();
//     bool timeouted = false;

//     /* If ember registration timestamp is 0, it means that we just estabilished connection
//        and we can start to wait for ember registration timeout */
//     if (mEmberRegistrationTimestamp == 0l) mEmberRegistrationTimestamp = msTime();

//     else if (currentTimeStamp - mEmberRegistrationTimestamp > EmberRegistrationTimeout)
//     {
//         EmberConnectErrorInfo errInfo;
//         std::cerr << "EmberController - Error: Registration timeout" << std::endl;
//         disconnect(true, EMBER_SOCK_ERR_SERVICE_ERR);
//         timeouted = true;
//     }
//     return timeouted;
// }

/**
 * Currently not used (as AvatusMDConnector doesn't issue keepalive messages)
 */
void EmberController::onKeepAliveReceived()
{
    EmberConnectionStatus status = getConnectionStatus();
    mLastKeepAliveTimeStamp = msTime();
    if (status.state == EMBER_STATE_DISCONNECTED)
    {
        /* This should never happen - if yes - let's force disconnecting to clean up the situation */
        disconnect();
    }
    else if (status.state == EMBER_STATE_CONNECTING)
    {
        // checkRegistrationTimeout();
    }
}

void EmberController::onParameterRegistered(std::string identifier)
{
    // TODO: Log parameter registered
    // std::cout << "EmberController: Registered identifier " << identifier << std::endl;
}

void EmberController::onConnectionInProgress()
{
    /* Every time when we get information from tcp/ip that connection is already in progress,
       we increase counter, to timeout after some time - as it may mean that connecting on selected port is
       impossible */
    ++mSocketResponseCounter;
    if(mSocketResponseCounter == 200){
        std::cerr << "EmberController - Socket connect response timeout" << std::endl;
        disconnect(true, EMBER_SOCK_ERR_TIMEOUT);
    }
}

void EmberController::pollWhileConnecting()
{
    unsigned long long currentTimeStamp = msTime();
    EmberConnectErrorInfo errInfo;

    /* Perform connection sequence */
    if (mSocketConnected == false)
    {
        // Connect
        if (!mEmberConnector.performConnectionRequest(mSocketConnected, errInfo))
        {
            changeStatus(EMBER_STATE_DISCONNECTED, true, errInfo);
        }
        else
        {
            if (mSocketConnected)
            {
                changeStatus(EMBER_STATE_CONNECTED);
                mConnectionResponseTimestamp = currentTimeStamp;
                mSocketResponseCounter = 0;
            }
        }
    }
    
    if(mSocketConnected)
    {
        /* let's trigger Ember+ setup (parameters registration) */
        // if (!mEmberConnector->performEmberSetup())
        // {
        //     disconnect(true, EMBER_SOCK_ERR_SERVICE_ERR);
        // }
        /* let's trigger reading from socket */
        if (!mEmberConnector.performReadSocket())
        {
            disconnect(true, EMBER_SOCK_ERR_CONNECTION_FAIL);
        }

        long long int diff = (currentTimeStamp > mConnectionResponseTimestamp) ?
             (currentTimeStamp - mConnectionResponseTimestamp) : (mConnectionResponseTimestamp - currentTimeStamp);
        if (diff > ConnectionResponseTimeout)
        {
            errInfo = EMBER_SOCK_ERR_TIMEOUT;
            disconnect(true, errInfo);
        }
    }
}

void EmberController::pollWhileConnected()
{
    // Read everything we got
    if (mEmberConnector.performReadSocket() == false)
    {
        /* it means that reading socket failed from some reasons and we should probably disconnect */
        std::cerr << "EmberController - read socket failed - disconnecting" << std::endl;
        disconnect(true, EMBER_SOCK_ERR_CONNECTION_LOST);
        return;
    }
    /* we don't interprete messages read from server - it is done by ember_connector, and ember_controller as
     * it's listener will be informed and should proceed with passing it to system model */
}

void EmberController::changeStatus(const EmberConnectionState newStatus, bool hasError,
                                   const EmberConnectErrorInfo error)
{
    const EmberConnectionStatus oldStatus = mConnectionStatus;
    (void)oldStatus;
    mConnectionStatus.state = newStatus;
    mConnectionStatus.errInfo = error;
}

void EmberController::onParameter(const std::string id, const GlowParameter *param){
    onKeepAliveReceived();
}
