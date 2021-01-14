#include "thread_pool.h"
#include "event_loop.h"
#include "tcp_conn.h"
#include <unistd.h>
#include <stdio.h>

//一旦有task过来，这个业务就是出席task业务的主流程
//只要有人调用thread_queue::send()方法就会触发此函数

void deal_task_message(event_loop * loop,int fd,void * args)
{
	//得到当前的消息队列，肯定是当前线程（包含这个消息队列的线程）拿到的这个线程
	thread_queue<task_msg>* queue=(thread_queue<task_msg>*) args;
	//将queue中的全部任务取出来
	std::queue<task_msg> tasks;
	queue->recv(tasks); //这一步不知道会不会有问题，因为我们的task在栈上，好像不会有问题，因为我们的当前的栈可以向上伸展
	//而且在thead里面的queue也不会出问题，因为那是在堆上的，当数据变大的时候会重新分配空间的

	while(!tasks.empty()){
		task_msg task=tasks.front();
		//弹出一个元素
		tasks.pop();
		if(task.type==task_msg::NEW_CONN){
			//是一个新建的连接任务 
			
			//并且这个tcp_conn加入到当前线程的loop中去监听
			//此后这个这个tcp_conn的消息都由这个线程的loop进行监听和负责读写。主线程已经不管它了。
			tcp_conn* conn=new tcp_conn(task.connfd,loop);
			if(conn==NULL){
				fprintf(stderr, "in thread new tcp_conn error\n");
                exit(1);
			}
			printf("[Thread]: get new connection succ!\n");
		}

		else if(task.type==task_msg::NEW_TASK){
			//=======是一个新的普通任务======
			//当前的Loop就是一个thread的事件监控loop,让当前的loop触发task任务的回调
			loop->add_task(task.task_cb,task.args);
			//================================================
		}
		else {
			//其他未识别的任务
			
		}
		
	}
	
}

void * thread_main(void * args){
	thread_queue<task_msg> * queue=(thread_queue<task_msg>*) args;
	//每个线程都应该有一个evetn_loop来监控客户端连接的读写事件
	event_loop* loop=new event_loop();
	if(loop==NULL) {
		fprintf(stderr,"new event_loop error\n");
		exit(1);
	}
	//注册一个触发消息任务读写的callback函数
	queue->set_loop(loop);
	queue->set_callback(deal_task_message,queue);
	//启动监听
	loop->event_process();
	return NULL;
}


thread_pool::thread_pool(int thread_cnt){
	_index=0;
	_queues=NULL;
	_thread_cnt=thread_cnt;
	if(_thread_cnt<=0) {
		fprintf(stderr, "_thread_cnt < 0\n");
        exit(1);
	}
	//任务队列的个数和线程个数一致
	_queues=new thread_queue<task_msg>*[thread_cnt];
	_tids= new pthread_t[thread_cnt];

	int ret;
	for(int i=0;i<thread_cnt;i++){
		//创建线程
		printf("create %d thread\n",i);
		//给当前线程创建一个消息队列
		_queues[i]=new thread_queue<task_msg>();
		ret=pthread_create(&_tids[i],NULL,thread_main,_queues[i]);
		if(ret==-1){
			perror("thread_pool, create thread");
            exit(1);
		}
		//将线程脱离
		pthread_detach(_tids[i]);
	}
	
}


thread_queue<task_msg>* thread_pool::get_thread()
{
	if(_index==_thread_cnt){
		_index=0;
	}
	
	return _queues[_index++];
}


void thread_pool::send_task(task_func func,void * args){
	task_msg task;
	//封装一个task消息
		task.type=task_msg::NEW_TASK;
		task.task_cb=func;
		task.args=args;
	//给当前thread_pool里面的每个thread里面的pool添加一个task任务
	for(int i=0;i<_thread_cnt;i++){
		//取出第i个thread的消息队列
		thread_queue<task_msg> * queue=_queues[i];
		//发送task消息
		queue->send(task);
	}
}

