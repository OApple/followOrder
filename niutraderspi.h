#ifndef NIUTRADER_H
#define NIUTRADER_H
#include <list>
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <ThostFtdcTraderApi.h>
#include "property.h"
#include "dataprocessor.h"
#include "traderspi.h"
using namespace std;
class NiuTraderSpi:public CThostFtdcTraderSpi
{
public:
    NiuTraderSpi();
    ~ NiuTraderSpi();
   NiuTraderSpi(DataInitInstance&di, string &config);

    NiuTraderSpi(DataInitInstance&di,string  investorID,string passWord);

    NiuTraderSpi( DataInitInstance&di,  bool loginOK,CThostFtdcTraderApi* pUserApi);

    bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

    virtual void OnFrontConnected();

    ///登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询结算信息确认响应
    virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询投资者结算结果响应
    virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///投资者结算结果确认响应
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    //    ///请求查询合约响应
    //    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询资金账户响应
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询投资者持仓响应
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///报单通知
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

    ///成交通知
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

    void startApi();

    void stopApi();
    //    int ReqOrderInsert(UserOrderField* userOrderField);


    string getPassword() const;
    void setPassword(const string &value);

    string getInvestorID() const;
    void setInvestorID(const string &investorID);


    unordered_map<string, CTraderSpi *> &getSlave();
    void setSlave(unordered_map<string, CTraderSpi *> slaves);

    void addUser(string &config);
    void delUser(string investorid);
    void setSlaveConfig(const string &slave, const string &config);
    unordered_map<string, string>getSlaveConfig();
    int total_trade_num() const;
    void setTotal_trade_num(int total_trade_num);

    int profit_num() const;
    void setProfit_num(int profit_num);

    int close_num() const;
    void setClose_num(int close_num);

    double profit() const;
    void setProfit(double profit);

    double loss() const;
    void setLoss(double loss);

    int loss_num() const;
    void setLoss_num(int loss_num);


    void setFollows(const unordered_map<string, CTraderSpi *> &follows);


    CThostFtdcTraderSpi* pUserSpi ;

    CThostFtdcTraderApi* _pUserApi;

protected:


private:
    string _trade_front_addr;
    int _requestID=1;
    bool _all_follow_ok=true;
    bool _positon_req_send=false;
    bool _loginOK;
    std::mutex mtx;
    unordered_map<string, CTraderSpi*>_slaves;
//    vector<CTraderSpi *> _follow;

    string _brokerID;
    string  _investorID="";
    string _password="";

    int _frontID;
    int _sessionID;
    int _orderRef;
    string _settlemsg;
    DataInitInstance &dii;
    SQLite::Database *sqlite_handle;
    int _total_trade_num;

    int _profit_num;
    int _close_num;

    double _profit;
    double _loss;

    int _loss_num;

    ///用户登录请求
    void ReqUserLogin();

    ///请求查询结算信息确认
    int ReqQrySettlementInfoConfirm();

    ///投资者结算结果确认
    void ReqQrySettlementInfo();

    void ReqSettlementInfoConfirm();

    void ReqQryTradingAccount();

    void ReqQryInvestorPosition();

    bool IsMyOrder(CThostFtdcOrderField *pOrder);
    void SaveTransactionRecord();
    HoldPositionInfo* initpst(CThostFtdcInvestorPositionField *pInvestorPosition);
};

#endif // NIUTRADER_H
