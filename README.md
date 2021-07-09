

## 基于C++负载均衡远程服务调度系统

## 概述

(**LR** **L**oad **B**alance And Remote service schedule system)

给Lrlb的定位是一个易用、高性能的服务间远程调用**管理、调度、负载均衡**系统。

其应用场景是针对大型服务访问量高的特点，通过弹性负载均衡将用户的访问流量均匀的分发到多个后端云服务器上，确保业务快速平稳的运行。

### **特点：**

**1.高性能、支持高并发链接，满足海量业务访问需求**

**2.自动分发，活扩展用户用于的对外服务能力**

**3.快速部署、管理方便，用户可以高效的管理和调整服务分发策略等**

![](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510175340263.png)

### 系统开发环境：

`Linux` ：ubuntu 18.04

`protobuf`: version  3.11.3

`mysql` : version 5.7.33

`g++`: version 7.5.0

### Lrlb系统总体架构

 对于一个服务系统一个服务可以被抽象为ID：modid+cmdid的组合，称为**一个模块**，而这个服务往往有**多个服务节点**，其所有服务节点的地址集合被称为这个模块下的**路由**，节点地址简称为**节点**

- modid：标识业务的大类，如：“视频相关”

- cmdid：标识具体服务内容，如：“批量最热小视频”

**API**:业务代码利用modid,cmdid和API就可以找Lrlb系统获取节点、汇报节点调用结果。

**DNSService** ： 运行于一台服务器上，负责modid,cmdid到节点路由的转换

- 功能一：提供agent获取一个最新的modID/cmdID和真实配置的host的IP和Port的对应关系
- 功能二：具备订阅和推送最新的modID/cmdID给Agent
- 功能三：定期的去监控持久性关系表的modID/cmdID和host主机信息的数据，保证版本的更新和最新数据的迭代

![image-20210510201631070](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510201631070.png)

**Report Service** ： 运行于DNS Service同机服务器上，负责收集各modid,cmdid下各节点调用状况并将其存储在Mysql数据库中，可用于观察、报警。

功能1：Agent将某个host主机(ip,host) 本次的调用结果 发送给report service

功能2：开设一个存储线程池，专门用来处理入库操作

![image-20210510202757725](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510202757725.png)

**LoadBalance Agent**：运行于每个服务器上，负责**为此服务器上的业务**提供节点获取、节点状态汇报、路由管理、负载调度等核心功能

1.业务每次要向远程服务发送消息时，先利用modid+cmdid去向LB Agent获取一个可用节点，然后向该节点发送消息，完成一次远程调用；具体获取modid+cmdid下的哪个节点是由**LB Agent**负责的

2.对节点的一次远程调用后，调用结果会**汇报给LB Agent**，以便LB Agent根据自身的LB算法来感知远程服务节点的状态是空闲还是过载，进而控制节点获取时的节点调度

![image-20210510210006969](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510210006969.png)

**MYSQL**:至于modid,cmdid的注册、删除可以利用MySQL操作

（后期可以写个web界面，添加删除更加方便）



### Reactor模型服务器框架

#### TCP/UDP服务器框架线程模型

![image-20210510124627421](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510124627421.png)

进一步细化：

![image-20210709085916814](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210709085916814.png)

#### 框架中内存池

网络框架中，利用了io_buf类来封装基本的buffer结构。然后用一个io_buf pool来管理全部的buffer集合。

buffer pool的管理方法是利用map类型进行管理，其中Key是每个组内的空间容量，每个key下面挂了一个io_buf链表。每次用户需要取用直接从对应的链表中取出即可. key :int,value:io_buf*

![image-20210510125920380](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510125920380.png)



#### 对于i/o事件的封装

原生的epoll实际上编程起来扩展性不是很强，那么我们就需要封装一套IO事件处理机制。

```cpp
struct io_event 
{    		 				      
    io_event():read_callback(NULL),write_callback(NULL),rcb_args(NULL),wcb_args(NULL) {}
    int mask; //EPOLLIN EPOLLOUT
    io_callback *read_callback; //EPOLLIN事件 触发的回调 
    io_callback *write_callback;//EPOLLOUT事件 触发的回调
    void *rcb_args; //read_callback的回调函数参数
    void *wcb_args; //write_callback的回调函数参数
};
```

过event_loop类来实现io_event的基础操作，放在原生的epoll堆中



#### Message消息封装

为了解决服务器在发送接受的消息的TCP传输的粘包问题，我们要将所发的数据做一个规定，采用TLV的格式，来进行封装。

![image-20210510172241122](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510172241122.png)

```cpp
struct msg_head
{
    int msgid;
    int msglen;
};
```



#### 消息业务路由分发机制

现在我们发送的消息都是message结构的，有个message头里面其中有两个关键的字段，msgid和msglen.

其中加入msgid的意义就是我们可以甄别是哪个消息，从而对这类消息做出不同的业务处理。但是常规的消息回调是写死了业务的，比如就是"回显业务"、“https报文处理”等等,显然这并不满足我们作为服务器框架的需求。我们需要开发者可以注册自己的回调业务。所以我们需要提供一个注册业务的入口，然后在后端根据不同的msgid来激活不同的回调业务函数。

![image-20210510173910554](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510173910554.png)

```cpp
class msg_router 
{
public:
    //给一个消息ID注册一个对应的回调业务函数
    int register_msg_router(int msgid, msg_callback *msg_cb, void *user_data) 
    //调用注册的对应的回调业务函数
    void call(int msgid, uint32_t msglen, const char *data, net_connection *net_conn) 
private:
    //针对消息的路由分发，key为msgID, value为注册的回调业务函数
    std::unordered_map<int, msg_callback*> _router;
    //回调业务函数对应的参数，key为msgID, value为对应的参数
    std::unordered_map<int, void*> _args;
};
```



#### 负载均衡算法

负载均衡算法，我们称之为是**一个**"load balance", 一个"load balance"对应**针对一组**modID/cmdID下挂在的全部host信息进行负载。**每个"load balance"都会有两个节点队列**。一个队列是"idle_list"，存放目前可用的Host主机信息(ip+port), 一个队列是"overload_list",存放目前已经过载的Host主机信息(ip+port).

![image-20210510212909339](https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510212909339.png)

算法流程：

1.Load Balance从空闲队列拿出队列头部节点，作为选取的节点返回，同时将此节点重追到队列尾部；

2.如果此模块过载队列非空，则每经过probe_num次节点获取后（默认=10），给过载队列中的节点一个机会，从过载队列拿出队列头部节点，作为选取的节点返回，让API试探性的用一下，同时将此节点重追到队列尾部；

3.如果空闲队列为空，说明整个模块过载了，**返回过载错误；**同时2也继续执行

算法的一些参数：

```
probe_num=10
init_succ_cnt=180 //防止刚启动时少量失败就认为过载 vsucc的
init_err_cnt=20 //防止少量成功就认为overload  verr的
err_rate=0.1   //当idle节点失败率高于此值，节点变overload状态
succ_rate=0.95 //当overload节点成功率高于此值，节点变成idle状态
contin_err_limit=15 //当idle节点连续失败次数超过此值，节点变成overload状态
contin_succ_limit=15 //当overload节点连续成功次数超过此值, 节点变成idle状态
idle_timeout=15 // 为防止长期运行的成功个数太多，使得需要更多的错误汇报才会让LB感知到host过载。将idle节点每隔一定时间（默认15s）重置为idle初始状态.
overload_timeout=15 //对于某个modid/cmdid/下的某个overload状态的host，在过载队列等待的最大时间
window_err_rate=0.7  //整个窗口的真实失败率阈值（只在窗口检查时使用）
```

<img src="https://github.com/liangruijin/Lrlb/blob/master/README.assets/image-20210510214420636.png" alt="image-20210510214420636" style="zoom:80%;" />

### API开发者使用流程

agent 请求过程

```c++
API:get_host()—>Agent UDP Server—>route_lb->load_balance.get_host()
```

agent 请求全部路由过程

```c++
API:get_route()—>Agent UDP Server—>route_lb->load_balance.get_all_hosts()
```

agent上报过程

```c++
API:report()—>Agent UDP Server—>route_lb->load_balance.report()  //上报调用失败或者成功
```

## 性能测试工具

自己写了客户端来进行测试

用两个主机（同一台主机上的虚机），测试了一些数据

客户端是开启一个线程进行测试。dns server 和 reporter server在另外一台虚机上

CPU个数：2个 ， 内存: 2GB , 系统：Ubuntu18.04虚拟机

| 线程数 | QPS    |
| :----- | :----- |
| 1      | 0.5w/s |
| 2      | 2.1w/s |
| 10     | 3.2w/s |
| 100    | 2.7w/s |

## 代码量统计

统计统计 SourceCounter

![](https://github.com/liangruijin/Lrlb/blob/master/README.assets/0306219bd775d870cdd59bfff58a4f9.png)

