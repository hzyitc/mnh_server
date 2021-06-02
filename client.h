#include "node.h"

#ifndef __client_h__
#define __client_h__

void client_close(int epoll_fd, NODE *node);
void client_handle(int epoll_fd, NODE *node, void *buf, int len);
void client_handleTCP(int epoll_fd, NODE *node);

time_t client_getMinAfter();
void client_cleanTimeouts(int epoll_fd);

#endif