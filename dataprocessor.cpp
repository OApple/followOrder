#pragma once
#ifdef WIN32
#include <Windows.h>
typedef HMODULE		T_DLL_HANDLE;
#else
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <mysql++.h>
#include<ssqls.h>
#include <sqlite3.h>

#include "property.h"
#include "traderspi.h"
#include "niutraderspi.h"
#include "mysqlconnectpool.h"
#include"DBtable.h"
#include "user_order_field.h"
#include "dataprocessor.h"
using boost::locale::conv::between;
using boost::lexical_cast;
using boost::split;
using boost::is_any_of;
using boost::algorithm::trim_copy;

using namespace std;
// 请求编号
extern int iRequestID;

DataInitInstance::DataInitInstance(void){
}
DataInitInstance::~DataInitInstance(void)
{
}


//user=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2
void DataInitInstance::insert_follow_user(string users,vector<string>&userlist,vector<CTraderSpi*>& vac)
{
    string spreadList = users;
    vector<string> tmp_splists ;
    boost::split(tmp_splists,spreadList,boost::is_any_of(","));//
    for (unsigned int i = 0; i < tmp_splists.size();i++) {//num follow user
        vector<string> tmpacc ;
        boost::split(tmpacc,tmp_splists[i],boost::is_any_of("/"));
        CTraderSpi* ba=new CTraderSpi(*this,tmpacc[0]);
        ba->password=tmpacc[1];
        ba->ratio=tmpacc[2];
        ba->priceType=boost::lexical_cast<int>(tmpacc[3]);
        ba->followTick=boost::lexical_cast<int>(tmpacc[4]);
        userlist.push_back(tmpacc[0]);
        vac.push_back(ba);
    }

}

void DataInitInstance::GetConfigFromFile(){
    std::ifstream myfile("config/global.properties");
    if (!myfile) {
        LOG(ERROR) << "读取global.properties文件失败" << endl;
    }
    string str;
    while (getline(myfile, str)) {
        int pos = str.find("#");
        if (pos == 0) {
            //cout << "注释:" << str << endl;
            continue;
        }
        else {
            LOG(ERROR)  <<str<<endl;
            vector<string> name_value;
            split(name_value,str, is_any_of("="));
            if(name_value.size()!=2){
                continue;
            } else if ("investorid" == name_value[0]) {
                //INVESTOR_ID = vec[1];
                //strcpy(INVESTOR_ID, vec[1].c_str());
            }else if ("environment" == name_value[0]) {
                environment = name_value[1];
                //strcpy(BROKER_ID, vec[1].c_str());
            }
            else if ("marketServerIP" == name_value[0]) {
                marketServerIP = name_value[1];
            }
            else if ("marketServerPort" == name_value[0]) {
                marketServerPort = boost::lexical_cast<int>(name_value[1]);
            }else if ("tradeServerIP" == name_value[0]) {
                tradeServerIP = name_value[1];
            }
            else if ("tradeServerPort" == name_value[0]) {
                tradeServerPort = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("queryServerPort" == name_value[0]) {
                queryServerPort = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("queryServerIP" == name_value[0]) {
                queryServerIP = name_value[1];
            }else if ("exgTradeFrontIPCSHFE" == name_value[0]) {
                exgTradeFrontIPCSHFE = name_value[1];
            }else if ("exgParticipantIDCSHFE" == name_value[0]) {
                exgParticipantIDCSHFE = name_value[1];
            }else if ("brokerid" == name_value[0]) {
                BROKER_ID = name_value[1];
                //strcpy(BROKER_ID, vec[1].c_str());
            }else if ("exgTraderIDCSHFE" == name_value[0]) {
                exgTraderIDCSHFE = name_value[1];
            }else if ("exgTraderPasswdCSHFE" == name_value[0]) {
                exgTraderPasswdCSHFE = name_value[1];
            }
            else if ("exgFlowType" == name_value[0]) {
                exgFlowType = name_value[1];
            }
            else if ("profitValue" == name_value[0]) {
                //profitValue = boost::lexical_cast<double>(vec[1]);
            }
            else if ("remoteTradeServerPort" == name_value[0]) {
                remoteTradeServerPort = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("remoteMkdataServerPort" == name_value[0]) {
                mkdatasrvport = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("password" == name_value[0]) {
                PASSWORD = name_value[1];
                //strcpy(PASSWORD, vec[1].c_str());
            }else if ("loginid" == name_value[0]) {
                LOGIN_ID = name_value[1];
            }
            else if ("notActiveInsertAmount" == name_value[0]) {
                notActiveInsertAmount = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("arbVolumeMetric" == name_value[0]) {
                arbVolumeMetric = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("arbVolume" == name_value[0]) {
                arbVolume = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("orderInsertInterval" == name_value[0]) {
                orderInsertInterval = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("maxFollowTimes" == name_value[0]) {
                maxFollowTimes = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("maxUntradeNums" == name_value[0]) {
                maxUntradeNums = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("biasTickNums" == name_value[0]) {
                biasTickNums = boost::lexical_cast<int>(name_value[1]);
            }
            else if ("mdFrontAddr" == name_value[0]) {
                _market_front_addr= name_value[1];
            }
            else if ("tradeFrontAddr" == name_value[0]) {
                _trade_front_addr=name_value[1];
            }else if ("followTimes" == name_value[0]) {
                followTimes=boost::lexical_cast<int>(name_value[1]);
            }else if("DBHost"== name_value[0]){
                db_host=name_value[1];
            }else if("DBUser"==name_value[0]){
                db_user=name_value[1];
            }else if("DBPWD"==name_value[0]){
                db_pwd=name_value[1];
            }else if("DBMaxConnSize"==name_value[0]){
                db_maxConnSize=boost::lexical_cast<int>(name_value[1]);
            }else if("DBNAME"==name_value[0]){
                db_name = name_value[1];
            }else if("DBPORT"==name_value[0]){
                db_port=boost::lexical_cast<int>(name_value[1]);
            }else if("DBCharSet"==name_value[0]){
                db_charset=name_value[1];
            }else if("redis_host"==name_value[0]){
                redis_host=name_value[1];
            }
            else if("redis_port"==name_value[0]){
                redis_port=name_value[1];
            }
            else if("redis_pwd"==name_value[0]){
                redis_pwd=name_value[1];
            }
        }
    }
}

void DataInitInstance:: GetConfigFromRedis()
{
    redis_con=Redis(redis_host,redis_port,redis_pwd);
    redis_con.connect();
    string followUser=redis_con.get("followUser");
    LOG(ERROR) <<"redis followuser="<<followUser<<endl;
    vector<string> user_nman;
    boost::split(user_nman,followUser,boost::is_any_of("&"));//user~nman&user~nman
    int cnt_num=user_nman.size();
    for(unsigned int i = 0; i <cnt_num ;i++)
    {
        vector<string> tmp;
        boost::split(tmp,user_nman[i],boost::is_any_of("~"));//user~nman  tmp[0]=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2   tmp[1]=5555/78899

        vector<string> user;
        vector<CTraderSpi*> vac;
        insert_follow_user(tmp[0],user,vac);

        vector<string> nmanspli;
        boost::split(nmanspli,tmp[1],boost::is_any_of("/"));
        NiuTraderSpi* ba=new NiuTraderSpi(*this,nmanspli[0],(nmanspli[1]));
        ba->setFollow(vac);
        unordered_map<string, NiuTraderSpi*>::iterator it_map_strs = NBAccountMap.find(ba->getInvestorID());
        if (it_map_strs == NBAccountMap.end())
        {
            NBAccountMap[ba->getInvestorID()] = ba;
        }
        else
        {//update

        }
    }


}

void DataInitInstance::DataInit()
{
    mysql_pool = new MysqlConnectPool(db_name, db_host, db_user, db_pwd, db_port, db_charset, db_maxConnSize);


    mysqlpp::ScopedConnection con(*mysql_pool, true);

    mysqlpp::Query query = con->query("delete from ctp_investor_position");
    try
    {
        query.execute();
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR) << "exec error: " << er.what() << endl;
        query.execute("commit");
    }
}




//| variety_ch            |
//| instrument_id         |
//| broker_id             |
//| investor_id           |
//| posi_direction        |
//| hedge_flag            |
//| position_date         |
//| position              |
//| yd_position           |
//| long_frozen           |
//| short_frozen          |
//| long_frozen_amount    |
//| short_frozen_amount   |
//| open_volume           |
//| close_volume          |
//| open_amount           |
//| close_amount          |
//| position_cost         |
//| pre_margin            |
//| use_margin            |
//| frozen_margin         |
//| frozen_cash           |
//| frozen_commission     |
//| cash_in               |
//| commission            |
//| close_profit          |
//| position_profit       |
//| pre_settlement_price  |
//| settlement_price      |
//| trading_day           |
//| settlement_id         |
//| open_cost             |
//| exchange_margin       |
//| comb_position         |
//| comb_long_frozen      |
//| comb_short_frozen     |
//| close_profit_by_date  |
//| close_profit_by_trade |
//| today_position        |
//| margin_rate_by_money  |
//| margin_rate_by_volume |
//| strike_frozen         |
//| abandon_frozen        |
//| strike_frozen_amount  |
//| open_price            |

void DataInitInstance::saveThostFtdcInvestorPositionFieldToDb(CThostFtdcInvestorPositionField *pInvestorPosition)
{
    if(0==pInvestorPosition->Position)
        return ;
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    mysqlpp::Query query = con->query();

    string positonsql="insert into ctp_investor_position ("
            " variety_ch, "
            " instrument_id, "
            " broker_id,"
            "investor_id,"
            "posi_direction ,"
            "hedge_flag  , "
            "position_date,"
            "position,"
            "yd_position,"
            "long_frozen,"
            "short_frozen,"
            "long_frozen_amount,"
            "short_frozen_amount,"
            "open_volume,"
            "close_volume,"
            "open_amount ,"
            "close_amount,"
            "position_cost ,"
            "pre_margin   ,"
            "use_margin   ,"
            "frozen_margin  ,"
            "frozen_cash   ,"
            "frozen_commission ,"
            "cash_in       ,"
            "commission            ,"
            "close_profit          ,"
            "position_profit       ,"
            "pre_settlement_price  ,"
            "settlement_price      ,"
            "trading_day           ,"
            "settlement_id         ,"
            "open_cost             ,"
            "exchange_margin       ,"
            "comb_position         ,"
            "comb_long_frozen      ,"
            "comb_short_frozen     ,"
            "close_profit_by_date  ,"
            "close_profit_by_trade ,"
            "today_position        ,"
            "margin_rate_by_money  ,"
            "margin_rate_by_volume ,"
            "strike_frozen         ,"
            "abandon_frozen        ,"
            "strike_frozen_amount  )"
            "VALUES (";
    //boost::trim(positonsql);
    vector<string> res = getInstrumentIDZH(string(pInvestorPosition->InstrumentID));
    positonsql.append("'"+res[1]+"'")  ;positonsql.append(",");
    positonsql.append("'"+string(pInvestorPosition->InstrumentID)+"'")  ;positonsql.append(",'");
    positonsql.append(pInvestorPosition->BrokerID)  ;positonsql.append("','");
    positonsql.append(pInvestorPosition->InvestorID)  ;positonsql.append("',");
    positonsql.push_back(pInvestorPosition->PosiDirection);
    positonsql.append(",");
    positonsql.push_back(pInvestorPosition->HedgeFlag);
    positonsql.append(",");
    positonsql.push_back(pInvestorPosition->PositionDate);
    positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->Position))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->YdPosition))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->LongFrozen))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->ShortFrozen))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->LongFrozenAmount))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->ShortFrozenAmount))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->OpenVolume))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CloseVolume))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->OpenAmount))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CloseAmount))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->PositionCost))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->PreMargin))  ;positonsql.append(",");
    string tmp=boost::lexical_cast<string>(pInvestorPosition->UseMargin);
    positonsql.append(tmp.substr(0,tmp.find(".",0)+2))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->FrozenMargin))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->FrozenCash))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->FrozenCommission))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CashIn))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->Commission))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CloseProfit))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->PositionProfit))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->PreSettlementPrice))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->SettlementPrice))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->TradingDay))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->SettlementID))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->OpenCost))  ;positonsql.append(",");
    tmp=boost::lexical_cast<string>(pInvestorPosition->ExchangeMargin);
    positonsql.append(tmp.substr(0,tmp.find(".",0)+2))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CombPosition))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CombLongFrozen))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CombShortFrozen))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CloseProfitByDate))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->CloseProfitByTrade))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->TodayPosition))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->MarginRateByMoney))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->MarginRateByVolume))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->StrikeFrozen))  ;positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->AbandonFrozen));positonsql.append(",");
    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->StrikeFrozenAmount));positonsql.append(");");
    //    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->OpenPrice))
    //   LOG(ERROR) <<"position sql="<<positonsql<<endl;

    try
    {
        query.execute(positonsql);
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() << endl;
        query.execute("commit");
        return ;
    }
}


void DataInitInstance::saveThostFtdcOrderFieldToDb(CThostFtdcOrderField *pOrder)
{
    ctp_trade_record row;
    try
    {
        row.instrumentid = pOrder->InstrumentID;
        vector<string> res = getInstrumentIDZH(row.instrumentid);

        row.instrumentid_ch=res[0].c_str();
        row.variety_ch=res[1].c_str();

        //double price =((int)(pDepthMarketData->LastPrice + 0.005)*100)/100;
        row.price=pOrder->LimitPrice;
        row.volume=pOrder->VolumeTotalOriginal;
        row.offsetflag = pOrder->CombOffsetFlag;
        row.brokerid = pOrder->BrokerID;
        row.direction = pOrder->Direction;
        row.tradetype = "0";
        row.tradedatetime = mysqlpp::DateTime(time(NULL));
        row.tradeid=boost::algorithm::trim_copy(std::string(pOrder->OrderSysID));
        //row.tradedatetime_ctp=mysqlpp::DateTime(strTime.c_str());
        row.tradedatetime_ctp=mysqlpp::DateTime(time(NULL));
    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        LOG(ERROR)  << "Query error: " << er.what() << endl;
        return ;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        LOG(ERROR)  << "Conversion error: " << er.what() << endl <<
                       "\tretrieved data size: " << er.retrieved <<
                       ", actual size: " << er.actual_size << endl;
        return ;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        LOG(ERROR)  << "Error: " << er.what() << endl;
        return ;
    }
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    // mysqlpp:: Connection*p_con=con->;
    mysqlpp::Query query = con->query();
    query.insert(row);
    try
    {
        query.execute();
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() << endl;
        query.execute("commit");
        return ;
    }

}

void DataInitInstance::saveThostFtdcTradeFieldToDb(CThostFtdcTradeField *pTrade)
{
    ctp_trade_record row;
    try
    {
        row.instrumentid = pTrade->InstrumentID;
        vector<string> res = getInstrumentIDZH(row.instrumentid);

        row.instrumentid_ch=res[0].c_str();

        row.variety_ch=res[1].c_str();

        //double price =((int)(pDepthMarketData->LastPrice + 0.005)*100)/100;
        row.price=pTrade->Price;
        row.volume=pTrade->Volume;
        row.offsetflag = pTrade->OffsetFlag;
        row.brokerid = pTrade->BrokerID;
        row.direction = pTrade->Direction;
        row.tradetype = "0";
        row.investorid=pTrade->InvestorID;
        row.exchangeid=pTrade->ExchangeID;

        row.tradedatetime = mysqlpp::DateTime(time(NULL));
        row.tradeid=boost::algorithm::trim_copy(std::string(pTrade->OrderSysID));
        row.tradedatetime_ctp=mysqlpp::DateTime(time(NULL));
        row.feedback="交易成功";
    }catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        LOG(ERROR)  << "Query error: " << er.what() << endl;
        return ;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        LOG(ERROR)  << "Conversion error: " << er.what() << endl <<
                       "\tretrieved data size: " << er.retrieved <<
                       ", actual size: " << er.actual_size << endl;
        return ;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        LOG(ERROR)  << "Error: " << er.what() << endl;
        return ;
    }
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    mysqlpp::Query query = con->query();
    query.insert(row);
    try
    {
        query.execute();
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() << endl;
        query.execute("commit");
        return ;
    }

}

void DataInitInstance::saveThostFtdcInputOrderFieldToDb(CThostFtdcInputOrderField *pInputOrder,string  info)
{
    ctp_trade_record row;
    try
    {
        row.instrumentid = pInputOrder->InstrumentID;
        vector<string> res = getInstrumentIDZH(row.instrumentid);

        row.instrumentid_ch=res[0].c_str();

        row.variety_ch=res[1].c_str();

        //double price =((int)(pDepthMarketData->LastPrice + 0.005)*100)/100;
        row.price=pInputOrder->LimitPrice;
        row.volume=pInputOrder->VolumeTotalOriginal;
        row.offsetflag = pInputOrder->CombHedgeFlag[0];
        row.brokerid = pInputOrder->BrokerID;
        row.direction = pInputOrder->Direction;
        row.tradetype = "0";
        row.investorid=pInputOrder->InvestorID;
        //         row.exchangeid=pInputOrder->ExchangeID;

        row.tradedatetime = mysqlpp::DateTime(time(NULL));
        //         row.tradeid=boost::algorithm::trim_copy(std::string(pInputOrder->OrderSysID));
        row.tradedatetime_ctp=mysqlpp::DateTime(time(NULL));
        row.feedback=info;
    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        LOG(ERROR)  << "Query error: " << er.what() << endl;
        return ;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        LOG(ERROR)  << "Conversion error: " << er.what() << endl <<
                       "\tretrieved data size: " << er.retrieved <<
                       ", actual size: " << er.actual_size << endl;
        return ;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        LOG(ERROR)  << "Error: " << er.what() << endl;
        return ;
    }
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    //     con->set_option();
    mysqlpp::Query query = con->query();
    query.insert(row);
    try
    {
        query.execute();
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() << endl;
        query.execute("commit");
        return ;
    }

}

void DataInitInstance::saveCThostFtdcTradingAccountFieldToDb(CThostFtdcTradingAccountField *pTradingAccount)
{
    string positonsql="update ctp_account set "+
            //         " user_id      ,         "
            //         " ctp_account      ,     "
            //         " password        ,      "
            //         " create_time      ,     "
            //         " login_status     ,     "
            //         " modify_time     ,      "
            string("brokerid='")+ string(pTradingAccount->BrokerID)+"'"+
            " ,address_port='" +boost::lexical_cast<string>(_trade_front_addr)+"'"+
            " ,pre_balance=" +boost::lexical_cast<string>(pTradingAccount->PreBalance)   +
            " ,balance="  +   boost::lexical_cast<string>(pTradingAccount->Balance)+
            " ,bond="+boost::lexical_cast<string>(pTradingAccount->CurrMargin)+
            //         " income     ,           "
            //         " total_income ,         "
            //         " today_income_rate ,    "
            //         " income_rate       ,    "
            //         " reference_income_rate , "
            //         " total_trade_num     ,  "
            //         " single_max_income  ,   "
            //         " win_rate           ,   "
            //            " ,commission="+boost::lexical_cast<string>(pTradingAccount->Commission)+
            " ,close_profit= "+boost::lexical_cast<string>(pTradingAccount->CloseProfit)+
            " ,position_profit="+boost::lexical_cast<string>(pTradingAccount->PositionProfit)+
            " ,deposit="   +    boost::lexical_cast<string>(pTradingAccount->Deposit)  +
            " ,withdraw="  +    boost::lexical_cast<string>(pTradingAccount->Withdraw)    +
            //         " order_ref             "
            " ,total_assets="+boost::lexical_cast<string>(pTradingAccount->Balance)+
            //         " trade_days_num        "
            //         " profit_loss_rate      "
            //         " profit_num            "
            //         " loss_num              "
            //         " avg_profit            "
            //         " avg_loss              "
            //         " max_profit_trade_num  "
            //         " max_loss_trade_num    "
            //         " max_loss_days_num     "
            //         " bear_position_days  "
            "  where ctp_account='"+(pTradingAccount->AccountID)+"'";


    mysqlpp::ScopedConnection con(*mysql_pool, true);
    mysqlpp::Query query = con->query();
    try
    {
        query.execute(positonsql);
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() <<positonsql<< endl;
        query.execute("commit");
        return ;
    }
}


void DataInitInstance:: saveSettlementToDB(CTraderSpi &TraderSpi)
{
    int total_trade_num;
    double win_rate;
    double profit_loss_rate;
    int profit_num;
    int loss_num;
    string AccountID;

    total_trade_num=TraderSpi.total_trade_num();

    win_rate=(double)TraderSpi.profit_num()/TraderSpi.close_num();
    if(TraderSpi.loss()!=0)
        profit_loss_rate=(double)TraderSpi.profit()/TraderSpi.loss();

    profit_num=TraderSpi.profit_num();
    loss_num=TraderSpi.loss_num();
    AccountID=  TraderSpi.investorID();
    string positonsql="update ctp_account set "+
            string(",total_trade_num= ")+lexical_cast<string>(total_trade_num)+
            ",win_rate="+lexical_cast<string>(win_rate)+
            ",profit_loss_rate="+lexical_cast<string>(profit_loss_rate)+
            ",profit_num="+lexical_cast<string>(profit_num)+
            ",loss_num="+lexical_cast<string>(loss_num)+
            "  where ctp_account='"+(AccountID)+"'";
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    mysqlpp::Query query = con->query();
    try
    {
        query.execute(positonsql);
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() <<positonsql<< endl;
        query.execute("commit");
        return ;
    }
}

void DataInitInstance:: saveSettlementToDB(NiuTraderSpi &TraderSpi)
{
    int total_trade_num;
    double win_rate;
    double profit_loss_rate;
    int profit_num;
    int loss_num;
    string AccountID;

    total_trade_num=TraderSpi.total_trade_num();
    win_rate=(double)TraderSpi.profit_num()/TraderSpi.close_num();
    if(TraderSpi.loss()!=0)
        profit_loss_rate=(double)TraderSpi.profit()/TraderSpi.loss();

    profit_num=TraderSpi.profit_num();
    loss_num=TraderSpi.loss_num();
    AccountID=  TraderSpi.getInvestorID();
    string positonsql="replace INTO `ctp_tran_record` VALUES ("+
            string("'")+TraderSpi.getInvestorID()+
            "' ,"+
            string(",total_trade_num= ")+lexical_cast<string>(total_trade_num)+
            ",win_rate="+lexical_cast<string>(win_rate)+
            ",profit_loss_rate="+lexical_cast<string>(profit_loss_rate)+
            ",profit_num="+lexical_cast<string>(profit_num)+
            ",loss_num="+lexical_cast<string>(loss_num)+
            "  where ctp_account='"+(AccountID)+"'";
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    mysqlpp::Query query = con->query();
    try
    {
        query.execute(positonsql);
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() <<positonsql<< endl;
        query.execute("commit");
        return ;
    }
}


vector<string> DataInitInstance::getInstrumentIDZH(string InstrumentID)
{
    vector<string> res ;
    string variety_ch="";
    string instrumentid_ch="";
    boost::algorithm::to_upper(InstrumentID);
    for (int i = 0; i < InstrumentID.length(); i++)
    {
        char s = InstrumentID[i] ;
        if(s >='0' && s<='9')
            instrumentid_ch.push_back(s);
        else
            variety_ch.push_back(s);
    }

    variety_ch = getHeyueName(variety_ch);

    instrumentid_ch = variety_ch+instrumentid_ch;

    res.push_back(instrumentid_ch);
    res.push_back(variety_ch);
    return res;
}

string DataInitInstance::getHeyueName(string str)
{
    if("A"==str) return "豆一";
    else if("B"==str) return "豆二";
    else if("BB"==str) return "胶板";
    else if("C"==str) return "玉米";
    else if("CS"==str) return "淀粉";
    else if("FB"==str) return "纤板";
    else if("I"==str) return "铁矿";
    else if("J"==str) return "焦炭";
    else if("JD"==str) return "鸡蛋";
    else if("JM"==str) return "焦煤";
    else if("L"==str) return "塑料";
    else if("M"==str) return "豆粕";
    else if("P"==str) return "棕榈";
    else if("PP"==str) return "PP";
    else if("V"==str) return "PVC";
    else if("Y"==str) return "豆油";
    else if("AG"==str) return "白银";
    else if("AL"==str) return "铝";
    else if("AU"==str) return "黄金";
    else if("BU"==str) return "沥青";
    else if("CU"==str) return "铜";
    else if("FU"==str) return "燃油";
    else if("HC"==str) return "热卷";
    else if("NI"==str) return "镍";
    else if("PB"==str) return "铅";
    else if("RB"==str) return "螺纹";
    else if("RU"==str) return "橡胶";
    else if("SN"==str) return "锡";
    else if("WR"==str) return "线材";
    else if("ZN"==str) return "锌";
    else if("CF"==str) return "棉花";
    else if("FG"==str) return "玻璃";
    else if("JR"==str) return "粳稻";
    else if("LR"==str) return "晚稻";
    else if("MA"==str) return "甲醇";
    else if("OI"==str) return "菜油";
    else if("PM"==str) return "普麦";
    else if("RI"==str) return "早稻";
    else if("RM"==str) return "菜粕";
    else if("RS"==str) return "油籽";
    else if("SF"==str) return "硅铁";
    else if("SM"==str) return "锰硅";
    else if("SR"==str) return "白糖";
    else if("TA"==str) return "PTA";
    else if("WH"==str) return "强麦";
    else if("ZC"==str) return "动煤";
    else if("IC"==str) return "中证500股指";
    else if("IF"==str) return "IF";
    else if("IH"==str) return "IH";
    else if("T"==str) return "T";
    else if("TF"==str) return "TF";
    else if("AP"==str) return "鲜苹果";
    else return "";

}


void DataInitInstance::initTradeApi()
{
    for( unordered_map<string, NiuTraderSpi*>::iterator iter=NBAccountMap.begin();iter!=NBAccountMap.end();iter++ )
    {
        NiuTraderSpi* ba=iter->second;
        typedef  CTraderSpi*CTS;
        BOOST_FOREACH(CTS&cts,ba->getFollow())
        {
            cts->_pUserApi->Init();
        }

        ba->startApi();
        CTPInterface *interface=new CTPInterface();
        interface->pUserApi=ba->_pUserApi;
        interface->pUserSpi=ba;
        interface->investorID=ba->getInvestorID();
        interface->loginOK=false;
        tradeApiMap[ba->getInvestorID()]=interface;
    }
}

string DataInitInstance::getTime(){
    time_t timep;
    time (&timep);
    char tmp[64];
    //strftime(tmp, sizeof(tmp), "%Y-%m-%d~%H:%M:%S",localtime(&timep) );
    strftime(tmp, sizeof(tmp), "%Y-%m-%d",localtime(&timep) );
    return tmp;
}

#endif
