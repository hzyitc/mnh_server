#include "udpServer.h"
#include "epoll.h"
#include "log.h"
#include "node.h"
#include "client.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

int udpServer_create(int port) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
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

	return fd;
}

void udpServer_handle(int epoll_fd, int server_fd) {
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buf[9001];

	int len = recvfrom(server_fd, buf, 9000, 0, (struct sockaddr*)&address, &addrlen);
	if(len < 0) {
		log_e("recvfrom error %d: %s", errno, strerror(errno));
		return;
	}
	log_d("recv %dB from %s:%d", len, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	NODE *node = node_queryByAddress(true, address);
	if(node == NULL) {
		// New connection
		log_d("new connection from %s:%d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

		node = node_malloc(true, server_fd, address);
		if(node == NULL) {
			log_e("node_create error: %d(%s)", errno, strerror(errno));
			return;
		}
	}

	client_handle(epoll_fd, node, buf, len);
}