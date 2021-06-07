#ifndef __udpServer_h__
#define __udpServer_h__

int udpServer_create(int port);
void udpServer_handle(int epoll_fd, int server_fd);

#endif