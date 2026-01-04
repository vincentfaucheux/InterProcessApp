#ifndef PIPECOMWRITESO_H
#define PIPECOMWRITESO_H

#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <thread>
#include <unistd.h>

typedef void (*tCbWriteCreated)( void* Ctx_Ptr);

class tPipeComWrite {
public:
    tPipeComWrite(
        std::string PipePath, 
        int* iAllOk_Ptr,
        tCbWriteCreated Cb_Ptr,
        void* Ctx_Ptr
    );
    ~tPipeComWrite();
    bool WriteData(const u_int8_t* u8Data_Ptr, int iDataSize);
    bool IsConnected();

private:
    int fd = -1;
    std::thread threadCreateWrite = std::thread();
    tCbWriteCreated CbWriteCreated = nullptr;
    void* Ctx_Ptr = nullptr;
    std::string PipePath ="";
    bool bConnected = false;
    void connectWriteThread();
};

#endif //PIPECOMWRITESO_H
