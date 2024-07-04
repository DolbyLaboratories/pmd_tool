/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#ifndef __PMD_STUDIO_CONSOLE_EMBER_PLUS_H__
#define __PMD_STUDIO_CONSOLE_EMBER_PLUS_H__

class PMDStudioConsoleEmberPlus;

#include "pmd_studio_console.h"
#include "ember_controller.h"
#include "pmd_studio.h"
#include "ember_definitions.h"
#include <string>
#include <queue>
extern "C"{
    #include "glow.h"
}
#include <unordered_map>
#include "parameter_manager.h"
#include "ember_connector.h"

using namespace dolby::dlb_pmd::ember;


class 
PMDStudioConsoleEmberPlus
    :public PMDStudioConsoleInterface
    ,public dolby::dlb_pmd::ember::EmberController
{
    
    public:
    
    dlb_pmd_success connect() override;
    dlb_pmd_success disconnect() override;
    dlb_pmd_success poll(int timeout) override;
    
    PMDStudioConsoleEmberPlus
        (pmd_studio *studio
        );

    ~PMDStudioConsoleEmberPlus();

    void 
    registerEmberParameter
        ( std::string *path
        , int pathLength
        , int unique_id
        , void (*callback)(PMDStudioConsoleEmberPlus*, int, const GlowParameter*)
        );

    private:
    typedef std::unordered_map<std::string, void*> GlowParameterCallbackMap;
    
    GlowParameterCallbackMap mGlowParameterCallbackMap;

    void 
    onParameter
        ( const std::string id
        , const GlowParameter *param
        ) override;

    void 
    onParameterRegistered
        (std::string identifier
        ) override;


    enum class UIParameterType{
        FADER_GAIN,
        MUTE_BTN
    };

    struct GlowParamMapping{
        std::string id;
        void *callback;
    };


    void 
    changeStatus
        (const EmberConnectionState newStatus, bool hasError = false
        ,const EmberConnectErrorInfo error = EMBER_SOCK_ERR_NONE
        ) override;
    

    class 
    CallbackManager
    {
        public:
        CallbackManager
            (PMDStudioConsoleEmberPlus *parent
            ,EmberConnector &connector
            ,pmd_studio* studio);

        void queue
            (std::string *path
            ,int pathLength
            ,int unique_id
            ,void (*callback)(PMDStudioConsoleEmberPlus*, int, const GlowParameter *)
            );
        
        bool 
        handleParameter
            (const std::string id
            ,const GlowParameter *param);

        void 
        issueNextRegistration
            (void
            );

        void 
        finishRegistration
            (const std::string id
            );

        struct 
        EmberCallbackMapping{
            const std::string *path;
            int pathLength;
            int id;
            void (*callback)(PMDStudioConsoleEmberPlus*, int, const GlowParameter *);
        };

        std::queue<EmberCallbackMapping> mEmberCallbackMappingQueue;
        std::unordered_map<std::string, EmberCallbackMapping> mEmberCallbackMap;
        EmberConnector *mEmberConnector;
        bool registrationOngoing;
        pmd_studio *mPmdStudio;
        PMDStudioConsoleEmberPlus *parent;
    };

    CallbackManager mCallbackManager;
};

#endif
