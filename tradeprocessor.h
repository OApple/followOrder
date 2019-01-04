#ifndef TRADEPROCESSOR_H
#define TRADEPROCESSOR_H
#include <ThostFtdcTraderApi.h>
#include "property.h"
#include "traderspi.h"
class TradeProcessor{
public:
    list<OrderInfo*> bidList;//order list
    list<OrderInfo*> askList;//
    list<OrderInfo*> longList;// trade list
    list<OrderInfo*> shortList;//trade list
    void addNewOrder(UserOrderField*);
    string getCloseMethod(string instrumentID, string type);
    string getOrderInfo(OrderInfo* info);
    //UserAccount* getUserAccount(string);
    //void setUserAccount(UserAccount* ua);
};

#endif // TRADEPROCESSOR_H
