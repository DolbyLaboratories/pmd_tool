/************************************************************************
 * dlb_pmd
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


#include "pmd_studio_console_emberplus.h"
#include "pmd_studio_console.h"
#include "ember_connector.h"
#include "parameter_manager.h"
#include "pmd_studio.h"
#include "pmd_studio_settings.h"
#include "dlb_pmd_api.h"
#include "utils.h"

extern "C"
{
#include "ui.h"
}
#include <iostream>
#include <queue>
#include <string>

std::string PATH_AVATUS_FADER_1_GAIN[] {"AvatusMDConnector", "Fader 1", "Gain"};
std::string PATH_AVATUS_FADER_2_GAIN[] {"AvatusMDConnector", "Fader 2", "Gain"};
std::string PATH_AVATUS_FADER_3_GAIN[] {"AvatusMDConnector", "Fader 3", "Gain"};
std::string PATH_AVATUS_FADER_4_GAIN[] {"AvatusMDConnector", "Fader 4", "Gain"};
std::string PATH_AVATUS_FADER_5_GAIN[] {"AvatusMDConnector", "Fader 5", "Gain"};
std::string PATH_AVATUS_FADER_6_GAIN[] {"AvatusMDConnector", "Fader 6", "Gain"};
std::string PATH_AVATUS_FADER_7_GAIN[] {"AvatusMDConnector", "Fader 7", "Gain"};
std::string PATH_AVATUS_FADER_8_GAIN[] {"AvatusMDConnector", "Fader 8", "Gain"};

using namespace dolby::dlb_pmd::ember;


static
void 
handleAvatusParam
    (PMDStudioConsoleEmberPlus* console_interface
    ,int id
    ,const GlowParameter *param
    )
{
    pmd_studio *studio = console_interface->mPmdStudio;
    if(0<id && id<=4)
    {
        // BED
        dlb_pmd_element_id *eids;
        unsigned int *labels;
        int nbeds = pmd_studio_audio_beds_get_eids(&eids, &labels, studio, false);

        if(nbeds > id-1)
        {
            console_interface->setBedGain(id-1, param->value.choice.integer);
        }
    }
    else if(4<id && id <=8)
    {
        // Object
        dlb_pmd_element_id *eids;
        int nobjs = pmd_studio_audio_objects_get_eids(&eids, studio, false);

        int objectno = id-5;
        if(nobjs > objectno)
        {
            console_interface->setObjGain(objectno, param->value.choice.integer);
        }
    }
}


PMDStudioConsoleEmberPlus::PMDStudioConsoleEmberPlus
    (pmd_studio *studio
    )
    :PMDStudioConsoleInterface(studio),
    EmberController(),
    mCallbackManager{this, mEmberConnector, studio}
{
    this->init();
    this->mCallbackManager.queue(PATH_AVATUS_FADER_1_GAIN, 3, 1, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_2_GAIN, 3, 2, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_3_GAIN, 3, 3, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_4_GAIN, 3, 4, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_5_GAIN, 3, 5, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_6_GAIN, 3, 6, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_7_GAIN, 3, 7, handleAvatusParam);
    this->mCallbackManager.queue(PATH_AVATUS_FADER_8_GAIN, 3, 8, handleAvatusParam);
}

PMDStudioConsoleEmberPlus::~PMDStudioConsoleEmberPlus()
{
    if(this->getConnectionStatus().state != EMBER_STATE_DISCONNECTED)
    {
        this->disconnect();
    }
}

dlb_pmd_success 
PMDStudioConsoleEmberPlus::connect()
{
    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(this->settings->address.sin_addr), addrstr, INET_ADDRSTRLEN);
    
    std::string s = std::string(addrstr);
    dlb_pmd_success ret = EmberController::connect(s, this->settings->address.sin_port)? PMD_SUCCESS:PMD_FAIL;
    if(!ret)
    {
        changeStatus(EMBER_STATE_CONNECTING);
    }
    return ret;
}



dlb_pmd_success 
PMDStudioConsoleEmberPlus::disconnect()
{
    EmberController::disconnect();
    return PMD_SUCCESS;
}


dlb_pmd_success 
PMDStudioConsoleEmberPlus::poll
    (int timeout
    )
{
    if(EmberController::poll(timeout) != PMD_SUCCESS)
    {
        EmberConnectionStatus s = this->getConnectionStatus();
        if(s.errInfo != EMBER_SOCK_ERR_NONE)
        {
            this->disconnect();
            if (
                s.errInfo == EMBER_SOCK_ERR_CONFIG_FAIL ||
                s.errInfo == EMBER_SOCK_ERR_SOCK_CREATION_FAIL
            )
            {
                // fprintf(stdout, "Issue during poll. Reconnecting.\n\n\n");
                return this->connect();
            }
        }
        return PMD_FAIL;
    }
    return PMD_SUCCESS;
}

void 
PMDStudioConsoleEmberPlus::changeStatus
    (const EmberConnectionState newStatus
    ,bool hasError
    ,const EmberConnectErrorInfo error
    )
{
    dolby::dlb_pmd::ember::EmberController::changeStatus(newStatus, hasError, error);
    this->status.state = (ConsoleConnectionState) newStatus;
    this->status.errInfo = (ConsoleConnectErrorInfo) error;
    switch(newStatus)
    {
        case EMBER_STATE_CONNECTED:
            PMDStudioConsoleInterface::onConnected();
            mCallbackManager.issueNextRegistration();
            break;
        case EMBER_STATE_CONNECTING:
            PMDStudioConsoleInterface::onConnecting();
            break;
        case EMBER_STATE_DISCONNECTED:
            PMDStudioConsoleInterface::onDisconnected();
            break;
        default:
            break;
    }
}

void 
PMDStudioConsoleEmberPlus::registerEmberParameter
    (std::string *path
    ,int pathLength
    ,int unique_id
    ,void (*callback)(PMDStudioConsoleEmberPlus*, int, const GlowParameter *)
    )
{
    mCallbackManager.queue(path, pathLength, unique_id, callback);
}

void 
PMDStudioConsoleEmberPlus::onParameter
    (const std::string id
    ,const GlowParameter *parameter
    )
{
    EmberController::onParameter(id, parameter);
    mCallbackManager.handleParameter(id, parameter);
}

void
PMDStudioConsoleEmberPlus::onParameterRegistered
    (std::string identifier
    )
{
    mCallbackManager.finishRegistration(identifier);
}

PMDStudioConsoleEmberPlus::CallbackManager::CallbackManager
    (PMDStudioConsoleEmberPlus *parent
    ,EmberConnector &connector
    ,pmd_studio *studio)
{
    mEmberConnector = &connector;
    registrationOngoing = false;
    mPmdStudio = studio;
    this->parent = parent;
}

void 
PMDStudioConsoleEmberPlus::CallbackManager::queue
    (std::string *path
    ,int pathLength
    ,int unique_id
    ,void(* callback)(PMDStudioConsoleEmberPlus*, int, const GlowParameter *)
    )
{
    PMDStudioConsoleEmberPlus::CallbackManager::EmberCallbackMapping mapping{path, pathLength, unique_id, callback};
    mEmberCallbackMappingQueue.push(mapping);
}

bool 
PMDStudioConsoleEmberPlus::CallbackManager::handleParameter
    (const std::string id
    ,const GlowParameter *param
    )
{
    // TODO: debug ifdef wrap
    // fprintf(stdout, "Received parameter: [%s] %d\n", id.c_str(), param->value.choice.integer);
    std::unordered_map<std::string, EmberCallbackMapping>::iterator it = mEmberCallbackMap.find(id);
    if (it != mEmberCallbackMap.end())
    {
        EmberCallbackMapping m = it->second;
        // TODO: Replace with more generic callback
        m.callback(parent, m.id, param);
        
    }
    return true;
}
    
void 
PMDStudioConsoleEmberPlus::CallbackManager::issueNextRegistration()
{
    if(registrationOngoing)
    {
        this->mEmberCallbackMappingQueue.pop();
    }
    
    if(this->mEmberCallbackMappingQueue.size() == 0)
    {
        registrationOngoing = false;
        return;
    }
    CallbackManager::EmberCallbackMapping *next = &mEmberCallbackMappingQueue.front();
    this->mEmberConnector->registerParameter(
        next->path,
        next->pathLength
    );
    registrationOngoing = true;
}

void 
PMDStudioConsoleEmberPlus::CallbackManager::finishRegistration
    (const std::string id
    )
{
    CallbackManager::EmberCallbackMapping *next = &mEmberCallbackMappingQueue.front();
    mEmberCallbackMap.insert({id, *next});
    try{
        // Try finding new identifier by path strings of attempted registration.
        std::string s = mEmberConnector->mEmberManager->findParamIdentifierByPath(next->path, next->pathLength);
        // If we got this far, we know the registration was successful.
        mEmberCallbackMap.insert({s, *next});

        // std::cout << "PMDStudioConsoleEmberPlus - Parameter registered: [" << s << "] '";
        // std::cout << next->path[0];
        // for(int i=1; i<next->pathLength; i++){
        //     std::cout << "->" << next->path[i];
        // }
        // std::cout << "'" << std::endl;

        issueNextRegistration();
    }
    catch(std::logic_error)
    {
        std::cerr << "PMDStudioConsoleEmberPlus - This should never happen. Unexpected registration: " << id << std::endl;
    }
}
