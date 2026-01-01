
#include "PipeComWriteSo.h"



tPipeComWrite ::tPipeComWrite(std::string PipePath, bool* bAllOk_Ptr) {
    *bAllOk_Ptr = true;
    fd = open(PipePath.c_str(), O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        *bAllOk_Ptr = false;
    }
}

tPipeComWrite ::~tPipeComWrite() {
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
