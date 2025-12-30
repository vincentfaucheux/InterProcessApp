#pragma once

#include <cstring>
#include <iostream>
#include <yaml-cpp/yaml.h>

class tConfigData{
public:
    tConfigData();
    ~tConfigData();
    std::string Zigbee2MqttPath;
    std::string ZigbeeAppLib;
};

class tMainConfig{
public:
    tMainConfig();
    ~tMainConfig();
    bool LoadConfig();
    tConfigData* ConfigData_Ptr;
private:
};

