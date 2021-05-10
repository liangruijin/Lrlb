# Lrlb

#### TCP/UDP服务器框架线程模型

![image-20210510124627421](C:\Users\liangrui\Desktop\Lrlb\README.assets\image-20210510124627421.png)

网络框架中，利用了io_buf类来封装基本的buffer结构。然后用一个io_buf pool来管理全部的buffer集合。

#### 框架中线程池模型

buffer pool的管理方法是利用map类型进行管理，其中Key是每个组内的空间容量，每个key下面挂了一个io_buf链表。每次用户需要取用直接从对应的链表中取出即可. key :int,value:io_buf*

![image-20210510125920380](C:\Users\liangrui\Desktop\Lrlb\README.assets\image-20210510125920380.png)