/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby International AB.
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

#ifndef EMBER_CONTROLLER_H
#define EMBER_CONTROLLER_H
#include "ember_connector.h"
#include "ember_definitions.h"
#include "dlb_pmd_api.h"
#include <string>

namespace dolby { namespace dlb_pmd { namespace ember {

typedef std::shared_ptr<EmberConnector> EmberConnectorPtr;

/**
 * @brief Implementation of the Lawo device controller for usage with the Lawo proprietary interface
 */
class EmberController : EmberConnector::EmberConnectorListener
{
public:

    //! Object representing the actual connection to Lawo
    EmberConnector mEmberConnector;

    /**
     * @brief Standard constructor
     */
    EmberController();
    /**
     * @brief Destructor
     */
   ~EmberController();
    
    /**
     * @see EmberControllerInterface::init()
     */
    dlb_pmd_success init();
    
    /**
     * @see EmberControllerInterface::connect()
     */
    bool connect(const std::string &host, int port);
    
    /**
     * @see EmberControllerInterface::disconnect()
     */
    bool disconnect(bool hasError = false, const EmberConnectErrorInfo error = EMBER_SOCK_ERR_NONE);
    
    /**
     * @see EmberControllerInterface::getConnectionStatus()
     */  
    EmberConnectionStatus getConnectionStatus() const;

    /**
     * @see PollableInterface::poll()
     */
    dlb_pmd_success poll(unsigned int timeoutMsec);
    
    /**
     * Unused as AvatusMDConnector doesn't issue keep alive messages.
     * @see EmberConnectorListener::onKeepAliveReceived()
     */
    void onKeepAliveReceived();

    /**
     * Callback from EmberConnector when new parameter is received from
     * Ember+ provider.
     * @see EmberConnectorListener::onParameter
     */
    void onParameter(const std::string id, const GlowParameter *param);

    /**
     * @see EmberConnectorListener::onParameterRegistered()
     */
    virtual void onParameterRegistered(std::string identifier);
    
    /**
     * @brief Changes the current Ember device status
     *
     * @param newStatus New connection status
     * @param hasError Is the error code present
     * @param error New error information
     */
    virtual void changeStatus(const EmberConnectionState newStatus, bool hasError = false,
                      const EmberConnectErrorInfo error = EMBER_SOCK_ERR_NONE);

private:
    /**
     * Callback from EmberConnector when polling during connection
     * @see EmberConnectorListener::onConnectionInProgress()
     */
    void onConnectionInProgress();

    /**
     * @brief Performs necessary poll actions in the connecting state
     */
    void pollWhileConnecting();
    /**
     * @brief Performs necessary poll actions in the connecting state
     */
    void pollWhileConnected();
    // /**
    //  * @brief Send to Ember+ server parameter to be modified
    //  * @param identifier of the parameter to be set
    //  * @param value to be set
    //  */
    // void sendParameter(const EmberParam param);

    //! Timeout period while refreshing registered parameters info
    static const unsigned long long ConnectionResponseTimeout;
    //! Timeout period from mantaining connection to getting response about registering to all ember+ parameters
    static const unsigned long long EmberRegistrationTimeout;
    //! Maximum number of trials to connect on given port before client decides that connecting on this port is
    //! not possible (case when host exists but refuses connection on Ember+ port)
    static const unsigned int SocketResponseLimit;
    //! Distance in time between forced reconnection trials, in case if connection was lost
    static const unsigned long long ReconnectionTimeDistance;

    //! The last time the keep alive method was invoked
    unsigned long long mLastKeepAliveTimeStamp;
    //! The time when we started waiting for connection response
    unsigned long long mConnectionResponseTimestamp;
    //! The time when we started for ember+ registration response
    unsigned long long mEmberRegistrationTimestamp;
    //! The time when last reconnection trial finished with failure or connection was lost
    unsigned long long mReconnectionTimestamp;
    //! Amount of times, when we tried to connect on given port but we haven't
    unsigned int mSocketResponseCounter;

    //! Current connection status
    EmberConnectionStatus mConnectionStatus;
    //! The sub-state of the connection process - the socket is connected
    bool mSocketConnected;

    //! Counter of registered ember+ parameters
    int mRegistrationCounter;
};

}}}

#endif // EMBER_CONTROLLER_H
