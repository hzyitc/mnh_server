#include "epoll.h"

#include <string.h>

int epoll_add(int epoll_fd, int fd, void *data) {
	struct epoll_event ev;
	memset(&ev, 0, sizeof(struct epoll_event));
	ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	ev.data.ptr = data;
	return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int epoll_del(int epoll_fd, int fd) {
	return epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}