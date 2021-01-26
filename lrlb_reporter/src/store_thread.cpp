#include "lrlb.pb.h"
#include "lrlb_reactor.h"
#include "store_report.h"
#include <queue>


struct Args
{
	thread_queue<lrlb::ReportStatusReq>* first;
	StoreReport * second;
};

void thread_report(event_loop * loop,int fd,void *args){
	//1.从queue中取出需要的report数据
	thread_queue<lrlb::ReportStatusReq>* queue=((Args*)args)->first;
	StoreReport* sr=((Args*)args)->second;

	std::queue<lrlb::ReportStatusReq> report_msgs;

	//1.1从消息队列中取出全部的消息元素集合
	queue->recv(report_msgs);

	while(!report_msgs.empty()){
		lrlb::ReportStatusReq msg=report_msgs.front();
		report_msgs.pop();
		//2.将数据存到DB中
		sr->store(msg);
	}
}

void * store_main(void * args){
	//得到对应的thread_queue;
	thread_queue<lrlb::ReportStatusReq>* queue=(thread_queue<lrlb::ReportStatusReq>*) args;
	//定义时间触发机制
	event_loop loop;

	//定义一个存储对象
	StoreReport sr;
	Args callback_args;
	callback_args.first=queue;
	callback_args.second=&sr;

	queue->set_loop(&loop);
	queue->set_callback(thread_report,&callback_args);

	//启动事件监听
	loop.event_process();
	return NULL;
}




