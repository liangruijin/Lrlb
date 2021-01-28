#pragma once

#include "lrlb_reactor.h"
#include "lrlb.pb.h"
#include "route_lb.h"


struct Config{
		int probe_num;
		int init_succ_cnt;
};


extern struct Config lb_config;


//与report_client通信的threadq_queue消息队列
extern thread_queue<lrlb::ReportStatusReq>* report_queue;
//与dns_client通信的thread_queue消息队列
extern thread_queue<lrlb::GetRouteRequest>* dns_queue;



//每个Agent UDP server的 负载均衡器路由 route_lb
extern route_lb * r_lb[3];


//启动udp server服务，用来接受业务层的消息
void start_UDP_servers(void);

//启动lrlb_reporter client线程
void start_report_client(void);

//启动lrlb_dns_client 线程
void start_dns_client(void);














