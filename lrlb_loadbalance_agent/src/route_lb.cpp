#include "route_lb.h"
#include "lrlb.pb.h"

//构造初始化
route_lb::route_lb(int id):_id(id)
{
	pthread_mutex_init(&_mutex,NULL);
}

//agent获取一个host主机，将返回的主机结果存放在rsp中
int route_lb::get_host(int modid, int cmdid, lrlb :: GetHostResponse & rsp)
{
	int ret=lrlb::RET_SUCC;
	//1.得到key
	uint64_t key=((uint64_t)modid<<32) +cmdid;

	pthread_mutex_lock(&_mutex);
	//2.当前key已经存在_route_lb_map中
	if(_route_lb_map.count(key)){
		//2.1取出对应的load_balance
		load_balance* lb=_route_lb_map[key];
		if(lb->empty()){
			//存在lb里面的host为空，说明正在pull中，还没有从dns_service返回来，所以直接回复不存在
			assert(lb->status==load_balance::PULLING);
			rsp.set_retcode(lrlb::RET_NOEXIST);
		}
		else{
			ret=lb->choice_one_host(rsp);
			rsp.set_retcode(ret);
			//todo 超时了重新拉取
			//超时重拉路由
            //检查是否要重新拉路由信息
            //若路由并没有处于PULLING状态，且有效期已经超时，则重新拉取
            if (lb->status == load_balance::NEW && time(NULL) - lb->last_update_time > lb_config.update_timeout) {
                lb->pull();
            }
		}
	}
	else{ //3当前key不存在_route_lb_map中
		//3.1新建一个load_balance
		load_balance* lb=new load_balance(modid,cmdid);
		if(lb==NULL){
			fprintf(stderr, "no more space to create loadbalance\n");
            exit(1);
		}
		//3.2新建的balance放入到map中
		_route_lb_map[key]=lb;
		//3.3 从dns server服务拉取具体的host信息
		lb->pull();
		//3.4 设置rsp的回执retcode
		rsp.set_retcode(lrlb::RET_NOEXIST);
		ret=lrlb::RET_NOEXIST;
	}
	pthread_mutex_unlock(&_mutex);
	return ret;
	
}


int route_lb::update_host(int modid, int cmdid, lrlb :: GetRouteResponse & rsp)
{
	//1.得到Key
	uint64_t key=((uint64_t)modid<<32)+cmdid;

	pthread_mutex_lock(&_mutex);

	//2.在_route_map中找到对应的key
	if(_route_lb_map.count(key)){
		load_balance* lb=_route_lb_map[key];
		if(rsp.host_size()==0){
			//2.1如果返回的结果lb没有任何host信息，应该删除该Key
			delete lb;
			_route_lb_map.erase(key);
		}
		else{
			//2.2更新信息
			lb->update(rsp);
		}
	}
	//这里没有else,因为我们的get_host在pull之前已经验证过必有key的。如果没有key说明信息不对称
	pthread_mutex_unlock(&_mutex);
	return 0;
}


void route_lb::report_host(lrlb :: ReportRequest req){
	int modid=req.modid();
	int cmdid=req.cmdid();
	int retcode=req.retcode();
	int ip=req.host().ip();
	int port=req.host().port();

	uint64_t key=((uint64_t)modid<<32)+cmdid;

	pthread_mutex_lock(&_mutex);

	if(_route_lb_map.count(key)){
		load_balance* lb=_route_lb_map[key];
		//上报信息个远程reporter服务器
		lb->report(ip,port,retcode);
		lb->commit();
	}

	pthread_mutex_unlock(&_mutex);
	
}


//将全部的load_balance都重置为NEW状态
void route_lb::reset_lb_status(){
	pthread_mutex_lock(&_mutex);
	for(route_map_it it=_route_lb_map.begin();it!=_route_lb_map.end();it++){
		load_balance* lb=it->second;
		if(lb->status==load_balance::PULLING){
			lb->status=load_balance::NEW;
		}
	}
	pthread_mutex_unlock(&_mutex);
}


//agent获取某个modid/cmdid的全部主机，将返回的主机结果存放在rsp中
int route_lb::get_route(int modid, int cmdid, lrlb::GetRouteResponse &rsp){
	int ret = lrlb::RET_SUCC;
	//1. 得到key
    uint64_t key = ((uint64_t)modid << 32) + cmdid;
	pthread_mutex_lock(&_mutex);
    //2. 当前key已经存在_route_lb_map中
    if (_route_lb_map.find(key) != _route_lb_map.end()) {
        //2.1 取出对应的load_balance
        load_balance *lb = _route_lb_map[key];

        std::vector<host_info*> vec;
        lb->get_all_hosts(vec);

        for (std::vector<host_info*>::iterator it = vec.begin(); it != vec.end(); it++) {
            lrlb::HostInfo host;
            host.set_ip((*it)->ip);
            host.set_port((*it)->port);
            rsp.add_host()->CopyFrom(host);
        }

        //超时重拉路由
        //检查是否要重新拉路由信息
        //若路由并没有处于PULLING状态，且有效期已经超时，则重新拉取
        if (lb->status == load_balance::NEW && time(NULL) - lb->last_update_time > lb_config.update_timeout) {
            lb->pull();
        }
    }
	//3. 当前key不存在_route_lb_map中
    else {
        //3.1 新建一个load_balance
        load_balance *lb = new load_balance(modid, cmdid);
        if (lb == NULL) {
            fprintf(stderr, "no more space to create loadbalance\n");
            exit(1);
        }

        //3.2 新建的load_balance加入到map中
        _route_lb_map[key] = lb;

        //3.3 从dns service服务拉取具体的host信息
        lb->pull();

        ret = lrlb::RET_NOEXIST;
    }
    pthread_mutex_unlock(&_mutex);

    return ret;
	
}














