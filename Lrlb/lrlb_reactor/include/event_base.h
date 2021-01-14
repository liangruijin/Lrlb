#pragma once 

//提前声明
class event_loop;

typedef void io_callback(event_loop * loop,int fd,void * args);

struct io_event
{
	int mask; //EPOLLIN,EPOLLOUT;
	io_callback* read_callback;
	io_callback* write_callback;
	void * rcb_args; //read call args;
	void * wcb_args;
};

















