#pragma once
#include <list>
#include <string>
//#include <SQLiteCpp/SQLiteCpp.h>
//#include <SQLiteCpp/VariadicBind.h>
#include <ThostFtdcTraderApi.h>

#include "property.h"
#include "dataprocessor.h"

using namespace std;
class  UserOrderField;
class CTraderSpi : public CThostFtdcTraderSpi
{
public:

    CTraderSpi( DataInitInstance&di,  bool loginOK,CThostFtdcTraderApi* pUserApi);

    ~CTraderSpi();
    //126373/123456/1:1/1/1
    CTraderSpi(DataInitInstance&di, string &config);

    CTraderSpi();

    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected();

    ///登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询结算信息确认响应
    virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询投资者结算结果响应
    virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///投资者结算结果确认响应
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询合约响应
    //    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询资金账户响应
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///请求查询投资者持仓响应
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///报单录入请求
    int ReqOrderInsert(UserOrderField* userOrderField);
    ///报单录入请求响应
    virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///报单录入错误回报
    virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);






    ///报单操作请求
    int ReqOrderAction(UserOrderField* orderInfo);
    ///报单操作请求响应
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///报单操作错误回报
    virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);


    ///错误应答
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    virtual void OnFrontDisconnected(int nReason);

    ///心跳超时警告。当长时间未收到报文时，该方法被调用。
    virtual void OnHeartBeatWarning(int nTimeLapse);

    virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder,CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);

    ///报单通知
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

    ///成交通知
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);



    bool GetLoginOK();
    void SetLoginOK();


    ///请求查询合约
    //    void ReqQryInstrument();

    //    void update_followusers( unordered_map<string, UserAccountInfo*> followusers);

    void  startApi();
    void stopApi();



    ///请求查询资金账户
    void ReqQryTradingAccount();
    ///请求查询投资者持仓
    void ReqQryInvestorPosition();

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

    string investorID() const;
    void setInvestorID(const string &investorID);

    string config() const;
    void setConfig(const string &config);

    vector<string> getParameter();

    ///投资者结算结果确认
    void ReqQrySettlementInfo();

    void ReqSettlementInfoConfirm();

    int sessionID() const;
    void setSessionID(int sessionID);

    int orderRefInc() ;
    void setOrderRef(int orderRef);

    int frontID() const;
    void setFrontID(int frontID);

    string brokerID() const;
    void setBrokerID(const string &brokerID);


    unordered_map<string, HoldPositionInfo*> positionmap;
private:

    std::mutex mtx;
    string _ratio;
    string _followTick="1";
    string _priceType;

    string _settlemsg;
    //    boost::recursive_mutex _spi_mtx;
    bool _loginOK;
    CThostFtdcTraderApi* _pUserApi;
    DataInitInstance &dii;

    bool _positon_req_send=false;
//    SQLite::Database *sqlite_handle;
    string  _investorID="";
    string _password="";
    string _trade_front_addr="0";
    string _brokerID;

    int _frontID;
    int _sessionID;
    int _orderRef;

    int _total_trade_num;

    int _profit_num;
    int _close_num;

    double _profit;
    double _loss;

    int _loss_num;

    string _config;
    int _requestID=1;
    ///用户登录请求
    void ReqUserLogin();

    ///请求查询结算信息确认
    int ReqQrySettlementInfoConfirm();







    int ReqQryOrder();

    // 是否收到成功的响应
    bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
    // 是否我的报单回报
    bool IsMyOrder(CThostFtdcOrderField *pOrder);
    // 是否正在交易的报单
    bool IsTradingOrder(CThostFtdcOrderField *pOrder);
    void processHowManyHoldsCanBeClose(CThostFtdcOrderField *pOrder,string type);
    void processtrade(CThostFtdcOrderField *pOrder);
    //初始化持仓信息
    HoldPositionInfo* initpst(CThostFtdcInvestorPositionField *pInvestorPosition);
    void SaveTransactionRecord();

};
