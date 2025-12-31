#ifndef PIPECOMWRITESO_H
#define PIPECOMWRITESO_H

#include <string>
#include <iostream>
#include "PipeComSo_config.h"

class tPipeComWrite {
public:
    tPipeComWrite(std::string MqttPath, bool* bAllOk_Ptr);
    ~tPipeComWrite();
    //int GetDevicesNumber() override;
    //std::string GetDeviceID(int index) override;
    //void Switch(std::string module, 
    //        std::string state) override;

private:
    tPipeComSoConfig* config_Ptr = nullptr;
};

#endif //PIPECOMWRITESO_H
