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
	lb_config.err_rate = config_file::instance()->GetFloat("loadbalance", "err_rate", 0.1);
    lb_config.succ_rate = config_file::instance()->GetFloat("loadbalance", "succ_rate", 0.92);
    lb_config.contin_succ_limit = config_file::instance()->GetNumber("loadbalance", "contin_succ_limit", 10);
    lb_config.contin_err_limit = config_file::instance()->GetNumber("loadbalance", "contin_err_limit", 10);
	lb_config.window_err_rate = config_file::instance()->GetFloat("loadbalance", "window_err_rate", 0.7);
    lb_config.idle_timeout = config_file::instance()->GetNumber("loadbalance", "idle_timeout", 15);
    lb_config.overload_timeout = config_file::instance()->GetNumber("loadbalance", "overload_timeout", 15);
	//2.初始化3个route_lb模块
	create_route_lb();

	//3.加载本地ip
	char my_host_name[1024];
	if(gethostname(my_host_name,1024)==0){
		struct hostent *hd=gethostbyname(my_host_name);
		if(hd){
			struct sockaddr_in myaddr;
			myaddr.sin_addr=*(struct in_addr*)hd->h_addr;
			lb_config.local_ip=ntohl(myaddr.sin_addr.s_addr);
		}
	}
	if (!lb_config.local_ip)  {
        struct in_addr inaddr;
        inet_aton("127.0.0.1", &inaddr);
        lb_config.local_ip = ntohl(inaddr.s_addr);
    }
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

