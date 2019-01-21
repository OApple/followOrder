#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <ThostFtdcTraderApi.h>


using namespace std;
using boost::locale::conv::between;
using boost::lexical_cast;
using boost::split;
using boost::algorithm::trim_copy;
using boost::is_any_of;

#include "traderspi.h"
#include "dataprocessor.h"
#include "property.h"
#include "util.h"
#include "chkthread.h"
#include "user_order_field.h"

#pragma warning(disable : 4996)

//126373/123456/1:1/1/1
CTraderSpi:: CTraderSpi(DataInitInstance&di, string&config):dii(di)
{
    vector<string> config_list ;
    boost::split(config_list,config,boost::is_any_of("/"));
     _investorID=config_list[0];
    password=config_list[1];
    ratio=config_list[2];
    priceType=boost::lexical_cast<int>(config_list[3]);
    followTick=boost::lexical_cast<int>(config_list[4]);


    _trade_front_addr= dii._trade_front_addr;
    _brokerID=dii.broker_id;

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


CTraderSpi::CTraderSpi( DataInitInstance &di,
                        bool loginOK,
                        CThostFtdcTraderApi* pUserApi):dii(di),_loginOK(loginOK),_pUserApi(pUserApi)
{}

// CTraderSpi:: CTraderSpi()
//  {

//  }
//void CTraderSpi::update_followusers(unordered_map<string, UserAccountInfo*> followusers)
//{


//}


void CTraderSpi::OnFrontConnected()
{
    LOG(INFO) <<(lexical_cast<string>(this)+"------>>>>OnFrontConnected") ;
    ReqUserLogin();
}
void CTraderSpi::ReqUserLogin()
{
    //    boost::recursive_mutex::scoped_lock SLock(dii.initapi_mtx);
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));


    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.UserID, _investorID.c_str());
    strcpy(req.Password, password.c_str());
    int iResult = _pUserApi->ReqUserLogin(&req, ++iRequestID);
    string strreq=strReqUserLoginField(&req);
    LOG(INFO) << (lexical_cast<string>(this)+"<<<<----- ReqUserLogin: " + ((iResult == 0) ? "success" : "failed")+strreq );

}

///请求查询结算信息确认
int CTraderSpi:: ReqQrySettlementInfoConfirm()
{
    CThostFtdcQrySettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(true)
    {
        int iResult = _pUserApi->ReqQrySettlementInfoConfirm(&req, ++iRequestID);
        LOG(INFO) << lexical_cast<string>(this) << "<<<----- ReqQrySettlementInfoConfirm: investorID="<<_investorID << ((iResult == 0) ? "success=" : "failed  result= ") <<iResult<< endl;
        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            return -1;
    }

}

///投资者结算结果确认
void CTraderSpi:: ReqQrySettlementInfo()
{
    CThostFtdcQrySettlementInfoField  req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(1)
    {
        int iResult = _pUserApi->ReqQrySettlementInfo(&req, ++iRequestID);
        LOG(INFO) << lexical_cast<string>(this) << "<<<<----- ReqQrySettlementInfo: " <<" investorID= "<<_investorID<<((iResult == 0) ? "success" : "failed result=  ")<<iResult << endl;
        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            return;
    }

}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //    cerr << "----->>>>OnRspUserLogin" << endl;
    string str;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        str=strRspUserLoginField(pRspUserLogin);
        LOG(INFO)<<(lexical_cast<string>(this)+"----->>>>OnRspUserLogin"+str);
        // 保存会话参数
        frontID = pRspUserLogin->FrontID;
        sessionID= pRspUserLogin->SessionID;
        orderRef= lexical_cast<int>(pRspUserLogin->MaxOrderRef)+1;
        _brokerID=pRspUserLogin->BrokerID;

        char tradingDay[12] = {"\0"};
        strcpy(tradingDay,_pUserApi->GetTradingDay());
        LOG(INFO) << lexical_cast<string>(this) << "<<<------>>> current tradingday = " << tradingDay << endl;

        boost::this_thread::sleep(boost::posix_time::seconds(1));
        ReqQrySettlementInfo();
        //        ReqQrySettlementInfoConfirm();
    }
    else
    {
        str=strRspInfoField(pRspInfo);
        LOG(INFO)<<(lexical_cast<string>(this)+"----->>>>OnRspUserLogin"+str);
        LOG(INFO) << lexical_cast<string>(this)<<lexical_cast<string>(pRspUserLogin->UserID)+" login errori."<<endl;
    }
}

void CTraderSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(INFO) << lexical_cast<string>(this) << "---->>>OnRspQrySettlementInfoConfirm"<<endl;
    string investorID;
    string brokerID;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)&&pSettlementInfoConfirm)
    {
        LOG(INFO) << lexical_cast<string>(this) << "--------->>> " << "OnRspQrySettlementInfoConfirm"<<";userid="<<pSettlementInfoConfirm->InvestorID<<"date="<<
                     pSettlementInfoConfirm->ConfirmDate<<"time="<<pSettlementInfoConfirm->ConfirmTime<<endl;
        investorID=boost::lexical_cast<string>(pSettlementInfoConfirm->InvestorID);
        brokerID=boost::lexical_cast<string>(pSettlementInfoConfirm->BrokerID);
        //        _loginOK=true;
        ReqQryTradingAccount();
        return;
    }
    else
    {
        LOG(INFO) << lexical_cast<string>(this) << "--------->>>OnRspQrySettlementInfoConfirm"
                  <<"investorID="<<investorID<<endl;
        ReqQrySettlementInfo();
    }
}

void CTraderSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(INFO) << lexical_cast<string>(this) << "----->>>OnRspQrySettlementInfo" << endl;
    if (pSettlementInfo && !IsErrorRspInfo(pRspInfo)){
        _settlemsg.append(boost::lexical_cast<string>(pSettlementInfo->Content));
    }
    if(pSettlementInfo&&!IsErrorRspInfo(pRspInfo)&&bIsLast){
        string lastmsg="--->>>OnRspQrySettlementInfo,InvestorID="+string(pSettlementInfo->InvestorID)+"\n";
        _settlemsg=between(_settlemsg,"UTF-8","GBK");
        lastmsg+=_settlemsg;
        LOG(INFO) << lexical_cast<string>(this)<<lastmsg<<endl;

        SaveTransactionRecord();
        ReqSettlementInfoConfirm();
        _settlemsg.clear();
    }
}

void CTraderSpi::ReqSettlementInfoConfirm(){
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    int iResult = _pUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
    LOG(INFO) << lexical_cast<string>(this) << "<<<----- ReqSettlementInfoConfirm: investorID=" <<_investorID<< ((iResult == 0) ? "success" : "failed") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(INFO) << lexical_cast<string>(this) << "------->>>OnRspSettlementInfoConfirm" << endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)&&pSettlementInfoConfirm)
    {
        LOG(INFO) << lexical_cast<string>(this) << "----->>> OnRspSettlementInfoConfirm: investorID=" <<pSettlementInfoConfirm->InvestorID<< endl;
        //        _loginOK=true;
        ReqQryTradingAccount();
    }
}

//void CTraderSpi::ReqQryInstrument()
//{
//	CThostFtdcQryInstrumentField req;
//	memset(&req, 0, sizeof(req));
//    //strcpy(req.InstrumentID, dii->INSTRUMENT_ID);
//    boost::this_thread::sleep(boost::posix_time::seconds(1));
//  //  CTPInterface* interface=dii.getTradeApi(dii.loginInvestorID);
//    int iResult = _pUserApi->ReqQryInstrument(&req, ++iRequestID);
//            //dii->pUserApi->ReqQryInstrument(&req, ++iRequestID);
//    cerr << "--->>> ReqQryInstrument: " << ((iResult == 0) ? "success" : "failed") << endl;
//}

//void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//     cerr << "-------->>>OnRspQryInstrument" << endl;
//    if (pInstrument) {
//        dii.processRspReqInstrument(pInstrument);
//    }
//    if (bIsLast && !IsErrorRspInfo(pRspInfo))
//    {
//        string msg="OnRspQryInstrument is done.";
//        LOG(INFO)<<msg;
//        cout<<msg<<endl;
//        dii.isDoneSometh=true;
//    }
//}

void CTraderSpi::ReqQryTradingAccount(){
    //boost::recursive_mutex::scoped_lock SLock(dii.initapi_mtx);
    CThostFtdcQryTradingAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(true){
        int iResult = _pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
        string tmp = string("<<<<----- ReqQryTradingAccount: brokerID=" )+
                ((iResult == 0) ? "success" : "failed result=  ")+
                lexical_cast<string>(iResult);
        LOG(INFO)<<(lexical_cast<string>(this)+tmp);
        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            return;
    }
}
void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

    //    LOG(INFO) << lexical_cast<string>(this)<<"-------->>>OnRspQryTradingAccount:查询投资者账户信息:"<<pTradingAccount->AccountID<<endl;
    string str;
    if (bIsLast && !IsErrorRspInfo(pRspInfo)&&pTradingAccount )
    {
        str=strTradingAccountField(pTradingAccount);
        LOG(INFO)<<(lexical_cast<string>(this)+"-------->>>OnRspQryTradingAccount:"+str);
        //cout<<str<<endl;
        //todo save account data to db;
        dii.saveCThostFtdcTradingAccountFieldToDb(pTradingAccount);
        ///请求查询投资者持仓
        //        boost::this_thread::sleep(boost::posix_time::seconds(1));
        //todo clear positon db??
        if(!_loginOK)
            ReqQryInvestorPosition();
    }
    else
    {

    }
}

void CTraderSpi::ReqQryInvestorPosition()
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
        LOG(INFO) << lexical_cast<string>(this) << "exec error: " << er.what() << endl;
        query.execute("commit");
        LOG(ERROR)<<delSql;
        return ;
    }
    //  catch(const mysqlpp::ConnectionFailed& er)
    //    {

    //    }
    CThostFtdcQryInvestorPositionField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    while(true&&(!_positon_req_send))
    {
        int iResult = _pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
        LOG(INFO) << lexical_cast<string>(this) << "<<<----- ReqQryInvestorPosition: " <<"BrokerID="<<_brokerID<<";investorID="<<_investorID<< ((iResult == 0) ? " success" : "failed") <<iResult<< endl;

        if(iResult)
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        else
            _positon_req_send=true;
    }
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //    cerr << "------->>>>OnRspQryInvestorPosition" << endl;
    HoldPositionInfo*hp=NULL;
    if (!IsErrorRspInfo(pRspInfo) && pInvestorPosition)
    {
        hp=initpst(pInvestorPosition);
        string info=strInvestorPositionField(pInvestorPosition);
        LOG(INFO)<<lexical_cast<string>(this)<<"------->>>>OnRspQryInvestorPosition"<<info;
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
        //        cout << pst_msg << endl;
        LOG(INFO) << lexical_cast<string>(this)<<pst_msg;
        delete hp;
    }
    if(bIsLast)
    {
        _loginOK=true;
        _positon_req_send=false;
    }
}

void CTraderSpi::SaveTransactionRecord()
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
        //        LOG(INFO)<< "total_trade_num=" << _total_trade_num;

        //        _profit_num = sqlite_handle->execAndGet("select count(*) from TransactionRecord where RealizedPL>0;");
        //        LOG(INFO)<< "yinglibi shu=" << _profit_num ;

        //        _close_num = sqlite_handle->execAndGet("select count(*) from TransactionRecord where OC not in ('开');");
        //        LOG(INFO) << "close all=" << _close_num ;

        //        _profit= sqlite_handle->execAndGet("select sum(RealizedPL) from TransactionRecord where RealizedPL>0;");
        //        LOG(INFO)<< "profit=" << _profit ;

        //        _loss = sqlite_handle->execAndGet("select sum(RealizedPL) from TransactionRecord where RealizedPL<0;");
        //        LOG(INFO)<< "loss=" << _loss;

        //        _loss_num = sqlite_handle->execAndGet("select count(*) from TransactionRecord where RealizedPL<0;");
        //        LOG(INFO)<< "loss count=" << _loss_num ;
    }
}

//初始化持仓信息
HoldPositionInfo* CTraderSpi:: initpst(CThostFtdcInvestorPositionField *pInvestorPosition)
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
        LOG(ERROR) << string(InstrumentID) + ";error:持仓类型无法判断PosiDirection=" + str_dir;
        return tmpinfo;
    }

}



int CTraderSpi::ReqOrderInsert(UserOrderField* userOrderField)
{
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    ///经纪公司代码
    strcpy(req.BrokerID, userOrderField->brokerID.c_str());

    ///投资者代码
    strcpy(req.InvestorID, userOrderField->_investorID.c_str());

    ///合约代码
    strcpy(req.InstrumentID, userOrderField->_instrumentID.c_str());

    ///报单引用
    strcpy(req.OrderRef, lexical_cast<string>(userOrderField->_order_ref).c_str());

    ///用户代码
    strcpy(req.UserID,userOrderField->_investorID.c_str());

    ///报单价格条件: 限价
    req.OrderPriceType  = userOrderField->orderPriceType.c_str()[0];

    ///买卖方向:
    req.Direction=userOrderField->_direction;

    ///组合开平标志
    req.CombOffsetFlag[0]=userOrderField->_offset_flag.c_str()[0];

    ///组合投机套保标志
    req.CombHedgeFlag[0]=userOrderField->_hedge_flag.c_str()[0];

    ///价格
    req.LimitPrice = userOrderField->_price;

    ///数量
    req.VolumeTotalOriginal = userOrderField->_volume;

    ///有效期类型: 当日有效
    req.TimeCondition = THOST_FTDC_TC_GFD;

    ///GTD日期
    //	TThostFtdcDateType	GTDDate;

    ///成交量类型: 任何数量
    req.VolumeCondition = THOST_FTDC_VC_AV;

    ///最小成交量: 1
    req.MinVolume = 1;

    ///触发条件: 立即
    req.ContingentCondition = THOST_FTDC_CC_Immediately;

    ///止损价
    req.StopPrice=0;

    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;

    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;

    ///业务单元
    //     TThostFtdcBusinessUnitType	BusinessUnit;

    ///请求编号
    req.RequestID = userOrderField->_requestID;

    ///用户强评标志: 否
    req.UserForceClose = 0;

    ///互换单标志
    //	TThostFtdcBoolType	IsSwapOrder;

    int iResult =_pUserApi->ReqOrderInsert(&req,iRequestID++);
    string orderinsertstr = strInputOrderField(&req);
    string msg=string("<<<----- ReqOrderInsert: ") + ((iResult == 0) ? "success" : "failed")+";"+orderinsertstr;
    LOG(INFO) <<(lexical_cast<string>(this)+msg);
    return iResult;

}

int CTraderSpi::ReqQryOrder()
{
    CThostFtdcQryOrderField req;
    memset(&req,0,sizeof(req));
    strcpy(req.InvestorID,_investorID.c_str());
    int iResult=_pUserApi->ReqQryOrder(&req,iRequestID++);
    LOG(INFO) << lexical_cast<string>(this) << "<<<<--- ReqQryOrder: " << ((iResult == 0) ? "success" : "failed") << endl;
    return iResult;
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    //记录错误信息
    //    LOG(INFO) << lexical_cast<string>(this) << "------>>>OnRspOrderInsert InvestorID="<<pInputOrder->InvestorID<< endl;
    bool err = IsErrorRspInfo(pRspInfo);
    string sInputOrderInfo=strInputOrderField(pInputOrder);
    string rsp=strRspInfoField(pRspInfo);

    LOG(INFO)<<(lexical_cast<string>(this)+"-------->>>OnRspOrderInsert:"+rsp+"###"+sInputOrderInfo);

    ChkThread&ct=  ChkThread::GetInstance();
    string key=GetKey2(pInputOrder);
    UserOrderField*userOrderField =  ct.get_Fuser_order(key);
    if(userOrderField!=NULL)
        userOrderField->SetStatus('7');
    // save to db
    dii.saveThostFtdcInputOrderFieldToDb(pInputOrder,rsp);
    return ;

}
///报单录入错误回报
void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    //    cout<<"----->>>>OnErrRtnOrderInsert"<<endl;

    bool err = IsErrorRspInfo(pRspInfo);
    string sInputOrderInfo = strInputOrderField(pInputOrder);
    string prsp=strRspInfoField(pRspInfo);
    LOG(INFO)<<(lexical_cast<string>(this)+"-------->>>OnErrRtnOrderInsert:"+sInputOrderInfo+prsp);

}

void CTraderSpi:: OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    //    cout<<"------>>>OnErrRtnOrderAction"<<endl;
    string sOrderActionField = strOrderActionField(pOrderAction);
    string prsp=strRspInfoField(pRspInfo);
    LOG(INFO)<<(lexical_cast<string>(this)+"-------->>>OnErrRtnOrderAction:"+sOrderActionField+prsp);
}

//void CTraderSpi::ReqOrderActionTwo(CThostFtdcInputOrderActionField *req,CThostFtdcTraderApi* pUserApi){
//    string investorID=lexical_cast<string>(req->InvestorID);
//    if(pUserApi){
//        //委托类操作，使用客户端定义的请求编号格式
//        int iResult = pUserApi->ReqOrderAction(req,iRequestID++);
//        cerr << "<<<<--- ReqOrderAction: " << ((iResult == 0) ? "success" : "failed") << endl;
//        string tmp=((iResult == 0) ? "success" : "failed");
//        string msg="<<<--- ReqOrderAction: " + tmp+";";
//        //记录报单录入信息
//        string orderinsertstr = strInputOrderActionField(req);
//        msg+=orderinsertstr;
//        LOG(INFO) <<msg;
//    }else{
//        string msg="ReqOrderAction:investorID="+investorID+",can't find tradeApi in tradeApiMap.";
//        cerr<<msg<<endl;
//        LOG(ERROR) <<msg;
//    }
//}



int CTraderSpi::ReqOrderAction(UserOrderField* orderInfo)
{
    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(CThostFtdcInputOrderActionField));

    ///经纪公司代码
    strcpy(req.BrokerID, orderInfo->brokerID.c_str());

    ///投资者代码
    strcpy(req.InvestorID, orderInfo->_investorID.c_str());

    ///报单操作引用
    //	TThostFtdcOrderActionRefType	OrderActionRef;

    ///报单引用
    strcpy(req.OrderRef, (lexical_cast<string>(orderInfo->_order_ref)).c_str());

    ///请求编号
    req.RequestID = iRequestID++;

    ///前置编号
    req.FrontID = orderInfo->_frontID;

    ///会话编号
    req.SessionID = orderInfo->_sessionID;

    ///交易所代码
    //strcpy(req.ExchangeID, pOrder->ExchangeID);

    ///报单编号
    strcpy(req.OrderSysID, orderInfo->orderSysID.c_str());

    ///操作标志
    req.ActionFlag = THOST_FTDC_AF_Delete;

    ///合约代码
    strcpy(req.InstrumentID, orderInfo->_instrumentID.c_str());


    int iResult = _pUserApi->ReqOrderAction(&req,iRequestID++);
    string tmp=((iResult == 0) ? "success" : "failed");
    string msg="<<<--- ReqOrderAction: " + tmp+";";
    string orderinsertstr = strInputOrderActionField(&req);
    LOG(INFO) <<(lexical_cast<string>(this)+msg+orderinsertstr);
    return iResult;

}
/*
void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
    //暂时不适用
// 	static bool ORDER_ACTION_SENT = false;		//是否发送了报单
// 	if (ORDER_ACTION_SENT)
// 		return;

    CThostFtdcInputOrderActionField req;
    memset(&req, 0, sizeof(req));
    if(strcmp(pOrder->BrokerID,"") != 0){
        ///经纪公司代码
        strcpy(req.BrokerID, pOrder->BrokerID);
    }
    if(strcmp(pOrder->InvestorID,"") != 0){
        ///投资者代码
        strcpy(req.InvestorID, pOrder->InvestorID);
    }
    if(strcmp(pOrder->OrderRef,"") != 0){
        ///报单引用
        strcpy(req.OrderRef, pOrder->OrderRef);
    }
    if(pOrder->RequestID != 0){
        ///请求编号
        req.RequestID = pOrder->RequestID;
    }
    if(pOrder->FrontID != 0){
        ///前置编号
        req.FrontID = pOrder->FrontID;
    }
    if(pOrder->SessionID != 0){
        ///会话编号
        req.SessionID = pOrder->SessionID;
    }
    if(strcmp(pOrder->ExchangeID,"") != 0){
        ///交易所代码
        strcpy(req.ExchangeID,pOrder->ExchangeID);
    }
    if(strcmp(pOrder->OrderSysID,"") != 0){
        ///交易所代码
        strcpy(req.OrderSysID,pOrder->OrderSysID);
    }
    if(strcmp(pOrder->InstrumentID,"") != 0){
        ///交易所代码
        strcpy(req.InstrumentID,pOrder->InstrumentID);
    }
    ///操作标志
    req.ActionFlag = THOST_FTDC_AF_Delete;
    CTPInterface* interface=dii->getTradeApi(boost::lexical_cast<string>(pOrder->InvestorID));
    if(interface){
        int iResult = interface->pUserApi->ReqOrderAction(&req, pOrder->RequestID);
        //int iResult = interface->pUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
        //int iResult = dii->pUserApi->ReqQryTradingAccount(&req, ++iRequestID);
        cerr << "--->>> ReqOrderAction: " << ((iResult == 0) ? "success" : "failed") << endl;
        string tmp=((iResult == 0) ? "success" : "failed");
        string msg="--->>> ReqOrderAction: " + tmp+";";
        //记录报单录入信息
        string orderinsertstr = getOrderActionInfoByDelimater(&req);
        msg+=orderinsertstr;
        LOG(INFO) <<msg;
    }else{
        string msg="ReqOrderAction:investorID="+boost::lexical_cast<string>(pOrder->InvestorID)+",can't find tradeApi in tradeApiMap.";
        cerr<<msg<<endl;
        LOG(ERROR) <<msg;
    }


}
*/
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    bool err = IsErrorRspInfo(pRspInfo);
    string prsp=strRspInfoField(pRspInfo);
    string sInputOrderInfo = strInputOrderActionField(pInputOrderAction);
    LOG(INFO)<<(lexical_cast<string>(this)+"----->>> OnRspOrderAction"+sInputOrderInfo+prsp);


}

bool CTraderSpi:: GetLoginOK()
{
    return _loginOK;
}
void CTraderSpi::SetLoginOK()
{
    _loginOK=true;
}


void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast)
{
    LOG(INFO) << lexical_cast<string>(this)<<"--->>>OnRspQryOrder  InvestorID="<<pOrder->InvestorID<<"status=" <<pOrder->OrderStatus<< endl;
    string tmpstr = strOrderField(pOrder);
    if(pOrder->OrderStatus==0)
    {

    }
    else
    {
        //cancal and reinsert
    }
}

///报单通知
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    //    typedef  UserAccountInfo*tmacc;

    string tmpstr = strOrderField(pOrder);

    LOG(INFO) << (lexical_cast<string>(this)+"--->>>OnRtnOrder:" + tmpstr);

    //    if(!IsMyOrder(pOrder))
    //        return;

    if(pOrder->OrderStatus == '0')
    {
        //modify
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if(userOrderField!=NULL)
            userOrderField->SetStatus('5');
        return ;
    }

    if(pOrder->OrderStatus == '1')
    {
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if(userOrderField!=NULL)
            userOrderField->SetStatus('1');
        return ;
    }

    if(pOrder->OrderStatus == '2')
    {
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if(userOrderField!=NULL)
            userOrderField->SetStatus('2');
        return ;
    }

    if(pOrder->OrderStatus == '3')
    {
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        //        UserOrderField*userOrderField =  ct->get_Fuser_order(key);
        //        if(userOrderField!=NULL)
        //            userOrderField->status='3';
    }

    if(pOrder->OrderStatus == '4')
    {

    }

    if(pOrder->OrderStatus == '5')
    {


    }

    if(pOrder->OrderStatus == 'a')
    {

    }

    if(pOrder->OrderStatus == 'b')
    {

    }

    if(pOrder->OrderStatus == 'c')
    {

    }
    return ;

    /*
    ///全部成交
    #define THOST_FTDC_OST_AllTraded '0'
    ///部分成交还在队列中
    #define THOST_FTDC_OST_PartTradedQueueing '1'
    ///部分成交不在队列中
    #define THOST_FTDC_OST_PartTradedNotQueueing '2'
    ///未成交还在队列中
    #define THOST_FTDC_OST_NoTradeQueueing '3'
    ///未成交不在队列中
    #define THOST_FTDC_OST_NoTradeNotQueueing '4'
    ///撤单
    #define THOST_FTDC_OST_Canceled '5'
    ///未知
    #define THOST_FTDC_OST_Unknown 'a'
    ///尚未触发
    #define THOST_FTDC_OST_NotTouched 'b' */

}

///成交通知
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    //       cerr << "------->>>>OnRtnTrade InvestorID=" << pTrade->InvestorID<< endl;
    // boost::recursive_mutex::scoped_lock SLock(_spi_mtx);
    string tmpstr = strTradeField(pTrade);
    LOG(INFO) << (lexical_cast<string>(this)+"--->>>OnRtnTrade:" + tmpstr);
    dii.saveThostFtdcTradeFieldToDb(pTrade);

    //todo modify account
    ReqQryTradingAccount();

    ReqQryInvestorPosition();

    //    dii.deleteOriOrder(boost::lexical_cast<string>(pTrade->OrderSysID));

}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
    LOG(INFO) << lexical_cast<string>(this) << "--->>>OnFrontDisconnected   Reason = " << nReason << endl;
    string tmpstr = "error:OnFrontDisconnected:reason=";
    tmpstr.append(boost::lexical_cast<string>(nReason));
    //tradequeue.push_back(tmpstr);
}

void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
    LOG(INFO) << lexical_cast<string>(this) << "--->>>OnHeartBeatWarning   nTimerLapse = " << nTimeLapse << endl;
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    LOG(INFO) << lexical_cast<string>(this)  << "------>>>OnRspError" << endl;
    IsErrorRspInfo(pRspInfo);
}
//委托有错误时，才会有该报文；否则 pRspInfo本身就是空指针。
bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
    //	if (bResult){
    //        string tmpstr =
    //       "ErrorID="+ lexical_cast<string>(pRspInfo->ErrorID)+
    //        ", ErrorMsg="+string(pRspInfo->ErrorMsg);

    //       LOG(INFO)<<between(tmpstr,"UTF-8","GBK");
    //	}
    return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->FrontID == frontID) &&
            (pOrder->SessionID == sessionID) &&
            (lexical_cast<int>(pOrder->OrderRef)==orderRef) );
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}



//提取成交回报信息
//using boost::lexical_cast;





//将投资者持仓信息写入文件保存
//string storeInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition)
//{
//	///合约代码
//	char	*InstrumentID = pInvestorPosition->InstrumentID;
//	///经纪公司代码
//	char	*BrokerID = pInvestorPosition->BrokerID;
//	///投资者代码
//	char	*InvestorID = pInvestorPosition->InvestorID;
//	///持仓多空方向
//	TThostFtdcPosiDirectionType	dir = pInvestorPosition->PosiDirection;
//	char PosiDirection[] = {dir,'\0'};
//	///投机套保标志
//	TThostFtdcHedgeFlagType	flag = pInvestorPosition->HedgeFlag;
//	char HedgeFlag[] = {flag,'\0'};
//	///持仓日期
//	TThostFtdcPositionDateType	positionDate = pInvestorPosition->PositionDate;
//	char PositionDate[] = {positionDate,'\0'};
//	///上日持仓
//	TThostFtdcVolumeType	ydPosition = pInvestorPosition->YdPosition;
//	char YdPosition[100];
//	sprintf(YdPosition,"%d",ydPosition);
//	///今日持仓
//	TThostFtdcVolumeType	position = pInvestorPosition->Position;
//	/*
//	if (position == 0)
//	{
//		return 0;
//	}*/
//	char Position[100];
//	sprintf(Position,"%d",position);
//	///多头冻结
//	TThostFtdcVolumeType	LongFrozen = pInvestorPosition->LongFrozen;
//	///空头冻结
//	TThostFtdcVolumeType	ShortFrozen = pInvestorPosition->ShortFrozen;
//	///开仓冻结金额
//	TThostFtdcMoneyType	LongFrozenAmount = pInvestorPosition->LongFrozenAmount;
//	///开仓冻结金额
//	TThostFtdcMoneyType	ShortFrozenAmount = pInvestorPosition->ShortFrozenAmount;
//	///开仓量
//	TThostFtdcVolumeType	openVolume = pInvestorPosition->OpenVolume;
//	char OpenVolume[100] ;
//	sprintf(OpenVolume,"%d",openVolume);
//	///平仓量
//	TThostFtdcVolumeType	closeVolume = pInvestorPosition->CloseVolume;
//	char CloseVolume[100];
//	sprintf(CloseVolume,"%d",closeVolume);
//	///开仓金额
//	TThostFtdcMoneyType	OpenAmount = pInvestorPosition->OpenAmount;
//	///平仓金额
//	TThostFtdcMoneyType	CloseAmount = pInvestorPosition->CloseAmount;
//	///持仓成本
//	TThostFtdcMoneyType	positionCost = pInvestorPosition->PositionCost;
//	char PositionCost[100];
//	sprintf(PositionCost,"%f",positionCost);
//	///上次占用的保证金
//	TThostFtdcMoneyType	PreMargin = pInvestorPosition->PreMargin;
//	///占用的保证金
//	TThostFtdcMoneyType	UseMargin = pInvestorPosition->UseMargin;
//	///冻结的保证金
//	TThostFtdcMoneyType	FrozenMargin = pInvestorPosition->FrozenMargin;
//	///冻结的资金
//	TThostFtdcMoneyType	FrozenCash = pInvestorPosition->FrozenCash;
//	///冻结的手续费
//	TThostFtdcMoneyType	FrozenCommission = pInvestorPosition->FrozenCommission;
//	///资金差额
//	TThostFtdcMoneyType	CashIn = pInvestorPosition->CashIn;
//	///手续费
//	TThostFtdcMoneyType	Commission = pInvestorPosition->Commission;
//	///平仓盈亏
//	TThostFtdcMoneyType	CloseProfit = pInvestorPosition->CloseProfit;
//	///持仓盈亏
//	TThostFtdcMoneyType	PositionProfit = pInvestorPosition->PositionProfit;
//	///上次结算价
//	TThostFtdcPriceType	preSettlementPrice = pInvestorPosition->PreSettlementPrice;
//	char PreSettlementPrice[100];
//	sprintf(PreSettlementPrice,"%f",preSettlementPrice);
//	///本次结算价
//	TThostFtdcPriceType	SettlementPrice = pInvestorPosition->PreSettlementPrice;
//	///交易日
//	char	*TradingDay = pInvestorPosition->TradingDay;
//	///结算编号
//	TThostFtdcSettlementIDType	SettlementID;
//	///开仓成本
//	TThostFtdcMoneyType	openCost = pInvestorPosition->OpenCost;
//	char OpenCost[100] ;
//	sprintf(OpenCost,"%f",openCost);
//	///交易所保证金
//	TThostFtdcMoneyType	exchangeMargin = pInvestorPosition->ExchangeMargin;
//	char ExchangeMargin[100];
//	sprintf(ExchangeMargin,"%f",exchangeMargin);
//	///组合成交形成的持仓
//	TThostFtdcVolumeType	CombPosition;
//	///组合多头冻结
//	TThostFtdcVolumeType	CombLongFrozen;
//	///组合空头冻结
//	TThostFtdcVolumeType	CombShortFrozen;
//	///逐日盯市平仓盈亏
//	TThostFtdcMoneyType	CloseProfitByDate = pInvestorPosition->CloseProfitByDate;
//	///逐笔对冲平仓盈亏
//	TThostFtdcMoneyType	CloseProfitByTrade = pInvestorPosition->CloseProfitByTrade;
//	///今日持仓
//	TThostFtdcVolumeType	todayPosition = pInvestorPosition->TodayPosition;
//	char TodayPosition[100] ;
//	sprintf(TodayPosition,"%d",todayPosition);
//	///保证金率
//	TThostFtdcRatioType	marginRateByMoney = pInvestorPosition->MarginRateByMoney;
//	char MarginRateByMoney[100];
//	sprintf(MarginRateByMoney,"%f",marginRateByMoney);
//	///保证金率(按手数)
//	TThostFtdcRatioType	marginRateByVolume = pInvestorPosition->MarginRateByVolume;
//	char MarginRateByVolume[100];
//	sprintf(MarginRateByVolume,"%f",marginRateByVolume);
//	string sInvestorInfo;
//	//文件写入字段定义
//	if(!isPositionDefFieldReady)
//	{
//		isPositionDefFieldReady = true;
//		sInvestorInfo.append("InstrumentID\t");
//		sInvestorInfo.append("BrokerID\t");
//		sInvestorInfo.append("InvestorID\t");
//		sInvestorInfo.append("PosiDirection\t");
//		sInvestorInfo.append("HedgeFlag\t");
//		sInvestorInfo.append("PositionDate\t");
//		sInvestorInfo.append("YdPosition\t");
//		sInvestorInfo.append("Position\t");
//		sInvestorInfo.append("OpenVolume\t");
//		sInvestorInfo.append("CloseVolume\t");
//		sInvestorInfo.append("PositionCost\t");
//		sInvestorInfo.append("PreSettlementPrice\t");
//		sInvestorInfo.append("TradingDay\t");
//		sInvestorInfo.append("OpenCost\t");
//		sInvestorInfo.append("ExchangeMargin\t");
//		sInvestorInfo.append("TodayPosition\t");

//		sInvestorInfo.append("MarginRateByMoney\t");
//		sInvestorInfo.append("MarginRateByVolume\t");
//        //loglist.push_back(sInvestorInfo);
//	}
//	sInvestorInfo.clear();

//	sInvestorInfo.append("InstrumentID=");sInvestorInfo.append(InstrumentID);sInvestorInfo.append("\t");
//	sInvestorInfo.append(BrokerID);sInvestorInfo.append("\t");
//	sInvestorInfo.append(InvestorID);sInvestorInfo.append("\t");
//	sInvestorInfo.append(PosiDirection);sInvestorInfo.append("\t");
//	sInvestorInfo.append(HedgeFlag);sInvestorInfo.append("\t");
//	sInvestorInfo.append(PositionDate);sInvestorInfo.append("\t");
//	sInvestorInfo.append(YdPosition);sInvestorInfo.append("\t");
//	sInvestorInfo.append(Position);sInvestorInfo.append("\t");
//	sInvestorInfo.append(OpenVolume);sInvestorInfo.append("\t");
//	sInvestorInfo.append(CloseVolume);sInvestorInfo.append("\t");
//	sInvestorInfo.append(PositionCost);sInvestorInfo.append("\t");
//	sInvestorInfo.append(PreSettlementPrice);sInvestorInfo.append("\t");

//	sInvestorInfo.append(TradingDay);sInvestorInfo.append("\t");
//	sInvestorInfo.append(OpenCost);sInvestorInfo.append("\t");
//	sInvestorInfo.append(ExchangeMargin);sInvestorInfo.append("\t");
//	sInvestorInfo.append(TodayPosition);sInvestorInfo.append("\t");
//	sInvestorInfo.append(MarginRateByMoney);sInvestorInfo.append("\t");
//	sInvestorInfo.append(MarginRateByVolume);sInvestorInfo.append("\t");
//    //loglist.push_back(sInvestorInfo);
//	///////////////////////////////////////////////////////////////////////////////////////////////
//	sInvestorInfo.clear();
//	sInvestorInfo.append("InstrumentID=");sInvestorInfo.append(InstrumentID);sInvestorInfo.append(sep);
//	sInvestorInfo.append("BrokerID=");sInvestorInfo.append(BrokerID);sInvestorInfo.append(sep);
//	sInvestorInfo.append("InvestorID=");sInvestorInfo.append(InvestorID);sInvestorInfo.append(sep);
//	sInvestorInfo.append("PosiDirection=");sInvestorInfo.append(PosiDirection);sInvestorInfo.append(sep);
//	sInvestorInfo.append("HedgeFlag=");sInvestorInfo.append(HedgeFlag);sInvestorInfo.append(sep);
//	sInvestorInfo.append("PositionDate=");sInvestorInfo.append(PositionDate);sInvestorInfo.append(sep);
//	sInvestorInfo.append("YdPosition=");sInvestorInfo.append(YdPosition);sInvestorInfo.append(sep);
//	sInvestorInfo.append("Position=");sInvestorInfo.append(Position);sInvestorInfo.append(sep);
//	sInvestorInfo.append("OpenVolume=");sInvestorInfo.append(OpenVolume);sInvestorInfo.append(sep);
//	sInvestorInfo.append("CloseVolume=");sInvestorInfo.append(CloseVolume);sInvestorInfo.append(sep);
//	sInvestorInfo.append("PositionCost=");sInvestorInfo.append(PositionCost);sInvestorInfo.append(sep);
//	sInvestorInfo.append("PreSettlementPrice=");sInvestorInfo.append(PreSettlementPrice);sInvestorInfo.append(sep);

//	sInvestorInfo.append("TradingDay=");sInvestorInfo.append(TradingDay);sInvestorInfo.append(sep);
//	sInvestorInfo.append("OpenCost=");sInvestorInfo.append(OpenCost);sInvestorInfo.append(sep);
//	sInvestorInfo.append("ExchangeMargin=");sInvestorInfo.append(ExchangeMargin);sInvestorInfo.append(sep);
//	sInvestorInfo.append("TodayPosition=");sInvestorInfo.append(TodayPosition);sInvestorInfo.append(sep);
//	sInvestorInfo.append("MarginRateByMoney=");sInvestorInfo.append(MarginRateByMoney);sInvestorInfo.append(sep);
//	sInvestorInfo.append("MarginRateByVolume=");sInvestorInfo.append(MarginRateByVolume);sInvestorInfo.append(sep);
//    //loglist.push_back(sInvestorInfo);
//	return sInvestorInfo;
//}

//提取投资者报单信息

//将投资者成交信息写入文件保存
void CTraderSpi::storeInvestorTrade(CThostFtdcTradeField *pTrade)
{
    string tradeInfo;
    ///经纪公司代码
    char	*BrokerID = pTrade->BrokerID;
    ///投资者代码
    char	*InvestorID = pTrade->InvestorID;
    ///合约代码
    char	*InstrumentID =pTrade->InstrumentID;
    ///报单引用
    char	*OrderRef = pTrade->OrderRef;
    ///用户代码
    char	*UserID = pTrade->UserID;
    ///交易所代码
    char	*ExchangeID =pTrade->ExchangeID;
    ///成交编号
    //TThostFtdcTradeIDType	TradeID;
    ///买卖方向
    TThostFtdcDirectionType	direction = pTrade->Direction;
    char Direction[]={direction,'\0'};
    //sprintf(Direction,"%s",direction);
    ///报单编号
    char	*OrderSysID = pTrade->OrderSysID;
    ///会员代码
    //TThostFtdcParticipantIDType	ParticipantID;
    ///客户代码
    char	*ClientID = pTrade->ClientID;
    ///交易角色
    //TThostFtdcTradingRoleType	TradingRole;
    ///合约在交易所的代码
    //TThostFtdcExchangeInstIDType	ExchangeInstID;
    ///开平标志
    TThostFtdcOffsetFlagType	offsetFlag = pTrade->OffsetFlag;
    char OffsetFlag[]={offsetFlag,'\0'};
    //sprintf(OffsetFlag,"%s",offsetFlag);
    ///投机套保标志
    TThostFtdcHedgeFlagType	hedgeFlag = pTrade->HedgeFlag;
    char HedgeFlag[]={hedgeFlag,'\0'};
    //sprintf(HedgeFlag,"%s",hedgeFlag);
    ///价格
    TThostFtdcPriceType	price = pTrade->Price;
    char Price[100];
    sprintf(Price,"%f",price);
    ///数量
    TThostFtdcVolumeType	volume = pTrade->Volume;
    char Volume[100];
    sprintf(Volume,"%d",volume);
    ///成交时期
    //TThostFtdcDateType	TradeDate;
    ///成交时间
    char	*TradeTime = pTrade->TradeTime;
    ///成交类型
    TThostFtdcTradeTypeType	tradeType = pTrade->TradeType;
    char TradeType[]={tradeType,'\0'};
    //sprintf(TradeType,"%s",tradeType);
    ///成交价来源
    //TThostFtdcPriceSourceType	PriceSource;
    ///交易所交易员代码
    //TThostFtdcTraderIDType	TraderID;
    ///本地报单编号
    char	*OrderLocalID = pTrade->OrderLocalID;
    ///结算会员编号
    //TThostFtdcParticipantIDType	ClearingPartID;
    ///业务单元
    //TThostFtdcBusinessUnitType	BusinessUnit;
    ///序号
    //TThostFtdcSequenceNoType	SequenceNo;
    ///交易日
    char	*TradingDay = pTrade->TradingDay;
    ///结算编号
    //TThostFtdcSettlementIDType	SettlementID;
    ///经纪公司报单编号
    //TThostFtdcSequenceNoType	BrokerOrderSeq;
    if (!isTradeDefFieldReady)
    {
        isTradeDefFieldReady = true;
        tradeInfo.append("BrokerID\t");
        tradeInfo.append("InvestorID\t");
        tradeInfo.append("InstrumentID\t");
        tradeInfo.append("OrderRef\t");
        tradeInfo.append("UserID\t");
        tradeInfo.append("ExchangeID\t");
        tradeInfo.append("Direction\t");
        tradeInfo.append("ClientID\t");
        tradeInfo.append("OffsetFlag\t");
        tradeInfo.append("HedgeFlag\t");
        tradeInfo.append("Price\t");
        tradeInfo.append("Volume\t");
        tradeInfo.append("TradeTime\t");
        tradeInfo.append("TradeType\t");
        tradeInfo.append("OrderLocalID\t");
        tradeInfo.append("TradingDay\t");
        tradeInfo.append("ordersysid\t");
        //loglist.push_back(tradeInfo);
    }
    tradeInfo.clear();
    tradeInfo.append(BrokerID);tradeInfo.append("\t");
    tradeInfo.append(InvestorID);tradeInfo.append("\t");
    tradeInfo.append(InstrumentID);tradeInfo.append("\t");
    tradeInfo.append(OrderRef);tradeInfo.append("\t");
    tradeInfo.append(UserID);tradeInfo.append("\t");
    tradeInfo.append(ExchangeID);tradeInfo.append("\t");
    tradeInfo.append(Direction);tradeInfo.append("\t");
    tradeInfo.append(ClientID);tradeInfo.append("\t");
    tradeInfo.append(OffsetFlag);tradeInfo.append("\t");
    tradeInfo.append(HedgeFlag);tradeInfo.append("\t");
    tradeInfo.append(Price);tradeInfo.append("\t");
    tradeInfo.append(Volume);tradeInfo.append("\t");
    tradeInfo.append(TradeTime);tradeInfo.append("\t");
    tradeInfo.append(TradeType);tradeInfo.append("\t");
    tradeInfo.append(OrderLocalID);tradeInfo.append("\t");
    tradeInfo.append(TradingDay);tradeInfo.append("\t");
    tradeInfo.append(OrderSysID);tradeInfo.append("\t");
    //loglist.push_back(tradeInfo);
}
//将成交信息组装成对冲报单
// CThostFtdcInputOrderField assamble(CThostFtdcTradeField *pTrade)
// {
// 	CThostFtdcInputOrderField order;
// 	memset(&order,0,sizeof(order));
// 	//经济公司代码
// 	strcpy(order.BrokerID,pTrade->BrokerID);
// 	///投资者代码
// 	strcpy(order.InvestorID,pTrade->InvestorID);
// 	///合约代码
// 	strcpy(order.InstrumentID,pTrade->InstrumentID);
// 	///报单引用
// 	strcpy(order.OrderRef ,pTrade->OrderRef);
// 	///报单价格条件: 限价
// 	order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
// 	///买卖方向: 这个要和对手方的一致，即如果我的成交为买，那么这里变成卖
// 	TThostFtdcDirectionType	Direction = pTrade->Direction;
// 	if (Direction == '0'){
// 		order.Direction = THOST_FTDC_D_Sell;
// 	} else {
// 		order.Direction = THOST_FTDC_D_Buy;
// 	}
// 	///组合开平标志: 和对手方一致
// 	order.CombOffsetFlag[0] = pTrade->OffsetFlag;
// 	///组合投机套保标志
// 	order.CombHedgeFlag[0] = pTrade->HedgeFlag;
// 	///价格
// 	TThostFtdcPriceType price = pTrade->Price;
// 	if (order.Direction == THOST_FTDC_D_Sell){
// 		//在原对手方报价基础上加上自定义tick
// 		order.LimitPrice = price + tickSpreadSell * tick;
// 	} else {
// 		//在原对手方报价基础上减去自定义tick
// 		order.LimitPrice = price - tickSpreadSell * tick;
// 	}
// 	///数量: 1
// 	order.VolumeTotalOriginal = pTrade->Volume;
// 	///有效期类型: 当日有效
// 	order.TimeCondition = THOST_FTDC_TC_GFD;
// 	///成交量类型: 任何数量
// 	order.VolumeCondition = THOST_FTDC_VC_AV;
// 	///最小成交量: 1
// 	order.MinVolume = 1;
// 	///触发条件: 立即
// 	order.ContingentCondition = THOST_FTDC_CC_Immediately;
// 	///强平原因: 非强平
// 	order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
// 	///自动挂起标志: 否
// 	order.IsAutoSuspend = 0;
// 	///用户强评标志: 否
// 	order.UserForceClose = 0;
// 	return order;
// }
///撤单报单组装
CThostFtdcOrderField CTraderSpi::AssambleOrderAction(list<string> orderAction)
{
    //    //loglist.push_back("开始组装撤单、修改报单请求信息......");
    //	///经纪公司代码
    //	TThostFtdcBrokerIDType	Broker_ID;
    //	///投资者代码
    //	TThostFtdcInvestorIDType Investor_ID;
    //	///合约代码
    //	char InstrumentID[31];
    //	///请求编号
    //	int RequestID = 0;
    //	//报单引用编号
    //	char OrderRef[13];
    //	///前置编号
    //	TThostFtdcFrontIDType FrontID = 1;
    //	///会话编号
    //	TThostFtdcSessionIDType SessionID=0;
    //	///操作标志
    //	char ActionFlag[3] = "\0";
    //	///交易所代码
    //	TThostFtdcExchangeIDType	ExchangeID="";
    //	///报单编号
    //	TThostFtdcOrderSysIDType OrderSysID;

    //	//报单结构体
    //	CThostFtdcOrderField req ;
    //	//CThostFtdcInputOrderField req;
    //	memset(&req, 0, sizeof(req));
    //	//cout << "~~~~~~~~~~~~~~============>报单录入"<<endl;

    //	//const char * split = "="; //分割符号
    //	int fieldSize = orderAction.size();
    //	/************************************************************************/
    //	/* 每个字段，按照=分隔符进行分割                                        */
    //	/************************************************************************/
    //	try{
    //		int i = 0;
    //		for(list<string>::iterator beg = orderAction.begin();beg != orderAction.end();beg ++){
    //			i++;
    //			string tmpstr = *beg;
    //			cout << tmpstr <<endl;
    //			//分割之后的字符
    //			char * p = 0;
    //			//string转char*
    //			char * rawfields =new char[tmpstr.size() + 1];
    //			strcpy(rawfields,tmpstr.c_str());
    //			p = strtok (rawfields,split); //分割字符串
    //			vector<string> strlist;
    //			while(p!=NULL)
    //			{
    //				//cout << p <<endl;
    //				strlist.push_back(p);
    //				p = strtok(NULL,split); //指向下一个指针
    //			}
    //			if(strlist.size() != 2){
    //				//有字段为空，不填
    //				string tmpstr2 = "字段值为空:";
    //                //loglist.push_back(tmpstr2 += tmpstr);
    //				string tmpstr3 = "there is field value is null!!!:";
    //				tmpstr3.append(tmpstr);
    //                //tradequeue.push_back(tmpstr3);
    //				continue;
    //			}
    //			/************************************************************************/
    //			/* 变量赋值                                                                     */
    //			/*Broker_ID			1
    //			/*Investor_ID		2
    //			/*InstrumentID		3
    //			/*RequestID			4
    //			/*OrderRef			5
    //			/*FrontID			6
    //			/*SessionID			7
    //			/*ExchangeID		8
    //			/*OrderSysID		9
    //			/************************************************************************/
    //			string ttt = strlist.at(1);
    //			//cout << "赋值为:" + ttt<<endl;
    //			if(i == 1){
    //				strcpy(Broker_ID,ttt.c_str());
    //			}else if(i == 2){
    //				strcpy(Investor_ID,ttt.c_str());
    //			}else if(i == 3){
    //				strcpy(InstrumentID,ttt.c_str());
    //			}else if(i == 4){
    //				RequestID = atoi(ttt.c_str());
    //			}else if(i == 5){
    //				strcpy(OrderRef,ttt.c_str());
    //			}else if(i == 6){
    //				FrontID = atoi(ttt.c_str());
    //			}else if(i == 7){
    //				SessionID = atol(ttt.c_str());
    //			}else if(i == 8){
    //				strcpy(ExchangeID,ttt.c_str());
    //			}else if(i == 9){
    //				strcpy(OrderSysID,ttt.c_str());
    //			}
    //		}
    //		///经纪公司代码

    //		strcpy(req.BrokerID, Broker_ID);
    //		///投资者代码
    //		strcpy(req.InvestorID, Investor_ID);
    //		///合约代码
    //		strcpy(req.InstrumentID, InstrumentID);
    //		///报单引用
    //		strcpy(req.OrderRef, OrderRef);
    //		req.RequestID = RequestID;
    //		///前置编号
    //		req.FrontID = FrontID;
    //		req.SessionID = SessionID;
    //		strcpy(req.ExchangeID , ExchangeID);
    //		strcpy(req.OrderSysID , OrderSysID);
    //	}catch(const runtime_error &re){
    //		cerr << re.what()<<endl;
    //	}catch (exception* e)
    //	{
    //		cerr << e->what()<<endl;
    //        //loglist.push_back(e->what());
    //	}
    //	return req;
}
///撤单报单组装2
CThostFtdcOrderField CTraderSpi::AssambleOrderActionTwo(list<string> orderAction){
    //    //loglist.push_back("开始组装撤单、修改报单请求信息......");
    //	///经纪公司代码
    //	TThostFtdcBrokerIDType	Broker_ID = {"\0"};
    //	///投资者代码
    //	TThostFtdcInvestorIDType Investor_ID = {"\0"};
    //	///合约代码
    //	char InstrumentID[31] = {"\0"};
    //	///请求编号
    //	int RequestID = 0;
    //	//报单引用编号
    //	char OrderRef[13]={"\0"};
    //	///前置编号
    //	TThostFtdcFrontIDType FrontID = 1;
    //	///会话编号
    //	TThostFtdcSessionIDType SessionID=0;
    //	///操作标志
    //	char ActionFlag[3];
    //	///交易所代码
    //	TThostFtdcExchangeIDType	ExchangeID= {"\0"};
    //	///报单编号
    //	TThostFtdcOrderSysIDType OrderSysID = {"\0"};

    //	//报单结构体
    //	CThostFtdcOrderField req ;
    //	//CThostFtdcInputOrderField req;
    //	memset(&req, 0, sizeof(req));
    //	//cout << "~~~~~~~~~~~~~~============>报单录入"<<endl;

    //	//const char * split = "="; //分割符号
    //	int fieldSize = orderAction.size();
    //	/************************************************************************/
    //	/* 每个字段，按照=分隔符进行分割                                        */
    //	/************************************************************************/
    //	try{
    //		for(list<string>::iterator beg = orderAction.begin();beg != orderAction.end();beg ++){
    //			string tmpstr = *beg;

    //            vector<string> vec = UniverseTools::split(tmpstr,"=");
    //			if("FrontID"==vec[0]){
    //				FrontID = boost::lexical_cast<int>(vec[1]);
    //				req.FrontID = FrontID;
    //			}
    //			if("SessionID"==vec[0]){
    //				SessionID = boost::lexical_cast<int>(vec[1]);
    //				req.SessionID = SessionID;
    //			}
    //			if("OrderRef"==vec[0]){
    //				strcpy(OrderRef,vec[1].c_str());
    //				strcpy(req.OrderRef, OrderRef);
    //			}
    //			if("InstrumentID"==vec[0]){
    //				strcpy(InstrumentID,vec[1].c_str());
    //				strcpy(req.InstrumentID, InstrumentID);
    //			}
    //			if("OrderSysID"==vec[0]){
    //				strcpy(OrderSysID,vec[1].c_str());
    //				strcpy(req.OrderSysID, OrderSysID);
    //			}
    //			if("ExchangeID"==vec[0]){
    //				strcpy(ExchangeID,vec[1].c_str());
    //				strcpy(req.ExchangeID, ExchangeID);
    //			}
    //		}
    //		///经纪公司代码

    //		//strcpy(req.BrokerID, Broker_ID);
    //		///投资者代码
    //		//strcpy(req.InvestorID, Investor_ID);
    //		///合约代码
    //		//strcpy(req.InstrumentID, InstrumentID);
    //		///报单引用
    //		//strcpy(req.OrderRef, OrderRef);
    //		//req.RequestID = RequestID;
    //		///前置编号
    //		//req.FrontID = FrontID;
    //		//req.SessionID = SessionID;
    //		//strcpy(req.ExchangeID , ExchangeID);
    //		//strcpy(req.OrderSysID , OrderSysID);
    //	}catch(const runtime_error &re){
    //		cerr << re.what()<<endl;
    //	}catch (exception* e)
    //	{
    //		cerr << e->what()<<endl;
    //        //loglist.push_back(e->what());
    //	}
    //return req;
}
int CTraderSpi::total_trade_num() const
{
    return _total_trade_num;
}

void CTraderSpi::setTotal_trade_num(int total_trade_num)
{
    _total_trade_num = total_trade_num;
}
int CTraderSpi::profit_num() const
{
    return _profit_num;
}

void CTraderSpi::setProfit_num(int profit_num)
{
    _profit_num = profit_num;
}
int CTraderSpi::close_num() const
{
    return _close_num;
}

void CTraderSpi::setClose_num(int close_num)
{
    _close_num = close_num;
}
double CTraderSpi::profit() const
{
    return _profit;
}

void CTraderSpi::setProfit(double profit)
{
    _profit = profit;
}
double CTraderSpi::loss() const
{
    return _loss;
}

void CTraderSpi::setLoss(double loss)
{
    _loss = loss;
}
int CTraderSpi::loss_num() const
{
    return _loss_num;
}

void CTraderSpi::setLoss_num(int loss_num)
{
    _loss_num = loss_num;
}
string CTraderSpi::investorID() const
{
    return _investorID;
}

void CTraderSpi::setInvestorID(const string &investorID)
{
    _investorID = investorID;
}







