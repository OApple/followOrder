#ifndef TIMEPROCESSER_H
#define TIMEPROCESSER_H
#pragma once
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <mutex>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include "property.h"


using namespace std;


#define BOOST_DATE_TIME_SOURCE
/*获取时间函数*/
boost::posix_time::ptime getCurrentTimeByBoost();
/*字符串转换成boost时间*/
boost::posix_time::ptime changeStringToTime(string strTime);
/*计算时间间隔
返回秒数
type: t,返回至以微妙为单位;s,返回值以秒为单位
*/
int getTimeInterval(boost::posix_time::ptime begin, boost::posix_time::ptime end,string type);
/*根据行情计算各项指标*/
//void metricProcesser();
//void ttt();
#endif // TIMEPROCESSER_H
