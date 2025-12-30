#include "main_config.h"

tConfigData::tConfigData(){
    Zigbee2MqttPath = "";
    ZigbeeAppLib = "";
}

tConfigData::~tConfigData(){
}

tMainConfig::tMainConfig(){
    ConfigData_Ptr = new tConfigData();
}

tMainConfig::~tMainConfig(){
    if( ConfigData_Ptr != nullptr ) {
        delete ConfigData_Ptr;
    }
}

bool tMainConfig::LoadConfig(){
    //init return code
    bool bRet = true;

    //check data pointer is valid
    if( ConfigData_Ptr != nullptr ) {
        //data pointer is valid, load yaml file
        try {
            YAML::Node YamlConfig = YAML::LoadFile( "configuration.yaml");

            if( YamlConfig["mqtt_config_path"] ) {
                ConfigData_Ptr->Zigbee2MqttPath = YamlConfig["mqtt_config_path"].as<std::string>();
                std::cout << "mqtt_config_path:" << ConfigData_Ptr->Zigbee2MqttPath << std::endl;
            } else {
                std::cout << "mqtt_config_path not found in configuration.yaml" << std::endl;
                bRet = false;
            }

            if( YamlConfig["zigbee_app_lib"] ) {
                ConfigData_Ptr->ZigbeeAppLib = YamlConfig["zigbee_app_lib"].as<std::string>();
                std::cout << "zigbee_app_lib:" << ConfigData_Ptr->ZigbeeAppLib << std::endl;
            } else {
                std::cout << "zigbee_app_lib not found in configuration.yaml" << std::endl;
                bRet = false;
            }
        }

        //error bad file
        catch (const YAML::BadFile& e) {
            std::cerr << "Error reading YAML file: " << e.what() << std::endl;
            bRet = false;
        }

        //error parsing file
        catch (const YAML::ParserException& e) {
            std::cerr << "Error parsing YAML file: " << e.what() << std::endl;
            bRet = false;
        }

    } else {
        //data pointer not valid
        bRet = false;
    }

    return bRet;
}

