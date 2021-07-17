/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
