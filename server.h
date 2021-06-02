#ifndef __server_h__
#define __server_h__

int server_create(int port);
void server_handle(int epoll_fd, int server_fd);

#endif