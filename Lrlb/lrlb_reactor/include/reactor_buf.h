#pragma once 

#include "io_buf.h"
#include "buf_pool.h"
#include <assert.h>
#include <unistd.h>

class reactor_buf  //其实就是将io_buf作为自己的成员 ，没意思
{
public:
	reactor_buf();
	~reactor_buf();

	const int length() const ;
	void pop(int len);
	void clear();
protected:
	io_buf* _buf;
	
};


class input_buf : public reactor_buf 
{
public:
    //从一个fd中读取数据到reactor_buf中
    int read_data(int fd);

    //取出读到的数据
    const char *data() const;

    //重置缓冲区
    void adjust();
};


class output_buf:public reactor_buf
{
public:
	//将数据放入到buf中
	int send_data(const char * data,int datalen);
	//将数据写到一个fd中去
	int write2fd(int fd);
};










