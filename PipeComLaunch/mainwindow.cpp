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
        }
    } else {
        qDebug() << "Not able to allocate the config class";
    }

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
    bool bRet = true;
    handle = dlopen( InterProcessLib.c_str(), RTLD_LAZY);
    if (!handle) {
        qDebug() << dlerror();
        bRet = false;
    }

    if( bRet == true) {
        //reset errors
        dlerror();
        //load the class for reading
        auto createR = reinterpret_cast<tPipeComRead*(*)(std::string)>(
            dlsym(handle, "create_pipe_com_read")
        );

        if(!createR) {
            qDebug() << dlerror();
            bRet = false;
        }
        else {
            ReadPlugin = createR( App2ServPipeName.c_str());
            if( ReadPlugin == nullptr ) {
                qDebug() << "Not able to create the pipe communication read interface";
                bRet = false;
            }
        }

        if( bRet == true) {
            //reset errors
            dlerror();
            //load the class for writing
            auto createW = reinterpret_cast<tPipeComWrite*(*)(std::string)>(
                dlsym(handle, "create_pipe_com_write")
            );

            if(!createW) {
                qDebug() << dlerror();
                bRet = false;
            }
            else {
                WritePlugin = createW( App2ServPipeName.c_str());
                if( WritePlugin == nullptr ) {
                    qDebug() << "Not able to create the pipe communication write interface";
                    bRet = false;
                }
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
            auto destroy = reinterpret_cast<void(*)(tPipeComRead*)>(
                dlsym(handle, "destroy_pipe_com_read")
            );
            destroy(ReadPlugin);
        }
        if (WritePlugin) {
            auto destroy = reinterpret_cast<void(*)(tPipeComWrite*)>(
                dlsym(handle, "destroy_pipe_com_write")
            );
            destroy(WritePlugin);
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
        std::vector<uint8_t>OutMess;
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

        auto write = reinterpret_cast<bool(*)(tPipeComWrite*, const u_int8_t*, int)>(
            dlsym(handle, "WriteData")
        );
        if(!write) {
            qDebug() << dlerror();
            bContinue = false;
        }

        auto read = reinterpret_cast<bool(*)(tPipeComRead*, std::vector<uint8_t>*)>(
            dlsym(handle, "ReadData")
        );
        if(!read) {
            qDebug() << dlerror();
            bContinue = false;
        }

        if( bContinue == true){
            bContinue = write(WritePlugin, (const u_int8_t*)MessageSelected_std.c_str(), iDataSize);
        }
        if( bContinue == false ) {
            qDebug() << "Not able to write the message";
        } else {
            qDebug() << "Message sent";
            bContinue = read(ReadPlugin, &OutMess);
        }

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
}

