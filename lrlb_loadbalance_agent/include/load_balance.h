#pragma once
#include <unordered_map>
#include <list>
#include <set>
#include "host_info.h"
#include "lrlb.pb.h"

//ip+port为主键 的Host信息集合
typedef std::unordered_map<uint64_t,host_info*> host_map;
typedef std::unordered_map<uint64_t,host_info*>::iterator host_map_it;

//host_info list集合
typedef std::list<host_info*> host_list;
typedef std::list<host_info*> ::iterator host_list_it;




//负载均衡算法核心模块,针对一组(modid/cmdid)下的全部host节点的负载规则
class load_balance{
public:
	load_balance(int modid,int cmdid):_modid(modid),_cmdid(cmdid){};
	//判断是否有没有host在当前的LB节点中
	bool empty() const;
	//从当前的双队列中获取host信息
	int choice_one_host(lrlb::GetHostResponse & rsp);

	//如果list中没有host信息，需要从远程的DNS service发送GetRouteHost请求申请
	int pull();

	//根据dns service远程返回的结果，更新_host_map
	void update(lrlb::GetRouteResponse &rsp);

	//当前load_balance模块的状态
    enum STATUS
    {
        PULLING, //正在从远程dns service通过网络拉取
        NEW      //正在创建新的load_balance模块
    };
    STATUS status;  //当前的状态

private:
	int _modid;
	int _cmdid;
	int _access_cnt; //请求次数，每次请求加一，判断是否超过probe_num阈值

	host_map _host_map; //当前Load_balance模块管理的全部ip+port为主键的host信息

	host_list _idle_list;  //空闲队列
	host_list _overload_list; //过载队列
	
};


