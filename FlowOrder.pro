TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
TARGET = followOrder
QMAKE_CXXFLAGS += -Wno-unused-parameter

QMAKE_CXXFLAGS += -Wno-unused-variable

SOURCES += main.cpp \
    util.cpp \
    dataprocessor.cpp \
    traderspi.cpp \
    chkthread.cpp \
    niutraderspi.cpp \
    user_order_field.cpp


HEADERS += \
    property.h \
    mysqlconnectpool.h \
    rediscpp.h \
    util.h \
    dataprocessor.h \
    DBtable.h \
    traderspi.h \
    chkthread.h \
    niutraderspi.h \
    user_order_field.h


LIBS += -L$$PWD/../lib/   -lthostmduserapi  -lthosttraderapi

INCLUDEPATH += $$PWD/../include


#LIBS += -L$$PWD/../glog_0_3_3/ -lglog
#INCLUDEPATH += $$PWD/../glog_0_3_3
LIBS += -lglog
INCLUDEPATH += $$PWD/../SQLiteCpp/include


LIBS += -L$$PWD/../boost_1_61_0/stage/lib/ -lboost_system -lboost_date_time -lboost_system -lboost_thread -lpthread -lboost_chrono  -lboost_locale

INCLUDEPATH += $$PWD/../boost_1_61_0/


LIBS +=  -licui18n -licuuc -licudata  -ldl -lhiredis  -lSQLiteCpp -L/usr/local/lib -lsqlite3

OTHER_FILES += \
    ../build-FlowOrder-gcc5_4_0-Debug/config/global.properties


unix:!macx: LIBS += -L/usr/lib64/mysql/ -lmysqlclient

unix:!macx: LIBS += -L/usr/local/lib -lmysqlpp

INCLUDEPATH += /usr/local/include/mysql++ /usr/include/mysql

DEPENDPATH += /usr/local/include/mysql++
