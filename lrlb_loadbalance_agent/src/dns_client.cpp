#include "lrlb_reactor.h"
#include "main_server.h"
#include <pthread.h>

static void conn_init(net_connection* conn,void *args){
	for(int i=0;i<3;i++){
		r_lb[i]->reset_lb_status();
	}
}


void new_dns_request(event_loop * loop,int fd,void * args){
	tcp_client* client=(tcp_client*) args;
	//1.将数据从thread_queue中取出
	std::queue<lrlb::GetRouteRequest> msgs;
	//2.将数据放在queue队列中
	dns_queue->recv(msgs);
	//3。遍历都队列，将clientyjcijiang miege msg发送给report server;
	while(msgs.size()){
		lrlb::GetRouteRequest req=msgs.front();
		msgs.pop();
		std::string requestString;
        req.SerializeToString(&requestString);
		//client发送数据
		client->send_message(requestString.c_str(),requestString.size(),lrlb::ID_GetRouteRequest);
	}
}


/*
 * 处理远程dns service回复的modid/cmdid对应的路由信息
 * */
void deal_recv_route(const char*data,uint32_t len,int msgid,net_connection* net_conn,void * usr_data){
	lrlb::GetRouteResponse rsp;
	//解析远程消息到proto结构中
	rsp.ParseFromArray(data,len);
	int modid=rsp.modid();
	int cmdid=rsp.cmdid();
	int index=(modid+cmdid)%3;
	//将该modid/cmdid交给一个route_lb处理，将rsp中的hostinfo集合加入到对应的route_lb中
	r_lb[index]->update_host(modid,cmdid,rsp);
} 


void * dns_client_thread(void* args)
{
	printf("dns client thread start\n");
#if 1
	event_loop loop;
	//1.加载配置文件到dns ip+port
	std::string ip=config_file::instance()->GetString("dnssever","ip","127.0.0.1");
	short port=config_file::instance()->GetNumber("dnssever","port",7778);

	//2.创建客户端
	tcp_client client(&loop,ip.c_str(),port,"dns client");

	//3.将thread_queue消息回调事件绑定到Loop中
	dns_queue->set_loop(&loop);
	dns_queue->set_callback(new_dns_request,&client);

	//4.设置当前收到dns service回执的消息ID_GetRouteResponse处理函数
	client.add_msg_router(lrlb::ID_GetRouteResponse,deal_recv_route);

	//5.设置连接成功/连接断开重连成功之后，通过conn_init来清理之前的任务
	client.set_conn_start(conn_init);
	//6.启动事件监听
	loop.event_process();
#endif
	return NULL;
}


void start_dns_client()
{
    //开辟一个线程
    pthread_t tid;

    //启动线程业务函数
    int ret = pthread_create(&tid, NULL, dns_client_thread, NULL);
    if (ret == -1) {
        perror("pthread_create");
        exit(1);
    }

    //设置分离模式
    pthread_detach(tid);
}

