#include "main_server.h"
#include "load_balance.h"
#include "lrlb.pb.h"


//判断是否已经没有host在当前LB节点中
bool load_balance::empty() const
{
    return _host_map.empty();
}


static void get_host_from_list(lrlb::GetHostResponse & rsp,host_list& l){
	//从list中选择第一个节点
	host_info* host=l.front();

	//HostInfo自定义类型,proto3并没提供set方法,而是通过mutable_接口返回HostInfo的指针,可根据此指针进行赋值操作
	lrlb::HostInfo* hip=rsp.mutable_host();
	hip->set_ip(host->ip);
	hip->set_port(host->port);
	//将上面处理过的第一个节点放在队列末尾
	l.pop_front();
	l.push_back(host);
}

//从两个队列中获取一个Host给上层
int load_balance::choice_one_host(lrlb :: GetHostResponse & rsp)
{
	//1.判断_dile_list队列是否已经空，如果空表示没有空闲节点
	if(_idle_list.empty()){
		//1.1判断是否已经超过了probe_num
		if(_access_cnt>=lb_config.probe_num){
			_access_cnt=0;
			//从verload_list中选择一个已经过载的节点
			get_host_from_list(rsp,_overload_list);
		}
		else {
			++_access_cnt;
			return lrlb::RET_OVERLOAD;
		}
	}
	else{
		//2.判断过载例表是否为空
		if(_overload_list.empty()){
			//2.1 当前modid/cmdid所有的节点均为正常
			//选择一个idle节点
			get_host_from_list(rsp,_idle_list);
		}
		else{
			//2.2部分节点过载
			if(_access_cnt>=lb_config.probe_num){
				_access_cnt=0;
				//从verload_list中选择一个已经过载的节点
				get_host_from_list(rsp,_overload_list);
			}
			else{
				//正常从idle_list中选出一个节点
                ++_access_cnt;
                get_host_from_list(rsp, _idle_list);
			}
		}
		
	}
	return lrlb::RET_SUCC;
}

//如果list中没有host消息，需要从远程的DNS service 发送GetRouteHost请求申请
int load_balance::pull(){
	//请求dns service请求
	lrlb::GetRouteRequest route_req;
	route_req.set_modid(_modid);
	route_req.set_cmdid(_cmdid);
	//通过dns client的thread queue发送请求
	dns_queue->send(route_req);
	//由于远程有一定的延迟，所以应该把当前Load_balance模块标记一个正在拉取的状态
	status=PULLING;
	return 0;
}

//根据dns service 远程返回的结果，更新_host_map
void load_balance::update(lrlb :: GetRouteResponse & rsp)
{
	//确保dns service 返回的结果有host信息
	assert(rsp.host_size()!=0);
	std::set<uint64_t> remote_hosts;
	std::set<uint64_t> need_delete;
	//1.插入新增的host信息到 _host_map中
	for(int i=0;i<rsp.host_size();i++){
		//1.1 得到rsp中的一个host
		const lrlb::HostInfo & h=rsp.host(i);
		//1.2 得到一个ip+port的key值
		uint64_t key=((uint64_t)h.ip()<<32+h.port());
		remote_hosts.insert(key);
		//1.3如果自身的_host_map找不到key,说明是新增
		if(!_host_map.count(key)){
			 //新增
            host_info *hi = new host_info(h.ip(), h.port(), lb_config.init_succ_cnt);
			if(hi==NULL){
				fprintf(stderr, "new host_info error!\n");
             	exit(1);
			}
			_host_map[key]=hi;
			//新增的host信息加入到 空闲列表中
			_idle_list.push_back(hi);
		}
		//else 如果没有这里不需要更新吗？
		
	}

	//2. 删除减少的host信息 从_host_map中
	//2.1 得到哪些节点需要删除
	for (host_map_it it = _host_map.begin(); it != _host_map.end(); it++) {
        if (remote_hosts.find(it->first) == remote_hosts.end())  {
            //该key在host_map中存在，而在远端返回的结果集不存在，需要锁定被删除
            need_delete.insert(it->first);
        }
    }
	//2.2 删除
	for (std::set<uint64_t>::iterator it = need_delete.begin();
        it != need_delete.end(); it++)  {
        uint64_t key = *it;

        host_info *hi = _host_map[key];

        if (hi->overload == true) {
            //从过载列表中删除
            _overload_list.remove(hi);
        } 
        else {
            //从空闲列表删除
            _idle_list.remove(hi);
        }

        delete hi;
		_host_map.erase(key);
    }
}

void load_balance::report(int ip, int port, int retcode){
	uint64_t key=((uint64_t)ip<<32)+port;
	if(!_host_map.count(key)) return ;
	//定义当前时间
	long current_time=time(NULL);
	//1.计数统计
	host_info * hi=_host_map[key];
	if(retcode==lrlb::RET_SUCC)  //retcode==0
	{
		//更新虚拟节点，真实成功的次数
		hi->vsucc++;
		hi->rsucc++;
		//连续成功增加
		hi->contin_succ++;
		hi->contin_err=0;
	}
	else{
		hi->verr++;
		hi->rerr++;
		//连续失败个数增加
		hi->contin_err++;
		hi->contin_succ=0;
	}
	//2 检查节点状态
	//检查idle节点是否满足overload条件
	//或者overload节点是否能满足idle条件
	if(hi->overload==false&&retcode!=lrlb::RET_SUCC){
		bool overload=false;
		//idle节点，检查是否达到overload的条件
		//计算失败率
		double err_rate=hi->verr*1.0/(hi->vsucc+hi->verr);
		if(err_rate>lb_config.err_rate) overload=true;
		//2.连续失败是否大大阈值
		if(overload==false&&hi->contin_err>=(uint32_t)lb_config.contin_err_limit) overload=true;

		//判定overload需要做的更改流程
		if(overload){
			struct in_addr saddr;
			saddr.s_addr = htonl(hi->ip);
		 	 printf("[%d, %d] host %s:%d change overload, succ %u err %u\n", 
                    _modid, _cmdid, inet_ntoa(saddr), hi->port, hi->vsucc, hi->verr);
			 /设置hi为overload状态 
            hi->set_overload();
			//移出_idle_list联保，放在overload_list中
			_idle_list.remove(hi);
            _overload_list.push_back(hi);
            return;
		}
	}
	//如果是overload节点，则只有调用成功才有必要判断是否达到idle条件
	else if(hi->overload == true && retcode == lrlb::RET_SUCC){
		bool idle = false;
	
		//overload节点，检查是否达到回到idle状态的条件
		//(1).计算成功率，如果大于预设值的成功率，则为idle
		double succ_rate = hi->vsucc * 1.0 / (hi->vsucc + hi->verr);
		if (succ_rate > lb_config.succ_rate) {
            idle = true;
        }
		//(2).连续成功次数达到阈值，判定为idle
        if (idle == false && hi->contin_succ >= (uint32_t)lb_config.contin_succ_limit) {
            idle = true;
        }
		//判定为idle需要做的更改流程
        if (idle) {
            struct in_addr saddr;
            saddr.s_addr = htonl(hi->ip);
            printf("[%d, %d] host %s:%d change idle, succ %u err %u\n", 
                    _modid, _cmdid, inet_ntoa(saddr), hi->port, hi->vsucc, hi->verr);

            //设置为idle状态
            hi->set_idle();
            //移出overload_list, 放入_idle_list
            _overload_list.remove(hi);
            _idle_list.push_back(hi);
            return;
        }
		
	}

	
	//窗口检查和超时机制 

	if(hi->overload==false){
		//节点是idle状态
		if(current_time-hi->idle_ts>=lb_config.idle_timeout){
			//时间窗口已经到达,需要对idle节点清理负载均衡数据
			if(hi->check_window()==true){
				//将此节点设置为过载
				struct in_addr saddr;
				saddr.s_addr=htonl(hi->ip);
				printf("[%d, %d] host %s:%d change to overload cause windows err rate too high, read succ %u, real err %u\n",
                        _modid, _cmdid, inet_ntoa(saddr), hi->port, hi->rsucc, hi->rerr);
				//设置为overload状态,重置了信息
                hi->set_overload();
				//移出_idle_list,放入_overload_list
                _idle_list.remove(hi);
                _overload_list.push_back(hi);
			}
			else{
				//重置窗口,恢复负载默认信息
                hi->set_idle();
			}

		}
	}
	else{
		//节点为overload状态
		//那么处于overload的状态时间是否已经超时
		if (current_time - hi->overload_ts >= lb_config.overload_timeout) {
			struct in_addr saddr;
            saddr.s_addr = htonl(hi->ip); //转换为网络字节序
            printf("[%d, %d] host %s:%d reset to idle, vsucc %u,  verr %u\n",
                    _modid, _cmdid, inet_ntoa(saddr), hi->port, hi->vsucc, hi->verr);
			hi->set_idle();
			 //移出overload_list, 放入_idle_list
            _overload_list.remove(hi);
            _idle_list.push_back(hi);
		}
	}
	
}


//提交host的调用结果给远程reporter service上报结果
void load_balance::commit()
{
    if (this->empty() == true) {
        return;
    }

    //1. 封装请求消息
    lrlb::ReportStatusRequest req;
    req.set_modid(_modid);
    req.set_cmdid(_cmdid);
    req.set_ts(time(NULL));
    req.set_caller(lb_config.local_ip);

    //2. 从idle_list取值
    for (host_list_it it = _idle_list.begin(); it != _idle_list.end(); it++) {
        host_info *hi = *it;    
        lrlb::HostCallResult call_res;
        call_res.set_ip(hi->ip);
        call_res.set_port(hi->port);
        call_res.set_succ(hi->rsucc);
        call_res.set_err(hi->rerr);
        call_res.set_overload(false);
    
        req.add_results()->CopyFrom(call_res);
    }

    //3. 从over_list取值
    for (host_list_it it = _overload_list.begin(); it != _overload_list.end(); it++) {
        host_info *hi = *it;
        lrlb::HostCallResult call_res;
        call_res.set_ip(hi->ip);
        call_res.set_port(hi->port);
        call_res.set_succ(hi->rsucc);
        call_res.set_err(hi->rerr);
        call_res.set_overload(true);
    
        req.add_results()->CopyFrom(call_res);
    }

    //4 发送给report_client 的消息队列
    report_queue->send(req);
}



