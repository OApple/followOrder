#ifndef USER_ORDER_FIELD_H
#define USER_ORDER_FIELD_H
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include<iostream>
#include <chrono>
//#include <iconv.h>
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
#include <boost/algorithm/string/classification.hpp>

#include <glog/logging.h>
#include <glog/log_severity.h>

#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
//#include "dataprocessor.h"
//#include "property.h"
#include "traderspi.h"

using std::string;

//class CTraderSpi;
class UserOrderField
{
private:
    UserOrderField(){}
    char _status='z';
    bool _immediate_flag=false;
    char _direction;
    unsigned int _tick;

    int _frontID;
    int _sessionID;

    int _requestID;
    string _instrumentID;
    string _offset_flag;
    string _hedge_flag;
    double _price;
    double _price_tick;

    char _followcnt=0;
    string _key;

public:
    static UserOrderField*CreateUserOrderField(CThostFtdcOrderField *pOrder,CTraderSpi*uai);
    friend int CTraderSpi::ReqOrderInsert(UserOrderField* userOrderField);
    friend  int CTraderSpi::ReqOrderAction(UserOrderField* orderInfo);
    int   ReqOrderInsert();
    int    ReqOrderAction();
    void SetStatus(char status);
    char GetStatus();
    int UpdateRef();
    void UpdatePrice();
    void DecVol();
    string GetKey();
    string GetKey2();

        unsigned int _order_ref;
      string _investorID="";
     string  _key2;
     int _volume;
    //string orderType;//addition info
    string timeFlag="0";
   time_t begin_time;

    string brokerID="";
    //    int followCount=0;// follow order times
    char orderPriceType='2';
    CThostFtdcTraderSpi* _pTraderSpi;
    CThostFtdcTraderApi* _pUserApi;
    string orderSysID;

    string NOrderSysID;
    string NinvestorID;
    bool immediate_flag() const;
    void setImmediate_flag(bool immediate_flag);
};
string GetKey(CThostFtdcOrderField *pOrder,CTraderSpi*uai);
string GetKey2(CThostFtdcOrderField *pOrder);
string GetKey2(CThostFtdcInputOrderField *pInputOrder);
#endif // USER_ORDER_FIELD_H
