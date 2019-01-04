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
//extern unordered_map<string, CTraderSpi*> followNBAccountMap;
//extern unordered_map<string, NiuTraderSpi*> NBAccountMap;

DataInitInstance::DataInitInstance(void){
}
DataInitInstance::~DataInitInstance(void)
{
}
void DataInitInstance::initExgTraderApi(){
    string logdir="./log";
    /*
    cshfeTraderApi=CShfeFtdcTraderApi::CreateFtdcTraderApi(logdir.c_str());
    cshfeTraderSpi=new CSHFETraderSpi();
    cshfeTraderApi->RegisterSpi(cshfeTraderSpi);
    cshfeTraderApi->SetHeartbeatTimeout(10);
    */
    //cshfeTraderApi->SubscribePublicTopic(TERT_RESTART);
    //cshfeTraderApi->SubscribePrivateTopic(TERT_RESTART);//TERT_QUICK
    /*
    char tmpfnt[60];
    strcpy(tmpfnt,exgTradeFrontIPCSHFE.c_str());
    cshfeTraderApi->RegisterFront(tmpfnt);
    cout<<"exchange front ip="+exgTradeFrontIPCSHFE<<endl;
    if(cshfeTraderApi->OpenRequestLog("./log/cshfe.log")==0){
        LOG(ERROR)<<"open request info file OK.";
    }else{
        LOG(ERROR)<<"open request file failed!";
    }
    if(cshfeTraderApi->OpenResponseLog("./log/cshfe_rsp.log")==0){
        LOG(ERROR)<<"open response info file OK.";
    }else{
        LOG(ERROR)<<"open response file failed!";
    }
    cshfeTraderApi->Init();
    */
}

/*
void DataInitInstance::initExgTraderApi(){
    string dir="./log";
    ctpex_proxy_driver_log_set log_set;
    log_set.b_log_stdout=true;
    log_set.b_request_log=true;
    log_set.b_response_log=true;
    log_set.b_log_file=true;
    string user_id=exgTraderIDCSHFE;
    string participant_id=exgParticipantIDCSHFE;
    string password=exgTraderPasswdCSHFE;
    string user_product_info="";
    string trader_private_topic_resume_type=exgFlowType;
    int data_center_id=1;
    string fnt=exgTradeFrontIPCSHFE;
    ctpex_proxy_trader_cshfe* mytrader=new ctpex_proxy_trader_cshfe();
    ctpex_proxy_trader_spi* myspi=new ctpex_proxy_trader_spi();
    mytrader->init(dir,user_id,participant_id,password,user_product_info,trader_private_topic_resume_type,data_center_id,log_set);
    mytrader->register_spi(myspi);
    mytrader->add_front_server(fnt);
    mytrader->open();
    cout << "Hello World!" << endl;
}*/
//user=126373/123456/1:1/1/1,122467/lhh520/1:5/2/2
void DataInitInstance::insert_follow_user(string users,vector<string>&userlist,vector<CTraderSpi*>& vac)
{
    string spreadList = users;
    vector<string> tmp_splists ;
    boost::split(tmp_splists,spreadList,boost::is_any_of(","));//
    for (unsigned int i = 0; i < tmp_splists.size();i++) {//num follow user
        // string tmp_str = tmp_splists[i];
        //many nb account
        vector<string> tmpacc ;
        boost::split(tmpacc,tmp_splists[i],boost::is_any_of("/"));
        //account concat by $
        //    for(int i=0;i<tmpacc.size();i++)
        //        cout<<tmpacc[i]<<endl;
        CTraderSpi* ba=new CTraderSpi(*this,tmpacc[0]);
        ba->password=tmpacc[1];
        ba->ratio=tmpacc[2];
        ba->priceType=boost::lexical_cast<int>(tmpacc[3]);
        ba->followTick=boost::lexical_cast<int>(tmpacc[4]);
//        ba->isNiu=false;
        userlist.push_back(tmpacc[0]);
        vac.push_back(ba);
        //account concat by $
        //  vector<string> instr_s = UniverseTools::split(followAccountStr[1],"$");
        //  ba->nbman=instr_s;
        //        unordered_map<string, UserAccountInfo*>::iterator it_map_strs = followNBAccountMap.find(tmpacc[0]);
        //        if (it_map_strs == followNBAccountMap.end()) {
        //            this->followNBAccountMap[tmpacc[0]] = ba;
        //        }else {//update

        //        }
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



            //  126373/123456/1:1/1/1,122467/lhh520/1:5/2/2
            //            else if ("follow" == name_value[0]) {
            //                cout<<name_value[1]<<endl;
            //                string spreadList = name_value[1];
            //                vector<string> tmp_splists = UniverseTools::split(spreadList,",");//
            //                for (unsigned int i = 0; i < tmp_splists.size();i++) {//num follow user
            //                    string tmp_str = tmp_splists[i];
            //                    //many nb account
            //                    vector<string> followAccountStr = UniverseTools::split(tmp_str,"~");
            //                    //account concat by $
            //                    vector<string> tmpacc = UniverseTools::split(followAccountStr[0],"%");
            //                    UserAccountInfo* ba=new UserAccountInfo();
            //                    ba->investorID=tmpacc[0];
            //                    ba->password=tmpacc[1];
            //                    //account concat by $
            //                  //  vector<string> instr_s = UniverseTools::split(followAccountStr[1],"$");
            //                  //  ba->nbman=instr_s;
            //                    unordered_map<string, BaseAccount*>::iterator it_map_strs = this->followNBAccountMap.find(tmpacc[0]);
            //                    if (it_map_strs == this->followNBAccountMap.end()) {
            //                        this->followNBAccountMap[tmpacc[0]] = ba;
            //                        //allAccountMap[tmpacc[0]]=ba;
            //                    }else {//update

            //                    }

            //                }

            //                /*
            //                for( unordered_map<string, BaseAccount*>::iterator iter=this->followNBAccountMap.begin();iter!=this->followNBAccountMap.end();iter++ ){
            //                       cout<<"key="<<iter->first<<endl;
            //                       UserAccountInfo* ua=(UserAccountInfo*)iter->second;
            //                       cout<<"pwd="<<ua->password<<endl;
            //                }*/

            //            }else if ("nbman" == name_value[0]) {
            //                cout<<name_value[1]<<endl;
            //                string spreadList = name_value[1];
            //                vector<string> tmp_splists = UniverseTools::split(spreadList,",");
            //                for (unsigned int i = 0; i < tmp_splists.size();i++) {
            //                    string tmp_str = tmp_splists[i];
            //                    vector<string> followAccountStr = UniverseTools::split(tmp_str,"~");
            //                    vector<string> tmpacc = UniverseTools::split(followAccountStr[0],"%");
            //                    UserAccountInfo* ba=new UserAccountInfo();
            //                    ba->investorID=tmpacc[0];
            //                    ba->password=tmpacc[1];
            //                    vector<string> instr_s = UniverseTools::split(followAccountStr[1],"$");
            //                    ba->nbman=instr_s;
            //                    unordered_map<string, BaseAccount*>::iterator it_map_strs = this->NBAccountMap.find(tmpacc[0]);
            //                    if (it_map_strs == NBAccountMap.end()) {//未建立配对关系,需新建一个vector
            //                        NBAccountMap[tmpacc[0]] = ba;
            //                        allAccountMap[tmpacc[0]]=ba;
            //                    }else {

            //                    }
            //                }
            //                for( unordered_map<string, BaseAccount*>::iterator iter=NBAccountMap.begin();iter!=NBAccountMap.end();iter++ ){
            //                    cout<<"NBAmap key="<<iter->first<<endl;
            //                }
            //            }





            //            else if ("instrumentList" == name_value[0]) {

            //                const char *expr = name_value[1].c_str();
            //                //cout<<expr<<endl;
            //                char *inslist = new char[strlen(expr) + 1];
            //                strcpy(inslist, expr);
            //                //cout<<inslist<<endl;
            //                const char * splitlt = ","; //分割符号
            //                char *plt = 0;
            //                plt = strtok(inslist, splitlt);
            //                while (plt != NULL) {
            //                    quoteList.push_back(plt);
            //                    //cout<<plt<<endl;
            //                    plt = strtok(NULL, splitlt); //指向下一个指针
            //                }
            //                //动态分配字符数组
            //                ppInstrumentID = new char*[quoteList.size()];
            //                for (int i = 0, j = quoteList.size(); i < j; i++) {
            //                    const char * tt2 = quoteList[i].c_str();
            //                    char* pid = new char[strlen(tt2) + 1];
            //                    strcpy(pid, tt2);
            //                    ppInstrumentID[i] = pid;
            //                    cout<<ppInstrumentID[i]<<endl;
            //                }
            //                iInstrumentID = quoteList.size();
            //            }
        }

    }


    //MySQLPool.initDBSetting(db_host,db_user,db_pwd,db_maxConnSize);



    //    mysqlpp::ScopedConnection conn(*g_db_pool, false);
    //    mysqlpp::Query query = conn->query("select * from testsql");

    //        mysqlpp::StoreQueryResult ares = query.store();
    //        std::cout << "ares.num_rows() = " << ares.num_rows() << std::endl;
    //        for (size_t i = 0; i < ares.num_rows(); i++)
    //        {
    //            std::cout << "your data" << std::endl;
    //            std::cout << "id: " << ares[i]["id"] <<  std::endl;
    //        }

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
//        ba->nbman=user;
        ba->setFollow(vac);
        //        ba->isNiu=true;
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

//bool DataInitInstance::isNBMANTrade(CThostFtdcTradeField *pTrade){
//    string clientid=boost::lexical_cast<string>(pTrade->InvestorID);
//    unordered_map<string, UserAccountInfo*>::iterator nbIT=NBAccountMap.find(clientid);
//    if(nbIT==NBAccountMap.end()){//not find
//        return false;
//    }else{
//        return true;
//    }
//}
//bool DataInitInstance::isNBMAN(string investorID){
//    unordered_map<string, UserAccountInfo*>::iterator nbIT=NBAccountMap.find(investorID);
//    if(nbIT==NBAccountMap.end()){//not find
//        cout<<"investorID="+investorID+" is not naman."<<endl;
//        return false;
//    }else{
//        cout<<"investorID="+investorID+" is naman."<<endl;
//        return true;
//    }
//}



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

        //  std::string strTime = boost::posix_time::to_iso_string(getCurrentTimeByBoost());
        // 这时候strTime里存放时间的格式是YYYYMMDDTHHMMSS，日期和时间用大写字母T隔开了

        //cout<<strTime<<endl;
        //       int pos = strTime.find('T');
        //       strTime.replace(pos,1,std::string("-"));
        //       strTime.replace(pos + 3,0,std::string(":"));
        //       strTime.replace(pos + 6,0,std::string(":"));
        // cout<<strTime<<endl;
        //    row.tradedatetime = mysqlpp::DateTime(strTime.c_str());
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

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
 if(argv[0])
   *(int*)data=lexical_cast<int>(argv[0]);
//   for(i=0; i<argc; i++){
//      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//   }
//   printf("\n");
   return 0;
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



//bool DataInitInstance::isNBMANOrder(CThostFtdcOrderField *pOrder){
//    string clientid=boost::lexical_cast<string>(pOrder->InvestorID);
//    unordered_map<string, UserAccountInfo*>::iterator nbIT=NBAccountMap.find(clientid);
//    if(nbIT==NBAccountMap.end()){//not find
//        return false;
//    }else
//        return true;
//}
void DataInitInstance::delTraderApi(string investorID){
    unordered_map<string, CTPInterface*>::iterator iter=tradeApiMap.find(investorID);
    if(iter!=tradeApiMap.end()){
        LOG(ERROR) <<"delTraderApi:delete investorID="+investorID+"'s trader api"<<endl;
        tradeApiMap.erase(iter);
    }
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
//CTPInterface* DataInitInstance::instanceTradeApi(  UserAccountInfo* ba)
//{
//mkdir();
//    string prefix=investorID+getTime();

//    string prefix=ba->investorID+"/"+getTime()+"/";

//    system(("mkdir  -p "+prefix).c_str());
//    // 初始化UserApi
//    CThostFtdcTraderApi* pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(prefix.c_str());			// 创建UserApi
//   CThostFtdcTraderSpi *pUserSpi=NULL;
//    if(ba->isNiu)
//    {
//        pUserSpi= new NiuTraderSpi(*this,ba,false,pUserApi);
//        pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// 注册事件类
//        pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);					// 注册公有流
//        pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);					// 注册私有流
//        pUserApi->RegisterFront(TRADE_FRONT_ADDR);
//        pUserApi->Init();
//        ba->pUserApi=pUserApi;
//        ba->pUserSpi=pUserSpi;
//    }
//    else
//    {
//        pUserSpi = new CTraderSpi(*this,ba,false,pUserApi);

//        pUserApi->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi);			// 注册事件类
//        pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);					// 注册公有流
//        pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);					// 注册私有流
//        pUserApi->RegisterFront(TRADE_FRONT_ADDR);
//        pUserApi->Init();
//        ba->pUserApi=pUserApi;
//        ba->pUserSpi=pUserSpi;
//    }
//   CTraderSpi::_tradeApiMap[ba->investorID]=pUserApi;
//   CTraderSpi::_tradeSpiMap[ba->investorID]=pUserSpi;

//    CTPInterface *interface=new CTPInterface();
//    interface->pUserApi=pUserApi;
//    interface->pUserSpi=pUserSpi;

//    string msg=lexical_cast<string>(pUserSpi)+
//            "market="+lexical_cast<string>(MARKET_FRONT_ADDR)+
//            ";"+"trade="+lexical_cast<string>(TRADE_FRONT_ADDR);
//    LOG(INFO)<<msg;
//    return interface;
//}
//CTPInterface* DataInitInstance::getTradeApi(string investorID){
//    cout<<"investorID="+investorID<<endl;
//    unordered_map<string, CTPInterface*>::iterator famiter = tradeApiMap.find(investorID);
//    if (famiter == tradeApiMap.end()) {//未建立配对关系,需新建一个vector
//        string msg="getTradeApi:investorID="+investorID+",can't find tradeApi in tradeApiMap.";
//        cerr<<msg<<endl;
//        LOG(ERROR) <<msg;
//        return NULL;
//    }else{
//        CTPInterface* interface=famiter->second;
//        return interface;
//    }
//}
double DataInitInstance::getPriceTick(string instrumentID) {
    //合约priceTick
    unordered_map<string, InstrumentInfo*>::iterator ins_it = instruments.find(instrumentID);
    if (ins_it == instruments.end()) {
        LOG(ERROR) << "无法查找到合约信息.instrumentID=" + instrumentID;
        return 0;
    }
    InstrumentInfo* insinfo = ins_it->second;
    double priceTick = insinfo->PriceTick;
    return priceTick;
}
double DataInitInstance::getTickMetric(string instrumentID) {
    //合约priceTick
    unordered_map<string, InstrumentInfo*>::iterator ins_it = instruments.find(instrumentID);
    if (ins_it == instruments.end()) {
        LOG(ERROR) << "无法查找到合约信息.instrumentID=" + instrumentID;
        return 0;
    }
    InstrumentInfo* insinfo = ins_it->second;
    double priceTick = insinfo->PriceTick;
    double multiplyFactor = insinfo->VolumeMultiple;
    return priceTick*multiplyFactor;
}
double DataInitInstance::getMultipler(string instrumentID) {
    //合约乘数
    unordered_map<string, InstrumentInfo*>::iterator ins_it = instruments.find(instrumentID);
    if (ins_it == instruments.end()) {
        LOG(ERROR) << "无法查找到合约信息.instrumentID=" + instrumentID;
        return 0;
    }
    InstrumentInfo* insinfo = ins_it->second;
    double multiplyFactor = insinfo->VolumeMultiple;
    return multiplyFactor;
}

/*
UserAccountInfo* DataInitInstance::getUserAccount(string investorID){
    unordered_map<string, UserAccount*>::iterator usIT = followNBAccountMap.find(investorID);
    if (usIT != followNBAccountMap.end()) {//未建立配对关系,需新建一个vector
        string msg="getUserAccount:investorID="+investorID+"'s account is existed.";
        LOG(INFO)<<msg;
        return (UserAccountInfo*)usIT->second;
    }else {
        string msg="getUserAccount:investorID="+investorID+"'s account is not existed.";
        return NULL;
    }
}
void DataInitInstance::setUserAccount(UserAccount* ua){
    userAccMap[boost::lexical_cast<string>(ua->pTradingAccount->AccountID)]=ua;

}*/



void DataInitInstance::processOrder(CThostFtdcOrderField *pOrder,string type){
    string dir=boost::lexical_cast<string>(pOrder->Direction);
    if(dir=="0"){//buy
        if(type=="delete"){
            for(list<OrderInfo*>::iterator untrade_it = bidList.begin();untrade_it!=bidList.end();untrade_it++){
                OrderInfo* order=*untrade_it;
                if(order->frontID==pOrder->FrontID && order->sessionID==pOrder->SessionID && order->orderRef==pOrder->OrderRef){
                    DLOG(INFO) << ("processOrder,delete:frontid=" + boost::lexical_cast<string>(pOrder->FrontID) + ",sessionid="+boost::lexical_cast<string>(pOrder->SessionID)+",orderRef="+boost::lexical_cast<string>(string(pOrder->OrderRef))+","+
                                   "volume delete from "+boost::lexical_cast<string>(order->volume)+" to "+boost::lexical_cast<string>(order->volume-pOrder->VolumeTotalOriginal));
                    order->volume-=pOrder->VolumeTotalOriginal;
                    if(order->volume==0){
                        untrade_it = bidList.erase(untrade_it);
                        DLOG(INFO) << "processOrder,delete:volume=0,delete this info";
                    }
                    break;
                }
            }

        }else if(type=="upEx"){//update exchange info

        }

    }

}

/*执行平仓操作，需要查询持仓情况。做出平今平昨决定。只针对上期所品种.
参数：instrumentID 要查询对手的合约
type 持仓类型.执行卖平仓操作时,输入buy,表示要查询多头的持仓情况.执行买平仓操作时,输入sell，表示要查询空头的持仓情况.
返回值:开仓 '0';平仓 '1';平今 '3';平昨 '4';强平 '2'*/
//string DataInitInstance::getCloseMethod(string investorID,string instrumentID, string type) {
//    //boost::recursive_mutex::scoped_lock SLock(pst_mtx);
//    UserAccountInfo* uai;
//    unordered_map<string, UserAccountInfo*>::iterator fnamIT = followNBAccountMap.find(investorID);
//    if(fnamIT==followNBAccountMap.end()){
//        string msg="getCloseMethod:investorID="+investorID+",can't find UserAccountInfo in followNBAccountMap.";
//        LOG(ERROR)<<msg;
//        return "-1";
//    }else{
//        uai=(UserAccountInfo*)fnamIT->second;
//    }
//    unordered_map<string, HoldPositionInfo*> positionmap=uai->positionmap;
//    unordered_map<string, HoldPositionInfo*>::iterator map_iterator = positionmap.find(instrumentID);
//    if (map_iterator != positionmap.end()) {//查找到合约持仓
//        HoldPositionInfo* hold = map_iterator->second;
//        string tmpmsg;
//        int realShortPstLimit = hold->shortTotalPosition;
//        int realLongPstLimit = hold->longTotalPosition;
//        int avlpstLong = hold->longAvaClosePosition;
//        int avlpstShort = hold->shortAvaClosePosition;
//        int shortYdPst = hold->shortYdPosition;
//        int longYdPst = hold->longYdPosition;
//        if (type == "buy") {
//            //calculate
//            int untradeYdVolume = 0;
//            int untradeVolume = 0;
//            for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();it++){
//                OrderInfo* orderInfo = *it;
//                if(orderInfo->direction == "1" && orderInfo->offsetFlag == "4"){//close yesterday,but not traded now
//                    untradeYdVolume += orderInfo->volume;
//                }
//                if(orderInfo->direction == "1"){
//                    untradeVolume += orderInfo->volume;
//                }
//            }
//            DLOG(INFO) << instrumentID+" untradeVolume in askList is " + boost::lexical_cast<string>(untradeVolume) + ",untradeYdVolume in askList is " + boost::lexical_cast<string>(untradeYdVolume) + ",longYdPst=" + boost::lexical_cast<string>(longYdPst) +
//                          ",avlpstLong=" + boost::lexical_cast<string>(avlpstLong);
//            if (longYdPst - untradeYdVolume > 0) {
//                return "4";//平昨
//            } else if (avlpstLong > 0) {
//            //} else if (realLongPstLimit > 0) {
//                return "3";//平今，上期所品种
//            } else {
//                LOG(ERROR) << "合约无买持仓数据，无法判断平仓方式.instrumentID=" + instrumentID+",change close to open.";
//                return "0";//
//                //boost::this_thread::sleep(boost::posix_time::seconds(3));
//                //Sleep(1000);
//                //return "-1";
//            }
//        } else if (type == "sell") {
//            //calculate
//            int untradeYdVolume = 0;
//            int untradeVolume = 0;
//            for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();it++){
//                OrderInfo* orderInfo = *it;
//                if(orderInfo->direction == "0" && orderInfo->offsetFlag == "4"){//close yesterday,but not traded now
//                    untradeYdVolume += orderInfo->volume;
//                }
//                if(orderInfo->direction == "0"){
//                    untradeVolume += orderInfo->volume;
//                }
//            }
//            DLOG(INFO) << instrumentID+" untradeVolume in bidList is " + boost::lexical_cast<string>(untradeVolume) + ",untradeYdVolume in bidList is " + boost::lexical_cast<string>(untradeYdVolume) + ",shortYdPst=" + boost::lexical_cast<string>(shortYdPst) +
//                          ",avlpstShort=" + boost::lexical_cast<string>(avlpstShort);
//            if (shortYdPst - untradeYdVolume > 0) {
//                return "4";//平昨
//            } else if (avlpstShort > 0) {
//                return "3";//平今，上期所品种
//            } else {
//                LOG(ERROR) << "合约无卖持仓数据，无法判断平仓方式.instrumentID=" + instrumentID + ",change close to open.";
//                return "0";//
//                //boost::this_thread::sleep(boost::posix_time::seconds(3));
//                //Sleep(1000);
//                //return "-1";
//            }
//        }
//    } else {
//        LOG(ERROR) << "查找不到合约的持仓信息:instrumentID=" + instrumentID;
//    }
//}
//void DataInitInstance::processHowManyHoldsCanBeClose(CThostFtdcOrderField *pOrder,string type) {
//    /*********find holdPstIsLocked and positionmap*********/
//    UserAccountInfo* uai;
//    unordered_map<string, UserAccountInfo*>::iterator fnamIT = followNBAccountMap.find(boost::lexical_cast<string>(pOrder->InvestorID));
//    if(fnamIT==followNBAccountMap.end()){
//        string msg="getCloseMethod:investorID="+boost::lexical_cast<string>(pOrder->InvestorID)+",can't find UserAccountInfo in followNBAccountMap.";
//        LOG(ERROR)<<msg;
//        return;
//    }else{
//        uai=(UserAccountInfo*)fnamIT->second;
//    }
//    //unordered_map<string, HoldPositionInfo*> positionmap=uai->positionmap;
//    unordered_map<string, bool> holdPstIsLocked=uai->holdPstIsLocked;
//    string instrumentID = pOrder->InstrumentID;
//    string offsetFlag = string(pOrder->CombOffsetFlag);
//    string lockID = boost::lexical_cast<string>(pOrder->OrderRef) + boost::lexical_cast<string>(pOrder->SessionID) + boost::lexical_cast<string>(pOrder->FrontID);
//    if ("lock" == type) {//锁仓
//        unordered_map<string, bool>::iterator lockIT = holdPstIsLocked.find(lockID);
//        if (lockIT == holdPstIsLocked.end()) {//未锁定
//            holdPstIsLocked[lockID] = true;
//        } else {//已经锁定，不再锁定
//            return;
//        }
//        if (pOrder->Direction == '0' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4")) {//买平仓，锁定空头可平量
//            unordered_map<string, HoldPositionInfo*>::iterator it = uai->positionmap.find(instrumentID);
//            if (it == uai->positionmap.end()) {
//                LOG(ERROR) << "合约" + instrumentID + " 无持仓信息，买平仓锁仓操作错误!!";
//                return;
//            }
//            HoldPositionInfo* holdInfo = it->second;
//            if(isLogout){
//                DLOG(INFO) << "lock," + instrumentID + " can be closed volume from " + boost::lexical_cast<string>(holdInfo->shortAvaClosePosition) + " to " + boost::lexical_cast<string>(holdInfo->shortAvaClosePosition - pOrder->VolumeTotalOriginal);
//            }

//            holdInfo->shortAvaClosePosition = holdInfo->shortAvaClosePosition - pOrder->VolumeTotalOriginal;

//        } else if (pOrder->Direction == '1' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4")) {//卖平仓，锁定多头可平量
//            unordered_map<string, HoldPositionInfo*>::iterator it = uai->positionmap.find(instrumentID);
//            if (it == uai->positionmap.end()) {
//                LOG(ERROR) << "合约" + instrumentID + " 无持仓信息，卖平仓锁仓操作错误!!";
//                return;
//            }
//            HoldPositionInfo* holdInfo = it->second;
//            if(isLogout){
//                DLOG(INFO) << "lock," + instrumentID + " can be closed volume from " + boost::lexical_cast<string>(holdInfo->longAvaClosePosition) + " to " + boost::lexical_cast<string>(holdInfo->longAvaClosePosition - pOrder->VolumeTotalOriginal);
//            }
//            holdInfo->longAvaClosePosition = holdInfo->longAvaClosePosition - pOrder->VolumeTotalOriginal;
//        }else{
//            DLOG(INFO) << "open position,do not need lock.";
//        }
//    } else if ("release" == type) {//释放持仓
//        if (pOrder->Direction == '0' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4")) {//买平仓，释放空头可平量
//            unordered_map<string, HoldPositionInfo*>::iterator it = uai->positionmap.find(instrumentID);
//            if (it == uai->positionmap.end()) {
//                LOG(ERROR) << "合约" + instrumentID + " 无持仓信息，买平仓释放操作错误!!";
//                return;
//            }
//            HoldPositionInfo* holdInfo = it->second;
//            if(isLogout){
//                DLOG(INFO) << "release," + instrumentID + " can be closed volume from " + boost::lexical_cast<string>(holdInfo->shortAvaClosePosition) + " to " + boost::lexical_cast<string>(holdInfo->shortAvaClosePosition + pOrder->VolumeTotalOriginal);
//            }
//            holdInfo->shortAvaClosePosition = holdInfo->shortAvaClosePosition + pOrder->VolumeTotalOriginal;
//        } else if (pOrder->Direction == '1' && (offsetFlag == "1" || offsetFlag == "2" || offsetFlag == "3" || offsetFlag == "4")) {//卖平仓，释放多头可平量
//            unordered_map<string, HoldPositionInfo*>::iterator it = uai->positionmap.find(instrumentID);
//            if (it == uai->positionmap.end()) {
//                LOG(ERROR) << "合约" + instrumentID + " 无持仓信息，卖平仓释放操作错误!!";
//                return;
//            }
//            HoldPositionInfo* holdInfo = it->second;
//            if(isLogout){
//                DLOG(INFO) << "release," + instrumentID + " can be closed volume from " + boost::lexical_cast<string>(holdInfo->longAvaClosePosition) + " to " + boost::lexical_cast<string>(holdInfo->longAvaClosePosition + pOrder->VolumeTotalOriginal);
//            }
//            holdInfo->longAvaClosePosition = holdInfo->longAvaClosePosition + pOrder->VolumeTotalOriginal;
//        }
//    }
//}
//unordered_map<string, HoldPositionInfo*> DataInitInstance::getPositionMap(string investorID){
//    /*********find positionmap*********/
//    UserAccountInfo* uai;
//    unordered_map<string, UserAccountInfo*>::iterator fnamIT = followNBAccountMap.find(investorID);
//    if(fnamIT==followNBAccountMap.end()){
//        string msg="getCloseMethod:investorID="+investorID+",can't find UserAccountInfo in followNBAccountMap.";
//        LOG(ERROR)<<msg;
//        unordered_map<string, HoldPositionInfo*> xx;
//        return xx;
//    }else{
//        uai=(UserAccountInfo*)fnamIT->second;
//    }
//    return uai->positionmap;
//}

//int DataInitInstance::processtrade(TradeInfo *pTrade){
////int DataInitInstance::processtrade(CThostFtdcTradeField *pTrade){
//    /*********find positionmap*********/
//    string investorID=pTrade->investorID;
//    LOG(INFO)<<"processtrade:"<<pTrade->investorID;
//    UserAccountInfo* uai;
//    unordered_map<string, UserAccountInfo*>::iterator fnamIT = followNBAccountMap.find(pTrade->investorID);
//    if(fnamIT==followNBAccountMap.end()){
//        string msg="processtrade:investorID="+pTrade->investorID+",can't find UserAccountInfo in followNBAccountMap.";
//        LOG(ERROR)<<msg;
//        return -1;
//    }else{
//        uai=(UserAccountInfo*)fnamIT->second;
//        //LOG(INFO)<<"processtrade:"<<uai->investorID<<";"<<uai->tradingAccount.closeProfit;
//    }
//    uai->positionmap;
//    ///买卖方向
//    //TThostFtdcDirectionType	direction = pTrade->Direction;
//    //char Direction[] = { direction,'\0' };
//    //sprintf(Direction,"%s",direction);
//    ///开平标志
//    //TThostFtdcOffsetFlagType	offsetFlag = pTrade->OffsetFlag;
//    //char OffsetFlag[] = { offsetFlag,'\0' };
//    ///合约代码
//    //char	*InstrumentID = pTrade->InstrumentID;
//    string str_inst = pTrade->instrumentID;
//    int volume = pTrade->volume;
//    //买卖方向
//    string str_dir = pTrade->direction;
//    //开平方向
//    string str_offset = pTrade->offsetFlag;
//    //成交价格
//    double tradePrice = pTrade->tradePrice;
//    //合约乘数
//    unordered_map<string, InstrumentInfo*>::iterator ins_it = instruments.find(str_inst);
//    if (ins_it == instruments.end()) {
//        LOG(ERROR) << "处理成交信息时,无法查找到合约信息.instrumentID=" + str_inst;
//        return 0;
//    }
//    InstrumentInfo* insinfo = ins_it->second;
//    double multiplyFactor = insinfo->VolumeMultiple;

//    unordered_map<string, HoldPositionInfo*>::iterator map_iterator = uai->positionmap.find(str_inst);
//    //新开仓
//    if (map_iterator == uai->positionmap.end()) {
//        //unordered_map<string, int> tmpmap;
//        HoldPositionInfo* tmpmap = new HoldPositionInfo();
//        if (str_dir == "0") {//买
//                             //多头
//            tmpmap->longTdPosition = volume;
//            tmpmap->longYdPosition = 0;
//            tmpmap->longTotalPosition = volume;
//            tmpmap->longAvaClosePosition = volume;
//            tmpmap->longHoldAvgPrice = tradePrice;
//            tmpmap->longAmount = tradePrice*volume*multiplyFactor;
//            //空头
//            tmpmap->shortTdPosition = 0;
//            tmpmap->shortYdPosition = 0;
//            tmpmap->shortTotalPosition = 0;
//            tmpmap->shortHoldAvgPrice = 0;
//            tmpmap->shortAmount = 0;
//        } else if (str_dir == "1") {//卖
//                                    //空头
//            tmpmap->shortTdPosition = volume;
//            tmpmap->shortYdPosition = 0;
//            tmpmap->shortTotalPosition = volume;
//            tmpmap->shortAvaClosePosition = volume;
//            tmpmap->shortHoldAvgPrice = tradePrice;
//            tmpmap->shortAmount = tradePrice*volume*multiplyFactor;
//            //多头
//            tmpmap->longTdPosition = 0;
//            tmpmap->longYdPosition = 0;
//            tmpmap->longTotalPosition = 0;
//            tmpmap->longHoldAvgPrice = 0;
//            tmpmap->longAmount = 0;
//        }
//        uai->positionmap[str_inst] = tmpmap;
//    } else {
//        ///平仓
//        //        #define USTP_FTDC_OF_Close '1'
//        //        ///强平
//        //        #define USTP_FTDC_OF_ForceClose '2'
//        //        ///平今
//        //        #define USTP_FTDC_OF_CloseToday '3'
//        //        ///平昨
//        //        #define USTP_FTDC_OF_CloseYesterday '4'
//        HoldPositionInfo* tmpinfo = map_iterator->second;
//        if (str_dir == "0") {//买
//            double tmp_shortHoldAvgPrice = tmpinfo->shortHoldAvgPrice;//空头持仓均价
//            int tmp_totalPst = tmpinfo->shortTotalPosition;//原空头持仓量
//            double tmp_totalAmount = tmpinfo->shortAmount;//原空头交易金额
//            if (str_offset == "0") {//买开仓,多头增加
//                tmpinfo->longTdPosition = tmpinfo->longTdPosition + volume;
//                int tmp_tdpst = tmpinfo->longTdPosition;
//                int tmp_ydpst = tmpinfo->longYdPosition;
//                double tmp_longHoldAvgPrice = tmpinfo->longHoldAvgPrice;//多头持仓均价
//                int tmp_totalPst = tmpinfo->longTotalPosition;//原多头持仓量
//                double tmp_totalAmount = tmpinfo->longAmount;//原多头交易金额
//                realLongPstLimit = tmp_tdpst + tmp_ydpst;
//                tmpinfo->longTotalPosition = realLongPstLimit;
//                tmpinfo->longAvaClosePosition = tmpinfo->longAvaClosePosition + volume;
//                tmpinfo->longHoldAvgPrice = (tmp_longHoldAvgPrice*tmp_totalPst + tradePrice*volume) / (realLongPstLimit);//当前多头持仓均价
//                tmpinfo->longAmount = tmp_totalAmount + tradePrice*volume*multiplyFactor;//当前多头交易金额
//            } else if (str_offset == "1") {//买平仓,空头减少
//                int tmp_tdpst = tmpinfo->shortTdPosition;
//                int tmp_ydpst = tmpinfo->shortYdPosition;
//                //int tmp_num = map_iterator->second["shortTotalPosition"];
//                if (tmp_tdpst > 0) {
//                    if (tmp_tdpst <= volume) {
//                        tmp_ydpst = tmp_ydpst - (volume - tmp_tdpst);
//                        tmp_tdpst = 0;
//                    } else {
//                        tmp_tdpst = tmp_tdpst - volume;
//                    }
//                } else if (tmp_tdpst == 0) {
//                    tmp_ydpst = tmp_ydpst - volume;
//                } else {
//                    cout << "tdposition is error!!!" << endl;
//                    LOG(ERROR) << "tdposition is error!!!";
//                }
//                realShortPstLimit = tmp_ydpst + tmp_tdpst;
//                tmpinfo->shortTdPosition = tmp_tdpst;
//                tmpinfo->shortYdPosition = tmp_ydpst;
//                tmpinfo->shortTotalPosition = realShortPstLimit;
//            } else if (str_offset == "3") {//平今
//                int tmp_tdpst = tmpinfo->shortTdPosition;
//                int tmp_ydpst = tmpinfo->shortYdPosition;
//                tmp_tdpst = tmp_tdpst - volume;
//                realShortPstLimit = tmp_ydpst + tmp_tdpst;
//                tmpinfo->shortTdPosition = tmp_tdpst;
//                tmpinfo->shortTotalPosition = realShortPstLimit;
//            } else if (str_offset == "4") {//平昨
//                int tmp_tdpst = tmpinfo->shortTdPosition;
//                int tmp_ydpst = tmpinfo->shortYdPosition;
//                if (tmp_ydpst == 0) {
//                    char c_err[100];
//                    sprintf(c_err, "shortYdPosition is zero!!!,please check this rtn trade.");
//                    cout << c_err << endl;
//                    LOG(ERROR) << c_err;
//                }
//                tmp_ydpst = tmp_ydpst - volume;
//                realShortPstLimit = tmp_ydpst + tmp_tdpst;
//                tmpinfo->shortYdPosition = tmp_ydpst;
//                tmpinfo->shortTotalPosition = realShortPstLimit;
//            }
//            //买平仓处理空头平均价格;买开仓不需要单独处理
//            if (str_offset != "0") {
//                tmpinfo->shortHoldAvgPrice = (tmp_shortHoldAvgPrice*tmp_totalPst - tradePrice*volume) / (realShortPstLimit);//当前空头持仓均价
//                tmpinfo->shortAmount = tmp_totalAmount - tradePrice*volume*multiplyFactor;//当前空头交易金额
//                tmpinfo->shortAvaClosePosition = tmpinfo->shortAvaClosePosition - volume;//当前空头可平量
//            }
//        } else if (str_dir == "1") {//卖
//            double tmp_longHoldAvgPrice = tmpinfo->longHoldAvgPrice;//原多头持仓均价
//            int tmp_totalPst = tmpinfo->longTotalPosition;//原多头持仓量
//            double tmp_totalAmount = tmpinfo->longAmount;//原多头交易金额
//            if (str_offset == "0") {//卖开仓,空头增加
//                tmpinfo->shortTdPosition = tmpinfo->shortTdPosition + volume;
//                double tmp_shortHoldAvgPrice = tmpinfo->shortHoldAvgPrice;//原空头持仓均价
//                int tmp_totalPst = tmpinfo->shortTotalPosition;//原空头持仓量
//                double tmp_totalAmount = tmpinfo->shortAmount;//原空头交易金额
//                int tmp_tdpst = tmpinfo->shortTdPosition;
//                int tmp_ydpst = tmpinfo->shortYdPosition;
//                realShortPstLimit = tmp_tdpst + tmp_ydpst;
//                tmpinfo->shortTotalPosition = realShortPstLimit;
//                tmpinfo->shortAvaClosePosition = tmpinfo->shortAvaClosePosition + volume;
//                tmpinfo->shortHoldAvgPrice = (tmp_shortHoldAvgPrice*tmp_totalPst + tradePrice*volume) / (realShortPstLimit);//当前空头持仓均价
//                tmpinfo->shortAmount = tmp_totalAmount + tradePrice*volume*multiplyFactor;//当前空头交易金额
//            } else if (str_offset == "1") {//卖平仓,多头减少
//                int tmp_tdpst = tmpinfo->longTdPosition;
//                int tmp_ydpst = tmpinfo->longYdPosition;
//                if (tmp_tdpst > 0) {
//                    if (tmp_tdpst <= volume) {
//                        tmp_ydpst = tmp_ydpst - (volume - tmp_tdpst);
//                        tmp_tdpst = 0;
//                    } else {
//                        tmp_tdpst = tmp_tdpst - volume;
//                    }
//                } else if (tmp_tdpst == 0) {
//                    tmp_ydpst = tmp_ydpst - volume;
//                } else {
//                    cout << "tdposition is error!!!" << endl;
//                    LOG(ERROR) << "tdposition is error!!!";
//                }
//                realLongPstLimit = tmp_ydpst + tmp_tdpst;
//                tmpinfo->longTdPosition = tmp_tdpst;
//                tmpinfo->longYdPosition = tmp_ydpst;
//                tmpinfo->longTotalPosition = realLongPstLimit;
//            } else if (str_offset == "3") {//平今
//                int tmp_tdpst = tmpinfo->longTdPosition;
//                int tmp_ydpst = tmpinfo->longYdPosition;
//                tmp_tdpst = tmp_tdpst - volume;
//                realLongPstLimit = tmp_ydpst + tmp_tdpst;
//                tmpinfo->longTdPosition = tmp_tdpst;
//                tmpinfo->longTotalPosition = realLongPstLimit;
//            } else if (str_offset == "4") {//平昨
//                int tmp_tdpst = tmpinfo->longTdPosition;
//                int tmp_ydpst = tmpinfo->longYdPosition;
//                if (tmp_ydpst == 0) {
//                    char c_err[100];
//                    sprintf(c_err, "longYdPosition is zero!!!,please check this rtn trade.");
//                    cout << c_err << endl;
//                    LOG(ERROR) << c_err;
//                }
//                tmp_ydpst = tmp_ydpst - volume;
//                realLongPstLimit = tmp_ydpst + tmp_tdpst;
//                tmpinfo->longYdPosition = tmp_ydpst;
//                tmpinfo->longTotalPosition = realLongPstLimit;
//            }
//            //卖平仓处理多头平均价格;卖开仓不需要单独处理
//            if (str_offset != "0") {
//                tmpinfo->longHoldAvgPrice = (tmp_longHoldAvgPrice*tmp_totalPst - tradePrice*volume) / (realLongPstLimit);//当前多头持仓均价
//                tmpinfo->longAmount = tmp_totalAmount - tradePrice*volume*multiplyFactor;//当前多头交易金额
//                tmpinfo->longAvaClosePosition = tmpinfo->longAvaClosePosition - volume;
//            }
//        }
//    }
//    //processAverageGapGprice(pTrade);
//    //tradeParaProcessTwo();
//    string tmpmsg=investorID+";";
//    for (unordered_map<string, HoldPositionInfo*>::iterator it = uai->positionmap.begin(); it != uai->positionmap.end(); it++) {
//        HoldPositionInfo* tmpinfo = it->second;
//        //tmpmsg.append(it->first).append("持仓情况:");
//        tmpmsg.append(it->first).append("hold position:");
//        char char_tmp_pst[10] = { '\0' };
//        char char_longyd_pst[10] = { '\0' };
//        char char_longtd_pst[10] = { '\0' };
//        sprintf(char_tmp_pst, "%d", tmpinfo->longTotalPosition);
//        sprintf(char_longyd_pst, "%d", tmpinfo->longYdPosition);
//        sprintf(char_longtd_pst, "%d", tmpinfo->longTdPosition);
//        char str_avgBuy[25], str_avgSell[25];
//        int sig = 2;
//        gcvt(tmpinfo->longHoldAvgPrice, sig, str_avgBuy);
//        tmpmsg.append("longposition=");
//        tmpmsg.append(char_tmp_pst);
//        tmpmsg.append(";todayposition=");
//        tmpmsg.append(char_longtd_pst);
//        tmpmsg.append(";yesposition=");
//        tmpmsg.append(char_longyd_pst);
//        tmpmsg.append(";longAvaClosePosition=" + boost::lexical_cast<string>(tmpinfo->longAvaClosePosition));
//        tmpmsg.append(";longHoldAvgPrice=");
//        tmpmsg.append(boost::lexical_cast<string>(tmpinfo->longHoldAvgPrice));
//        tmpmsg.append(";longAmount=" + boost::lexical_cast<string>(tmpinfo->longAmount));
//        char char_tmp_pst2[10] = { '\0' };
//        char char_shortyd_pst[10] = { '\0' };
//        char char_shorttd_pst[10] = { '\0' };
//        gcvt(tmpinfo->shortHoldAvgPrice, sig, str_avgSell);
//        sprintf(char_tmp_pst2, "%d", tmpinfo->shortTotalPosition);
//        sprintf(char_shortyd_pst, "%d", tmpinfo->shortYdPosition);
//        sprintf(char_shorttd_pst, "%d", tmpinfo->shortTdPosition);
//        tmpmsg.append("shortposition=");
//        tmpmsg.append(char_tmp_pst2);
//        tmpmsg.append(";yesposition=");
//        tmpmsg.append(char_shorttd_pst);
//        tmpmsg.append(";yesposition=");
//        tmpmsg.append(char_shortyd_pst);
//        tmpmsg.append(";shortAvaClosePosition=" + boost::lexical_cast<string>(tmpinfo->shortAvaClosePosition));
//        tmpmsg.append(";shortHoldAvgPrice=");
//        tmpmsg.append(boost::lexical_cast<string>(tmpinfo->shortHoldAvgPrice));
//        tmpmsg.append(";shortAmount=" + boost::lexical_cast<string>(tmpinfo->shortAmount) + ";#");
//    }
//    cout << tmpmsg << endl;

//    return 0;
//}
bool DataInitInstance::isAllLoginOK(){
    for(unordered_map<string, CTPInterface*>::iterator it = tradeApiMap.begin();it!=tradeApiMap.end();it++){
        //cout<<it->first<<";ok="<<it->second<<endl;
        CTPInterface*cit=it->second;
        if(!cit->loginOK){//if any user is not ok,return false
            return false;
        }
    }

    return true;
}
//update order flags
//type:"ctp","exg"
void DataInitInstance::updateOriOrder(CThostFtdcOrderField *pOrder,string type){
    int sessionID=pOrder->SessionID;
    int frontID=pOrder->FrontID;
    string direction=boost::lexical_cast<string>(pOrder->Direction);
    string orderRef=boost::lexical_cast<string>(pOrder->OrderRef);
    if("ctp"==type){
        if(direction=="0"){//buy
            for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();it++){
                OrderInfo* orderInfo = *it;
                if(orderInfo->sessionID==sessionID &&orderInfo->frontID==frontID &&  orderInfo->orderRef == orderRef){
                    orderInfo->brokerOrderSeq=pOrder->BrokerOrderSeq;
                    orderInfo->orderStatus=boost::lexical_cast<string>(pOrder->OrderStatus);
                    orderInfo->orderLocalID=boost::lexical_cast<string>(pOrder->OrderLocalID);
                    LOG(INFO) << "updateOriOrder ctp,size="+boost::lexical_cast<string>(bidList.size())+";find buy order insert,and update:" + getOrderInfo(orderInfo);
                    break;
                }
            }
        }else if(direction=="1"){//sell
            for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();it++){
                OrderInfo* orderInfo = *it;
                if(orderInfo->sessionID==sessionID &&orderInfo->frontID==frontID &&  orderInfo->orderRef == orderRef){
                    orderInfo->brokerOrderSeq=pOrder->BrokerOrderSeq;
                    orderInfo->orderStatus=boost::lexical_cast<string>(pOrder->OrderStatus);
                    orderInfo->orderLocalID=boost::lexical_cast<string>(pOrder->OrderLocalID);
                    LOG(INFO) << "updateOriOrder ctp,size="+boost::lexical_cast<string>(askList.size())+";find sell order insert,and update:" + getOrderInfo(orderInfo);
                    break;
                }
            }
        }else{
            LOG(ERROR) << "ERROR:can not find order insert.";
        }
    }else if("exg"==type){
        if(direction=="0"){//buy
            for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();it++){
                OrderInfo* orderInfo = *it;
                if(orderInfo->sessionID==sessionID &&orderInfo->frontID==frontID &&  orderInfo->orderRef == orderRef){
                    orderInfo->orderSysID=pOrder->OrderSysID;
                    orderInfo->orderStatus=boost::lexical_cast<string>(pOrder->OrderStatus);
                    //orderInfo->orderLocalID=boost::lexical_cast<string>(pOrder->OrderLocalID);
                    LOG(INFO) << "updateOriOrder exg,size="+boost::lexical_cast<string>(bidList.size())+";find buy order insert,and update:" + getOrderInfo(orderInfo);
                    break;
                }
            }
        }else if(direction=="1"){//sell
            for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();it++){
                OrderInfo* orderInfo = *it;
                if(orderInfo->sessionID==sessionID &&orderInfo->frontID==frontID &&  orderInfo->orderRef == orderRef){
                    orderInfo->orderSysID=pOrder->OrderSysID;
                    orderInfo->orderStatus=boost::lexical_cast<string>(pOrder->OrderStatus);
                    LOG(INFO) << "updateOriOrder exg,size="+boost::lexical_cast<string>(askList.size())+";find sell order insert,and update:" + getOrderInfo(orderInfo);
                    break;
                }
            }
        }else{
            LOG(ERROR) << "ERROR:can not find order insert.";
        }
    }

}
bool DataInitInstance::orderIsResbonsed(string investorID){
    for(list<OrderInfo*>::iterator lit= bidList.begin();lit!=bidList.end();lit++){//order list
        OrderInfo* oinfo = *lit;
        if(oinfo->investorID==investorID &&oinfo->orderStatus=="-1"){//not receive order response
            LOG(INFO)<<"orderIsResbonsed:in bidList,there is order not received response.";
            return false;
        }
    }
    for(list<OrderInfo*>::iterator lit= askList.begin();lit!=askList.end();lit++){//order list
        OrderInfo* oinfo = *lit;
        if(oinfo->investorID==investorID &&oinfo->orderStatus=="-1"){//not receive order response
            LOG(INFO)<<"orderIsResbonsed:in askList,there is order not received response.";
            return false;
        }
    }
    if(bidList.size()==0&&askList.size()==0){
        LOG(INFO)<<"orderIsResbonsed:in bidList and askList,all is 0.";

    }
    return true;
}
//int DataInitInstance::getFollowTick(string investorID){
//    /*********find *********/
//    UserAccountInfo* uai;
//    unordered_map<string, UserAccountInfo*>::iterator fnamIT = followNBAccountMap.find(investorID);
//    if(fnamIT==followNBAccountMap.end()){
//        string msg="getFollowTick:investorID="+investorID+",can't find UserAccountInfo in followNBAccountMap.";
//        LOG(ERROR)<<msg;
//        return 0;
//    }else{
//        uai=(UserAccountInfo*)fnamIT->second;
//    }
//    return uai->followTick;
//}

//void DataInitInstance::processAction(CThostFtdcOrderField *pOrder){
//    string investorID=boost::lexical_cast<string>(pOrder->InvestorID);
//    for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();){
//        OrderInfo* orderInfo = *it;
//        if(orderInfo->investorID==investorID&&orderInfo->orderSysID==boost::lexical_cast<string>(pOrder->OrderSysID)){
//            if(orderInfo->status==FOLLOWFLAG){//is follow order action
//                //2.insert new order
//                double tickMetric=getTickMetric(orderInfo->instrumentID);
//                int followTick=getFollowTick(orderInfo->investorID);
//                UserOrderField* userOrderField = new UserOrderField();
//                userOrderField->direction=orderInfo->direction;
//                userOrderField->offsetFlag=orderInfo->offsetFlag;
//                userOrderField->orderInsertPrice=orderInfo->price+tickMetric*followTick;
//                string msg="processAction:investorID="+orderInfo->investorID+"'s followTick="+boost::lexical_cast<string>(followTick)+
//                        ",before price="+boost::lexical_cast<string>(orderInfo->price)+",after price="+boost::lexical_cast<string>(userOrderField->orderInsertPrice)+
//                        ",current follow count="+boost::lexical_cast<string>(orderInfo->followCount)+",multifactor="+boost::lexical_cast<string>(tickMetric)+
//                        ",instrumentID="+orderInfo->instrumentID;
//                LOG(INFO)<<msg;
//                userOrderField->brokerID=orderInfo->brokerID;
//                userOrderField->frontID=orderInfo->frontID;
//                userOrderField->sessionID=orderInfo->sessionID;
//                userOrderField->hedgeFlag=orderInfo->hedgeFlag;
//                userOrderField->instrumentID=orderInfo->instrumentID;
//                userOrderField->investorID=orderInfo->investorID;
//                strcpy(userOrderField->orderPriceType,orderInfo->orderPriceType.c_str());
//                userOrderField->orderRef=iRequestID++;
//                userOrderField->volume=orderInfo->volume;
//                userOrderField->followCount=orderInfo->followCount;
//                //addNewOrderInsert(userOrderField);
//            }
//            //1.delete original order
//            it=bidList.erase(it);
//            return;
//        }else{
//            it++;
//        }
//    }
//    for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();){
//        OrderInfo* orderInfo = *it;
//        if(orderInfo->investorID==investorID&&orderInfo->orderSysID==boost::lexical_cast<string>(pOrder->OrderSysID)){
//            if(orderInfo->status==FOLLOWFLAG){//is follow order action
//                //2.insert new order
//                double tickMetric=getTickMetric(orderInfo->instrumentID);
//                int followTick=getFollowTick(orderInfo->investorID);
//                UserOrderField* userOrderField = new UserOrderField();
//                userOrderField->direction=orderInfo->direction;
//                userOrderField->offsetFlag=orderInfo->offsetFlag;
//                userOrderField->orderInsertPrice=orderInfo->price-tickMetric*followTick;
//                string msg="processAction:investorID="+orderInfo->investorID+"'s followTick="+boost::lexical_cast<string>(followTick)+
//                        ",before price="+boost::lexical_cast<string>(orderInfo->price)+",after price="+boost::lexical_cast<string>(userOrderField->orderInsertPrice)+
//                        ",current follow count="+boost::lexical_cast<string>(orderInfo->followCount)+",multifactor="+boost::lexical_cast<string>(tickMetric)+
//                        ",instrumentID="+orderInfo->instrumentID;
//                LOG(INFO)<<msg;
//                userOrderField->brokerID=orderInfo->brokerID;
//                userOrderField->frontID=orderInfo->frontID;
//                userOrderField->sessionID=orderInfo->sessionID;
//                userOrderField->hedgeFlag=orderInfo->hedgeFlag;
//                userOrderField->instrumentID=orderInfo->instrumentID;
//                userOrderField->investorID=orderInfo->investorID;
//                strcpy(userOrderField->orderPriceType,orderInfo->orderPriceType.c_str());
//                userOrderField->orderRef=iRequestID++;
//                userOrderField->volume=orderInfo->volume;
//                userOrderField->followCount=orderInfo->followCount;
//                //addNewOrderInsert(userOrderField);
//            }
//            //1.delete original order
//            it=askList.erase(it);
//            return;
//        }else{
//            it++;
//        }

//    }
//}

void DataInitInstance::deleteOriOrder(string orderSysID){
    //string direction = cancledOrderInfo->Direction;
    DLOG(INFO) << "delete ori orderinfo:before cancle order,bidlist size=" + boost::lexical_cast<string>(bidList.size());
    for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();){
        OrderInfo* orderInfo = *it;
        if(orderInfo->orderSysID==orderSysID){
            LOG(INFO) << "deleteOriOrder;find order insert ,and delete:" + getOrderInfo(orderInfo);
            it = bidList.erase(it);
            break;
        }else{
            it++;
        }
    }
    DLOG(INFO) << "after cancle order,bidlist size=" + boost::lexical_cast<string>(bidList.size());

    DLOG(INFO) << "delete ori orderinfo:before cancle order,asklist size=" + boost::lexical_cast<string>(askList.size());
    for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();){
        OrderInfo* orderInfo = *it;
        if(orderInfo->orderSysID==orderSysID){
            LOG(INFO) << "deleteOriOrder;find order insert ,and delete:" + getOrderInfo(orderInfo);
            it = askList.erase(it);
            break;
        }else{
            it++;
        }
    }
    DLOG(INFO) << "after cancle order,askList size=" + boost::lexical_cast<string>(askList.size());
}

//only stop profit order action will be process,other order action will be deleted directly
void DataInitInstance::deleteOriOrder(int frontID,int sessionID,string orderRef){
    //string direction = cancledOrderInfo->Direction;
    DLOG(INFO) << "delete ori orderinfo:before cancle order,bidlist size=" + boost::lexical_cast<string>(bidList.size());
    for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();){
        OrderInfo* orderInfo = *it;
        if(orderInfo->sessionID==sessionID &&orderInfo->frontID==frontID &&  orderInfo->orderRef == orderRef){
            LOG(INFO) << "deleteOriOrder;find order insert ,and delete:" + getOrderInfo(orderInfo);
            it = bidList.erase(it);
            break;
        }else{
            it++;
        }
    }
    DLOG(INFO) << "after cancle order,bidlist size=" + boost::lexical_cast<string>(bidList.size());

    DLOG(INFO) << "delete ori orderinfo:before cancle order,asklist size=" + boost::lexical_cast<string>(askList.size());
    for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();){
        OrderInfo* orderInfo = *it;
        if(orderInfo->sessionID==sessionID &&orderInfo->frontID==frontID &&  orderInfo->orderRef == orderRef){
            LOG(INFO) << "deleteOriOrder;find order insert ,and delete:" + getOrderInfo(orderInfo);
            it = askList.erase(it);
            break;
        }else{
            it++;
        }
    }
    DLOG(INFO) << "after cancle order,askList size=" + boost::lexical_cast<string>(askList.size());
}
void DataInitInstance::setLoginOk(string investorID){
    unordered_map<string, CTPInterface*>::iterator it = tradeApiMap.find(investorID);
    if(it != tradeApiMap.end()){
        string msg="setLoginOk:set login flag   to true.";
        LOG(INFO)<<msg;
        LOG(ERROR) <<msg<<endl;
        CTPInterface *ci=it->second;
        ci->loginOK=true;
    }else{
        string msg="setLoginOk:to true.";
        LOG(INFO)<<msg;
        LOG(ERROR) <<msg<<endl;

        //loginOK[investorID]=true;
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
string DataInitInstance::processRspReqInstrument(CThostFtdcInstrumentField *pInstrument) {
    InstrumentInfo* info = new InstrumentInfo();
    strcpy(info->ExchangeID, pInstrument->ExchangeID);
    info->PriceTick = pInstrument->PriceTick;
    info->instrumentID = string(pInstrument->InstrumentID);
    info->VolumeMultiple = pInstrument->VolumeMultiple;
    instruments[string(pInstrument->InstrumentID)] = info;
    LOG(INFO) << "查询到合约信息:" + info->instrumentID + ";priceTick=" + boost::lexical_cast<string>(info->PriceTick) + ";volumeMultiple=" + boost::lexical_cast<string>(info->VolumeMultiple);
    return "";
}
string DataInitInstance::getOrderInfo(OrderInfo* info){
    string msg = "";
    msg += "brokerID=" + info->brokerID + ";";
    msg += "investorID=" + info->investorID + ";";
    msg += "frontID=" + boost::lexical_cast<string>(info->frontID) + ";";
    msg += "sessionID=" + boost::lexical_cast<string>(info->sessionID) + ";";
    msg += "orderRef=" + boost::lexical_cast<string>(info->orderRef) + ";";
    msg += "direction=" + info->direction + ";";
    msg += "offsetFlag=" + info->offsetFlag + ";";
    msg += "orderStatus=" + info->orderStatus + ";";
    msg += "status=" + info->status + ";";
    msg += "orderPriceType=" + info->orderPriceType + ";";
    msg += "followCout=" + boost::lexical_cast<string>(info->followCount) + ";";
    //msg += "clientOrderToken=" + boost::lexical_cast<string>(info->clientOrderToken) + ";";
    msg += "instrumentID=" + info->instrumentID + ";";
    msg += "function=" + info->function + ";";
    msg += "orderLocalID=" + info->orderLocalID + ";";
    msg += "orderSysID=" + info->orderSysID + ";";
    msg += "mkType=" + info->mkType + ";";
    msg += "orderType=" + info->orderType + ";";
    msg += "price=" + boost::lexical_cast<string>(info->price) + ";";
    msg += "volume=" + boost::lexical_cast<string>(info->volume) + ";";
    return msg;
}
void DataInitInstance::startStrategy(){
    int isbegin = 0;
    cout << "是否启动策略程序?0 否，1是" << endl;
    cin >> isbegin;
    if (isbegin == 1) {
        start_process = 1;
    }
}
#endif
