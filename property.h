#ifndef PROPERTY_H
#define PROPERTY_H
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include<iostream>
#include <chrono>
#include <iconv.h>
#include <mutex>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <pthread.h>
#endif  // _WIND32

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include <glog/logging.h>
#include <glog/log_severity.h>

#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>

using namespace std;

/*持仓信息情况*/
class HoldPositionInfo
{
public:
    int shortTotalPosition;//空头总持仓
    int longTotalPosition;//多头总持仓
    int shortYdPosition;//空头昨持仓
    int longYdPosition;//多头昨持仓
    int shortTdPosition;//空头今持仓
    int longTdPosition;//多头今持仓
    int shortAvaClosePosition;//空头可平量
    int longAvaClosePosition;//多头可平量
    double longAmount;//多头持仓交易金额
    double shortAmount;//空头持仓交易金额
    double shortHoldAvgPrice;//空头持仓均价
    double longHoldAvgPrice;//多头持仓均价

};
class InstrumentInfo
{
public:
    TThostFtdcExchangeIDType ExchangeID;
    ///合约数量乘数
    TThostFtdcVolumeMultipleType	VolumeMultiple;
    ///最小变动价位
    double	PriceTick;
    ///多头保证金率
    TThostFtdcRatioType	LongMarginRatio;
    ///空头保证金率
    TThostFtdcRatioType	ShortMarginRatio;
    string instrumentID;
    ///涨停板价
    TThostFtdcPriceType	UpperLimitPrice =0;
    ///跌停板价
    TThostFtdcPriceType	LowerLimitPrice = 0;
    bool isPriceInit = false;
};

/*报单信息*/
class OrderInfo
{
public:
    string tradingDay = "";
    string tradeTime = "";
    boost::posix_time::ptime orderInsertTime;
    boost::atomic_int32_t orderSeq;//组合报单序号
    string systemID = "";//系统编号，每个产品一个编号
    string investorID;
    string brokerID;
    string direction;
    string offsetFlag;
    string hedgeFlag;
    string orderPriceType;
    string orderType;//"0":buy open;"1":sell open;"01":buy close;"11":sell close;"2":stop profit;"3":stop loss
    string mkType;//"agg":aggressive mm;"pas":passive mm;"0":
    int sessionID;
    int frontID;
    string orderRef;
    string orderLocalID;
    string orderSysID;
    string brokerOrderSeq;
    double price;
    int volume;
    string instrumentID;
    int begin_up_cul = 0;
    int begin_down_cul = 0;
    int userID;//equal investorID
    int followCount=0;// follow order times
    unsigned int clientOrderToken;
    unsigned char m_Side;
    unsigned char m_SecType;
    string orderStatus="-1";
    string status= "0";//0:normal;1:now action(10:begin action;11:);a:unknown
    string function = "0";//0:normal order;100:stop loss order;200:stop profit order
};
/*CTP interface info*/
class CTPInterface
{
public:
    string investorID;
    bool loginOK;
    CThostFtdcTraderSpi* pUserSpi;
    CThostFtdcTraderApi* pUserApi;			// 创建UserApi

};

#endif // PROPERTY_H
