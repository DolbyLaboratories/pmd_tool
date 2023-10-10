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

#ifndef EMBER_DEFINITIONS_H
#define EMBER_DEFINITIONS_H

#include <string>

namespace dolby { namespace dlb_pmd { namespace ember {

typedef enum EmberConnectErrorInfo{
  EMBER_SOCK_ERR_NONE,
  EMBER_SOCK_ERR_HOST_NOT_FOUND, 
  EMBER_SOCK_ERR_SOCK_CREATION_FAIL,
  EMBER_SOCK_ERR_TIMEOUT,
  EMBER_SOCK_ERR_CONFIG_FAIL,
  EMBER_SOCK_ERR_CONNECTION_FAIL,
  EMBER_SOCK_ERR_SERVICE_ERR,
  EMBER_SOCK_ERR_CONNECTION_LOST} EmberConnectErrorInfo;

typedef enum EmberConnectionState{
    EMBER_STATE_DISCONNECTED,
    EMBER_STATE_CONNECTING,
    EMBER_STATE_CONNECTED
} EmberConnectionState;

class EmberConnectionStatus{
    public:
    EmberConnectionState state;
    EmberConnectErrorInfo errInfo;
};

//! Maximum size for incoming glow package
const unsigned int MAX_PACKAGE_SIZE = 1024 * 5;
//! Maximum size of ember command send by us to ember+ server
const unsigned int EMBER_COMMAND_SIZE = 512;
//! Asummed maximum length of the char buffer for received ember+ package
const unsigned int EMBER_BUFFER_LENGTH = 16;
//! Poll intervals value used while connecting in miliseconds
const unsigned int DEFAULT_POLL_INTERVAL = 50;
//! Default ember+ connection port number
const int DEFAULT_PORT_NUMBER  = 9000;
//! Ember keep alive valid package length
const int EMBER_PACKAGE_LENGTH = 4;
//! Maximum length of ember+ path to be given by user
const int MAX_PATH_LENGTH = 10;
//! Default hostname value
const std::string DEFAULT_HOSTNAME("localhost");
//! Predefined invalid socket value
const int INVALID_SOCKET = -1;
//! Predefined invalid result value
const int INVALID_RESULT = -1;

//! Number of ember+ buttons we can define
const int NUM_OF_BUTTONS            = 9;
//! Length of the path leading to user buttons panel
const int USER_BUT_PAN_PATH_LENGTH  = 4;
//! Length of the path leading to user buttons
const int USER_BUTTON_PATH_LENGTH   = 5;

//! Number of input channels we can define
const int NUM_OF_CHANNELS = 64;
//! Number of ember+ attributes used for every channel
const int PARAMS_PER_CHANNEL = 6;
//! Length of the path leading to input channels node
const int CHANNELS_NODE_PATH_LENGTH = 2;
//! Length of the path leading to input channels
const int INPUT_CHANNEL_PATH_LENGTH = 3;
//! Length of the path leading to input channels aparameters, like mute, panning, fader
const int CHANNEL_PARAM_PATH_LENGTH = 4;

//! Definition of pressed button state
const int BUTTON_STATE_PRESSED = 1;
//! Hardcoded number of parameters to be registered in first registration step - 9 buttons, it's state, lamp and label
const int REGISTERED_BUTTONS = 36;
//! Hardcoded number of parameters we know, that need to be registered, to change conenction status to CONNECTED
const int REGISTERED_PARAMETERS = REGISTERED_BUTTONS + NUM_OF_CHANNELS * PARAMS_PER_CHANNEL;
//! Value used to calculate fader between system model and ember+
const float FADER_FACTOR = 32.0;
//! Undefined value for input channel
const int CHANNEL_UNDEFINED   = -1;

//! Divider/multiplier used to recalculate panning possion between ember+ and system model
const float PANNER_DIVIDER = 40.0;
//! Offset used to recalculate panning possion between ember+ and system model
const float PANNER_OFFSET = 20.0;

enum EmberPackageIdentifiers
{
    EMBER_PACKAGE_ID_MESSAGE = 1,
    EMBER_PACKAGE_ID_TYPE    = 2
};

/**
 * @brief Enum defining button lamps states
 */
enum ButtonLamp
{
    BUTTON_LAMP_OFF         = 0,
    BUTTON_LAMP_RED         = 1,
    BUTTON_LAMP_GREEN       = 2,
    BUTTON_LAMP_YELLOW      = 3,
    BUTTON_LAMP_BLUE        = 4,
    BUTTON_LAMP_MAGENTA     = 5,
    BUTTON_LAMP_AQUAMARINE  = 6,
    BUTTON_LAMP_GREY        = 7,
};

/**
 * @brief Type defining function properties
 */
typedef enum
{
    BUTTON_LAMP     = 0,
    BUTTON_STATE    = 1,
    BUTTON_LABEL    = 2,
    CHANNEL_MUTE    = 3,
    CHANNEL_FADER   = 4,
    CHANNEL_PANN_X  = 5,
    CHANNEL_PANN_Y  = 6,
    CHANNEL_PANN_Z
} FunctionalityPropertyId;

/**
 * @brief Enum defining valid user buttons
 */
typedef enum
{
    BUTTON_UNDEFINED    = -1,
    BUTTON_1            = 0,
    BUTTON_2            = 1,
    BUTTON_3            = 2,
    BUTTON_4            = 3,
    BUTTON_5            = 4,
    BUTTON_6            = 5,
    BUTTON_7            = 6,
    BUTTON_8            = 7,
    BUTTON_9            = 8,
} ButtonIdentifier;

typedef enum
{
    CHAN_PARAM_UNDEFINED = -1,
    CHAN_PARAM_MUTE      = 0,
    CHAN_PARAM_FADER     = 1,
    CHAN_PARAM_PANNER    = 2
} ChannelParams;

typedef enum
{
    PARAM_UNDEFINED = -1,
    PARAM_BUTTON    = 0,
    PARAM_CHANNEL   = 1
} ParameterType;

typedef union
{
    char *label;
    int value;
}ParamValue;

typedef enum
{
    PARAM_TYPE_UNDEFINED = 0,
    PARAM_TYPE_INT       = 1,
    PARAM_TYPE_STRING    = 2,
    PARAM_TYPE_BOOLEAN   = 3
}ParamType;

typedef struct
{
    std::string identifier;
    ParamValue  value;
    ParamType   type;
} EmberParam;

//! Hardcoded path of identifiers leading to user buttons panel
const std::string PATH_USR_BUTTONS_PANEL[] = { "_9", "monitoringPanels" , "buttons", "panel25"};

//! Identifier of user buttons panel parameter
const std::string ID_STR_USR_BUTTONS_PANEL("panel25");
//! Identifier of buttons lamp parameters
const std::string ID_STR_LAMP("_3e9");
//! Identifier of buttons state parameters
const std::string ID_STR_STATE("_3e8");
//! Identifier of buttons label parameters
const std::string ID_STR_LABEL("_3ea");
//! Identifier of used user buttons
const std::string ID_STR_USR_BUTTONS[] = { "button1",
                                           "button2",
                                           "button3",
                                           "button4",
                                           "button5",
                                           "button6",
                                           "button7",
                                           "button8",
                                           "button9"};

//! Hardcoded path of identifiers leading to input channels
const std::string PATH_STR_CHANNEL[] = { "_2", "_1"};
//! Identifier of input channels node
const std::string ID_STR_CHANNEL_NODE("_1");
//! Identifiers of used input channels
const std::string ID_STR_CHANNEL[] = { "_1",  "_2",  "_3",  "_4",  "_5",  "_6",  "_7",  "_8" ,
                                       "_9",  "_a",  "_b",  "_c",  "_d",  "_e",  "_f",  "_10",
                                       "_11", "_12", "_13", "_14", "_15", "_16", "_17", "_18",
                                       "_19", "_1a", "_1b", "_1c", "_1d", "_1e", "_1f", "_20",
                                       "_21", "_22", "_23", "_24", "_25", "_26", "_27", "_28",
                                       "_29", "_2a", "_2b", "_2c", "_2d", "_2e", "_2f", "_30",
                                       "_31", "_32", "_33", "_34", "_35", "_36", "_37", "_38",
                                       "_39", "_3a", "_3b", "_3c", "_3d", "_3e", "_3f", "_40"};
//! Identifier of mute node
const std::string ID_STR_MUTE("_400016c0");
//! Identifier of mute parameter
const std::string ID_STR_MUTE_LEAF("_400016c1");
//! Identifier of fader node
const std::string ID_STR_FADER("_400016b0");
//! Identifier of fader parameter
const std::string ID_STR_FADER_LEAF("_400016b1");
//! Identifier of panning node
const std::string ID_STR_PAN("_400016d0");
//! Identifier of mute panning x parameter
const std::string ID_STR_PAN_X("_400016d1");
//! Identifier of mute panning y parameter
const std::string ID_STR_PAN_Y("_400016d2");
//! Identifier of mute panning z parameter
const std::string ID_STR_PAN_Z("_400016d3");

// AvatusMDConnector-specific paths
// const std::string PATH_AVATUS_FADER_1_GAIN[] = {"AvatusMDConnector", "Fader 1", "gain"};
// const std::string PATH_AVATUS_FADER_2_GAIN[] = {"AvatusMDConnector", "Fader 2", "gain"};
// const std::string PATH_AVATUS_FADER_3_GAIN[] = {"AvatusMDConnector", "Fader 3", "gain"};
// const std::string PATH_AVATUS_FADER_4_GAIN[] = {"AvatusMDConnector", "Fader 4", "gain"};
// const std::string PATH_AVATUS_FADER_5_GAIN[] = {"AvatusMDConnector", "Fader 5", "gain"};
// const std::string PATH_AVATUS_FADER_6_GAIN[] = {"AvatusMDConnector", "Fader 6", "gain"};
// const std::string PATH_AVATUS_FADER_7_GAIN[] = {"AvatusMDConnector", "Fader 7", "gain"};
// const std::string PATH_AVATUS_FADER_8_GAIN[] = {"AvatusMDConnector", "Fader 8", "gain"};
// const int PATH_AVATUS_FADER_PATH_LENGTH = 3;

}}}

#endif /*EMBER_DEFINITIONS_H*/
