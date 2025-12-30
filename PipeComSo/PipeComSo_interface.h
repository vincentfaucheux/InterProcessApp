
#ifndef PIPECOMSO_INTERFACE_H
#define PIPECOMSO_INTERFACE_H

#include <cstring>
#include <iostream>

class PipeComInterface {
public:
    virtual ~PipeComInterface() = default;
    //virtual int GetDevicesNumber() = 0;
    //virtual std::string GetDeviceID(int index) = 0;
    //virtual void Switch(std::string module, 
    //    std::string state) = 0;
};

extern "C" {
    PipeComInterface* create_pipe_com( std::string MqttPath);
    void destroy_pipe_com(PipeComInterface*);
}

#endif //PIPECOMSO_INTERFACE_H