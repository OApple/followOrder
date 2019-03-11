#include "updateacc.h"

#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <chrono>             // std::chrono::seconds
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable, std::cv_status
#include <boost/foreach.hpp>

#include "updatethread.h"
#include "dataprocessor.h"
#include "niutraderspi.h"
#include "chkthread.h"

using boost::split;
using boost::is_any_of;
void UpdateAcc::run()
{
    DataInitInstance& dii=DataInitInstance::GetInstance();
    std::unique_lock<std::mutex> lck(mtx);
    while(true)
    {
        cv.wait(lck);

        ChkThread& ct= ChkThread::GetInstance();
        if(ct.haveOrder())
        {
            LOG(ERROR)<<"maps have order refuse update";
            return ;
        }
        if(slaveMasters==dii.slaveMasters)
        {
            continue;
        }
        else
        {

        }
    }
}

void UpdateAcc::start()
{
    chkThread=thread(&UpdateAcc::run,this);
}

void UpdateAcc::notify()
{
    cv.notify_one();
}


