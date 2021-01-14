#pragma once


//定义一个buffer存放数据的结构
class io_buf{
public:
	io_buf(int size);
	//清空数据
	void clear();

	//将处理过的数据清空，将未处理的数据往前移
	void adjust();
	//拷贝其他的数据
	void copy(const io_buf* other);
	//处理长度为len的数据，移动head和修正length
	void pop(int len);
	//如果存在多个buffer，采用链表将其连接起来
	io_buf* next;

	//当前bufer的容量大小
	int capacity;
	//当前buffer的数据大小
	int length;
	//未处理的数据头部位置
	int head;
	//当前io_buf 所保存的数据地址
	char* data;
};
	



