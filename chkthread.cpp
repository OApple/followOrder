#include <unordered_map>
#include <mutex>
#include <boost/lexical_cast.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <glog/logging.h>
#include "chkthread.h"
#include "traderspi.h"
#include "user_order_field.h"

using namespace std;
using boost::lexical_cast;

void ChkThread::run()
{
    //check
    vector<unordered_map<string, UserOrderField*> ::iterator>ntmp;
    vector<string>ftmp;
    while (true)
    {
        {
            lock_guard<mutex> lck (mtx);
            unordered_map<string, UserOrderField*> ::iterator it=NorderMap.begin();
            for(;it!=NorderMap.end();it++)
            {
                UserOrderField*uof=it->second;
                LOG(ERROR)<<"NorderMap key="<<it->first<<"status="<<uof->GetStatus()<<endl;
                if(uof->GetStatus()=='r')
                {
                    //todo cancle
                    uof->ReqOrderAction();
//                    ForderMap.erase(uof->GetKey2());
                    //todo reqinsertorder
                    uof->UpdateRef();
                    uof->UpdatePrice();
//                    ForderMap[uof->GetKey2()]=uof;
//                    uof->ReqOrderInsert();
                }
                if(uof->GetStatus()=='5')
                {
                    string key2=uof->GetKey2();
                    LOG(ERROR)<<"erase  NorderMap key="<<it->first<<endl;
                    LOG(ERROR)<<"erase ForderMap key="<<key2<<endl;
                    // NorderMap.erase(it);
                    ntmp.push_back(it);
                    //                ForderMap.erase(key2);
                    ftmp.push_back(key2);
                    delete uof;
                    //it=NorderMap.begin();
                    //if(it!=NorderMap.end())
                    //goto next;
                    continue;
                }
                else
                {
                    continue;
                }

            }
            for(int i=0;i<ntmp.size();i++)
            {
                NorderMap.erase(ntmp[i]);
                ForderMap.erase(ftmp[i]);
            }
            ntmp.clear();
            ftmp.clear();
        }
        //        it=NorderMap.begin();


        //        if(it!=NorderMap.end())
        //            continue;
        //        else
        LOG(INFO)<<"NorderMap size="<<NorderMap.size()<<endl;
        LOG(INFO)<<"ForderMap size="<<ForderMap.size()<<endl;

        LOG(WARNING)<<"NorderMap size="<<NorderMap.size()<<endl;
        LOG(WARNING)<<"ForderMap size="<<ForderMap.size()<<endl;
        sleep(5);
    }
}
void ChkThread::start()
{
    chkThread=thread(&ChkThread::run,this);

}
void ChkThread::putOrder(UserOrderField*userOrderField)
{
    string key=userOrderField->GetKey();
    string key2=userOrderField->GetKey2();
//    mtx.lock();

    unordered_map<string, UserOrderField*> ::iterator nit=NorderMap.find(key);
    if(nit==NorderMap.end())
        NorderMap[key]=userOrderField;

    unordered_map<string, UserOrderField*> ::iterator fit=ForderMap.find(key2);
    if(fit==ForderMap.end())
        ForderMap[key2]=userOrderField;

//    mtx.unlock();
}

UserOrderField*ChkThread::get_Nuser_order(string key)
{
//    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=NorderMap.find(key);
    if(it==NorderMap.end())
    {
//        mtx.unlock();
        return NULL;
    }
    else
    {
//        mtx.unlock();
        return it->second;
    }
//    mtx.unlock();
}
UserOrderField*ChkThread::get_Fuser_order(string key)
{
//    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=ForderMap.find(key);
    if(it==ForderMap.end())
    {
//        mtx.unlock();
        return NULL;
    }
    else
    {
//        mtx.unlock();
        return it->second;
    }
    //    mtx.unlock();
}

bool ChkThread::haveOrder()
{
    return (NorderMap.size()||ForderMap.size());
}


