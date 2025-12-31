
#include "PipeComSo.h"

extern "C" tPipeComWrite* create_pipe_com_write(std::string MqttPath) {
    bool bAllOk = true;
    tPipeComWrite* PipeComWrite_Ptr = new tPipeComWrite( MqttPath, &bAllOk);
    if( bAllOk == false ) {
        delete PipeComWrite_Ptr;
        PipeComWrite_Ptr = nullptr;
    }

    return( PipeComWrite_Ptr);
}

extern "C" void destroy_pipe_com_write(tPipeComWrite* p) {
    delete p;
}

extern "C" tPipeComRead* create_pipe_com_read(std::string MqttPath) {
    bool bAllOk = true;
    tPipeComRead* PipeComRead_Ptr = new tPipeComRead( MqttPath, &bAllOk);
    if( bAllOk == false ) {
        delete PipeComRead_Ptr;
        PipeComRead_Ptr = nullptr;
    }

    return( PipeComRead_Ptr);
}

extern "C" void destroy_pipe_com_read(tPipeComRead* p) {
    delete p;
}
