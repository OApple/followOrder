#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H
//#include <chkthread.h>
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
class UpdateThread
{
private:
    std::mutex mtx;
     string followUser;
     bool updateFlag;
    std::thread chkThread;
    UpdateThread(){}
    void run();
public:
    static UpdateThread& GetInstance()
    {
        static UpdateThread Instance;
        return Instance;
    }

  void  start();
  void setUpdate();
};

#endif // UPDATETHREAD_H
