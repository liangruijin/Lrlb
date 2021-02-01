#pragma once

#include "lrlb_reactor.h"
#include "lrlb.pb.h"
#include "route_lb.h"


struct Config{
	int probe_num;
	int init_succ_cnt;
	//当idle节点失败率高于此值，节点变overload状态
    float err_rate;

    //当overload节点成功率高于此值，节点变成idle状态
    float succ_rate;

    //当idle节点连续失败次数超过此值，节点变成overload状态
    int contin_err_limit;

    //当overload节点连续成功次数超过此值, 节点变成idle状态
    int contin_succ_limit;
	
	//当前agent本地ip地址(用于上报 填充caller字段)
    uint32_t local_ip;

	//整个窗口的真实失败率阈值
    float window_err_rate;

    //对于某个modid/cmdid下的某个idle状态的host，需要清理一次负载信息的周期
    int idle_timeout;

    //对于某个modid/cmdid/下的某个overload状态的host，在过载队列等待的最大时间
    int overload_timeout;
		
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














