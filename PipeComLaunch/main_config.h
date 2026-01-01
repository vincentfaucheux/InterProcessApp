#pragma once

#include <cstring>
#include <iostream>
#include <yaml-cpp/yaml.h>

class tConfigData{
public:
    tConfigData();
    ~tConfigData();
    std::string InterProcessAppLib;
    std::string App2ServPipeName;
    std::string Serv2AppPipeName;
};

class tMainConfig{
public:
    tMainConfig();
    ~tMainConfig();
    bool LoadConfig();
    tConfigData* ConfigData_Ptr;
private:
};

