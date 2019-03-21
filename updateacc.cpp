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
    typedef  unordered_map<string, NiuTraderSpi*>::value_type  const_pair1;
    typedef  unordered_map<string, CTraderSpi*>::value_type  const_pair2;
    DataInitInstance& dii=DataInitInstance::GetInstance();
    std::unique_lock<std::mutex> lck(mtx);
    unordered_map<string, NiuTraderSpi*> &nmap=dii.masterAccountMap;
    while(true)
    {
        cv.wait(lck);
        BOOST_FOREACH(const_pair1&node1,nmap)
        {
            NiuTraderSpi*nsp=node1.second;
            nsp->ReqQryTradingAccount();
            nsp->ReqQryInvestorPosition();
            BOOST_FOREACH(const_pair2&node2,nsp->getSlave())
            {
                CTraderSpi*csp=node2.second;
                csp->ReqQryTradingAccount();
                csp->ReqQryInvestorPosition();
            }

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


