
#include "PipeComReadSo.h"

tPipeComRead ::tPipeComRead(std::string PipePath, bool* bAllOk_Ptr) {
    //open the named pipe for reading
    fd = open(PipePath.c_str(), O_RDONLY | O_NONBLOCK);
    if( fd == -1) {
        std::cerr << "Error opening pipe for reading: " << PipePath << std::endl;
        *bAllOk_Ptr = false;
    } else {
        bContinueReading = true;
        threadRead = std::thread(&tPipeComRead::receiverThread, this);
        if( threadRead.joinable() == false) {
            *bAllOk_Ptr = false;
        } else {
            // nothing to do
        }
    }
}

tPipeComRead ::~tPipeComRead() {
    bContinueReading = false;
    if( threadRead.joinable())
    {
        threadRead.join();
    }
    if( fd != -1) {
        close(fd);
    }
}

void tPipeComRead ::receiverThread() {
    const int bufferSize = 1024;
    uint8_t buffer[bufferSize];
    bool bDataReceived = false;

    while( bContinueReading) {
        ssize_t bytesRead = read(fd, buffer, bufferSize);
        if( bytesRead > 0) {
            bDataReceived = true;
            std::vector<uint8_t> message(buffer, buffer + bytesRead);
            std::lock_guard<std::mutex> lock(mtx);
            TmpMess.push(message);
        } else {
            //try to invoke callback if data was received previously
            if( bDataReceived == true ) {
                bDataReceived = false;
                //invoke callback if set
                if( CbReceivedData != nullptr ) {
                    CbReceivedData( Ctx_Ptr );
                }
            }
            //no data read, sleep briefly to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

bool tPipeComRead ::ReadData(std::vector<uint8_t>* OutMess_Ptr) {
    std::lock_guard<std::mutex> lock(mtx);
    if( !TmpMess.empty()) {
        *OutMess_Ptr = TmpMess.front();
        TmpMess.pop();
        return true;
    }
    return false;
}

bool tPipeComRead ::IsConnected() {
    return fd != -1;
}

bool tPipeComRead ::SetCbReceivedData(tCbDataReceived Cb_Ptr, void* Ctx_Ptr_in) {
    CbReceivedData = Cb_Ptr;
    Ctx_Ptr = Ctx_Ptr_in;
    return true; //always successful
}