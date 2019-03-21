#include "util.h"
//extern DataInitInstance dii;


string strDepthMarketData(CThostFtdcDepthMarketDataField *pmd)
{

    string tmp;
    if(pmd==NULL)
        return ( tmp);
    string invesinfo=
            "交易日 TradingDay="+lexical_cast<string>(pmd->TradingDay)+
            "  合约代码 InstrumentID="+pmd->InstrumentID+
            "  交易所代码 ExchangeID="+pmd->ExchangeID+
            "  合约在交易所的代码 ExchangeInstID="+pmd->ExchangeInstID+
            "  最新价 LastPrice="+lexical_cast<string>(pmd->LastPrice)+
            "  上次结算价 PreSettlementPrice="+lexical_cast<string>(pmd->PreSettlementPrice)+
            "  昨收盘 PreClosePrice="+lexical_cast<string>(pmd->PreClosePrice)+
            "  昨持仓量 PreOpenInterest="+lexical_cast<string>(pmd->PreOpenInterest)+
            "  今开盘 OpenPrice="+lexical_cast<string>(pmd->OpenPrice)+
            "  最高价 HighestPrice="+lexical_cast<string>(pmd->HighestPrice)+
            "  最低价 LowestPrice="+lexical_cast<string>(pmd->LowestPrice)+

            "  数量 Volume="+lexical_cast<string>(pmd->Volume)+
            "  成交金额 Turnover="+lexical_cast<string>(pmd->Turnover)+
            "  持仓量 OpenInterest="+lexical_cast<string>(pmd->OpenInterest)+
            "  今收盘 ClosePrice="+lexical_cast<string>(pmd->ClosePrice)+
            "  本次结算价 SettlementPrice="+lexical_cast<string>(pmd->SettlementPrice)+
            "  涨停板价 UpperLimitPrice="+lexical_cast<string>(pmd->UpperLimitPrice)+
            "  跌停板价 LowerLimitPrice="+lexical_cast<string>(pmd->LowerLimitPrice)+
            "  昨虚实度 PreDelta="+lexical_cast<string>(pmd->PreDelta)+
            "  今虚实度 CurrDelta="+lexical_cast<string>(pmd->CurrDelta)+
            "  最后修改时间 UpdateTime="+lexical_cast<string>(pmd->UpdateTime)+
            "  最后修改毫秒 UpdateMillisec="+lexical_cast<string>(pmd->UpdateMillisec)+
            "  申买价一 BidPrice1="+lexical_cast<string>(pmd->BidPrice1)+
            "  申买量一 BidVolume1="+lexical_cast<string>(pmd->BidVolume1)+
            "  申卖价一 AskPrice1="+lexical_cast<string>(pmd->AskPrice1)+
            "  申卖量一 AskVolume1="+lexical_cast<string>(pmd->AskVolume1)+
            "  申买价二 BidPrice2="+lexical_cast<string>(pmd->BidPrice2)+
            "  申买量二 BidVolume2="+lexical_cast<string>(pmd->BidVolume2)+
            "  申卖价二 AskPrice2="+lexical_cast<string>(pmd->AskPrice2)+

            "  申卖量二 AskVolume2="+lexical_cast<string>(pmd->AskVolume2)+
            "  申买价三 BidPrice3="+lexical_cast<string>(pmd->BidPrice3)+
            "  申买量三 BidVolume3="+lexical_cast<string>(pmd->BidVolume3)+
            "  申卖价三 AskPrice3="+lexical_cast<string>(pmd->AskPrice3)+
            "  申卖量三 AskVolume3="+lexical_cast<string>(pmd->AskVolume3)+
            "  申买价四 BidPrice4="+lexical_cast<string>(pmd->BidPrice4)+
            "  申买量四 BidVolume4="+lexical_cast<string>(pmd->BidVolume4)+
            "  申卖价四 AskPrice4="+lexical_cast<string>(pmd->AskPrice4)+
            "  申卖量四 AskVolume4="+lexical_cast<string>(pmd->AskVolume4)+
            "  申买价五 BidPrice5="+lexical_cast<string>(pmd->BidPrice5)+
            "  申买量五 BidVolume5="+lexical_cast<string>(pmd->BidVolume5)+
            "  申卖价五 AskPrice5="+lexical_cast<string>(pmd->AskPrice5)+
            "  申卖量五 AskVolume5="+lexical_cast<string>(pmd->AskVolume5)+
            "  当日均价 AveragePrice="+lexical_cast<string>(pmd->AveragePrice)+
             "  业务日期 ActionDay="+lexical_cast<string>(pmd->ActionDay);

    return invesinfo;
}

string strRspInfoField(CThostFtdcRspInfoField *pRspInfo)
{
    string tmp;
    if(pRspInfo==NULL)
        return ( tmp);
//    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
      string  tmpstr =
                " 错误代码 ErrorID="+ lexical_cast<string>(pRspInfo->ErrorID)+" 错误信息 ErrorMsg="+between(string(pRspInfo->ErrorMsg),"UTF-8","GBK");
    return tmpstr;
}

string strReqUserLoginField(CThostFtdcReqUserLoginField*req)
{
    string strreq=
            "经纪公司代码 BrokerID="+lexical_cast<string>(req->BrokerID)+
            "  交易日 TradingDay="+req->TradingDay+
            "  用户代码 UserID="+req->UserID+
            "  密码 Password="+req->Password+
            "  用户端产品信息 UserProductInfo="+req->UserProductInfo+
            "  接口端产品信息 InterfaceProductInfo="+req->InterfaceProductInfo+
            "  协议信息 ProtocolInfo="+req->ProtocolInfo+
            "  Mac地址 MacAddress="+req->MacAddress+
            "  动态密码 OneTimePassword="+req->OneTimePassword+
            "  终端IP地址 ClientIPAddress="+req->ClientIPAddress;
    return strreq;
}

string strRspUserLoginField(CThostFtdcRspUserLoginField *pRsp)
{
    string resp=
            "经纪公司代码 BrokerID="+lexical_cast<string>(pRsp->BrokerID)+
            "  交易日 TradingDay="+pRsp->TradingDay+
            "  登录成功时间 LoginTime="+pRsp->LoginTime+
            "  用户代码 UserID="+pRsp->UserID+
            "  交易系统名称 SystemName="+pRsp->SystemName+

            "  前置编号 FrontID="+lexical_cast<string>(pRsp->FrontID)+
            "  会话编号 SessionID="+lexical_cast<string>(pRsp->SessionID)+
            "  最大报单引用 MaxOrderRef="+lexical_cast<string>(pRsp->MaxOrderRef)+
            "  上期所时间 InvestorID="+pRsp->SHFETime+

            "  大商所时间 DCETime="+pRsp->DCETime+
            "  郑商所时间 CZCETime="+pRsp->CZCETime+
            "  中金所时间 FFEXTime="+pRsp->FFEXTime+
            "  能源中心时间 INETime="+pRsp->INETime;
    return resp;
}


string strQryTradingAccountField(CThostFtdcQryTradingAccountField*qry)
{
    string strreq=
            "经纪公司代码 BrokerID="+lexical_cast<string>(qry->BrokerID)+
            "  投资者代码 InvestorID="+(qry->InvestorID)+
            "  币种代码 CurrencyID="+(qry->CurrencyID);
    return strreq;

}


string strOrderActionField(CThostFtdcOrderActionField *pOrderAction)
{
    string tmp;
    if(pOrderAction==NULL)
        return ( tmp);
    string  action=
            "经纪公司代码 BrokerID="+lexical_cast<string>(pOrderAction->BrokerID)+
            "  投资者代码 InvestorID="+pOrderAction->InvestorID+
            "  报单操作引用 OrderActionRef="+lexical_cast<string>(pOrderAction->OrderActionRef)+
            "  报单引用 OrderRef="+(pOrderAction->OrderRef)+
            "  请求编号 RequestID="+lexical_cast<string>(pOrderAction->RequestID)+
            "  前置编号 FrontID="+lexical_cast<string>(pOrderAction->FrontID)+

            "  会话编号 SessionID="+lexical_cast<string>(pOrderAction->SessionID)+
            "  交易所代码 ExchangeID="+(pOrderAction->ExchangeID)+
            "  报单编号 InvestorID="+(pOrderAction->OrderSysID)+
            "  操作标志 ActionFlag="+(pOrderAction->ActionFlag)+
            "  价格 LimitPrice="+lexical_cast<string>(pOrderAction->LimitPrice)+

            "  数量变化 VolumeChange="+lexical_cast<string>(pOrderAction->VolumeChange)+
            "  操作日期 ActionDate="+(pOrderAction->ActionDate)+
            "  操作时间 ActionTime="+(pOrderAction->ActionTime)+
            "  交易所交易员代码 TraderID="+(pOrderAction->TraderID)+
            "  安装编号 InstallID="+lexical_cast<string>(pOrderAction->InstallID)+

            "  本地报单编号 OrderLocalID="+(pOrderAction->OrderLocalID)+
            "  操作本地编号 ActionLocalID="+(pOrderAction->ActionLocalID)+
            "  会员代码 ParticipantID="+(pOrderAction->ParticipantID)+
            "  客户代码 ClientID="+(pOrderAction->ClientID)+
            "  业务单元 BusinessUnit="+(pOrderAction->BusinessUnit)+

            "  报单操作状态 OrderActionStatus="+(pOrderAction->OrderActionStatus)+
            "  用户代码 UserID="+(pOrderAction->UserID)+
            "  状态信息 StatusMsg="+between(string(pOrderAction->StatusMsg),"UTF-8","GBK")+
            "  合约代码 InstrumentID="+(pOrderAction->InstrumentID);


    return action;
}


string strTradingAccountField(CThostFtdcTradingAccountField *pTradingAccount)
{
    string tmp;
    if(pTradingAccount==NULL)
        return ( tmp);
    string accinfo;
    accinfo="经纪公司代码 BrokerID="+string(pTradingAccount->BrokerID)+
            " 投资者帐号 AccountID="+string(pTradingAccount->AccountID)+
            " 上次质押金额 PreMortgage="+lexical_cast<string>(pTradingAccount->PreMortgage)+
            " 上次信用额度 PreCredit="+lexical_cast<string>(pTradingAccount->PreCredit)+
            " 上次存款额 PreDeposit="+lexical_cast<string>(pTradingAccount->PreDeposit)+
            " 上次结算准备金 PreBalance="+lexical_cast<string>(pTradingAccount->PreBalance)+
            " 上次占用的保证金 PreMargin="+lexical_cast<string>(pTradingAccount->PreMargin)+
            " 利息基数 InterestBase="+lexical_cast<string>(pTradingAccount->InterestBase)+
            " 利息收入Interest="+lexical_cast<string>(pTradingAccount->Interest)+
            " 入金金额 Deposit="+lexical_cast<string>(pTradingAccount->Deposit)+
            " 出金金额 Withdraw="+lexical_cast<string>(pTradingAccount->Withdraw)+
            " 冻结的保证金 FrozenMargin="+lexical_cast<string>(pTradingAccount->FrozenMargin)+
            " 冻结的资金 FrozenCash="+lexical_cast<string>(pTradingAccount->FrozenCash)+
            " 冻结的手续费 FrozenCommission="+lexical_cast<string>(pTradingAccount->FrozenCommission)+
            " 当前保证金总额 CurrMargin="+lexical_cast<string>(pTradingAccount->CurrMargin)+
            " 资金差额 CashIn="+lexical_cast<string>(pTradingAccount->CashIn)+
            " 手续费 Commission="+lexical_cast<string>(pTradingAccount->Commission)+
            " 平仓盈亏 CloseProfit="+lexical_cast<string>(pTradingAccount->CloseProfit)+
            " 持仓盈亏 PositionProfit="+lexical_cast<string>(pTradingAccount->PositionProfit)+
            " 期货结算准备金 Balance="+lexical_cast<string>(pTradingAccount->Balance)+
            " 可用资金 Available="+lexical_cast<string>(pTradingAccount->Available)+
            " 可取资金 WithdrawQuota="+lexical_cast<string>(pTradingAccount->WithdrawQuota)+
            " 基本准备金 Reserve="+lexical_cast<string>(pTradingAccount->Reserve)+
            " 交易日 TradingDay="+string(pTradingAccount->TradingDay)+
            " 结算编号 SettlementID="+lexical_cast<string>(pTradingAccount->SettlementID)+
            " 信用额度 Credit="+lexical_cast<string>(pTradingAccount->Credit)+
            " 质押金额 Mortgage="+lexical_cast<string>(pTradingAccount->Mortgage)+
            " 交易所保证金 ExchangeMargin="+lexical_cast<string>(pTradingAccount->ExchangeMargin)+
            " 投资者交割保证金 DeliveryMargin="+lexical_cast<string>(pTradingAccount->DeliveryMargin)+
            " 交易所交割保证金 ExchangeDeliveryMargin="+lexical_cast<string>(pTradingAccount->ExchangeDeliveryMargin)+
            " 保底期货结算准备金 ReserveBalance="+lexical_cast<string>(pTradingAccount->ReserveBalance)+
            " 币种代码 CurrencyID="+lexical_cast<string>(pTradingAccount->CurrencyID);
    return accinfo;
}


string strInvestorPositionField(CThostFtdcInvestorPositionField *pPosition)
{
    string tmp;
    if(pPosition==NULL)
        return ( tmp);
    string invesinfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(pPosition->BrokerID)+
            "  合约代码 InstrumentID="+pPosition->InstrumentID+
            "  投资者代码 InvestorID="+pPosition->InvestorID+
            "  持仓多空方向 PosiDirection="+pPosition->PosiDirection+
            "  投机套保标志 HedgeFlag="+pPosition->HedgeFlag+
            "  持仓日期 PositionDate="+pPosition->PositionDate+
            "  上日持仓 YdPosition="+lexical_cast<string>(pPosition->YdPosition)+
            "  今日持仓 Position="+lexical_cast<string>(pPosition->Position)+
            "  多头冻结 LongFrozen="+lexical_cast<string>(pPosition->LongFrozen)+
            "  空头冻结 ShortFrozen="+lexical_cast<string>(pPosition->ShortFrozen)+
            "  开仓冻结金额 LongFrozenAmount="+lexical_cast<string>(pPosition->LongFrozenAmount)+

            "  开仓冻结金额 ShortFrozenAmount="+lexical_cast<string>(pPosition->ShortFrozenAmount)+
            "  开仓量 OpenVolume="+lexical_cast<string>(pPosition->OpenVolume)+
            "  平仓量 CloseVolume="+lexical_cast<string>(pPosition->CloseVolume)+
            "  开仓金额 OpenAmount="+lexical_cast<string>(pPosition->OpenAmount)+
            "  平仓金额 CloseAmount="+lexical_cast<string>(pPosition->CloseAmount)+
            "  持仓成本 PositionCost="+lexical_cast<string>(pPosition->PositionCost)+
            "  上次占用的保证金 PreMargin="+lexical_cast<string>(pPosition->PreMargin)+
            "  占用的保证金 UseMargin="+lexical_cast<string>(pPosition->UseMargin)+
            "  冻结的保证金 FrozenMargin="+lexical_cast<string>(pPosition->FrozenMargin)+
            "  冻结的资金 FrozenCash="+lexical_cast<string>(pPosition->FrozenCash)+
            "  冻结的手续费 FrozenCommission="+lexical_cast<string>(pPosition->FrozenCommission)+
            "  资金差额 CashIn="+lexical_cast<string>(pPosition->CashIn)+
            "  手续费 Commission="+lexical_cast<string>(pPosition->Commission)+
            "  **平仓盈亏 CloseProfit="+lexical_cast<string>(pPosition->CloseProfit)+
            "  **持仓盈亏 PositionProfit="+lexical_cast<string>(pPosition->PositionProfit)+
            "  上次结算价 PreSettlementPrice="+lexical_cast<string>(pPosition->PreSettlementPrice)+
            "  本次结算价 SettlementPrice="+lexical_cast<string>(pPosition->SettlementPrice)+
            "  交易日 TradingDay="+pPosition->TradingDay+

            "  结算编号 SettlementID="+lexical_cast<string>(pPosition->SettlementID)+
            "  开仓成本 OpenCost="+lexical_cast<string>(pPosition->OpenCost)+
            "  交易所保证金 ExchangeMargin="+lexical_cast<string>(pPosition->ExchangeMargin)+
            "  组合成交形成的持仓 CombPosition="+lexical_cast<string>(pPosition->CombPosition)+
            "  组合多头冻结 CombLongFrozen="+lexical_cast<string>(pPosition->CombLongFrozen)+
            "  组合空头冻结 CombShortFrozen="+lexical_cast<string>(pPosition->CombShortFrozen)+
            "  逐日盯市平仓盈亏 CloseProfitByDate="+lexical_cast<string>(pPosition->CloseProfitByDate)+
            "  逐笔对冲平仓盈亏 CloseProfitByTrade="+lexical_cast<string>(pPosition->CloseProfitByTrade)+
            "  今日持仓 TodayPosition="+lexical_cast<string>(pPosition->TodayPosition)+
            "  保证金率 MarginRateByMoney="+lexical_cast<string>(pPosition->MarginRateByMoney)+
            "  保证金率(按手数) MarginRateByVolume="+lexical_cast<string>(pPosition->MarginRateByVolume)+
            "  执行冻结 StrikeFrozen="+lexical_cast<string>(pPosition->StrikeFrozen)+
            "  执行冻结金额 StrikeFrozenAmount="+lexical_cast<string>(pPosition->StrikeFrozenAmount)+
            "  放弃执行冻结 AbandonFrozen="+lexical_cast<string>(pPosition->AbandonFrozen);

    return invesinfo;

}


//提取投资者报单信息
string strInputOrderField(CThostFtdcInputOrderField *order)
{
    string tmp;
    if(order==NULL)
        return ( tmp);
    string ordreInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "  **投资者代码 InvestorID="+order->InvestorID+
            "  **合约代码 InstrumentID="+order->InstrumentID+
            "  报单引用 OrderRef="+order->OrderRef+
            "  用户代码 UserID="+order->UserID+
            "  报单价格条件 OrderPriceType="+order->OrderPriceType+
            "  **买卖方向 Direction="+order->Direction+
            "  组合开平标志 CombOffsetFlag="+order->CombOffsetFlag+
            "  组合投机套保标志 CombHedgeFlag="+order->CombHedgeFlag+
            "  **价格 LimitPrice="+lexical_cast<string>(order->LimitPrice)+
            "  **数量 VolumeTotalOriginal="+lexical_cast<string>(order->VolumeTotalOriginal)+
            "  有效期类型 TimeCondition="+order->TimeCondition+
            "  GTD日期 GTDDate="+order->GTDDate+
            "  成交量类型 VolumeCondition="+lexical_cast<string>(order->VolumeCondition)+
            "  最小成交量 MinVolume="+lexical_cast<string>(order->MinVolume)+
            "  触发条件 ContingentCondition="+order->ContingentCondition+
            "  止损价 StopPrice="+lexical_cast<string>(order->StopPrice)+
            "  强平原因 ForceCloseReason="+order->ForceCloseReason+
            "  自动挂起标志 IsAutoSuspend="+lexical_cast<string>(order->IsAutoSuspend)+
            "  业务单元 BusinessUnit="+order->BusinessUnit+
            "  请求编号 RequestID="+lexical_cast<string>(order->RequestID)+
            "  用户强评标志 UserForceClose="+lexical_cast<string>(order->UserForceClose)+
            "  互换单标志 IsSwapOrder="+lexical_cast<string>(order->IsSwapOrder);

    return ordreInfo;
}

//提取投资者报单信息
string strInputOrderActionField(CThostFtdcInputOrderActionField *order)
{
    string orderInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "  **投资者代码 InvestorID="+order->InvestorID+
            "  报单操作引用 OrderActionRef="+lexical_cast<string>(order->OrderActionRef)+
            "  **报单引用 OrderRef="+order->OrderRef+
            "  请求编号 RequestID="+lexical_cast<string>(order->RequestID)+
            "  前置编号 FrontID="+lexical_cast<string>(order->FrontID)+
            "  会话编号 SessionID="+lexical_cast<string>(order->SessionID)+
            "  交易所代码 ExchangeID="+order->ExchangeID+
            "  **报单编号 OrderSysID="+order->OrderSysID+
            "  **操作标志 ActionFlag="+order->ActionFlag+
            "  价格 LimitPrice="+lexical_cast<string>(order->LimitPrice)+
            "  数量变化 VolumeChange="+lexical_cast<string>(order->VolumeChange)+
            "  用户代码 UserID="+order->UserID+
            "  合约代码 InstrumentID="+order->InstrumentID;
    return orderInfo;
}


//提取委托回报信息
string strOrderField(CThostFtdcOrderField *order)
{

    string orderInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "  投资者代码 InvestorID="+order->InvestorID+
            "  **合约代码 InstrumentID="+order->InstrumentID+
            "  报单引用 OrderRef="+order->OrderRef+
            "  用户代码 UserID="+order->UserID+
            "  报单价格条件 OrderPriceType="+order->OrderPriceType+
            "  **买卖方向 Direction="+order->Direction+
            "  **组合开平标志 CombOffsetFlag="+order->CombOffsetFlag+
            "  组合投机套保标志 CombHedgeFlag="+order->CombHedgeFlag+
            "  **价格 LimitPrice="+lexical_cast<string>(order->LimitPrice)+
            "  **数量 VolumeTotalOriginal="+lexical_cast<string>(order->VolumeTotalOriginal)+
            "  有效期类型 TimeCondition="+order->TimeCondition+

            "  GTD日期 GTDDate="+order->GTDDate+
            "  成交量类型 VolumeCondition="+order->VolumeCondition+
            "  最小成交量 MinVolume="+lexical_cast<string>(order->MinVolume)+
            "  触发条件 ContingentCondition="+order->ContingentCondition+
            "  止损价 StopPrice="+lexical_cast<string>(order->StopPrice)+
            "  强平原因 ForceCloseReason="+order->ForceCloseReason+
            "  自动挂起标志 IsAutoSuspend="+lexical_cast<string>(order->IsAutoSuspend)+
            "  业务单元 BusinessUnit="+order->BusinessUnit+
            "  请求编号 RequestID="+lexical_cast<string>(order->RequestID)+
            "  本地报单编号 OrderLocalID="+order->OrderLocalID+
            "  交易所代码 ExchangeID="+order->ExchangeID+

            "  会员代码 ParticipantID="+order->ParticipantID+
            "  客户代码 ClientID="+order->ClientID+
            "  合约在交易所的代码 ExchangeInstID="+order->ExchangeInstID+
            "  交易所交易员代码 TraderID="+order->TraderID+
            "  安装编号 InstallID="+lexical_cast<string>(order->InstallID)+
            "  报单提交状态 OrderSubmitStatus="+order->OrderSubmitStatus+
            "  报单提示序号 NotifySequence="+lexical_cast<string>(order->NotifySequence)+
            "  交易日 TradingDay="+order->TradingDay+
            "  结算编号 SettlementID="+lexical_cast<string>(order->SettlementID)+
            "  **报单编号 OrderSysID="+order->OrderSysID+
            "  报单来源 OrderSource="+order->OrderSource+

            "  **报单状态 OrderStatus="+order->OrderStatus+
            "  报单类型 OrderType="+order->OrderType+
            "  今成交数量 VolumeTraded="+lexical_cast<string>(order->VolumeTraded)+
            "  剩余数量 VolumeTotal="+lexical_cast<string>(order->VolumeTotal)+
            "  报单日期 InsertDate="+order->InsertDate+
            "  委托时间 InsertTime="+order->InsertTime+
            "  激活时间 ActiveTime="+order->ActiveTime+
            "  挂起时间 SuspendTime="+order->SuspendTime+
            "  最后修改时间 UpdateTime="+order->UpdateTime+
            "  撤销时间 CancelTime="+order->CancelTime+
            "  最后修改交易所交易员代码 ActiveTraderID="+order->ActiveTraderID+

            "  结算会员编号 ClearingPartID="+order->ClearingPartID+
            "  **序号 SequenceNo="+lexical_cast<string>(order->SequenceNo)+
            "  前置编号 FrontID="+lexical_cast<string>(order->FrontID)+
            "  会话编号 SessionID="+lexical_cast<string>(order->SessionID)+
            "  用户端产品信息 UserProductInfo="+order->UserProductInfo+
            "  状态信息 StatusMsg="+between(order->StatusMsg,"UTF-8","GBK")+
            "  用户强评标志 UserForceClose="+lexical_cast<string>(order->UserForceClose)+
            "  操作用户代码 ActiveUserID="+order->ActiveUserID+
            "  经纪公司报单编号 BrokerOrderSeq="+lexical_cast<string>(order->BrokerOrderSeq)+
            "  相关报单 ZCETotalTradedVolume="+order->RelativeOrderSysID+
            "  郑商所成交数量 ZCETotalTradedVolume="+lexical_cast<string>(order->ZCETotalTradedVolume)+
            "  互换单标志 IsSwapOrder="+lexical_cast<string>(order->IsSwapOrder);
    return orderInfo;
}


string strTradeField(CThostFtdcTradeField *order)
{
    string orderInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "  **投资者代码 InvestorID="+order->InvestorID+
            "  **合约代码 InstrumentID="+order->InstrumentID+
            "  **报单引用 OrderRef="+order->OrderRef+
            "  用户代码 UserID="+order->UserID+
            "  交易所代码 ExchangeID="+order->ExchangeID+
            "  成交编号 TradeID="+order->TradeID+
            "  **买卖方向 Direction="+order->Direction+
            "  **报单编号 OrderSysID="+order->OrderSysID+
            "  会员代码 ParticipantID="+order->ParticipantID+
            "  客户代码 ClientID="+order->ClientID+
            "  交易角色 TradingRole="+lexical_cast<string>(order->TradingRole)+
            "  合约在交易所的代码 ExchangeInstID="+order->ExchangeInstID+
            "  **开平标志 OffsetFlag="+order->OffsetFlag+
            "  投机套保标志 HedgeFlag="+order->HedgeFlag+
            "  **价格 Price="+lexical_cast<string>(order->Price)+
            "  **数量 Volume="+lexical_cast<string>(order->Volume)+
            "  成交时期 TradeDate="+order->TradeDate+
            "  成交时间 TradeTime="+order->TradeTime+
            "  成交类型 TradeType="+lexical_cast<string>(order->TradeType)+
            "  成交价来源 PriceSource="+lexical_cast<string>(order->PriceSource)+
            "  交易所交易员代码 TraderID="+order->TraderID+
            "  本地报单编号 OrderLocalID="+order->OrderLocalID+
            "  结算会员编号 ClearingPartID="+order->ClearingPartID+
            "  业务单元 BusinessUnit="+order->BusinessUnit+
            "  序号 SequenceNo="+lexical_cast<string>(order->SequenceNo)+
            "  交易日 TradingDay="+order->TradingDay+
            "  结算编号 SettlementID="+lexical_cast<string>(order->SettlementID)+
            "  经纪公司报单编号 BrokerOrderSeq="+lexical_cast<string>(order->BrokerOrderSeq)+
            "  成交来源 TradeSource="+order->TradeSource;
    //    cout << orderInfo <<endl;
    return orderInfo;
}





