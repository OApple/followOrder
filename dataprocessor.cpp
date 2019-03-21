#pragma once
#ifdef WIN32
#include <Windows.h>
typedef HMODULE		T_DLL_HANDLE;
#else
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <signal.h>
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <mysql++.h>
#include<ssqls.h>
#include <sqlite3.h>

#include "property.h"
#include "traderspi.h"
#include "niutraderspi.h"
#include "mdspi.h"
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

DataInitInstance::~DataInitInstance(void)
{
}


//user=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2
 unordered_map<string, CTraderSpi*> DataInitInstance::makeSlaves(string users,vector<CTraderSpi*>& vac)
 {
     string spreadList = users;
     vector<string> tmp_splists ;

     unordered_map<string, CTraderSpi*>tmp;
     split(tmp_splists,spreadList,is_any_of(","));//
     for (unsigned int i = 0; i < tmp_splists.size();i++) {//num follow user
         CTraderSpi* ba=new CTraderSpi(*this,tmp_splists[i]);

         vector<string>user_config;
         split(user_config,tmp_splists[i],boost::is_any_of("/"));//
         if(user_config.size()!=7)
         {
             delete ba;
             continue;
         }
         vac.push_back(ba);
         tmp[user_config[0]]=ba;
     }
     return tmp;
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
            } else if ("MuseReal" == name_value[0]) {
                MuseReal = lexical_cast<int>(name_value[1]);
                //strcpy(BROKER_ID, vec[1].c_str());
            }
            else if ("SuseReal" == name_value[0]) {
                            SuseReal = lexical_cast<int>(name_value[1]);
                            //strcpy(BROKER_ID, vec[1].c_str());
                        }
            else if ("realTradeFrontAddr" == name_value[0]) {
                realTradeFrontAddr = name_value[1];
            }
           else if ("realBrokerID" == name_value[0]) {
                realBrokerID = name_value[1];
            }
            else if ("brokerID" == name_value[0]) {
                 broker_id = name_value[1];
             }


            else if ("mdFrontAddr" == name_value[0]) {
                _market_front_addr= name_value[1];
            }
            else if ("tradeFrontAddr" == name_value[0]) {
                _trade_front_addr=name_value[1];
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
            else if("redis_key"==name_value[0]){
                redis_key=name_value[1];
            }
        }
    }
}
//bool sortfunction (CTraderSpi* i,CTraderSpi* j)
//{ return (i->investorID()==j->investorID()); }
void server_on_exit(void)
{
     DataInitInstance& dii=DataInitInstance::GetInstance();
      dii.redis_con.set("start","0");
//      cout<<"ssssssssssss"<<endl;
}
void sig_act(int)
{
    server_on_exit();
     raise(SIGKILL);
//    exit(-1);
}
void DataInitInstance:: GetConfigFromRedis()
{
    redis_con=Redis(redis_host,redis_port,redis_pwd);
    redis_con.connect();
    slaveMasters=redis_con.get(redis_key);
    string key="start";
      string start=redis_con.get(key);
      if(start=="1")
      {
          cout<<"process is runing"<<endl;
          raise(SIGKILL);
          return ;
      }
      else
      {
//        atexit(server_on_exit);
         signal(SIGINT,  sig_act);
          signal(SIGTERM,  sig_act);
          redis_con.set("start","1");
      }
//slaveMasters=redis_con.getConfig();
    LOG(ERROR) <<"redis followuser="<<slaveMasters<<endl;
    vector<string> vslaveMaster;
    boost::split(vslaveMaster,slaveMasters,boost::is_any_of("&"));//user~nman&user~nman
    int cnt_num=vslaveMaster.size();
    for(unsigned int i = 0; i <cnt_num ;i++)
    {
        vector<string> slaveMaster;
        boost::split(slaveMaster,vslaveMaster[i],boost::is_any_of("~"));//user~nman  tmp[0]=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2   tmp[1]=5555/78899

        if(slaveMaster.size()!=2)
            continue;

        if(i==0)
        {

            CMdSpi*   mdspi=      new CMdSpi(*this,slaveMaster[1]);
            CMdSpi::mdspi=mdspi;
        }
        while(CMdSpi::mdspi->getStatus()==false)
        {
            this_thread::yield();
        }

        vector<CTraderSpi*> vSlave;
        unordered_map<string, CTraderSpi*>mSlave =  makeSlaves(slaveMaster[0],vSlave);


        NiuTraderSpi* ba=new NiuTraderSpi(*this,slaveMaster[1]);
        ba->setSlave(mSlave);
        masterAccountMap[ba->getInvestorID()] = ba;
        //    CTraderSpi*tmacc;

        typedef  unordered_map<string, CTraderSpi*>::value_type  const_pair;
        BOOST_FOREACH(const_pair&node,mSlave)
        {
            cout<<"follow users="<<node.first<<endl;
        }
    }
//new CMdSpi(*this,"slaveMaster[1]");

}

void DataInitInstance::DataInit()
{
    mysql_pool = new MysqlConnectPool(db_name, db_host, db_user, db_pwd, db_port, db_charset, db_maxConnSize);


    mysqlpp::ScopedConnection con(*mysql_pool, true);

//    mysqlpp::Query query = con->query("delete from ctp_investor_position");
//    try
//    {
//        query.execute();
//        query.execute("commit");
//    }
//    catch (const mysqlpp::Exception& er)
//    {
//        LOG(ERROR) << "exec error: " << er.what() << endl;
//        query.execute("commit");
//    }
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
            "strike_frozen_amount   ,"
            "open_price )"
            "VALUES (";
    //boost::trim(positonsql);
    vector<string> res = getInstrumentIDZH(string(pInvestorPosition->InstrumentID));
    positonsql.append("'"+res[1]+"'")  ;positonsql.append(",");
    positonsql.append("'"+string(pInvestorPosition->InstrumentID)+"'")  ;positonsql.append(",'");
    positonsql.append(pInvestorPosition->BrokerID)  ;positonsql.append("','");
    positonsql.append(pInvestorPosition->InvestorID)  ;positonsql.append("',");
    positonsql.push_back(pInvestorPosition->PosiDirection);positonsql.append(",");

    positonsql.push_back(pInvestorPosition->HedgeFlag); positonsql.append(",");
    positonsql.push_back(pInvestorPosition->PositionDate);positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->Position))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->YdPosition))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->LongFrozen))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->ShortFrozen))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->LongFrozenAmount))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->ShortFrozenAmount))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->OpenVolume))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CloseVolume))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->OpenAmount))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CloseAmount))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->PositionCost))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->PreMargin))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->UseMargin))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->FrozenMargin))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->FrozenCash))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->FrozenCommission))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CashIn))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->Commission))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->CloseProfit))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->PositionProfit))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->PreSettlementPrice))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->SettlementPrice))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->TradingDay))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->SettlementID))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->OpenCost))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->ExchangeMargin))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CombPosition))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CombLongFrozen))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->CombShortFrozen))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CloseProfitByDate))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->CloseProfitByTrade))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->TodayPosition))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->MarginRateByMoney))  ;positonsql.append(",");

    positonsql.append(lexical_cast<string>(pInvestorPosition->MarginRateByVolume))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->StrikeFrozen))  ;positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->AbandonFrozen));positonsql.append(",");
    positonsql.append(lexical_cast<string>(pInvestorPosition->StrikeFrozenAmount));positonsql.append(",");
    double open_price=0;
    int muli=getInstrumentMulti(pInvestorPosition->InstrumentID);
     open_price=pInvestorPosition->OpenCost/(muli*pInvestorPosition->Position);
     positonsql.append(lexical_cast<string>(open_price)); positonsql.append(");");
    //    positonsql.append(boost::lexical_cast<string>(pInvestorPosition->OpenPrice))
//       LOG(ERROR) <<"position sql="<<positonsql<<endl;
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
        row.tradeid=boost::algorithm::trim_copy(std::string(pTrade->TradeID));
        //    tradedatetime_ctp=mysqlpp::DateTime(time(NULL));
        string datetime=string(pTrade->TradeDate)+" "+pTrade->TradeTime;
//        cout<<tradedatetime_ctp<<endl;
        row.tradedatetime_ctp=mysqlpp::DateTime(datetime);
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
        row.offsetflag = pInputOrder->CombOffsetFlag[0];
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
//        query.execute("commit");
        return ;
    }
}

void DataInitInstance::saveDepthMarketDataToDb(CThostFtdcDepthMarketDataField *pDepthMarketData/*,string investor_id*/)
{
    mysqlpp::ScopedConnection con(*mysql_pool, true);
    mysqlpp::Query query = con->query();

//BidPrice1
    double LastPrice=lexical_cast<double>(pDepthMarketData->LastPrice);
    string instrument=lexical_cast<string>(pDepthMarketData->InstrumentID);
     int muti=getInstrumentMulti(instrument);
    string positonsql=string("update ctp_investor_position set close_profit_by_trade=(")+lexical_cast<string>(LastPrice)+
           "-open_price)*position*" +lexical_cast<string>(muti)+" where instrument_id= '"+instrument+
            "' and posi_direction=2";//DUO
//  LOG(ERROR)<<positonsql;
    try
    {
        query.execute(positonsql);
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() << endl;
         LOG(ERROR)<<positonsql;
        query.execute("commit");
        return ;
    }
    positonsql=string("update ctp_investor_position set close_profit_by_trade=(")+
               "open_price-"+lexical_cast<string>(LastPrice)+")*position*"+lexical_cast<string>(muti)+" where instrument_id= '"+instrument+
                "' and posi_direction=3";//KONG
//  LOG(ERROR)<<positonsql;
    try
    {
        query.execute(positonsql);
        query.execute("commit");
    }
    catch (const mysqlpp::Exception& er)
    {
        LOG(ERROR)  << "exec error: " << er.what() << endl;
         LOG(ERROR)<<positonsql;
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
            instrumentid_ch.push_back(s);//number
        else
            variety_ch.push_back(s);//alpha
    }

    variety_ch = getHeyueName(variety_ch);

    instrumentid_ch = variety_ch+instrumentid_ch;

    res.push_back(instrumentid_ch);
    res.push_back(variety_ch);
    return res;
}
double DataInitInstance::getPriceTick(string InstrumentID)
{
    string productid="";
    string instrumentid_ch="";
    boost::algorithm::to_upper(InstrumentID);
    for (int i = 0; i < InstrumentID.length(); i++)
    {
        char s = InstrumentID[i] ;
        if(s >='0' && s<='9')
            instrumentid_ch.push_back(s);//number
        else
            productid.push_back(s);//alpha
    }

    if("A"==productid) return 1;
    else if("B"==productid) return 1;
    else if("BB"==productid) return 0.05;
    else if("C"==productid) return 1;
    else if("CS"==productid) return 1;
    else if("FB"==productid) return 0.05;
    else if("I"==productid) return 0.5;
    else if("J"==productid) return 0.5;
    else if("JD"==productid) return 1;
    else if("JM"==productid) return 0.5;
    else if("L"==productid) return 5;
    else if("M"==productid) return 1;
    else if("P"==productid) return 2;
    else if("PP"==productid) return 1;
    else if("V"==productid) return 5;
    else if("Y"==productid) return 2;
    else if("AG"==productid) return 1;
    else if("AL"==productid) return 5;
    else if("AU"==productid) return 0.05;
    else if("BU"==productid) return 2;
    else if("CU"==productid) return 10;
    else if("FU"==productid) return 1;
    else if("HC"==productid) return 1;
    else if("NI"==productid) return 10;
    else if("PB"==productid) return 5;
    else if("RB"==productid) return 1;
    else if("RU"==productid) return 5;
    else if("SN"==productid) return 10;
    else if("WR"==productid) return 1;
    else if("ZN"==productid) return 5;
    else if("CF"==productid) return 5;
    else if("FG"==productid) return 1;
    else if("JR"==productid) return 1;
    else if("LR"==productid) return 1;
    else if("MA"==productid) return 1;
    else if("OI"==productid) return 1;
    else if("PM"==productid) return 1;
    else if("RI"==productid) return 1;
    else if("RM"==productid) return 1;
     else if("CY"==productid) return 5;
    else if("RS"==productid) return 1;
    else if("SF"==productid) return 2;
    else if("SP"==productid) return 2;
    else if("SM"==productid) return 2;
    else if("SR"==productid) return 1;
    else if("TA"==productid) return 2;
    else if("WH"==productid) return 1;
    else if("ZC"==productid) return 0.2;
    else if("IC"==productid) return 0.2;
    else if("IF"==productid) return 0.2;
    else if("IH"==productid) return 0.2;
    else if("T"==productid) return 0.005;
    else if("TF"==productid) return 0.005;
    else if("TS"==productid) return 0.005;
    else if("AP"==productid) return 1;
    else if("EG"==productid) return 1;
    else if("SC"==productid) return 0.1;
    else return 1;

}

int DataInitInstance::getInstrumentMulti(string InstrumentID)
{
    string productid="";
    string instrumentid_ch="";
    boost::algorithm::to_upper(InstrumentID);
    for (int i = 0; i < InstrumentID.length(); i++)
    {
        char s = InstrumentID[i] ;
        if(s >='0' && s<='9')
            instrumentid_ch.push_back(s);//number
        else
            productid.push_back(s);//alpha
    }

    if("A"==productid) return 10;
    else if("B"==productid) return 10;
    else if("BB"==productid) return 500;
    else if("C"==productid) return 10;
    else if("CS"==productid) return 10;
    else if("FB"==productid) return 500;
    else if("I"==productid) return 100;
    else if("J"==productid) return 100;
    else if("JD"==productid) return 10;
    else if("JM"==productid) return 60;
    else if("L"==productid) return 5;
    else if("M"==productid) return 10;
    else if("P"==productid) return 10;
    else if("PP"==productid) return 5;
    else if("V"==productid) return 5;
    else if("Y"==productid) return 10;
    else if("AG"==productid) return 15;
    else if("AL"==productid) return 5;
    else if("AU"==productid) return 1000;
    else if("BU"==productid) return 10;
    else if("CU"==productid) return 5;
    else if("FU"==productid) return 10;
    else if("HC"==productid) return 10;
    else if("NI"==productid) return 1;
    else if("PB"==productid) return 5;
    else if("RB"==productid) return 10;
    else if("RU"==productid) return 10;
    else if("SN"==productid) return 1;
    else if("WR"==productid) return 10;
    else if("ZN"==productid) return 5;
    else if("CF"==productid) return 5;
    else if("FG"==productid) return 20;
    else if("JR"==productid) return 20;
    else if("LR"==productid) return 20;
    else if("MA"==productid) return 10;
    else if("OI"==productid) return 10;
    else if("PM"==productid) return 50;
    else if("RI"==productid) return 20;
    else if("RM"==productid) return 10;
     else if("CY"==productid) return 5;
    else if("RS"==productid) return 10;
    else if("SF"==productid) return 5;
    else if("SP"==productid) return 10;
    else if("SM"==productid) return 5;
    else if("SR"==productid) return 10;
    else if("TA"==productid) return 5;
    else if("WH"==productid) return 20;
    else if("ZC"==productid) return 100;
    else if("IC"==productid) return 200;
    else if("IF"==productid) return 300;
    else if("IH"==productid) return 300;
    else if("T"==productid) return 10000;
    else if("TF"==productid) return 10000;
    else if("TS"==productid) return 20000;
    else if("AP"==productid) return 10;
    else if("EG"==productid) return 10;
    else if("SC"==productid) return 1000;
    else return 1;
}

void DataInitInstance::addMaster(vector<string> &slave_master)
{
    vector<CTraderSpi*> vac;
    unordered_map<string, CTraderSpi*>mSlaves =  makeSlaves(slave_master[0],vac);
    NiuTraderSpi* ba=new NiuTraderSpi(*this,slave_master[1]);
    ba->setSlave(mSlaves);
    masterAccountMap[ba->getInvestorID()] = ba;
    ba->startApi();
}

void DataInitInstance::delMaster(const string &master)
{
    auto it=masterAccountMap.find(master);
    masterAccountMap.erase(master);
    NiuTraderSpi *ntp=it->second;
      delete ntp;
}

void DataInitInstance::startMaster(const string &master)
{
    auto it = masterAccountMap.find(master);
     if (it != masterAccountMap.end())
         it->second->startApi();
}

NiuTraderSpi *DataInitInstance::getMaster(const string &master)
{
    auto it = masterAccountMap.find(master);
     if (it != masterAccountMap.end())
           return it->second;
     else
         return nullptr;
}

set<string> DataInitInstance::getMaster()
{
    set<string>tmp;
    for(auto&master:masterAccountMap)tmp.insert(master.first);
    return  tmp;
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
    else if("CY"==str) return "棉纱";
    else if("RS"==str) return "油籽";
    else if("SF"==str) return "硅铁";
    else if("SP"==str) return "木浆";
    else if("SM"==str) return "锰硅";
    else if("SR"==str) return "白糖";
    else if("TA"==str) return "PTA";
    else if("WH"==str) return "强麦";
    else if("ZC"==str) return "动煤";
    else if("IC"==str) return "中证500股指";
    else if("IF"==str) return "沪深300指数";
    else if("IH"==str) return "上证50指数";
    else if("T"==str) return "10年期国债";
    else if("TF"==str) return "5年期国债";
    else if("TS"==str) return "2年期国债";
    else if("AP"==str) return "鲜苹果";
    else if("EG"==str) return "乙二醇";
    else if("SC"==str) return "原油";
    else return "";

}


void DataInitInstance::startTradeApi()
{
    for( unordered_map<string, NiuTraderSpi*>::iterator iter=masterAccountMap.begin();iter!=masterAccountMap.end();iter++ )
    {
        NiuTraderSpi* ba=iter->second;
//        typedef  CTraderSpi*CTS;
//        typedef  unordered_map<string, CTraderSpi*>::value_type  cconst_pair;
//        BOOST_FOREACH(cconst_pair&node,ba->getSlave())
//        {
//            (node.second)->startApi();
//        }
        ba->startApi();
//        CTPInterface *interface=new CTPInterface();
//        interface->pUserApi=ba->_pUserApi;
//        interface->pUserSpi=ba;
//        interface->investorID=ba->getInvestorID();
//        interface->loginOK=false;
//        tradeApiMap[ba->getInvestorID()]=interface;
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
