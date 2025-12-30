#ifndef PIPECOMSO_CONFIG_H
#define PIPECOMSO_CONFIG_H

#include <cstring>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <vector>

class tPipeComSoConfig{
public:
    tPipeComSoConfig();
    ~tPipeComSoConfig();
    bool LoadConfig(std::string ConfigPath);
    int GetDevicesNumber();
    std::string GetDeviceID(int index);

private:
    std::vector<std::string> vModuleListe ;
};

#endif //PIPECOMSO_CONFIG_H