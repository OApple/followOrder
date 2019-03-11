//#pragma once
#ifndef MDSPI_H
#define MDSPI_H
#include <string>
#include <unordered_map>
#include <ThostFtdcMdApi.h>
#include "dataprocessor.h"
//#include "instrument.h"

using namespace std;

class CMdSpi : public CThostFtdcMdSpi
{
public:

    CMdSpi(DataInitInstance&di, const string &config);
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected();

    ///登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


    ///错误应答
    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast);


    virtual void OnFrontDisconnected(int nReason);


    virtual void OnHeartBeatWarning(int nTimeLapse);


    ///订阅行情应答
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///取消订阅行情应答
    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///深度行情通知
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

//    void SubscribeMarketData(vector<string>ovinstrument);
  void Subscribe(const string &instrument);
  static CMdSpi*mdspi;
  //    int SubscribeMarketData(Instrument &instrument);
  static CMdSpi *getInstance()
  {
      return mdspi;
  }
  bool getStatus() const;
  void setStatus(bool value);

  void UnSubscribe(const string &instrument);
private:
  void ReqUserLogin();

  //
  bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

    string investor_id;
    string password="";
    string _brokerID;
    int max_instrument;
    int instrument_start;
//    vector<Instrument*>vinstrument;
//    unordered_map<string,Instrument*> instrument_map;
    string _market_front_addr="0";
    CThostFtdcMdApi* pUserApi ;
   bool status=false;

    DataInitInstance &dii;
};
#endif
