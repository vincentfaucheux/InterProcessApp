#ifndef PIPECOMREADSO_H
#define PIPECOMREADSO_H

#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <queue>
#include <mutex>

typedef void (*tCbDataReceived)( void* Ctx_Ptr);

class tPipeComRead{
public:
    tPipeComRead(std::string PipePath, bool* bAllOk_Ptr);
    ~tPipeComRead();
    bool ReadData(std::vector<uint8_t>* OutMess);
    bool IsConnected();
    bool SetCbReceivedData(tCbDataReceived Cb_Ptr, void* Ctx_Ptr);

private:
    int fd = -1;
    std::queue<std::vector<uint8_t>> TmpMess;
    std::mutex mtx;
    std::thread threadRead = std::thread();
    tCbDataReceived CbReceivedData = nullptr;
    void* Ctx_Ptr = nullptr;
    bool bContinueReading = true;
    void receiverThread();

};

#endif //PIPECOMREADSO_H
