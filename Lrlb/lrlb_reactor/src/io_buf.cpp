#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <io_buf.h>

io_buf::io_buf(int size):
next(NULL),
capacity(size),
length(0),
head(0)
{
	data=new char[size]; //why there is use new,will it delete?
	assert(data);
}

void io_buf::clear(){
 length=head=0;
}

//put the data undeal into the head of buf;
void io_buf::adjust(){
	if(head!=0){
		if(length!=0){
			memmove(data,data+head,length);  //the time will be very high?
		}
		head=0;
	}
}

void io_buf::copy(const io_buf * other){
	memcpy(data,other->data,other->length);
	head=0;
	length=other->length;
}

void io_buf::pop(int len)
{
	if(len>length) len=length;
	length-=len;
	head+=len;
}









