#include "tcp_server.h"



//回显业务的回调函数
void callback_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("callback_busi ...\n");
    //直接回显
    conn->send_message(data, len, msgid);
}


//打印信息回调函数
void print_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("recv client: [%s]\n", data);
    printf("msgid: [%d]\n", msgid);
    printf("len: [%d]\n", len);
}


int main() {
	event_loop loop;
    tcp_server server(&loop,"192.168.237.128", 7777);
    //注册消息业务路由机制
	server.add_msg_router(1,callback_busi);
	server.add_msg_router(2,print_busi);
	
	
	loop.event_process();
    return 0;
}
