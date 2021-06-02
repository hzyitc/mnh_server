#include "client.h"
#include "epoll.h"
#include "log.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

#define TIMEOUT			30
#define HTTP_PREFIX		"GET /"
#define MNH_PREFIX		"mnhv1 "

int str_cmp_head(const char *str, const char *head) {
	int ca, cb;
	do {
		ca = tolower(*str++);
		cb = tolower(*head++);
	} while (ca == cb && cb != '\0');
	return cb ? (ca - cb) : 0;
}

void client_close(int epoll_fd, NODE *node) {
	int fd = node->sock;
	log_i("clean %d(%s)", fd, node->id);

	if(epoll_del(epoll_fd, fd) < 0)
		log_e("delete %d from epoll error %d: %s", fd, errno, strerror(errno));

	close(fd);
	node_free(node);
}

int client_http_sendResponse(int fd, int code, const char *body) {
	char *codeMsg;
	switch(code) {
		case 200: codeMsg = "OK"; break;
		case 404: codeMsg = "Not Found"; break;
		default: codeMsg = "Unknown"; break;
	}

	char buffer[2048];
	sprintf(buffer,
		"HTTP/1.1 %d %s\r\n"
		"Server: mnhv1\r\n"
		"Content-Length: %lu\r\n"
		"\r\n%s",
		code, codeMsg,
		strlen(body),
		body
	);
	return send(fd, buffer, strlen(buffer), 0);
}

void client_http_handleHandshake(int epoll_fd, NODE *node, void *buf, int len) {
	char *p = buf + strlen(HTTP_PREFIX);
	char *q = strstr(p, " ");
	if(q != NULL) {
		*q = 0;

		NODE *n = node_queryById(p);
		if(n != NULL) {
			char body[1024];
			sprintf(body, "%s:%d", inet_ntoa(n->address.sin_addr), ntohs(n->address.sin_port));
			client_http_sendResponse(node->sock, 200, body);
			log_i("%s:%d query \"%s\": %s", 
				inet_ntoa(node->address.sin_addr), ntohs(node->address.sin_port),
				p,
				body
				);
		} else {
			// not found
			client_http_sendResponse(node->sock, 404, "Not found");
			log_i("%s:%d query \"%s\": Not found", 
				inet_ntoa(node->address.sin_addr), ntohs(node->address.sin_port),
				p
				);
		}
	}

	client_close(epoll_fd, node);
	return;
}

void client_mnh_handleHandshake(int epoll_fd, NODE *node, void *buf, int len) {
	char *p = buf + strlen(MNH_PREFIX);
	char *q = strstr(p, "\n");
	if(q == NULL) {
		client_close(epoll_fd, node);
		return;
	}
	
	*q = 0;
	if(strlen(p) > (sizeof(node->id) - 1))
		p[sizeof(node->id) - 1] = 0;
	strcpy(node->id, p);

	log_i("%s:%d register \"%s\"",
		inet_ntoa(node->address.sin_addr), ntohs(node->address.sin_port),
		p
		);

	char buffer[1024];
	sprintf(buffer, "%s:%d", inet_ntoa(node->address.sin_addr), ntohs(node->address.sin_port));
	send(node->sock, buffer, strlen(buffer), 0);

	node->state = STATE_MNHV1_HEARTBEAT;
}

void client_handle(int epoll_fd, NODE *node, void *buf, int len) {
	node_update(node);

	if(node->state == STATE_HANDSHAKE) {
		((char *) buf)[len] = 0;
		if(str_cmp_head(buf, HTTP_PREFIX) == 0)
			client_http_handleHandshake(epoll_fd, node, buf, len);
		else if(str_cmp_head(buf, MNH_PREFIX) == 0)
			client_mnh_handleHandshake(epoll_fd, node, buf, len);
		else
			client_close(epoll_fd, node);
	} else {
		//log_d("%d(%s) recv %dB: %s", node->sock, node->id, len, buf);
		log_d("%d(%s) recv %dB: %s", node->sock, node->id, len, "");

		send(node->sock, ".", 1, 0);
	}
}

void client_handleTCP(int epoll_fd, NODE *node) {
	char buf[9001];
	int len = read(node->sock, buf, 9000);
	if(len < 0) {
		log_e("sock %d recv header fail: ret=%d", node->sock, len);
		return;
	}

	client_handle(epoll_fd, node, buf, len);
}

time_t client_getMinAfter() {
	NODE *first = node_getFirst();
	if(first == NULL)
		return -1;

	time_t t = TIMEOUT - node_getPast(first);
	return (t > 0) ? t : 0;
}

void client_cleanTimeouts(int epoll_fd) {
	NODE *node;
	while(((node = node_getFirst()) != NULL) 
		&& (node_getPast(node) > TIMEOUT)) {
		log_i("%d(%s) timeout: %lds", node->sock, node->id, node_getPast(node));
		client_close(epoll_fd, node);
	}
}