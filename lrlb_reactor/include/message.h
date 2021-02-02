#pragma once 
#include "net_connection.h"
//解决tcp粘包问题的消息头

#include <map>

struct msg_head
{
	int msgid;
	int msglen;
};

//消息头的二进制长度 固定为8
#define MESSAGE_HEAD_LEN 8
#define MESSAGE_LENGTH_LIMIT (65535- MESSAGE_HEAD_LEN)




typedef void msg_callback(const char* data,uint32_t len,int msgid ,net_connection* net_conn,void * user_data);

class msg_router
{
public:
	msg_router():_router(),_args(){ printf("msg router init ..\n");}

	//给消息ID注册一个对应的回调函数
	int register_msg_router(int msgid,msg_callback* msg_cb,void * user_data)
	{	
		//如果已经注册相应的回调函数
		if(_router.count(msgid)) return -1;

		_router[msgid] =msg_cb;
		_args[msgid]=user_data;
		printf("register id %d success!\n",msgid);
		return 0;
	}

	//调用注册的对应的回调业务函数
	void call(int msgid,uint32_t msglen,const char* data,net_connection* net_conn)
	{
		if(!_router.count(msgid)) {
			fprintf(stderr,"msgid %d is not redister!\n",msgid );
			return ;
		}

		//直接取出来回调函数
		msg_callback* callback=_router[msgid];
		void * user_data =_args[msgid];
		callback(data,msglen,msgid,net_conn,user_data);
		//printf("=======调用call结束======\n");
			
	}

private:
	//针对消息的路由分发，key为msgID,value 为注册的回调业务函数
	std::map<int,msg_callback*> _router;
	//针对不同的id，有着不同的对应参数
	std::map<int,void*> _args;
};
