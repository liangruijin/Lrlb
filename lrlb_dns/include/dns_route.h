#pragma once

#include <pthread.h>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include "mysql.h"
#include "lrlb.pb.h"

//#include "subscribe.h"

//定义用来保存modID/cmdID与 主机IP/port的对应关系
typedef std::unordered_map<uint64_t,std::unordered_set<uint64_t>> route_map;
typedef std::unordered_map<uint64_t,std::unordered_set<uint64_t>>::iterator route_map_it;

//定义用来保存host的IP/host的port的集合 数据类型
typedef std::unordered_set<uint64_t> host_set;
typedef std::unordered_set<uint64_t>::iterator host_set_it;



class Route
{
public:
	//创建单例的方法
	static void init(){
		_instance =new Route();
	}

	static Route* instance(){
		//保证init方法在这个线程中执行，只执行一次
		pthread_once(&_once,init);
		return _instance;
	}
	host_set get_hosts(int modid,int cmdid);
	void connect_db();
	void build_maps();
	int load_version();
	int	load_route_data() ;
	void load_changes(std::vector<uint64_t>& change_list);
	void remove_changes(bool remove_all);
	void swap();
private:
	//构造函数私有化
	Route();
	Route(const Route&);
	const Route& operator=(const Route&);

	//单例
	static Route* _instance;
	//单例锁
	static pthread_once_t _once;

	//属性
	//数据库
	MYSQL _db_conn;  //mysql连接
	char _sql[1000]; //sql语句
	long _version;
	//modid/cmdid------->ip/port
	route_map * _data_pointer;  //指向RouterDataMap_A    当前map
	route_map * _temp_pointer;  //临时的map
	pthread_rwlock_t _map_lock; //读写锁
	
	
};

//backendThread main
void *check_route_changes(void *args);

