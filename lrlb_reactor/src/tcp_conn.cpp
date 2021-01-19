#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>

#include "tcp_conn.h"
#include "message.h"
#include "tcp_server.h"



//回显业务
void callback_busi(const char* data,uint32_t len,int msgid,void *args,tcp_conn * conn)
{
	conn->send_message(data,len, msgid);
}


//连接的读事件回调
static void conn_rd_callback(event_loop * loop,int fd,void* args)
{
	tcp_conn* conn=(tcp_conn*) args;
	conn->do_read();
}


//连接的写事件回调
static void conn_wt_callback(event_loop * loop,int fd,void* args)
{
	tcp_conn* conn=(tcp_conn*) args;
	conn->do_write();
}

tcp_conn::tcp_conn(int connfd, event_loop * loop)
{
	_connfd=connfd;
	_loop=loop;
	//1 将connfd设置成非阻塞状态
	int flag=fcntl(_connfd,F_GETFL,0);
	fcntl(_connfd,F_SETFL,O_NONBLOCK|flag);

	//2.设置TCP_NODELAY ，禁止读写缓存，降低小包延迟

	int op=1;
	setsockopt(_connfd,IPPROTO_TCP,TCP_NODELAY,&op,sizeof op);
	//2.5 如果用户注册了连接建立的HOOK， 则需调用
	if(tcp_server::conn_start_cb){
		tcp_server::conn_start_cb(this,tcp_server::conn_start_cb_args);
	}

	//3.将该连接的读事件 交给loop监控
	_loop->add_io_event(_connfd,conn_rd_callback,EPOLLIN,this);

	//4. 将连接集成到对应的tcp_server中
	tcp_server::increase_conn(_connfd, this);
}


void tcp_conn::do_read()
{
 //1.从套接字读取数据
 	int ret=ibuf.read_data(_connfd);
 	if(ret==-1){
		fprintf(stderr,"read data from socket\n");
		this->clean_conn();
		return ;
 	}
	else if(ret==0){
		printf("connection closed by peer\n");
		this->clean_conn();
		return ;
	}

	//2.解析msg_head数据
	msg_head head;
	while(ibuf.length()>MESSAGE_HEAD_LEN){
		//2.1 读取msg_head头部，固定长度MESSAGE_HEAD_LEN
		memcpy(&head,ibuf.data(),MESSAGE_HEAD_LEN);
		if(head.msglen>MESSAGE_LENGTH_LIMIT||head.msglen<0){
			fprintf(stderr, "data format error, need close, msglen = %d\n", head.msglen);
            this->clean_conn();
            break;
		}
		//这里为什么不把缓存buf里面的头部弹出来之后再判断呢，而是加上头部之后再判断？
		//如果我们把头部弹出来之后，消息长度不够，那么偶尔肯定还要读
		//但是下次没有头部再处理就会有问题
		if(ibuf.length()<MESSAGE_HEAD_LEN+head.msglen){
			break;
		}

		//2.2再根据头长度读取数据体，然后针对数据体处理 业务

		//头部处理完了，往后偏移头部的长度
		ibuf.pop(MESSAGE_HEAD_LEN);
		//处理ibuf.data()业务数据
		//char tmp[ibuf.length()];
		//memcpy(tmp,ibuf.data(),ibuf.length());
		//printf("read data : %s ,length is %d \n",ibuf.data(),head.msglen);

		//回显业务 在第7章写了消息路由之后修改。
		//callback_busi(ibuf.data(), head.msglen, head.msgid, NULL, this);
		tcp_server::router.call(head.msgid,head.msglen,ibuf.data(),this);

		
		ibuf.pop(head.msglen);		
	}
	ibuf.adjust();
	return ;
}


void tcp_conn::do_write()
{
	//do_write 是触发完event事件要处理的事情
	//把out_buf里里面的 数据写回给客户端
	
	//这里不是在这里组装一个message再发
    //组装message的过程应该是主动调用


	 //只要obuf中有数据就写
    while (obuf.length()) {
        int ret = obuf.write2fd(_connfd);
        if (ret == -1) {
            fprintf(stderr, "write2fd error, close conn!\n");
            this->clean_conn();
            return ;
        }
        if (ret == 0) {
            //不是错误，仅返回0表示不可继续写
            break;
        }
    }

    if (obuf.length() == 0) {
        //数据已经全部写完，将_connfd的写事件取消掉
        _loop->del_io_event(_connfd, EPOLLOUT);
    }

    return ;
}


int tcp_conn::send_message(const char * data, int msglen, int msgid)
{
	//printf("server will send_message: %s:%d, msgid = %d\n", data, msglen, msgid);
	bool active_epollout=false;
	if(obuf.length()==0){
		//如果现在已经数据都发送完了，那么是一定要激活写事件的
        //如果有数据，说明数据还没有完全写完到对端，那么没必要再激活等写完再激活
        active_epollout = true;
	}
	//1封装消息头
	msg_head head;
	head.msgid=msgid;
	head.msglen=msglen;
	
	//1.1 写消息头
	int ret=obuf.send_data((const char*)&head,MESSAGE_HEAD_LEN);
	if(ret!=0){
		fprintf(stderr,"send head error\n");
		return -1;
	}

	//1.2 写消息体
	ret =obuf.send_data(data,msglen);
	if(ret!=0){
		obuf.pop(MESSAGE_HEAD_LEN);   //如果写消息体失败，那就回滚将消息头的发送也取消
		fprintf(stderr,"send body error\n");
		return -1;
	}

	if(active_epollout==true){
		//2.激活EPOLLOUT
		_loop->add_io_event(_connfd, conn_wt_callback,EPOLLOUT, this);
	}
	
	return 0;
	
}


//销毁tcp_conn
void tcp_conn::clean_conn()
{
	//如果用户注册了连接建立的HOOK， 则需调用
	if(tcp_server::conn_close_cb){
		tcp_server::conn_close_cb(this,tcp_server::conn_close_cb_args);
	}

	
	//连接 清理工作
	//1.将连接从tcp_server 清除掉
	tcp_server::decrease_conn(_connfd);
	//2.将该链接从event_loop中清除
	_loop->del_io_event(_connfd);

	//3. buf清空
	ibuf.clear();
	obuf.clear();
	//4.关闭套接字   为啥需要置为-1呢？
	int fd=_connfd;
	_connfd=-1;
	close(fd);
}









