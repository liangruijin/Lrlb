#pragma once 

#include "event_loop.h"

struct task_msg
{
	enum TASK_TYPE{
		NEW_CONN,  //新连接到来的任务
		NEW_TASK,  //一般的任务
	};
	TASK_TYPE type;  //任务类型

	//任务的一些参数
	union{
		//针对新的连接任务 ，我们需要传递connfd
		int connfd;
		//===暂时用不上====
		//针对new_task新建的任务
		//那么需要给任务提供一个回调函数
		struct {
			task_func task_cb;  //注册的任务函数
			void * args;		//任务函数对应的形参
		};
	};
	
};









