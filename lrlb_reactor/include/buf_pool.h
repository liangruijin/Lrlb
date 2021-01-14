#pragma once 

#include <unordered_map>

#include "io_buf.h"

typedef std::unordered_map<int,io_buf*> pool_t;

enum MEM_CAP{
	m4K=4096,
	m16K=16384,
	m64K=65526,
	m256K=262144,
	m1M=1048576,
	m4M=4194304,
	m8M=8388608
};

#define EXTRA_MEM_LIMIT (5U*1024*1024)
//5GB


class buf_pool
{
public:
	static void init(){
		_instance =new buf_pool();
	}

	static buf_pool* instance(){
		pthread_once(&_once,init);
		return _instance;
	}

	io_buf * alloc_buf(int N);
	io_buf * alloc_buf(){
		return alloc_buf(m4K);
	}

	//收回一个io_buf:
	void revert(io_buf* buffer);

private:
	pool_t _pool; //all buffer map
	buf_pool(); //because sigleton propram

	//there have some quesition :why the sigleton need copy func?
	const buf_pool& operator=(const buf_pool&);
	buf_pool(const buf_pool&);

	uint64_t _total_mem;

	static buf_pool* _instance;
	static pthread_once_t _once;
	static pthread_mutex_t _mutex;

};
	
	

		









