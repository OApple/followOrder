#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <ThostFtdcTraderApi.h>

#include "traderspi.h"
#include "dataprocessor.h"
#include "tradeprocessor.h"
#include "property.h"


using namespace std;
using boost::locale::conv::between;
using boost::lexical_cast;
using boost::split;
using boost::is_any_of;

string strReqUserLoginField(CThostFtdcReqUserLoginField*req);

string strRspUserLoginField(CThostFtdcRspUserLoginField *pRsp);

string strRspInfoField(CThostFtdcRspInfoField *pRspInfo);

string strQryTradingAccountField(CThostFtdcQryTradingAccountField*qry);

string strOrderActionField(CThostFtdcOrderActionField *pOrderAction);

string strInputOrderActionField(CThostFtdcInputOrderActionField *order);

string strTradingAccountField(CThostFtdcTradingAccountField *pTradingAccount);

string strInvestorPositionField(CThostFtdcInvestorPositionField *pInvestorPosition);

//提取投资者报单信息
string strInputOrderField(CThostFtdcInputOrderField *order);

string strOrderField(CThostFtdcOrderField *order);


string strTradeField(CThostFtdcTradeField *order);

CThostFtdcInputOrderField assamble(CThostFtdcTradeField *pTrade);

//将投资者对冲报单信息写入文件保存
void saveInvestorOrderInsertHedge(CThostFtdcInputOrderField *order,string filepath);

//保存报单回报信息
void saveRspOrderInsertInfo(CThostFtdcInputOrderField *pInputOrder);

//将交易所报单回报响应写入文件保存
//void saveRtnOrder(CThostFtdcOrderField *pOrder);


//提取投资者报单信息
string getOrderActionInfoByDelimater(CThostFtdcInputOrderActionField *order);

//提取成交回报信息

//获取交易所响应信息
string getRtnOrder(CThostFtdcOrderField *pOrder);
#endif // UTIL_H
