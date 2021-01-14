#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "tcp_server.h"
#include "tcp_conn.h"
#include "reactor_buf.h"



//tmp test message
struct message{
	char data[m4K];
	char len;
};

struct message msg;

// ==== 消息路由分发 ====
msg_router tcp_server::router;

void server_rd_callback(event_loop* loop,int fd,void * args);
void server_wt_callback(event_loop* loop,int fd,void * args);


void accept_callback(event_loop* loop,int fd,void * args){
	tcp_server* server=(tcp_server*) args;
	server->do_accept();
}


// ===连接资源的管理=== //
//全部已经在线的信息
tcp_conn ** tcp_server::conns =NULL;

//最大容量连接个数
int tcp_server::_max_conns=0;

//当前连接数量
int tcp_server::_curr_conns=0;
//保护_curr_conns的锁
pthread_mutex_t tcp_server::_conns_mutex=PTHREAD_MUTEX_INITIALIZER;

//
// 创建连接 的回调函数
 conn_callback tcp_server::conn_start_cb;
void * tcp_server::conn_start_cb_args;
// 销毁连接触发 回调函数
conn_callback tcp_server::conn_close_cb;
void * tcp_server::conn_close_cb_args;



tcp_server::tcp_server(event_loop* loop,const char* ip,uint16_t port)
{
	bzero(&_connaddr,sizeof (_connaddr)); //先把地址进行清空
	//忽略一些信号
	
	//SIGHUP:如果terminal关闭，会给当前进程发送该信号,默认操作是退出
	//如果忽略了之后，当终端关闭了，该服务器还是不会关闭的。
	if(signal(SIGHUP,SIG_IGN)==SIG_ERR){
		fprintf(stderr,"signal ignore SIGHUP\n");
		}
	//SIGPIPE:如果客户端关闭连接，服务端再次write就会产生sigpipe
	//其默认行为是关闭进程，我们肯定是不希望是因为写错误导致我们退出进程
	//这里要么忽略信号，要么接受信号并处理，那么利用iO多路复用技术，
	//通过IO复用函数来检测管道和socket连接的读端是否已经关闭。以POLL为例
	//当socket连接被对方关闭时，socket上的POLLRDHUP事件将被触发。
	if(signal(SIGPIPE,SIG_IGN)==SIG_ERR){
		fprintf(stderr,"signal ignore SIGPIPE\n");
		}

	
	//1. 创建socket
	//SOCK_CLOEXEC的作用是在子进程调用execd的时候自动关闭文件描述符
		_sockfd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/ | SOCK_CLOEXEC, IPPROTO_TCP);
		if (_sockfd == -1) {
			fprintf(stderr, "tcp_server::socket()\n");
			exit(1);
		}
	
		//2 初始化地址
		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		inet_aton(ip, &server_addr.sin_addr);
		server_addr.sin_port = htons(port);
	
		//2-1可以多次监听，设置REUSE属性
		int op = 1;
		if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
			fprintf(stderr, "setsocketopt SO_REUSEADDR\n");
		}
	
		//3 绑定端口
		if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			fprintf(stderr, "bind error\n");
			exit(1);
		}
	
		//4 监听ip端口
		if (listen(_sockfd, 500) == -1) {
			fprintf(stderr, "listen error\n");
			exit(1);
		}
		//5 初始化event_loop;
		
		_loop=loop;

		//6 创建连接管理
		_max_conns=MAX_CONNS;
		
		conns=new tcp_conn*[_max_conns+3]; 
		//原话 ：为啥呢，因为这里stdin,stdout,stderr已经被占用，所以我们呢
		//需要多开三个文件描述符，不加三就会溢出
		//思考：这里我觉得如果想表达最多连接数量的话，不应该用数组去表示表示，因为可能有其他fd被我们的
		//服务器占用了，那么剩下的fd肯定不够。我觉的应该离散的保存，比如用个hash表保存比较好
		//但是如果我们只是想表达最大连接fd的值的话，可以 用数组。但是就是担心越界了怎么办
		if (conns == NULL) {
        	fprintf(stderr, "new conns[%d] error\n", _max_conns);
        	exit(1);
    	}
		//7 ========创建线程池===========
		int thread_cnt=3 ; //todo 从配置文件中读取
		if(thread_cnt>0){
			_thread_pool=new thread_pool(thread_cnt);
			if(_thread_pool==NULL){
				fprintf(stderr,"tcp_server new thread_pool error\n");
				exit(1);
			}
		}
		//===================
		
		//8注册回调函数

		_loop->add_io_event(_sockfd, accept_callback, EPOLLIN, this);
		
		
		
	}


	tcp_server::tcp_server(event_loop* loop,const char* ip,uint16_t port,int pthread_num)
	{
		bzero(&_connaddr,sizeof (_connaddr)); //先把地址进行清空
		//忽略一些信号
		
		//SIGHUP:如果terminal关闭，会给当前进程发送该信号,默认操作是退出
		//如果忽略了之后，当终端关闭了，该服务器还是不会关闭的。
		if(signal(SIGHUP,SIG_IGN)==SIG_ERR){
			fprintf(stderr,"signal ignore SIGHUP\n");
			}
		//SIGPIPE:如果客户端关闭连接，服务端再次write就会产生sigpipe
		//其默认行为是关闭进程，我们肯定是不希望是因为写错误导致我们退出进程
		//这里要么忽略信号，要么接受信号并处理，那么利用iO多路复用技术，
		//通过IO复用函数来检测管道和socket连接的读端是否已经关闭。以POLL为例
		//当socket连接被对方关闭时，socket上的POLLRDHUP事件将被触发。
		if(signal(SIGPIPE,SIG_IGN)==SIG_ERR){
			fprintf(stderr,"signal ignore SIGPIPE\n");
			}
	
		
		//1. 创建socket
		//SOCK_CLOEXEC的作用是在子进程调用execd的时候自动关闭文件描述符
			_sockfd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/ | SOCK_CLOEXEC, IPPROTO_TCP);
			if (_sockfd == -1) {
				fprintf(stderr, "tcp_server::socket()\n");
				exit(1);
			}
		
			//2 初始化地址
			struct sockaddr_in server_addr;
			bzero(&server_addr, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			inet_aton(ip, &server_addr.sin_addr);
			server_addr.sin_port = htons(port);
		
			//2-1可以多次监听，设置REUSE属性
			int op = 1;
			if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
				fprintf(stderr, "setsocketopt SO_REUSEADDR\n");
			}
		
			//3 绑定端口
			if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
				fprintf(stderr, "bind error\n");
				exit(1);
			}
		
			//4 监听ip端口
			if (listen(_sockfd, 500) == -1) {
				fprintf(stderr, "listen error\n");
				exit(1);
			}
			//5 初始化event_loop;
			
			_loop=loop;
	
			//6 创建连接管理
			_max_conns=MAX_CONNS;
			
			conns=new tcp_conn*[_max_conns+3]; 
			//原话 ：为啥呢，因为这里stdin,stdout,stderr已经被占用，所以我们呢
			//需要多开三个文件描述符，不加三就会溢出
			//思考：这里我觉得如果想表达最多连接数量的话，不应该用数组去表示表示，因为可能有其他fd被我们的
			//服务器占用了，那么剩下的fd肯定不够。我觉的应该离散的保存，比如用个hash表保存比较好
			//但是如果我们只是想表达最大连接fd的值的话，可以 用数组。但是就是担心越界了怎么办
			if (conns == NULL) {
				fprintf(stderr, "new conns[%d] error\n", _max_conns);
				exit(1);
			}
			//7 ========创建线程池===========
			int thread_cnt=pthread_num ; //todo 从配置文件中读取
			if(thread_cnt>0){
				_thread_pool=new thread_pool(thread_cnt);
				if(_thread_pool==NULL){
					fprintf(stderr,"tcp_server new thread_pool error\n");
					exit(1);
				}
			}
			//===================
			
			//8注册回调函数
	
			_loop->add_io_event(_sockfd, accept_callback, EPOLLIN, this);
			
			
			
		}

	
	//开始提供创建链接服务
	void tcp_server::do_accept()
	{
		int connfd;    
		while(true) {  //循环读取呀
			//accept与客户端创建链接
			printf("begin accept\n");
			connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);
			if (connfd == -1) {
				if (errno == EINTR) {
					fprintf(stderr, "accept errno=EINTR\n");
					continue;
				}
				else if (errno == EMFILE) {
					//建立链接过多，资源不够
					fprintf(stderr, "accept errno=EMFILE\n");
				}
				else if (errno == EAGAIN) {
					fprintf(stderr, "accept errno=EAGAIN\n");
					break;
				}
				else {
					fprintf(stderr, "accept error");
					exit(1);
				}
			}

			
			else {
				
				//this->_loop->add_io_event(connfd, server_rd_callback, EPOLLIN,&msg);
				//break;
				int cur_conns;
				get_conn_num(&cur_conns);
				if(cur_conns>=_max_conns){
					fprintf(stderr, "so many connections, max = %d\n", _max_conns);
                	close(connfd);
				}
				else {
					//我们需要判断有没有线程池，没有自己处理
					if(_thread_pool!=NULL){  //多线程模式
						//1 选择一个线程来处理
						thread_queue<task_msg>* queue=_thread_pool->get_thread();
						//2创建一个task
						task_msg task;
						task.type=task_msg::NEW_CONN;
						task.connfd=connfd;
						//3.添加到消息队列中去
						queue->send(task);
					//================
					}
					else{       //单线程模式
						tcp_conn* conn=new tcp_conn(connfd,_loop);
						if(conn==NULL) {
							fprintf(stderr, "new tcp_conn error\n");
	               		 	exit(1);
						}
						printf("get new connection succ \n");
						break;
					}
										
				}				
				
			}

			
		}
	}
	
	//链接对象释放的析构
	tcp_server::~tcp_server()
	{
		close(_sockfd);
	}
	

void server_rd_callback(event_loop * loop, int fd, void * args)
{
	int ret=0;
	struct message* msg=(struct message*) args;
	 input_buf ibuf;
	ret=ibuf.read_data(fd);
	if(ret==-1){
		fprintf(stderr, "ibuf read_data error\n");
        //删除事件
        loop->del_io_event(fd);
        
        //对端关闭
        close(fd);

        return;
	}
	if(ret==0){
		//删除事件
        loop->del_io_event(fd);
        
        //对端关闭
        close(fd);

        return;
	
	}
	printf("--------------- \n");
	printf("ibuf.length() =  %d \n",ibuf.length());
	char tmp[ibuf.length()];
	memcpy(tmp,ibuf.data(),ibuf.length());
	tmp[ibuf.length()-1]='\0';
	printf("ibuf.data() is :  %s \n",tmp);
	printf("--------------- \n");

	//将数据放在msg中

	msg->len=ibuf.length();
	bzero(msg->data,sizeof msg->data);
	memcpy(msg->data,ibuf.data(),msg->len);
	ibuf.pop(msg->len);
	ibuf.adjust();
	printf("*********** \n");
	printf("recv data = %s ,msglen is %d\n", msg->data,msg->len);
	printf("*********** \n");
	
	 //删除读事件，添加写事件
    loop->del_io_event(fd, EPOLLIN);
    loop->add_io_event(fd, server_wt_callback, EPOLLOUT, msg); //相当于回显
	
}


void server_wt_callback(event_loop * loop, int fd, void * args)
{
	struct message* msg=(struct message*) args;
	output_buf obuf;
	//回显数据

	obuf.send_data(msg->data,msg->len);
	while(obuf.length()){
		int write_ret=obuf.write2fd(fd);
		if(write_ret==-1){  
			fprintf(stderr, "write connfd error\n");
        	 return;
		}
		else if(write_ret==0){
			break;  //不是错误，只是不可写，如果不break的话，就会一直循环写
		}
	}
	//删除写事件，添加读事件
	 loop->del_io_event(fd, EPOLLOUT);
	 loop->add_io_event(fd, server_rd_callback, EPOLLIN, msg);

}


//增加一个连接
void tcp_server::increase_conn(int connfd, tcp_conn * conn)
{
	pthread_mutex_lock(&_conns_mutex);
	conns[connfd]=conn;
	_curr_conns++;
	pthread_mutex_unlock(&_conns_mutex);
}


//减少一个连接

void tcp_server::decrease_conn(int connfd)
{
	pthread_mutex_lock(&_conns_mutex);
	conns[connfd]=NULL;
	_curr_conns--;
	pthread_mutex_unlock(&_conns_mutex);
}

//得到当前连接的数量
void tcp_server::get_conn_num(int * curr_conn)
{
	pthread_mutex_lock(&_conns_mutex);
	*curr_conn=_curr_conns;
	pthread_mutex_unlock(&_conns_mutex);
}













