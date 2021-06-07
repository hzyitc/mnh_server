#ifndef __tcpServer_h__
#define __tcpServer_h__

int tcpServer_create(int port);
void tcpServer_handle(int epoll_fd, int server_fd);

#endif