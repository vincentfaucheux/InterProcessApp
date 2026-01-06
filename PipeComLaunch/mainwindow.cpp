#include "mainwindow.h" 
#include <QDebug>

MainWindow::MainWindow() {
    //allocate config class
    mainConfig_Ptr = new tMainConfig();
    //if config class allocated
    if( mainConfig_Ptr != nullptr ) {
        //load the configuration file
        bool bConfigOk = mainConfig_Ptr->LoadConfig();
        //if path exists, continue
        if(( bConfigOk == true) && 
            ( mainConfig_Ptr->ConfigData_Ptr->InterProcessAppLib != "" ) &&
            ( mainConfig_Ptr->ConfigData_Ptr->App2ServPipeName != "" ) &&
            ( mainConfig_Ptr->ConfigData_Ptr->Serv2AppPipeName != "" )) {
            //try to open the shared library
            bool bInterProcessSoOk = openInterProcessSo( 
                mainConfig_Ptr->ConfigData_Ptr->InterProcessAppLib,
                mainConfig_Ptr->ConfigData_Ptr->App2ServPipeName,
                mainConfig_Ptr->ConfigData_Ptr->Serv2AppPipeName
            );

            //if nok quit the program
            if( bInterProcessSoOk == false) {
                    qDebug() << "Not able to open the shared library";
            }
            else {
                if(( iOpenRequestWriteStatus < 0) || (iOpenResponsWriteStatus < 0 )) {
                    qDebug() << "Wait for write connection (request or response)";
                } else {
                    DisplayMainWindow();
                }
            }
        }
    } else {
        qDebug() << "Not able to allocate the config class";
    }

 }

 void MainWindow::DisplayMainWindow() {
    // create the Window
    auto* central = new QWidget;

    // GroupBox choix des modules
    QGroupBox *gbCommands = new QGroupBox("Select command");
    // ComboBox choix des modules
    cbCommands = new QComboBox();
    cbCommands->addItem("1");
    cbCommands->addItem("2");
    // Layout GroupBox choix des modules
    auto* groupCommandsLayout = new QVBoxLayout();
    groupCommandsLayout->addWidget(cbCommands);
    gbCommands->setLayout(groupCommandsLayout);
    // GroupBox message to send
    QGroupBox *gbArg2Send= new QGroupBox("Request argument to send");
    //create textb box
    tbArg2Send = new QLineEdit();
    // Layout GroupBox message to send
    auto* groupArg2SendLayout = new QVBoxLayout();
    groupArg2SendLayout->addWidget(tbArg2Send);
    gbArg2Send->setLayout(groupArg2SendLayout);

    // GroupBox request to send
    QGroupBox *gbRequest2Send= new QGroupBox("Request to send");
    labelRequest2Send = new QLabel();
    // Layout GroupBox message to receive
    auto* groupRequest2SentLayout = new QVBoxLayout();
    groupRequest2SentLayout->addWidget(labelRequest2Send);
    gbRequest2Send->setLayout(groupRequest2SentLayout);

    // GroupBox request to receive
    QGroupBox *gbRequest2Get= new QGroupBox("Request to get");
    labelRequest2Get= new QLabel();
    // Layout GroupBox message to receive
    auto* groupRequest2GetLayout = new QVBoxLayout();
    groupRequest2GetLayout->addWidget(labelRequest2Get);
    gbRequest2Get->setLayout(groupRequest2GetLayout);

    // GroupBox response to send
    QGroupBox *gbRespons2Send= new QGroupBox("Response to send");
    labelRespons2Send = new QLabel();
    // Layout GroupBox message to receive
    auto* groupRespons2SentLayout = new QVBoxLayout();
    groupRespons2SentLayout->addWidget(labelRespons2Send);
    gbRespons2Send->setLayout(groupRespons2SentLayout);

    // GroupBox response to receive
    QGroupBox *gbRespons2Get= new QGroupBox("Response to get");
    labelRespons2Get= new QLabel();
    // Layout GroupBox message to receive
    auto* groupRespons2GetLayout = new QVBoxLayout();
    groupRespons2GetLayout->addWidget(labelRespons2Get);
    gbRespons2Get->setLayout(groupRespons2GetLayout);

    auto* groupsLayout = new QVBoxLayout();
    groupsLayout->addWidget(gbCommands);
    groupsLayout->addWidget(gbArg2Send);
    groupsLayout->addWidget(gbRequest2Send);
    groupsLayout->addWidget(gbRequest2Get);
    groupsLayout->addWidget(gbRespons2Send);
    groupsLayout->addWidget(gbRespons2Get);

    QPushButton *button = new QPushButton("Send message");

    auto* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(groupsLayout);
    mainLayout->addWidget(button);
    central->setLayout(mainLayout);

    setCentralWidget(central);

    setWindowTitle("Pipe Communication App");
    resize(400, 300);

    //connect the button
    QObject::connect(button, &QPushButton::clicked, [&]() {
        MainWindow::commandSend(cbCommands->currentText(),
                    tbArg2Send->text());
    });
}


MainWindow::~MainWindow() {
    //close the inter process shared library
    closeInterProcessSo();

    //delete the config class
    if( mainConfig_Ptr != nullptr ) {
        delete mainConfig_Ptr;
    }

    if( Request_Ptr != nullptr ) {
        delete[] Request_Ptr;
    }   
    if( Respons_Ptr != nullptr ) {
        delete[] Respons_Ptr;
    }   
}

bool MainWindow::openInterProcessSo( 
    std::string InterProcessLib, 
    std::string App2ServPipeName, 
    std::string Serv2AppPipeName 
) {
    //init local variables
    bool bRet = true;

    //open the shared library
    handle = dlopen( InterProcessLib.c_str(), RTLD_LAZY);
    if (!handle) {
        qDebug() << dlerror();
        bRet = false;
    }

    //Load the required functions
    if( bRet == true) {
        //load the class for reading
        dlerror();
        createR = reinterpret_cast<tPipeComRead*(*)(std::string)>(
            dlsym(handle, "create_pipe_com_read")
        );
        if(!createR) {
            qDebug() << dlerror();
            bRet = false;
        }

        //Set the callback for received data
        dlerror();
        setCb = reinterpret_cast<bool(*)(tPipeComRead*, tCbDataReceived, void*)>(
            dlsym(handle, "SetCbReceivedData")
        );
        if(!setCb) {
            qDebug() << dlerror();
            bRet = false;
        }

        //load the class for writing
        dlerror();
        createW = reinterpret_cast<tPipeComWrite*(*)(std::string, int*, tCbWriteCreated, void*)>(
            dlsym(handle, "create_pipe_com_write")
        );
        if(!createW) {
            qDebug() << dlerror();
            bRet = false;
        }

        //load the destroyer for writing
        dlerror();
        destroyW = reinterpret_cast<void(*)(tPipeComWrite*)>(
            dlsym(handle, "destroy_pipe_com_write")
        );
        if(!destroyW) {
            qDebug() << dlerror();
            bRet = false;
        }

        //load the destroyer for reading
        dlerror();
        destroyR = reinterpret_cast<void(*)(tPipeComRead*)>(
            dlsym(handle, "destroy_pipe_com_read")
        );
        if(!destroyR) {
            qDebug() << dlerror();
            bRet = false;
        }

        //Load the function to write data
        dlerror();
        writePipe = reinterpret_cast<bool(*)(tPipeComWrite*, const u_int8_t*, int)>(
            dlsym(handle, "WriteData")
        );
        if(!writePipe) {
            qDebug() << dlerror();
            bRet = false;
        }

        //Load the function to read data
        dlerror();
        readPipe = reinterpret_cast<bool(*)(tPipeComRead*, std::vector<uint8_t>*)>(
            dlsym(handle, "ReadData")
        );
        if(!readPipe) {
            qDebug() << dlerror();
            bRet = false;
        }

        //create the read plugin for the request
        if( bRet == true) {
            ReadPipeRequest = createR( App2ServPipeName.c_str());
            if( ReadPipeRequest == nullptr ) {
                qDebug() << "Not able to create the pipe communication read interface for request";
                bRet = false;
            }
        }

        //create the read plugin for the response
        if( bRet == true) {
            ReadPipeRespons = createR( Serv2AppPipeName.c_str());
            if( ReadPipeRespons == nullptr ) {
                qDebug() << "Not able to create the pipe communication read interface for response";
                bRet = false;
            }
        }

        //Set the callback for received data for the request
        if( bRet == true) {
            tCbDataReceived cb = FromPipeRequestReadCallback;
            bool bCbOk = setCb( ReadPipeRequest, cb, (void*)this);
            if( bCbOk == false ) {
                qDebug() << "Not able to set the callback for received data for the request";
                bRet = false;
            }
        }

        //Set the callback for received data for the response
        if( bRet == true) {
            tCbDataReceived cb = FromPipeResponsReadCallback;
            bool bCbOk = setCb( ReadPipeRespons, cb, (void*)this);
            if( bCbOk == false ) {
                qDebug() << "Not able to set the callback for received data for the response";
                bRet = false;
            }
        }

        //create the write plugin for request
        if( bRet == true) {
            iOpenRequestWriteStatus = -2;
            WritePipeRequest = createW( 
                App2ServPipeName.c_str(), 
                &iOpenRequestWriteStatus, 
                FromPipeRequestWriteCreateCallback, 
                (void*)this
            );
            if( WritePipeRequest == nullptr ) {
                qDebug() << "Not able to create the pipe communication write interface for request";
                bRet = false;
            }
        }

        //create the write plugin for response
        if( bRet == true) {
            iOpenResponsWriteStatus = -2;
            WritePipeRespons = createW( 
                Serv2AppPipeName.c_str(), 
                &iOpenResponsWriteStatus, 
                FromPipeResponsWriteCreateCallback, 
                (void*)this
            );
            if( WritePipeRespons == nullptr ) {
                qDebug() << "Not able to create the pipe communication write interface for response";
                bRet = false;
            }
        }

        if( bRet == false ) {
            //clean up
            closeInterProcessSo();
        }

    }

    return bRet;
}

void MainWindow::closeInterProcessSo() {
    if (handle) {
        if (ReadPipeRequest) {
            destroyR(ReadPipeRequest);
        }
        if (WritePipeRequest) {
            destroyW(WritePipeRequest);
        }
        if (ReadPipeRespons) {
            destroyR(ReadPipeRespons);
        }
        if (WritePipeRespons) {
            destroyW(WritePipeRespons);
        }
        dlclose(handle);
    }
    ReadPipeRequest = nullptr;
    WritePipeRequest = nullptr;
    ReadPipeRespons = nullptr;
    WritePipeRespons = nullptr;
    handle = nullptr;
}

void MainWindow::commandSend(const QString& CommandSelected, const QString& CmdArg) {
    //Check if plugins is valid
    if(( WritePipeRequest == nullptr ) &&
       ( ReadPipeRequest == nullptr ) && 
       ( WritePipeRespons == nullptr ) &&
       ( ReadPipeRespons == nullptr )
    ) {
        //plugins not valid
        qDebug() << "plugins for pipe read or write are not valid";
    }else {
        bool bContinue = true;
        //plugins valid, execute command
        qDebug() << "Pipe request";
        qDebug() << "module: " << CommandSelected;
        qDebug() << "state: " << CmdArg;
        std::string CommandSelected_std = CommandSelected.toStdString();
        std::string CmdArg_std = CmdArg.toStdString();

        //send the message
        int iDataSize = 0;
        char* pData = (char*)CmdArg_std.c_str();
        while( pData[iDataSize] != '\0' ) {
            iDataSize++;
        }
        iDataSize +=3; //include the the command and the argument size

        if( Request_Ptr != nullptr ) {
            delete[] Request_Ptr;
        } 
        Request_Ptr = new u_int8_t[iDataSize];
        Request_Ptr[0] = (u_int8_t)std::stoi(CommandSelected_std);
        Request_Ptr[1] = (u_int8_t)((iDataSize -3) /256);
        Request_Ptr[2] = (u_int8_t)((iDataSize -3) %256);
        for( int j=0; j< (int)CmdArg_std.size(); j++) {
            Request_Ptr[3 + j] = (u_int8_t)CmdArg_std[j];
        }

        bContinue = writePipe(WritePipeRequest, (const u_int8_t*)Request_Ptr, iDataSize);
        if( bContinue == true){
            qDebug() << "Request sent";
            std::string label_std = "Command: " + CommandSelected_std + 
            " size: " + std::to_string(iDataSize -3) +
            " Argument: " + CmdArg_std;
            labelRequest2Send->setText( QString::fromStdString(label_std) );
        } else {
            qDebug() << "Not able to write the message";
        }
    }
}

void MainWindow::GetDataFromPipeRequest() {
    bool bContinue = true;
    std::vector<uint8_t>OutMess;

    //Read data from the pipe
    bContinue = readPipe(ReadPipeRequest, &OutMess);

    //read the response message
    if( bContinue == false ) {
        qDebug() << "Not able to read the response message";
    } else if( OutMess.size() > 3 ) {
        //display the request message
        std::string label_std = "Command: " + std::to_string(OutMess[0]) + 
            " size: " + std::to_string((OutMess[1] * 256) + OutMess[2]) +
            " Argument: " + std::string((char*)&OutMess[3], OutMess.size() - 3);
        labelRequest2Get->setText( QString::fromStdString(label_std) );

        //build the response message
        OutMess[0] += 0x40; //response command
        //send the response message
        if( Respons_Ptr != nullptr ) {
            delete[] Respons_Ptr;
        } 
        Respons_Ptr = new u_int8_t[(int)OutMess.size()];
        for( int j=0; j< (int)OutMess.size(); j++) {
            Respons_Ptr[j] = OutMess[j];
        }
        bContinue = writePipe(WritePipeRespons, (const u_int8_t*)Respons_Ptr, (int)OutMess.size());
        if( bContinue == true){
            qDebug() << "Response sent";
            //display the response message
            label_std = "Command: " + std::to_string(OutMess[0]) + 
                " size: " + std::to_string(OutMess.size() - 3) +
                " Argument: " + std::string((char*)&OutMess[3], OutMess.size() - 3);
            labelRespons2Send->setText( QString::fromStdString(label_std) );
        } else {
            qDebug() << "Not able to write the response message";
        }
    } else {
        qDebug() << "Not able to read the response message";
    }
}

void MainWindow::AcknowledgePipeRequestWriteOpenOk(MainWindow* MainWindow_Ptr) {
    iOpenRequestWriteStatus = 0;
    if(( iOpenRequestWriteStatus == 0) && (iOpenResponsWriteStatus == 0 )) {
        QMetaObject::invokeMethod(MainWindow_Ptr,
            [this]() {
                DisplayMainWindow();
            },
            Qt::QueuedConnection);
    }
}
void MainWindow::AcknowledgePipeResponsWriteOpenOk(MainWindow* MainWindow_Ptr) {
    iOpenResponsWriteStatus = 0;
    if(( iOpenRequestWriteStatus == 0) && (iOpenResponsWriteStatus == 0 )) {
        QMetaObject::invokeMethod(MainWindow_Ptr,
            [this]() {
                DisplayMainWindow();
            },
            Qt::QueuedConnection);
    }
}

void MainWindow::GetDataFromPipeRespons() {
    bool bContinue = true;
    std::vector<uint8_t>OutMess;

    //Read data from the pipe
    bContinue = readPipe(ReadPipeRespons, &OutMess);

    //read the response message
    if( bContinue == false ) {
        qDebug() << "Not able to read the response message";
    } else if( OutMess.size() > 3 ) {
        //display the response message
        std::string label_std = "Command: " + std::to_string(OutMess[0]) + 
            " size: " + std::to_string(OutMess[1] * 256 + OutMess[2]) +
            " Argument: " + std::string((char*)&OutMess[3], OutMess.size() - 3);
        labelRespons2Get->setText( QString::fromStdString(label_std) );
    } else {
        qDebug() << "Not able to read the response message";
    }
}
static void FromPipeRequestReadCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->GetDataFromPipeRequest();
    }
}

static void FromPipeRequestWriteCreateCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->AcknowledgePipeRequestWriteOpenOk(mainWindow_Ptr);
    }
}

static void FromPipeResponsReadCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->GetDataFromPipeRespons();
    }
}

static void FromPipeResponsWriteCreateCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->AcknowledgePipeResponsWriteOpenOk(mainWindow_Ptr);
    }
}
