#ifndef ENDPOINTCOMSO_H
#define ENDPOINTCOMSO_H

#include <string>
#include <iostream>
#include "../PipeComSo/PipeComSo.h"
#include <dlfcn.h>

typedef void (*tCbEndPointDataReceived)( void* Ctx_Ptr);
typedef void (*tCbEndPointWriteCreated)( void* Ctx_Ptr);

class tEndPointCom {
public:
    tEndPointCom(
        std::string AppPipesPath, 
        std::string PipePathWrite, 
        std::string PipePathRead, 
        int* iAllOk_Ptr,
        tCbEndPointWriteCreated Cb_Ptr,
        void* Ctx_Ptr
    );
    ~tEndPointCom();
    bool WriteData(const u_int8_t* u8Data_Ptr, int iDataSize);
    bool ReadData(std::vector<uint8_t>* OutMess);
    bool SetCbReceivedData(tCbEndPointDataReceived Cb_Ptr, void* Ctx_Ptr);

private:
    void* handlePipe = nullptr;
    tPipeComWrite* WritePipe = nullptr;
    tPipeComRead* ReadPipe = nullptr;

    tPipeComWrite*(*createW)(std::string, int*, tCbEndPointWriteCreated, void*) = nullptr;
    tPipeComRead*(*createR)(std::string) = nullptr;
    bool(*setCb)(tPipeComRead*, tCbEndPointDataReceived, void*) = nullptr;
    void(*destroyR)(tPipeComRead*) = nullptr;
    void(*destroyW)(tPipeComWrite*) = nullptr;
    bool(*writePipe)(tPipeComWrite*, const u_int8_t*, int) = nullptr;
    bool(*readPipe)(tPipeComRead*, std::vector<uint8_t>*) = nullptr;
    
};

extern "C" {
    tEndPointCom * create_client_com( 
        std::string AppPipesPath, 
        std::string PipePathWrite, 
        std::string PipePathRead,
        int* iAllOk_Ptr,
        tCbEndPointWriteCreated Cb_Ptr,
        void* Ctx_Ptr);
    void destroy_client_com(tEndPointCom*);
    bool WriteClientData(tEndPointCom* t_Ptr, const u_int8_t* u8Data_Ptr, int iDataSize);
    bool ReadClientData(tEndPointCom* t_Ptr, std::vector<uint8_t>* OutMess);
    bool SetCbClientReadData(tEndPointCom* t_Ptr, tCbEndPointDataReceived Cb_Ptr, void* Ctx_Ptr);

}

#endif //ENDPOINTCOMSO_H
