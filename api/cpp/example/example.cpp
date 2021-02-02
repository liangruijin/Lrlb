#include <lrlb_api.h>
#include <iostream>

using namespace std;

void usage(){
	printf("usage: ./example [modid] [cmdid]\n");
}
int main(int argc,char* argv[]){
	if(argc!=3) {
		usage();
		return 1;
	}
	int modid=atoi(argv[1]);
	int cmdid=atoi(argv[2]);

	lrlb_client api;
	string ip;
	int port;
	//初始化
	int ret=api.reg_init(modid,cmdid);
	if (ret != 0) {
        std::cout << "modid " << modid << ", cmdid " << cmdid << " still not exist host, after register, ret = " << ret << std::endl;
    }
	route_set route;
	ret=api.getroute(modid,cmdid,route);
	if(ret==0){
		std::cout << "get route succ!" << std::endl;
        for (route_set_it it = route.begin(); it != route.end(); it++) {
            std::cout << "ip = " << (*it).first << ", port = " << (*it).second << std::endl;
        }
	}
	int cnt=0;
	ret=api.get_host(modid,cmdid,ip,port);
	if(ret==0){
		cout<<"host is "<<ip<<":"<<port<<endl;
		//todo 上报结果
		 //上报调用结果 0 表示成功， 1 表示过载
		if (cnt % 3 == 0) {
            api.report(modid, cmdid, ip, port, 1);
        }
        else {
            api.report(modid, cmdid, ip, port, 0);
        }
	}
	return 0;
}
