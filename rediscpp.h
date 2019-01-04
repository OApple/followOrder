#ifndef REDISCPP_H
#define REDISCPP_H

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <hiredis/hiredis.h>
using namespace std;
class Redis
{
public:

    Redis(){}
    Redis(string redis_host,string redis_port,string redis_pwd)
    {
        _redis_host=redis_host;
        _redis_port=redis_port;
        _redis_pwd=redis_pwd;

    }
    ~Redis()
    {
        this->_connect = NULL;
        this->_reply = NULL;
    }

    bool connect(std::string host, int port,string pwd)
    {
        _redis_pwd=pwd;
        _connect = redisConnect(host.c_str(), port);
        if(_connect != NULL && _connect->err)
        {
            printf("connect error: %s\n", _connect->errstr);
            return 0;
        }
        return 1;
    }
    bool connect()
    {
        this->_connect = redisConnect(_redis_host.c_str(), boost::lexical_cast<int>(_redis_port));
        if(this->_connect != NULL && this->_connect->err)
        {
            printf("connect error: %s\n", this->_connect->errstr);
            return false;
        }
        _reply=(redisReply* )redisCommand(_connect,("AUTH "+_redis_pwd).c_str());
        if(_reply&&(!strcasecmp(_reply->str,"OK")))
        {
            freeReplyObject(this->_reply);
            return true;
        }
        //        std::string str = this->_reply->str;
        else
        {
        freeReplyObject(this->_reply);
        return false;
        }
    }
    std::string get(std::string key)
    {
        this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
        std::string str = this->_reply->str;
        freeReplyObject(this->_reply);
        return str;
    }

    void set(std::string key, std::string value)
    {
        redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
    }

private:
    string _redis_host;
    string _redis_port;
    string _redis_pwd;
    redisContext* _connect;
    redisReply* _reply;

};


#endif // REDISCPP_H
