#include "updatethread.h"
#include "dataprocessor.h"


void UpdateThread::run()
{
    DataInitInstance& dii=DataInitInstance::GetInstance();
    string followUser=dii.redis_con.get("followUser");
    while(true)
    {
        followUser=dii.redis_con.get("followUser");
        if(followUser==dii.followUser)
            //        cout<<followUser<<" same"<<endl;
            sleep(2);
        else
        {
            vector<string> user_nman;
            boost::split(user_nman,followUser,boost::is_any_of("&"));//user~nman&user~nman
            int cnt_num=user_nman.size();
            for(unsigned int i = 0; i <cnt_num ;i++)
            {
//                vector<string> tmp;
//                boost::split(tmp,user_nman[i],boost::is_any_of("~"));//user~nman  tmp[0]=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2   tmp[1]=5555/78899

//                vector<CTraderSpi*> vac;
//                insert_follow_user(tmp[0],vac);


//                NiuTraderSpi* ba=new NiuTraderSpi(*this,tmp[1]);
//                ba->setFollow(vac);
//                unordered_map<string, NiuTraderSpi*>::iterator it_map_strs = NBAccountMap.find(ba->getInvestorID());
//                if (it_map_strs == NBAccountMap.end())
//                {
//                    NBAccountMap[ba->getInvestorID()] = ba;
//                }
//                else
//                {//update

//                }

            }
            dii.followUser=followUser;
        }
    }
}

void UpdateThread::start()
{
    chkThread=thread(&UpdateThread::run,this);
}

void UpdateThread::setUpdate()
{
    mtx.lock();
    updateFlag=true;
    mtx.unlock();
}
