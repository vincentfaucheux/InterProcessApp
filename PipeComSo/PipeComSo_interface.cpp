
#include "PipeComSo_interface.h"
#include "PipeComSo.h"

extern "C" PipeComInterface* create_pipe_com(std::string MqttPath) {
    bool bAllOk = true;
    PipeComInterface* PipeCom_P = new t_PipeCom( MqttPath, &bAllOk);
    if( bAllOk == false ) {
        delete PipeCom_P;
        PipeCom_P = nullptr;
    }

    return( PipeCom_P);
}

extern "C" void destroy_pipe_com(PipeComInterface* p) {
    delete p;
}
