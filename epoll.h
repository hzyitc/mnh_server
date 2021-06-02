#include <sys/epoll.h>

#ifndef __epoll_h__
#define __epoll_h__

int epoll_add(int epoll_fd, int fd, void *data);
int epoll_del(int epoll_fd, int fd);

#endif