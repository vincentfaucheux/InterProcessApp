#include "main_config.h"

tConfigData::tConfigData(){
    InterProcessPipesLib = "";
    InterProcessClientLib = "";
    InterProcessServerLib = "";
    App2ServPipeName = "";
    Serv2AppPipeName = "";
}

tConfigData::~tConfigData(){
}

tMainConfig::tMainConfig(){
    ConfigData_Ptr = new tConfigData();
}

tMainConfig::~tMainConfig(){
    if( ConfigData_Ptr != nullptr ) {
        delete ConfigData_Ptr;
    }
}

bool tMainConfig::LoadConfig(){
    //init return code
    bool bRet = true;

    //check data pointer is valid
    if( ConfigData_Ptr != nullptr ) {
        //data pointer is valid, load yaml file
        try {
            YAML::Node YamlConfig = YAML::LoadFile( "configuration.yaml");

            //read interprocess pipes lib
            if( YamlConfig["interprocess_pipes_lib"] ) {
                ConfigData_Ptr->InterProcessPipesLib = YamlConfig["interprocess_pipes_lib"].as<std::string>();
                std::cout << "interprocess_pipes_lib:" << ConfigData_Ptr->InterProcessPipesLib << std::endl;
            } else {
                std::cout << "interprocess_pipe_lib not found in configuration.yaml" << std::endl;
                bRet = false;
            }

            //read interprocess client lib
            if( YamlConfig["interprocess_client_lib"] ) {
                ConfigData_Ptr->InterProcessClientLib = YamlConfig["interprocess_client_lib"].as<std::string>();
                std::cout << "interprocess_client_lib:" << ConfigData_Ptr->InterProcessClientLib << std::endl;
            } else {
                std::cout << "interprocess_client_lib not found in configuration.yaml" << std::endl;
                bRet = false;
            }

            //read interprocess server lib
            if( YamlConfig["interprocess_server_lib"] ) {
                ConfigData_Ptr->InterProcessServerLib = YamlConfig["interprocess_server_lib"].as<std::string>();
                std::cout << "interprocess_server_lib:" << ConfigData_Ptr->InterProcessServerLib << std::endl;
            } else {
                std::cout << "interprocess_server_lib not found in configuration.yaml" << std::endl;
                bRet = false;
            }

            //read app2serv pipe name
            if( YamlConfig["app2serv_pipe_name"] ) {
                ConfigData_Ptr->App2ServPipeName = YamlConfig["app2serv_pipe_name"].as<std::string>();
                std::cout << "app2serv_pipe_name:" << ConfigData_Ptr->App2ServPipeName << std::endl;
            } else {
                std::cout << "app2serv_pipe_name not found in configuration.yaml" << std::endl;
                bRet = false;
            }

            //read serv2app pipe name
            if( YamlConfig["serv2app_pipe_name"] ) {
                ConfigData_Ptr->Serv2AppPipeName = YamlConfig["serv2app_pipe_name"].as<std::string>();
                std::cout << "serv2app_pipe_name:" << ConfigData_Ptr->Serv2AppPipeName << std::endl;
            } else {
                std::cout << "serv2app_pipe_name not found in configuration.yaml" << std::endl;
                bRet = false;
            }
        }

        //error bad file
        catch (const YAML::BadFile& e) {
            std::cerr << "Error reading YAML file: " << e.what() << std::endl;
            bRet = false;
        }

        //error parsing file
        catch (const YAML::ParserException& e) {
            std::cerr << "Error parsing YAML file: " << e.what() << std::endl;
            bRet = false;
        }

    } else {
        //data pointer not valid
        bRet = false;
    }

    return bRet;
}

