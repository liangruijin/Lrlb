#include "subscribe.h"


extern tcp_server* server;

//单例对象
SubscribeList * SubscribeList::_instance=NULL;

//用于保证创建单例的init方法只执行一次的锁
pthread_once_t SubscribeList::_once=PTHREAD_ONCE_INIT;

SubscribeList::SubscribeList(){};

//订阅
void SubscribeList::subscribe(uint64_t mod,int fd){
	//将mod->fd关系加入到_book_list中
	pthread_mutex_lock(&_book_list_lock);
	_book_list[mod].insert(fd);
	pthread_mutex_unlock(&_book_list_lock);
}

//取消订阅
void SubscribeList::unsubscribe(uint64_t mod,int fd){
	//将mod->fd关系在_book_list中删除
	pthread_mutex_lock(&_book_list_lock);
	if(_book_list.find(mod)!=_book_list.end()){
		_book_list[mod].erase(fd);
		if(_book_list[mod].empty()) _book_list.erase(mod);
	}
	pthread_mutex_unlock(&_book_list_lock);
}

void push_change_task(event_loop* loop,void * args){
	SubscribeList *subscribe = (SubscribeList*)args;
	//1.获取全部在线的客户端
	listen_fd_set online_fds;
	loop->get_listen_fds(online_fds);

	//2.从subscribe的_push_list中，找到与online_fds集合匹配，放在一个新的publish_map中
	publish_map need_publish;
	subscribe->make_publish_map(online_fds,need_publish);

	//3.依次从need_publish中取出数据，发给对应的客户端连接
	publish_map::iterator it;
	for(it =need_publish.begin();it!=need_publish.end();it++){
		int fd=it->first; 
		//遍历fd对应的modid/cmdid集合
		std::unordered_set<uint64_t>::iterator st;
		for(st=it->second.begin();st!=it->second.end();st++){
			int modid=int((*st)>>32);
			int cmdid=int(*st);
			//组装pb消息，发送给客户
			lrlb::GetRouteResponse rsp;
			rsp.set_modid(modid);
			rsp.set_cmdid(cmdid);
			//通过route查询到对应的Host ip/port信息，进行组装
			host_set hosts=Route::instance()->get_hosts(modid,cmdid);
			for(host_set_it hit=hosts.begin();hit!=hosts.end();hit++){
				uint64_t ip_port_pair =* hit;
				lrlb::HostInfo host_info;
				host_info.set_ip((uint32_t)(ip_port_pair>>32));
				host_info.set_port((int)ip_port_pair);
				//添加到rsp中
				rsp.add_host()->CopyFrom(host_info);
			}

			//给当前fd发送一个更新消息
			std::string responseString;
			rsp.SerializeToString(&responseString);

			//通过fd取出连接信息
			net_connection * conn=tcp_server::conns[fd];
			if(conn!=NULL){
				conn->send_message(responseString.c_str(),responseString.size(),lrlb::ID_GetRouteResponse);
			}
		}
	}
}


//根据在线用户fd得到发布列表
void SubscribeList::make_publish_map(listen_fd_set &online_fds, 
            publish_map &need_publish)
{
	publish_map::iterator it;
	pthread_mutex_lock(&_push_list_lock);
	//遍历_push_list找到和online_fds匹配的数据，放在need_publish中
	for(it=_push_list.begin();it!=_push_list.end();){
		//it->first是fd
		//it->second 是cmd/mod
		publish_map::iterator x=it++;        //防止出问题
		if(online_fds.count(x->first)){
			need_publish[x->first]=_push_list[x->first];
			//当改组数据从_push_list中删除
			_push_list.erase(x);
		}
	}
	pthread_mutex_unlock(&_push_list_lock);
}


void SubscribeList::publish(std::vector<uint64_t> & change_mods)
{
	//1.将change_mods已经修改的mod->fd放在发布清单_push_lish中
	pthread_mutex_lock(&_book_list_lock);
    pthread_mutex_lock(&_push_list_lock);
	std::vector<uint64_t>::iterator it;
	for (it = change_mods.begin(); it != change_mods.end(); it++) {
        uint64_t mod = *it;
        if (_book_list.find(mod) != _book_list.end()) {
            //将mod下面的fd set集合拷迁移到 _push_list中
            unordered_set<int>::iterator fds_it;
            for (fds_it = _book_list[mod].begin(); fds_it != _book_list[mod].end(); fds_it++) {
                int fd = *fds_it;
                _push_list[fd].insert(mod);
            }
        }
    }
	pthread_mutex_unlock(&_push_list_lock);
    pthread_mutex_unlock(&_book_list_lock);

	printf("point 3,finish the publish's _push_list load\n");
	//2.通知各个线程去执行推送任务
	server->get_thread_pool()->send_task(push_change_task,this);
}


