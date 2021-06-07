#include <stdbool.h>
#include <arpa/inet.h>

#ifndef __node_h__
#define __node_h__

typedef enum {
	STATE_HANDSHAKE,

	STATE_MNHV1_HEARTBEAT,
} STATE;

typedef struct NODE {
	bool udp;
	int sock;
	struct sockaddr_in address;

	char id[1024];
	STATE state;

	time_t lastUpdate;
	struct NODE *prev;
	struct NODE *next;
} NODE;

NODE *node_malloc(bool udp, int sock, struct sockaddr_in address);
void node_free(NODE *node);

NODE *node_getFirst();
NODE *node_queryById(const char *id);
NODE *node_queryByAddress(bool udp, struct sockaddr_in address);

void node_update(NODE *node);
time_t node_getPast(NODE *node);

#endif