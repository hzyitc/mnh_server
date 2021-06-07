#include "node.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>

NODE *head = NULL;
NODE *end = NULL;

void node_append(NODE *node) {
	if(head == NULL)
		head = node;
	
	node->prev = end;
	node->next = NULL;

	if(end)
		end->next = node;
	end = node;

	node->lastUpdate = time(NULL);
}

void node_delete(NODE *node) {
	NODE *prev = node->prev;
	NODE *next = node->next;

	if(prev)
		prev->next = next;
	else
		head = next;

	if(next)
		next->prev = prev;
	else
		end = prev;
}

NODE *node_malloc(bool udp, int sock, struct sockaddr_in address) {
	NODE *node = malloc(sizeof(NODE));
	if(node == NULL)
		return NULL;

	node->udp = udp;
	node->sock = sock;
	node->address = address;

	node->id[0] = 0;
	node->state = STATE_HANDSHAKE;

	node_append(node);
	return node;
}

void node_free(NODE *node) {
	node_delete(node);
	free(node);
}

NODE *node_getFirst() {
	return head;
}

NODE *node_queryById(const char *id) {
	for(NODE *node = end; node; node = node->prev) {
		if(strcmp(node->id, id) == 0)
			return node;
	}

	return NULL;
}

NODE *node_queryByAddress(bool udp, struct sockaddr_in address) {
	for(NODE *node = head; node; node = node->next) {
		if((node->udp == udp) && (memcmp(&node->address, &address, sizeof(struct sockaddr_in)) == 0))
			return node;
	}

	return NULL;
}

void node_update(NODE *node) {
	node_delete(node);
	node_append(node);
}

time_t node_getPast(NODE *node) {
	return (time(NULL) - node->lastUpdate);
}