#pragma once

#include <vector>
#include <pthread.h>
#include <unordered_set>
#include <unordered_map>
#include "lrlb_reactor.h"
#include "dns_route.h"

using namespace std;

//定义订阅列表的数据关系类型，key-->modid/cmdid ，value --->fds（订阅的客服端）
typedef unordered_map<uint64_t,unordered_set<int>> subscribe_map;

//定义发布列表的数据关系类型，key--->fd(订阅客户端) value--->modids;
typedef unordered_map<int,unordered_set<uint64_t>> publish_map;


class SubscribeList{
public:
	//设计成单例
	static void init(){
		_instance =new SubscribeList();
	}
	static SubscribeList* instance(){
		//保证Init方法在这个进程中执行，只执行一次
		pthread_once(&_once,init);
		return _instance;
	}

	//订阅
	void subscribe(uint64_t mod,int fd);
	//取消订阅
	void unsubscribe(uint64_t mod,int fd);
	//发布
	void publish(vector<uint64_t> & change_mods);
	//根据在线用户fd得到需要发布的列表
	void make_publish_map (listen_fd_set& online_fds,publish_map& need_publish);
	

	
private:
	//设计成单例
	SubscribeList();
	SubscribeList(const SubscribeList &);
	const SubscribeList& operator=(const SubscribeList);

	static SubscribeList * _instance;
	static pthread_once_t _once;

	subscribe_map _book_list;  //订阅清单
	pthread_mutex_t _book_list_lock;
	
	publish_map _push_list;  //发布清单
	pthread_mutex_t _push_list_lock;
	
};












