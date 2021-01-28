#include "main_server.h"
#include "lrlb.pb.h"


//--------- 全局资源 ----------
struct Config lb_config;


//与report_client通信的thread_queue消息队列
thread_queue<lrlb::ReportStatusReq>* report_queue = NULL;
//与dns_client通信的thread_queue消息队列
thread_queue<lrlb::GetRouteRequest>* dns_queue=NULL;

//每个Agent UDP server的 负载均衡器路由 route_lb
route_lb * r_lb[3];


static void create_route_lb(){
	for(int i=0;i<3;i++){
		int id=i+1;
		r_lb[i]=new route_lb(id);
		if(r_lb[i]==NULL){
			fprintf(stderr, "no more space to new route_lb\n");
            exit(1);
		}
	}
}

static void init_lb_agent(){
	//1加载配置文件
	config_file::setPath("../conf/lrlb_lb_agent.conf");
	lb_config.probe_num=config_file::instance()->GetNumber("loadbalance","probe_num",10);
	lb_config.init_succ_cnt=config_file::instance()->GetNumber("loadbalance","init_succ_cnt",180);
	//2.初始化3个route_lb模块
	create_route_lb();
}

int main(){
	
	//1初始化环境
	init_lb_agent();
	//1.5初始化loadbalance服务器
	
	//2.启动udp server服务 ，用来接受业务
	start_UDP_servers();

	//3.启动lrlb_reportr client线程
	report_queue =new thread_queue<lrlb::ReportStatusReq>();
	if(report_queue==NULL){
		fprintf(stderr, "create report queue error!\n");
        exit(1);
	}
	start_report_client();

	//4 启动lrlb_dns client线程
	dns_queue=new thread_queue<lrlb::GetRouteRequest>();
	if (dns_queue == NULL) {
        fprintf(stderr, "create dns queue error!\n");
        exit(1);
    }
	start_dns_client();

	printf("done!\n");
	while(1) sleep(10);

	return 0;
	
}

