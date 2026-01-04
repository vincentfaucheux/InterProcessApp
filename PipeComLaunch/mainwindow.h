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

static void FromPipeComReadCallback( void* Ctx_Ptr);
static void FromPipeComWriteCreateCallback( void* Ctx_Ptr);

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    void GetDataFromPipe();
    void AcknowledgeWriteOpenOk(MainWindow* MainWindow_Ptr);

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
    tPipeComWrite*(*createW)(std::string, int*, tCbWriteCreated, void*) = nullptr;
    tPipeComRead*(*createR)(std::string) = nullptr;
    bool(*setCb)(tPipeComRead*, tCbDataReceived, void*) = nullptr;
    void(*destroyR)(tPipeComRead*) = nullptr;
    void(*destroyW)(tPipeComWrite*) = nullptr;
    bool(*writePipe)(tPipeComWrite*, const u_int8_t*, int) = nullptr;
    bool(*readPipe)(tPipeComRead*, std::vector<uint8_t>*) = nullptr;
    int iOpenWriteStatus = -2;

    //void commandZigbeeObsolete(const QString& path);
    void commandSend(const QString& DirectionSelected, const QString& MessageSelected);
    void closeInterProcessSo();
    bool openInterProcessSo( 
        std::string InterProcessLib, 
        std::string App2ServPipeName, 
        std::string Serv2AppPipeName 
    );
    void DisplayMainWindow();
};
