#pragma once

#include <queue>
#include <pthread.h>
#include <sys/eventfd.h>
#include <stdio.h>
#include <unistd.h>
#include "event_loop.h"

//每个thread都对应着一个 消息队列 
//其实这个 挺好的 没有写死，我们取出队列的后续操作都看回调函数怎样 去处理queue里面的数据

template <typename T>
class thread_queue{
public:
	thread_queue(){
		_loop=NULL;
		pthread_mutex_init(&_queue_mutex,NULL);
		_evfd=eventfd(0,EFD_NONBLOCK);
		if (_evfd == -1) {
            perror("evenfd(0, EFD_NONBLOCK)");
            exit(1);
        }
	}

	~thread_queue(){
		pthread_mutex_destroy(&_queue_mutex);
		close(_evfd);
	}

	//向队列里面添加任务
	void send(const T& task){
		//触发消息事件的占位传输内容,往_evfd里面写
		unsigned long long idle_num=1;  
		pthread_mutex_lock(&_queue_mutex);
		//将任务添加到队列里面
		_queue.push(task);

		//向_evfd写，触发对应的EPOLLIN事件，来处理该任务
		int ret=write(_evfd,&idle_num,sizeof(unsigned long long));
		if(ret==-1){
			perror("_evfd write");
		}
		pthread_mutex_unlock(&_queue_mutex);
	}

	//获取队列，（当前队列里面已经有任务）
	//传入一个新的队列，然后替换出已经有任务的队列
	void recv(std::queue<T>& new_queue){
		unsigned int long long idle_num=1;
		pthread_mutex_lock(&_queue_mutex);
		//把占位的数据读出来，确保底层缓冲没有数据存留
        int ret = read(_evfd, &idle_num, sizeof(unsigned long long));
		if (ret == -1) {
			perror("_evfd read");
		}
		
		//将当前的队列拷贝出去,将一个空队列换回当前队列,同时清空自身队列，确保new_queue是空队列
		//感觉这里换成指针换入换出是不是效率要高一些哦
		std::swap(new_queue,_queue);
		pthread_mutex_unlock(&_queue_mutex);
		
	}

	void set_loop(event_loop * loop){
		
			_loop=loop;
		
	}
	//设置当前的消息队列的 每个任务触发的回调业务
 	void  set_callback(io_callback * cb,void* args =NULL){
 		if(_loop!=NULL){
			_loop->add_io_event(_evfd,cb,EPOLLIN,args);
		}
	}
	//得到当前的loop
	event_loop * get_loop(){
		return _loop;
	}
	


private:
	int _evfd;  //促发消息队列读取的每个消息业务的fd
	//  eventfd是Linux 2.6提供的一种系统调用，它可以用来实现事件通知。
	
	event_loop * _loop;  //当前消息任务队列绑定在那个event_loop事件触发机制中
	std::queue<T> _queue; //队列结构
	pthread_mutex_t _queue_mutex;
};






















