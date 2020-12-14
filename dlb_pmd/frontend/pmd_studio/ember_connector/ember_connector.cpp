/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#include "parameter_manager.h"
#include "ember_connector.h"

extern "C"{
#include "emberplus.h"
#include "glow.h"
}
#include "dlb_pmd_api.h"

#ifndef _WIN32
  #include <sys/socket.h>
  #include <arpa/inet.h>
#else
  #include <WinSock>
#endif

#include <netdb.h>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <cassert>


using namespace dolby::dlb_pmd::ember;

EmberConnector::EmberConnector(EmberConnectorListener& listener):
    mEmberManager(new ParameterManager(*this))
    , mEmberInitialized{false}
    , mSocket(INVALID_SOCKET)
    , mGlowReader(NULL)
    , mListener(listener)
    , mRegistrationStarted(false)
{
    memset(&mEmberAddress, 0, sizeof(mEmberAddress));
    assert(mEmberManager.get() != NULL);
}

EmberConnector::~EmberConnector()
{
    glowReader_free(mGlowReader);
    delete mGlowReader;
    performDisconnection();
}

void EmberConnector::onParameterRegistered(std::string identifier)
{
    mListener.onParameterRegistered(identifier);
}

dlb_pmd_success EmberConnector::init()
{
    if(mEmberInitialized){
        return PMD_SUCCESS;
    }
    mEmberInitialized = true;
    ember_init(EmberConnector::onThrowError, EmberConnector::onFailAssertion, malloc, free);
    if (!mEmberInitialized)
    {
        fprintf(stderr, "Failed to initialize Ember+ Library!\n");
        return PMD_FAIL;
    }

    mGlowReader = new GlowReader();

    glowReader_init(mGlowReader,
                    EmberConnector::onNode,
                    EmberConnector::onParameter,
                    EmberConnector::onCommand,
                    EmberConnector::onStreamEntry,
                    (void*)this,
                    mGlowBuffer,
                    MAX_PACKAGE_SIZE);

    return (EmberConnector::mEmberInitialized ? PMD_SUCCESS : PMD_FAIL);
}

bool EmberConnector::performDisconnection()
{
    memset(&mEmberAddress, 0, sizeof(mEmberAddress));
    /* We also need to unregister all registered parameters, as they may differ within next connection */

    mEmberManager->clear();
    mRegistrationStarted = false;
    if (mSocket >= 0)
    {
        close(mSocket);
        mSocket = INVALID_SOCKET;
        return true;
    }
    return false;
}

bool EmberConnector::performSetupSocket(const std::string& host, int port, EmberConnectErrorInfo &errInfo)
{
    // Convert host to IP
    memset(&mEmberAddress, 0, sizeof(mEmberAddress));
    mEmberAddress.sin_family = AF_INET;
    mEmberAddress.sin_port = htons(port);

    if (hostToAddress(host, mEmberAddress.sin_addr) == false)
    {
        fprintf(stderr, "[ER] EmberConnector - Host not found.\n");
        errInfo = EMBER_SOCK_ERR_HOST_NOT_FOUND;
        return false;
    }

    // Create socket
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket < 0)
    {
        fprintf(stderr, "[ER] EmberConnector - Socket creation failed.\n");
        errInfo = EMBER_SOCK_ERR_SOCK_CREATION_FAIL;
        return false;
    }

    // Set timeout for blocking receive operations (1 sec)
    struct timeval tv{1, 0};
    if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0)
    {
        fprintf(stderr, "[ER] EmberConnector - Socket timed out.\n");
        errInfo = EMBER_SOCK_ERR_TIMEOUT;
        return false;
    }

    // Set non-blocking mode for connect
    if (fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL) | O_NONBLOCK) != 0)
    {
        fprintf(stderr, "[ER] EmberConnector - Unable to set blocking mode.\n");
        errInfo = EMBER_SOCK_ERR_CONFIG_FAIL;
        return false;
    }

    errInfo = EMBER_SOCK_ERR_NONE;
    return true;
}

bool EmberConnector::performConnectionRequest(bool &connected, EmberConnectErrorInfo &errInfo)
{
    if (connect(mSocket, (struct sockaddr*)&mEmberAddress, sizeof(mEmberAddress)))
    {
        switch (errno)
        {
        case EINPROGRESS:
            break;
        case EALREADY:
            mListener.onConnectionInProgress();
            break;
        case EISCONN:
            // Bring back blocked IO and wait for confirmation
            if (fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL) & (~O_NONBLOCK)) != 0)
            {
                fprintf(stderr, "[ER] %s Socket configuration (blocking) failed: %s\n", __func__, strerror(errno));
                errInfo = EMBER_SOCK_ERR_CONFIG_FAIL;
                return false;
            }
            connected = true;
            break;
        default:
            // _logger->Error("%s Socket connection failed: %s", __func__, strerror(errno));

            // Status: disconnected
            // errInfo.set_error_code(GPIO_ERROR_CODE_CONNECTION_FAILED);
            fprintf(stderr, "[ER] EmberConnector - Socket connection failed.");
            errInfo = EMBER_SOCK_ERR_CONNECTION_FAIL;
            return false;
        }
    }
    errInfo = EMBER_SOCK_ERR_NONE;
    return true;
}

dlb_pmd_success EmberConnector::sendToClient(unsigned char* buffer, int bufferSize)
{
    dlb_pmd_success result = PMD_SUCCESS;
    if (mSocket != INVALID_SOCKET)
    {
        if (send(mSocket, buffer, bufferSize, 0) != bufferSize)
        {
            result = PMD_FAIL;
        }
    }
    return result;
}

bool EmberConnector::performReadSocket()
{
    while (1)
    {
        unsigned char buffer[MAX_PACKAGE_SIZE];

        int bytesAvailable = recv(mSocket, (char*)buffer, sizeof(buffer), MSG_DONTWAIT);

        if (bytesAvailable < 0)
        {
            // Lack of data is not an error
            return ((errno == EAGAIN) || (errno == EWOULDBLOCK));
        }

        if (bytesAvailable == 0)
        {
            // Socket closed
            return false;
        }

        // Show data
        if (bytesAvailable > 0)
        {
            glowReader_readBytes(mGlowReader, buffer, bytesAvailable);
            return true;
        }
    }
    // _logger->Error("%s reading socket failed - should never happen!", __func__);
    // Should never get here but just in case - a fancy error :)
    errno = EDEADLK;
    return false;
}

bool EmberConnector::convertIpAddress(const std::string & ip, struct in_addr & address)
{
    int result = inet_pton(AF_INET, ip.c_str(), &address);
    return result != 0;
}

bool EmberConnector::hostToAddress(const std::string & host, struct in_addr & address)
{
    // Check if host is IP
    if (convertIpAddress(host, address))
    {
        return true;
    }

    // Resolve host
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), NULL, &hints, &servinfo) != 0)
    {
        return false;
    }

    address = ((struct sockaddr_in *)servinfo->ai_addr)->sin_addr;

    freeaddrinfo(servinfo);

    return true;
}

void EmberConnector::onThrowError(int error, pcstr pMessage)
{
    // mEmberInitialized = false;
    std::cerr << "EmberConnector - Throw error " << error << ": " << pMessage << std::endl;
}

void EmberConnector::onFailAssertion(pcstr pFileName, int lineNumber)
{
    // mEmberInitialized = false;
    std::cerr << "EmberConnector - Fail assertion @ '" << pFileName << "' line " << lineNumber << std::endl;
}

void EmberConnector::onParameter(const GlowParameter *pParameter, GlowFieldFlags fields, \
                                 const berint *path, int pathLength, voidptr state)
{
    reinterpret_cast<EmberConnector*>(state)->onParameter(pParameter, fields, path, pathLength);
}

void EmberConnector::onParameter(const GlowParameter *parameter, GlowFieldFlags fields,
                                 const berint *path, int pathLength)
{
    // mListener.onKeepAliveReceived();
    if(fields & GlowFieldFlag_Identifier){
        if(!mEmberManager->isRegisteredParameter(path,pathLength)){
            // Incase we're in the process of registering new parameter:
            if(!mEmberManager->registerParameter(parameter, path, pathLength)){
                // Parameter unwanted. Ignore.
                return;
            }
            else{
                // Show interest in the parameter to the provider by issuing a Get request.
                sendGetDirectoryCommand(path, pathLength, GlowElementType_Parameter);
            }
        }
    }
    else if(fields & GlowFieldFlag_Value){
        std::string param_id = generateIdentifierFromGlowPath(path, pathLength);
        ParameterManager::ParameterDescriptor p = mEmberManager->getParamDescriptor(param_id);
        if(!mEmberManager->isRegisteredParameter(param_id)){
            fprintf(stderr, "EmberConnector - Error: Received value from unregistered parameter\n");
            return;
        }
        mListener.onParameter(param_id, parameter);
    }
}

void EmberConnector::onCommand(const GlowCommand *pCommand, const berint *path, int pathLength, voidptr state)
{
    reinterpret_cast<EmberConnector*>(state)->onCommand(pCommand, path, pathLength);
}

void EmberConnector::onCommand(const GlowCommand *pCommand, const berint *path, int pathLength)
{
    std::cout << "Ember Connector - Warning: Unhandled GlowCommand: " << pCommand->number << std::endl;
}

void EmberConnector::onPackageReceived(const byte *pPackage, int length, voidptr state)
{
    reinterpret_cast<EmberConnector*>(state)->onPackageReceived(pPackage, length);
}

void EmberConnector::onPackageReceived(const byte *pPackage, int length)
{
    unsigned char buffer[EMBER_BUFFER_LENGTH];
    unsigned int txLength;

    if(length >= EMBER_PACKAGE_LENGTH)
    {
        if (pPackage[EMBER_PACKAGE_ID_MESSAGE] == EMBER_MESSAGE_ID
         && pPackage[EMBER_PACKAGE_ID_TYPE] == EMBER_COMMAND_KEEPALIVE_REQUEST)
        {
            txLength = emberFraming_writeKeepAliveResponse(buffer, sizeof(buffer), pPackage[0]);
            sendToClient(buffer, txLength);
        }
    }
    else
    {
        // _logger->Debug("%s Unknown package received: length: %d, package id: %d, command: %d", __func__, length,
        //                pPackage[1], pPackage[2]);
    }
}

void EmberConnector::onStreamEntry(const GlowStreamEntry *pStreamEntry, voidptr state)
{
    reinterpret_cast<EmberConnector*>(state)->onStreamEntry(pStreamEntry);
}

void EmberConnector::onStreamEntry(const GlowStreamEntry *pStreamEntry)
{
    // mListener.onKeepAliveReceived();
    // _logger->Error("%s: stream received from Ember+ server! - not implemented", __func__);
}

void EmberConnector::onNode(const GlowNode *node, GlowFieldFlags fields, const berint *path, int pathLength,
                            voidptr state)
{
    reinterpret_cast<EmberConnector*>(state)->onNode(node, fields, path, pathLength);
}

void EmberConnector::onNode(const GlowNode *node, GlowFieldFlags fields, const berint *path, int pathLength)
{
    /* We are checking if incoming node identifier is on current path descriptor identifiers list
     * and if yes, we are sending GetDirectory command on it, to get access to it's children.
     * We need also send it for the last child on the list, to be registered on it's changes.
     */
    if (mEmberManager->isNodeDestinedToSend(node, fields, path, pathLength))
    {
        sendGetDirectoryCommand(path, pathLength);
    }
}

void EmberConnector::registerParameter(const std::string path[], int pathLength)
{
    mEmberManager->setCurrentParamDescriptor(path, pathLength);
    sendGetDirectoryCommand();
}

dlb_pmd_success EmberConnector::sendGetDirectoryCommand()
{
    dlb_pmd_success result = PMD_SUCCESS;
    berint path[GLOW_MAX_TREE_DEPTH];
    result = sendGetDirectoryCommand(path, 0);
    return result;
}

dlb_pmd_success EmberConnector::sendGetDirectoryCommand(const berint *path, int pathLength, GlowElementType type)
{
    unsigned char buffer[MAX_PACKAGE_SIZE];
    GlowCommand command;
    GlowOutput output;
    dlb_pmd_success result = PMD_SUCCESS;

    memset(&command, 0, sizeof(command));
    command.number = GlowCommandType_GetDirectory;

    glowOutput_init(&output, buffer, MAX_PACKAGE_SIZE, 0);
    glowOutput_beginPackage(&output, true);
    glow_writeQualifiedCommand(&output, &command, path, pathLength, type);

    result = sendToClient(buffer, glowOutput_finishPackage(&output));


    /* 
     * TODO: Wrap the following commented out code around some kind of 
     * debug-only ifdef.
     */

    // std::cout << "EmberConnector - ";
    // if(pathLength > 0){
    //     const std::string id = generateIdentifierFromGlowPath(path, pathLength);
    //     std::cout << "[" << id << "] ";
    // }   
    // std::cout << "Ember+ GetDirectory command ";
    // if(result == PMD_SUCCESS){
    //     std::cout << "sent successfully" << std::endl;
    // }
    // else{
    //     std::cout << "failed" << std::endl;
    // }
    
    return result;
}

