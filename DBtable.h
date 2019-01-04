#ifndef DBTABLE_H
#define DBTABLE_H
#include<mysql++.h>
#include<ssqls.h>
//ctp_trade_record
/*
 *  `id` int(50) NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `instrumentid` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '合约号',
  `instrumentid_ch` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT '' COMMENT '合约中文名称',
  `variety_ch` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT '' COMMENT '品种中文',
  `price` decimal(50,6) DEFAULT NULL COMMENT '交易价格',
  `volume` int(50) DEFAULT NULL COMMENT '交易数量',
  `direction` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '买卖方向（0-Buy-买；1-Sell-卖）',
  `offsetflag` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '开平标志（0-Open-开仓；1-Close-平仓；2-ForceClose-强平；3-CloseToday-平今；4-CloseYesterday-平昨；5-ForceOff-强减；6-LocalForceClose-本地强平；）',
  `brokerid` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '经纪公司代码',
  `exchangeid` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '交易所代码',
  `investorid` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '投资者代码',
  `tradetype` varchar(100) COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT '交易类型（0-Common-普通成交；1-OptionsExecution-期权执行；2-OTC-OTC成交；3-EFPDerived -期转现衍生成交；\r\n4-CombinationDerived- 组合衍生成交；#-SplitCombination-组合持仓拆分为单一持仓,初始化不应包含该类型的持仓；）',
  `tradedatetime` datetime DEFAULT NULL COMMENT '交易日期时间',
  `tradeid` varchar(100) COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '成交编号',
  `tradedatetime_ctp` datetime DEFAULT NULL COMMENT 'ctp返回交易时间',
 */
sql_create_15(ctp_trade_record,
    1, 15 ,// The meaning of these values is covered in the user manua
    mysqlpp::sql_char,tradeid,
    mysqlpp::sql_char, instrumentid,
    mysqlpp::sql_char, instrumentid_ch,
    mysqlpp::sql_char, variety_ch,
    mysqlpp::sql_decimal, price,
    mysqlpp::sql_bigint, volume,
    mysqlpp::sql_char, direction,
    mysqlpp::sql_char, offsetflag,
    mysqlpp::sql_char, brokerid,
    mysqlpp::sql_char, exchangeid,
    mysqlpp::sql_char, investorid,
    mysqlpp::sql_char, tradetype,
    mysqlpp::sql_datetime, tradedatetime,
    mysqlpp::sql_datetime, tradedatetime_ctp,
    mysqlpp::sql_char, feedback)
#endif // DBTABLE_H
