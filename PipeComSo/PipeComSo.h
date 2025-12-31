#ifndef PIPECOMSO_H
#define PIPECOMSO_H

#include <string>
#include <iostream>
#include "PipeComReadSo.h"
#include "PipeComWriteSo.h"

extern "C" {
    tPipeComWrite * create_pipe_com_write( std::string MqttPath);
    void destroy_pipe_com_write(tPipeComWrite*);
    tPipeComRead * create_pipe_com_read( std::string MqttPath);
    void destroy_pipe_com_read(tPipeComRead*);
}

#endif //PIPECOMSO_H
