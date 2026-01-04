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
                if( iOpenWriteStatus < 0 ) {
                    qDebug() << "Not able to open the write pipe";
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
    QGroupBox *groupBoxDirections = new QGroupBox("Select direction");
    // ComboBox choix des modules
    comboDirections = new QComboBox();
    comboDirections->addItem("APP -> SERV");
    comboDirections->addItem("SERV -> APP");
    // Layout GroupBox choix des modules
    auto* groupDirectionsLayout = new QVBoxLayout();
    groupDirectionsLayout->addWidget(comboDirections);
    groupBoxDirections->setLayout(groupDirectionsLayout);

    // GroupBox message to send
    QGroupBox *groupBoxMess2Send= new QGroupBox("Message a envoyer");
    //create textb box
    tbMess2Send = new QLineEdit();
    // Layout GroupBox message to send
    auto* groupMess2SendLayout = new QVBoxLayout();
    groupMess2SendLayout->addWidget(tbMess2Send);
    groupBoxMess2Send->setLayout(groupMess2SendLayout);

    // GroupBox message to receive
    QGroupBox *groupBoxMess2Receiv= new QGroupBox("Message recu");
    labelMess2Receiv = new QLabel();
    // Layout GroupBox message to receive
    auto* groupMess2ReceivLayout = new QVBoxLayout();
    groupMess2ReceivLayout->addWidget(labelMess2Receiv);
    groupBoxMess2Receiv->setLayout(groupMess2ReceivLayout);

    auto* groupsLayout = new QVBoxLayout();
    groupsLayout->addWidget(groupBoxDirections);
    groupsLayout->addWidget(groupBoxMess2Send);
    groupsLayout->addWidget(groupBoxMess2Receiv);

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
        MainWindow::commandSend(comboDirections->currentText(),
                    tbMess2Send->text());
    });
}


MainWindow::~MainWindow() {
    //close the inter process shared library
    closeInterProcessSo();

    //delete the config class
    if( mainConfig_Ptr != nullptr ) {
        delete mainConfig_Ptr;
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

        //create the read plugin
        if( bRet == true) {
            ReadPlugin = createR( App2ServPipeName.c_str());
            if( ReadPlugin == nullptr ) {
                qDebug() << "Not able to create the pipe communication read interface";
                bRet = false;
            }
        }

        //Set the callback for received data
        if( bRet == true) {
            tCbDataReceived cb = FromPipeComReadCallback;
            bool bCbOk = setCb( ReadPlugin, cb, (void*)this);
            if( bCbOk == false ) {
                qDebug() << "Not able to set the callback for received data";
                bRet = false;
            }
        }

        //create the write plugin
        if( bRet == true) {
            iOpenWriteStatus = -2;
            WritePlugin = createW( 
                App2ServPipeName.c_str(), 
                &iOpenWriteStatus, 
                FromPipeComWriteCreateCallback, 
                (void*)this
            );
            if( WritePlugin == nullptr ) {
                qDebug() << "Not able to create the pipe communication write interface";
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
        if (ReadPlugin) {
            destroyR(ReadPlugin);
        }
        if (WritePlugin) {
            destroyW(WritePlugin);
        }
        dlclose(handle);
    }
    ReadPlugin = nullptr;
    WritePlugin = nullptr;
    handle = nullptr;
}

void MainWindow::commandSend(const QString& DirectionSelected, const QString& MessageSelected) {
    //Check if plugins is valid
    if(( WritePlugin == nullptr ) &&
       ( ReadPlugin == nullptr )
    ) {
        //plugins not valid
        qDebug() << "plugins for pipe read or write are not valid";
    }else {
        bool bContinue = true;
        //plugins valid, execute command
        qDebug() << "Pipe communication";
        qDebug() << "module: " << DirectionSelected;
        qDebug() << "state: " << MessageSelected;
        std::string DirectionSelected_std = DirectionSelected.toStdString();
        std::string MessageSelected_std = MessageSelected.toStdString();

        //send the message
        int iDataSize = 0;
        char* pData = (char*)MessageSelected_std.c_str();
        while( pData[iDataSize] != '\0' ) {
            iDataSize++;
        }
        iDataSize++; //include the null termination

        bContinue = writePipe(WritePlugin, (const u_int8_t*)MessageSelected_std.c_str(), iDataSize);
        if( bContinue == true){
            qDebug() << "Message sent";
        } else {
            qDebug() << "Not able to write the message";
        }
    }
}

void MainWindow::GetDataFromPipe() {
    bool bContinue = true;
    std::vector<uint8_t>OutMess;

    //Read data from the pipe
    bContinue = readPipe(ReadPlugin, &OutMess);

    //read the response message
    if( bContinue == false ) {
        qDebug() << "Not able to read the response message";
    } else if( OutMess.size() > 0 ) {
        QString ReceivedMessage;
        for( size_t i=0; i< OutMess.size(); i++) {
            if( OutMess[i] != 0 ) {
                ReceivedMessage.append( QChar( OutMess[i] ) );
            }
        }
        labelMess2Receiv->setText( ReceivedMessage );
    } else {
        qDebug() << "Not able to read the response message";
    }
}

void MainWindow::AcknowledgeWriteOpenOk(MainWindow* MainWindow_Ptr) {
    iOpenWriteStatus = 0;
    //MainWindow_Ptr->DisplayMainWindow();
    QMetaObject::invokeMethod(MainWindow_Ptr,
    [this]() {
        DisplayMainWindow();
    },
    Qt::QueuedConnection);

}

static void FromPipeComReadCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->GetDataFromPipe();
    }
}

static void FromPipeComWriteCreateCallback( void* Ctx_Ptr) {
    MainWindow* mainWindow_Ptr = reinterpret_cast<MainWindow*>( Ctx_Ptr);
    if( mainWindow_Ptr != nullptr ) {
        mainWindow_Ptr->AcknowledgeWriteOpenOk(mainWindow_Ptr);
    }
}

