#include <iostream>
#include <memory>
#include <dlfcn.h>
#include <glog/logging.h>

using namespace std;
#include "dataprocessor.h"
#include "mysqlconnectpool.h"
#include "traderspi.h"
#include "niutraderspi.h"
#include "property.h"
#include "chkthread.h"
#include "updatethread.h"


int main(int argc, char**argv)
{
    google::InitGoogleLogging(argv[0]);
    system("mkdir -p log");
    google::SetLogDestination(google::GLOG_INFO, "./log/info_");
    google::SetLogDestination(google::GLOG_WARNING, "./log/warn_");
    google::SetLogDestination(google::GLOG_ERROR, "./log/error_");

    DataInitInstance& dii=DataInitInstance::GetInstance();

    dii.GetConfigFromFile();

    dii.GetConfigFromRedis();

    dii.DataInit();
    ChkThread&ct=ChkThread::GetInstance();
    ct.start();
    UpdateThread&ut=UpdateThread::GetInstance();
    ut.start();
    dii.startTradeApi();

    while(1){
        sleep(2);

    }
    google::ShutdownGoogleLogging();
    return 0;
}



