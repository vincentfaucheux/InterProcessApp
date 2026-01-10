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
            ( mainConfig_Ptr->ConfigData_Ptr->InterProcessPipesLib != "" ) &&
            ( mainConfig_Ptr->ConfigData_Ptr->InterProcessClientLib != "" ) &&
            ( mainConfig_Ptr->ConfigData_Ptr->InterProcessServerLib != "" ) &&
            ( mainConfig_Ptr->ConfigData_Ptr->App2ServPipeName != "" ) &&
            ( mainConfig_Ptr->ConfigData_Ptr->Serv2AppPipeName != "" )) {
            //try to open the shared library
            bool bInterProcessSoOk = openInterProcessSo( 
                mainConfig_Ptr->ConfigData_Ptr->InterProcessPipesLib,
                mainConfig_Ptr->ConfigData_Ptr->InterProcessClientLib,
                mainConfig_Ptr->ConfigData_Ptr->InterProcessServerLib,
                mainConfig_Ptr->ConfigData_Ptr->App2ServPipeName,
                mainConfig_Ptr->ConfigData_Ptr->Serv2AppPipeName
            );

            //if nok quit the program
            if( bInterProcessSoOk == false) {
                    qDebug() << "Not able to open the shared library";
            }
            else {
                if(( iOpenClientWriteStatus < 0) || (iOpenServerWriteStatus < 0 )) {
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
    std::string InterProcessPipeLib, 
    std::string InterProcessClientLib, 
    std::string InterProcessServerLib, 
    std::string App2ServPipeName, 
    std::string Serv2AppPipeName 
) {
    //init local variables
    bool bRet = true;

    //open the shared library for client communication
    handleClient = dlopen( InterProcessClientLib.c_str(), RTLD_LAZY);
    if (!handleClient) {
        qDebug() << dlerror();
        bRet = false;
    }

    if( bRet == true) {
        //load the class for client communication
        dlerror();
        EndPointCreate = reinterpret_cast<tEndPointCom*(*)(std::string, std::string, std::string, int*, tCbEndPointWriteCreated, void*)>(
            dlsym(handleClient, "create_client_com")
        );
        if(!EndPointCreate) {
            qDebug() << dlerror();
            bRet = false;
        }

        //load the destroyer for the client
        dlerror();
        EndPointDestroy = reinterpret_cast<void(*)(tEndPointCom*)>(
            dlsym(handleClient, "destroy_client_com")
        );
        if(!EndPointDestroy) {
            qDebug() << dlerror();
            bRet = false;
        }

        //Set the callback for client communication
        dlerror();
        EndPointSetReadCb = reinterpret_cast<bool(*)(tEndPointCom*, tCbEndPointDataReceived, void*)>(
            dlsym(handleClient, "SetCbClientReadData")
        );
        if(!EndPointSetReadCb) {
            qDebug() << dlerror();
            bRet = false;
        }

        //Load the function to write to the client
        dlerror();
        EndPointWrite = reinterpret_cast<bool(*)(tEndPointCom*, const u_int8_t*, int)>(
            dlsym(handleClient, "WriteClientData")
        );
        if(!EndPointWrite) {
            qDebug() << dlerror();
            bRet = false;
        }

        //Load the function to read to the client
        dlerror();
        EndPointRead = reinterpret_cast<bool(*)(tEndPointCom*, std::vector<uint8_t>*)>(
            dlsym(handleClient, "ReadClientData")
        );
        if(!EndPointRead) {
            qDebug() << dlerror();
            bRet = false;
        }

        //create the client communication
        if( bRet == true) {
            iOpenClientWriteStatus = -2;
            ClientCom = EndPointCreate( 
                InterProcessPipeLib.c_str(), 
                App2ServPipeName.c_str(), 
                Serv2AppPipeName.c_str(), 
                &iOpenClientWriteStatus, 
                FromClientWriteCreateCallback, 
                this);
            if( ClientCom == nullptr ) {
                qDebug() << "Not able to create the client communication";
                bRet = false;
            }
        }

        //Set the callback for received data for the response
        if( bRet == true) {
            tCbEndPointDataReceived cb = FromClientReadCallback;
            bool bCbOk = EndPointSetReadCb( ClientCom, cb, (void*)this);
            if( bCbOk == false ) {
                qDebug() << "Not able to set the callback for received data for the response";
                bRet = false;
            }
        }

        //create the server communication
        if( bRet == true) {
            iOpenServerWriteStatus = -2;
            ServerCom = EndPointCreate( 
                InterProcessPipeLib.c_str(), 
                Serv2AppPipeName.c_str(), 
                App2ServPipeName.c_str(), 
                &iOpenServerWriteStatus, 
                FromServerWriteCreateCallback, 
                this);
            if( ServerCom == nullptr ) {
                qDebug() << "Not able to create the server communication";
                bRet = false;
            }
        }

        //Set the callback for received data for the request
        if( bRet == true) {
            tCbEndPointDataReceived cb = FromServerReadCallback;
            bool bCbOk = EndPointSetReadCb( ServerCom, cb, (void*)this);
            if( bCbOk == false ) {
                qDebug() << "Not able to set the callback for received data for the request";
                bRet = false;
            }
        }

    }

    return bRet;
}

void MainWindow::closeInterProcessSo() {
    if( handleClient ) {
        if( EndPointDestroy && ClientCom ) {
            EndPointDestroy( ClientCom );
        }
        if( EndPointDestroy && ServerCom ) {
            EndPointDestroy( ServerCom );
        }
        ClientCom = nullptr;
        ServerCom = nullptr;
        dlclose(handleClient);
    }
    handleClient = nullptr;
}

void MainWindow::commandSend(const QString& CommandSelected, const QString& CmdArg) {
    //Check if plugins is valid
    if(( ClientCom== nullptr ) ||
       ( ServerCom == nullptr ) 
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

        bContinue = EndPointWrite(ClientCom, (const u_int8_t*)Request_Ptr, iDataSize);
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

void MainWindow::GetDataFromClient() {
    bool bContinue = true;
    std::vector<uint8_t>OutMess;

    //Read data from the pipe
    bContinue = EndPointRead(ClientCom, &OutMess);

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

void MainWindow::GetDataFromServer() {
    bool bContinue = true;
    std::vector<uint8_t>OutMess;

    //Read data from the pipe
    bContinue = EndPointRead(ServerCom, &OutMess);

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
        bContinue = EndPointWrite(ServerCom, (const u_int8_t*)Respons_Ptr, (int)OutMess.size());
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

void MainWindow::AcknowledgeClientWriteOpenOk(MainWindow* MainWindow_Ptr) {
    iOpenClientWriteStatus = 0;
    if(( iOpenClientWriteStatus >= 0) && (iOpenServerWriteStatus >= 0 )) {
        QMetaObject::invokeMethod(MainWindow_Ptr,
            [this]() {
                DisplayMainWindow();
            },
            Qt::QueuedConnection);
    }
}
void MainWindow::AcknowledgeServerWriteOpenOk(MainWindow* MainWindow_Ptr) {
    iOpenServerWriteStatus = 0;
    if(( iOpenClientWriteStatus >= 0) && (iOpenServerWriteStatus >= 0 )) {
        QMetaObject::invokeMethod(MainWindow_Ptr,
            [this]() {
                DisplayMainWindow();
            },
            Qt::QueuedConnection);
    }
}

static void FromClientWriteCreateCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->AcknowledgeClientWriteOpenOk(mainWindow_Ptr);
    }
}

static void FromClientReadCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->GetDataFromClient();
    }
}

static void FromServerWriteCreateCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->AcknowledgeServerWriteOpenOk(mainWindow_Ptr);
    }
}

static void FromServerReadCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->GetDataFromServer();
    }
}
