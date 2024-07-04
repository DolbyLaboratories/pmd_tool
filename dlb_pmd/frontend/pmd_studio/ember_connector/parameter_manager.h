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
#ifndef PARAMETER_MANAGER_H
#define PARAMETER_MANAGER_H

#include <stdlib.h>
#include <string>
#include <unordered_map>

#include "ember_definitions.h"
extern "C"{
    #include "emberplus.h"
    #include "emberinternal.h"
    #include "bertypes.h"
}

namespace dolby { namespace dlb_pmd { namespace ember {

const std::string generateIdentifierFromGlowPath(const berint *path, int pathLength);

/**
 * @brief The class responsible for keeping registered Ember+ parameters and managing them.
 */ 
class ParameterManager
{
public:
    //! Type for structure holding Ember+ node/parameter descriptor
    typedef struct
    {
        std::string pathOfIdentifiers[MAX_PATH_LENGTH]; /**< Table of strings, which are constant, unique path identifier
                                                             for example: _9/monitoringPanels/buttons/panel7/button1 */
        int pathLength;                                 /**< Ember+ path length (also pathOfIdentifiers as they
                                                             are equal length) */
        int *path;                                      /**< Ember+ parameter path assigned to path of identifiers
                                                             by current server */
    } ParameterDescriptor;
    
    /**
     * @brief Class providing interface for listener to events dispatched by parameter_manager
     */
    class EmberManagerListener
    {
    public:
        /**
         * @brief Called when parameter is succesfully registered
         * @param identifier of succesfully registered paramater
         */
        virtual void onParameterRegistered(std::string identifier) = 0;
    };

    ParameterManager(EmberManagerListener& listener);
    ~ParameterManager();

    //! List of node descriptors
    typedef std::unordered_map<std::string, ParameterDescriptor> ParameterDescriptorList;
    
    /**
     * @brief Flush current ember+ parameter manager members
     */
    void clear();

    /**
     * @brief Search amongst registered nodes/parameters and find parameter with path matching given pattern
     * @param path pattern to be searched for
     * @param pathLength length of the path patterns
     * @return identifier of found parameter (string)
     */
    // std::string findParamIdentifierByPath(const berint *path, int pathLength);
    std::string findParamIdentifierByPath(const std::string *path, int pathLength);
    
    /**
    * @brief Return information if parameter is on the list of registered parameters
    * @param identifier of parameter to be checked
    * @return true if parameter identifier is on list of registered parameters
    */
    bool isRegisteredParameter(const std::string identifier);
    /**
     * @brief Return information if parameter is on the list of registered parameters
     * @param path of the parameter to be checked
     * @param pathLength length of the path of the parameter to be checked
     * @return true if parameter path identification matches registered parameter
     */
    bool isRegisteredParameter(const berint *path, int pathLength);
    
    /**
     * @brief Get ParameterDescriptor by string identifier
     * @param identifier of parameter descriptor, we want to get
     * @return ParameterDescriptor with given identifier
     */
    ParameterDescriptor getParamDescriptor(const std::string identifier);
    
    /**
     * @brief Get ParameterDescriptor by string identifier
     * @param identifier of parameter descriptor, we want to get
     * @return ParameterDescriptor with given identifier
     */
    ParameterDescriptor operator[](const std::string identifier);

    /**
     * @brief Set current ParameterDescriptor
     * @param currentPathdescriptor oath descriptor to be set as current
     */
    void setCurrentParamDescriptor(const ParameterDescriptor &currentParamDescriptor);
    
    /**
     * @brief Set current ParameterDescriptor
     * @param identifier unique identifier we want to be used for this parameter descriptor
     * @param path of identifiers to be set for current parameter descriptor
     * @param pathLength length of the path to be set
     */
    void setCurrentParamDescriptor(const std::string path[], const int pathLength);
    
    /**
     * @brief Function checks if node given as parameter is on path of current parameter descriptor.
     * It returns true, if it is. It also registers some demanded nodes, like user buttons panel and user buttons.
     * @param node to be checked
     * @param fields flags describing incoming node fields
     * @param path of incoming node
     * @param pathLength length of the path of incoming node
     * @return true if node is on current parameter descriptor path
     */
    bool isNodeDestinedToSend(const GlowNode *node, GlowFieldFlags fields, const berint *path, int pathLength);
    
    /**
     * @brief Register incoming ember+ parameter
     * @param parameter glow parameter to be registered
     * @param path of the parameter
     * @param pathLength of the parameter
     * @param function index of the functionality (button/input channel), to which parameter applies to
     * @param type of the parameter - is it button or channel parameter
     * @return true if registration was succesful
     */
    bool registerParameter(const GlowParameter *parameter, const berint *path, int pathLength);

private:
    /**
     * @brief Cleanup current PathDescriptor
     */
    void clearCurrentParamDescriptor();
    /**
     * @brief Cleanup path descriptor list
     */
    void clearParamDescriptorList();
    /**
     * @brief Add ParamDescriptor to the map of known Ember+ parameters
     * @param identifier of parameter to be added
     * @param parameter descriptor describing added parameter
     * @param path of the parameter
     * @param pathLength length of the path
     */
    void addParamDescriptor(std::string identifier, const ParameterDescriptor *param, const int *path,
                            const int pathLength);
    /**
     * @brief Function checks if given parameter descriptor is valid
     * @param descriptor handler to descriptor to be checked
     * @return true if valid
     */
    inline bool isValidParameterDescriptor(const ParameterDescriptor *descriptor)
    {
        return (descriptor->pathLength > 0);
    }

    //! Current path descriptor - one which connector currently tries to acquire from Ember+ server. NOT THREAD SAFE!
    ParameterDescriptor mCurrentParamDescriptor;
    //! List of descriptors of already known (registered) Ember+ parameters. NOT THREAD SAFE!
    ParameterDescriptorList mParamDescriptorsList;
    //! Instance of EmberManagerListener
    EmberManagerListener& mListener;


    //! Information for manager, how deep node tree should be search, while checking parameter identifiers.
    //! It is used, because many nodes on different levels have the same identifier. To avoid confusing the same
    //! identifiers from different node tree level, this variable was introduced. It is reset after every
    //! registration group finishes.
    int mNodeDepthCounter;

};

}}}

#endif /* PARAMETER_MANAGER_H */
