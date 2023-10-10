#ifndef NMOS_CPP_NODE_NODE_IMPLEMENTATION_H
#define NMOS_CPP_NODE_NODE_IMPLEMENTATION_H

#include "cpprest/json_utils.h"
#include "nmos/connection_activation.h"
#include "nmos/resources.h"
#include "nmos/connection_api.h"
#include "nmos/node_behaviour.h"

namespace slog
{
    class base_gate;
}

namespace nmos
{
    struct node_model;
    typedef web::json::value settings;
    
    typedef std::function<void(const std::string sdp)> NmosNodeConnectionReqCallBack; /** This callback provides a request to the application to connect a source specified in the SDP*/
    typedef std::function<void(const std::string registryUrl)> NmosNodeRegistryFoundCallBack; /** This callback provides a request to the application to connect a source specified in the SDP*/

    struct node_call_backs
    {
        NmosNodeRegistryFoundCallBack nmosNodeRegistryFoundCallBack;
        NmosNodeConnectionReqCallBack nmosNodeConnectionReqCallBack;
    };


    /*
    struct NmosNodeConnectionReqCallBackInfo
    {
        void (*callBack)                // Function to be called back 
        (std::string sdp,               // SDP of stream to be connected to
         void *userData                 // Opaque pointer that can be used to convey object etc 
         );
        void *userData;
    }; */

    namespace experimental
    {
        struct node_implementation;
    }
}



// This is an example of how to integrate the nmos-cpp library with a device-specific underlying implementation.
// It constructs and inserts a node resource and some sub-resources into the model, based on the model settings,
// starts background tasks to emit regular events from the temperature event source and then waits for shutdown.
void node_implementation_thread(nmos::node_model& model, slog::base_gate& gate);

// This constructs all the callbacks used to integrate the example device-specific underlying implementation
// into the server instance for the NMOS Node.
nmos::experimental::node_implementation make_node_implementation(nmos::node_model& model, slog::base_gate& gate, const nmos::node_call_backs &callBacks);

nmos::registration_handler make_node_implementation_registration_handler(slog::base_gate& gate, const nmos::node_call_backs& callBacks);
nmos::connection_resource_auto_resolver make_node_implementation_auto_resolver(const nmos::settings& settings);
nmos::connection_sender_transportfile_setter make_node_implementation_transportfile_setter(const nmos::resources& node_resources, const nmos::settings& settings);
nmos::transport_file_parser make_node_implementation_transport_file_parser();
nmos::details::connection_resource_patch_validator make_node_implementation_patch_validator();


#endif
