#pragma comment (lib, "thosttraderapi.lib")
#define WIN32_LEAN_AND_MEAN     // 在#include<windows.h>前定义
#include "MdSpi.h"

//#include <windows.h>
#include "./lib/ThostFtdcTraderApi.h"
#include "TraderSpi.h"
#include "property.h"
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
using namespace std;
extern DataInitInstance dii;
#pragma warning(disable : 4996)
extern string systemID;//系统编号，每个产品一个编号
extern bool isLogout;
extern string tradingDayT;
//存放行情消息队列
extern list<string> mkdata;
// 请求编号
extern int iRequestID;
//行情各字段分割符
char sep[] = ";";
// USER_API参数
extern CThostFtdcMdApi* mdUserApi;
void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	cout << "market" + boost::lexical_cast<string>(PthreadSelf());
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
    for( unordered_map<string, BaseAccount*>::iterator iter=dii.followNBAccountMap.begin();iter!=dii.followNBAccountMap.end();iter++ ){
           cout<<"key="<<iter->first<<endl;
           BaseAccount* ba=iter->second;
           string ivestorID=iter->first;
           strcpy(req.BrokerID, dii.BROKER_ID.c_str());
           strcpy(req.UserID, ivestorID.c_str());
           strcpy(req.Password, ba->password.c_str());
           int iResult = mdUserApi->ReqUserLogin(&req, 9);
           cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
           break;
    }

	//
	// 初始化UserApi
	/*
	ptradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
	pTradeUserSpi = new CTraderSpi();
	ptradeApi->RegisterSpi((CThostFtdcTraderSpi*)pTradeUserSpi);			// 注册事件类
	ptradeApi->SubscribePublicTopic(TERT_RESUME);					// 注册公有流
	ptradeApi->SubscribePrivateTopic(TERT_RESUME);					// 注册私有流
	ptradeApi->RegisterFront(TRADE_FRONT_ADDR);
	ptradeApi->Init();
	*/
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << mdUserApi->GetTradingDay() << endl;
		SubscribeMarketData();
	}
}

void CMdSpi::SubscribeMarketData()
{
    int iResult=-1;
    //int iResult =mdUserApi->SubscribeMarketData(dii.ppInstrumentID, dii.iInstrumentID);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "=========================>hello " << pSpecificInstrument->InstrumentID << endl;
	string mkinfo;
	mkinfo.append("InstrumentID=");
	mkinfo.append(pSpecificInstrument->InstrumentID);
	cerr << __FUNCTION__ << endl;
    dii.isDoneSometh=true;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}
int cou=1;
bool tttt=false;
void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    //cout<<"start="<<boost::lexical_cast<string>(dii->start_process)<<endl;
    if(dii.start_process == 0){
        return;
    }
    unsigned char buf[20]={'\0'};
    memset(buf,0,20);
    if (abs(pDepthMarketData->AskPrice1) > 10000000000 || abs(pDepthMarketData->LastPrice) > 10000000000 ) {
        LOG(ERROR) << (boost::lexical_cast<string>(pDepthMarketData->InstrumentID) + "初始化行情出现问题;");
        return;
    }
    if (pDepthMarketData->InstrumentID == NULL||pDepthMarketData->BidPrice1 == NULL) {

        LOG(ERROR) << (boost::lexical_cast<string>(pDepthMarketData->InstrumentID) +"行情数据为空！！！！！");
        return;
    }
    //4she5ru
    int a = (pDepthMarketData->LastPrice + 0.005)*100;
    pDepthMarketData->LastPrice = a/100;
    a = (pDepthMarketData->BidPrice1 + 0.005)*100;
    pDepthMarketData->BidPrice1 = a/100;
    a = (pDepthMarketData->AskPrice1 + 0.005)*100;
    pDepthMarketData->AskPrice1 = a/100;
    string instrumentid = pDepthMarketData->InstrumentID;
    double lastPrice = pDepthMarketData->LastPrice;
    int volume = pDepthMarketData->Volume;
    double bidprice = pDepthMarketData->BidPrice1;
    double askprice = pDepthMarketData->AskPrice1;
    string tmpstr = "instrumentID=" + instrumentid + ";volume=" + boost::lexical_cast<string>(volume) + ";lastprice=" + boost::lexical_cast<string>(lastPrice) +
        ";bidprice=" + boost::lexical_cast<string>(bidprice) + ";askprice=" + boost::lexical_cast<string>(askprice);
    //cout<<tmpstr<<endl;
    //锁持仓处理
    boost::recursive_mutex::scoped_lock SLock(dii.pst_mtx);
    /*
    if(!tttt){
        tttt=true;
        string investorID="029797";
        unordered_map<string, BaseAccount*>::iterator iter=dii->followNBAccountMap.find("029797");
        UserAccountInfo* uai=(UserAccountInfo*)iter->second;
        CTPInterface* interface=dii->getTradeApi(investorID);
        CTraderSpi *userSpi=(CTraderSpi*)interface->pUserSpi;
        UserOrderField* userOrderField = new UserOrderField();
        userOrderField->brokerID="9999";
        userOrderField->direction="0";
        userOrderField->frontID=uai->frontID;
        userOrderField->sessionID=uai->sessionID;
        userOrderField->hedgeFlag=uai->hedgeFlag;
        userOrderField->instrumentID=instrumentid;
        userOrderField->investorID=investorID;
        userOrderField->offsetFlag="0";
        userOrderField->orderInsertPrice=askprice;
        string priceType="2";
        strcpy(userOrderField->orderPriceType,priceType.c_str());
        userOrderField->orderRef=iRequestID++;
        userOrderField->volume=1;
        dii->addNewOrderInsert(userOrderField);
    }else{
        return;
    }
*/
    if(cou>20){
        for( unordered_map<string, BaseAccount*>::iterator iter=dii.followNBAccountMap.begin();iter!=dii.followNBAccountMap.end();iter++ ){
               //cout<<"key="<<iter->first<<endl;
               string ivestorID=iter->first;
               if(!dii.orderIsResbonsed(ivestorID)){
                    return;
               }
               UserAccountInfo* uai=(UserAccountInfo*)iter->second;
               CTPInterface* interface=dii.getTradeApi(ivestorID);
               CTraderSpi *userSpi=(CTraderSpi*)interface->pUserSpi;
               unordered_map<string, HoldPositionInfo*>::iterator psIT=uai->positionmap.find(instrumentid);
               if(psIT!=uai->positionmap.end()){
                   HoldPositionInfo* hpi = psIT->second;
                   UserOrderField* userOrderField = new UserOrderField();
                   userOrderField->brokerID=dii.BROKER_ID;
                   if(hpi->longAvaClosePosition>0){
                        userOrderField->direction="1";
                        userOrderField->offsetFlag="1";
                        userOrderField->orderInsertPrice=askprice+dii.getTickMetric(instrumentid)*2;
                        userOrderField->frontID=uai->frontID;
                        userOrderField->sessionID=uai->sessionID;
                        userOrderField->hedgeFlag=uai->hedgeFlag;
                        userOrderField->instrumentID=instrumentid;
                        userOrderField->investorID=ivestorID;
                        string priceType="2";
                        strcpy(userOrderField->orderPriceType,priceType.c_str());
                        userOrderField->orderRef=iRequestID++;
                        userOrderField->volume=1;
                        dii.addNewOrderInsert(userOrderField);
                        break;
                   }else if(hpi->shortAvaClosePosition>0){
                       userOrderField->direction="0";
                       userOrderField->offsetFlag="1";
                       userOrderField->orderInsertPrice=bidprice-dii.getTickMetric(instrumentid)*2;
                       userOrderField->frontID=uai->frontID;
                       userOrderField->sessionID=uai->sessionID;
                       userOrderField->hedgeFlag=uai->hedgeFlag;
                       userOrderField->instrumentID=instrumentid;
                       userOrderField->investorID=ivestorID;
                       string priceType="2";
                       strcpy(userOrderField->orderPriceType,priceType.c_str());
                       userOrderField->orderRef=iRequestID++;
                       userOrderField->volume=1;
                       dii.addNewOrderInsert(userOrderField);
                       break;
                   }else if(hpi->shortTotalPosition==0&&hpi->longTotalPosition==0){
                       uai->positionmap.erase(psIT);
                   }
                   //break;
               }

        }
        for( unordered_map<string, BaseAccount*>::iterator iter=dii.followNBAccountMap.begin();iter!=dii.followNBAccountMap.end();iter++ ){
               //cout<<"key="<<iter->first<<endl;
               string ivestorID=iter->first;
               UserAccountInfo* uai=(UserAccountInfo*)iter->second;
               CTPInterface* interface=dii.getTradeApi(ivestorID);
               CTraderSpi *userSpi=(CTraderSpi*)interface->pUserSpi;
               if(uai->positionmap.size()!=0){
                    return;
               }
        }
        cou=0;
    }else{
        for( unordered_map<string, BaseAccount*>::iterator iter=dii.followNBAccountMap.begin();iter!=dii.followNBAccountMap.end();iter++ ){

               string ivestorID=iter->first;
               UserAccountInfo* uai=(UserAccountInfo*)iter->second;
               //CTPInterface* interface=dii->getTradeApi(ivestorID);
               //CTraderSpi *userSpi=(CTraderSpi*)interface->pUserSpi;
               cout<<"add order investor="<<iter->first<<";uai="<<uai->investorID<<endl;
               UserOrderField* userOrderField = new UserOrderField();
               userOrderField->brokerID=dii.BROKER_ID;
               userOrderField->direction="0";
               userOrderField->frontID=uai->frontID;
               userOrderField->sessionID=uai->sessionID;
               userOrderField->hedgeFlag=uai->hedgeFlag;
               userOrderField->instrumentID=instrumentid;
               userOrderField->investorID=ivestorID;
               userOrderField->offsetFlag="0";
               userOrderField->orderInsertPrice=askprice;
               string priceType="2";
               strcpy(userOrderField->orderPriceType,priceType.c_str());
               userOrderField->orderRef=iRequestID++;
               userOrderField->volume=1;
               dii.addNewOrderInsert(userOrderField);
               cou+=1;

        }
    }


}


bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}



