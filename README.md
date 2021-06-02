# mnh_server
[![GitHub release](https://img.shields.io/github/v/tag/hzyitc/mnh_server?label=release)](https://github.com/hzyitc/mnh_server/releases)

[README](README.md) | [中文文档](README_zh.md)

## Introduction

`mnh_server` is a server implement `mnhv1` protocol.

`mnhv1` is a protocol which help to punch nat hole.

## Usage

### Clone and build

```
> git clone https://github.com/hzyitc/mnh_server
> cd mnh_server
> make
```

### Run mnh_server

```
> ./mnh_server <port>
```

## Protocol Details

`mnh_server` will handle `mnhv1` and `HTTP` protocol in same port.

All messages are text.

### mnhv1

Client:
```
mnhv1 {id}\n
```

Server:
```
{ip}:{port}\n
```

Then,

Every 10s, client send a heartbeat to server:
```
heartbeat\n
```

Every time when server recieves a packet, it needs to send a heartbeat back:
```
.
```

### HTTP

Client:
```
GET /{id} {Don't care the following data}
```

Server:
```
HTTP/1.1 200 OK\r\n
Server: mnhv1\r\n
Content-Length: {len}\r\n
\r\n
{ip}:{port}
```

Then, server closes the connection.