#ifndef PIPECOMSO_H
#define PIPECOMSO_H

#include <string>
#include <iostream>
#include "./PipeComReadSo.h"
#include "./PipeComWriteSo.h"

extern "C" {
    tPipeComWrite * create_pipe_com_write( std::string PipePath);
    void destroy_pipe_com_write(tPipeComWrite*);
    bool WriteData(tPipeComWrite* t_Ptr, const u_int8_t* u8Data_Ptr, int iDataSize);
    tPipeComRead * create_pipe_com_read( std::string PipePath);
    void destroy_pipe_com_read(tPipeComRead*);
    bool ReadData(tPipeComRead* t_Ptr, std::vector<uint8_t>* OutMess);
    bool SetCbReceivedData(tPipeComRead* t_Ptr, tCbDataReceived Cb_Ptr, void* Ctx_Ptr);

}

#endif //PIPECOMSO_H
