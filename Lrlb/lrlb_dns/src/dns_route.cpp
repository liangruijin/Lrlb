#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <lrlb_reactor.h>
#include "dns_route.h"
#include <stirng.h>

using namespace std;

//单例对象
Route * Route::_instance =NULL;

//用于保证创建单例的init方法只执行一次的锁

pthread_once_t Route::_once=PTHRED_ONCE_INIT;

Route::Route()
{
	//1 初始化锁
	pthread_rwlock_init(&_map_lock,NULL);
	//2.初始化map
	_data_pointer =new route_map();
	_temp_pointer =new route_map();
	
	//3.连接数据库
	this->connect_db();
	//4.查询数据库，创建_data_pointer 与 _temp_pointer 两个map
	this->build_maps();
	
}


void Route::connect_db()
{
	//---mysql数据库配置
	string db_host=config_file::instance()->GetString("mysql", "host", "127.0.0.1");
	short db_port=config_file::instance()->GetNumber("mysql", "port", 3306);
	string db_user=config_file::instance()->GetString("mysql", "user", "root");
	string db_passwd=config_file::instance()->GetString("mysql", "passwd", "110");
	string db_name = config_file::instance()->GetString("mysql", "name", "lrlb_dns");

	mysql_init(&_db_conn);

	//超时断开
	mysql_options(&_db_conn,MYSQL_OPT_CONNECT_TIMEOUT,"30");
	//设置mysql连接断开自动重连
	my_bool reconnect =1;
	mysql_options(&_db_conn,MYSQL_OPT_RECONNECT,&reconnect);

	if(!mysql_real_connect(&_db_conn,db_host.c_str(),db_user.c_str(),db_passwd.c_str(),db_name.c_str(),db_port,NULL,0))
	{
		fprintf(stderr"Failed to connect mysql\n");
		exit(1);
	}
}






