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

static void FromPipeRequestReadCallback( void* Ctx_Ptr);
static void FromPipeResponsReadCallback( void* Ctx_Ptr);
static void FromPipeRequestWriteCreateCallback( void* Ctx_Ptr);
static void FromPipeResponsWriteCreateCallback( void* Ctx_Ptr);

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    void GetDataFromPipeRequest();
    void GetDataFromPipeRespons();
    void AcknowledgePipeRequestWriteOpenOk(MainWindow* MainWindow_Ptr);
    void AcknowledgePipeResponsWriteOpenOk(MainWindow* MainWindow_Ptr);

private slots:
    //void onPluginChanged(int index);

private:
    QComboBox* cbCommands = nullptr;
    QLineEdit *tbArg2Send= nullptr;
    QLabel *labelRequest2Send = nullptr;
    QLabel *labelRequest2Get = nullptr;
    QLabel *labelRespons2Send = nullptr;
    QLabel *labelRespons2Get = nullptr;
    void* handle = nullptr;
    tPipeComWrite* WritePipeRequest = nullptr;
    tPipeComRead* ReadPipeRequest = nullptr;
    tPipeComWrite* WritePipeRespons = nullptr;
    tPipeComRead* ReadPipeRespons = nullptr;
    tMainConfig* mainConfig_Ptr = nullptr;
    tPipeComWrite*(*createW)(std::string, int*, tCbWriteCreated, void*) = nullptr;
    tPipeComRead*(*createR)(std::string) = nullptr;
    bool(*setCb)(tPipeComRead*, tCbDataReceived, void*) = nullptr;
    void(*destroyR)(tPipeComRead*) = nullptr;
    void(*destroyW)(tPipeComWrite*) = nullptr;
    bool(*writePipe)(tPipeComWrite*, const u_int8_t*, int) = nullptr;
    bool(*readPipe)(tPipeComRead*, std::vector<uint8_t>*) = nullptr;
    int iOpenRequestWriteStatus = -2;
    int iOpenResponsWriteStatus = -2;
    u_int8_t* Request_Ptr = nullptr;
    u_int8_t* Respons_Ptr = nullptr;

    //void commandZigbeeObsolete(const QString& path);
    void commandSend(const QString& CommandSelected, const QString& CmdArg);
    void closeInterProcessSo();
    bool openInterProcessSo( 
        std::string InterProcessLib, 
        std::string App2ServPipeName, 
        std::string Serv2AppPipeName 
    );
    void DisplayMainWindow();
};
