#ifndef CSHFETRADERSPI_H
#define CSHFETRADERSPI_H
#pragma once
#include "cshfe/FtdcTraderApi.h"
#include <list>
#include <string>
using namespace std;
class CSHFETraderSpi : public CShfeFtdcTraderSpi{
public:
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected();

    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    ///@param nReason 错误原因
    ///        0x1001 网络读失败
    ///        0x1002 网络写失败
    ///        0x2001 接收心跳超时
    ///        0x2002 发送心跳失败
    ///        0x2003 收到错误报文
    virtual void OnFrontDisconnected(int nReason);

    ///心跳超时警告。当长时间未收到报文时，该方法被调用。
    ///@param nTimeLapse 距离上次接收报文的时间
    virtual void OnHeartBeatWarning(int nTimeLapse);

    ///报文回调开始通知。当API收到一个报文后，首先调用本方法，然后是各数据域的回调，最后是报文回调结束通知。
    ///@param nTopicID 主题代码（如私有流、公共流、行情流等）
    ///@param nSequenceNo 报文序号
    virtual void OnPackageStart(int nTopicID, int nSequenceNo);

    ///报文回调结束通知。当API收到一个报文后，首先调用报文回调开始通知，然后是各数据域的回调，最后调用本方法。
    ///@param nTopicID 主题代码（如私有流、公共流、行情流等）
    ///@param nSequenceNo 报文序号
    virtual void OnPackageEnd(int nTopicID, int nSequenceNo);

    ///错误应答
    virtual void OnRspError(CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///用户登录应答
    virtual void OnRspUserLogin(CShfeFtdcRspUserLoginField *pRspUserLogin, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///用户退出应答
    virtual void OnRspUserLogout(CShfeFtdcRspUserLogoutField *pRspUserLogout, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///报单录入应答
    virtual void OnRspOrderInsert(CShfeFtdcInputOrderField *pInputOrder, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

    ///报单操作应答
    virtual void OnRspOrderAction(CShfeFtdcOrderActionField *pOrderAction, CShfeFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///成交回报
    virtual void OnRtnTrade(CShfeFtdcTradeField *pTrade);

    ///报单回报
    virtual void OnRtnOrder(CShfeFtdcOrderField *pOrder);

    //委托有错误时，才会有该报文；否则 pRspInfo本身就是空指针。
    bool IsErrorRspInfo(CShfeFtdcRspInfoField *pRspInfo);
};

#endif // CSHFETRADERSPI_H
