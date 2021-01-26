#include "lrlb_reactor.h"
#include "lrlb.pb.h"
#include "store_report.h"
#include <string>


thread_queue<lrlb::ReportStatusReq> ** reportQueues=NULL;
int thread_cnt=0;
void * store_main(void * args);


void get_report_status(const char*data,uint32_t len,int msgid,net_connection* conn,void * user_data){
	lrlb::ReportStatusReq req;
	req.ParseFromArray(data,len);
	//将上报的数据放在db
	//StoreReport sr;
	//sr.store(req);

	//轮询的方式将消息平均发送到每个线程的消息队列中去
	static int idx=0;
	//将消息发给某个线程消息队列
	reportQueues[idx]->send(req);
	idx++;
	idx=idx%thread_cnt;
}

void create_reportdb_threads(){
	thread_cnt=config_file::instance()->GetNumber("reproter","db_thread_cnt",3);
	
	//开线程池的消息队列
    reportQueues = new thread_queue<lrlb::ReportStatusRequest>*[thread_cnt];
	
	if(reportQueues==NULL){
		fprintf(stderr, "create thread_queue<lrlb::ReportStatusRequest>*[%d], error", thread_cnt) ;
        exit(1);
	}

	for(int i=0;i<thread_cnt;i++){
		//给当前线程创建一个消息队列
		reportQueues[i]=new thread_queue<lrlb::ReportStatusReq>();
		if(reportQueues==NULL){
			fprintf(stderr, "create thread_queue error\n");
            exit(1);
		}
		pthread_t tid;
		int ret=pthread_create(&tid,NULL,store_main,reportQueues[i]);
		if(ret==-1){
			perror("pthread_create");
            exit(1);
		}
		pthread_detach(tid);
	}
}


int main(){
	event_loop loop;
	//加载配置文件
	config_file::setPath("../conf/lrlb_reporter.conf");
	std::string ip=config_file::instance()->GetString("reactor","ip","0.0.0.0");
	short port=config_file::instance()->GetNumber("reactor", "port", 7779);

	//创建tcp server
	tcp_server server(&loop,ip.c_str(),port);

	//添加数据上报请求处理的消息分发处理业务
	server.add_msg_router(lrlb::ID_ReportStatusRequest,get_report_status);
	
	//为了防止再业务中出现Io阻塞，需要启动一个线程池来对IO进行操作，接受业务的请求存储消息
	create_reportdb_threads();
	//启动事件监听
	loop.event_process();


	return 0;
}










