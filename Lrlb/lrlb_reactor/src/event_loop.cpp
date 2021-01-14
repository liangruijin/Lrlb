#include "event_loop.h"
#include <assert.h>


event_loop::event_loop(){
	_epfd=epoll_create1(0);
	if(_epfd==-1){
		fprintf(stderr,"epoll_create error\n");
		exit(1);
	}
}


void event_loop::event_process(){
	while(true){
		io_event_map_it ev_it;

		int nfds=epoll_wait(_epfd,_fired_evs,MAXEVENTS,10);
		for(int i=0;i<nfds;i++){
			ev_it= _io_evs.find(_fired_evs[i].data.fd); //这一步肯定是能找的，但是找到能干嘛呢？
			assert(ev_it!=_io_evs.end());

			io_event* ev=&(ev_it->second);
			//为什么要这样写呢？因为如果我们不建个map，那么我们只知道
			//这个fd发生了in or out ,并且其callback是什么呢。
			//一般的服务器就是监听到了事件了，直接传到一个统一的处理函数中处理
			//注意data.ptr这个指针还可以放一些东西

			if(_fired_evs[i].events&EPOLLIN){
				//读事件，调用我们的读回调函数
				void* args=ev->rcb_args;
				ev->read_callback(this,_fired_evs[i].data.fd,args);
			}
			else if(_fired_evs[i].events&EPOLLOUT){
				//写事件，调用我们的写回调函数
				void* args=ev->wcb_args;
				ev->write_callback(this,_fired_evs[i].data.fd,args);
			}
			else if(_fired_evs[i].events &(EPOLLHUP|EPOLLERR)){
				//出现这种情况，可能是对面断开了连接(对方发送RST)
				//水平触发未处理，可能出现HUP事件，正常处理读写？？

				
				 //删除
                 fprintf(stderr, "fd %d get error, delete it from epoll\n", _fired_evs[i].data.fd);
                 this->del_io_event(_fired_evs[i].data.fd);
			}

			

						
		}

		//每次处理完一组epoll_wait触发是事件后，处理异步任务
		this->execute_ready_tasks();
	}
}

/*
 * 这里我们处理的事件机制是
 * 如果EPOLLIN 在mask中， EPOLLOUT就不允许在mask中
 * 如果EPOLLOUT 在mask中， EPOLLIN就不允许在mask中
 * 如果想注册EPOLLIN|EPOLLOUT的事件， 那么就调用add_io_event() 方法两次来注册。
 * */


//添加一个io事件

void event_loop::add_io_event(int fd,io_callback* proc,int mask,void * args)
{
	int final_mask;
	int op;

	//看看当前的fd是否已经有事件了
	if(!_io_evs.count(fd)){
		//没有我们添加
		final_mask=mask;
		op=EPOLL_CTL_ADD;
	}
	else{
		//如果有就是操作MOD
		final_mask=_io_evs[fd].mask|mask;
		op=EPOLL_CTL_MOD;
	}
	//注册回调函数

	if(mask&EPOLLIN){
		_io_evs[fd].read_callback=proc;
		_io_evs[fd].rcb_args=args;
	}
	else if(mask&EPOLLOUT){
	
		_io_evs[fd].write_callback=proc;
		_io_evs[fd].wcb_args=args;
	
	}
	_io_evs[fd].mask = final_mask;

	struct epoll_event event;
	event.events=final_mask;
	event.data.fd=fd;
	if(epoll_ctl(_epfd,op,fd,&event)==-1){
		fprintf(stderr, "epoll ctl %d error\n", fd);
        return;
	}

	
	//将fd添加到我们监听集合中去
	listen_fds.insert(fd);
	
}

void event_loop::del_io_event(int fd)
{	
	_io_evs.erase(fd);

	listen_fds.erase(fd);

	epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,NULL);
}

//删除mask，然后修正监听的事件
void event_loop::del_io_event(int fd, int mask)  
{
	if(!_io_evs.count(fd)) return ;
	int o_mask=_io_evs[fd].mask;
	o_mask =o_mask&(~mask);
	if(o_mask==0){ //完全没有什么监听的了
		del_io_event(fd);
	}
	else{  //进行修改
		
		struct epoll_event event;
        event.events = o_mask;
        event.data.fd = fd;
        epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event);
		
		
	}
}

//添加一个任务task到ready_tasks集合中
void event_loop::add_task(task_func func, void * args){
	task_func_pair func_pair(func,args);
	_ready_tasks.push_back(func_pair);
}


//执行全部的ready_tasks里面的任务
void event_loop::execute_ready_tasks()
{
	std::vector<task_func_pair>::iterator it;
	for(it=_ready_tasks.begin();it!=_ready_tasks.end();it++){
		task_func func=it->first;  //任务的回调函数
		void * args=it->second;  	//回调函数的形参

		//执行任务
		func(this,args);
	}

	//全部执行完毕，清空当前的_ready_task;
	_ready_tasks.clear();
}



