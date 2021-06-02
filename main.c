#include "log.h"
#include "epoll.h"
#include "node.h"
#include "client.h"
#include "server.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define	EPOLL_EVENTS	1

void usage() {
	printf("Usage:\n");
	printf("    ./mnh_server <port>\n");
}

int main(int argc, char const *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if(argc != 2) {
		usage();
		return 1;
	}
	int port = atoi(argv[1]);

	int epoll_fd = epoll_create(100);
	if(epoll_fd < 0) {
		log_e("epoll_create error %d: %s", errno, strerror(errno));
		return 1;
	}

	int server_fd = server_create(port);
	if(server_fd < 0) {
		log_e("server_create error %d: %s", errno, strerror(errno));
		return 1;
	}
	if(epoll_add(epoll_fd, server_fd, 0) < 0) {
		log_e("add server to epoll error %d: %s", errno, strerror(errno));
		return 1;
	}

	log_i("running");
	while(true) {
		struct epoll_event evs[EPOLL_EVENTS];
		int ready = epoll_wait(epoll_fd, evs, EPOLL_EVENTS, (1000 * client_getMinAfter()));

		if(ready < 0) {
			if(errno = EINTR) {
				log_e("epoll interrupted");
				continue;
			} else {
				log_e("epoll error %d: %s", errno, strerror(errno));
				break;
			}
		}

		for(int i = 0; i < ready; i++) {
			if(evs[i].data.ptr == NULL)
				server_handle(epoll_fd, server_fd);
			else {
				NODE *node = evs[i].data.ptr;

				if(evs[i].events & EPOLLERR) {
					log_e("ERROR %d(%s)", node->sock, node->id);
					client_close(epoll_fd, node);
				} else if(evs[i].events & EPOLLRDHUP) {
					log_i("close %d(%s)", node->sock, node->id);
					client_close(epoll_fd, node);
				} else if(evs[i].events & EPOLLIN)
					client_handleTCP(epoll_fd, node);
			}
		}

		client_cleanTimeouts(epoll_fd);
	}

	return 0;
} 
