#ifndef CHKTHREAD_H
#define CHKTHREAD_H
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include<iostream>
#include <chrono>
#include <iconv.h>
#include <mutex>
#include <time.h>
//#include <pthread.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/bind.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/thread/thread_pool.hpp>
//#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include <glog/logging.h>
#include <glog/log_severity.h>

#include <ThostFtdcTraderApi.h>
#include <ThostFtdcUserApiDataType.h>
#include "property.h"
#include "user_order_field.h"

using namespace std;

class ChkThread
{
private:
    unordered_map<string, UserOrderField*> NorderMap;
    unordered_map<string, UserOrderField*> ForderMap;

    std::thread chkThread;
    ChkThread(){}
    void run();

public:
     std::mutex mtx;
    static ChkThread& GetInstance()
    {
        static ChkThread Instance;
        return Instance;
    }

    void  start();
    void putNOrder(UserOrderField*userOrderField);
    void putFOrder(UserOrderField*userOrderField);
    void eraseNOrder(UserOrderField*userOrderField);
    void eraseFOrder(string key);
    UserOrderField*get_Nuser_order(string key);
    UserOrderField*get_Fuser_order(string key);
    bool haveOrder();

};


#endif // CHKTHREAD_H
