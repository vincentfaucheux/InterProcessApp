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
        if(( mainConfig_Ptr->ConfigData_Ptr->Zigbee2MqttPath != "" ) && 
            ( mainConfig_Ptr->ConfigData_Ptr->ZigbeeAppLib != "" )) {
            //try to open the shared library
            bool bZigbeeSoOk = openZigbeeSo( 
                mainConfig_Ptr->ConfigData_Ptr->ZigbeeAppLib, 
                mainConfig_Ptr->ConfigData_Ptr->Zigbee2MqttPath
            );

            //if nok quit the program
            if( bZigbeeSoOk == false) {
                    qDebug() << "Not able to open the shared library";
            }
            else {

                // create the Window
                auto* central = new QWidget;

                // GroupBox choix des modules
                QGroupBox *groupBoxModules = new QGroupBox("Select module");
                // ComboBox choix des modules
                comboModules = new QComboBox();
                // if( plugin != nullptr ) {
                //     //plugin valid, fill the combo box
                //     int iNum = plugin->GetDevicesNumber();
                //     for( int i=0; i<iNum; i++ ) {
                //         std::string deviceID = plugin->GetDeviceID(i);
                //         comboModules->addItem( QString::fromStdString( deviceID ) );
                //     }
                // }
                // Layout GroupBox choix des modules
                auto* groupModulesLayout = new QVBoxLayout();
                groupModulesLayout->addWidget(comboModules);
                groupBoxModules->setLayout(groupModulesLayout);

                // GroupBox choix des etats
                QGroupBox *groupBoxStates= new QGroupBox("Select state");
                // ComboBox choix des modules
                comboStates = new QComboBox();
                comboStates->addItem("CONFORT");
                comboStates->addItem("ECO");
                comboStates->addItem("HORS GEL");
                comboStates->addItem("OFF");
                // Layout GroupBox choix des modules
                auto* groupStatesLayout = new QVBoxLayout();
                groupStatesLayout->addWidget(comboStates);
                groupBoxStates->setLayout(groupStatesLayout);

                auto* groupsLayout = new QVBoxLayout();
                groupsLayout->addWidget(groupBoxModules);
                groupsLayout->addWidget(groupBoxStates);

                QPushButton *button = new QPushButton("Action Zigbee");

                auto* mainLayout = new QVBoxLayout();
                mainLayout->addLayout(groupsLayout);
                mainLayout->addWidget(button);
                central->setLayout(mainLayout);

                setCentralWidget(central);

                //connect(comboModules, QOverload<int>::of(&QComboBox::currentIndexChanged),
                //        this, &MainWindow::onPluginChanged);

                //onPluginChanged(0);

                QObject::connect(button, &QPushButton::clicked, [&]() {
                    MainWindow::commandZigbee(comboModules->currentText(),
                                comboStates->currentText());
                });
            }
        }
    } else {
        qDebug() << "Not able to allocate the config class";
    }

 }


MainWindow::~MainWindow() {
    //close the zigbee shared library
    closeZigbeeSo();

    //delete the config class
    if( mainConfig_Ptr != nullptr ) {
        delete mainConfig_Ptr;
    }
}

bool MainWindow::openZigbeeSo( std::string ZigbeeLib, std::string Zigbee2MqttPath  ){
    bool bRet = true;
//    handle = dlopen( ZigbeeLib.c_str(), RTLD_LAZY);
//     if (!handle) {
//         qDebug() << dlerror();
//         bRet = false;
//     }

//     if( bRet == true) {
//         auto create = reinterpret_cast<ZigbeeInterface*(*)(std::string)>(
//             dlsym(handle, "create_zigbee")
//         );

//         if(!create) {
//             qDebug() << dlerror();
//             bRet = false;
//         }
//         else {
//             plugin = create( Zigbee2MqttPath);
//             if( plugin == nullptr ) {
//                 qDebug() << "Not able to create the zigbee interface";
//                 bRet = false;
//             }
//         }
//     }

    return bRet;
}

void MainWindow::closeZigbeeSo() {
    // if (plugin && handle) {
    //     auto destroy = reinterpret_cast<void(*)(ZigbeeInterface*)>(
    //         dlsym(handle, "destroy_zigbee")
    //     );
    //     destroy(plugin);
    //     dlclose(handle);
    // }
    // plugin = nullptr;
    // handle = nullptr;
}

//void MainWindow::commandZigbeeObsolete(const QString& path) {
//
//    if( plugin != nullptr ) {
//        qDebug() << "Plugin chargé:" << plugin->name();
//        qDebug() << "Compute:" << plugin->compute(4, 5);
//    }
//}

void MainWindow::commandZigbee(const QString& ModuleSelected, const QString& StateSelected) {
    // //Check if plugin is valid
    // if( plugin == nullptr ) {
    //     //plugin not valid
    //     qDebug() << "Zigbee plugin not loaded";
    // }else {
    //     //plugin valid, execute command
    //     qDebug() << "Action zigbee";
    //     qDebug() << "module: " << ModuleSelected;
    //     qDebug() << "state: " << StateSelected;
    //     std::string ModuleSelected_std = ModuleSelected.toStdString();
    //     std::string StateSelected_std = StateSelected.toStdString();
    //     plugin->Switch(ModuleSelected_std, StateSelected_std);
    // }
}

//void MainWindow::onPluginChanged(int) {
//    commandZigbeeObsolete(comboModules->currentData().toString());
//}