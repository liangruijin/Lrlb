#include "reactor_buf.h"
#include <sys/ioctl.h>
#include <string.h>

reactor_buf::reactor_buf(){
	_buf=NULL;
}

reactor_buf::~reactor_buf(){
	clear(); //调用自己的成员函数
}

const int reactor_buf::length() const 
{
	return _buf!=NULL?_buf->length:0;
}

void reactor_buf::pop(int len){
	assert(_buf!=NULL&&len<=_buf->length);
	_buf->pop(len);
	if(_buf->length==0){ //如果这里的可用长度为0 了，需要收回吗？我觉得这里不需要呢，我只是暂时不用而已
//		buf_pool::instance()->revert(_buf);
//		_buf=NULL;		
	}
	
}

void reactor_buf::clear(){
	if(_buf!=NULL){ //这才是清理我们_buf
		buf_pool::instance()->revert(_buf);
		_buf=NULL;
	}
}

//从一个fd读取数据到我们reactor_buf中来
int input_buf::read_data(int fd)
{
	int need_read; //硬件需要读取多少数据

	//我们默认调用一次这个函数，将一次性读取全部数据
	//需要给fd设置FIONREAD得到有多少数据是可以读取的

	//这个手法其实我没怎么见过，需要后面好好琢磨

	if(ioctl(fd,FIONREAD,&need_read)==-1){
		fprintf(stderr,"ioctl FIONREAD\n");
		return -1;
	}

	if(_buf==NULL){
		//如果io_buf为空，从内存池中申请
		_buf=buf_pool::instance()->alloc_buf(need_read);
		if(_buf==NULL){
			fprintf(stderr,"no idle buf for alloc\n");
			return -1;
		}
	}
	else {
		//assert(_buf->head==0); //为什么head必须为0 呢？因为我们要从后往前放数据，
		//改：
		if(_buf->head!=0) _buf->adjust();
		//head等于0方便我们计算剩余的长度
		if(_buf->capacity-_buf->length<(int)need_read){
			io_buf* new_buf=buf_pool::instance()->alloc_buf(need_read+_buf->length);
			if(_buf==NULL){
				fprintf(stderr,"no idle buf for alloc\n");
				return -1;
			}

			//将之前的_buf拷贝一下到我们的新的buf中
			new_buf->copy(_buf);
			buf_pool::instance()->revert(_buf);
			_buf=new_buf;
		}
	}
	//以上是处理好了我们的buf了

	int already_read=0;

	do{
		//读到的数据放在之前的数据之后
		if(need_read==0){
			//什么时候needread是0呢？就是fd缓存没有数据，但是我们调用了read，那么就是阻塞读的情况
			//对方并没有读数据
			already_read=read(fd,_buf->data+_buf->length,m4K);//此时head是为0的。
		}
		else{
			//有数据的
			already_read=read(fd,_buf->data+_buf->length,need_read);
		}
	}while(already_read==-1&&errno==EINTR); //中断引起的错误，继续读
	if(already_read>0){
		if(need_read!=0){
			assert(already_read==need_read); //数据必须读完 ，如果need_read==0，那就随便读了
		}
		_buf->length+=already_read;
		//_buf->data[_buf->length]='\0';
		
	}
	return already_read;	//如果读到的0，或者-1，我们直接将这些数字返回即可
}


const char* input_buf::data() const 
{
	return _buf!=NULL?_buf->data+_buf->head:NULL;
	//这儿代码有问题，以下为错误示范
	//if(_buf==NULL) return NULL;
	//char tmp[_buf->length()];
	//memcpy(tmp,_buf->data+_buf->head,_buf->length());
	//return tmp;
}
void input_buf::adjust()
{
	if(_buf!=NULL){
		_buf->adjust();  //这接去调用即可
	}
}


int output_buf::send_data(const char * data, int datalen)
{
	if(_buf==NULL){
		_buf=buf_pool::instance()->alloc_buf(datalen);
		if(_buf==NULL) {
			fprintf(stderr,"no idle buf for alloc in send_data\n");
			return -1;
		}
	}
	else {
		if(_buf->head!=0) _buf->adjust();
		if(_buf->capacity-_buf->length<datalen){
			//不够存
			io_buf* new_buf=buf_pool::instance()->alloc_buf(datalen+_buf->length);
			if(new_buf==NULL){
				fprintf(stderr,"no idle buf for alloc in send_data\n");
				return -1;
			}
			//换个buf
			new_buf->copy(_buf);
			buf_pool::instance()->revert(_buf);
			_buf=new_buf;
		}
	}
	memcpy(_buf->data+_buf->length,data,datalen);
	_buf->length+=datalen;
	return 0;
	
}


int output_buf::write2fd(int fd)
{
	assert(_buf!=NULL);
	if(_buf->head!=0) _buf->adjust(); //这样写好吗，其实我也很怀疑的。

	int already_write=0;

	do{
		already_write=write(fd,_buf->data,_buf->length); 
	}while(already_write==-1&&errno==EINTR);

	if(already_write>0){
		_buf->pop(already_write);
		_buf->adjust();
	}

	if(already_write==-1&&errno==EAGAIN){
		already_write=0;  //如果不是错误，而只是因为fd是非阻塞的，返回0 ，表示目前是不可以继续写的。
		//如果你是用户，你希望得到的是什么呢？
		//其实我更加愿意认为直接原样返回即可。
	}
	return already_write;
	
}












