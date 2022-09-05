/************************************************************************
 * dlb_pmd
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
#ifndef EMBER_CONNECTOR_H
#define EMBER_CONNECTOR_H


#ifndef _WIN32
  #include <netinet/in.h>
  #include <sys/socket.h>
#else
  #include <unistd.h>
#endif
#include "parameter_manager.h"
#include <string>
#include "ember_definitions.h"

#include "dlb_pmd_api.h"

extern "C"{
#include "emberplus.h"
}

#include <memory>

namespace dolby { namespace dlb_pmd { namespace ember {

typedef std::shared_ptr<ParameterManager> ParameterManagerPtr;

/**
 * @brief The class implementing various operations on the Lawo ember+ device using its proprietary protocol
 */
class EmberConnector : dolby::dlb_pmd::ember::ParameterManager::EmberManagerListener
{
public:
    /**
     * @brief Class providing interface for listener to events dispatched by ember_connector
     */
    class EmberConnectorListener
    {
    public:
        /**
         * @brief Called after receiving parameter from EmberProvider.
         * @param id registration identifier of parameter
         * @param param Received GlowParameter
         */
        virtual void onParameter(const std::string id, const GlowParameter *param)=0;
    
        /**
         * @brief Called when parameter is succesfully registered
         * @param identifier of succesfully registered paramater
         */
        virtual void onParameterRegistered(std::string identifier) = 0;
        
        /**
         * @brief Called when cennecting operation is in progress (errno 144)
         */
        virtual void onConnectionInProgress() = 0;
    };

    /**
     * @brief Standard constructor
     */
    EmberConnector(EmberConnectorListener& listener);
    /**
     * @bried Destructor
     */
    ~EmberConnector();

    /**
     * @see ParameterManagerListener::onParameterRegistered
     */
    void onParameterRegistered(std::string identifier);

    /**
     * @brief Initalizes Ember+ connector
     * @return SUCCESS if initilized properly, ERROR if failed
     */
    dlb_pmd_success init();
    /**
     * @brief Performs setup of the connection socket. Looks up the host and creates the socket.
     *
     * @param host Host to connect to
     * @param port Port to connect to
     * @param errInfo Output structure for possible errors
     *
     * @return True if the operattion succeeded, False otherwise. False indicates invalid state of the connection.
     */
    bool performSetupSocket(const std::string& host, int port, EmberConnectErrorInfo &errInfo);
    /**
     * @brief Performs single attempt on connecting to a host. Asynchronous.
     *
     * @param connected The result. True if connection is fully estbalished.
     * @param errInfo Output structure for possible errors
     *
     * @return True if the operattion succeeded, False otherwise. False indicates invalid state of the connection.
     */
    bool performConnectionRequest(bool &connected, EmberConnectErrorInfo &errInfo);
    /**
     * @brief Reads all possible input on the socket and analyzes the content extracting the Lawo message.
     * Might block in case of network problems.
     *
     * @return True if the operattion succeeded, False otherwise. False indicates invalid state of the connection.
     */
    bool performReadSocket();
    /**
     * @brief Disconnects the socket fromt the host.
     *
     * @return Reserved for future use :)
     */
    bool performDisconnection();
    /**
     * @brief Setup Ember+ communication
     *
     * @return Reserved for future use :)
     */
    bool performEmberSetup();
    // /**
    //  * @brief Send Ember+ parameter to server
    //  * @param parameter to be set
    //  * @return SUCCESS if sending parameter was OK
    //  */
    // dlb_pmd_success sendParameter(EmberParam parameter);
    /**
     * @brief Tell Ember+ server that we want to register on this parameter
     * @param identifier to be set for registered parameter, it has to be unique
     * @param path of identifiers leading to parameter we want to register to
     * @param pathLength length of the path of identifiers
     */
    void registerParameter(const std::string path[], int pathLength);

    ParameterManagerPtr mEmberManager;

private:
    /**
     * @brief Sends GetDirectory command to Ember+ server on main directory
     * @return SUCCESS if sending command succeded
     */
    dlb_pmd_success sendGetDirectoryCommand();
    /**
     * @brief Send GetDirectory command to Ember+ server for node defined by path
     * @param path of the node, for which GetDirectory command has to be called
     * @param pathLength length of the node path
     * @return SUCCESS if sending command succeded
     */
    dlb_pmd_success sendGetDirectoryCommand(const berint *path, int pathLength, GlowElementType type = GlowElementType_Node);
    /**
     * @brief Function throwing errors from ember+ library
     * @param error number
     * @param message error message
     */
    static void onThrowError(int error, pcstr message);
    /**
     * @brief Function called on failed assertion from ember+ library
     * @param pFileName name of file assertion comes from
     * @param lineNumber number of line where assertion comes from
     */
    static void onFailAssertion(pcstr fileName, int lineNumber);
    /**
     * @brief Function type used by NonFramingGlowReader to notify the application of an incoming glow node.
     * @param node pointer to the read node.
     * @param fields flags indicating which fields of @p node have been read.
     * @param path pointer to the first number in the node path, which is
     *     the number of the tree's root node. May be NULL only if pathLength is 0.
     *     See documentation of glow_writeQualifiedNode for an example.
     * @param pathLength number of node numbers at @p path.
     * @param state application-defined state as stored in NonFramingGlowReader.
     */
    static void onNode(const GlowNode *node, GlowFieldFlags fields, const berint *path, int pathLength,
                       voidptr state);

    /**
     * @brief Function handling incoming node
     * @param node handler to incoming node
     * @param fields mask with information, which fields are in incoming parameter
     * @param path handler to node path on ember+ tree
     * @param pathLength length of the path
     */
    void onNode(const GlowNode *node, GlowFieldFlags fields, const berint *path, int pathLength);

    /**
      * Function type used by NonFramingGlowReader to notify the application
      * of an incoming glow parameter.
      * @param pParameter pointer to the read parameter.
      * @param fields flags indicating which fields of @p pParameter have been
      *     read.
      * @param path pointer to the first number in the node path, which is
      *     the number of the tree's root node. May be NULL only if
      *     pathLength is 0.
      *     See documentation of glow_writeQualifiedParameter for an example.
      * @param pathLength number of node numbers at @p path.
      * @param state application-defined state as stored in NonFramingGlowReader.
      */
    static void onParameter(const GlowParameter *pParameter, GlowFieldFlags fields,
                            const berint *path, int pathLength, voidptr state);
    /**
     * @brief Function handling incoming parameter
     * @param parameter handler to incoming parameter
     * @param fields mask with information, which fields are in incoming parameter
     * @param path handler to parameter path on ember tree
     * @param pathLength length of the path
     */
    void onParameter(const GlowParameter *parameter, GlowFieldFlags fields, const berint *path, int pathLength);
    /**
      * Function type used by NonFramingGlowReader to notify the application
      * of an incoming glow command.
      * @param pCommand pointer to the read command.
      * @param path pointer to the first number in the node path, which is
      *     the number of the tree's root node. May be NULL only if
      *     pathLength is 0.
      *     See documentation of glow_writeQualifiedCommand for an example.
      * @param pathLength number of node numbers at @p path.
      * @param state application-defined state as stored in NonFramingGlowReader.
      */
    static void onCommand(const GlowCommand *pCommand, const berint *path, int pathLength, voidptr state);
    /**
     * @brief Function handling incoming command
     * @param pCommand handler to command
     * @param path path on the ember+ tree
     * @param pathLength length of the path
     */
    void onCommand(const GlowCommand *pCommand, const berint *path, int pathLength);
    /**
      * @brief Callback invoked everytime a valid framing package has been received.
      * @param pPackage handler to incoming package
      * @param length length of incoming package
      * @param state application-defined state as stored in NonFramingGlowReader.
      */
    static void onPackageReceived(const byte *pPackage, int length, voidptr state);
    /**
     * @brief Function handling incoming package
     * @param pPackage handler to incoming package
     * @param length length of incoming package
     */
    void onPackageReceived(const byte *pPackage, int length);
    /**
      * Function type used by NonFramingGlowReader to notify the application
      * of an incoming glow stream entry.
      * @param pStreamEntry pointer to the read stream entry.
      * @param state application-defined state as stored in NonFramingGlowReader.
      */
    static void onStreamEntry(const GlowStreamEntry *pStreamEntry, voidptr state);
    /**
     * @brief Function handling incoming stream entry
     * @param pStreamEntry handler to incoming stream entry
     */
    void onStreamEntry(const GlowStreamEntry *pStreamEntry);
    /**
     * @brief Verifies whether the given string is an IP address and converts to internet address
     *
     * @param ip String to be verified
     * @param address Output address in the connect-acceptable format
     *
     * @return True when string is an IP, False otherwise
     */
    bool convertIpAddress(const std::string& ip, struct in_addr& address);
    /**
     * @brief Verifies and converts the given hostname into an internet address
     *
     * @param host Host to be processed (host name or IP address)
     * @param address Output address in the connect-acceptable format
     *
     * @return True on success, False otherwise (indicates problem with address resolution)
     */
    bool hostToAddress(const std::string& host, struct in_addr& address);
    /**
     * @brief Sends prepared data buffer to server
     * @param buffer handler to the buffer to be send
     * @param bufferSize size of the buffer to be send
     * @return SUCCESS if sending buffer was succesfull, ERROR otherwise
     */
    dlb_pmd_success sendToClient(unsigned char *buffer, int bufferSize);

    //! Information if Ember+ library was properly initialized
    bool mEmberInitialized;
    //! Socket connector is trying to connected to
    int mSocket;
    //! The Lawo Ember+ device address
    struct sockaddr_in mEmberAddress;
    //! Handler to GlowReader, which allows to read Ember+ messages
    GlowReader* mGlowReader;
    //! Buffer to keep messages read by GlowReader
    unsigned char mGlowBuffer[MAX_PACKAGE_SIZE];
    //! Instance of the EmberConnectorListener
    EmberConnectorListener& mListener;
    //! Pointer to ember manager
    //! Information for connector if parameters registration has already started
    bool mRegistrationStarted;
};

}}}

#endif /* EMBER_CONNECTOR_H */
