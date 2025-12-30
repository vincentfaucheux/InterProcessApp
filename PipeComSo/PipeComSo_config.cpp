#include "PipeComSo_config.h"

tPipeComSoConfig::tPipeComSoConfig() {
    vModuleListe.clear();
}

tPipeComSoConfig::~tPipeComSoConfig() {
}

bool tPipeComSoConfig::LoadConfig(std::string ConfigPath) {
    bool bRet = true;
    vModuleListe.clear();
    try {
        YAML::Node MqttConfig = YAML::LoadFile(ConfigPath + "/configuration.yaml");
        if( !MqttConfig["devices"] ) {
            std::cerr << "No device in this configuration file" << std::endl;
            bRet = false;
        }
        else {
            YAML::Node devicesNode = MqttConfig["devices"];
            if(( devicesNode) &&( devicesNode.IsMap())) {
                for (const auto& device : devicesNode) {
                    std::string device_id = device.first.as<std::string>();
                    YAML::Node device_data = device.second;
                    if ((device_data["friendly_name"]) &&
                        (device_data["friendly_name"].IsScalar())) {
                        std::string ModuleName =device_data["friendly_name"].as<std::string>();
                        vModuleListe.push_back( ModuleName);
                    }
                    else {
                    }
                }
            }
        }
    }

    catch (const YAML::BadFile& e) {
        std::cerr << "Error reading MQTT configuration file: " << e.what() << std::endl;
        bRet = false;
    }

    catch (const YAML::ParserException& e) {
        std::cerr << "Error parsing MQTT configuration file: " << e.what() << std::endl;
        bRet = false;
    }

    return bRet;
}

int tPipeComSoConfig::GetDevicesNumber() {
    return static_cast<int>(vModuleListe.size());
}

std::string tPipeComSoConfig::GetDeviceID(int index) {
    if( (index >=0) && ( index < static_cast<int>(vModuleListe.size()))) {
        return vModuleListe[index];
    } else {
        return "";
    }
}
