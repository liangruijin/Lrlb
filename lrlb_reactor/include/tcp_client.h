#pragma once 

#include "io_buf.h"
#include "event_loop.h"
#include "message.h"
#include "net_connection.h"


#include <sys/types.h>
#include <sys/socket.h>
#include<sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>


//typedef void msg_callback(const char *data, uint32_t len, int msgid, void *conn, void *user_data);

class tcp_client:public net_connection
{
public:
	//初始化客户端套接字
	tcp_client(event_loop* loop,const char * ip,unsigned short port,const char * name);
	//发送message
	int send_message(const char * data,int msglen,int msgid);
	//创建连接
	void do_connect();
	//处理读
	int do_read();
	//处理写
	int do_write();
	//释放连接资源
	void clean_conn();
	int get_fd(){
		return _sockfd;
	};
	~tcp_client();
	
	//在第7章设置消息路由之后注释掉
	//设置业务处理回调函数  
	//void set_msg_callback(msg_callback* msg_cb){
	//	this->_msg_callback=msg_cb;
	//}

	void add_msg_router(int msgid,msg_callback* cb,void* user_data=NULL){
		_router.register_msg_router(msgid,cb,user_data);
	}
	
	bool connected;
	//server 端的地址
	struct sockaddr_in _server_addr;
	io_buf _ibuf;
	io_buf _obuf;

	//=== 创建连接/销毁连接Hook部分 ============

	//设置连接创建的HOOK函数
	void set_conn_start(conn_callback cb,void * args=NULL){
		_conn_start_cb =cb;
		_conn_start_cb_args=args;
	}
	//设置连接销毁之后的hook函数
	void set_conn_close(conn_callback cb,void *args=NULL){
		_conn_close_cb=cb;
		_conn_close_cb_args=args;
	}
	// 创建连接 的回调函数
	 conn_callback _conn_start_cb;
	 void * _conn_start_cb_args;
	// 销毁连接触发 回调函数
	conn_callback _conn_close_cb;
	void * _conn_close_cb_args;
	
	
	
private:
	//处理消息的路由
	msg_router _router;
	
	int _sockfd;
	socklen_t _addrlen;
	//客户端的时间处理机制
	event_loop* _loop;
	//当前客户端的名字
	const char* _name;

	msg_callback* _msg_callback;

	
};































