#include "lrlb_reactor.h"
#include "mysql.h"
#include "dns_route.h"
#include "lrlb.pb.h"

void get_route(const char *data, uint32_t len, int msgid, net_connection *net_conn, void *user_data )
{
	//1.解析proto文件
	lrlb::GetRouteRequest req;
	req.ParseFromArray(data,len);
	//2.得到modid 和cmdid
	int modid,cmdid;

	modid=req.modid();
	cmdid=req.cmdid();
	//3.根据modid/cmdid获取host信息
	host_set hosts=Route::instance()->get_hosts(modid,cmdid);

	//4.将数据打包成protobuf
	lrlb::GetRouteResponse rsp;

	rsp.set_modid(modid);
	rsp.set_cmdid(cmdid);
	for(host_set_it it=hosts.begin();it!=hosts.end();it++){
		uint64_t ip_port=*it;
		lrlb::HostInfo host;
		host.set_ip((uint32_t)(ip_port>>32));
		host.set_port((int)(ip_port));
		rsp.add_host()->CopyFrom(host);
	}

	//5.发送给客户端
	std::string responseString;
	rsp.SerializeToString(&responseString);
	net_conn->send_message(responseString.c_str(), responseString.size(),lrlb::ID_GetRouteResponse);
}



int main(int argc, char **argv)
{
	event_loop loop;

    //加载配置文件
    config_file::setPath("../conf/lrlb_dns.conf");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 7778);
	
	//创建tcp服务器
    tcp_server *server = new tcp_server(&loop, ip.c_str(), port);
	
	//注册路由业务
	server->add_msg_router(lrlb::ID_GetRouteRequest, get_route);
	
	////测试mysql接口
    //MYSQL dbconn;
    //mysql_init(&dbconn);
	
	
	 //开始事件监听    
    printf("lrlb dns service ....\n");
    loop.event_process();

}