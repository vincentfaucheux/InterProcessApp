#ifndef PIPECOMWRITESO_H
#define PIPECOMWRITESO_H

#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>


class tPipeComWrite {
public:
    tPipeComWrite(std::string PipePath, bool* bAllOk_Ptr);
    ~tPipeComWrite();
    bool WriteData(const u_int8_t* u8Data_Ptr, int iDataSize);
    bool IsConnected();

private:
    int fd = -1;
};

#endif //PIPECOMWRITESO_H
