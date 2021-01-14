#pragma once

#include <netinet/in.h>
#include "event_loop.h"
#include "tcp_conn.h"
#include "message.h"
#include "thread_pool.h"


class tcp_server
{
public:
	//构造函数
	tcp_server(event_loop* loop,const char* ip,uint16_t port);
	tcp_server(event_loop* loop,const char* ip,uint16_t port,int pthread_num);
	//提供连接服务
	void do_accept();

	~tcp_server();

	void add_msg_router(int msgid,msg_callback * cb,void * user_data=NULL){
		router.register_msg_router(msgid,cb,user_data);
	}

private:
	int _sockfd; //套接字
	struct sockaddr_in _connaddr; //客服端连接地址
	socklen_t _addrlen; //客服端连接地址长度
	//event_loop epoll时间机制
	event_loop* _loop;

	//客户端管理连接部分
public:
	static void increase_conn(int connfd,tcp_conn* conn); //新增加一个连接
	static void decrease_conn(int connfd);  //减少一个连接
	static void get_conn_num(int * curr_conn); //得到连接数量
	static tcp_conn** conns;  //全部已经在线的连接信息

	//=== 消息路由分发 ====
	static msg_router router;

	//=== 创建连接/销毁连接Hook部分 ============
	//设置链接的创建hook函数
    static void set_conn_start(conn_callback cb, void *args = NULL) {
        conn_start_cb = cb;
        conn_start_cb_args = args;
    }

    //设置链接的销毁hook函数
    static void set_conn_close(conn_callback cb, void *args = NULL) {
        conn_close_cb = cb;
        conn_close_cb_args = args;
    }
	
	// 创建连接 的回调函数
	static conn_callback conn_start_cb;
	static void * conn_start_cb_args;
	// 销毁连接触发 回调函数
	static conn_callback conn_close_cb;
	static void * conn_close_cb_args;
	//获取当前server的线程池
	thread_pool * get_thread_pool(){
		return _thread_pool;
	}

private:
	 //TOdo

	 //从配置文件中读取信息

#define MAX_CONNS 1024
	static int _max_conns;    //最大的连接数量
	static int _curr_conns;  //当前的连接数量
	static pthread_mutex_t _conns_mutex; //保护当前连接刻度修改的锁
	thread_pool * _thread_pool;
	 
};
	






























