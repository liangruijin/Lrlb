#include "host_info.h"
#include "main_server.h"


/*
当idle节点的失败率>预设值（默认10%），将节点判定为overload；
当overload节点的成功率>预设值（默认95%），将节点判定为idle；

而不可以idle/overload节点都只关注成功率or都只关注失败率，这样可能造成节点在idle/overload状态间频繁切换

*/
void host_info::set_idle()
{
    vsucc = lb_config.init_succ_cnt;
    verr = 0;
    rsucc = 0;
    rerr = 0;
    contin_succ = 0;
    contin_err = 0;
    overload = false;
	idle_ts = time(NULL);
}

void host_info::set_overload()
{
    vsucc = 0;
    verr = lb_config.init_err_cnt;//overload的初试虚拟err错误次数
    rsucc = 0;
    rerr = 0;
    contin_err = 0;
    contin_succ = 0;
    overload = true;
	overload_ts = time(NULL);
}


//计算整个窗口内的真实失败率，如果达到连续失败窗口值则返回true，代表需要调整状态
bool host_info::check_window() {
    if (rsucc + rerr == 0) {
        return false; //分母不能为0
    }

    if (rerr * 1.0 / (rsucc + rerr) >= lb_config.window_err_rate) {
        return true;
    }

    return false;
}

