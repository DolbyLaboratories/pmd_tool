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


#include <stdlib.h>
#include <string>
#include <unordered_map>

#include "ember_definitions.h"
extern "C"{
#include "emberplus.h"
#include "emberinternal.h"
#include "bertypes.h"
}
#include "parameter_manager.h"
#include <sstream>
#include <cstring>

using namespace dolby::dlb_pmd::ember;
using namespace std;

const string dolby::dlb_pmd::ember::generateIdentifierFromGlowPath(const berint *path, int pathLength){
    std::ostringstream os;
    os << path[0];
    if(pathLength > 1){
        for(int i=1; i<pathLength; i++){
            os << '.' << path[i];
        }
    }
    return os.str();
}

ParameterManager::ParameterManager(EmberManagerListener& listener):
mListener(listener){}

ParameterManager::~ParameterManager(){}

void ParameterManager::clear()
{
    clearParamDescriptorList();
    clearCurrentParamDescriptor();
}

void ParameterManager::clearParamDescriptorList()
{
    ParameterDescriptorList::const_iterator it;
    for (it = mParamDescriptorsList.begin(); it != mParamDescriptorsList.end(); ++it)
    {
        if (it->second.path != NULL)
        {
            free(it->second.path);
        }
    }
    mParamDescriptorsList.clear();
}

void ParameterManager::clearCurrentParamDescriptor()
{
    /* we do not free current descriptor path, as it was never allocated for current descriptor */
    mCurrentParamDescriptor.pathLength = 0;
}

// string ParameterManager::findParamIdentifierByPath(const berint *pPath, int pathLength)
// {
//     int i      = 0;
//     bool found = false;
//     string identifier;
//     ParameterDescriptorList::iterator it;
//     for (it = mParamDescriptorsList.begin(); it != mParamDescriptorsList.end(); ++it)
//     {
//         if (it->second.pathLength == pathLength && !found)
//         {
//             for (i = 0; i < pathLength; ++i)
//             {
//                 if (pPath[i] != it->second.path[i])
//                 {
//                     break;
//                 }
//                 if (i == pathLength - 1)
//                 {
//                     identifier = it->first;
//                     // _logger->Debug("Identifier of found parameter: %s, path[%d] = %d", identifier.c_str(), i, pPath[i]);
//                     found = true;
//                     break;
//                 }
//             }
//         }
//     }
//     return identifier;
// }

string ParameterManager::findParamIdentifierByPath(const std::string *pPath, int pathLength)
{
    int i      = 0;
    string identifier;
    ParameterDescriptorList::iterator it;
    for (it = mParamDescriptorsList.begin(); it != mParamDescriptorsList.end(); ++it)
    {
        if (it->second.pathLength == pathLength)
        {
            for (i = 0; i < pathLength; ++i)
            {
                if (pPath[i].compare(it->second.pathOfIdentifiers[i])!=0)
                {
                    break;
                }
                if (i == pathLength - 1)
                {
                    return it->first;
                }
            }
        }
    }
    throw std::logic_error("Invalid identifier path");
}
void ParameterManager::addParamDescriptor(string identifier, const ParameterDescriptor *param,
                                          const int *path, const int pathLength)
{
    if (param == NULL)
    {
        // _logger->Error("%s Parameter descriptor is NULL, cannot add it!", __func__);
        return;
    }

    ParameterDescriptor paramDescriptor;
    int i = 0;
    int size = pathLength * sizeof(int);

    paramDescriptor.pathLength = pathLength;
    paramDescriptor.path = (int *)malloc(size);
    memcpy(paramDescriptor.path, path, size);
    paramDescriptor.pathLength = param->pathLength;
    for (i = 0; i < param->pathLength; ++i)
    {
        paramDescriptor.pathOfIdentifiers[i] = param->pathOfIdentifiers[i];
    }
    mParamDescriptorsList.insert({identifier, paramDescriptor});

    // fprintf(stdout, "ParameterManager: Registered identifier [%s] %s\n", identifier.c_str(), param->pathOfIdentifiers[param->pathLength-1].c_str());
    if(pathLength == 3){
        mListener.onParameterRegistered(identifier);
    }
}

bool ParameterManager::isRegisteredParameter(const string identifier)
{
    ParameterDescriptor descriptor = getParamDescriptor(identifier);
    return (isValidParameterDescriptor(&descriptor));
}

bool ParameterManager::isRegisteredParameter(const berint *path, int pathLength)
{
    bool registered = false;
    ParameterDescriptorList::iterator it;
    for (it = mParamDescriptorsList.begin(); it != mParamDescriptorsList.end(); ++it)
    {
        if (registered)
        {
            break;
        }
        if (it->second.pathLength == pathLength)
        {
            for (int i = 0; i < pathLength; ++i)
            {
                if (path[i] != it->second.path[i])
                {
                    registered = false;
                    break;
                }
                registered = true;
            }
        }
    }
    return registered;
}

void ParameterManager::setCurrentParamDescriptor(const string path[], const int pathLength)
{
    int i = 0;
    clearCurrentParamDescriptor();
    mCurrentParamDescriptor.pathLength   = pathLength;
    for (i = 0; i < pathLength; ++i)
    {
        mCurrentParamDescriptor.pathOfIdentifiers[i] = path[i];
        // _logger->Debug("path[%d] set to : %s path= %s", i, mCurrentParamDescriptor.pathOfIdentifiers[i].c_str(),
        //                path[i].c_str());
    }
    /* we do not set path, as we do not know it yet -
     * it will be known after ember+ server will response with it's value */

    mNodeDepthCounter = 1;
}

void ParameterManager::setCurrentParamDescriptor(const ParameterDescriptor &currentParamDescriptor)
{
    mCurrentParamDescriptor = currentParamDescriptor;
}

ParameterManager::ParameterDescriptor ParameterManager::getParamDescriptor(const string identifier)
{
    ParameterDescriptor emptyDescriptor;
    memset(&emptyDescriptor, 0, sizeof(ParameterDescriptor));
    ParameterDescriptorList::iterator it = mParamDescriptorsList.find(identifier);
    if (it != mParamDescriptorsList.end())
    {
        // _logger->Debug("Parameter descriptor for identifier %s found!", identifier.c_str());
        return it->second;
    }
    return emptyDescriptor;
}

ParameterManager::ParameterDescriptor
ParameterManager::operator[](const std::string identifier){
    return getParamDescriptor(identifier);
}

bool ParameterManager::isNodeDestinedToSend(const GlowNode *node, GlowFieldFlags fields, const berint *path,
                                            int pathLength)
{
    bool send = false;
    int i = 0;

    if (node != nullptr && (fields & GlowFieldFlag_Identifier))
    {
        /* First we check if node is on path of current parameter descriptor - if yes - we suppose to send
           GetDirectoryCommand for it */
        for (i = 0; i < mNodeDepthCounter; ++i)
        {
            if (strcmp(node->pIdentifier, mCurrentParamDescriptor.pathOfIdentifiers[i].c_str()) == 0)
            {
                send = true;
                if (mNodeDepthCounter == mCurrentParamDescriptor.pathLength)
                {
                    mNodeDepthCounter = 0;
                }
                else
                {
                    ++mNodeDepthCounter;
                }
                break;
            }
        }

        // If we're going to use, register node in known parameter list (if not already)
        if(send){
            string childid = generateIdentifierFromGlowPath(path, pathLength);
            ParameterDescriptor child = getParamDescriptor(childid);
            if(!isValidParameterDescriptor(&child)){
                string parentid = generateIdentifierFromGlowPath(path, pathLength-1);
                ParameterDescriptor parent = getParamDescriptor(parentid);
                if(!isValidParameterDescriptor(&parent) && pathLength > 1){
                    // Should not happen.
                    fprintf(stderr, "[ER] Received node parent not registered. Unable to handle.\n");
                    return false;
                }
                else{
                    for(int i=0; i<pathLength-1; i++){
                        child.pathOfIdentifiers[i] = parent.pathOfIdentifiers[i];
                    }
                    child.pathOfIdentifiers[pathLength-1] = node->pIdentifier;
                    child.pathLength = pathLength;
                    addParamDescriptor(childid, &child, path, pathLength);
                }
            }
        }

    }
    return send;
}

bool ParameterManager::registerParameter(const GlowParameter *parameter, const berint *path, int pathLength)
{
    // Check for unwanted registration.
    if(strcmp(parameter->pIdentifier, mCurrentParamDescriptor.pathOfIdentifiers[pathLength-1].c_str()) != 0){ return false; }

    string id = generateIdentifierFromGlowPath(path, pathLength);
    ParameterDescriptor *cpy = (ParameterDescriptor *) malloc(sizeof(ParameterDescriptor));
    memcpy(cpy, &mCurrentParamDescriptor, sizeof(ParameterDescriptor));
    if(cpy == nullptr){
        // Something happened?
        return false;
    }

    addParamDescriptor(id, cpy, path, pathLength);
    return true;
}

