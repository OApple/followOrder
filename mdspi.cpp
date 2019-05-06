#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <thread>
#include <mutex>
#include <glog/logging.h>
#include <boost/thread/recursive_mutex.hpp>
#include <ThostFtdcMdApi.h>

#include "mdspi.h"
#include "property.h"
#include "util.h"
#include "dataprocessor.h"

using namespace std;
using boost::lexical_cast;
using boost::locale::conv::between;


#pragma warning(disable : 4996)
CMdSpi* CMdSpi:: mdspi=nullptr;

CMdSpi::CMdSpi(DataInitInstance&di, const string &config):dii(di)
{
    _market_front_addr= dii._market_front_addr;
    investor_id="121791";
    max_instrument=50;
    instrument_start=0;
    string prefix=string("MDdate")+"/";
    system(("mkdir  -p "+prefix).c_str());
    pUserApi = CThostFtdcMdApi::CreateFtdcMdApi(prefix.c_str());			// 创建UserApi
    pUserApi->RegisterSpi((CThostFtdcMdSpi*)this);			// 注册事件类
    pUserApi->RegisterFront((char*)(_market_front_addr.c_str()));
    pUserApi->Init();
}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
                        int nRequestID, bool bIsLast)
{
    IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
    LOG(INFO) << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
    LOG(INFO) << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
    LOG(ERROR) <<(lexical_cast<string>(this)+"------>>>>OnFrontConnected") ;
    ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{

    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));

    int iResult = pUserApi->ReqUserLogin(&req, 9);
    LOG(INFO) << (lexical_cast<string>(this)+"<<<<----- ReqUserLogin: " + ((iResult == 0) ? "success" : "failed") );

}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(INFO)<<(lexical_cast<string>(this)+"----->>>>OnRspUserLogin");
    string str;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)){
//        str=strRspUserLoginField(pRspUserLogin);
//        LOG(ERROR)<<(lexical_cast<string>(this)+"----->>>>OnRspUserLogin"+str);
    status=true;
    }
//    if (bIsLast && !IsErrorRspInfo(pRspInfo))
//    {
//        SubscribeMarketData();
//    }
}

void CMdSpi::Subscribe(const string &instrument)
{
    //    int i=0;
    //    int vlen=vinstrument.size();
    //    if(vlen==0)
    //        return;
//     LOG(ERROR)<<instrument;
    int vlen=1;
    char**ppinstrument=new char*[vlen];

    // for(int i=0;i<vlen;i++)
    {
        ppinstrument[0]=new char[instrument.size()+1];
        strcpy(ppinstrument[0],instrument.c_str());
    }
    int iResult =pUserApi->SubscribeMarketData(ppinstrument, 1);
    LOG(ERROR)<< string(ppinstrument[0])<< "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
    delete [] ppinstrument[0];
    delete [] ppinstrument;
    //    i=0;
    //    for(;i<vlen;i++)
    //    {
    //        delete [] ppinstrument[i];
    //    }
    //    delete [] ppinstrument;
}
void CMdSpi::UnSubscribe(const string &instrument)
{

    int vlen=1;
    char**ppinstrument=new char*[vlen];

    {
        ppinstrument[0]=new char[instrument.size()+1];
        strcpy(ppinstrument[0],instrument.c_str());
    }
    int iResult =pUserApi->UnSubscribeMarketData(ppinstrument, 1);
    LOG(ERROR)<< string(ppinstrument[0])<< "--->>> 发送退订行情请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
    delete [] ppinstrument[0];
    delete [] ppinstrument;

}


//void CMdSpi::SubscribeMarketData(vector<string>ovinstrument)
//{
//    int i=0;
//    int vlen=ovinstrument.size();
//    if(vlen==0)
//        return;
//    for(;i<vlen;i++)
//    {
//        vinstrument.push_back(ovinstrument[i]);
//    }
//    SubscribeMarketData();
//}

//int CMdSpi::SubscribeMarketData(Instrument& instrument)
//{
//    if(instrument.InstrumentID.length()==0)
//        return -1;
//    if(vinstrument.size()>=max_instrument)
//        return -2;

//    Instrument *pin=new Instrument();
//    pin->ExchangeID=instrument.ExchangeID;
//    pin->InstrumentID=instrument.InstrumentID;
//    pin->ProductID=instrument.ProductID;
//    instrument_map[pin->InstrumentID]=pin;
//    vinstrument.push_back(pin);
////    SubscribeMarketData();
//    return 1;
//}



void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
//    string str=strRspInfoField(pRspInfo);
//    cout<<str<<endl;
//    LOG(ERROR)<<"OnRspSubMarketData"<<str;

}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
//    LOG(ERROR) << strRspInfoField(pRspInfo)<< endl;
}



void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
 string datastr=strDepthMarketData(pDepthMarketData);
//   LOG(ERROR)<<datastr;
    unsigned char buf[20]={'\0'};
    memset(buf,0,20);

    if (pDepthMarketData->InstrumentID == NULL||pDepthMarketData->BidPrice1 == NULL)
    {
        LOG(ERROR) << (boost::lexical_cast<string>(pDepthMarketData->InstrumentID) +"行情数据为空！！！！！");
        return;
    }


//    dii.saveDepthMarketDataToDb(pDepthMarketData);
//should lock !!!!
    InstrumentInfo*pins=dii.instruments[pDepthMarketData->InstrumentID];
    if(!pins)
    {
      LOG(ERROR)<<"can not find instrument "<<pDepthMarketData->InstrumentID;
        return;
    }
    else
    {
        pins->UpperLimitPrice=pDepthMarketData->UpperLimitPrice;
        pins->LowerLimitPrice=pDepthMarketData->LowerLimitPrice;
//        cout<<pins->instrumentID<<pins->UpperLimitPrice<<" : "<<pins->LowerLimitPrice<<endl;
          UnSubscribe(pins->instrumentID);
    }

}


bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult)
        LOG(INFO) << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
    return bResult;
}
bool CMdSpi::getStatus() const
{
    return status;
}

void CMdSpi::setStatus(bool value)
{
    status = value;
}




