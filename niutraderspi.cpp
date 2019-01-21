#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
using namespace std;
using boost::locale::conv::between;
using boost::lexical_cast;
using boost::split;
using boost::is_any_of;
using boost::algorithm::trim_copy;

#include <ThostFtdcTraderApi.h>
#include "traderspi.h"
#include "dataprocessor.h"
#include "property.h"
#include "util.h"
#include "chkthread.h"
#include "niutraderspi.h"
#include "user_order_field.h"
#include "updatethread.h"

//BrokerID统一为：9999
//标准CTP：
//    第一组：Trade Front：180.168.146.187:10000，Market Front：180.168.146.187:10010；【电信】
//    第二组：Trade Front：180.168.146.187:10001，Market Front：180.168.146.187:10011；【电信】
//    第三组：Trade Front：218.202.237.33 :10002，Market Front：218.202.237.33 :10012；【移动】
//CTPMini1：
//    第一组：Trade Front：180.168.146.187:10003，Market Front：180.168.146.187:10013；【电信】
NiuTraderSpi::NiuTraderSpi(DataInitInstance&di, string&config):dii(di)
{
    vector<string> config_list ;
    boost::split(config_list,config,boost::is_any_of("/"));
    _investorID=config_list[0];
    _password=config_list[1];
    _trade_front_addr= dii._trade_front_addr;
    _brokerID=dii.broker_id;
    if(dii.useReal==1)
    {
        _trade_front_addr=dii.realTradeFrontAddr;
         _brokerID=dii.realBrokerID;
    }

    string prefix=_investorID+"/"+dii.getTime()+"/";
    system(("mkdir  -p "+prefix).c_str());
    CThostFtdcTraderApi* pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(prefix.c_str());			// 创建UserApi
    pUserApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// 注册事件类
    pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);					// 注册公有流
    pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);					// 注册私有流

    pUserApi->RegisterFront("tcp://180.168.146.187:10001");
    pUserApi->RegisterFront("tcp://218.202.237.33:10002");
//    pUserApi->RegisterFront("tcp://180.168.146.187:10030");
      pUserApi->RegisterFront((char*)(_trade_front_addr.c_str()));

    _pUserApi=pUserApi;
}

NiuTraderSpi::NiuTraderSpi(DataInitInstance&di, string investorID, string passWord):dii(di)
{
    _trade_front_addr= dii._trade_front_addr;

    _investorID=investorID;
    _password=passWord;
    _brokerID=dii.broker_id;
    string prefix=_investorID+"/"+dii.getTime()+"/";
    system(("mkdir  -p "+prefix).c_str());
    CThostFtdcTraderApi* pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(prefix.c_str());			// 创建UserApi
    pUserApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// 注册事件类
    pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);					// 注册公有流
    pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);					// 注册私有流
    pUserApi->RegisterFront((char*)(_trade_front_addr.c_str()));
    _pUserApi=pUserApi;

    //    std::cout << "SQlite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")" << std::endl;
    //    std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;
    //    sqlite_handle=new SQLite::Database(":memory:", SQLite::OPEN_READWRITE);


    //    string TransactionRecordSql=
    //            string("CREATE TABLE TransactionRecord(")  +
    //            "Date                 TEXT     NOT NULL," +
    //            "Exchange         TEXT    NOT NULL," +
    //            "Product            TEXT     NOT NULL," +
    //            "Instrument       TEXT      NOT NULL," +
    //            "BS                    TEXT        NOT NULL," +
    //            "Price               DOUBLE     NOT NULL," +
    //            "Lots                INT         NOT NULL," +
    //            "Turnover           DOUBLE     NOT NULL," +
    //            "OC                TEXT              NOT NULL," +
    //            "Fee              DOUBLE        NOT NULL," +
    //            "RealizedPL        DOUBLE       NOT NULL," +
    //            "Trans_No        INT               NOT NULL );";
    //    sqlite_handle->exec(TransactionRecordSql);
    //    if(SQLite::OK!=sqlite_handle->getErrorCode())
    //    {
    //        cout<<sqlite_handle->getErrorMsg()<<endl;
    //    }
    //    else
    //    {
    //        cout<<"sqlite inti successfule"<<endl;
    //    }

}
NiuTraderSpi::NiuTraderSpi(DataInitInstance&di, bool loginOK, CThostFtdcTraderApi* pUserApi): dii(di),_loginOK(loginOK),_pUserApi(pUserApi)
{

}

bool NiuTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    if (bResult){
        string tmpstr = "ErrorID=";
        tmpstr.append(boost::lexical_cast<string>(pRspInfo->ErrorID));
        tmpstr.append(", ErrorMsg=");
        tmpstr.append(pRspInfo->ErrorMsg);
        LOG(WARNING)<<between(tmpstr,"UTF-8","GBK")<<endl;
    }
    return bResult;
}

void NiuTraderSpi::OnFrontConnected()
{
    LOG(WARNING) <<"------>>>>OnFrontConnected" << endl;
    ReqUserLogin();
}
void NiuTraderSpi::ReqUserLogin()
{
    //    boost::recursive_mutex::scoped_lock SLock(dii.initapi_mtx);
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));

    //    UserAccountInfo* ba=(UserAccountInfo*)_ba;
    LOG(WARNING) <<"investorID="<<_investorID<<endl;
    LOG(WARNING) <<"password="<<_password<<endl;

    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.UserID, _investorID.c_str());
    strcpy(req.Password, _password.c_str());
    int iResult = _pUserApi->ReqUserLogin(&req, ++_requestID);
    LOG(WARNING) << "<<<<----- ReqUserLogin: " << ((iResult == 0) ? "success" : "failed") << endl;

}

///请求查询结算信息确认
int NiuTraderSpi:: ReqQrySettlementInfoConfirm()
{
    CThostFtdcQrySettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(true)
    {
        int iResult = _pUserApi->ReqQrySettlementInfoConfirm(&req, ++_requestID);
        LOG(WARNING) << "<<<----- ReqQrySettlementInfoConfirm: investorID="<<_investorID << ((iResult == 0) ? "success=" : "failed  result= ") <<iResult<< endl;
        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            return -1;
    }

}

///投资者结算结果确认
void NiuTraderSpi:: ReqQrySettlementInfo()
{
    CThostFtdcQrySettlementInfoField  req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(1)
    {
        int iResult = _pUserApi->ReqQrySettlementInfo(&req, ++_requestID);
        LOG(WARNING) << "<<<<----- ReqQrySettlementInfo: " <<" investorID= "<<_investorID<<((iResult == 0) ? "success" : "failed result=  ")<<iResult << endl;
        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            return;
    }

}

void NiuTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    string str;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)){
        str=strRspUserLoginField(pRspUserLogin);
        LOG(WARNING)<<(lexical_cast<string>(this)+"----->>>>OnRspUserLogin"+str);
        // 保存会话参数

        int iNextOrderRef = lexical_cast<int>(pRspUserLogin->MaxOrderRef);
        iNextOrderRef++;

        //        UserAccountInfo* uai = (UserAccountInfo*)_ba;
        _frontID=pRspUserLogin->FrontID;
        _sessionID=pRspUserLogin->SessionID;
        _orderRef=lexical_cast<int>(pRspUserLogin->MaxOrderRef)+1;
        _brokerID=pRspUserLogin->BrokerID;


        char tradingDay[12] = {"\0"};
        strcpy(tradingDay,_pUserApi->GetTradingDay());
        LOG(WARNING) << "<<<------>>> current tradingday = " << tradingDay << endl;

        boost::this_thread::sleep(boost::posix_time::seconds(1));
        ReqQrySettlementInfo();
        //        ReqQrySettlementInfoConfirm(broker_ID,investorID);
    }
    else
    {
        str=strRspInfoField(pRspInfo);
        LOG(WARNING)<<(lexical_cast<string>(this)+"----->>>>OnRspUserLogin"+str);
    }
}

void NiuTraderSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(WARNING) << "---->>>OnRspQrySettlementInfoConfirm"<<endl;
    string investorID;
    string brokerID;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)&&pSettlementInfoConfirm)
    {
        LOG(WARNING) << "--------->>> " << "OnRspQrySettlementInfoConfirm"<<";userid="<<pSettlementInfoConfirm->InvestorID<<"date="<<
                        pSettlementInfoConfirm->ConfirmDate<<"time="<<pSettlementInfoConfirm->ConfirmTime<<endl;
        investorID=boost::lexical_cast<string>(pSettlementInfoConfirm->InvestorID);
        brokerID=boost::lexical_cast<string>(pSettlementInfoConfirm->BrokerID);
        //        _loginOK=true;
        ReqQryTradingAccount();
        return;
    }
    else
    {
        //    UserAccountInfo*uai=(UserAccountInfo*)_ba;
        LOG(WARNING) << "--------->>>OnRspQrySettlementInfoConfirm"
                     <<"investorID="<<investorID<<endl;
        ReqQrySettlementInfo();
    }
}

void NiuTraderSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(WARNING) << "----->>>OnRspQrySettlementInfo" << endl;
    if (pSettlementInfo && !IsErrorRspInfo(pRspInfo)){
        _settlemsg.append(boost::lexical_cast<string>(pSettlementInfo->Content));
    }
    if(pSettlementInfo&&!IsErrorRspInfo(pRspInfo)&&bIsLast){
        string lastmsg="--->>>OnRspQrySettlementInfo,InvestorID="+string(pSettlementInfo->InvestorID)+"\n";
        _settlemsg=between(_settlemsg,"UTF-8","GBK");
        lastmsg+=_settlemsg;
        LOG(WARNING)<<lastmsg<<endl;
        //        dii.saveTransactionRecordToSqlite(_settlemsg);
        //        SaveTransactionRecord();
        ReqSettlementInfoConfirm();
        _settlemsg.clear();
    }
}

void NiuTraderSpi::ReqSettlementInfoConfirm(){
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    int iResult = _pUserApi->ReqSettlementInfoConfirm(&req, ++_requestID);
    LOG(WARNING) << "<<<----- ReqSettlementInfoConfirm: investorID=" <<_investorID<< ((iResult == 0) ? "success" : "failed") << endl;
}

void NiuTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(WARNING) << "------->>>OnRspSettlementInfoConfirm" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)&&pSettlementInfoConfirm)
    {
        LOG(WARNING) << "----->>> OnRspSettlementInfoConfirm: investorID=" <<pSettlementInfoConfirm->InvestorID<< endl;
        //        _loginOK=true;
        ReqQryTradingAccount();
    }
}
void NiuTraderSpi::ReqQryTradingAccount(){
    //boost::recursive_mutex::scoped_lock SLock(dii.initapi_mtx);
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(true){
        int iResult = _pUserApi->ReqQryTradingAccount(&req, ++_requestID);
        LOG(WARNING) << "<<<<----- ReqQryTradingAccount: brokerID=" << _brokerID <<" investorID= "<<_investorID<<((iResult == 0) ? "success" : "failed result=  ")<<iResult << endl;
        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            return;
    }
}
void NiuTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

    LOG(WARNING)<<"-------->>>OnRspQryTradingAccount:查询投资者账户信息:"<<pTradingAccount->AccountID<<endl;
    string str;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)&&pTradingAccount )
    {
        str=strTradingAccountField(pTradingAccount) ;
        LOG(WARNING)<<str<<endl;
        //todo save account data to db;
        dii.saveCThostFtdcTradingAccountFieldToDb(pTradingAccount);
        ///请求查询投资者持仓
        //        boost::this_thread::sleep(boost::posix_time::seconds(1));
        //todo clear positon db??
        if(!_loginOK)
            ReqQryInvestorPosition();
    }
}

void NiuTraderSpi::ReqQryInvestorPosition()
{
    //    boost::recursive_mutex::scoped_lock SLock(dii.initapi_mtx);
    //clear user position from db befrom qry position in order to insert successful when OnRspQryInvestorPosition called

    mysqlpp::ScopedConnection con(*dii.mysql_pool, true);
    mysqlpp::Query query = con->query();
    string delSql="delete from ctp_investor_position where broker_id='"+_brokerID+"' and "+
            " investor_id='"+_investorID+"'";
    try
    {
        query.execute(delSql);
        query.execute("commit");
    }
    catch(const mysqlpp::BadQuery& er)
    {
        LOG(WARNING) << "exec error: " << er.what() << endl;
        query.execute("commit");
        LOG(ERROR)<<delSql;
        return ;
    }

    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(true&&(!_positon_req_send))
    {
        int iResult = _pUserApi->ReqQryInvestorPosition(&req, ++_requestID);
        LOG(WARNING) << "<<<----- ReqQryInvestorPosition: " <<"BrokerID="<<_brokerID<<";investorID="<<_investorID<< ((iResult == 0) ? " success" : "failed") <<iResult<< endl;

        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            _positon_req_send=true;
    }
}


//int NiuTraderSpi::ReqOrderInsert(UserOrderField* userOrderField)
//{
//    //CTP
//    //报单结构体
//    CThostFtdcInputOrderField req;
//    memset(&req, 0, sizeof(req));
//    ///经纪公司代码
//    strcpy(req.BrokerID, userOrderField->brokerID.c_str());
//    ///投资者代码
//    strcpy(req.InvestorID, userOrderField->investorID.c_str());
//    ///合约代码
//    strcpy(req.InstrumentID, userOrderField->instrumentID.c_str());
//    ///报单引用
//    strcpy(req.OrderRef, lexical_cast<string>(userOrderField->orderRef).c_str());
//    req.RequestID = userOrderField->requestID;
//    ///用户代码

//    ///报单价格条件: 限价
//    req.OrderPriceType  = userOrderField->orderPriceType.c_str()[0];

//    ///买卖方向:
//    req.Direction=userOrderField->direction.c_str()[0];
//    ///组合开平标志: 开仓
//    req.CombOffsetFlag[0]=userOrderField->offsetFlag.c_str()[0];

//    ///组合投机套保标志
//    req.CombHedgeFlag[0]=userOrderField->hedgeFlag.c_str()[0];

//    ///价格
//    req.LimitPrice = userOrderField->orderInsertPrice;

//    ///数量: 1
//    req.VolumeTotalOriginal = userOrderField->volume;
//    ///有效期类型: 当日有效
//    req.TimeCondition = THOST_FTDC_TC_GFD;
//    ///GTD日期
//    //	TThostFtdcDateType	GTDDate;
//    ///成交量类型: 任何数量
//    req.VolumeCondition = THOST_FTDC_VC_AV;
//    ///最小成交量: 1
//    req.MinVolume = 1;
//    ///触发条件: 立即
//    req.ContingentCondition = THOST_FTDC_CC_Immediately;
//    ///止损价
//    //	TThostFtdcPriceType	StopPrice;
//    ///强平原因: 非强平
//    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
//    ///自动挂起标志: 否
//    req.IsAutoSuspend = 0;
//    ///业务单元
//    ///请求编号

//    ///用户强评标志: 否
//    req.UserForceClose = 0;

//    int iResult =_pUserApi->ReqOrderInsert(&req,iRequestID++);
//    cerr << "--->>> ReqOrderInsert:investorID=" <<userOrderField->investorID<< ((iResult == 0) ? "success" : "failed") << endl;

//    string orderinsertstr = strInputOrderField(&req);
//    string tmp=((iResult == 0) ? "success" : "failed");
//    string msg="ReqOrderInsert:--->>> ReqOrderInsert: " + tmp+";"+orderinsertstr;
//    cout <<msg;
//    return iResult;

//}

void NiuTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(WARNING) << "------->>>>OnRspQryInvestorPosition" << endl;
    HoldPositionInfo*hp=NULL;
    if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition)
    {
        hp=initpst(pInvestorPosition);
        string info=strInvestorPositionField(pInvestorPosition);
        LOG(WARNING) << "------->>>>OnRspQryInvestorPosition" <<info<<endl;
        dii.saveThostFtdcInvestorPositionFieldToDb(pInvestorPosition);
        int tmpArbVolume = 0;
        HoldPositionInfo* tmppst = hp;
        char char_tmp_pst[10] = { '\0' };
        char char_longyd_pst[10] = { '\0' };
        char char_longtd_pst[10] = { '\0' };
        sprintf(char_tmp_pst, "%d", tmppst->longTotalPosition);
        sprintf(char_longyd_pst, "%d", tmppst->longYdPosition);
        sprintf(char_longtd_pst, "%d", tmppst->longTdPosition);

        char char_tmp_pst2[10] = { '\0' };
        char char_shortyd_pst[10] = { '\0' };
        char char_shorttd_pst[10] = { '\0' };
        sprintf(char_tmp_pst2, "%d", tmppst->shortTotalPosition);
        sprintf(char_shortyd_pst, "%d", tmppst->shortYdPosition);
        sprintf(char_shorttd_pst, "%d", tmppst->shortTdPosition);
        int currHoldPst = 0;
        int pdHoldPst = 0;
        if (tmppst->longTotalPosition == 0) {
            currHoldPst = tmppst->shortTotalPosition;
        } else if (tmppst->shortTotalPosition == 0) {
            currHoldPst = tmppst->longTotalPosition;
        }else if (tmppst->longTotalPosition >= tmppst->shortTotalPosition) {
            currHoldPst = tmppst->shortTotalPosition;
        } else {
            currHoldPst = tmppst->longTotalPosition;
        }

        if (currHoldPst == 0) {
            tmpArbVolume = 0;
        } else if (pdHoldPst == 0) {
            tmpArbVolume = 0;
        } else if (currHoldPst >= pdHoldPst) {
            tmpArbVolume = pdHoldPst;
        } else {
            tmpArbVolume = currHoldPst;
        }
        string pst_msg =
                "持仓结构:" +string(pInvestorPosition->InstrumentID) +
                ",多头持仓量=" + string(char_tmp_pst) +
                ",今仓数量=" + string(char_longtd_pst) +
                ",昨仓数量=" + string(char_longyd_pst) +
                ",可平量=" + boost::lexical_cast<string>(tmppst->longAvaClosePosition) +
                ",持仓均价=" + boost::lexical_cast<string>(tmppst->longHoldAvgPrice) +
                ";空头持仓量=" + string(char_tmp_pst2) +
                ",今仓数量=" + string(char_shorttd_pst) +
                ",昨仓数量=" + string(char_shortyd_pst) +
                ",可平量=" + boost::lexical_cast<string>(tmppst->shortAvaClosePosition) +
                ",持仓均价=" + boost::lexical_cast<string>(tmppst->shortHoldAvgPrice) +
                ";组合持仓量=" + boost::lexical_cast<string>(tmpArbVolume);
        LOG(WARNING) << pst_msg << endl;
        delete hp;
    }
    if(bIsLast)
    {
        _loginOK=true;
        _positon_req_send=false;
    }
}

///报单通知
///全部成交
//#define THOST_FTDC_OST_AllTraded '0'
///部分成交还在队列中
//#define THOST_FTDC_OST_PartTradedQueueing '1'
///部分成交不在队列中
//#define THOST_FTDC_OST_PartTradedNotQueueing '2'
///未成交还在队列中
//#define THOST_FTDC_OST_NoTradeQueueing '3'
///未成交不在队列中
//#define THOST_FTDC_OST_NoTradeNotQueueing '4'
///撤单
//#define THOST_FTDC_OST_Canceled '5'
///未知
//#define THOST_FTDC_OST_Unknown 'a'
///尚未触发
//#define THOST_FTDC_OST_NotTouched 'b'
///已触发
//#define THOST_FTDC_OST_Touched 'c'

void NiuTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    //    return ;// test
    //get config from redis
//     dii.followUser=dii.redis_con.get("followUser");
    UpdateThread&upt= UpdateThread::GetInstance();
    upt.setUpdate();
    typedef  CTraderSpi*tmacc;
    LOG(WARNING) << "--->>>OnRtnOrder  InvestorID="<<pOrder->InvestorID<<"status=" <<pOrder->OrderStatus<< endl;
    string tmpstr = strOrderField(pOrder);
    LOG(WARNING) << (lexical_cast<string>(this)+"----->>>OnRtnOrder:" + tmpstr);
    if(!_all_follow_ok)
    {
        BOOST_FOREACH(tmacc&node,_follow)
        {
            //        cout<<"follow users="<<node->investorID<<endl;
            if(!node->GetLoginOK())
            {
                LOG(WARNING) <<(lexical_cast<string>(node))<<"is not ok";
                return;
            }
        }
        LOG(WARNING) <<"all follow ok";
        _all_follow_ok=true;
    }
    if((pOrder->OrderStatus == '0')||(pOrder->OrderStatus == '1')||(pOrder->OrderStatus == '2'))
    {
        //modify status
        BOOST_FOREACH(tmacc&node,_follow)
        {
            string key=GetKey(pOrder,node);
            ChkThread &ct=  ChkThread::GetInstance();
            UserOrderField*uof=ct.get_Nuser_order(key);
            if((uof!=NULL)&&(uof->GetStatus()=='3'))
            {
                uof->SetStatus('r');
                continue;
            }
            if((uof!=NULL)&&(uof->GetStatus()=='7'))
            {
                uof->SetStatus('5');
                continue;
            }
            if(uof==NULL)// no into queue and trade directly
            {
                UserOrderField* uof = UserOrderField::CreateUserOrderField(pOrder,node);
                uof->SetStatus('r');
                cout<<"ReqOrderInsert"<<endl;
                int ret= uof->ReqOrderInsert();
                if(ret==0)
                    ct.putOrder(uof);
                else
                {
                    delete uof;
                }
                continue ;
            }
        }
    }


    if(pOrder->OrderStatus == '3')
    {
        BOOST_FOREACH(tmacc&node,_follow)
        {
            string key=GetKey(pOrder,node);
            ChkThread&ct=  ChkThread::GetInstance();
            UserOrderField*uof=ct.get_Nuser_order(key);
            int len=strlen(pOrder->ActiveUserID);
            if((uof!=NULL)||(len))
            {
                continue;
            }
            uof = UserOrderField::CreateUserOrderField(pOrder,node);
            uof->SetStatus('3');
            cout<<"ReqOrderInsert22"<<endl;
            int ret= uof->ReqOrderInsert();
            //add to map for check
            if(ret==0)
            {
                ct.putOrder(uof);
                //                userOrderField->SetStatus('3');
                //                 userOrderField->status='r';//test
            }
            else
            {
                delete uof;
            }
            continue ;
        }
    }

    if(pOrder->OrderStatus == '4')
    {
        BOOST_FOREACH(tmacc&node,_follow)
        {}
    }

    if(pOrder->OrderStatus == '5')
    {
        BOOST_FOREACH(tmacc&node,_follow)
        {
            string key=GetKey(pOrder,node);
            ChkThread&ct=  ChkThread::GetInstance();
            UserOrderField*uof=ct.get_Nuser_order(key);
            if(uof!=NULL)
            {
                if(uof->GetStatus()!='7')
                    uof->ReqOrderAction();
                uof->SetStatus('5');

            }
        }
    }

    if(pOrder->OrderStatus == 'a')
    {//cancle
        //        BOOST_FOREACH(tmacc&node,followUser)
        //        {
        //            //dii.processAction(pOrder);
        //        }
    }

    if(pOrder->OrderStatus == 'b')
    {
        BOOST_FOREACH(tmacc&node,_follow)
        {}
    }

    if(pOrder->OrderStatus == 'c')
    {
        BOOST_FOREACH(tmacc&node,_follow)
        {}
    }

    return ;

}

bool NiuTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->FrontID == _frontID) &&
            (pOrder->SessionID == _sessionID) &&
            (lexical_cast<int>(pOrder->OrderRef)==_orderRef) );
}

///成交通知
void NiuTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    LOG(WARNING) << "------->>>>OnRtnTrade InvestorID=" << pTrade->InvestorID<< endl;
    // boost::recursive_mutex::scoped_lock SLock(_spi_mtx);

    string tmpstr = strTradeField(pTrade);
    LOG(WARNING)<< ("--->>>OnRtnTrade:" + tmpstr);
    dii.saveThostFtdcTradeFieldToDb(pTrade);

    //todo modify account
    ReqQryTradingAccount();

    ReqQryInvestorPosition();
    return;//test
    //    typedef  UserAccountInfo*tmacc;
    //    vector<UserAccountInfo*>followUser=_ba->follow;
    //    BOOST_FOREACH(tmacc&node,followUser)
    //    {
    //        vector<string> tmp;
    //        split(tmp,node->ratio,is_any_of(":"));
    //        int n1=lexical_cast<int>(tmp[1]);
    //        int n2=lexical_cast<int>(tmp[0]);
    //        int totalVolume=(pTrade->Volume*n1/n2);
    //        UserOrderField* userOrderField = new UserOrderField();
    //        userOrderField->brokerID=dii.BROKER_ID;
    //        userOrderField->direction=pTrade->Direction;
    //        userOrderField->frontID=node->frontID;
    //        userOrderField->sessionID=node->sessionID;
    //        userOrderField->hedgeFlag=node->hedgeFlag;
    //        userOrderField->instrumentID=pTrade->InstrumentID;
    //        userOrderField->investorID=node->investorID;
    //        userOrderField->offsetFlag=pTrade->OffsetFlag;
    //        if(node->priceType==1)// nman price
    //        {
    //            userOrderField->orderInsertPrice=pTrade->Price;
    //            userOrderField->orderPriceType="2";
    //        }
    //        if(node->priceType==2)//market price
    //        {
    //            userOrderField->orderInsertPrice=0;
    //            userOrderField->orderPriceType="4";
    //        }

    //        userOrderField->orderRef=node->orderRef++;
    //        userOrderField->volume=totalVolume;
    ////        userOrderField->send_time=time(NULL);
    //         userOrderField->_pTraderSpi=node->pUserSpi;
    //          userOrderField->_pUserApi=node->pUserApi;
    //         CTraderSpi*pTraderSpi =( CTraderSpi*)userOrderField->_pTraderSpi;
    //        int ret=pTraderSpi->ReqOrderInsert(userOrderField);

    //      ChkThread*ct=  ChkThread::GetInstance();

    //        //add to map for check
    //      if(ret==0)
    //        ct->putOrder(userOrderField);
    //      else
    //      {
    //          delete userOrderField;
    //      }
    //      return ;
    //    }

    //todo modify position
    //ReqQryInvestorPosition(brokerID,investorID);
    //    LOG(INFO) << ("--->>>OnRtnTrade:" + tmpstr);
    //    dii.deleteOriOrder(boost::lexical_cast<string>(pTrade->OrderSysID));

}
string NiuTraderSpi::getInvestorID() const
{
    return _investorID;
}

void NiuTraderSpi::setInvestorID(const string &investorID)
{
    _investorID = investorID;
}

string NiuTraderSpi::getPassword() const
{
    return _password;
}

void NiuTraderSpi::setPassword(const string &value)
{
    _password = value;
}



vector<CTraderSpi *> & NiuTraderSpi::getFollow()
{
    return _follow;
}

void NiuTraderSpi::setFollow(const vector<CTraderSpi *> &follow)
{
    _follow = follow;
}
int NiuTraderSpi::total_trade_num() const
{
    return _total_trade_num;
}

void NiuTraderSpi::setTotal_trade_num(int total_trade_num)
{
    _total_trade_num = total_trade_num;
}
int NiuTraderSpi::profit_num() const
{
    return _profit_num;
}

void NiuTraderSpi::setProfit_num(int profit_num)
{
    _profit_num = profit_num;
}
int NiuTraderSpi::close_num() const
{
    return _close_num;
}

void NiuTraderSpi::setClose_num(int close_num)
{
    _close_num = close_num;
}
double NiuTraderSpi::profit() const
{
    return _profit;
}

void NiuTraderSpi::setProfit(double profit)
{
    _profit = profit;
}
double NiuTraderSpi::loss() const
{
    return _loss;
}

void NiuTraderSpi::setLoss(double loss)
{
    _loss = loss;
}
int NiuTraderSpi::loss_num() const
{
    return _loss_num;
}

void NiuTraderSpi::setLoss_num(int loss_num)
{
    _loss_num = loss_num;
}









void  NiuTraderSpi::startApi()
{
    _pUserApi->Init();
}

void NiuTraderSpi::SaveTransactionRecord()
{
    int start=-1,end=-1;
    int j=-1;
    vector<string> line;
    split(line,_settlemsg,is_any_of("\n"));
    BOOST_FOREACH(string&xline,line)
    {

        j++;
        std::size_t found = xline.find("成交记录 Transaction Record");
        if (found!=std::string::npos)
        {
            start=j;
        }
        found = xline.find("能源中心---INE  上期所---SHFE   中金所---CFFEX  大商所---DCE   郑商所---CZCE");
        if ((found!=std::string::npos)&&(start>0))
        {
            end=j;
            break;
        }
    }
    string sql;
    if(start>0&&end>0)
    {
        start+=5;
        end-=4;
        mysqlpp::ScopedConnection con(*dii.mysql_pool, true);
        mysqlpp::Query query = con->query();
        for(start;start<=end;start++)
        {
            vector<string> col;
            split(col,line[start],is_any_of("|"));
            sql = "replace INTO ctp_transaction_record (investorid,close_date,exchange,product,instrument,bs,price,lots,turnover,oc,fee,realizedpl,trans_no)  VALUES (";
            sql.append("'"+_investorID+"'"+",");
            sql.append("'"+trim_copy(col[1])+"'"+",");
            sql.append("'"+trim_copy(col[2])+"'"+",");
            sql.append("'"+trim_copy(col[3])+"'"+",");
            sql.append("'"+trim_copy(col[4])+"'"+",");
            sql.append("'"+trim_copy(col[5])+"'"+",");
            sql.append(col[7]+",");
            sql.append(col[8]+",");
            sql.append(col[9]+",");
            sql.append("'"+trim_copy(col[10])+"'"+",");
            sql.append(col[11]+",");
            sql.append(col[12]+",");
            sql.append(col[14]+"); ");
            try
            {
                query.execute(sql);
                query.execute("commit");
            }
            catch (const mysqlpp::Exception& er)
            {
                LOG(ERROR)  << "exec error: " << er.what() <<"  "<<sql<< endl;
                query.execute("commit");
                // return ;
            }
            //            sqlite_handle->exec(sql);
        }

        //        _total_trade_num = sqlite_handle->execAndGet("select sum(Lots) from  TransactionRecord;");
        //        LOG(WARNING)<< "_total_trade_num=" << _total_trade_num;

        //        _profit_num = sqlite_handle->execAndGet("select count(*) from TransactionRecord where RealizedPL>0;");
        //        LOG(WARNING)<< "_profit_num=" << _profit_num ;

        //        _close_num = sqlite_handle->execAndGet("select count(*) from TransactionRecord where OC not in ('开');");
        //        LOG(WARNING) << "_close_num=" << _close_num ;

        //        _profit= sqlite_handle->execAndGet("select sum(RealizedPL) from TransactionRecord where RealizedPL>0;");
        //        LOG(WARNING)<< "_profit=" << _profit ;

        //        _loss = sqlite_handle->execAndGet("select sum(RealizedPL) from TransactionRecord where RealizedPL<0;");
        //        LOG(WARNING)<< "_loss=" << _loss;

        //        _loss_num = sqlite_handle->execAndGet("select count(*) from TransactionRecord where RealizedPL<0;");
        //        LOG(WARNING)<< "_loss_num=" << _loss_num ;
    }
}

HoldPositionInfo* NiuTraderSpi:: initpst(CThostFtdcInvestorPositionField *pInvestorPosition)
{
    //    boost::recursive_mutex::scoped_lock SLock(dii.pst_mtx);
    ///合约代码
    char	*InstrumentID = pInvestorPosition->InstrumentID;
    string str_instrumentid = string(InstrumentID);
    ///持仓多空方向
    TThostFtdcPosiDirectionType	dir = pInvestorPosition->PosiDirection;
    char PosiDirection[] = { dir,'\0' };
    ///投机套保标志
    TThostFtdcHedgeFlagType	flag = pInvestorPosition->HedgeFlag;
    char HedgeFlag[] = { flag,'\0' };
    ///上日持仓
    TThostFtdcVolumeType	ydPosition = pInvestorPosition->YdPosition;
    ///今日持仓
    TThostFtdcVolumeType	position = pInvestorPosition->Position;
    ///
    string str_dir = string(PosiDirection);
    ///
    double multiplier=0; //= dii.getMultipler(str_instrumentid);
    ///持仓日期
    TThostFtdcPositionDateType	positionDate = pInvestorPosition->PositionDate;
    char PositionDate[] = { positionDate,'\0' };
    string str_pst_date = string(PositionDate);


    // no position
    HoldPositionInfo* tmpinfo = new HoldPositionInfo();
    if ("2" == str_dir)
    {//买  //多头
        tmpinfo->longTotalPosition = position;
        tmpinfo->longAvaClosePosition = position;
        tmpinfo->longAmount = pInvestorPosition->PositionCost;
        tmpinfo->longHoldAvgPrice = pInvestorPosition->PositionCost / (multiplier*position);
        //空头
        tmpinfo->shortTotalPosition = 0;
        if ("2" == str_pst_date) {//昨仓
            tmpinfo->longYdPosition = position;
        } else if ("1" == str_pst_date) {//今仓
            tmpinfo->longTdPosition = position;
        }
        return tmpinfo;
    }
    else if ("3" == str_dir)
    {//空
        //空头
        tmpinfo->longTotalPosition = 0;
        tmpinfo->shortTotalPosition = position;
        tmpinfo->shortAvaClosePosition = position;
        tmpinfo->shortAmount = pInvestorPosition->PositionCost;
        tmpinfo->shortHoldAvgPrice = pInvestorPosition->PositionCost / (multiplier*position);
        if ("2" == str_pst_date)
        {//昨仓
            tmpinfo->shortYdPosition = position;
        }
        else if ("1" == str_pst_date) {//今仓
            tmpinfo->shortTdPosition = position;
        }
        return tmpinfo;
    }
    else
    {
        //        LOG(ERROR) << string(InstrumentID) + ";error:持仓类型无法判断PosiDirection=" + str_dir;
        LOG(WARNING)<< string(InstrumentID) <<";error:持仓类型无法判断PosiDirection="<<str_dir<<endl;
        return tmpinfo;
    }

}
