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
void UpdateThread::run()
{
    DataInitInstance& dii=DataInitInstance::GetInstance();
//    string followUser=dii.redis_con.getConfig();
    std::unique_lock<std::mutex> lck(mtx);
    while(true)
    {
        cv.wait_for(lck,std::chrono::seconds(10));
        slaveMasters=dii.redis_con.get(dii.redis_key);

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
            LOG(ERROR)<<"different"<<endl;
            cout<<slaveMasters<<"<-->\n"<<dii.slaveMasters<<endl;
            vector<string> vslaveMaster;
            set<string> new_master_set;
            set<string> old_master_set=dii.getMaster();
            boost::split(vslaveMaster,slaveMasters,boost::is_any_of("&"));//user~nman&user~nman
            BOOST_FOREACH(string&node,vslaveMaster)
            {
                vector<string> slave_master;
                vector<string> master_config;
                split(slave_master,node,boost::is_any_of("~"));//user~nman  tmp[0]=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2   tmp[1]=5555/78899
                if(slave_master.size()==0)
                    continue;
                split(master_config,slave_master[1],is_any_of("/"));
                if(master_config.size()==0)
                    continue;
                new_master_set.insert(master_config[0]);

                NiuTraderSpi*ntp = dii.getMaster(master_config[0]);
                if (ntp != nullptr)
                {
                    unordered_map<string, string> new_config_map ;
                    unordered_map<string, string> old_config_map ;

                    old_config_map=ntp->getSlaveConfig();
                    vector<string> tmp_users ;
                    split(tmp_users,slave_master[0],is_any_of(","));//
                    for (unsigned int i = 0; i < tmp_users.size();i++)
                    {
                        vector<string> user_config;
                        split(user_config,tmp_users[i],is_any_of("/"));//
                        if(user_config.size()==0)
                            continue;
                        new_config_map[user_config[0]]=tmp_users[i];
                    }

                    //tmp_follows_map - nfollows_map    add new user
                    //set_difference(new_follows_map.begin(),new_follows_map.end(),old_follows_map.begin(),old_follows_map.end(),add_follows.begin());
                    typedef  unordered_map<string, string>::value_type  const_pair;
                    BOOST_FOREACH(const_pair&node,new_config_map)
                    {
                        auto it=old_config_map.find(node.first);
                        if(it==old_config_map.end())
                        {
                            ntp->addUser(new_config_map[node.first]);
                            cout<<"add slave users="<<node.first<<endl;
                        }
                    }

                    //nfollows_map - tmp_follows_map  delete user
                    BOOST_FOREACH(const_pair&node,old_config_map)
                    {
                        auto it=new_config_map.find(node.first);
                        if(it==new_config_map.end())
                        {
                             ntp->delUser(node.first);
                            cout<<"del slave users="<<node.first<<endl;
                        }
                    }

                    // nfollows_map U tmp_follows_map  check config
                    BOOST_FOREACH(const_pair&node,old_config_map)
                    {
                        auto it=new_config_map.find(node.first);
                        if(it!=new_config_map.end())
                        {
                            if(it->second!=node.second)
                            {
                                   ntp->setSlaveConfig(node.first,new_config_map[node.first]);
                                    cout<<"modify slave users="<<node.first<<endl;
                            }
                        }
                    }

                }
                else//add new nman
                {
                    dii.addMaster(slave_master);
                }
            }
            for(const string&node:old_master_set)
            {
                auto it=new_master_set.find(node);
                if(it==new_master_set.end())
                {
                    dii.delMaster(node);
                }
            }
            dii.slaveMasters=slaveMasters;
        }
    }
}

void UpdateThread::start()
{
    chkThread=thread(&UpdateThread::run,this);
}

void UpdateThread::notify()
{
    cv.notify_one();
}


