#pragma once
#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H
#include <string.h>
#include <string>
#include <list>
#include <iostream>
#include <mutex>
#include <string>
#include <time.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <sqlite3.h>
#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>


#include "mysqlconnectpool.h"
#include "rediscpp.h"
//#include "traderspi.h"
#include "property.h"
//#include "user_order_field.h"

using namespace std;
class CTraderSpi;
class NiuTraderSpi;

class DataInitInstance{
public:

    DataInitInstance(void);
    virtual ~DataInitInstance(void);
    /************************************************************************/
    /* 初始化参数列表                                                                     */
    /************************************************************************/
    void initTradeApi();

    void GetConfigFromFile();
    void GetConfigFromRedis();
    void DataInit();
    void insert_follow_user(string users, vector<string>&userlist, vector<CTraderSpi *> &vac);
    string getTime();


    ///save orderField to DB
    void saveThostFtdcOrderFieldToDb(CThostFtdcOrderField *pOrder);
    void  saveThostFtdcInvestorPositionFieldToDb(CThostFtdcInvestorPositionField *pOrder);
    void saveThostFtdcTradeFieldToDb(CThostFtdcTradeField *pTrade);
    void saveThostFtdcInputOrderFieldToDb(CThostFtdcInputOrderField *pInputOrder, string info);
    void saveCThostFtdcTradingAccountFieldToDb(CThostFtdcTradingAccountField *pTradingAccount);
    void  saveSettlementToDB(NiuTraderSpi &TraderSpi);
    void saveSettlementToDB(CTraderSpi &TraderSpi);
    vector<string> getInstrumentIDZH(string InstrumentID);
    string getHeyueName(string str);
    //UserAccount* getTradeAccount(string);
    //void setUserAccount(UserAccount* ua);

    unordered_map<string, CTraderSpi*> followNBAccountMap;
    unordered_map<string, NiuTraderSpi*> NBAccountMap;


    //property config
    string environment="1";//test environment
    string marketServerIP = "";
    int  marketServerPort = 0;
    string tradeServerIP = "";
    int tradeServerPort = 0;
    int queryServerPort = 0;
    string queryServerIP = "";
    string	BROKER_ID = "0077";				// 经纪公司代码
    string exgTradeFrontIPCSHFE;
    string exgParticipantIDCSHFE;
    string exgTraderIDCSHFE;
    string exgTraderPasswdCSHFE;
    string exgFlowType;
    int remoteTradeServerPort = 0;//交易端口
    int mkdatasrvport = 0;
    string  PASSWORD = "0";			// 用户密码
    string LOGIN_ID = "";
    int notActiveInsertAmount = 1;//不活跃合约重复下单次数
    int arbVolumeMetric = 0;//套利单总共能下多少手，单边
    int arbVolume = 0;//当前持仓量
    int orderInsertInterval = 1000;//下单间隔
    int maxFollowTimes = 2;//最大追单次数
    int maxUntradeNums = 3;//最大未成交套利单笔数(非手数，手数=maxUntradeNums*defaultVolume)
    int biasTickNums = 2;//价格偏移多少个tick进行追单
    string _market_front_addr;
    string _trade_front_addr;
    int followTimes=1;
    string db_host;
    string db_user;
    string db_pwd;
    string db_name;
    int     db_maxConnSize=10;
    string db_charset="utf8";
    int     db_port=3306;

    string redis_host="127.0.0.1";
    string redis_pwd;
    string redis_port="6379";
    Redis redis_con;
    // UserApi对象
    unordered_map<string, CTPInterface*> tradeApiMap;

    MysqlConnectPool *mysql_pool ;

};

#endif // DATAPROCESSOR_H
