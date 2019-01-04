#include "tradeprocessor.h"
extern DataInitInstance* dii;
UserAccount* TradeProcessor::getUserAccount(string investorID){
    unordered_map<string, UserAccount*>::iterator usIT = userAccMap.find(investorID);
    if (usIT != userAccMap.end()) {//未建立配对关系,需新建一个vector
        string msg="getUserAccount:investorID="+investorID+"'s account is existed.";
        LOG(INFO)<<msg;
        return (UserAccount*)usIT->second;
    }else {
        string msg="getUserAccount:investorID="+investorID+"'s account is not existed.";
        return NULL;
    }
}
void TradeProcessor::setUserAccount(UserAccount* ua){
    userAccMap[boost::lexical_cast<string>(ua->pTradingAccount->AccountID)]=ua;

}
void TradeProcessor::addNewOrder(UserOrderField* userOrderField){
    //string instrumentID,string direction,string offsetFlag,double orderInsertPrice,int volume,string mkType,AdditionOrderInfo* addinfo
    //string instrumentID = boost::lexical_cast<string>(pDepthMarketData->InstrumentID);
    int frontID;
    int sessionID;
    string orderRef=boost::lexical_cast<string>(userOrderField->orderRef);
    string direction=userOrderField->direction;
    string instrumentID=userOrderField->instrumentID;
    string offsetFlag=userOrderField->offsetFlag;
    string hedgeFlag=userOrderField->hedgeFlag;
    double orderInsertPrice=userOrderField->orderInsertPrice;
    int volume=userOrderField->volume;
    string mkType=userOrderField->mkType;
    //string orderType=userOrderField->orderType;//addition info
    //unsigned int clientOrderToken;
    string function;//addition info
    string timeFlag="0";
    string investorID=userOrderField->investorID;
    string brokerID=userOrderField->brokerID;
    //unsigned int newOrderToken = orderRef;
    unsigned int requestID = userOrderField->requestID;
    //double price = orderInsertPrice;
    char orderPriceType[2]="";
    strcpy(orderPriceType,userOrderField->orderPriceType);
    string orderType = "-1";
    if(direction == "0"){
        //wait for trade,used for orderAction
        OrderInfo* orderInfo = new OrderInfo();
        orderInfo->investorID = investorID;
        orderInfo->orderRef = orderRef;
        orderType = "0";
        if(offsetFlag == "1"){//choose close today or close yestoday
            orderType = "0" + offsetFlag;
            offsetFlag = getCloseMethod(instrumentID,"sell");
            DLOG(INFO) << "buy close,orderType=" + orderType;
        }else{
            DLOG(INFO) << "buy open,orderType=" + orderType;
        }
        orderInfo->offsetFlag = offsetFlag;
        orderInfo->direction = direction;
        orderInfo->price = orderInsertPrice;
        orderInfo->orderType = orderType;
        orderInfo->volume = volume;
        orderInfo->instrumentID = instrumentID;
        //orderInfo->begin_down_cul = down_culculate;
        //orderInfo->mkType = mkType;
        //orderInfo->function = addinfo->function;
        bidList.emplace_back(orderInfo);
        //DLOG(INFO) << "down_culculate=" + boost::lexical_cast<string>(down_culculate) + ",up_culculate=" + boost::lexical_cast<string>(up_culculate) + ",buy open.";
        DLOG(INFO) << "new add bid list." + getOrderInfo(orderInfo);
    }else{
        //wait for trade,used for orderAction
        OrderInfo* orderInfo = new OrderInfo();
        orderInfo->investorID = investorID;
        orderInfo->orderRef = orderRef;
        orderInfo->direction = direction;
        orderType = "1";
        if(offsetFlag == "1"){//choose close today or close yestoday
            orderType = "1" + offsetFlag;
            offsetFlag = getCloseMethod(instrumentID,"buy");

            DLOG(INFO) << "sell close,orderType=" + orderType;
        }else{
            DLOG(INFO) << "sell open,orderType=" + orderType;
        }
        //m_side = changeSignalFromNormalToSL(direction,offsetFlag);
        //orderInfo->m_Side = m_side;
        orderInfo->offsetFlag = offsetFlag;
        orderInfo->price = orderInsertPrice;
        orderInfo->orderType = orderType;
        orderInfo->volume = volume;
        orderInfo->instrumentID = instrumentID;
        //orderInfo->begin_up_cul = up_culculate;
        //orderInfo->mkType = mkType;
        //orderInfo->function = addinfo->function;
        askList.emplace_back(orderInfo);
        //DLOG(INFO) << "down_culculate=" + boost::lexical_cast<string>(down_culculate) + ",up_culculate=" + boost::lexical_cast<string>(up_culculate) + ",sell open.";
        DLOG(INFO) << "new add ask list." + getOrderInfo(orderInfo);
    }
    //CTP
    //报单结构体
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    //EES_EnterOrderField orderField;
    //memset(&orderField, 0, sizeof(EES_EnterOrderField));
    if(mkType=="pas" || mkType=="0"){
        ///有效期类型: 当日有效
        req.TimeCondition = THOST_FTDC_TC_GFD;
        //orderField.m_Tif = EES_OrderTif_Day;//立即单 EES_OrderTif_Day  EES_OrderTif_IOC
    }else if(mkType == "agg"){
        ///有效期类型: 当日有效
        req.TimeCondition = THOST_FTDC_TC_IOC;
        //orderField.m_Tif = EES_OrderTif_IOC;//立即单 EES_OrderTif_Day  EES_OrderTif_IOC
    }/*
    orderField.m_HedgeFlag = EES_HedgeFlag_Speculation;////组合投机套保标 投机 '1'套保 '3'
    strcpy(orderField.m_Account, INVESTOR_ID.c_str());
    strcpy(orderField.m_Symbol, instrumentID.c_str());//modify 2.
    orderField.m_Side = m_side;//modify 1.buy open
    orderField.m_Exchange = EES_ExchangeID_shfe;
    orderField.m_SecType = EES_SecType_fut;
    orderField.m_Price = price;//modify 3.
    orderField.m_Qty = volume;
    orderField.m_ClientOrderToken = newOrderToken;*/
    ///经纪公司代码
    strcpy(req.BrokerID, brokerID.c_str());
    ///投资者代码
    strcpy(req.InvestorID, investorID.c_str());
    ///合约代码
    strcpy(req.InstrumentID, instrumentID.c_str());
    ///报单引用
    strcpy(req.OrderRef, orderRef.c_str());
    req.RequestID = requestID;
    ///用户代码
//	TThostFtdcUserIDType	UserID;
    ///报单价格条件: 限价
    //req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    req.OrderPriceType  = orderPriceType[0];
    ///买卖方向:
    //char tmpdir[2]="0";
    //strcpy(tmpdir,direction.c_str());
    req.Direction=direction.c_str()[0];
    ///组合开平标志: 开仓
    req.CombOffsetFlag[0]=offsetFlag.c_str()[0];
    //strcpy(req.CombOffsetFlag[0],offsetFlag.c_str());
    ///组合投机套保标志
    req.CombHedgeFlag[0]=hedgeFlag.c_str()[0];
    //strcpy(req.CombHedgeFlag[0],hedgeFlag.c_str());
    ///价格
    req.LimitPrice = orderInsertPrice;
    ///数量: 1
    req.VolumeTotalOriginal = volume;
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
//	TThostFtdcPriceType	StopPrice;
    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;
    ///业务单元
//	TThostFtdcBusinessUnitType	BusinessUnit;
    ///请求编号
//	TThostFtdcRequestIDType	RequestID;
    ///用户强评标志: 否
    req.UserForceClose = 0;
    CTPInterface* interface=dii->getTradeApi(dii->loginInvestorID);
    if(interface){
        //interface->pUserApi->(req,pUserApi);
        string msg="addNewOrder:--->>> OK";
        LOG(INFO) <<msg;
    }else{
        string msg="addNewOrder:investorID="+investorID+",can't find tradeApi in tradeApiMap.";
        cerr<<msg<<endl;
        LOG(ERROR) <<msg;
    }
}
/*执行平仓操作，需要查询持仓情况。做出平今平昨决定。只针对上期所品种.
参数：instrumentID 要查询对手的合约
type 持仓类型.执行卖平仓操作时,输入buy,表示要查询多头的持仓情况.执行买平仓操作时,输入sell，表示要查询空头的持仓情况.
返回值:开仓 '0';平仓 '1';平今 '3';平昨 '4';强平 '2'*/
string TradeProcessor::getCloseMethod(string instrumentID, string type) {
    //boost::recursive_mutex::scoped_lock SLock(pst_mtx);
    unordered_map<string, HoldPositionInfo*>::iterator map_iterator = dii->positionmap.find(instrumentID);
    if (map_iterator != dii->positionmap.end()) {//查找到合约持仓
        HoldPositionInfo* hold = map_iterator->second;
        string tmpmsg;
        int realShortPstLimit = hold->shortTotalPosition;
        int realLongPstLimit = hold->longTotalPosition;
        int avlpstLong = hold->longAvaClosePosition;
        int avlpstShort = hold->shortAvaClosePosition;
        int shortYdPst = hold->shortYdPosition;
        int longYdPst = hold->longYdPosition;
        if (type == "buy") {
            //calculate
            int untradeYdVolume = 0;
            int untradeVolume = 0;
            for(list<OrderInfo*>::iterator it = askList.begin();it != askList.end();it++){
                OrderInfo* orderInfo = *it;
                if(orderInfo->direction == "1" && orderInfo->offsetFlag == "4"){//close yesterday,but not traded now
                    untradeYdVolume += orderInfo->volume;
                }
                if(orderInfo->direction == "1"){
                    untradeVolume += orderInfo->volume;
                }
            }
            DLOG(INFO) << "untradeVolume in askList is " + boost::lexical_cast<string>(untradeVolume) + "untradeYdVolume in askList is " + boost::lexical_cast<string>(untradeYdVolume) + ",longYdPst=" + boost::lexical_cast<string>(longYdPst) +
                          ",avlpstLong=" + boost::lexical_cast<string>(avlpstLong);
            if (longYdPst - untradeYdVolume > 0) {
                return "4";//平昨
            } else if (avlpstLong > 0) {
            //} else if (realLongPstLimit > 0) {
                return "3";//平今，上期所品种
            } else {
                LOG(ERROR) << "合约无买持仓数据，无法判断平仓方式.instrumentID=" + instrumentID+",change close to open.";
                return "0";//
                //boost::this_thread::sleep(boost::posix_time::seconds(3));
                //Sleep(1000);
                //return "-1";
            }
        } else if (type == "sell") {
            //calculate
            int untradeYdVolume = 0;
            int untradeVolume = 0;
            for(list<OrderInfo*>::iterator it = bidList.begin();it != bidList.end();it++){
                OrderInfo* orderInfo = *it;
                if(orderInfo->direction == "0" && orderInfo->offsetFlag == "4"){//close yesterday,but not traded now
                    untradeYdVolume += orderInfo->volume;
                }
                if(orderInfo->direction == "0"){
                    untradeVolume += orderInfo->volume;
                }
            }
            DLOG(INFO) << "untradeVolume in bidList is " + boost::lexical_cast<string>(untradeVolume) + "untradeYdVolume in bidList is " + boost::lexical_cast<string>(untradeYdVolume) + ",shortYdPst=" + boost::lexical_cast<string>(shortYdPst) +
                          ",avlpstShort=" + boost::lexical_cast<string>(avlpstShort);
            if (shortYdPst - untradeYdVolume > 0) {
                return "4";//平昨
            } else if (avlpstShort > 0) {
                return "3";//平今，上期所品种
            } else {
                LOG(ERROR) << "合约无卖持仓数据，无法判断平仓方式.instrumentID=" + instrumentID + ",change close to open.";
                return "0";//
                //boost::this_thread::sleep(boost::posix_time::seconds(3));
                //Sleep(1000);
                //return "-1";
            }
        }
    } else {
        LOG(ERROR) << "查找不到合约的持仓信息:instrumentID=" + instrumentID;
    }
}
string TradeProcessor::getOrderInfo(OrderInfo* info){
    string msg = "";
    msg += "direction=" + info->direction + ";";
    msg += "offsetFlag=" + info->offsetFlag + ";";
    msg += "clientOrderToken=" + boost::lexical_cast<string>(info->clientOrderToken) + ";";
    msg += "instrumentID=" + info->instrumentID + ";";
    msg += "function=" + info->function + ";";
    msg += "orderSysID=" + info->orderSysID + ";";
    msg += "mkType=" + info->mkType + ";";
    msg += "orderType=" + info->orderType + ";";
    msg += "price=" + boost::lexical_cast<string>(info->price) + ";";
    msg += "volume=" + boost::lexical_cast<string>(info->volume) + ";";
    return msg;


}
