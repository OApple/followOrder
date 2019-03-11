#ifndef UPDATEACC_H
#define UPDATEACC_H

#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include<iostream>
#include <chrono>
#include <iconv.h>
#include <mutex>
#include <time.h>
#include <pthread.h>
#include <condition_variable>
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
using namespace std;
class UpdateAcc
{
private:
    std::mutex mtx;
    string slaveMasters;
    bool updateFlag;
    std::condition_variable cv;
    std::thread chkThread;
    UpdateAcc(){}
    void run();
public:
    static UpdateAcc& GetInstance()
    {
        static UpdateAcc Instance;
        return Instance;
    }

    void  start();
    void notify();
};

#endif // UPDATEACC_H
