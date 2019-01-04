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
#include "timeprocesser.h"
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
    void initExgTraderApi();
    void delTraderApi(string investorID);
    void initTradeApi();

    void GetConfigFromFile();
    void GetConfigFromRedis();
     void DataInit();
    void insert_follow_user(string users, vector<string>&userlist, vector<CTraderSpi *> &vac);
    double getMultipler(string instrumentID);
//    CTPInterface* instanceTradeApi(UserAccountInfo* );
    CTPInterface* getTradeApi(string);
//    void addNewOrderInsert(UserOrderField*);
    void addNewOrderAction(OrderInfo* orderInfo);
    string getCloseMethod(string investorID,string instrumentID, string type);
    string getOrderInfo(OrderInfo* info);
    void processHowManyHoldsCanBeClose(CThostFtdcOrderField *pOrder,string type);
    int processtrade(TradeInfo *pTrade);
    unordered_map<string, HoldPositionInfo*> getPositionMap(string investorID);
    string processRspReqInstrument(CThostFtdcInstrumentField *pInstrument);
    void setLoginOk(string investorID);
    bool isAllLoginOK();
    void deleteOriOrder(int frontID,int sessionID,string orderRef);
    void startStrategy();
    void processOrder(CThostFtdcOrderField *pOrder,string type);
    string getTime();
    void updateOriOrder(CThostFtdcOrderField *pOrder,string type);
    void deleteOriOrder(string orderSysID);
    bool orderIsResbonsed(string investorID);
    void processAction(CThostFtdcOrderField *pOrder);
    int getFollowTick(string investorID);
    double getTickMetric(string instrumentID);
    double getPriceTick(string instrumentID);
//    bool isNBMANOrder(CThostFtdcOrderField *pOrder);
//    bool isNBMANTrade(CThostFtdcTradeField *pTrade);
//    bool isNBMAN(string investorID);


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


    string FOLLOWFLAG="1";
    bool isInstrumentOK=false;
    bool isLogout=true;
    bool isDoneSometh=false;//when do something,send a sigal
    string whichAccount="";
    //CodeConverter *codeCC;
    /********some lock*******/
    boost::recursive_mutex initapi_mtx;//tradeapi lock
    boost::recursive_mutex pst_mtx;//position lock
    /***************/

//    unordered_map<string, UserAccountInfo*> allAccountMap;
//    unordered_map<string, string> seq_map_ordersysid;
    //list<string> followNBAccountList;//long
    //list<string> NBAccountList;//long
    unordered_map<string, InstrumentInfo*> instruments;			//合约信息

    // UserApi对象
    //CShfeFtdcTraderApi* cshfeTraderApi;
    //CSHFETraderSpi* cshfeTraderSpi;
    unordered_map<string, CTPInterface*> tradeApiMap;
    unordered_map<string, bool> loginOK;
    list<OrderInfo*> bidList;//order list
    list<OrderInfo*> askList;//
    list<OrderInfo*> longList;// trade list
    list<OrderInfo*> shortList;//trade list

    int orderProInterval=3;
    char **ppInstrumentID;// 行情订阅列表
    int iInstrumentID = 1;									// 行情订阅数量
    bool isUserLogin=false;
//    string loginInvestorID="";

    ///多个字段组合时使用的分隔符
    string sep = ";";
    char tradingDay[12];
    // 会话参数
//    string MAC_ADDRESS = "00-01-02-03-04-05";
//    int	FRONT_ID;	//前置编号
//    int	SESSION_ID;	//会话编号
//    string	ORDER_REF;	//报单引用
//    int isbegin = 0;//是否启动策略
    int realLongPstLimit = 0;
    int realShortPstLimit = 0;
//    int lastABSSpread = 0;
//    int firstGap = 2;
//    int secondGap = 5;
//    int longpstlimit;
    //shortpstlimit
    int shortpstlimit;
    //记录时间
    int long_offset_flag;
    int short_offset_flag;
    //买平标志,1开仓；2平仓
    //有空头持仓，可以执行"买平仓"
    int longPstIsClose;
    //有多头持仓，可以执行"卖平仓"
    int shortPstIsClose;
    int start_process = 0;

    //交易对象
    boost::thread_group thread_log_group;
    // 配置参数
    //char FRONT_ADDR[] = "tcp://asp-sim2-md1.financial-trading-platform.com:26213";		// 前置地址
    //180.168.146.181:10100
//    string tradingDayT = "";//2010-01-01
    //存放行情消息队列
//    list<string> mkdata;



    /**************exchange info***************/
//    uint32_t private_stream_topic_id=2;
//    uint32_t cur_private_stream_sequence_number=0;
//    uint32_t max_private_stream_sequence_number=0;
//    uint32_t b_new_ps_package;

//    string INVESTOR_ID = "00042";			// 投资者代码

    int USER_ID = 0;


    string hedgeFlag = "1";//账号类型，组合投机套保标 投机 '1'套保 '3'
    int defaultVolume = 1;//默认下单手数

    //OrderFieldInfo* cancledOrderInfo = new OrderFieldInfo();//cancled order info
    //OrderFieldInfo* acceptShLiOrderInfo = new OrderFieldInfo();//shengli plantform order response info
    //OrderFieldInfo* acceptMarketOrderInfo = new OrderFieldInfo();//exchange order response info
    //TradeFieldInfo* orderExecInfo = new TradeFieldInfo();//when order is executed,trade response is sent
//    vector<string> quoteList;	//合约列表											//价格变动单位
//    unordered_map<string, vector<string>> instr_map;				//一个合约和哪些合约配对

    //触发套利单时，保存套利信息

//    double asTradeListMinGap;
//    double asTradeListMaxGap;
//    double dsTradeListMinGap;
//    double dsTradeListMaxGap;

    int gapCanBeOpenNums = 4;//每个gap上面可以开仓的数量
    //double stopLossTickNums = 10;//价差往不利方向损失多少tick时候，组合止损



    //int maxUntradeCount = 3;//最大未成交笔数(非手数，手数=maxUntradeNums*defaultVolume)
//    string systemID = "";//系统编号，每个产品一个编号
    double openTick = 5.0;//开仓时候每次增加的tick值5
    double closeTick = 15.0;//平仓时每次增加的tick值15


     MysqlConnectPool *mysql_pool ;


};

#endif // DATAPROCESSOR_H
