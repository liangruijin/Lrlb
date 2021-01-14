#pragma once 

#include "reactor_buf.h"
#include "event_loop.h"
#include "net_connection.h"
//一个TCP连接信息
class tcp_conn :public net_connection
{
public:
	tcp_conn(int connfd,event_loop* loop);
	//处理读业务
	void do_read();
	//处理写业务
	void do_write();
	//销毁tcp_conn
	void clean_conn();
	
	//发送信息的方法
	int send_message(const char* data,int msglen,int msgid);
	//获取当前连接的fd
	int get_fd(){
		return _connfd;
	}

private:
	//当前连接的fd
	int _connfd;
	//当前连接归属的eventloop
	event_loop * _loop;
	//输出Buf
	output_buf obuf;
	//
	input_buf ibuf;
		
};
