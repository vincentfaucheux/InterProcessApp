#ifndef PIPECOMSO_H
#define PIPECOMSO_H

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

class tPipeComRead{
public:
    tPipeComRead(std::string MqttPath, bool* bAllOk_Ptr);
    ~tPipeComRead();
    //int GetDevicesNumber() override;
    //std::string GetDeviceID(int index) override;
    //void Switch(std::string module, 
    //        std::string state) override;

private:
    tPipeComSoConfig* config_Ptr = nullptr;
};

extern "C" {
    tPipeComWrite * create_pipe_com_write( std::string MqttPath);
    void destroy_pipe_com_write(tPipeComWrite*);
    tPipeComRead * create_pipe_com_read( std::string MqttPath);
    void destroy_pipe_com_read(tPipeComRead*);
}

#endif //PIPECOMSO_H
