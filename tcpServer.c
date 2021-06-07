#include "tcpServer.h"
#include "epoll.h"
#include "log.h"
#include "node.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

int tcpServer_create(int port) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
		return -1;

	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(port);

	if(bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		close(fd);
		return -1;
	}

	if(listen(fd, 3) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

void tcpServer_handle(int epoll_fd, int server_fd) {
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	int new_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if(new_fd < 0) {
		log_e("accept error %d: %s", errno, strerror(errno));
		return;
	}
	log_i("accepted %d from %s:%d", new_fd, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	NODE *node = node_malloc(false, new_fd, address);
	if(node == NULL) {
		log_e("node_create error: %d(%s)", errno, strerror(errno));
		close(new_fd);
		return;
	}

	if(epoll_add(epoll_fd, new_fd, node) < 0) {
		log_e("add %d to epoll error %d: %s", new_fd, errno, strerror(errno));
		close(new_fd);
		node_free(node);
		return;
	}
}