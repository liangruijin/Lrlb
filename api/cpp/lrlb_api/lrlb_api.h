#pragma once

#include "lrlb_reactor.h"
#include <string>
#include <vector>

typedef std::pair<std::string,int> ip_port;
typedef std::vector<ip_port> route_set;
typedef route_set::iterator route_set_it;

class lrlb_client{
public:
	lrlb_client();
	~lrlb_client();
	//lrlb 系统获取host信息 ，得到可用host的ip和port
	int get_host(int modid,int cmdid,std::string&ip ,int & port);
	//lrlb 系统上报host调用信息
	void report(int modid,int cmdid,const std::string & ip,int port,int retcode);
	//系统获取某modid/cmdid全部的hosts(route)信息
	int  get_route(int modid, int cmdid, route_set&route);
	int reg_init(int modid,int cmdid);
private:
	int _sockfd[3]; //3个udp socket fd对应的agent 3个udp server
	uint32_t _seqid;  //消息的序列号
};





