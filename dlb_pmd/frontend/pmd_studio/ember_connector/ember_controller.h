/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
