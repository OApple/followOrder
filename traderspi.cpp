#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
//#include <SQLiteCpp/SQLiteCpp.h>
//#include <SQLiteCpp/VariadicBind.h>
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
#include "mdspi.h"
#include "updateacc.h"
#pragma warning(disable : 4996)

//126373/123456/1:1/1/1
CTraderSpi:: CTraderSpi(DataInitInstance&di, string&config):dii(di)
{
    vector<string> config_list ;
    boost::split(config_list,config,boost::is_any_of("/"));
    if(config_list.size()!=7)
        return ;
    _config=config;
    _investorID=config_list[0];
    _password=config_list[1];
    _ratio=config_list[4];
    _priceType=(config_list[5]);
    _followTick=(config_list[6]);

    _trade_front_addr= "tcp://"+config_list[2];
    _brokerID=config_list[3];
//    cout<<_trade_front_addr<<endl;
//    _trade_front_addr= dii._trade_front_addr;
//    _brokerID=dii.broker_id;
//     if(dii.SuseReal==1)
//     {
//         _trade_front_addr=dii.realTradeFrontAddr;
//          _brokerID=dii.realBrokerID;
//     }

    string prefix=_investorID+"/";
    system(("mkdir  -p "+prefix).c_str());
    CThostFtdcTraderApi* pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(prefix.c_str());			// 创建UserApi
    pUserApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// 注册事件类
    pUserApi->SubscribePublicTopic(THOST_TERT_QUICK);					// 注册公有流
    pUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);					// 注册私有流

//    pUserApi->RegisterFront("tcp://180.168.146.187:10001");
//    pUserApi->RegisterFront("tcp://218.202.237.33:10002");
    pUserApi->RegisterFront((char*)(_trade_front_addr.c_str()));

    _pUserApi=pUserApi;
}


CTraderSpi::CTraderSpi( DataInitInstance &di,
                        bool loginOK,
                        CThostFtdcTraderApi* pUserApi):dii(di),_loginOK(loginOK),_pUserApi(pUserApi)
{}

CTraderSpi::~CTraderSpi()
{
    if(_pUserApi)
    {
        _pUserApi->RegisterSpi(NULL);
        _pUserApi->Release();
    }
}

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
    strcpy(req.Password, _password.c_str());
    int iResult = _pUserApi->ReqUserLogin(&req, ++_requestID);
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
        int iResult = _pUserApi->ReqQrySettlementInfoConfirm(&req, ++_requestID);
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
        int iResult = _pUserApi->ReqQrySettlementInfo(&req, ++_requestID);
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
        _frontID = pRspUserLogin->FrontID;
        _sessionID= pRspUserLogin->SessionID;
        _orderRef= lexical_cast<int>(pRspUserLogin->MaxOrderRef)+1;
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

//        SaveTransactionRecord();
        ReqSettlementInfoConfirm();
        _settlemsg.clear();
    }
}

void CTraderSpi::ReqSettlementInfoConfirm(){
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, _brokerID.c_str());
    strcpy(req.InvestorID, _investorID.c_str());
    int iResult = _pUserApi->ReqSettlementInfoConfirm(&req, ++_requestID);
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
        int iResult = _pUserApi->ReqQryTradingAccount(&req, ++_requestID);
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
        int iResult = _pUserApi->ReqQryInvestorPosition(&req, ++_requestID);
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

        string info=strInvestorPositionField(pInvestorPosition);
        LOG(INFO) << "------->>>>OnRspQryInvestorPosition" <<info<<endl;
        dii.saveThostFtdcInvestorPositionFieldToDb(pInvestorPosition);
        CMdSpi*mdspi=CMdSpi::getInstance();
//        if(pInvestorPosition->Position)
//           mdspi->Subscribe(string(pInvestorPosition->InstrumentID));
        if(!_loginOK)
        {
            hp=initpst(pInvestorPosition);
//            return ;
        }

    }
    if(bIsLast)
    {
        _loginOK=true;
        _positon_req_send=false;
        unordered_map<string, HoldPositionInfo*>::iterator tmpit = positionmap.begin();
        if (tmpit == positionmap.end()) {
            LOG(INFO) << "当前无持仓信息" << endl;
        }
        else
        {
            for (; tmpit != positionmap.end(); tmpit++)
            {
                string str_instrument = tmpit->first;
                HoldPositionInfo* tmppst = tmpit->second;
                int tmpArbVolume = 0;
                 if((tmppst->longTotalPosition==0)&&(tmppst->shortTotalPosition==0))
                     continue;
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
                        "持仓结构:" +string(str_instrument) +
                        ",多头持仓量=" + string(char_tmp_pst) +
                        ",今仓数量=" + string(char_longtd_pst) +
                        ",昨仓数量=" + string(char_longyd_pst) +
                        ",可平量=" + boost::lexical_cast<string>(tmppst->longAvaClosePosition) +
                        ";空头持仓量=" + string(char_tmp_pst2) +
                        ",今仓数量=" + string(char_shorttd_pst) +
                        ",昨仓数量=" + string(char_shortyd_pst) +
                        ",可平量=" + boost::lexical_cast<string>(tmppst->shortAvaClosePosition) +
                        ";组合持仓量=" + boost::lexical_cast<string>(tmpArbVolume);
                //        cout << pst_msg << endl;
                LOG(INFO) <<_investorID<<" " <<pst_msg;
                //        delete hp;
            }
        }
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
            if(col.size()<15)
                continue;
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

void CTraderSpi::processtrade(CThostFtdcOrderField *pOrder){
    /*********find positionmap*********/

    string str_dir=lexical_cast<string>(pOrder->Direction);
    string str_inst = pOrder->InstrumentID;
    int volume = pOrder->VolumeTotalOriginal;
    string instrumentID=pOrder->InstrumentID;
    //开平方向
    string str_offset = pOrder->CombOffsetFlag;

    unordered_map<string, HoldPositionInfo*>::iterator map_iterator = positionmap.find(str_inst);
//    if (map_iterator != positionmap.end())

    if(str_offset != "0")//平方向
    {
        if (map_iterator == positionmap.end())
        {
            LOG(ERROR)<<"close error";
            return ;
        }
        else
        {
            HoldPositionInfo* tmpmap = map_iterator->second;
             if (str_dir == "0") {//买
                  tmpmap->shortTotalPosition -= volume;
             }
             else if (str_dir == "1") {//卖
                  tmpmap->longTotalPosition -= volume;
             }
             LOG(ERROR) <<" close investorid="<<_investorID <<"  instrumentID="<<instrumentID<< "  shortTotalPosition="
                       <<tmpmap->shortTotalPosition<<"  longTotalPosition="<<tmpmap->longTotalPosition;
        }
        return;
    }
  if(str_offset == "0")//开方向
  {
    if (map_iterator == positionmap.end()) //新开仓
    {
        HoldPositionInfo* tmpmap = new HoldPositionInfo();
        if (str_dir == "0") {//买
            tmpmap->longTotalPosition = volume;
            tmpmap->longAvaClosePosition = volume;

        } else if (str_dir == "1") {//卖
            tmpmap->shortTotalPosition = volume;
            tmpmap->shortAvaClosePosition = volume;

        }
        positionmap[str_inst] = tmpmap;
        LOG(ERROR) <<"new open investorid="<<_investorID <<"  instrumentID="<<instrumentID<< "  shortAvaClosePosition="
                  <<tmpmap->shortAvaClosePosition<<"  longAvaClosePosition="<<tmpmap->longAvaClosePosition;
    }
    else//开仓
    {
          HoldPositionInfo* tmpmap = map_iterator->second;
          if (str_dir == "0") {//买
                               //多头
              tmpmap->longTotalPosition += volume;
              tmpmap->longAvaClosePosition += volume;

          } else if (str_dir == "1") {//卖
                                      //空头
              tmpmap->shortTotalPosition += volume;
              tmpmap->shortAvaClosePosition += volume;

          }
           LOG(ERROR) <<" open investorid="<<_investorID <<"  instrumentID="<<instrumentID<< "  shortAvaClosePosition="
                     <<tmpmap->shortAvaClosePosition<<"  longAvaClosePosition="<<tmpmap->longAvaClosePosition;
    }
    }

}


void CTraderSpi::processHowManyHoldsCanBeClose(CThostFtdcOrderField *pOrder,string type)
{
    /*********find holdPstIsLocked and positionmap*********/

    string instrumentID = pOrder->InstrumentID;
    string offsetFlag = string(pOrder->CombOffsetFlag);
    if(offsetFlag=="0")//open
    {
        return ;
    }
    HoldPositionInfo* holdInfo=nullptr;
    if ("lock" == type)
    {//锁仓
        unordered_map<string, HoldPositionInfo*>::iterator it = positionmap.find(instrumentID);
        if (it == positionmap.end())
        {
            LOG(ERROR) << "合约" + instrumentID + " 无持仓信息，买平仓锁仓操作错误!!";
            return;
        }
        if (pOrder->Direction == '0' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4"))
        {//买平仓，锁定空头可平量
            holdInfo = it->second;
            holdInfo->shortAvaClosePosition = holdInfo->shortAvaClosePosition - pOrder->VolumeTotalOriginal;
        }
        else if (pOrder->Direction == '1' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4"))
        {//卖平仓，锁定多头可平量
            holdInfo = it->second;
            holdInfo->longAvaClosePosition = holdInfo->longAvaClosePosition - pOrder->VolumeTotalOriginal;
        }
        else
        {
            DLOG(INFO) << "open position,do not need lock.";
        }
        if(holdInfo)
        LOG(ERROR) <<"lock investorid="<<_investorID <<"  instrumentID="<<instrumentID<< "  shortAvaClosePosition="
                  <<holdInfo->shortAvaClosePosition<<"  longAvaClosePosition="<<holdInfo->longAvaClosePosition;
    }
    else if ("release" == type)
    {//释放持仓
        unordered_map<string, HoldPositionInfo*>::iterator it = positionmap.find(instrumentID);
        if (it == positionmap.end()) {
            LOG(ERROR) << "合约" + instrumentID + " 无持仓信息，买平仓释放操作错误!!";
            return;
        }
        if (pOrder->Direction == '0' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4"))
        {//买平仓，释放空头可平量
            holdInfo = it->second;
            holdInfo->shortAvaClosePosition = holdInfo->shortAvaClosePosition + pOrder->VolumeTotal;
        }
        else if (pOrder->Direction == '1' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4"))
        {//卖平仓，释放多头可平量
            holdInfo = it->second;
            holdInfo->longAvaClosePosition = holdInfo->longAvaClosePosition + pOrder->VolumeTotal;
        }
          if(holdInfo)
        LOG(ERROR) <<"release investorid="<<_investorID <<"  instrumentID="<<instrumentID<< "  shortAvaClosePosition="
                  <<holdInfo->shortAvaClosePosition<<"  longAvaClosePosition="<<holdInfo->longAvaClosePosition;
    }
    }





//初始化持仓信息
HoldPositionInfo* CTraderSpi:: initpst(CThostFtdcInvestorPositionField *pInvestorPosition)
{
//    boost::recursive_mutex::scoped_lock SLock(dii->pst_mtx);
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
    char YdPosition[100];
    sprintf(YdPosition, "%d", ydPosition);
    ///今日持仓
    TThostFtdcVolumeType	position = pInvestorPosition->Position;
    char Position[100];
    sprintf(Position, "%d", position);
    string str_dir = string(PosiDirection);
//    double multiplier = dii->getMultipler(str_instrumentid);
    ///持仓日期
    TThostFtdcPositionDateType	positionDate = pInvestorPosition->PositionDate;
    char PositionDate[] = { positionDate,'\0' };
    string str_pst_date = string(PositionDate);
    /*********find positionmap*********/

    if (positionmap.find(str_instrumentid) == positionmap.end()) {//暂时没有处理，不需要考虑多空方向
        HoldPositionInfo* tmpinfo = new HoldPositionInfo();
        if ("2" == str_dir) {//买  //多头
            tmpinfo->longTotalPosition = position;
            tmpinfo->longAvaClosePosition = position-pInvestorPosition->LongFrozen;
            tmpinfo->longAmount = pInvestorPosition->PositionCost;
//            tmpinfo->longHoldAvgPrice = pInvestorPosition->PositionCost / (multiplier*position);
            //tmpmap["longTotalPosition"] = position;
            //空头
            tmpinfo->shortTotalPosition = 0;
            //tmpmap["shortTotalPosition"] = 0;
            if ("2" == str_pst_date) {//昨仓
                tmpinfo->longYdPosition = position;
            } else if ("1" == str_pst_date) {//今仓
                tmpinfo->longTdPosition = position;
            }
        } else if ("3" == str_dir) {//空
                                    //空头
                                    //tmpmap["shortTotalPosition"] = position;
            tmpinfo->longTotalPosition = 0;
            tmpinfo->shortTotalPosition = position;
            tmpinfo->shortAvaClosePosition = position-pInvestorPosition->ShortFrozen;
            tmpinfo->shortAmount = pInvestorPosition->PositionCost;
//            tmpinfo->shortHoldAvgPrice = pInvestorPosition->PositionCost / (multiplier*position);
            //tmpmap["longTotalPosition"] = 0;
            if ("2" == str_pst_date) {//昨仓
                tmpinfo->shortYdPosition = position;
            } else if ("1" == str_pst_date) {//今仓
                tmpinfo->shortTdPosition = position;
            }
        } else {
            LOG(ERROR) << string(InstrumentID) + ";error:持仓类型无法判断PosiDirection=" + str_dir;

        }
        positionmap[str_instrumentid] = tmpinfo;
        return tmpinfo;
    } else {
        unordered_map<string, HoldPositionInfo*>::iterator tmpmap = positionmap.find(str_instrumentid);
        HoldPositionInfo* tmpinfo = tmpmap->second;
        //对应的反方向应该已经存在，这里后续需要确认
        if ("2" == str_dir) {//多
            tmpinfo->longTotalPosition = position + tmpinfo->longTotalPosition;
            tmpinfo->longAvaClosePosition = position -pInvestorPosition->LongFrozen+ tmpinfo->longAvaClosePosition;
            tmpinfo->longAmount = pInvestorPosition->PositionCost + tmpinfo->longAmount;
//            tmpinfo->longHoldAvgPrice = tmpinfo->longAmount / (multiplier*tmpinfo->longTotalPosition);
            if ("2" == str_pst_date) {//昨仓
                tmpinfo->longYdPosition = position + tmpinfo->longYdPosition;
            } else if ("1" == str_pst_date) {//今仓
                tmpinfo->longTdPosition = position + tmpinfo->longTdPosition;
            }
        } else if ("3" == str_dir) {//空
            tmpinfo->shortTotalPosition = position + tmpinfo->shortTotalPosition;
            tmpinfo->shortAvaClosePosition = position-pInvestorPosition->ShortFrozen + tmpinfo->shortAvaClosePosition;
            tmpinfo->shortAmount = pInvestorPosition->PositionCost + tmpinfo->shortAmount;
//            tmpinfo->shortHoldAvgPrice = tmpinfo->shortAmount / (multiplier*tmpinfo->shortTotalPosition);
            if ("2" == str_pst_date) {//昨仓
                tmpinfo->shortYdPosition = position + tmpinfo->shortYdPosition;
            } else if ("1" == str_pst_date) {//今仓
                tmpinfo->shortTdPosition = position + tmpinfo->shortTdPosition;
            }
        } else {
            //cout << InstrumentID << ";error:持仓类型无法判断PosiDirection=" << str_dir << endl;
            LOG(ERROR) << string(InstrumentID) + ";error:持仓类型无法判断PosiDirection=" + str_dir;

        }
           return tmpinfo;
    }
    //storeInvestorPosition(pInvestorPosition);
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
    req.OrderPriceType  ='2' ;

    ///买卖方向:
    req.Direction=userOrderField->_direction;

    ///组合开平标志
    req.CombOffsetFlag[0]=userOrderField->_offset_flag.c_str()[0];

    ///组合投机套保标志
    req.CombHedgeFlag[0]=userOrderField->_hedge_flag.c_str()[0];

    ///价格
    req.LimitPrice = userOrderField->_price;
    if(userOrderField->orderPriceType=='1')
    {
        InstrumentInfo*pins=dii.instruments[userOrderField->_instrumentID];
        if(!pins)
        {
            LOG(ERROR)<<"can not find instrument";
            return -1;
        }
        else
        {
            if( req.Direction=='0')//buy
                req.LimitPrice= pins->UpperLimitPrice;
            if( req.Direction=='1')//sell
                req.LimitPrice= pins->LowerLimitPrice;
        }
    }

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

    int iResult =_pUserApi->ReqOrderInsert(&req,_requestID++);
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
    int iResult=_pUserApi->ReqQryOrder(&req,_requestID++);
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
    {
        lock_guard<mutex> lck (ct.mtx);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if((userOrderField!=NULL))
        {
//            if(userOrderField->immediate_flag()==true)
                userOrderField->SetStatus('5');
//            else
//                userOrderField->SetStatus('7');
        }
        // save to db
    }
    dii.saveThostFtdcInputOrderFieldToDb(pInputOrder,rsp);
    return ;

}
///报单录入错误回报
void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    //    cout<<"----->>>>OnErrRtnOrderInsert"<<endl;

//    bool err = IsErrorRspInfo(pRspInfo);
//    string sInputOrderInfo = strInputOrderField(pInputOrder);
//    string prsp=strRspInfoField(pRspInfo);
//    LOG(INFO)<<(lexical_cast<string>(this)+"-------->>>OnErrRtnOrderInsert:"+sInputOrderInfo+prsp);

}

void CTraderSpi:: OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
    //    cout<<"------>>>OnErrRtnOrderAction"<<endl;
    string sOrderActionField = strOrderActionField(pOrderAction);
    string prsp=strRspInfoField(pRspInfo);
    LOG(INFO)<<(lexical_cast<string>(this)+"-------->>>OnErrRtnOrderAction:"+sOrderActionField+prsp);
}


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
    req.RequestID = _requestID++;

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


    int iResult = _pUserApi->ReqOrderAction(&req,_requestID++);
    string tmp=((iResult == 0) ? "success" : "failed");
    string msg="<<<--- ReqOrderAction: " + tmp+";";
    string orderinsertstr = strInputOrderActionField(&req);
    LOG(INFO) <<(lexical_cast<string>(this)+msg+orderinsertstr);
    return iResult;

}

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
        processtrade(pOrder);
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        lock_guard<mutex> lck (ct.mtx);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if(userOrderField!=NULL)
            userOrderField->SetStatus('5');
        return ;
    }

    if(pOrder->OrderStatus == '1')
    {
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        lock_guard<mutex> lck (ct.mtx);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if(userOrderField!=NULL)
            userOrderField->SetStatus('1');
        return ;
    }

    if(pOrder->OrderStatus == '2')
    {
        ChkThread&ct=  ChkThread::GetInstance();
        string key=GetKey2(pOrder);
        lock_guard<mutex> lck (ct.mtx);
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
        ChkThread&ct=  ChkThread::GetInstance();
        processHowManyHoldsCanBeClose(pOrder, "release");
        string key=GetKey2(pOrder);
        lock_guard<mutex> lck (ct.mtx);
        UserOrderField*userOrderField =  ct.get_Fuser_order(key);
        if((userOrderField!=NULL)&&(userOrderField->GetStatus()=='r')&&(strlen(pOrder->ActiveUserID)))
        {
            userOrderField->UpdateRef();
            userOrderField->UpdatePrice();
            //erase
            ct.eraseFOrder(key);

            int ret=userOrderField->ReqOrderInsert();
             // put
            if(ret!=0)
            {
                LOG(ERROR)<<"ReqOrderInsert fail investorid="<<userOrderField->_investorID;
            }
            else
            {
//                newuof->SetStatus('r');
                 userOrderField->begin_time=time(NULL);
                  ct.putFOrder(userOrderField);
                LOG(INFO)<<"###ReqOrderInsert investorid="<< userOrderField->_investorID;
            }
        }
        else
        {
            if(userOrderField!=NULL)
                userOrderField->SetStatus('5');
        }
        return ;

    }

    if((pOrder->OrderStatus == 'a') &&(strlen(pOrder->OrderSysID)==0))
    {
        processHowManyHoldsCanBeClose(pOrder, "lock");
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
    if(pTrade->OffsetFlag!='0')
    {
        CMdSpi*mdspi=CMdSpi::getInstance();
        mdspi->UnSubscribe(string(pTrade->InstrumentID));
    }

     int  volume=lexical_cast<int>(pTrade->Volume);
    //todo modify account
//    ReqQryTradingAccount();

//    ReqQryInvestorPosition();
    //    dii.deleteOriOrder(boost::lexical_cast<string>(pTrade->OrderSysID));
    ChkThread &ct=  ChkThread::GetInstance();
   lock_guard<mutex> lck (ct.mtx);


       string key=lexical_cast<string>(pTrade->InvestorID) +trim_copy(string(pTrade->OrderRef));
       UserOrderField*uof=ct.get_Fuser_order(key);
       if(uof!=NULL)
       {
           uof->_volume= uof->_volume-volume;
           if( uof->_volume==0)
               uof->SetStatus('5');
       }


    UpdateAcc&upt= UpdateAcc::GetInstance();
    upt.notify();
    return ;
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
    return ((pOrder->FrontID == _frontID) &&
            (pOrder->SessionID == _sessionID) &&
            (lexical_cast<int>(pOrder->OrderRef)==_orderRef) );
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
    return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
            (pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}


void CTraderSpi::startApi()
{
    _pUserApi->Init();
}

void CTraderSpi::stopApi()
{
    _pUserApi->RegisterSpi(NULL);
    _pUserApi->Release();
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
string CTraderSpi::config() const
{
    return _config;
}
vector<string>CTraderSpi::getParameter()
{
    vector<string>tmp;
    mtx.lock();
    tmp.push_back(_ratio);
    tmp.push_back(_priceType);
    tmp.push_back(_followTick);
    mtx.unlock();
    return tmp;
}
int CTraderSpi::sessionID() const
{
    return _sessionID;
}

void CTraderSpi::setSessionID(int sessionID)
{
    _sessionID = sessionID;
}
int CTraderSpi::orderRefInc()
{
    return _orderRef++;
}

void CTraderSpi::setOrderRef(int orderRef)
{
    _orderRef = orderRef;
}
int CTraderSpi::frontID() const
{
    return _frontID;
}

void CTraderSpi::setFrontID(int frontID)
{
    _frontID = frontID;
}
string CTraderSpi::brokerID() const
{
    return _brokerID;
}

void CTraderSpi::setBrokerID(const string &brokerID)
{
    _brokerID = brokerID;
}




void CTraderSpi::setConfig(const string &config)
{
    vector<string> config_list ;
    _config=config;
    boost::split(config_list,config,boost::is_any_of("/"));
    if(config_list.size()!=7)
        return ;
    //lock
    mtx.lock();
    //    _investorID=config_list[0];
    //    _password=config_list[1];
    _ratio=config_list[4];
    _priceType=(config_list[5]);
    _followTick=(config_list[6]);
    mtx.unlock();
    //unlock
}








