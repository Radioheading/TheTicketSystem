# 火车票管理系统

SJTU CS1951课程大作业

## 简介

火车票管理系统是一个为用户提供购票、订单查询、火车查询、换乘查询、购票查询，为管理员提供设置信息、修改信息等功能的系统。其中的数据以二进制文件的形式存储（包括空间回收）。

## 程序功能概述

1、**交互**：我们通过标准输入进行输入和输出，使用tokenscanner进行指令的切片和划分，为接下来的执行和交互做好准备。

2、**总系统**：我们根据tokenscanner的划分调用用户系统和火车系统，并将得到的输出结果返回到主程序中输出。

3、**用户系统**：用户系统中进行了在线用户（内存）和已注册用户（外存）的存储，可以进行检索、修改、注册、登录等功能。

4、**火车系统**：火车系统中存储了火车信息、各车次座位余量信息、各车站经过火车信息、各用户订单信息、各车次候补订单信息（均在外存），可以进行加入火车、删除火车、发布火车、查询车票、查询车次、查询换乘、购票、查询订单、退票等功能。

指令的具体信息和规则请见[管理系统文档](https://github.com/ACMClassCourse-2022/Ticket-System/blob/master/management_system.md)

## 代码文件结构

我们由**main.cpp**连接至**parse_program.hpp**，再通过其中的总系统连接到**user.hpp和train.hpp**。而其中用到的各种工具（如数据结构、排序、时间类）则被存储在**./lib**中。

```plain
├── lib
│   ├── CacheList.hpp（缓存）
│   ├── bpt.hpp（单键值B＋树）
│   ├── exceptions.hpp
│   ├── map.hpp
│   ├── multibpt.hpp（含第一关键字和第二关键字）
│   ├── recycle.hpp（空间回收）
│   ├── CacheList.hpp（缓存）
│   ├── utils.hpp（含定长字符串、日期类、时间类、哈希函数、快速排序）
│   ├── vector.hpp
├── src
│   ├── parse_program.hpp
│   ├── train.hpp
│   ├── user.hpp
├── CMakeLists.txt
├── README.md
```

## 文件存储说明

对于上述提到的将储存在外存的信息，我们将其存在B＋树中。具体来说，对于树上的节点和叶子节点下的数据节点，我们会分别用一个文件来存储；同时，两者均有空间回收，而可被回收的节点地址也用了两个文件来存储。

```plain
├── OrderData.db
├── OrderData's garbage.db
├── OrderTree.db
├── OrderTree's garbage.db
├── PendingData.db
├── PendingData's garbage.db
├── PendingTree.db
├── PendingTree's garbage.db
├── SeatData.db
├── SeatData's garbage.db
├── SeatTree.db
├── SeatTree's garbage.db
├── StationData.db
├── StationData's garbage.db
├── StationTree.db
├── StationTree's garbage.db
├── UserData.db
├── UserData's garbage.db
├── UserTree.db
├── UserTree's garbage.db
```
