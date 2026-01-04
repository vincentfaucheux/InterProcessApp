
#include "PipeComSo.h"

extern "C" tPipeComWrite* create_pipe_com_write(
        std::string PipePath, 
        int* iAllOk_Ptr,
        tCbWriteCreated Cb_Ptr,
        void* Ctx_Ptr
    ) {
    tPipeComWrite* PipeComWrite_Ptr = new tPipeComWrite( PipePath, iAllOk_Ptr, Cb_Ptr, Ctx_Ptr);
    if( *iAllOk_Ptr < -1 ) {
        delete PipeComWrite_Ptr;
        PipeComWrite_Ptr = nullptr;
    }

    return( PipeComWrite_Ptr);
}

extern "C" void destroy_pipe_com_write(tPipeComWrite* p) {
    delete p;
}

extern "C" bool WriteData(tPipeComWrite* t_Ptr, const u_int8_t* u8Data_Ptr, int iDataSize) {
    return t_Ptr->WriteData( u8Data_Ptr, iDataSize );
}

extern "C" tPipeComRead* create_pipe_com_read(std::string PipePath) {
    bool bAllOk = true;
    tPipeComRead* PipeComRead_Ptr = new tPipeComRead( PipePath, &bAllOk);
    if( bAllOk == false ) {
        delete PipeComRead_Ptr;
        PipeComRead_Ptr = nullptr;
    }

    return( PipeComRead_Ptr);
}

extern "C" void destroy_pipe_com_read(tPipeComRead* p) {
    delete p;
}

extern "C" bool ReadData(tPipeComRead* t_Ptr, std::vector<uint8_t>* OutMess) {
    return t_Ptr->ReadData( OutMess );
}

extern "C" bool SetCbReceivedData(tPipeComRead* t_Ptr, tCbDataReceived Cb_Ptr, void* Ctx_Ptr) {
    return t_Ptr->SetCbReceivedData( Cb_Ptr, Ctx_Ptr );
}
