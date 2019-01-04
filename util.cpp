#include "util.h"
extern DataInitInstance dii;


string strRspInfoField(CThostFtdcRspInfoField *pRspInfo)
{
    string tmp;
    if(pRspInfo==NULL)
        return ( tmp);
    bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
      string tmpstr;
    if (bResult){
       tmpstr =
       "错误代码 ErrorID="+ lexical_cast<string>(pRspInfo->ErrorID)+
        "\n 错误信息 ErrorMsg="+between(string(pRspInfo->ErrorMsg),"UTF-8","GBK");

    }
    return tmpstr;
}

string strReqUserLoginField(CThostFtdcReqUserLoginField*req)
{
    string strreq=
            "经纪公司代码 BrokerID="+lexical_cast<string>(req->BrokerID)+
            "\n 交易日 TradingDay="+req->TradingDay+
            "\n 用户代码 UserID="+req->UserID+
            "\n 密码 Password="+req->Password+
            "\n 用户端产品信息 UserProductInfo="+req->UserProductInfo+
            "\n 接口端产品信息 InterfaceProductInfo="+req->InterfaceProductInfo+
            "\n 协议信息 ProtocolInfo="+req->ProtocolInfo+
            "\n Mac地址 MacAddress="+req->MacAddress+
            "\n 动态密码 OneTimePassword="+req->OneTimePassword+
            "\n 终端IP地址 ClientIPAddress="+req->ClientIPAddress;
    return strreq;
}

string strRspUserLoginField(CThostFtdcRspUserLoginField *pRsp)
{
    string resp=
            "经纪公司代码 BrokerID="+lexical_cast<string>(pRsp->BrokerID)+
            "\n 交易日 TradingDay="+pRsp->TradingDay+
            "\n 登录成功时间 LoginTime="+pRsp->LoginTime+
            "\n 用户代码 UserID="+pRsp->UserID+
            "\n 交易系统名称 SystemName="+pRsp->SystemName+

            "\n 前置编号 FrontID="+lexical_cast<string>(pRsp->FrontID)+
            "\n 会话编号 SessionID="+lexical_cast<string>(pRsp->SessionID)+
            "\n 最大报单引用 MaxOrderRef="+lexical_cast<string>(pRsp->MaxOrderRef)+
            "\n 上期所时间 InvestorID="+pRsp->SHFETime+

            "\n 大商所时间 DCETime="+pRsp->DCETime+
            "\n 郑商所时间 CZCETime="+pRsp->CZCETime+
            "\n 中金所时间 FFEXTime="+pRsp->FFEXTime+
            "\n 能源中心时间 INETime="+pRsp->INETime;
    return resp;
}


string strQryTradingAccountField(CThostFtdcQryTradingAccountField*qry)
{
    string strreq=
            "经纪公司代码 BrokerID="+lexical_cast<string>(qry->BrokerID)+
            "\n 投资者代码 InvestorID="+(qry->InvestorID)+
            "\n 币种代码 CurrencyID="+(qry->CurrencyID);
    return strreq;

}


string strOrderActionField(CThostFtdcOrderActionField *pOrderAction)
{
    string tmp;
    if(pOrderAction==NULL)
        return ( tmp);
string  action=
        "经纪公司代码 BrokerID="+lexical_cast<string>(pOrderAction->BrokerID)+
        "\n 投资者代码 InvestorID="+pOrderAction->InvestorID+
        "\n 报单操作引用 OrderActionRef="+lexical_cast<string>(pOrderAction->OrderActionRef)+
        "\n 报单引用 OrderRef="+(pOrderAction->OrderRef)+
        "\n 请求编号 RequestID="+lexical_cast<string>(pOrderAction->RequestID)+
        "\n 前置编号 FrontID="+lexical_cast<string>(pOrderAction->FrontID)+

        "\n 会话编号 SessionID="+lexical_cast<string>(pOrderAction->SessionID)+
        "\n 交易所代码 ExchangeID="+(pOrderAction->ExchangeID)+
        "\n 报单编号 InvestorID="+(pOrderAction->OrderSysID)+
        "\n 操作标志 ActionFlag="+(pOrderAction->ActionFlag)+
        "\n 价格 LimitPrice="+lexical_cast<string>(pOrderAction->LimitPrice)+

        "\n 数量变化 VolumeChange="+lexical_cast<string>(pOrderAction->VolumeChange)+
        "\n 操作日期 ActionDate="+(pOrderAction->ActionDate)+
        "\n 操作时间 ActionTime="+(pOrderAction->ActionTime)+
        "\n 交易所交易员代码 TraderID="+(pOrderAction->TraderID)+
        "\n 安装编号 InstallID="+lexical_cast<string>(pOrderAction->InstallID)+

        "\n 本地报单编号 OrderLocalID="+(pOrderAction->OrderLocalID)+
        "\n 操作本地编号 ActionLocalID="+(pOrderAction->ActionLocalID)+
        "\n 会员代码 ParticipantID="+(pOrderAction->ParticipantID)+
        "\n 客户代码 ClientID="+(pOrderAction->ClientID)+
        "\n 业务单元 BusinessUnit="+(pOrderAction->BusinessUnit)+

        "\n 报单操作状态 OrderActionStatus="+(pOrderAction->OrderActionStatus)+
        "\n 用户代码 UserID="+(pOrderAction->UserID)+
        "\n 状态信息 StatusMsg="+between(string(pOrderAction->StatusMsg),"UTF-8","GBK")+
        "\n 合约代码 InstrumentID="+(pOrderAction->InstrumentID);


  return action;
}


string strTradingAccountField(CThostFtdcTradingAccountField *pTradingAccount)
{
    string tmp;
    if(pTradingAccount==NULL)
        return ( tmp);
    string accinfo;
    accinfo="经纪公司代码 BrokerID="+string(pTradingAccount->BrokerID)+
            "\n投资者帐号 AccountID="+string(pTradingAccount->AccountID)+
            "\n上次质押金额 PreMortgage="+lexical_cast<string>(pTradingAccount->PreMortgage)+
            "\n上次信用额度 PreCredit="+lexical_cast<string>(pTradingAccount->PreCredit)+
            "\n上次存款额 PreDeposit="+lexical_cast<string>(pTradingAccount->PreDeposit)+
            "\n上次结算准备金 PreBalance="+lexical_cast<string>(pTradingAccount->PreBalance)+
            "\n上次占用的保证金 PreMargin="+lexical_cast<string>(pTradingAccount->PreMargin)+
            "\n利息基数 InterestBase="+lexical_cast<string>(pTradingAccount->InterestBase)+
            "\n利息收入Interest="+lexical_cast<string>(pTradingAccount->Interest)+
            "\n入金金额 Deposit="+lexical_cast<string>(pTradingAccount->Deposit)+
            "\n出金金额 Withdraw="+lexical_cast<string>(pTradingAccount->Withdraw)+
            "\n冻结的保证金 FrozenMargin="+lexical_cast<string>(pTradingAccount->FrozenMargin)+
            "\n冻结的资金 FrozenCash="+lexical_cast<string>(pTradingAccount->FrozenCash)+
            "\n冻结的手续费 FrozenCommission="+lexical_cast<string>(pTradingAccount->FrozenCommission)+
            "\n当前保证金总额 CurrMargin="+lexical_cast<string>(pTradingAccount->CurrMargin)+
            "\n资金差额 CashIn="+lexical_cast<string>(pTradingAccount->CashIn)+
            "\n手续费 Commission="+lexical_cast<string>(pTradingAccount->Commission)+
            "\n平仓盈亏 CloseProfit="+lexical_cast<string>(pTradingAccount->CloseProfit)+
            "\n持仓盈亏 PositionProfit="+lexical_cast<string>(pTradingAccount->PositionProfit)+
            "\n期货结算准备金 Balance="+lexical_cast<string>(pTradingAccount->Balance)+
            "\n可用资金 Available="+lexical_cast<string>(pTradingAccount->Available)+
            "\n可取资金 WithdrawQuota="+lexical_cast<string>(pTradingAccount->WithdrawQuota)+
            "\n基本准备金 Reserve="+lexical_cast<string>(pTradingAccount->Reserve)+
            "\n交易日 TradingDay="+string(pTradingAccount->TradingDay)+
            "\n结算编号 SettlementID="+lexical_cast<string>(pTradingAccount->SettlementID)+
            "\n信用额度 Credit="+lexical_cast<string>(pTradingAccount->Credit)+
            "\n质押金额 Mortgage="+lexical_cast<string>(pTradingAccount->Mortgage)+
            "\n交易所保证金 ExchangeMargin="+lexical_cast<string>(pTradingAccount->ExchangeMargin)+
            "\n投资者交割保证金 DeliveryMargin="+lexical_cast<string>(pTradingAccount->DeliveryMargin)+
            "\n交易所交割保证金 ExchangeDeliveryMargin="+lexical_cast<string>(pTradingAccount->ExchangeDeliveryMargin)+
            "\n保底期货结算准备金 ReserveBalance="+lexical_cast<string>(pTradingAccount->ReserveBalance)+
            "\n币种代码 CurrencyID="+lexical_cast<string>(pTradingAccount->CurrencyID);
    return accinfo;
}


string strInvestorPositionField(CThostFtdcInvestorPositionField *pPosition)
{
    string tmp;
    if(pPosition==NULL)
        return ( tmp);
    string invesinfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(pPosition->BrokerID)+
            "\n 合约代码 InstrumentID="+pPosition->InstrumentID+
            "\n 投资者代码 InvestorID="+pPosition->InvestorID+
            "\n 持仓多空方向 PosiDirection="+pPosition->PosiDirection+
            "\n 投机套保标志 HedgeFlag="+pPosition->HedgeFlag+
            "\n 持仓日期 PositionDate="+pPosition->PositionDate+
            "\n 上日持仓 YdPosition="+lexical_cast<string>(pPosition->YdPosition)+
            "\n 今日持仓 Position="+lexical_cast<string>(pPosition->Position)+
            "\n 多头冻结 LongFrozen="+lexical_cast<string>(pPosition->LongFrozen)+
            "\n 空头冻结 ShortFrozen="+lexical_cast<string>(pPosition->ShortFrozen)+
            "\n 开仓冻结金额 LongFrozenAmount="+lexical_cast<string>(pPosition->LongFrozenAmount)+

            "\n 开仓冻结金额 ShortFrozenAmount="+lexical_cast<string>(pPosition->ShortFrozenAmount)+
            "\n 开仓量 OpenVolume="+lexical_cast<string>(pPosition->OpenVolume)+
            "\n 平仓量 CloseVolume="+lexical_cast<string>(pPosition->CloseVolume)+
            "\n 开仓金额 OpenAmount="+lexical_cast<string>(pPosition->OpenAmount)+
            "\n 平仓金额 CloseAmount="+lexical_cast<string>(pPosition->CloseAmount)+
            "\n 持仓成本 PositionCost="+lexical_cast<string>(pPosition->PositionCost)+
            "\n 上次占用的保证金 PreMargin="+lexical_cast<string>(pPosition->PreMargin)+
            "\n 占用的保证金 UseMargin="+lexical_cast<string>(pPosition->UseMargin)+
            "\n 冻结的保证金 FrozenMargin="+lexical_cast<string>(pPosition->FrozenMargin)+
            "\n 冻结的资金 FrozenCash="+lexical_cast<string>(pPosition->FrozenCash)+
            "\n 冻结的手续费 FrozenCommission="+lexical_cast<string>(pPosition->FrozenCommission)+
            "\n 资金差额 CashIn="+lexical_cast<string>(pPosition->CashIn)+
            "\n 手续费 Commission="+lexical_cast<string>(pPosition->Commission)+
            "\n **平仓盈亏 CloseProfit="+lexical_cast<string>(pPosition->CloseProfit)+
            "\n **持仓盈亏 PositionProfit="+lexical_cast<string>(pPosition->PositionProfit)+
            "\n 上次结算价 PreSettlementPrice="+lexical_cast<string>(pPosition->PreSettlementPrice)+
            "\n 本次结算价 SettlementPrice="+lexical_cast<string>(pPosition->SettlementPrice)+
            "\n 交易日 TradingDay="+pPosition->TradingDay+

            "\n 结算编号 SettlementID="+lexical_cast<string>(pPosition->SettlementID)+
            "\n 开仓成本 OpenCost="+lexical_cast<string>(pPosition->OpenCost)+
            "\n 交易所保证金 ExchangeMargin="+lexical_cast<string>(pPosition->ExchangeMargin)+
            "\n 组合成交形成的持仓 CombPosition="+lexical_cast<string>(pPosition->CombPosition)+
            "\n 组合多头冻结 CombLongFrozen="+lexical_cast<string>(pPosition->CombLongFrozen)+
            "\n 组合空头冻结 CombShortFrozen="+lexical_cast<string>(pPosition->CombShortFrozen)+
            "\n 逐日盯市平仓盈亏 CloseProfitByDate="+lexical_cast<string>(pPosition->CloseProfitByDate)+
            "\n 逐笔对冲平仓盈亏 CloseProfitByTrade="+lexical_cast<string>(pPosition->CloseProfitByTrade)+
            "\n 今日持仓 TodayPosition="+lexical_cast<string>(pPosition->TodayPosition)+
            "\n 保证金率 MarginRateByMoney="+lexical_cast<string>(pPosition->MarginRateByMoney)+
            "\n 保证金率(按手数) MarginRateByVolume="+lexical_cast<string>(pPosition->MarginRateByVolume)+
            "\n 执行冻结 StrikeFrozen="+lexical_cast<string>(pPosition->StrikeFrozen)+
            "\n 执行冻结金额 StrikeFrozenAmount="+lexical_cast<string>(pPosition->StrikeFrozenAmount)+
            "\n 放弃执行冻结 AbandonFrozen="+lexical_cast<string>(pPosition->AbandonFrozen);

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
            "\n **投资者代码 InvestorID="+order->InvestorID+
            "\n **合约代码 InstrumentID="+order->InstrumentID+
            "\n 报单引用 OrderRef="+order->OrderRef+
            "\n 用户代码 UserID="+order->UserID+
            "\n 报单价格条件 OrderPriceType="+order->OrderPriceType+
            "\n **买卖方向 Direction="+order->Direction+
            "\n 组合开平标志 CombOffsetFlag="+order->CombOffsetFlag+
            "\n 组合投机套保标志 CombHedgeFlag="+order->CombHedgeFlag+
            "\n **价格 LimitPrice="+lexical_cast<string>(order->LimitPrice)+
            "\n **数量 VolumeTotalOriginal="+lexical_cast<string>(order->VolumeTotalOriginal)+
            "\n 有效期类型 TimeCondition="+order->TimeCondition+
            "\n GTD日期 GTDDate="+order->GTDDate+
            "\n 成交量类型 VolumeCondition="+lexical_cast<string>(order->VolumeCondition)+
            "\n 最小成交量 MinVolume="+lexical_cast<string>(order->MinVolume)+
            "\n 触发条件 ContingentCondition="+order->ContingentCondition+
            "\n 止损价 StopPrice="+lexical_cast<string>(order->StopPrice)+
            "\n 强平原因 ForceCloseReason="+order->ForceCloseReason+
            "\n 自动挂起标志 IsAutoSuspend="+lexical_cast<string>(order->IsAutoSuspend)+
            "\n 业务单元 BusinessUnit="+order->BusinessUnit+
            "\n 请求编号 RequestID="+lexical_cast<string>(order->RequestID)+
            "\n 用户强评标志 UserForceClose="+lexical_cast<string>(order->UserForceClose)+
            "\n 互换单标志 IsSwapOrder="+lexical_cast<string>(order->IsSwapOrder);

    return ordreInfo;
}

//提取投资者报单信息
string strInputOrderActionField(CThostFtdcInputOrderActionField *order)
{
    string orderInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "\n **投资者代码 InvestorID="+order->InvestorID+
            "\n 报单操作引用 OrderActionRef="+lexical_cast<string>(order->OrderActionRef)+
            "\n **报单引用 OrderRef="+order->OrderRef+
            "\n 请求编号 RequestID="+lexical_cast<string>(order->RequestID)+
            "\n 前置编号 FrontID="+lexical_cast<string>(order->FrontID)+
            "\n 会话编号 SessionID="+lexical_cast<string>(order->SessionID)+
            "\n 交易所代码 ExchangeID="+order->ExchangeID+
            "\n **报单编号 OrderSysID="+order->OrderSysID+
            "\n **操作标志 ActionFlag="+order->ActionFlag+
            "\n 价格 LimitPrice="+lexical_cast<string>(order->LimitPrice)+
            "\n 数量变化 VolumeChange="+lexical_cast<string>(order->VolumeChange)+
            "\n 用户代码 UserID="+order->UserID+
            "\n 合约代码 InstrumentID="+order->InstrumentID;
    return orderInfo;
}


//提取委托回报信息
string strOrderField(CThostFtdcOrderField *order)
{

    string orderInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "\n 投资者代码 InvestorID="+order->InvestorID+
            "\n **合约代码 InstrumentID="+order->InstrumentID+
            "\n 报单引用 OrderRef="+order->OrderRef+
            "\n 用户代码 UserID="+order->UserID+
            "\n 报单价格条件 OrderPriceType="+order->OrderPriceType+
            "\n **买卖方向 Direction="+order->Direction+
            "\n **组合开平标志 CombOffsetFlag="+order->CombOffsetFlag+
            "\n 组合投机套保标志 CombHedgeFlag="+order->CombHedgeFlag+
            "\n **价格 LimitPrice="+lexical_cast<string>(order->LimitPrice)+
            "\n **数量 VolumeTotalOriginal="+lexical_cast<string>(order->VolumeTotalOriginal)+
            "\n 有效期类型 TimeCondition="+order->TimeCondition+

            "\n GTD日期 GTDDate="+order->GTDDate+
            "\n 成交量类型 VolumeCondition="+order->VolumeCondition+
            "\n 最小成交量 MinVolume="+lexical_cast<string>(order->MinVolume)+
            "\n 触发条件 ContingentCondition="+order->ContingentCondition+
            "\n 止损价 StopPrice="+lexical_cast<string>(order->StopPrice)+
            "\n 强平原因 ForceCloseReason="+order->ForceCloseReason+
            "\n 自动挂起标志 IsAutoSuspend="+lexical_cast<string>(order->IsAutoSuspend)+
            "\n 业务单元 BusinessUnit="+order->BusinessUnit+
            "\n 请求编号 RequestID="+lexical_cast<string>(order->RequestID)+
            "\n 本地报单编号 OrderLocalID="+order->OrderLocalID+
            "\n 交易所代码 ExchangeID="+order->ExchangeID+

            "\n 会员代码 ParticipantID="+order->ParticipantID+
            "\n 客户代码 ClientID="+order->ClientID+
            "\n 合约在交易所的代码 ExchangeInstID="+order->ExchangeInstID+
            "\n 交易所交易员代码 TraderID="+order->TraderID+
            "\n 安装编号 InstallID="+lexical_cast<string>(order->InstallID)+
            "\n 报单提交状态 OrderSubmitStatus="+order->OrderSubmitStatus+
            "\n 报单提示序号 NotifySequence="+lexical_cast<string>(order->NotifySequence)+
            "\n 交易日 TradingDay="+order->TradingDay+
            "\n 结算编号 SettlementID="+lexical_cast<string>(order->SettlementID)+
            "\n **报单编号 OrderSysID="+order->OrderSysID+
            "\n 报单来源 OrderSource="+order->OrderSource+

            "\n **报单状态 OrderStatus="+order->OrderStatus+
            "\n 报单类型 OrderType="+order->OrderType+
            "\n 今成交数量 VolumeTraded="+lexical_cast<string>(order->VolumeTraded)+
            "\n 剩余数量 VolumeTotal="+lexical_cast<string>(order->VolumeTotal)+
            "\n 报单日期 InsertDate="+order->InsertDate+
            "\n 委托时间 InsertTime="+order->InsertTime+
            "\n 激活时间 ActiveTime="+order->ActiveTime+
            "\n 挂起时间 SuspendTime="+order->SuspendTime+
            "\n 最后修改时间 UpdateTime="+order->UpdateTime+
            "\n 撤销时间 CancelTime="+order->CancelTime+
            "\n 最后修改交易所交易员代码 ActiveTraderID="+order->ActiveTraderID+

            "\n 结算会员编号 ClearingPartID="+order->ClearingPartID+
            "\n **序号 SequenceNo="+lexical_cast<string>(order->SequenceNo)+
            "\n 前置编号 FrontID="+lexical_cast<string>(order->FrontID)+
            "\n 会话编号 SessionID="+lexical_cast<string>(order->SessionID)+
            "\n 用户端产品信息 UserProductInfo="+order->UserProductInfo+
            "\n 状态信息 StatusMsg="+between(order->StatusMsg,"UTF-8","GBK")+
            "\n 用户强评标志 UserForceClose="+lexical_cast<string>(order->UserForceClose)+
            "\n 操作用户代码 ActiveUserID="+order->ActiveUserID+
            "\n 经纪公司报单编号 BrokerOrderSeq="+lexical_cast<string>(order->BrokerOrderSeq)+
            "\n 相关报单 ZCETotalTradedVolume="+order->RelativeOrderSysID+
            "\n 郑商所成交数量 ZCETotalTradedVolume="+lexical_cast<string>(order->ZCETotalTradedVolume)+
            "\n 互换单标志 IsSwapOrder="+lexical_cast<string>(order->IsSwapOrder);
    return orderInfo;
}


string strTradeField(CThostFtdcTradeField *order)
{
    string orderInfo=
            "经纪公司代码 BrokerID="+lexical_cast<string>(order->BrokerID)+
            "\n **投资者代码 InvestorID="+order->InvestorID+
            "\n **合约代码 InstrumentID="+order->InstrumentID+
            "\n **报单引用 OrderRef="+order->OrderRef+
            "\n 用户代码 UserID="+order->UserID+
            "\n 交易所代码 ExchangeID="+order->ExchangeID+
            "\n 成交编号 TradeID="+order->TradeID+
            "\n **买卖方向 Direction="+order->Direction+
            "\n **报单编号 OrderSysID="+order->OrderSysID+
            "\n 会员代码 ParticipantID="+order->ParticipantID+
            "\n 客户代码 ClientID="+order->ClientID+
            "\n 交易角色 TradingRole="+lexical_cast<string>(order->TradingRole)+
            "\n 合约在交易所的代码 ExchangeInstID="+order->ExchangeInstID+
            "\n **开平标志 OffsetFlag="+order->OffsetFlag+
            "\n 投机套保标志 HedgeFlag="+order->HedgeFlag+
            "\n **价格 Price="+lexical_cast<string>(order->Price)+
            "\n **数量 Volume="+lexical_cast<string>(order->Volume)+
            "\n 成交时期 TradeDate="+order->TradeDate+
            "\n 成交时间 TradeTime="+order->TradeTime+
            "\n 成交类型 TradeType="+lexical_cast<string>(order->TradeType)+
            "\n 成交价来源 PriceSource="+lexical_cast<string>(order->PriceSource)+
            "\n 交易所交易员代码 TraderID="+order->TraderID+
            "\n 本地报单编号 OrderLocalID="+order->OrderLocalID+
            "\n 结算会员编号 ClearingPartID="+order->ClearingPartID+
            "\n 业务单元 BusinessUnit="+order->BusinessUnit+
            "\n 序号 SequenceNo="+lexical_cast<string>(order->SequenceNo)+
            "\n 交易日 TradingDay="+order->TradingDay+
            "\n 结算编号 SettlementID="+lexical_cast<string>(order->SettlementID)+
            "\n 经纪公司报单编号 BrokerOrderSeq="+lexical_cast<string>(order->BrokerOrderSeq)+
            "\n 成交来源 TradeSource="+order->TradeSource;
//    cout << orderInfo <<endl;
    return orderInfo;
}





