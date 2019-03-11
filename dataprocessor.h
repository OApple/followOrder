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
#include "property.h"

using namespace std;
class CTraderSpi;
class NiuTraderSpi;
class CMdSpi;

class DataInitInstance{
private:
     DataInitInstance(){}
public:
     static DataInitInstance& GetInstance()
     {
         static DataInitInstance Instance;
         return Instance;
     }
//    DataInitInstance(void);
    virtual ~DataInitInstance(void);
    /************************************************************************/
    /* 初始化参数列表                                                                     */
    /************************************************************************/
    void startTradeApi();

    void GetConfigFromFile();
    void GetConfigFromRedis();
    void DataInit();
    unordered_map<string, CTraderSpi *> makeSlaves(string users, vector<CTraderSpi *> &vac);
    string getTime();


    ///save orderField to DB
    void saveThostFtdcOrderFieldToDb(CThostFtdcOrderField *pOrder);
    void  saveThostFtdcInvestorPositionFieldToDb(CThostFtdcInvestorPositionField *pOrder);
    void saveThostFtdcTradeFieldToDb(CThostFtdcTradeField *pTrade);
    void saveThostFtdcInputOrderFieldToDb(CThostFtdcInputOrderField *pInputOrder, string info);
    void saveCThostFtdcTradingAccountFieldToDb(CThostFtdcTradingAccountField *pTradingAccount);
     void saveDepthMarketDataToDb(CThostFtdcDepthMarketDataField *pDepthMarketData);
    void  saveSettlementToDB(NiuTraderSpi &TraderSpi);
    void saveSettlementToDB(CTraderSpi &TraderSpi);


    vector<string> getInstrumentIDZH(string InstrumentID);
    double getPriceTick(string InstrumentID);
    string getHeyueName(string str);
    int getInstrumentMulti(string InstrumentID);

    void addMaster(vector<string>&slave_master);
    void delMaster(const string &master);
    void startMaster(const string &master);
    NiuTraderSpi *getMaster(const string &master);
    set<string>  getMaster();
    unordered_map<string, NiuTraderSpi*> masterAccountMap;


    //property config
    int MuseReal=0;//test environment
     int SuseReal=0;//test environment
    string  realTradeFrontAddr ;
    string	 broker_id = "0077";
    string	 realBrokerID = "9999";
    string slaveMasters;
    string _market_front_addr;
    string _trade_front_addr;
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
    string redis_key;
    Redis redis_con;
    // UserApi对象
    unordered_map<string, CTPInterface*> tradeApiMap;

    MysqlConnectPool *mysql_pool ;

};

#endif // DATAPROCESSOR_H
