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
    void initTradeApi();

    void GetConfigFromFile();
    void GetConfigFromRedis();
    void DataInit();
    void insert_follow_user(string users, vector<CTraderSpi *> &vac);
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
    double getPriceTick(string InstrumentID);
    string getHeyueName(string str);
    int getInstrumentMulti(string InstrumentID);
    //UserAccount* getTradeAccount(string);
    //void setUserAccount(UserAccount* ua);

    unordered_map<string, CTraderSpi*> followNBAccountMap;
    unordered_map<string, NiuTraderSpi*> NBAccountMap;


    //property config
    int useReal=0;//test environment
    string  realTradeFrontAddr ;
    string	 broker_id = "0077";
    string	 realBrokerID = "9999";
    string followUser;
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
    Redis redis_con;
    // UserApi对象
    unordered_map<string, CTPInterface*> tradeApiMap;

    MysqlConnectPool *mysql_pool ;

};

#endif // DATAPROCESSOR_H
