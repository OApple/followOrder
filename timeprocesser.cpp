#include <string>
#include <unordered_map>
#include <glog/logging.h>

#include "timeprocesser.h"
#include "property.h"
#include "dataprocessor.h"
using namespace std;


// 请求编号
//extern int iRequestID;
//extern DataInitInstance dii;
//extern bool isLogout;
//extern string systemID;//系统编号，每个产品一个编号
//extern unordered_map<string, vector<string>> instr_map;				//一个合约和哪些合约配对
//extern char tradingDay[12];
//extern string tradingDayT;//2010-01-01
//extern boost::recursive_mutex techMetric_mtx;//技术指标
//extern boost::recursive_mutex mkdata_mtx; //行情锁
//extern boost::lockfree::queue<LogMsg*> networkTradeQueue;////报单、成交消息队列,网络通讯使用





boost::posix_time::ptime getCurrentTimeByBoost() {
    boost::posix_time::ptime time_now;
    // 这里为微秒为单位;这里可以将microsec_clock替换成second_clock以秒为单位;
    time_now = boost::posix_time::microsec_clock::universal_time();
   // cout<<"cxdfsf"<<time_now<<endl;
    return time_now;
}

int getTimeInterval(boost::posix_time::ptime timeBegin, boost::posix_time::ptime timeEnd, string timeType) {
    boost::posix_time::millisec_posix_time_system_config::time_duration_type time_elapse;
    //时间间隔
    time_elapse = timeEnd - timeBegin;
    if (timeType == "t") {
        // 类似GetTickCount，只是这边得到的是2个时间的ticket值的差，以微秒为单位;
        int ticks = time_elapse.ticks();
        return ticks;
    } else if (timeType == "s") {
        // 得到两个时间间隔的秒数;
        int sec = time_elapse.total_seconds();
        return sec;
    }
}
boost::posix_time::ptime changeStringToTime(string strTime) {
    //把字符串转换为boost 时间类对象 :
    //ptime p1 = from_iso_string("20121101T202020");
    //return  boost::posix_time::time_from_string("2012-3-5 01:00:00");
    return  boost::posix_time::time_from_string(strTime);
}

//void sigalrm_fn(int sig){
//    boost::recursive_mutex::scoped_lock SLock(dii.pst_mtx);//锁定
//    string tmpmsg="check if order is time out.bid list size="+boost::lexical_cast<string>(dii.bidList.size())+",ask list size="+boost::lexical_cast<string>(dii.askList.size());
//    cout<<tmpmsg<<endl;
//    LOG(INFO)<<tmpmsg;
//    boost::posix_time::ptime endTime=getCurrentTimeByBoost();
//    for(list<OrderInfo*>::iterator it = dii.bidList.begin();it != dii.bidList.end();it++){
//        OrderInfo* orderInfo = *it;
//        int orderInv = getTimeInterval(orderInfo->orderInsertTime,endTime,"s");
//        if(orderInv>=3&&(orderInfo->orderSysID.size()!=0)){//time over 3 seconds and already stay at exchange
//            string msg="order time out:"+dii.getOrderInfo(orderInfo);
//            LOG(INFO)<<msg;
//            if(orderInfo->followCount<3&&orderInfo->status=="0"){//already order action
//                msg="follow order cout is "+boost::lexical_cast<string>(orderInfo->followCount)+",status="+orderInfo->status+", go on follow.";
//                LOG(INFO)<<msg;
//                orderInfo->followCount+=1;//add follow count
//                orderInfo->status="1";//set follow flag
//            }else{
//                msg="follow order cout is "+boost::lexical_cast<string>(orderInfo->followCount)+",status="+orderInfo->status+", not follow,just action.";
//                LOG(INFO)<<msg;
//            }
//            dii.addNewOrderAction(orderInfo);
//        }else{
//            string msg="order is not time out or not receive response."+dii.getOrderInfo(orderInfo);
//            LOG(INFO)<<msg;
//        }
//    }
//    for(list<OrderInfo*>::iterator it = dii.askList.begin();it != dii.askList.end();it++){
//        OrderInfo* orderInfo = *it;
//        int orderInv = getTimeInterval(orderInfo->orderInsertTime,endTime,"s");
//        if(orderInv>=3&&(orderInfo->orderSysID.size()!=0)){//time over 3 seconds and already stay at exchange
//            string msg="order time out:"+dii.getOrderInfo(orderInfo);
//            LOG(INFO)<<msg;
//            if(orderInfo->followCount<3&&orderInfo->status=="0"){
//                msg="follow order cout is "+boost::lexical_cast<string>(orderInfo->followCount)+",status="+orderInfo->status+", go on follow.";
//                LOG(INFO)<<msg;
//                orderInfo->followCount+=1;//add follow count
//                orderInfo->status="1";//set follow flag
//            }else{
//                msg="follow order cout is "+boost::lexical_cast<string>(orderInfo->followCount)+",status="+orderInfo->status+", not follow,just action.";
//                LOG(INFO)<<msg;
//            }
//            dii.addNewOrderAction(orderInfo);
//        }else{
//            string msg="order is not time out or not receive response."+dii.getOrderInfo(orderInfo);
//            LOG(INFO)<<msg;
//        }
//    }
//}
int isExit = 1;

//通过调用alarm来设置计时器，然后继续做别的事情。当计时器计时到0时，信号发送，处理函数被调用。
static void timer(int sig) {

    static int count = 0;
    count++;
    printf("\ncount = %d\n", count);

    if (sig == SIGALRM)
    {
        printf("timer\n");
    }
    signal(SIGALRM, timer); //重新注册定时器
    alarm(1);//定时1s
    if (count == 5)
    {
        alarm(0);
        isExit = 0;
    }
    return;
}

void metricProcesser() {

    try {
        signal(SIGALRM, timer);//执行定时器函数
        printf("wait 1s\n");
        alarm(1); //定时1s
        while (isExit)
        {

        };


    }catch (const runtime_error &re) {
        cerr << re.what() << endl;
        LOG(ERROR) << re.what();
    }
    catch (exception* e) {
        cerr << e->what() << endl;
        LOG(ERROR) << e->what();
    }

}
//void ttt(){
//    try {
//        struct sigaction act;
//        union sigval tsval;
//        act.sa_handler = sigalrm_fn;
//        act.sa_flags = 0;
//        sigemptyset(&act.sa_mask);
//        sigaction(50, &act, NULL);
//        //len = strlen(msg);
//        while( 1 )
//        {
//            sleep(dii.orderProInterval); ///*睡眠2秒
//            ///向主进程发送信号，实际上是自己给自己发信号
//            sigqueue(getpid(), 50, tsval);
//        }
//    }catch (const runtime_error &re) {
//        cerr << re.what() << endl;
//        LOG(ERROR) << re.what();
//    }
//    catch (exception* e) {
//        cerr << e->what() << endl;
//        LOG(ERROR) << e->what();
//    }

//}
