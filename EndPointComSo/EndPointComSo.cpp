
#include "EndPointComSo.h"

extern "C" tEndPointCom* create_client_com(
        std::string AppPipesPath, 
        std::string PipePathWrite, 
        std::string PipePathRead,
        int* iAllOk_Ptr,
        tCbEndPointWriteCreated Cb_Ptr,
        void* Ctx_Ptr
    ) {
    tEndPointCom* EndPointCom_Ptr = new tEndPointCom( AppPipesPath, PipePathWrite, PipePathRead, iAllOk_Ptr, Cb_Ptr, Ctx_Ptr);
    if( *iAllOk_Ptr < -1 ) {
        delete EndPointCom_Ptr;
        EndPointCom_Ptr = nullptr;
    }

    return( EndPointCom_Ptr);
}

extern "C" void destroy_client_com(tEndPointCom* p) {
    delete p;
}

extern "C" bool WriteClientData(tEndPointCom* t_Ptr, const u_int8_t* u8Data_Ptr, int iDataSize) {
    return t_Ptr->WriteData( u8Data_Ptr, iDataSize );
}

extern "C" bool ReadClientData(tEndPointCom* t_Ptr, std::vector<uint8_t>* OutMess) {
    return t_Ptr->ReadData( OutMess );
}

extern "C" bool SetCbClientReadData(tEndPointCom* t_Ptr, tCbEndPointDataReceived Cb_Ptr, void* Ctx_Ptr) {
    return t_Ptr->SetCbReceivedData( Cb_Ptr, Ctx_Ptr );
}

tEndPointCom::tEndPointCom(
        std::string AppPipesPath, 
        std::string PipePathWrite, 
        std::string PipePathRead, 
        int* iAllOk_Ptr,
        tCbEndPointWriteCreated Cb_Ptr,
        void* Ctx_Ptr
) {
   //init local variables
    bool bRet = true;

    //open the shared library for pipes
    handlePipe = dlopen( AppPipesPath.c_str(), RTLD_LAZY);
    if (!handlePipe) {
        std::cerr << dlerror() << std::endl;
        bRet = false;
    }

    //Load the required functions
    if( bRet == true) {
        //load the class for reading
        dlerror();
        createR = reinterpret_cast<tPipeComRead*(*)(std::string)>(
            dlsym(handlePipe, "create_pipe_com_read")
        );
        if(!createR) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //load the class for writing
        dlerror();
        createW = reinterpret_cast<tPipeComWrite*(*)(std::string, int*, tCbWriteCreated, void*)>(
            dlsym(handlePipe, "create_pipe_com_write")
        );
        if(!createW) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //load the destroyer for writing
        dlerror();
        destroyW = reinterpret_cast<void(*)(tPipeComWrite*)>(
            dlsym(handlePipe, "destroy_pipe_com_write")
        );
        if(!destroyW) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //load the destroyer for reading
        dlerror();
        destroyR = reinterpret_cast<void(*)(tPipeComRead*)>(
            dlsym(handlePipe, "destroy_pipe_com_read")
        );
        if(!destroyR) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //Set the callback for received data
        dlerror();
        setCb = reinterpret_cast<bool(*)(tPipeComRead*, tCbDataReceived, void*)>(
            dlsym(handlePipe, "SetCbReceivedData")
        );
        if(!setCb) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //Load the function to write data
        dlerror();
        writePipe = reinterpret_cast<bool(*)(tPipeComWrite*, const u_int8_t*, int)>(
            dlsym(handlePipe, "WriteData")
        );
        if(!writePipe) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //Load the function to read data
        dlerror();
        readPipe = reinterpret_cast<bool(*)(tPipeComRead*, std::vector<uint8_t>*)>(
            dlsym(handlePipe, "ReadData")
        );
        if(!readPipe) {
            std::cerr << dlerror() << std::endl;
            bRet = false;
        }

        //create the write plugin for request
        if( bRet == true) {
            *iAllOk_Ptr = -2;
            WritePipe = createW( 
                PipePathWrite.c_str(), 
                iAllOk_Ptr, 
                (tCbWriteCreated) Cb_Ptr, 
                Ctx_Ptr
            );
            if( WritePipe == nullptr ) {
                std::cerr << "Not able to create the pipe communication write interface for request" << std::endl;
                bRet = false;
            }
        }

        //create the read plugin for the request
        if( bRet == true) {
            ReadPipe = createR( PipePathRead.c_str());
            if( ReadPipe == nullptr ) {
                std::cerr << "Not able to create the pipe communication read interface for request" << std::endl;
                bRet = false;
            }
        }
    }
}

tEndPointCom::~tEndPointCom() {
    if (handlePipe) {
        if (ReadPipe) {
            destroyR(ReadPipe);
        }
        if (WritePipe) {
            destroyW(WritePipe);
        }
         dlclose(handlePipe);
    }
    ReadPipe = nullptr;
    WritePipe = nullptr;
    handlePipe = nullptr;
}

bool tEndPointCom::WriteData(const u_int8_t* u8Data_Ptr, int iDataSize) {
    bool bRet = true;
    bRet = writePipe(WritePipe, u8Data_Ptr, iDataSize);
    if( bRet == true){
        std::string Debug_std = "Command: " + std::to_string(u8Data_Ptr[0]) + 
            " size: " + std::to_string((u8Data_Ptr[1] * 256) + u8Data_Ptr[2]) +
            " Argument: " + std::string((char*)&u8Data_Ptr[3], iDataSize - 3);
        std::cout << Debug_std << std::endl;
    } else {
        std::cerr << "Not able to write the message" << std::endl;
    }
    return bRet;
}

bool tEndPointCom::ReadData(std::vector<uint8_t>* OutMess) {
    bool bRet = true;
    bRet = readPipe(ReadPipe, OutMess);
    if( bRet == true){
        std::string Debug_std = "Command: " + std::to_string((*OutMess)[0]) + 
            " size: " + std::to_string(((*OutMess)[1] * 256) + (*OutMess)[2]) +
            " Argument: " + std::string((char*)&(*OutMess)[3], OutMess->size() - 3);
        std::cout << Debug_std << std::endl;
    } else {
        std::cerr << "Not able to read the message" << std::endl;
    }
    return bRet;
}

bool tEndPointCom::SetCbReceivedData(tCbEndPointDataReceived Cb_Ptr, void* Ctx_Ptr) {
    bool bRet = true;
    //Set the callback for received data for the response
    if( ReadPipe != nullptr) {
        tCbDataReceived cb = (tCbDataReceived)Cb_Ptr;
        bool bCbOk = setCb( ReadPipe, cb, Ctx_Ptr);
        if( bCbOk == false ) {
            std::cerr << "Not able to set the callback for received data for the response" << std::endl;
            bRet = false;
        }
    }
    return bRet;
}