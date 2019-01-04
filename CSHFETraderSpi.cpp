#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
using namespace std;

#include "CSHFETraderSpi.h"
#include "DataProcessor.h"
extern DataInitInstance* dii;
#pragma warning(disable : 4996)
/* TraderSpi interface */
void CSHFETraderSpi::OnFrontConnected()
{
    int32_t rv;

    cout<<"trade---->OnFrontConnected"<<endl;

    CShfeFtdcReqUserLoginField reqUserLogin;
    memset(&reqUserLogin, 0, sizeof(CShfeFtdcReqUserLoginField));

    strcpy(reqUserLogin.TradingDay, dii->cshfeTraderApi->GetTradingDay());

    strcpy(reqUserLogin.ParticipantID, dii->exgParticipantIDCSHFE.c_str());
    strcpy(reqUserLogin.UserID, dii->exgTraderIDCSHFE.c_str());
    strcpy(reqUserLogin.Password, dii->exgTraderPasswdCSHFE.c_str());

    //strcpy(reqUserLogin.UserProductInfo, user_product_info.c_str());

    reqUserLogin.DataCenterID = 1;
    /******************************/
    cout<<"trade---->ReqUserLogin"<<endl;
    rv = dii->cshfeTraderApi->ReqUserLogin(&reqUserLogin, 0);
    if ( rv )
    {
        LOG(ERROR)<<"ReqLogin Failed";
    }

    return;
}

void CSHFETraderSpi::OnFrontDisconnected(int nReason)
{
    LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi:: OnHeartBeatWarning(int nTimeLapse)
{
    LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi::OnPackageStart(int nTopicID, int nSequenceNo)
{
    if(dii->start_process == 0){
        return;
    }
    if ( (uint32_t)nTopicID == dii->private_stream_topic_id ){
        dii->cur_private_stream_sequence_number = nSequenceNo;
        if ( (uint32_t)nSequenceNo > dii->max_private_stream_sequence_number )
        {
            dii->max_private_stream_sequence_number = nSequenceNo;
            dii->b_new_ps_package = 1;
        }
        else
        {
            dii->b_new_ps_package = 0;
        }
    }
    //LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi::OnPackageEnd(int nTopicID, int nSequenceNo)
{
    if(dii->start_process == 0){
        return;
    }
    dii->b_new_ps_package = 0;
    //LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi::OnRspError(CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi::OnRspUserLogin(CShfeFtdcRspUserLoginField *pRspUserLogin, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(ERROR)<<"OnRspUserLogin";
    if ( bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        int data_center_id = pRspUserLogin->DataCenterID;
        //int max_order_local_id = pRspUserLogin->MaxOrderLocalID;
        //connection_state = connection_state_ready;

        //trader_spi->OnRspMaxOrderLocalID(pRspUserLogin->MaxOrderLocalID, exchange_id);

        string str = "OnRspUserLogin:OK:" +
                     boost::lexical_cast<std::string>(data_center_id) + ":" +
                     boost::lexical_cast<std::string>(pRspUserLogin->MaxOrderLocalID);
        cout<<str<<endl;
        dii->isExgOk=true;
    }else{
        string err_str = "OnRspUserLogin:Fail:";
        cout<<err_str<<endl;
    }
    return;
}
void CSHFETraderSpi::OnRspUserLogout(CShfeFtdcRspUserLogoutField *pRspUserLogout, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi::OnRspOrderInsert(CShfeFtdcInputOrderField *pInputOrder, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if(dii->start_process == 0){
        return;
    }
    //LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
void CSHFETraderSpi::OnRspOrderAction(CShfeFtdcOrderActionField *pOrderAction, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    if(dii->start_process == 0){
        return;
    }
    //LOG(ERROR)<<"OnFrontDisconnected";
    return;
}
///成交回报
void CSHFETraderSpi::OnRtnTrade(CShfeFtdcTradeField *pTrade){
    //cout<<"------------------------>>>OnRtnTrade"<<endl;
    //LOG(INFO)<<"OnRtnTrade";
    if ( dii->b_new_ps_package )
    {

    }
}

///报单回报
void CSHFETraderSpi::OnRtnOrder(CShfeFtdcOrderField *pOrder){
    //cout<<"------------------------>>>OnRtnOrder"<<endl;

    if ( dii->b_new_ps_package )
    {
        LOG(INFO)<<"OnRtnOrder";
        if(pOrder->OrderStatus=='0'){//trade order
            string userid=boost::lexical_cast<string>(pOrder->UserID);
            string clientid=boost::lexical_cast<string>(pOrder->ClientID);
            string direction=boost::lexical_cast<string>(pOrder->Direction);
            string offsetflag=boost::lexical_cast<string>(pOrder->CombOffsetFlag);
            string instrumentid=boost::lexical_cast<string>(pOrder->InstrumentID);
            string msg="tradingDay="+boost::lexical_cast<string>(pOrder->ActionDay)+";"
                    +"userid="+userid+";"
                    +"clientID="+clientid+";"
                    +"instrumentID="+instrumentid+";"
                    +"orderStatus="+boost::lexical_cast<string>(pOrder->OrderStatus)+";"
                    +"OrderSysID="+boost::lexical_cast<string>(pOrder->OrderSysID)+";"
                    +"direction="+direction+";"
                    +"offsetflag="+offsetflag+";"
                    +"price="+boost::lexical_cast<string>(pOrder->LimitPrice)+";"
                    +"volume="+boost::lexical_cast<string>(pOrder->VolumeTotalOriginal)+";";
            cout<<msg<<endl;
            LOG(INFO)<<msg;
            /*
            unordered_map<string, vector<string>>::iterator nbIT=dii->NBAccountMap.find(clientid);
            if(nbIT!=dii->NBAccountMap.end()){//this is subscripted account
                vector<string> follows=nbIT->second;
                for(vector<string>::iterator flwIT=follows.begin();flwIT!=follows.end();flwIT++){
                    string followAcc=(*flwIT);
                    //find follow account info order inserting
                    unordered_map<string, BaseAccount*>::iterator iter=dii->followNBAccountMap.find(followAcc);
                    if(iter!=dii->followNBAccountMap.end()){
                        string flwmsg="";
                        flwmsg="account="+followAcc+" is following clientid="+clientid+",order:"+msg;
                        LOG(INFO)<<flwmsg;
                        string ivestorID=iter->first;
                        UserAccountInfo* uai=(UserAccountInfo*)iter->second;
                        //CTPInterface* interface=dii->getTradeApi(ivestorID);
                        //CTraderSpi *userSpi=(CTraderSpi*)interface->pUserSpi;
                        cout<<"add order investor="<<iter->first<<";uai="<<uai->investorID<<endl;
                        UserOrderField* userOrderField = new UserOrderField();
                        userOrderField->brokerID=dii->BROKER_ID;
                        userOrderField->direction=direction;
                        userOrderField->frontID=uai->frontID;
                        userOrderField->sessionID=uai->sessionID;
                        userOrderField->hedgeFlag=uai->hedgeFlag;
                        userOrderField->instrumentID=instrumentid;
                        userOrderField->investorID=ivestorID;
                        userOrderField->offsetFlag=offsetflag;
                        userOrderField->orderInsertPrice=pOrder->LimitPrice;
                        string priceType="2";
                        strcpy(userOrderField->orderPriceType,priceType.c_str());
                        userOrderField->orderRef=uai->orderRef++;
                        userOrderField->volume=1;
                        dii->addNewOrderInsert(userOrderField);
                    }
                }
            }*/

        }
    }
}

//委托有错误时，才会有该报文；否则 pRspInfo本身就是空指针。
bool CSHFETraderSpi::IsErrorRspInfo(CShfeFtdcRspInfoField *pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult){
        string tmpstr = "--->>> ErrorID=";
        char tmpchar[1024];
        tmpstr.append(boost::lexical_cast<string>(pRspInfo->ErrorID));
        tmpstr.append(", ErrorMsg=");
        tmpstr.append(pRspInfo->ErrorMsg);
        strcpy(tmpchar,tmpstr.c_str());
        char errMsg[1024];
        dii->codeCC->convert(tmpchar,strlen(tmpchar),errMsg,1024);
        //cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << errMsg << endl;
        LOG(ERROR)<<errMsg;
        //loglist.push_back(tmpstr);
        //tradequeue.push_back(tmpstr);
    }
    return bResult;
}
