#include "log.h"
#include "epoll.h"
#include "node.h"
#include "client.h"
#include "tcpServer.h"
#include "udpServer.h"

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

	int tcpServer_fd = tcpServer_create(port);
	if(tcpServer_fd < 0) {
		log_e("tcpServer_create error %d: %s", errno, strerror(errno));
		return 1;
	}
	if(epoll_add(epoll_fd, tcpServer_fd, &tcpServer_fd) < 0) {
		log_e("add tcp server to epoll error %d: %s", errno, strerror(errno));
		return 1;
	}

	int udpServer_fd = udpServer_create(port);
	if(udpServer_fd < 0) {
		log_e("udpServer_create error %d: %s", errno, strerror(errno));
		return 1;
	}
	if(epoll_add(epoll_fd, udpServer_fd, &udpServer_fd) < 0) {
		log_e("add udp server to epoll error %d: %s", errno, strerror(errno));
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
			if(evs[i].data.ptr == &tcpServer_fd)
				tcpServer_handle(epoll_fd, tcpServer_fd);
			else if(evs[i].data.ptr == &udpServer_fd)
				udpServer_handle(epoll_fd, udpServer_fd);
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
