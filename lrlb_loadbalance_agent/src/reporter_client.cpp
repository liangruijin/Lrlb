#include "lrlb_reactor.h"
#include "main_server.h"
#include <string>
#include <pthread.h>


void new_report_request(event_loop * loop,int fd,void * args){
	tcp_client* client=(tcp_client*) args;
	//1.将数据从thread_queue中取出
	std::queue<lrlb::ReportStatusReq> msgs;
	//2.将数据放在queue队列中
	report_queue->recv(msgs);
	//3。遍历都队列，将clientyjcijiang miege msg发送给report;
	while(msgs.size()){
		lrlb::ReportStatusReq req=msgs.front();
		msgs.pop();
		std::string requestString;
        req.SerializeToString(&requestString);
		//client发送数据
		client->send_message(requestString.c_str(),requestString.size(),lrlb::ID_ReportStatusRequest);
	}
}

void * report_client_thread(void * args){
	printf("report client thread start\n");
#if 1
	event_loop loop;
	//1.加载配置文件到reporter ip+port
	std::string ip=config_file::instance()->GetString("reporter","ip","127.0.0.1");
	short port=config_file::instance()->GetNumber("reporter","port",0);

	//2.创建客户端
	tcp_client client(&loop,ip.c_str(),port,"reporter client");

	//3.将thread_queue消息回调事件绑定到Loop中
	report_queue->set_loop(&loop);
	report_queue->set_callback(new_report_request,&client); 

	//4.启动事件监听
	loop.event_process();
#endif
	return NULL;
}


void start_report_client(void){
	//开辟一个线程
	pthread_t tid;
	int ret=pthread_create(&tid,NULL,report_client_thread,NULL);
	if(ret==-1){
		perror("pthread_create");
        exit(1);
	}
	//设置分离模式
	pthread_detach(tid);
}