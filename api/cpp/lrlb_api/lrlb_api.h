#pragma once

#include "lrlb_reactor.h"
#include <string>

class lrlb_client{
public:
	lrlb_client();
	~lrlb_client();
	//lrlb 系统获取host信息 ，得到可用host的ip和port
	int get_host(int modid,int cmdid,std::string&ip ,int & port);
	//lrlb 系统上报host调用信息
	void report(int modid,int cmdid,const std::string & ip,int port,int retcode);
private:
	int _sockfd[3]; //3个udp socket fd对应的agent 3个udp server
	uint32_t _seqid;  //消息的序列号
};





