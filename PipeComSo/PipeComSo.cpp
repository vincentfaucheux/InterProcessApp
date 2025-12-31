#include "PipeComSo.h"

tPipeComWrite ::tPipeComWrite(std::string MqttPath, bool* bAllOk_Ptr) {
    // Init config module
    config_Ptr = new tPipeComSoConfig();
    // if( config_Ptr != nullptr) {
    //     // Load MQTT configuration
    //      *bAllOk_Ptr = config_Ptr->LoadConfig(MqttPath);

    // //Check if item in the vModuleListe
    // if(config_Ptr->GetDevicesNumber() == 0) {
    //     std::cerr << "No module found in configuration" << std::endl;
    //     *bAllOk_Ptr = false;
    // } else {
    //     // Initialize Mosquitto
    //     mosquitto_lib_init();
    //     mosq = mosquitto_new(nullptr, true, nullptr);
    //     // if mosq is nullptr, handle the error
    //     if(!mosq) {
    //         std::cerr << "Failed to create Mosquitto instance" << std::endl;
    //         *bAllOk_Ptr = false;
    //     } else {
    //         if(mosquitto_connect(mosq, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
    //             std::cerr << "Failed to connect to Mosquitto broker" << std::endl;
    //             *bAllOk_Ptr = false;
    //         }
    //     }
    // }
    // } else {
    //     std::cerr << "not able to open the config class" << std::endl;
    //     *bAllOk_Ptr = false;
    // }
}

tPipeComWrite ::~tPipeComWrite() {
    //delete config instance
    if( config_Ptr != nullptr) {
        delete config_Ptr;
        config_Ptr = nullptr;
    }
}

tPipeComRead ::tPipeComRead(std::string MqttPath, bool* bAllOk_Ptr) {
    // Init config module
    config_Ptr = new tPipeComSoConfig();
    // if( config_Ptr != nullptr) {
    //     // Load MQTT configuration
    //      *bAllOk_Ptr = config_Ptr->LoadConfig(MqttPath);

    // //Check if item in the vModuleListe
    // if(config_Ptr->GetDevicesNumber() == 0) {
    //     std::cerr << "No module found in configuration" << std::endl;
    //     *bAllOk_Ptr = false;
    // } else {
    //     // Initialize Mosquitto
    //     mosquitto_lib_init();
    //     mosq = mosquitto_new(nullptr, true, nullptr);
    //     // if mosq is nullptr, handle the error
    //     if(!mosq) {
    //         std::cerr << "Failed to create Mosquitto instance" << std::endl;
    //         *bAllOk_Ptr = false;
    //     } else {
    //         if(mosquitto_connect(mosq, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
    //             std::cerr << "Failed to connect to Mosquitto broker" << std::endl;
    //             *bAllOk_Ptr = false;
    //         }
    //     }
    // }
    // } else {
    //     std::cerr << "not able to open the config class" << std::endl;
    //     *bAllOk_Ptr = false;
    // }
}

tPipeComRead ::~tPipeComRead() {
    //delete config instance
    if( config_Ptr != nullptr) {
        delete config_Ptr;
        config_Ptr = nullptr;
    }
}

// int t_Zigbee ::GetDevicesNumber() {
//     if( config_Ptr != nullptr) {
//         return( config_Ptr->GetDevicesNumber());
//     } else {
//         return(0);
//     }
// }

// std::string t_Zigbee ::GetDeviceID(int index) {
//     if( config_Ptr != nullptr) {
//         return( config_Ptr->GetDeviceID(index));
//     } else {
//         return("");
//     }
// }

// void t_Zigbee ::Switch(std::string module, 
//         std::string state) {
//     std::string state_l1 = "";
//     std::string state_l2 = "";
//     // Simple parsing logic for state
//     if(state == "CONFORT") {
//         state_l1 = "OFF";
//         state_l2 = "OFF";
//     } else if(state == "ECO") {
//         state_l1 = "ON";
//         state_l2 = "ON";
//     } else if(state == "HORS GEL") {
//         state_l1 = "ON";
//         state_l2 = "OFF";
//     } else {
//         state_l1 = "OFF";
//         state_l2 = "ON";
//     }

//     // Request to Mosquitto
//     std::string topic = "zigbee2mqtt/" + module + "/set";
//     std::string payload = "{\"state_l1\":\"" + state_l1 + "\"" 
//         +", " 
//         +"\"state_l2\":\"" + state_l2  + "\"" 
//         +"}";
//     mosquitto_publish(
//         mosq,
//         nullptr,
//         topic.c_str(),
//         payload.length(),
//         payload.c_str(),
//     0,
//     false
//     );
// }

