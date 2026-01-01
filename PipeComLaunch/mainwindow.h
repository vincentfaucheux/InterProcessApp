#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
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
    QComboBox* comboDirections = nullptr;
    QLineEdit *tbMess2Send= nullptr;
    QLabel *labelMess2Receiv = nullptr;
    void* handle = nullptr;
    tPipeComWrite* WritePlugin = nullptr;
    tPipeComRead* ReadPlugin = nullptr;
    tMainConfig* mainConfig_Ptr = nullptr;

    //void commandZigbeeObsolete(const QString& path);
    void commandSend(const QString& DirectionSelected, const QString& MessageSelected);
    void closeInterProcessSo();
    bool openInterProcessSo( 
        std::string InterProcessLib, 
        std::string App2ServPipeName, 
        std::string Serv2AppPipeName 
    );
};
