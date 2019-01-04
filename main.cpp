#include <iostream>
#include <memory>
#include <glog/logging.h>

#include "dataprocessor.h"
#include "timeprocesser.h"
#include "mysqlconnectpool.h"
#include "traderspi.h"
#include "niutraderspi.h"
#include "property.h"
#include "chkthread.h"

//using namespace std;
DataInitInstance dii;

// UserApi对象
// 请求编号
//int iRequestID;
void startSendMDThread(int sendtype);//发送类型 0：以客户端方式发送；1，以服务端方式发送
boost::thread_group thread_log_group;

//unordered_map<string, CThostFtdcTraderApi*>CTraderSpi:: _tradeApiMap;
//unordered_map<string,  CTraderSpi*>CTraderSpi:: _tradeSpiMap;
int main(int argc, char**argv)
{
    //netTest();
    google::InitGoogleLogging(argv[0]);
    //    google::InitGoogleLogging("");
    system("mkdir -p log");
    google::SetLogDestination(google::GLOG_INFO, "./log/info_");
    google::SetLogDestination(google::GLOG_WARNING, "./log/warn_");
    google::SetLogDestination(google::GLOG_ERROR, "./log/error_");
    //google::SetStderrLogging(2);
    //    google::SetLogFilenameExtension("log_");
    dii.GetConfigFromFile();
    dii.GetConfigFromRedis();
    dii.DataInit();
    ChkThread*ct=ChkThread::GetInstance();
    ct->start();
    dii.initTradeApi();

    while(1){
        sleep(2);
        //        boost::this_thread::sleep(boost::posix_time::seconds(1000));    //microsecond,millisecn
    }
    google::ShutdownGoogleLogging();
    return 0;
}


void startSendMDThread(int sendtype)
{
    //TraderDemo temp;
    //temp.m_queryServerIp = boost::lexical_cast<string>(TRADE_FRONT_ADDR);
   // thread_log_group.create_thread(&ttt);
  
}
