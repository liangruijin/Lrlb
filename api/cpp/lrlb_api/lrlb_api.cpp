#include "lrlb_api.h"
#include "lrlb.pb.h"

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

lrlb_client::lrlb_client():_seqid(0)
{
	printf("lrlb_client()\n");
	//1.初始化服务器地址
	struct sockaddr_in servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	//默认Ip地址是本地，因为api和agent应该是部署在同一台主机上
	inet_aton("127.0.0.1",&servaddr.sin_addr);
	//2.创建3个UDPsocket
	for(int i=0;i<3;i++){
		_sockfd[i]=socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,IPPROTO_UDP);
		if(_sockfd[i]==-1){
			perror("socket()");
            exit(1);
		}
		//agent的3个DUP端口默认为8888，8889，8890
		servaddr.sin_port=htons(8888+i);
		int ret=connect(_sockfd[i],(const struct sockaddr*)&servaddr,sizeof(servaddr));
		if(ret==-1){
			perror("connect()");
            exit(1);
		}
		printf("connection agent udp succ!\n");
	}
}


lrlb_client::~lrlb_client(){
	printf("~lrlb_client()\n");
	for(int i=0;i<3;i++) close(_sockfd[i]);
}


int lrlb_client::get_host(int modid, int cmdid, std :: string & ip, int & port)
{
	//每个消息都有一个seq序列号，防止udp的丢包和消息不对称的情况
	//从本地的agent service获取host信息
	uint32_t seq=_seqid++;
	//1.封装消息
	lrlb::GetHostResponse req;
	req.set_seq(seq);
	req.set_modid(modid);
	req.set_cmdid(cmdid);

	//2.send
	char write_buf[4096],read_buf[80*1024];
	//消息头
	msg_head head;
	head.msglen=req.ByteSizeLong();
	head.msgid=lrlb::ID_GetHostRequest;
	memcpy(write_buf,&head,MESSAGE_HEAD_LEN);
	//消息体
	req.SerializeToArray(write_buf+MESSAGE_HEAD_LEN,head.msglen);
	//简单的hash来发送给对应的agent udp server
	int index=(modid+cmdid)%3;
	int ret=sendto(_sockfd[index],write_buf,head.msglen+MESSAGE_HEAD_LEN,0,NULL,0);
	if(ret==-1){
		perror("sendto");
        return lrlb::RET_SYSTEM_ERROR;
	}

	//3.recv
	int message_len;
	lrlb::GetHostResponse rsp;
	do{
		message_len=recvfrom(_sockfd[index],read_buf,sizeof(read_buf),0,NULL,NULL);
		if(message_len==-1) {
			perror("recvfrom");
            return lrlb::RET_SYSTEM_ERROR;
		}
		//消息头
		memcpy(&head,read_buf,MESSAGE_HEAD_LEN);
		if(head.msgid!=lrlb::ID_GetHostResponse){
			fprintf(stderr, "message ID error!\n");
            return lrlb::RET_SYSTEM_ERROR;
		}
		//消息体
		ret=rsp.ParseFromArray(read_buf+MESSAGE_HEAD_LEN,message_len-MESSAGE_HEAD_LEN);
		if(!ret){
			fprintf(stderr, "message format error: head.msglen = %d, message_len = %d, \
				message_len - MESSAGE_HEAD_LEN = %d, head msgid = %d, ID_GetHostResponse = %d\n",
				head.msglen, message_len, message_len-MESSAGE_HEAD_LEN, 
				head.msgid, lrlb::ID_GetHostResponse);
            return lrlb::RET_SYSTEM_ERROR;
		}
	}while(rsp.seq()<seq);

	if(rsp.seq()!=seq||rsp.modid()!=modid||rsp.cmdid()!=cmdid){
		fprintf(stderr, "message format error\n");
        return lrlb::RET_SYSTEM_ERROR;
	}

	//4.处理消息
	if(rsp.retcode()==0){
		lrlb::HostInfo host=rsp.host();
		struct in_addr inaddr;
		inaddr.s_addr=host.ip();
		ip=inet_ntoa(inaddr);
		port=host.port();
	}
	return rsp.retcode();
	
}


//lrlb 系统上报host调用信息
void lrlb_client::report(int modid,int cmdid,const std::string & ip,int port,int retcode)
{
	//1 封装上报消息
	lrlb::ReportRequest req;
	
	req.set_modid(modid);
	req.set_cmdid(cmdid);
	req.set_retcode(retcode);
	
	//1.1 host信息
	lrlb::HostInfo * hp=req.mutable_host();
	//ip
	struct in_addr inaddr;
	inet_aton(ip.c_str(),&inaddr); //转换成网络字节序
	int ip_num=inaddr.s_addr;
	hp->set_ip(ip_num);
	//port
	hp->set_port(port);
	
	//2. send
	char write_buf[4096];
	msg_head head;
	head.msglen=req.ByteSizeLong();
	head.msgid=lrlb::ID_GetHostRequest;
	memcpy(write_buf,&head,MESSAGE_HEAD_LEN);
	req.SerializeToArray(write_buf+MESSAGE_HEAD_LEN,head.msglen);
	
	int index=(cmdid+modid)%3;
	int ret=sendto(_sockfd[index],write_buf,head.msglen+MESSAGE_HEAD_LEN,0,NULL,0);
	if(ret==-1){
		perror("sendto");
	}
}





//系统获取某modid/cmdid全部的hosts(route)信息
int lrlb_client::get_route(int modid, int cmdid, route_set&route)
{
	//每个消息都有一个seq序列号，防止udp的丢包和消息不对称的情况
	//但是这里并不需要，因为获取的是全部的route信息，只要保证是同一种类型的包就好了
	//从本地的agent service获取host信息
	//uint32_t seq=_seqid++;
	//1.封装消息
	lrlb::GetRouteRequest req;
	//req.set_seq(seq);
	req.set_modid(modid);
	req.set_cmdid(cmdid);

	//2.send
	char write_buf[4096],read_buf[80*1024];
	//消息头
	msg_head head;
	head.msglen=req.ByteSizeLong();
	head.msgid=lrlb::ID_API_GetRouteRequest;
	memcpy(write_buf,&head,MESSAGE_HEAD_LEN);
	//消息体
	req.SerializeToArray(write_buf+MESSAGE_HEAD_LEN,head.msglen);
	//简单的hash来发送给对应的agent udp server
	int index=(modid+cmdid)%3;
	int ret=sendto(_sockfd[index],write_buf,head.msglen+MESSAGE_HEAD_LEN,0,NULL,0);
	if(ret==-1){
		perror("sendto");
        return lrlb::RET_SYSTEM_ERROR;
	}

	//3.recv
	int message_len;
	lrlb::GetRouteResponse rsp;
	
		message_len=recvfrom(_sockfd[index],read_buf,sizeof(read_buf),0,NULL,NULL);
		if(message_len==-1) {
			perror("recvfrom");
            return lrlb::RET_SYSTEM_ERROR;
		}
		//消息头
		memcpy(&head,read_buf,MESSAGE_HEAD_LEN);
		if(head.msgid!=lrlb::ID_API_GetRouteResponse){
			fprintf(stderr, "message ID error!\n");
            return lrlb::RET_SYSTEM_ERROR;
		}
		//消息体
		ret=rsp.ParseFromArray(read_buf+MESSAGE_HEAD_LEN,message_len-MESSAGE_HEAD_LEN);
		if(!ret){
			fprintf(stderr, "message format error: head.msglen = %d, message_len = %d, \
				message_len - MESSAGE_HEAD_LEN = %d, head msgid = %d, ID_API_GetRouteResponse = %d\n",
				head.msglen, message_len, message_len-MESSAGE_HEAD_LEN, 
				head.msgid, lrlb::ID_API_GetRouteResponse);
            return lrlb::RET_SYSTEM_ERROR;
		}
	

	if(rsp.modid()!=modid||rsp.cmdid()!=cmdid){
		fprintf(stderr, "message format error\n");
        return lrlb::RET_SYSTEM_ERROR;
	}

	
	//4 处理消息
    for (int i = 0; i < rsp.host_size(); i++) {
        const lrlb::HostInfo &host = rsp.host(i);
        struct in_addr inaddr;
        inaddr.s_addr = host.ip();
        std::string ip = inet_ntoa(inaddr);
        int port = host.port();
        route.push_back(ip_port(ip,port));
    }
	return lrlb::RET_SUCC;
	
}

int lrlb_client::reg_init(int modid,int cmdid){
	route_set route;
	int retry_cnt=0;
	while(route.empty()&&retry_cnt<3){
		get_route(modid,cmdid,route);
		if(route.empty()==true){
			usleep(50000);
		}
		else break;
		++retry_cnt;
	}
	if(route.empty()==true){
		return lrlb::RET_NOEXIST;
	}
	return lrlb::RET_SUCC;
}












