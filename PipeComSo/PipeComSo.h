#ifndef PIPECOMSO_H
#define PIPECOMSO_H

#include <string>
#include <iostream>
#include "PipeComSo_interface.h"
#include "PipeComSo_config.h"

class t_PipeCom : public PipeComInterface {
public:
    t_PipeCom(std::string MqttPath, bool* bAllOk_Ptr);
    ~t_PipeCom();
    //int GetDevicesNumber() override;
    //std::string GetDeviceID(int index) override;
    //void Switch(std::string module, 
    //        std::string state) override;

private:
    tPipeComSoConfig* config_Ptr = nullptr;
};

#endif //PIPECOMSO_H
