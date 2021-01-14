#pragma once 

#include <pthread.h>
#include "task_msg.h"
#include "thread_queue.h"

class thread_pool
{
public:
	//构造 ，初始化线程池，开辟thread_cnt个
	thread_pool(int thread_cnt);
	//获取一个thread_queue
	thread_queue<task_msg> * get_thread();
	//发送一个task任务给thread_pool里面的全部thread
	void send_task(task_func func,void* args=NULL);
	
private:
	//_queue是但概念thread_pool全部消息任务队列的头指针
	thread_queue<task_msg> **_queues;
	//当前线程的的个数
	int _thread_cnt;
	//已经启动全部thread标号
	pthread_t * _tids;

	//当前选中线程队列下标
	int _index;

};






















