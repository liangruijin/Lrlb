#include "tcp_server.h"

int main() {
	event_loop loop;
    tcp_server server(&loop,"192.168.237.128", 7777);
    
	loop.event_process();
    return 0;
}
