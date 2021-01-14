#pragma once 

#include <sys/epoll.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "event_base.h"

#define MAXEVENTS 10

//MAP : FD------>io_event
typedef std::unordered_map<int,io_event> io_event_map;

//SET : all fd which is listening
typedef std::unordered_set<int> listen_fd_set;
//这个迭代器后期感觉可以取消
typedef std::unordered_map<int,io_event>::iterator io_event_map_it;

//定义异步任务回调函数类型
typedef void (*task_func) (event_loop * loop,void * args);

class event_loop
{
public:
	event_loop();
	//处理我们的事件 ，这里会阻塞我们的loop
	void event_process();
	//添加事件到我们监听中
	void add_io_event(int fd,io_callback* proc,int mask,void * args=NULL);

	//从loop中删除一个事件
	void del_io_event(int fd);
	//删除一个io事件的EPOLLIN/EPOLLOUT   
	void del_io_event(int fd,int mask);

	//====================================
	//获得全部监听事件的fd集合
	void get_listen_fds(listen_fd_set & fds){
		fds=listen_fds;
	}

	
	//=======异步任务task模块需要的方法=======
	//添加一个任务到ready_tasks集合中
	void add_task(task_func func,void * args);
	//执行全部ready_tasks里面的任务
	void execute_ready_tasks();
	//====================================

private:
	int _epfd; //原生堆监听的fd

	io_event_map _io_evs;

	listen_fd_set listen_fds;

	struct epoll_event _fired_evs[MAXEVENTS]; //一次最大的处理事件

	//需要被执行的task集合
	typedef std::pair<task_func,void*> task_func_pair;
	std::vector<task_func_pair> _ready_tasks;
	
	
};


