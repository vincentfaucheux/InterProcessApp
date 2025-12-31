#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <dlfcn.h>
#include "../PipeComSo/PipeComSo.h"
#include <string>
#include "main_config.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private slots:
    //void onPluginChanged(int index);

private:
    QComboBox* comboModules = nullptr;
    QComboBox* comboStates= nullptr;
    void* handle = nullptr;
    tPipeComWrite* WritePlugin = nullptr;
    tPipeComRead* ReadPlugin = nullptr;
    tMainConfig* mainConfig_Ptr = nullptr;

    //void commandZigbeeObsolete(const QString& path);
    void commandZigbee(const QString& ModuleSelected, const QString& StateSelected);
    void closeZigbeeSo();
    bool openZigbeeSo( std::string ZigbeeLib, std::string Zigbee2MqttPath  );
};
