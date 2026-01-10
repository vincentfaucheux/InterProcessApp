
#include "PipeComWriteSo.h"



tPipeComWrite ::tPipeComWrite(
    std::string PipePath_in, 
    int* iAllOk_Ptr,
    tCbWriteCreated Cb_in_Ptr,
    void* Ctx_in_Ptr
) {
    *iAllOk_Ptr = -2;
    bRequestConnect = false;
    PipePath = PipePath_in;
    CbWriteCreated = Cb_in_Ptr;
    Ctx_Ptr = Ctx_in_Ptr;
    fd = open(PipePath.c_str(), O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        if(errno == ENXIO) {
            std::cerr << "No reader connected to the pipe: " << PipePath << std::endl;
            bRequestConnect = true;
            *iAllOk_Ptr = -1;
            //Launch a thread to wait for the reader?
            threadCreateWrite = std::thread( &tPipeComWrite::connectWriteThread, this);
        } else {
            perror("open");
        }
    } else {
        *iAllOk_Ptr = 0;
    }
}

void tPipeComWrite ::connectWriteThread() {
    // Implementation for connecting write thread if needed
    while((bRequestConnect == true) && ( fd < 0 )) {
        fd = open(PipePath.c_str(), O_WRONLY | O_NONBLOCK);
        if( fd >= 0 ) {
            bRequestConnect = false;
            //notify that the write pipe is created
            if( CbWriteCreated != nullptr ) {
                CbWriteCreated( Ctx_Ptr );
            }
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

tPipeComWrite ::~tPipeComWrite() {
    if( threadCreateWrite.joinable())
    {
        threadCreateWrite.join();
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

bool tPipeComWrite ::WriteData(const u_int8_t* u8Data_Ptr, int iDataSize) {
    bool bRet = true;
    if (fd < 0) {
        std::cerr << "Pipe not connected for writing." << std::endl;
        bRet = false;
    } else {
        ssize_t bytesWritten = write(fd, u8Data_Ptr, iDataSize);
        if (bytesWritten < 0) {
            if (errno == EAGAIN) {
                std::cerr << "Pipe plein ou pas de reader.\n";
            } else {
                perror("write");
            }
            bRet = false;
        } else if (static_cast<size_t>(bytesWritten) != iDataSize) {
            std::cerr << "Partial write occurred." << std::endl;
            bRet = false;
        } else {
            // Successfully written all data
            std::cout << "All Bytes sent" << std::endl;
        }
    }

    return bRet;
}

bool tPipeComWrite ::IsConnected() {
    return (fd >= 0);
}
