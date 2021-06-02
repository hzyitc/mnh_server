# mnh_server
[![GitHub release](https://img.shields.io/github/v/tag/hzyitc/mnh_server?label=release)](https://github.com/hzyitc/mnh_server/releases)

[README](README.md) | [中文文档](README_zh.md)

## 介绍

`mnh_server`是一个实现了`mnhv1`协议的服务器。

`mnhv1`是一种用来协助打NAT洞的协议。

## 使用指南

### 克隆并编译

```
> git clone https://github.com/hzyitc/mnh_server
> cd mnh_server
> make
```

### 运行mnh_server

```
> ./mnh_server <端口>
```

## 协议详解

`mnh_server`会使用同个端口处理`mnhv1`和`HTTP`协议。

所有消息都是文本格式。

### mnhv1

客户端：
```
mnhv1 {id}\n
```

服务端:
```
{IP}:{端口号}\n
```

然后，

每10s，客户端向服务器发送心跳包:
```
heartbeat\n
```

当服务器收到包后，需要返回一个心跳包:
```
.
```

### HTTP

客户端:
```
GET /{id} {不关心后面的数据}
```

服务端:
```
HTTP/1.1 200 OK\r\n
Server: mnhv1\r\n
Content-Length: {长度}\r\n
\r\n
{IP}:{端口号}
```

然后，服务器关闭连接。