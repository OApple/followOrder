#include "user_order_field.h"
#include "traderspi.h"
using boost::locale::conv::between;
using boost::lexical_cast;
using boost::split;
using boost::is_any_of;
using boost::algorithm::trim_copy;

UserOrderField*UserOrderField::CreateUserOrderField(CThostFtdcOrderField *pOrder,CTraderSpi*uai)
{

    vector<string> tmp;
    split(tmp,uai->ratio,is_any_of(":"));
    int n1=lexical_cast<int>(tmp[1]);
    int n2=lexical_cast<int>(tmp[0]);
    int totalVolume=(pOrder->VolumeTotalOriginal*n1/n2);
    UserOrderField* userOrderField = new UserOrderField();
    userOrderField->brokerID=uai->_brokerID;
    userOrderField->_direction=pOrder->Direction;
    userOrderField->NOrderSysID=trim_copy(string(pOrder->OrderSysID));
    userOrderField->NinvestorID=pOrder->InvestorID;
    userOrderField->_frontID=uai->frontID;
    userOrderField->_sessionID=uai->sessionID;
    userOrderField->_hedge_flag=pOrder->CombHedgeFlag;
    userOrderField->_instrumentID=pOrder->InstrumentID;
    userOrderField->_investorID=uai->investorID();
    userOrderField->_tick=uai->followTick;
    userOrderField->_offset_flag=lexical_cast<string>(pOrder->CombOffsetFlag);
    if(uai->priceType==1)// nman price
    {
        userOrderField->_price=pOrder->LimitPrice;
        userOrderField->orderPriceType="2";
    }
    if(uai->priceType==2)//market price
    {
        userOrderField->_price=0;
        userOrderField->orderPriceType="1";
    }

    userOrderField->_order_ref=uai->orderRef++;
    userOrderField->_volume=totalVolume;
    userOrderField->_pTraderSpi=uai;
    userOrderField->_pUserApi=uai->pUserApi;
    userOrderField->_status='z';
    userOrderField->_key=userOrderField->NinvestorID+userOrderField->NOrderSysID+userOrderField->_investorID;
    userOrderField->_key2=lexical_cast<string>(userOrderField->_investorID)+lexical_cast<string>(userOrderField->_order_ref);
    return userOrderField;
}

int  UserOrderField:: ReqOrderInsert()
{
    CTraderSpi* pTraderSpi= (CTraderSpi*)  _pTraderSpi;
    int ret=pTraderSpi->ReqOrderInsert(this);
    return ret;
    //    ChkThread*ct=  ChkThread::GetInstance();

}

int UserOrderField:: ReqOrderAction()
{
    CTraderSpi* pTraderSpi= (CTraderSpi*) _pTraderSpi;
    int ret=pTraderSpi->ReqOrderAction(this);
    return ret;
}

void UserOrderField::SetStatus(char status)
{
    _status=status;
}

char UserOrderField::GetStatus()
{
    return _status;
}
int UserOrderField:: UpdateRef()
{
    CTraderSpi* pTraderSpi= (CTraderSpi*) _pTraderSpi;
    _order_ref=pTraderSpi->orderRef++;
    _key2=lexical_cast<string>(_investorID)+lexical_cast<string>(_order_ref);

}
void UserOrderField::UpdatePrice()
{
    if(_direction=='0')//buy
    {
        _price+=_tick;
    }
    else//sell
    {
        _price-=_tick;
    }

}
string UserOrderField::GetKey()
{
    return _key;
}
string UserOrderField:: GetKey2()
{
    return _key2;
}

string GetKey(CThostFtdcOrderField *pOrder,CTraderSpi*uai)
{
    string key="";
    key=pOrder->InvestorID+trim_copy(string(pOrder->OrderSysID))+uai->investorID();
    return key;
}
string GetKey2(CThostFtdcOrderField *pOrder)
{
    string key2="";
    key2=lexical_cast<string>(pOrder->InvestorID) +trim_copy(string(pOrder->OrderRef));
    return key2;
}
string GetKey2(CThostFtdcInputOrderField *pInputOrder)
{
    string key2="";
    key2=lexical_cast<string>(pInputOrder->InvestorID) +trim_copy(string(pInputOrder->OrderRef));
    return key2;

}
