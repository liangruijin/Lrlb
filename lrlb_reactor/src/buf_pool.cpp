
#include "buf_pool.h"
#include <assert.h>

buf_pool* buf_pool::_instance=NULL;
//这里为什么不写成这种格式呢？为啥前面要加个指针？
//buf_pool::_instance=NULL;

//保证单例只执行一次
pthread_once_t buf_pool::_once=PTHREAD_ONCE_INIT;

//互斥锁初始化
pthread_mutex_t buf_pool::_mutex=PTHREAD_MUTEX_INITIALIZER;

//我们内存池的格式如下：
//_pool -->     [m4K] -- io_buf-io_buf-io_buf-io_buf...
//              [m16K] -- io_buf-io_buf-io_buf-io_buf...
//              [m64K] -- io_buf-io_buf-io_buf-io_buf...
//              [m256K] -- io_buf-io_buf-io_buf-io_buf...
//              [m1M] -- io_buf-io_buf-io_buf-io_buf...
//担心虚拟机后面的内存不够，所以4M和8M暂时没有分配
//              [m4M] -- io_buf-io_buf-io_buf-io_buf...
//              [m8M] -- io_buf-io_buf-io_buf-io_buf...


buf_pool::buf_pool():_total_mem(0)
{
	//开辟4k的空间
	_pool[m4K]=new io_buf(m4K);
	if(_pool[m4K]==NULL) {
		fprintf(stderr,"new iobuf m4K first time error");
		exit(1);
	}
	io_buf * prev=_pool[m4K];
	//开辟5000个4k的buf  	  20MB
	for(int i=1;i<5000;i++){
		prev->next=new io_buf(m4K);
		if(prev->next==NULL){
			fprintf(stderr,"new iobuf m4K error");
			exit(1);
		}
		prev=prev->next;
	}
	_total_mem+=4*5000;


	//开辟16k的空间
		_pool[m16K]=new io_buf(m16K);
		if(_pool[m16K]==NULL) {
			fprintf(stderr,"new iobuf m16K first time error");
			exit(1);
		}
		 prev=_pool[m16K];
		//开辟1000个16k的buf 	  16MB
		for(int i=1;i<1000;i++){
			prev->next=new io_buf(m16K);
			if(prev->next==NULL){
				fprintf(stderr,"new iobuf m16K error");
				exit(1);
			}
			prev=prev->next;
		}
		_total_mem+=16*1000;


	
		//开辟16k的空间
				_pool[m64K]=new io_buf(m64K);
				if(_pool[m64K]==NULL) {
					fprintf(stderr,"new iobuf m64K first time error");
					exit(1);
				}
				 prev=_pool[m64K];
				//开辟500个64k的buf       32MB
				for(int i=1;i<500;i++){
					prev->next=new io_buf(m64K);
					if(prev->next==NULL){
						fprintf(stderr,"new iobuf m64K error");
						exit(1);
					}
					prev=prev->next;
				}
				_total_mem+=64*500;

				
		//开辟256k的空间
		_pool[m256K]=new io_buf(m256K);
		if(_pool[m256K]==NULL) {
			fprintf(stderr,"new iobuf m256K first time error");
			exit(1);
		}
		 prev=_pool[m256K];
		//开辟200个256k的buf       50MB
		for(int i=1;i<200;i++){
			prev->next=new io_buf(m256K);
			if(prev->next==NULL){
				fprintf(stderr,"new iobuf m256K error");
				exit(1);
			}
			prev=prev->next;
		}
		_total_mem+=256*200;


		//开辟1M的空间
		_pool[m1M]=new io_buf(m1M);
		if(_pool[m1M]==NULL) {
			fprintf(stderr,"new iobuf m1M first time error");
			exit(1);
		}
		 prev=_pool[m1M];
		//开辟50个1M的buf       50MB
		for(int i=1;i<50;i++){
			prev->next=new io_buf(m1M);
			if(prev->next==NULL){
				fprintf(stderr,"new iobuf m1M error");
				exit(1);
			}
			prev=prev->next;
		}
		_total_mem+=1024*50;

		//开辟4M的空间
		_pool[m4M]=new io_buf(m4M);
		if(_pool[m4M]==NULL) {
			fprintf(stderr,"new iobuf m4M first time error");
			exit(1);
		}
		 prev=_pool[m4M];
		//开辟20个m4M的buf       80MB
		for(int i=1;i<20;i++){
			prev->next=new io_buf(m4M);
			if(prev->next==NULL){
				fprintf(stderr,"new iobuf m4M error");
				exit(1);
			}
			prev=prev->next;
		}
		_total_mem+=4096*50;


		
	
}


io_buf* buf_pool::alloc_buf(int N){
	int index;
	if(N<=m4K){
		index=m4K;
	}
	else if(N<=m16K){
		index=m16K;
	}
	else if(N<=m64K){
		index=m64K;
	}
	else if(N<=m256K){
		index=m256K;
	}
	else if(N<=m1M){
		index=m1M;
	}
	else if(N<=m4M){
		index=m4M;
	}
	else return NULL;

	pthread_mutex_lock(& _mutex); //先加锁
	if(_pool[index]==NULL){  //如果当前这个型号的buf没有了，再分配
		if(_total_mem+index/1024>=EXTRA_MEM_LIMIT){  //但是不能超出限制的大小
			fprintf(stderr,"already use too much memory!\n");
			exit(0);
		}
		io_buf* new_buf=new io_buf(index);
		if(new_buf==NULL){
			fprintf(stderr,"new io_buf error,the index is %d \n",index);
			exit(1);
		}
		_total_mem+=index/1024;
		pthread_mutex_unlock(&_mutex);
		return new_buf;
	}
	io_buf * target=_pool[index];
	_pool[index]=target->next;
	pthread_mutex_unlock(& _mutex);
	target->next=NULL;  //这一步很关键，不要返回一个列表给用户了
	return target;
}


void buf_pool::revert(io_buf * buffer)
{
	int index=buffer->capacity;
	//重置buffer
	buffer->length=0;
	buffer->head=0;
	
	pthread_mutex_lock(& _mutex);

	assert(_pool.count(index));

	buffer->next=_pool[index];
	_pool[index]=buffer;
	pthread_mutex_unlock(& _mutex);
	
}


	








