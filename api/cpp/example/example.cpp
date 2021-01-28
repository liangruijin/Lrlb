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
	int ret=api.get_host(modid,cmdid,ip,port);
	if(ret==0){
		cout<<"host is "<<ip<<":"<<port<<endl;
		//todo 上报结果
	}
	return 0;
	
	
}
