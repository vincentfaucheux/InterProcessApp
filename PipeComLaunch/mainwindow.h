#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <dlfcn.h>
#include "../EndPointComSo/EndPointComSo.h"
#include <string>
#include "main_config.h"

static void FromServerReadCallback( void* Ctx_Ptr);
static void FromClientReadCallback( void* Ctx_Ptr);
static void FromClientWriteCreateCallback( void* Ctx_Ptr);
static void FromServerWriteCreateCallback( void* Ctx_Ptr);

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    void GetDataFromClient();
    void GetDataFromServer();
    void AcknowledgeClientWriteOpenOk(MainWindow* MainWindow_Ptr);
    void AcknowledgeServerWriteOpenOk(MainWindow* MainWindow_Ptr);

private slots:
    //void onPluginChanged(int index);

private:
    QComboBox* cbCommands = nullptr;
    QLineEdit *tbArg2Send= nullptr;
    QLabel *labelRequest2Send = nullptr;
    QLabel *labelRequest2Get = nullptr;
    QLabel *labelRespons2Send = nullptr;
    QLabel *labelRespons2Get = nullptr;

    void* handleClient = nullptr;
    void* handleServer = nullptr;

    tEndPointCom* ClientCom = nullptr;
    tEndPointCom* ServerCom = nullptr;

    tMainConfig* mainConfig_Ptr = nullptr;

    tEndPointCom*(*EndPointCreate)(std::string, std::string, std::string, int*, tCbEndPointWriteCreated, void*) = nullptr;
    void(*EndPointDestroy)(tEndPointCom*) = nullptr;
    bool(*EndPointSetReadCb)(tEndPointCom*, tCbEndPointDataReceived, void*) = nullptr;
    bool(*EndPointWrite)(tEndPointCom*, const u_int8_t*, int) = nullptr;
    bool(*EndPointRead)(tEndPointCom*, std::vector<uint8_t>*) = nullptr;

    int iOpenClientWriteStatus = -2;
    int iOpenServerWriteStatus = -2;

    u_int8_t* Request_Ptr = nullptr;
    u_int8_t* Respons_Ptr = nullptr;

    //void commandZigbeeObsolete(const QString& path);
    void commandSend(const QString& CommandSelected, const QString& CmdArg);
    void closeInterProcessSo();
    bool openInterProcessSo( 
        std::string InterProcessPipeLib, 
        std::string InterProcessClientLib, 
        std::string InterProcessServerLib, 
        std::string App2ServPipeName, 
        std::string Serv2AppPipeName 
    );
    void DisplayMainWindow();
};
