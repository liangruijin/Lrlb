#include "lrlb_reactor.h"
#include "main_server.h"

static void get_host_cb(const char*data,uint32_t len,int msgid,net_connection* net_conn,void* user_data)
{
	printf("get_host_cb is called ...\n");
	//解析api发来的包
	lrlb::GetHostRequest req;
	req.ParseFromArray(data,len);
	int modid=req.modid();
	int cmdid=req.cmdid();
	//设置回执信息
	lrlb::GetHostResponse rsp;
	rsp.set_seq(rsp.seq());
	rsp.set_modid(modid);
	rsp.set_cmdid(cmdid);

	route_lb* ptr_route_lb=(route_lb*) user_data;
	//调用route_lb获取host方法，得到rsp返回结果
	ptr_route_lb->get_host(modid,cmdid,rsp);
	
	//打包发回给api
	std::string responseString; 
    rsp.SerializeToString(&responseString);
    net_conn->send_message(responseString.c_str(), responseString.size(), lrlb::ID_GetHostResponse);
    printf("rspstring size = %d\n", responseString.size());
}

void * agent_server_main(void * args){
	long index=(long) args;

	short port=index +8888;
	event_loop loop;
	udp_server server(&loop,"0.0.0.0",port);

	//todo 给server注册消息分发路由器
	server.add_msg_router(lrlb::ID_GetHostRequest,get_host_cb,r_lb[port-8888]);
	
	printf("agent UDP server:port%d is started ...\n",port);

	loop.event_process();
	return NULL;
}

void start_UDP_servers(void){
	
	for(int i=0;i<3;i++){
		pthread_t tid;
		//这里把i传进去会不会有竞争的关系呀，三个服务器的i都是相同的
		int ret=pthread_create(&tid,NULL,agent_server_main,(void *)i);
		if(ret==-1){
			perror("pthread_create");
			exit(1);
		}
		pthread_detach(tid);
	}
}




