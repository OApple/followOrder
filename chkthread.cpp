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
    while (true)
    {
        mtx.lock();
        unordered_map<string, UserOrderField*> ::iterator it=NorderMap.begin();
        for(;it!=NorderMap.end();it++)
        {
            UserOrderField*uof=it->second;
            LOG(ERROR)<<"NorderMap key="<<it->first<<"status="<<uof->GetStatus()<<endl;
            if(uof->GetStatus()=='r')
            {
                //todo cancle
                uof->ReqOrderAction();
                ForderMap.erase(uof->GetKey2());
                //todo reqinsertorder
                uof->UpdateRef();
                uof->UpdatePrice();
                ForderMap[uof->GetKey2()]=uof;
                uof->ReqOrderInsert();
            }
            if(uof->GetStatus()=='5')
            {
                string key2=uof->GetKey2();
                LOG(ERROR)<<"erase  NorderMap key="<<it->first<<endl;
                LOG(ERROR)<<"erase ForderMap key="<<key2<<endl;
                NorderMap.erase(it);
                ForderMap.erase(key2);
                delete uof;

                    break;
            }
            else
            {
                continue;
            }

        }
        LOG(INFO)<<"NorderMap size="<<NorderMap.size()<<endl;
        LOG(INFO)<<"ForderMap size="<<ForderMap.size()<<endl;

        LOG(WARNING)<<"NorderMap size="<<NorderMap.size()<<endl;
        LOG(WARNING)<<"ForderMap size="<<ForderMap.size()<<endl;
        mtx.unlock();
//        it=NorderMap.begin();
        if(it!=NorderMap.end())
            continue;
        sleep(10);
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
    mtx.lock();

    unordered_map<string, UserOrderField*> ::iterator nit=NorderMap.find(key);
    if(nit==NorderMap.end())
        NorderMap[key]=userOrderField;

    unordered_map<string, UserOrderField*> ::iterator fit=ForderMap.find(key2);
    if(fit==ForderMap.end())
        ForderMap[key2]=userOrderField;

    mtx.unlock();
}
bool ChkThread::is_in_Nmap(string key)
{
    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=NorderMap.find(key);
    if(it==NorderMap.end())
    {
        mtx.unlock();
        return false;
    }
    else
    {
        mtx.unlock();
        return true;
    }
    mtx.unlock();
}
UserOrderField*ChkThread::get_Nuser_order(string key)
{
    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=NorderMap.find(key);
    if(it==NorderMap.end())
    {
        mtx.unlock();
        return NULL;
    }
    else
    {
        mtx.unlock();
        return it->second;
    }
    mtx.unlock();
}
UserOrderField*ChkThread::get_Fuser_order(string key)
{
    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=ForderMap.find(key);
    if(it==ForderMap.end())
    {
        mtx.unlock();
        return NULL;
    }
    else
    {
        mtx.unlock();
        return it->second;
    }
    mtx.unlock();
}

void ChkThread::erase(UserOrderField* userOrderField)
{
    string key=userOrderField->GetKey();
    string key2=userOrderField->GetKey2();
    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=NorderMap.find(key);
    if(it==NorderMap.end())
    {
        mtx.unlock();
        return ;
    }
    else
    {
        NorderMap.erase(key);
        ForderMap.erase(key2);
    }
    mtx.unlock();
}
void ChkThread::erase(string key)
{
    mtx.lock();
    unordered_map<string, UserOrderField*> ::iterator it=ForderMap.find(key);
    UserOrderField*userOrderField;
    if(it==NorderMap.end())
    {
        mtx.unlock();
        return;
    }
    else
    {
        userOrderField=it->second;
        string key=userOrderField->GetKey();
        string key2=userOrderField->GetKey2();
        NorderMap.erase(key);
        ForderMap.erase(key2);
    }

    mtx.unlock();
}
