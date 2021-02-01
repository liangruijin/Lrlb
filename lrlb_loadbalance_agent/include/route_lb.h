#pragma once

#include "load_balance.h"

//key modid+cmdid  value :loadbalance
typedef std::unordered_map<uint64_t,load_balance*> route_map;
typedef std::unordered_map<uint64_t,load_balance*>::iterator  route_map_it;

/*
*
*针对多组modid/cmdid，route_lb是管理load_balance模块的
*目前设计为3个，和udp_server数量一致 
*每个route_lb根据modid/cmdid的值做hash,分管不同modid/cmdid
*/

class route_lb{
public:
	route_lb(int id);
	//agent获取一个host主机，将返回的主机结果保存到rsp中
	int get_host(int modid,int cmdid,lrlb::GetHostResponse & rsp);
	//根据DNS server返回的结果更新自己的route_lb_map
	int update_host(int modid,int cmdid,lrlb::GetRouteResponse &rsp);
	//agent 上报某主机获取结果
	void report_host(lrlb::ReportRequest req);
private:
	route_map _route_lb_map;  //当前route_lb下管理的Loadbalance
	pthread_mutex_t _mutex;
	int _id; //当前route_lb的ID编号
	
};

